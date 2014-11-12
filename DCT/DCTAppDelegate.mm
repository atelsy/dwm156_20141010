//
//  DCTAppDelegate.m
//  DCT
//
//  Created by Fengping Yu on 11-10-10.
//  Copyright 2011 MediaTek Inc. All rights reserved.
//

#import "DCTAppDelegate.h"
#import "SMSViewCtrl.h"
#import "pbViewControl.h"
#import "smsSettingControl.h"
#import "GeneralSettingsController.h"
#import "cellData.h"
#include <fcntl.h>
#include <sys/disk.h>
#include <sys/ioctl.h>
#include "Win2Mac.h"
#import "utility.h"
#import "devMgr.h"
#import "ExitDialog.h"
#import "nsstrex.h"
#import <CoreServices/CoreServices.h>
#import <QuartzCore/QuartzCore.h>
#import "ToolbarCtrl.h"
#include "basestr.h"
#include <ctype.h>
#include <stdlib.h>
#include <stdio.h>

#include <mach/mach_port.h>
#include <mach/mach_interface.h>
#include <mach/mach_init.h>

#include <IOKit/pwr_mgt/IOPMLib.h>
#include <IOKit/IOMessage.h>

#import "ComAgent.h"
#import "HSButton.h"
#pragma mark --
#pragma mark constant define
#pragma mark--

#define FILE_PREFIX_LEN		12
#define FILE_MAIN_VERSION   2
#define FILE_SUB_VERSION	0
#define STATUS_BAR_HEIGHT   30

#define SIGNAL_INDEX15
#define CONNECT_INDEX		16

#define FILE_PLMN_LIST	"plmnlist.txt"
#define FILE_PREFIX "DCT PROFILE"
#define FILE_NAME   "DCT.bin"

#pragma mark--
#pragma mark WndMsgArg class, NSString category and device change notification implementation
#pragma mark--

enum {
    
	ID_DIAL_DISCONN,
	ID_DIAL_GPRS,
	ID_DIAL_3G,
	ID_DIAL_EDGE,
	ID_DIAL_HSUPA,
	ID_DIAL_HPLUS

	
};

@implementation WndMsgArg

@end

void devChangeFunc(char *path, devType dev)
{
	DCTAppDelegate *delegate = TheDelegate;
	NSString* nspath = nil;
	if (path)
		nspath = [NSString stringWithUTF8String:path];
	
	switch (dev)
	{
		case RS232_DEVICE_TYPE:
			//	[delegate activeComPort:path];
			[delegate performSelectorOnMainThread:@selector(activeComPort:)
									   withObject:nspath
									waitUntilDone:FALSE];
			break;
		case MODEM_DEVICE_TYPE:
			//	[delegate activeModemPort:path];
			[delegate performSelectorOnMainThread:@selector(activeModemPort:)
									   withObject:nspath
									waitUntilDone:FALSE];
			break;
			//	20120523 by foil
		case VOICE_DEVICE_TYPE:
			[delegate performSelectorOnMainThread:@selector(activeVoicePort:)
									   withObject:nspath
									waitUntilDone:FALSE];
			break;
		default:
			break;
	}
}

#pragma mark--
#pragma mark begin DCTAppDelegate
#pragma mark--

@implementation DCTAppDelegate
@synthesize missCallButton;

@synthesize window, smsController, pbController, staticsController, dialupController, settingsCtrl, currentView, dialupMgr, voiceMgr, chController, callCtrl,ussdViewController,softwareVersion,btnContact,btnDialup,btnHelp,btnHistory,btnSetting,btnSMS,btnUssd;
@synthesize newMessage, m_indexMap;

#pragma mark--
#pragma mark init, construct and destruct function
#pragma mark--

- (void)activeComPort:(NSString*)nspath
{
	const char* path = NULL;
	if (nspath)
		path = [nspath UTF8String]; 
	
	if (path && strcmp(path, m_ComportPath) != 0)
	{
		strcpy(m_ComportPath, path);
		comAgent.Initialize(CString(m_ComportPath));
	}
	else if (path == NULL && strlen(m_ComportPath))
	{
		m_ComportPath[0] = 0;
		comAgent.StopDevice();
	}
	
	[[NSNotificationCenter defaultCenter] postNotificationName:WM_TOOLBARSTATE object:self userInfo:nil];
}

- (void) activeModemPort:(NSString*)nspath
{
	const char* path = NULL;
	if (nspath)
		path = [nspath UTF8String]; 
	
	if (path && strcmp(path, m_ModemPath) != 0)
		strcpy(m_ModemPath, path);
	else if (path == NULL && strlen(m_ModemPath))
		m_ModemPath[0] = 0;
}

//	20120523 by foil
- (void) activeVoicePort:(NSString*)nspath
{
	const char* path = NULL;
	
	if (nspath)
		path = [nspath UTF8String]; 
	
	if (path && strcmp(path, m_VoicePath) != 0)
	{
		NSLog(@"activeVoicePort. 1\n");
		
		strcpy(m_VoicePath, path);
		[voiceMgr AttachPort:m_VoicePath];
	}
	else if (path == NULL && strlen(m_VoicePath))
	{
		NSLog(@"activeVoicePort. 2\n");
		
		m_VoicePath[0] = 0;
		[voiceMgr ReleasePort];
	}
	else
	{
		//NSLog(@"activeVoicePort. 3\n");
	}
}

- (void)awakeFromNib
{
	//create system statusbar
//	statusItem = [[[NSStatusBar systemStatusBar] statusItemWithLength:NSVariableStatusItemLength] retain];
	//[statusItem setImage:[NSImage imageNamed:@"statusbar.png"]];
	networkStatus = [[NSArray alloc] initWithObjects:@"sigG.png", @"sigG.png", @"sigW.png",@"sigG.png",@"sigW.png",@"sigW.png",@"sigW.png",@"sigEmpty.png",nil];
	networkStatusTip = [[NSArray alloc] initWithObjects:@"2G", @"2G", @"3G",@"2G",@"3G",@"3G",@"3G",@"Disconn",nil];
	  aboutdialog=[[AboutDialog alloc]initWithWindowNibName:@"AboutDialog"];
	// create window style status bar
	[self setSignalAndConnStatus:0 andConn:NO];
	
	[self modifyMsgStatusBarData:@"Ready"];   
	
	EMSTATUS status = EMSInitialize();
	if(status != EMS_OK)
	{
		[utility showMessageBox:LocalizeString(@"MSGBOX_CAPTION") withMsg:LocalizeString(@"IDS_ERROR_EMSLIBRARY") forType:MB_OK];
	}
    
	[self readFile];
	
	// read message index key from file
	[self readSMSIndexFile];
	
	//newMessage = NO;
	m_nSignal = 0;
    m_nAct= -1;
	m_DialID = 7;//ID_DIAL_DISCONN;
	m_ComportPath[0] = 0;
	m_ModemPath[0] = 0;
	
	//	20120523 by foil
#if 1
	m_VoicePath[0] = 0;
	voiceMgr = [[VoiceMgr alloc]init];
	[voiceMgr Prepare];
#endif 
	//
	
	mainThread = [NSThread currentThread];
	mainRunLoop = [NSRunLoop currentRunLoop];
	dialupMgr = [[DialupMgr alloc]init];
	
	[self.window setBackgroundColor:[NSColor lightGrayColor]];
	
	comAgent.Initialize(_T(""));
	comAgent.RegisterURC(URC_ECSQ, self, WM_URCMSG);
	comAgent.RegisterURC(URC_SYS, self, WM_URCMSG);

/************ 20131111 by Zhuwei **********************************************/
    queryNetworkTypeTimer = nil;
    m_isChangeNetworkMode = NO;
    flag = 1;
/******************************************************************************/
    btnCtr = NO;
	if(callCtrl == nil)
	{
		callCtrl = [[callControl alloc] initWithNibName:@"callControl" bundle:nil];
	}
	
	if(chController == nil)
	{
		chController = [[callHistoryViewController alloc] initWithNibName:@"callHistoryViewController" bundle:nil];
	}
	
	if(smsController == nil)
	{
		smsController = [[SMSViewCtrl alloc] initWithNibName:@"SMSViewCtrl" bundle:nil];
	}
	
	if(pbController == nil)
	{
		pbController = [[pbViewControl alloc] initWithNibName:@"pbViewControl" bundle:nil];
	}
	
	if(dialupController == nil)
	{
		dialupController = [[DialupViewCtrl alloc] initWithNibName:@"Dialup" bundle:nil];
	}
	
	if(staticsController == nil)
	{
		staticsController = [[staticsViewController alloc] initWithNibName:@"staticsViewController" bundle:nil];
	}
	if(ussdViewController == nil)
	{
		ussdViewController = [ [UssdViewController alloc] initWithNibName:@"UssdViewController" bundle:nil];
	}
	settingsCtrl = [[settingsController alloc] initWithWindowNibName:@"settings"];
    
	
	[self showDefaultView];
	
	//[missCallButton setEnabled:NO];
	//[missCallButton setHidden:YES];
	[self initBtnCtr:btnCtr];
	mStatusBtnRect = [statusBtn frame];
	mMisscallBtnRect = [missCallButton frame];
	
	// add for test 10-18
	
	SInt32 osxVersionMajor, osxVersionMinor;
	
	if(Gestalt(gestaltSystemVersionMajor, &osxVersionMajor) == noErr && 
	   Gestalt(gestaltSystemVersionMinor, &osxVersionMinor) == noErr)
	{
		if(osxVersionMajor == 10 && osxVersionMinor < 6)
		{
			NSLog(@"register device mounted notification.\n");
			
			[[[NSWorkspace sharedWorkspace] notificationCenter] addObserver:self 
																   selector:@selector(deviceMounted:)
																	   name:NSWorkspaceDidMountNotification 
																	 object:nil];
			
			[[[NSWorkspace sharedWorkspace] notificationCenter] addObserver:self 
																   selector:@selector(deviceUnmounted:) 
																	   name:NSWorkspaceDidUnmountNotification
																	 object:nil];
		}
	}
}

- (void)dealloc
{
	[window release];
	[callCtrl release];
	[chController release];
	[smsController release];
	[dialupController release];
	[staticsController release];
	[settingsCtrl release];
    [ussdViewController release];
	[deviceThread release];
	
	[statusItem release];
	[networkStatus release];
	[networkStatusTip release];
	
	[super dealloc];
}

- (void)deviceMounted:(NSNotification*)notif
{
	NSLog(@"deviceMounted notification info: %@", [notif userInfo]);
	
	NSArray *devices = [[NSWorkspace sharedWorkspace] mountedRemovableMedia];
	
	NSLog(@"devices are : %@.\n", devices);
	
	//[devMgr getDeviceAttributes]
												
	[devMgr registerCDROMChangeNotificationWithCritical:&search andCallback:devChangeFunc];
	
}

- (void)deviceUnmounted:(NSNotification*)notif
{
	NSLog(@"deviceUnmounted notification info: %@", [notif userInfo]);
}

#pragma mark - sleep and wake


io_connect_t  root_port=0; // a reference to the Root Power Domain IOService
void * currMessageArg=NULL;
void
MySleepCallBack( void * refCon, io_service_t service, natural_t messageType, void * messageArgument )
{	
	CComAgent * pcomAgent = NULL;
	
	searchCritical* psearch;

	//HANDLE pEvent=NULL;
	CAERR err;
			
	NSLog( @"messageType %08lx, arg %08lx\n",
		  (long unsigned int)messageType,
		  (long unsigned int)messageArgument );
	
    switch ( messageType )
    {
			
        case kIOMessageCanSystemSleep:
            /* Idle sleep is about to kick in. This message will not be sent for forced sleep.
			 Applications have a chance to prevent sleep by calling IOCancelPowerChange.
			 Most applications should not prevent idle sleep.
			 
			 Power Management waits up to 30 seconds for you to either allow or deny idle
			 sleep. If you don't acknowledge this power change by calling either
			 IOAllowPowerChange or IOCancelPowerChange, the system will wait 30
			 seconds then go to sleep.
			 */
			
			NSLog( @"idle sleep message\n");
			
            //Uncomment to cancel idle sleep
            IOCancelPowerChange( root_port, (long)messageArgument );
            // we will allow idle sleep
            //IOAllowPowerChange( root_port, (long)messageArgument );
            break;
			
        case kIOMessageSystemWillSleep:
            /* The system WILL go to sleep. If you do not call IOAllowPowerChange or
			 IOCancelPowerChange to acknowledge this message, sleep will be
			 delayed by 30 seconds.
			 
			 NOTE: If you call IOCancelPowerChange to deny sleep it returns
			 kIOReturnSuccess, however the system WILL still go to sleep.
			 */
			NSLog( @"force sleep message\n");
             	 
			pcomAgent = [(DCTAppDelegate *)refCon getComAgent];
			err = pcomAgent->PowerOff(refCon, WM_POWEROFFDONE, 60, 0);
			if(err != CA_OK)
			{
				[(DCTAppDelegate *)refCon CloseAllPort];

			    IOAllowPowerChange( root_port, (long)messageArgument );
				currMessageArg = NULL;
			}
			else
			{
				//wait until poweroff operation done
				//and deal with in OnMessage
				currMessageArg = messageArgument;
			}
			
            break;
			
        case kIOMessageSystemWillPowerOn:
            //System has started the wake up process...
			NSLog( @"begin power on\n");

            break;
			
        case kIOMessageSystemHasPoweredOn:
            //System has finished waking up...
			NSLog( @"power on done\n");

			psearch =[(DCTAppDelegate *)refCon getSearch];
			[devMgr registerCDROMChangeNotificationWithCritical:psearch andCallback:devChangeFunc];
			
			break;
			
        default:
            break;
			
    }
}

- (void)CloseAllPort
{
	comAgent.StopDevice();
	m_ModemPath[0]=0;
	
	m_ComportPath[0] = 0;
	
	//pvoiceMgr = [(DCTAppDelegate *)refCon getVoiceMgr];
	[voiceMgr ReleasePort];
	m_VoicePath[0]=0;
}

#pragma mark--
#pragma mark application delegate method
#pragma mark--

- (void)applicationDidFinishLaunching:(NSNotification *)aNotification
{
	// Insert code here to initialize your application  
	//launch thread to detect device plug-in and out
	
    /************ 20131111 by Zhuwei **********************************************/
    self.softwareVersion = @"";
    /******************************************************************************/
    
	deviceThread = [[NSThread alloc] initWithTarget:self selector:@selector(initConnection) object:nil];
	
	
	[deviceThread start];
	
	//////////////
	//I/O kit sleep/wake notification
	// notification port allocated by IORegisterForSystemPower
    IONotificationPortRef  notifyPortRef;
	
    // notifier object, used to deregister later
    io_object_t            notifierObject;
	// this parameter is passed to the callback	
    // register to receive system sleep notifications
	  
    root_port = IORegisterForSystemPower( (void *)self, &notifyPortRef, MySleepCallBack, &notifierObject );
    if ( root_port == 0 )
    {
        printf("IORegisterForSystemPower failed\n");
        return;
    }
	
    
    // add the notification port to the application runloop
    CFRunLoopAddSource( CFRunLoopGetCurrent(),
					   IONotificationPortGetRunLoopSource(notifyPortRef), kCFRunLoopCommonModes );
/************ 20131122 by Zhuwei **********************************************/
	
    [[[NSWorkspace sharedWorkspace] notificationCenter] addObserver:self selector:@selector(willSleepNotification:) name: @"NSWorkspaceWillSleepNotification" object: nil];
	[[[NSWorkspace sharedWorkspace] notificationCenter] addObserver:self selector:@selector(didWakeNotification:) name: @"NSWorkspaceDidWakeNotification" object: nil];
/******************************************************************************/
    
	////////////

}

- (NSApplicationTerminateReply)applicationShouldTerminate:(NSApplication *)sender
{
	NSUserDefaults *defaults = [NSUserDefaults standardUserDefaults];
	NSNumber *number = (NSNumber*)[defaults valueForKey:DISCONNECT];
	if([number intValue] != NSOnState && dialupMgr->curConnStatus == kSCNetworkConnectionConnected)
	{
		[utility showMessageBox:LocalizeString(@"IDS_DISCONN_BEFORE_EXIT")
						withMsg:LocalizeString(@"IDS_PROMPT_CAPTION_TOOL")
						forType:MB_OK];
		return NSTerminateCancel;
	}
	
//	[TheDelegate.window orderOut:nil];
	ExitDialog* pDlg = [[ExitDialog alloc]initWithWindowNibName:@"ExitDialog"];
	[NSApp runModalForWindow:[pDlg window]];
	
	[pDlg close];
	[pDlg release];
	
	return NSTerminateNow;
}

- (BOOL)applicationShouldTerminateAfterLastWindowClosed:(NSApplication *)sender
{
	NSUserDefaults *defaults = [NSUserDefaults standardUserDefaults];
	NSNumber *number = (NSNumber*)[defaults valueForKey:MINIMIZE];
	NSLog(@"application should terminate after last window closed.\n");
	if([number intValue] == NSOnState)
	{
		return NO;
	}
	else
	{
		return YES;
	}
}

- (void)applicationDidHide:(NSNotification *)notification
{
	NSLog(@"application did hide.\n");
	
	if([utility getModalCounter] > 0)
	{
		NSLog(@"Hide all windows.\n");
		NSWindow* win = [NSApp modalWindow];
		[NSApp endSheet:win];
		[win orderOut:self];
	}
}

- (void)applicationDidUnhide:(NSNotification *)notification
{
	NSLog(@"unhide.\n");
	[self setAppMenuStatus:YES];
}

- (BOOL)applicationShouldHandleReopen:(NSApplication *)sender hasVisibleWindows:(BOOL)flag
{
	[window makeKeyAndOrderFront:self];
	
	if(currentView == SMSVIEW)
		[self showPhonebook:nil andMessage:smsController];
	else if(currentView == PHONEBOOKVIEW)
		[self showPhonebook:pbController andMessage:nil];
	
	return YES;
}

- (void)applicationWillTerminate:(NSNotification *)notification
{
	//NSString *string = [NSString stringWithFormat:@"%d", (int)currentView];
    NSString *string = [NSString stringWithFormat:@"%d", (int)DIALUPVIEW];
	NSUserDefaults *defaults = [NSUserDefaults standardUserDefaults];
	
	[defaults setObject:string forKey:@"ViewTag"];
	[defaults synchronize];
	
	[self stopConnection];
	
	[self writeFile];
	
	[self writeSMSIndexFile];
}

#pragma mark - window delegate method

- (void)windowWillMiniaturize:(NSNotification *)notification
{
	NSWindow* win = nil;
	
	if(currentView == SMSVIEW)
		win = [smsController getWindow];
	else if(currentView == PHONEBOOKVIEW)
		win = [pbController getWindow];
	
	if(win != nil)
		[win orderOut:nil];
}

- (void)windowDidDeminiaturize:(NSNotification *)notification
{
	NSWindow* win = nil;
	
	if(currentView == SMSVIEW)
		win = [smsController getWindow];
	else if(currentView == PHONEBOOKVIEW)
		win = [pbController getWindow];
	
	if(win != nil)
		[win orderFront:nil];
}

- (BOOL)windowShouldClose:(id)sender
{
	NSUserDefaults *defaults = [NSUserDefaults standardUserDefaults];
	NSNumber *number = (NSNumber*)[defaults valueForKey:DISCONNECT];
	NSNumber *miniNumber = (NSNumber*)[defaults valueForKey:MINIMIZE];
	
	if([miniNumber intValue] != NSOnState)
	{
		if(([number intValue] != NSOnState && dialupMgr->curConnStatus == kSCNetworkConnectionConnected))
		{
			[utility showMessageBox:LocalizeString(@"IDS_PROMPT_CAPTION_TOOL")
							withMsg:LocalizeString(@"IDS_DISCONN_BEFORE_EXIT")
							forType:MB_OK];
			return NO;
		}
	}
	
	return YES;
}

- (void)windowWillClose:(NSNotification*)notification
{
	NSLog(@"Enter window will close.\n");
	
	NSWindow* win = nil;
	
	if(currentView == SMSVIEW)
		win = [smsController getWindow];
	else if(currentView == PHONEBOOKVIEW)
		win = [pbController getWindow];
	
	if(win != nil)
	{
		[win orderOut:nil];
	}
	
	NSUserDefaults *defaults = [NSUserDefaults standardUserDefaults];
	
	NSNumber *miniNumber = (NSNumber*)[defaults valueForKey:MINIMIZE];
	
	if([miniNumber intValue] == NSOffState)
	{
		NSLog(@"window will close.\n");
		
		[self setAppMenuStatus:NO];
	}
	
	NSLog(@"Exit dialog has exit.\n");
	
	return;
}

- (void)initConnection
{
	id pool = [[NSAutoreleasePool alloc] init];
	//searchCritical search;
	
	//read vendor and interface number from plist
	
	NSDictionary *dict = [[NSBundle mainBundle] infoDictionary];
	int productID = [[dict objectForKey:@"kIOProductIDKey"] intValue];
	int vendorID = [[dict objectForKey:@"kIOVenderIDKey"] intValue];
	int modemInterfaceNumber = [[dict objectForKey:@"kModemInterfaceNumberKey"] intValue];
	int comportInterfaceNumber = [[dict objectForKey:@"kCOMPortInterfaceNumberKey"] intValue];
	
	search.productID = productID;
	search.vendorID = vendorID;
	search.modemInterfaceNumber = modemInterfaceNumber;
	search.rs232InterfaceNumber = comportInterfaceNumber;
	//	20120523 by foil
	search.voiceInterfaceNumber = [[dict objectForKey:@"kVoicePortInterfaceNumberKey"] intValue];
	//
	search.cdromInterfaceNumber = -1;
	
	[devMgr registerCDROMChangeNotificationWithCritical:&search andCallback:devChangeFunc];
	CFRunLoopRun(); 
	[pool release];
}

- (void)stopConnection
{
	[devMgr unRegister];
	
	[dialupMgr quit];
}

#pragma mark--
#pragma mark messages handler
#pragma mark--

//	!!!note: these handlers are for post/send message
- (void)HandleWndMsg: (void*)wnd 
			 Message: (unsigned int)msg
			  Param1: (long)wparam
			  Param2: (long)lparam
	   WaitUntilDone: (BOOL)bWait;
{
	WndMsgArg *arg = [[WndMsgArg alloc]init];
	arg->wnd = wnd;
	arg->msg = msg;
	arg->param1 = wparam;
	arg->param2 = lparam;
	
	[self performSelector:@selector(wndMsgHandler:)
				 onThread:mainThread
			   withObject:arg
			waitUntilDone:bWait];
}

- (void)wndMsgHandler: (id)arg
{
	//	in UI thread
	WndMsgArg* p = (WndMsgArg*)arg;
	
	if (*(unsigned int*)((long*)(p->wnd)+1) == CPPCLASS_MAGIC)
	{	
		//	target is a CWnd class object
		if (p->msg == WM_KILLTIMER) 
		{	
			//	invalidate: The NSRunLoop object removes and releases the timer!!!
			NSTimer* timer = (NSTimer*)(p->param1);
			WndMsgArg* pp = (WndMsgArg*)[timer userInfo];
			ASSERT(pp->msg == WM_TIMER);
			[pp release];
			[timer invalidate];
		}
		else
		{
			CWnd::HandleMsg(
							(CWnd*)p->wnd, 
							(UINT)p->msg, 
							(WPARAM)p->param1, 
							(LPARAM)p->param2);
		}
	}
	else 
	{
		//	target is an objc class object
		id target = (id)(p->wnd);
		
		if ([target respondsToSelector: @selector(OnMessage:wParam:lParam:)])
		{
			[target OnMessage:p->msg wParam:p->param1 lParam:p->param2];
		}
		else
		{
			ASSERT(0);
		}

	}
	
	[p release];
}

- (NSTimer*)HandleWndTimer:(void*)hWnd
					Elapse:(unsigned int)nElapse
				   TimerID:(unsigned int)nIDEvent
				 TimerFunc:(void*)lpTimerFunc;
{
	WndMsgArg *arg = [[WndMsgArg alloc]init];
	arg->wnd = hWnd;
	arg->msg = WM_TIMER;
	arg->param1 = nIDEvent;
	arg->param2 = (long)lpTimerFunc;
	
	NSTimer * timer = [NSTimer timerWithTimeInterval:((NSTimeInterval)nElapse/1000)
											  target:self 
											selector:@selector(wndTimerHandler:) 
											userInfo:arg 
											 repeats:TRUE];

	[mainRunLoop addTimer:timer forMode:NSDefaultRunLoopMode];
	[mainRunLoop addTimer:timer forMode:NSModalPanelRunLoopMode];
	[mainRunLoop addTimer:timer forMode:NSEventTrackingRunLoopMode];
	
	return timer;
}

- (void)wndTimerHandler: (NSTimer*)timer
{
	//	in UI thread
	WndMsgArg* p = (WndMsgArg*)[timer userInfo];
	
	if (*(unsigned int*)((long*)(p->wnd)+1) == CPPCLASS_MAGIC)		//	target is a CWnd class object
	{
		CWnd::HandleMsg(
						(CWnd*)p->wnd, 
						(UINT)p->msg, 
						(WPARAM)p->param1,
						(LPARAM)p->param2);
	}
	else
	{
		//	target is an objc class object
		id target = (id)(p->wnd);
		if ([target respondsToSelector: @selector(OnMessage:wParam:lParam:)])
		{
			[target OnMessage:p->msg wParam:p->param1 lParam:p->param2];
		}
		else
		{
			ASSERT(0);
		}

	}
	
//	[p release];
}

#pragma mark--
#pragma mark new message notification
#pragma mark--

- (void)newMessageNotification
{
	[smsController showNewMessage];
}

#pragma mark--
#pragma mark status bar
#pragma mark--

- (NSRect)newFrameForNewContentView:(NSView*)view
{
	NSWindow *mainWindow = [self window];
	NSRect newFrameRect = [mainWindow frameRectForContentRect:[view frame]];
	NSRect oldFrameRect = [mainWindow frame];
	
	NSSize newSize = newFrameRect.size;
	NSSize oldSize = oldFrameRect.size;
	
	NSRect frame = [mainWindow frame];
	
	frame.size = newSize;
	frame.origin.y -= (newSize.height - oldSize.height); 
	
	return frame;
}

#pragma mark--
#pragma mark load view
#pragma mark--

- (void)addView:(NSView*)view below:(NSView*)destView
{
	[[[self window] contentView] addSubview:view
								 positioned:NSWindowBelow
								 relativeTo:destView];
	
	[view setFrameOrigin:NSMakePoint(0, STATUS_BAR_HEIGHT)];
	
	[view setHidden:YES];
}

- (void)showDefaultView
{
	NSUserDefaults *defaults = [NSUserDefaults standardUserDefaults];
	NSString *view = [defaults objectForKey:@"ViewTag"];
	
	if(view != nil)
	{
		currentView = (viewTag)[view intValue];
		
		switch((int)currentView)
		{
			case CALLVIEW:
				[[[self window] contentView] addSubview:callCtrl.view];
				
				[callCtrl.view setFrameOrigin:NSMakePoint(0, STATUS_BAR_HEIGHT)];
				
				lastViewController = callCtrl;
				
				[self addView:smsController.view below:callCtrl.view];
				
				[self addView:pbController.view below:callCtrl.view];
				
				[self addView:chController.view below:callCtrl.view];
				
				[self modifyMsgStatusBarData:@"Ready"];
				
				break;
				
			case CALLHISTORYVIEW:
				[[[self window] contentView] addSubview:chController.view];
				
				[chController.view setFrameOrigin:NSMakePoint(0, STATUS_BAR_HEIGHT)];
				
				lastViewController = chController;
				
				[self addView:smsController.view below:chController.view];
				
				[self addView:pbController.view below:chController.view];
				
				[self addView:callCtrl.view below:chController.view];
				
			//	[self modifyMsgStatusBarData:@"Ready"];
				
				break;
				
			case PHONEBOOKVIEW:
				[[[self window] contentView] addSubview:pbController.view];
				
				[pbController.view setFrameOrigin:NSMakePoint(0, STATUS_BAR_HEIGHT)];
				
				lastViewController = pbController;
				
				// purpose: for init sms (call awakefromxib) to auto download
				[self addView:chController.view below:pbController.view];
				[self addView:smsController.view below:pbController.view];  
				[self addView:callCtrl.view below:pbController.view];
				
				break;
				
			case SMSVIEW:
				[[[self window] contentView] addSubview:smsController.view];
				
				[smsController.view setFrameOrigin:NSMakePoint(0, STATUS_BAR_HEIGHT)];
				
				lastViewController = smsController;
				
				[self addView:pbController.view below:smsController.view];
				[self addView:chController.view below:smsController.view];
				[self addView:callCtrl.view below:smsController.view];
				
				break;
				
			case DIALUPVIEW:
				[[[self window] contentView] addSubview:dialupController.view];
				
				[dialupController.view setFrameOrigin:NSMakePoint(0, STATUS_BAR_HEIGHT)];
				
				lastViewController = dialupController;
				
				[self addView:smsController.view below:dialupController.view];
				
				[self addView:pbController.view below:dialupController.view];
				
				[self addView:chController.view below:dialupController.view];
				
				[self addView:callCtrl.view below:dialupController.view];
				
				[self modifyMsgStatusBarData:@"Ready"];
				
				break;
				
			case STATISTICSVIEW:
				[[[self window] contentView] addSubview:staticsController.view];
				
				[staticsController.view setFrameOrigin:NSMakePoint(0, STATUS_BAR_HEIGHT)];
				
				lastViewController = staticsController;
				
				[self addView:smsController.view below:staticsController.view];
				
				[self addView:pbController.view below:staticsController.view];
				
				[self addView:callCtrl.view below:staticsController.view];
				[self addView:dialupController.view below:staticsController.view];
				[self addView:chController.view below:staticsController.view];
				[self addView:ussdViewController.view below:staticsController.view];
				[self modifyMsgStatusBarData:@"Ready"];
				
				break;
            case USSD:
				[[[self window] contentView] addSubview:ussdViewController.view];
				
				[ussdViewController.view setFrameOrigin:NSMakePoint(0, STATUS_BAR_HEIGHT)];
				
				lastViewController = ussdViewController;
				
				[self addView:smsController.view below:ussdViewController.view];
				
				[self addView:pbController.view below:ussdViewController.view];
				
				[self addView:callCtrl.view below:ussdViewController.view];
				
				[self addView:chController.view below:ussdViewController.view];
                [self addView:dialupController.view below:ussdViewController.view];
				
				[self modifyMsgStatusBarData:@"Ready"];
				
				break;
		}
		
		[self setMenuState:(int)currentView];
		
	}
	else
	{
		[[[self window] contentView] addSubview:smsController.view];
		
		[smsController.view setFrameOrigin:NSMakePoint(0, STATUS_BAR_HEIGHT)];
		
		lastViewController = smsController;
		
		[self setMenuState:SMSVIEW];
		
		[self addView:pbController.view below:smsController.view];
		[self addView:chController.view below:smsController.view];
		[self addView:callCtrl.view below:smsController.view];
	}
	
	[mToolbarControl modifyToolbarItemStatus:currentView];
}

- (IBAction)loadCallView:(id)sender
{
	[callCtrl.view setHidden:NO];
	
	if([lastViewController isKindOfClass:[callControl class]] == YES)
		return;
	
	[self hidePhonebook:pbController andMessage:smsController];
	
	[[[self window] contentView] replaceSubview:lastViewController.view with:callCtrl.view];
	
	[callCtrl.view setFrameOrigin:NSMakePoint(0, STATUS_BAR_HEIGHT)];
	
	lastViewController = callCtrl;
	
	currentView = CALLVIEW;
	
	[self setMenuState: CALLVIEW];
	
	[self modifyMsgStatusBarData:@"Ready"];

	[mToolbarControl modifyToolbarItemStatus:CALLVIEW];
}

- (IBAction)loadCallHistoryView:(id)sender
{
	[chController.view setHidden:NO];
	
	if([lastViewController isKindOfClass:[chController class]] == YES)
		return;
	
	[self hidePhonebook:pbController andMessage:smsController];
	
	[[[self window] contentView] replaceSubview:lastViewController.view with:chController.view];
	
	[chController.view setFrameOrigin:NSMakePoint(0, STATUS_BAR_HEIGHT)];
	
	lastViewController = chController;
	
	currentView = CALLHISTORYVIEW;
	
	[self setMenuState: CALLHISTORYVIEW];
	
	[chController.chList updateStatusBar];
	
	[chController.searchField setStringValue:@""];
	
	[mToolbarControl modifyToolbarItemStatus:CALLHISTORYVIEW];
}

- (IBAction)loadPhonebookView:(id)sender
{
	[pbController.view setHidden:NO];
	if([lastViewController isKindOfClass:[pbController class]] == YES)
		return;
	
	[self hidePhonebook:nil andMessage:smsController];
	
	[self showPhonebook:pbController andMessage:nil];
	
	[[[self window] contentView] replaceSubview:lastViewController.view with:pbController.view];
	
	[pbController.view setFrameOrigin:NSMakePoint(0, STATUS_BAR_HEIGHT)];
	
	lastViewController = pbController;
	
	currentView = PHONEBOOKVIEW;
	
	[self setMenuState: PHONEBOOKVIEW];
	
	[pbController.pbList UpdateStatusBar];
	
	[mToolbarControl modifyToolbarItemStatus:PHONEBOOKVIEW];
    
}

- (IBAction)loadSmsView:(id)sender
{
	[smsController.view setHidden:NO];
	
	if([lastViewController isKindOfClass:[smsController class]] == YES)
	{
		return;
	}
	
	[self hidePhonebook:pbController andMessage:nil];
	
	[self showPhonebook:nil andMessage:smsController];
	
	[[[self window] contentView] replaceSubview:lastViewController.view with:smsController.view];
	
	[smsController.view setFrameOrigin:NSMakePoint(0, STATUS_BAR_HEIGHT)];
	
	[smsController.smsTree setSelectedItemWithIndex:[smsController getLastSelectItem]];
	
	lastViewController = smsController;
	
	currentView = SMSVIEW;
	
	[self setMenuState: SMSVIEW];
	
	[smsController.smsList updateStatusBar];
	
	[smsController.searchField setStringValue:@""];
	
	[mToolbarControl modifyToolbarItemStatus:SMSVIEW];
}

- (IBAction)loadDialupView:(id)sender
{
	if([lastViewController isKindOfClass:[dialupController class]] == YES)
		return;
	
	[self hidePhonebook:pbController andMessage:smsController];
	
	[[[self window] contentView] replaceSubview:lastViewController.view with:dialupController.view];
	
	[dialupController.view setFrameOrigin:NSMakePoint(0, STATUS_BAR_HEIGHT)];
	
	lastViewController = dialupController;
	
	currentView = DIALUPVIEW;
	
	[self setMenuState: DIALUPVIEW];
	
	[self modifyMsgStatusBarData:@"Ready"];
	
	[mToolbarControl modifyToolbarItemStatus:DIALUPVIEW];
}

- (IBAction)loadStatisticsView:(id)sender
{
	[[staticsController view] setHidden:NO];
	
	if([lastViewController isKindOfClass:[staticsController class]] == YES)
		return;
	
	[self hidePhonebook:pbController andMessage:smsController];
	
	[[[self window] contentView] replaceSubview:lastViewController.view with:staticsController.view];
	
	[staticsController.view setFrameOrigin:NSMakePoint(0, STATUS_BAR_HEIGHT)];
	
	lastViewController = staticsController;
	
	currentView = STATISTICSVIEW;
	
	[self setMenuState:STATISTICSVIEW];
	
	[self modifyMsgStatusBarData:@"Ready"];
	
	[mToolbarControl modifyToolbarItemStatus:STATISTICSVIEW];
}

- (IBAction)showSettingsWindow:(id)sender
{
	[NSApp runModalForWindow:[settingsCtrl window]];
	[settingsCtrl close];
}
- (IBAction)showHelp:(id)sender
{
    //dwm156
    [[NSWorkspace sharedWorkspace] openFile:[[NSBundle mainBundle] pathForResource:@"UserManual_En_Mac" ofType:@"pdf"]] ;
     
    
}
- (IBAction)showUssd:(id)sender
{

   [[ussdViewController view] setHidden:NO];
	
	if([lastViewController isKindOfClass:[ussdViewController class]] == YES)
		return;
	
    [self hidePhonebook:pbController andMessage:smsController];
	
   [[[self window] contentView] replaceSubview:lastViewController.view with:ussdViewController.view];
	
    
    
	[ussdViewController.view setFrameOrigin:NSMakePoint(0, STATUS_BAR_HEIGHT)];
	
	lastViewController = ussdViewController;
	
	 currentView = USSD;
	
	[self setMenuState:USSD];
	
	[self modifyMsgStatusBarData:@"Ready"];
	
	[mToolbarControl modifyToolbarItemStatus:USSD];
   
  

}
-(IBAction)showAbout:(id)sender
{
    // const char* command = [self.textCommand.stringValue UTF8String];
  
   //  m_pComAgent->GetVERNO(void *hWnd, unsigned int uMsg);

    [NSApp runModalForWindow:[aboutdialog window]];
    [aboutdialog close];
    [ NSApp abortModal];



}
- (void)hidePhonebook:(pbViewControl *)pb andMessage:(SMSViewCtrl *)sms
{
	if(pb != nil && [pb getWindow])
		[[pb getWindow] orderOut:nil];
	
	if(sms != nil && [sms getWindow])
		[[sms getWindow] orderOut:nil];
}

- (void)showPhonebook:(pbViewControl *)pb andMessage:(SMSViewCtrl *)sms
{
	if(pb && [pb getWindow])
		[[pb getWindow] makeKeyAndOrderFront:nil];
	
	if(sms && [sms getWindow])
	{
		[[sms getWindow] makeKeyAndOrderFront:self];
		
		[[sms getWindow] makeMainWindow];
		
		[[sms getWindow] acceptsFirstResponder];
	}
}

#pragma mark--
#pragma mark menu state
#pragma mark--
- (BOOL)validateMenuItem:(NSMenuItem *)menuItem
{
	if ([NSApp modalWindow])
	{
		return FALSE;
	}
	
	return YES;
}

- (void)setAppMenuStatus:(signed char)bEnable
{
	NSMenu* menu = [NSApp mainMenu];
	
	if([NSApp modalWindow])
		[[menu itemAtIndex:0] setEnabled:NO];
	else
		[[menu itemAtIndex:0] setEnabled:bEnable];
}

- (void)setMenuState:(int)tag
{
	switch(tag)
	{
		case CALLVIEW:
			[callMenuItem setState:NSOnState];
			break;
			
		case CALLHISTORYVIEW:
			[callHistoryMenuItem setState:NSOnState];
			break;
			
		case PHONEBOOKVIEW:
			[pbMenuItem setState:NSOnState];
			break;
			
		case SMSVIEW:
			[smsMenuItem setState:NSOnState];
			break;
			
		case DIALUPVIEW:
			[dialupMenuItem setState:NSOnState];
			break;
			
		case STATISTICSVIEW:
			[settingMenuItem setState:NSOnState];
			break;
			
		case OPTIONVIEW:
			[optionMenuItem setState:NSOnState];
			break;
        case USSD:
			[ussdMenuItem setState:NSOnState];
			break;
	}
	
	if([callMenuItem tag] != tag)
		[callMenuItem setState:NSOffState];
	
	if([callHistoryMenuItem tag] != tag)
		[callHistoryMenuItem setState:NSOffState];
	
	if([pbMenuItem tag] != tag)
		[pbMenuItem setState:NSOffState];
	
	if([smsMenuItem tag] != tag)
		[smsMenuItem setState:NSOffState];
	
	if([dialupMenuItem tag] != tag)
		[dialupMenuItem setState:NSOffState];
	
	if([settingMenuItem tag] != tag)
		[settingMenuItem setState:NSOffState];
	
	if([optionMenuItem tag] != tag)
		[optionMenuItem setState:NSOffState];
    if([ussdMenuItem tag] != tag)
        [ussdMenuItem setState:NSOffState];
}

// may be modify future for more information, such as to distinguage call and sms
- (void)updateStatusIcon
{
	if(statusItem != nil)
	{
		[statusItem setImage:[NSImage imageNamed:@"inbox.png"]];
	}
}

#pragma mark--
#pragma mark simple function for get or set
#pragma mark--

- (searchCritical *)getSearch
{
	return &search;
}

- (VoiceMgr *)getVoiceMgr
{
	return voiceMgr;
}

- (CComAgent*)getComAgent
{
	return &comAgent;
}

- (BOOL)getConnected
{
	return bConnected;
}

- (char*)getModemPath
{
	return m_ModemPath;
}

- (SmsFolder*)getSmsFolder
{
	return &smsfolder;
}

- (PbFolder*)getPbFolder
{
	return &m_PbFolder;
}

- (ChFolder*)getChFolder
{
	return &chFolder;
}

- (SMSViewCtrl*)getSmsViewController
{
	return smsController;
}

- (pbViewControl*)getPbViewController
{
	return pbController;
}

- (callHistoryViewController*)getCallhistoryViewController
{
	return chController;
}
-(settingsController*)getSettingsController
{

    return settingsCtrl;

}
#pragma mark--
#pragma mark read or write operation
#pragma mark--

- (void)readSMSIndexFile
{
	NSString *path = [[NSBundle mainBundle] resourcePath];
	
	path = [path stringByAppendingPathComponent:@"/smsIndexMap.plist"];
	
	BOOL exist = [[NSFileManager defaultManager] fileExistsAtPath:path];
	
	if(exist == YES)
		m_indexMap = [[NSMutableDictionary dictionaryWithContentsOfFile:path] retain];
	else
		m_indexMap = [[NSMutableDictionary alloc] init];
}

- (void)readFile
{
	char *appPath = strcat((char*)[self getAppPath], FILE_NAME);
	
	NSString *path = [NSString stringWithUTF8String:appPath];  
	
	if([[NSFileManager defaultManager] fileExistsAtPath:path] == NO)
		return;
	
	ifstream is;
	
	is.open(appPath);
	
	char temp[256];
	is.read(temp, FILE_PREFIX_LEN);
	temp[FILE_PREFIX_LEN] = '\0';
	
	if(strcmp(temp, FILE_PREFIX) != 0)
	{
		return;
	}
	
	int nMainVersion, nSubVersion;
	is.read((char*)&(nMainVersion), sizeof(nMainVersion));
	
	if(nMainVersion != FILE_MAIN_VERSION)
		return;
	
	is.read((char*)&nSubVersion, sizeof(nSubVersion));
	if(nSubVersion != FILE_SUB_VERSION)
		return;
	
	is >> smsfolder;
	
	is >> m_PbFolder;
}

- (void)writeSMSIndexFile
{
	NSString *path = [[NSBundle mainBundle] resourcePath];
	
	path = [path stringByAppendingPathComponent:@"/smsIndexMap.plist"];
	
	BOOL exist = [[NSFileManager defaultManager] fileExistsAtPath:path];
	
	if(exist == NO)
		[[NSFileManager defaultManager] createFileAtPath:path contents:nil attributes:nil];
	
	[m_indexMap writeToFile:path atomically:YES];
}

- (void)writeFile
{
	[self clearHandsetFolder];
	
	try 
	{
		ofstream os;
		
		char *appPath = strcat((char*)[self getAppPath], FILE_NAME);
		
		if(strlen(appPath))
		{
			os.open(appPath);
			
			os.write(FILE_PREFIX, FILE_PREFIX_LEN);
			
			int nVersion = FILE_MAIN_VERSION;
			os.write((const char*)&(nVersion), sizeof(nVersion));
			
			nVersion = FILE_SUB_VERSION;
			os.write((const char*)&(nVersion), sizeof(nVersion));
			
			os << smsfolder;
			
			os << m_PbFolder;
			
			os.close();
		}
		else
		{
			
		}
		
	} 
	catch (...) 
	{
		
	}
}

- (void)clearSMSHandsetFolder
{
	SmsFolder *pFolder, *pSubFolder;
	SubfolderPos pos = smsfolder.GetFirstSubfolderPos();
	
	if(pos != 0)
	{
		pFolder = smsfolder.GetNextSubfolder(pos);
		if(pFolder)
		{
			SubfolderPos subpos = pFolder->GetFirstSubfolderPos();
			while(subpos != 0)
			{
				pSubFolder = pFolder->GetNextSubfolder(subpos);
				if(pSubFolder)
					pSubFolder->DeleteAll(false);
			}
			pSubFolder->CloseNextSubfolder(subpos);
		}
	}
	
	smsfolder.CloseNextSubfolder(pos);
}

- (void)clearPbHandsetFolder
{
	PbFolder *pFolderPb;
	SubfolderPos pos = m_PbFolder.GetFirstSubfolderPos();
	if(pos != 0)
	{
		pFolderPb = m_PbFolder.GetNextSubfolder(pos);
		if(pFolderPb)
			pFolderPb->DeleteAll(INCLUDE_SUBFOLDER);
	}
	m_PbFolder.CloseNextSubfolder(pos);
}

- (void)clearHandsetFolder
{
	[self clearSMSHandsetFolder];
	[self clearPbHandsetFolder];
}

#pragma mark--
#pragma mark common function
#pragma mark--

- (const char*)getAppPath
{
    NSString* docPath = [NSSearchPathForDirectoriesInDomains(NSDocumentDirectory, NSUserDomainMask, NO) objectAtIndex:0];
    NSString* saveFolder = [[NSString stringWithFormat:@"%@/D-LINK",docPath] stringByExpandingTildeInPath];
    if(![[NSFileManager defaultManager] fileExistsAtPath:saveFolder]) {
        [[NSFileManager defaultManager] createDirectoryAtPath:saveFolder withIntermediateDirectories:YES attributes:nil error:nil];
    }
	return [[saveFolder stringByAppendingString:@"/"] UTF8String];
}

- (NSString*)convertNumber:(CString)strNumber
{
	ssize_t nIdx;
	CString str, strLeft, strTemp;
	CString strName;
    NSString *nsstr=nil;
	strName.Empty();
	str = strNumber;
	
	while (!str.IsEmpty())
	{
		nIdx = str.FindOneOf(_T(",;"));
		if(nIdx == -1)
		{
			strLeft = str;
			str.Empty();
		}
		else
		{
			strLeft = str.Left(nIdx);
			str = str.Right(str.GetLength() - nIdx - 1);
		}
		
		if(!strLeft.IsEmpty())
		{
			if([self pbFindFoler:&m_PbFolder withNumber:strLeft andName:strTemp] == NO)
				strTemp = strLeft;
			
			if(strName.IsEmpty())
            {
                strName = strTemp;
                nsstr=[NSString CStringToNSString:strName];
                
            }
			else
            {
                strName += _T("; ") + strTemp;
                nsstr=[NSString CStringToNSString:strName];
                
            }
		}
	}
	
	if(strName.GetLength() <= 0)
		return nil;
	else
		return nsstr;
}

- (BOOL)pbFindFoler:(PbFolder *)pFoler
		 withNumber:(CString &)strNumber
			andName:(CString &)strName
{
	if(!pFoler)
		return NO;
	
	strName.Empty();
	
	if([self pbFindItem:pFoler withNUmber:strNumber andName:strName])
		return YES;
	
	PbFolder *psubFolder;
	SubfolderPos pos = pFoler->GetFirstSubfolderPos();
	
	while (pos)
	{
		psubFolder = pFoler->GetNextSubfolder(pos);
		if([self pbFindFoler:psubFolder withNumber:strNumber andName:strName])
			break;
	}
	pFoler->CloseNextSubfolder(pos);
	
	return (strName.IsEmpty() == FALSE);
}

- (BOOL)pbFindItem:(PbFolder *)pFolder withNUmber:(CString &)strNumber andName:(CString &)strName
{
    
	if(!pFolder)
		return NO;
    
    
	PbFolder *pSubFolder;
	SubfolderPos pos = pFolder->GetFirstSubfolderPos();
	
	while (pos)
    {
		pSubFolder = pFolder->GetNextSubfolder(pos);
		
		if([self pbSearchName:strName withNumber:strNumber inFolder:pSubFolder])
        {
			pFolder->CloseNextSubfolder(pos);
			return YES;
        }
    }
	
	pFolder->CloseNextSubfolder(pos);
	
    
	return YES;

}

#pragma mark--
#pragma mark status bar
#pragma mark--

- (void)modifyMsgStatusBarData:(NSString *)info
{
	[msgStatusBar setStringValue:info];
}

- (void)setSignalAndConnStatus:(int)signal andConn:(BOOL)bConn
{
	NSButtonCell *netCell = [statusBar cellWithTag:ID_STATUS_NETWORK_TYPE];
/************ 20131111 by Zhuwei **********************************************/
    //注释掉只有联网状态才去设置网络类型
//	if (dialupMgr == nil || dialupMgr->curConnStatus != kSCNetworkConnectionConnected)
//		m_DialID = ID_DIAL_DISCONN;
/******************************************************************************/
    
	NSString *networkImage = [networkStatus objectAtIndex:m_DialID];
	
	NSLog(@"[Dialup type] image = %@\n", networkImage);
	[netCell setImage:[NSImage imageNamed:networkImage]];
	[statusBar setToolTip:[networkStatusTip objectAtIndex:m_DialID] forCell:netCell];
	
	NSString *signalImage = [@"Sig" stringByAppendingString:[NSString stringWithFormat:@"%d", signal]];
	NSButtonCell *signalCell = [statusBar cellWithTag:ID_STATUS_SIGNAL];
	[signalCell setImage:[NSImage imageNamed:signalImage]];
	[statusBar setToolTip:LocalizeString(@"Signal") forCell:signalCell];
	
	NSButtonCell *connCell = [statusBar cellWithTag:ID_STATUS_CONNECT];
	if(bConn == YES)
	{
		[connCell setImage:[NSImage imageNamed:@"icoConnected.png"]];

		[statusBar setToolTip:LocalizeString(@"IDS_BTNTITLE_CONNECT") forCell:connCell];
	}
	else
	{
		[connCell setImage:[NSImage imageNamed:@"icoDisconnected.png"]];
		[statusBar setToolTip:LocalizeString(@"IDS_BTNTITLE_DISCONNECT") forCell:connCell];
	}
}

#pragma mark--
#pragma mark URC notification
#pragma mark--

- (void)OnMessage:(long)Msg wParam:(long)wparam lParam:(long)lparam
{
/************ 20131111 by Zhuwei **********************************************/
    //对应用程序注册一个5秒的timer 实现每隔5秒查询一次当前网络类型
    if(!queryNetworkTypeTimer) {
        queryNetworkTypeTimer = [[NSTimer scheduledTimerWithTimeInterval:5.0 target:self selector:@selector(doQueryNetworkType:) userInfo:nil repeats:YES] retain];
    }
/******************************************************************************/
    
	if(Msg == WM_URCMSG)
	{
		[self onUrcMsg:wparam lParam:lparam];
	}
	else if(Msg == WM_POWEROFFDONE)
	{
		//....set event
		NSLog(@"OnMessage: power off message comes.\n");  
		
		[self CloseAllPort];
		 
		if ( root_port != 0 )
		{
			//usleep(5*1000000); //for test
			IOAllowPowerChange(root_port, (long)currMessageArg );
			currMessageArg=NULL;
		}
	} else if(Msg == WM_RSPCALLBACK) {
        WATCH_POINTER(lparam);
        switch (wparam)
        {
/************ 20131111 by Zhuwei **********************************************/
    case RESP_GET_NETWORKTP:
        {
            //设置当前网络类型状态
            CRespArray *p = (CRespArray*)lparam;
            [self OnActChange:p->Value()[0]];
            //启动自动切换至auto选择网络类型状态
            if(!m_isChangeNetworkMode) {
                if([self getComAgent]->SetNetworkType(self, WM_RSPCALLBACK, RESP_SET_NETWORKTP, 2) == CA_OK) {
                    m_isChangeNetworkMode = YES;
                }
            }
        }
        break;
/******************************************************************************/
        }
    }
}

/************ 20131111 by Zhuwei **********************************************/
//timer执行函数，发送查询网络类型
- (void)doQueryNetworkType:(NSTimer *)timer {
    [self getComAgent]->QueryPSStatus(self, WM_RSPCALLBACK, RESP_GET_NETWORKTP);
}
/******************************************************************************/
- (void)onUrcMsg:(long)wParam lParam:(long)lParam
{
	WATCH_POINTER(lParam);
	
	switch (wParam)
	{
		case URC_ECSQ:
       
		{
            CUrcInt *p = (CUrcInt*)lParam;
            [self setSignal:p->Value()];
            [self getComAgent]->QueryPSStatus(self, WM_RSPCALLBACK, RESP_GET_CURNETTP);
        
        }
                   
		break;
			
		case URC_SYS:
		{
        CUrcSys *p = (CUrcSys*)lParam;
        int nStat = p->Value().alpha;
        btnCtr = nStat >= SYS_ATREADY;
        [self setConnection:btnCtr];
        [self initBtnCtr:btnCtr];// 初始化控件控制
        }
        break;
		default:
			break;
	}
}

#pragma mark--
#pragma mark update signal and connection status
#pragma mark--

- (void)setSignal:(int)nSignal
{
	if(nSignal < 0)
		nSignal = 0;
	else if(nSignal > 5)
		nSignal = 5;
	
	m_nSignal = nSignal;
    NSLog(@"m_nSignal %i",m_nSignal);
    
	[self setSignalAndConnStatus:nSignal andConn:bConnected];
}

- (void)setConnection:(BOOL)bConn
{
	bConnected = bConn;
	
	[self setSignalAndConnStatus:m_nSignal andConn:bConn];
}

- (void) OnActChange: (int)act
{
	//psbearer
	switch (act)
    {
        /*
		case 0:
			m_DialID = ID_DIAL_DISCONN;
		case 1:		//	GSM: 2G
			m_DialID = ID_DIAL_GPRS;
			break;
		case 2:		//	GSM w/EGPRS: EDGE
			m_DialID = ID_DIAL_EDGE;//2G
			break;
		case 3:		//	UTRAN: 3G
			m_DialID = ID_DIAL_3G;
			break;
		case 4:		//	UTRAN w/HSDPA
		case 5:		//	UTRAN w/HSUPA
		case 6:     //UTRAN w/HSDPA & HSUPA
        case 7:
			m_DialID = ID_DIAL_HSUPA;//3G
			break;
		default:
			m_DialID = ID_DIAL_HPLUS;
			break;
        */
            
            
        
          
           
     /*   case 0:
            m_DialID=ID_DIAL_DISCONN;
            break;
        case 1:
            m_DialID=ID_DIAL_GPRS;//2G
            break;
        case 2:
            m_DialID=ID_DIAL_3G;//3G
        default:
            m_DialID=ID_DIAL_3G;
            break;*/
    
         
    }
    m_DialID = act;
	 NSLog(@"[Dialup type] id = %d,act = %d\n", m_DialID,act);
	[self setSignalAndConnStatus:m_nSignal andConn:bConnected];
}

- (void) OnDialStatChange
{
	[self setSignalAndConnStatus:m_nSignal andConn:bConnected];
}

#pragma mark - search phonebook

- (BOOL)searchName:(CString &)strName 
		withNumber:(const CString &)strFind
{
	return [self pbSearchName:strName 
				   withNumber:strFind 
					 inFolder:&m_PbFolder];
}

- (BOOL)pbSearchName:(CString &)strName
		  withNumber:(const CString &)strFind 
			inFolder:(PbFolder *)pFolder
{
	if(!pFolder)
		return NO;
	
	if([self pbSearchItem:strName 
			   withNumber:strFind
				 inFolder:pFolder])
	{
		return YES;
	}
	
	PbFolder *pSubFolder;
	SubfolderPos pos = pFolder->GetFirstSubfolderPos();
	
	while (pos)
	{
		pSubFolder = pFolder->GetNextSubfolder(pos);
		
		if([self pbSearchName:strName withNumber:strFind inFolder:pSubFolder])
		{
			pFolder->CloseNextSubfolder(pos);
			return YES;
		}
	}
	
	pFolder->CloseNextSubfolder(pos);
	
	return NO;
}

- (BOOL)pbSearchItem:(CString &)name
		  withNumber:(const CString &)strFind
			inFolder:(PbFolder *)pFolder
{
	if(!pFolder)
		return NO;
	
	CString strName, strNumber, strTemp;
	PhoneBookItem *pItem =  NULL;
	
	ItemPos pos = pFolder->GetFirstItemPos();
	
	while (pos)
	{
		if((pItem = pFolder->GetNextItem(pos)))
		{
			pItem->GetPhone(strName, strNumber);
			if(strFind.GetLength() <= 7)
			{
				if(strNumber.Compare(strFind) == 0)
				{
					name = strName;
					pFolder->CloseNextItem(pos);
					return YES;
				}
			}
			else 
			{
				if(strNumber.GetLength() > 7)
				{
					strNumber = strNumber.Right(7);
					strTemp = strFind.Right(7);
					
					if(strNumber.Compare(strTemp) == 0)
					{
						name = strName;
						pFolder->CloseNextItem(pos);
						return YES;
					}
				}
			}
		}
	}
	
	pFolder->CloseNextItem(pos);
	
	return NO;
}

#pragma mark- misscall status

/*- (void)setMissCallStatus:(int)number
{
	NSDictionary *dict = [NSDictionary dictionaryWithObjectsAndKeys:[NSColor redColor], NSForegroundColorAttributeName,
						  [NSFont fontWithName:@"Lucida Grande" size:8], NSFontAttributeName, nil];
	NSString *mcNum = [NSString stringWithFormat:@"%d", number];
	
	NSAttributedString *str = [[NSAttributedString alloc] initWithString:mcNum attributes:dict];
	[[missCallButton cell] setAttributedTitle:str];
	
	NSString *tooltip =[NSString stringWithCString:"Missed call: " encoding:NSUTF8StringEncoding];
    tooltip = [tooltip stringByAppendingString:mcNum];
	
	[missCallButton setToolTip:tooltip];
	
	[missCallButton setEnabled:YES];
	
	[missCallButton setHidden:NO];
	
	NSRect rect = mStatusBtnRect;
	
	rect.origin.x = rect.origin.x + rect.size.width - mMisscallBtnRect.size.width + 10;
	
	rect.size.width = mMisscallBtnRect.size.width;
	
	[missCallButton setFrame:rect];
	
	rect = mStatusBtnRect;
	
	rect.origin.x = mMisscallBtnRect.origin.x + 25;
	
	[statusBtn setFrame:rect];
	
	[self setMissCallAnimation];
}*/

/*-(IBAction)downloadMissCall:(id)sender
{
	[self loadCallHistoryView:nil];
	
	PostMessage(chController, WM_DOWNLOADPROMPT, 0, 0);
	
	[missCallButton setEnabled:NO];
	
	[missCallButton setHidden:YES];
	
	[statusBtn setFrame:mStatusBtnRect];
	
	[missCallButton setFrame:mMisscallBtnRect];
}*/
/*
- (void)onTimechange:(NSNotification *)notification
{
	CComAgent *pCom = [self getComAgent];
	
	pCom->UpdateTime(self, 0);
}
*/
- (void)setMissCallAnimation
{
	CABasicAnimation *animation = [CABasicAnimation animationWithKeyPath:@"transform"];
	[animation setTimingFunction:[CAMediaTimingFunction functionWithName:kCAMediaTimingFunctionEaseInEaseOut]];
	[animation setDuration:0.5];
	[animation setRepeatCount:HUGE_VALF];
	[animation setAutoreverses:YES];
	[animation setRemovedOnCompletion:YES];
	[animation setToValue:[NSValue valueWithCATransform3D:CATransform3DMakeScale(1.2, 1.2, 1.0)]];
	[[missCallButton layer] addAnimation:animation forKey:nil];
}

- (void)setFlipAnimation
{
	CABasicAnimation *animation=[CABasicAnimation animationWithKeyPath:@"opacity"];
    animation.fromValue=[NSNumber numberWithFloat:1.0];
    animation.toValue=[NSNumber numberWithFloat:0.0];
    animation.autoreverses=YES;
    animation.duration=0.5;
    animation.repeatCount=FLT_MAX;
    animation.removedOnCompletion=NO;
    animation.fillMode=kCAFillModeForwards;
    [[missCallButton layer] addAnimation:animation forKey:nil];
}

- (IBAction)onStatusBtn:(id)sender
{
	if (comAgent.GetSysState() == SYS_PINREQ)
	{
		[settingsCtrl showPwdDialog:2/*SEC_VERIFYPIN*/];
	}
	else if (comAgent.GetSysState() == SYS_PUKREQ)
	{
		[settingsCtrl showPwdDialog:5/*SEC_VERIFYPUK*/];
	}
}

- (void)setStatusText:(NSString*)stat
{
	[statusBtn setTitle:stat];
}

- (void)setStatusInd:(int)stat
{
	switch (stat)
	{
		case SYS_NODEV:		// no device
			[statusBtn setTitle:LoadString(@"IDS_STATUS_NO_DEVICE")];
             m_DialID = 7;
            [self loadDialupView:nil];
          //  [self clearHandsetFolder];
/************ 20131120 by Zhuwei **********************************************/
            if(flag == 1) {
                [NSApp terminate:nil];// not dev USB exit
            }
            flag = 1;
/******************************************************************************/
            
            
			break;
		case SYS_DISC:		// disconnected
		case SYS_CONN:		// connected
			[statusBtn setTitle:LoadString(@"IDS_STATUS_NOT_CONNECTED")];
			break;
		case SYS_CMUX:		// cmux ready
		case SYS_ATREADY:	// AT ready
			[statusBtn setTitle:LoadString(@"IDS_STATUS_CONNECTING")];
			break;
			
		case SYS_NOSIM:		// SIM not inserted
			[statusBtn setTitle:LoadString(@"IDS_STATUS_NO_SIM")];
            m_DialID = 7;
			break;
		case SYS_BADSIM:		// SIM blocked or destroyed
			[statusBtn setTitle:LoadString(@"IDS_STATUS_BAD_SIM")];
			break;
			
		case SYS_PINREQ:		// PIN waiting
		case SYS_PUKREQ:		// PUK waiting
			[statusBtn setTitle:LoadString(@"IDS_STATUS_PIN_REQ")];
			break;
			
		case SYS_NONETWORK:	// Registration denied
            //[statusBtn setTitle:LoadString(@"IDS_STATUS_NO_NET")];
            [statusBtn setTitle:LoadString(@"IDS_STATUS_NO_SIM")];
			break;
		case SYS_NETSEARCH:	// Searching for network
			[statusBtn setTitle:LoadString(@"IDS_STATUS_SEARCHING")];
			break;
		default:
			break;
	}
}

- (NSString*)findOperName:(const CString&)opid
{

	_Size_T length = opid.GetLength();
	
	NSString *plmn = [NSString CStringToNSString:opid];
	
	// plmn = 5 or 6 digits
	if ((5 == length) || (6 == length))
	{
		for (_Size_T i=0; i<length; ++i)
		{
			if (!isdigit(opid[i]))
			{
				return plmn;	// opid is the name itself
			}
		}
	}
	else
	{
		return plmn;	// opid is the name itself
	}
	
	NSString *path = [[NSBundle mainBundle] pathForResource:@"plmn"
													 ofType:@"plist"];
		
	NSDictionary *plmnMap = [NSDictionary dictionaryWithContentsOfFile:path];

	return [plmnMap objectForKey:plmn];

}

-(NSDictionary*)findOperInfor:(const CString&)opid
{
	NSString *plmn = [NSString CStringToNSString:opid];
		
	NSString *path = [[NSBundle mainBundle] pathForResource:@"aplmn"
													 ofType:@"plist"];
    
	NSDictionary *plmnMap = [NSDictionary dictionaryWithContentsOfFile:path];
    
    //dwm156
    NSString *mcc;
    NSString *mnc;
    NSString *id;
    for(NSString *key in plmnMap)
    {
        mcc = [[plmnMap objectForKey:key] objectForKey:@"MCC"];
        mnc = [[plmnMap objectForKey:key] objectForKey:@"MNC"];
        id = [mcc stringByAppendingString:mnc];
        //NSLog(@"key: %@ value: %@", key, plmnMap[key]);
        if([id caseInsensitiveCompare:plmn] == NSOrderedSame)
           return [plmnMap objectForKey:key];
    }
    //没有找到就默认第一个索引
    return   [plmnMap objectForKey:@"1"];
    
//return   [plmnMap objectForKey: plmn];

}
/************ 20131122 by zhangshaohua **********************************************/


- (void)willSleepNotification:(NSNotification *)notification {
    flag = 0;
}

- (void)didWakeNotification:(NSNotification *)notification {
    
}

/******************************************************************************/
/***************************20131224 by zhang shaohua**************************/
- (void)initBtnCtr:(BOOL) btnFlag
{
    if( btnFlag){
    
        [btnDialup setEnabled:YES];
        [btnSMS    setEnabled:YES];
        [btnHelp  setEnabled:YES];
        [btnHistory setEnabled:YES];
        [btnUssd   setEnabled:YES];
        [btnContact setEnabled:YES];
        [btnSetting  setEnabled:YES];
    }
    else
        {
        [btnDialup setEnabled:NO];
        [btnSMS    setEnabled:NO];
        [btnHelp  setEnabled:NO];
        [btnHistory setEnabled:NO];
        [btnUssd   setEnabled:NO];
        [btnContact setEnabled:NO];
        [btnSetting  setEnabled:NO];
        
 
        
        }

}
/*****************************************************************************/
@end
