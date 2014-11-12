//
//  DCTAppDelegate.h
//  DCT
//
//  Created by Fengping Yu on 11-10-10.
//  Copyright 2011 MediaTek Inc. All rights reserved.
//

#import <Cocoa/Cocoa.h>
#import "callControl.h"
#import "callHistoryViewController.h"
#import "SMSViewCtrl.h"
#import "pbViewControl.h"
#import "DialupViewCtrl.h"
#import "staticsViewController.h"
#import "settingsController.h"
#import "ComAgent.h"
#import "DialupMgr.h"
#import "devMgr.h"
#import "VoiceMgr.h"
#import "Folder.h"
#import "SMSMessage.h"
#import "PhoneBookItem.h"
#import "utility.h"
#import "UssdViewController.h"
#import "AboutDialog.h"


@class ToolbarCtrl;
@class HSButton;
typedef enum
{
	CALLVIEW = 0,
	CALLHISTORYVIEW,
	PHONEBOOKVIEW,
	SMSVIEW,
	DIALUPVIEW,
	STATISTICSVIEW,
	OPTIONVIEW,
    NEWHELP,
    USSD,
    
}viewTag;
/*typedef enum
{
	//CALLVIEW = 0,
	//CALLHISTORYVIEW,
    DIALUPVIEW=0,
    SMSVIEW,
	PHONEBOOKVIEW,
    STATISTICSVIEW,
    USSD,	
	OPTIONVIEW,
    NEWHELP,
    
    
}viewTag;*/

typedef enum
{
	ID_STATUS_NETWORK_TYPE = 0,
	ID_STATUS_SIGNAL,
	ID_STATUS_CONNECT,
}statusFlag;

#define IMSIKEY		@"imsiKey"

//	!!!note: the message argument pack
@interface WndMsgArg : NSObject
{
@public
	void*	wnd;
	long	msg;
	long	param1;
	long	param2;
}
@end

//static void devChangeFunc(char *path, int dev);
//static void activeComPort(char *path);

#define WM_TOOLBARSTATE @"OnToolbarEnable"

//@class smsPopUp;

@interface DCTAppDelegate 
	: NSObject <
		NSApplicationDelegate, 
		NSToolbarDelegate, 
		NSWindowDelegate
	>
{
@private
	NSWindow *window;
	//	!!!note: the UI thread & runloop
	NSThread *mainThread;
	NSRunLoop *mainRunLoop;
	////////////////////////////////////
	IBOutlet callControl *callCtrl;
	IBOutlet callHistoryViewController *chController;
	IBOutlet SMSViewCtrl *smsController;
	IBOutlet pbViewControl *pbController;
	IBOutlet DialupViewCtrl *dialupController;
	IBOutlet staticsViewController *staticsController;
    IBOutlet UssdViewController    *ussdViewController;
	NSViewController *lastViewController;   // record the last view controller;
	//*******************About//by shao hua 2013 11 10
    AboutDialog*    aboutdialog;
     CComAgent*              m_pComAgent;
    //*****************************************
	IBOutlet settingsController *settingsCtrl;
	
	// status bar
	IBOutlet NSMenu *statusMenu;
	
	// implement popview fqwor message tip
	NSStatusItem *statusItem;
	
	BOOL newMessage;
	BOOL btnCtr;
	IBOutlet NSMenuItem *callMenuItem;
	IBOutlet NSMenuItem *callHistoryMenuItem;
	IBOutlet NSMenuItem *pbMenuItem;
	IBOutlet NSMenuItem *smsMenuItem;
	IBOutlet NSMenuItem *dialupMenuItem;
	IBOutlet NSMenuItem *settingMenuItem;
	IBOutlet NSMenuItem *optionMenuItem;
    IBOutlet NSMenuItem *ussdMenuItem;
	IBOutlet ToolbarCtrl *mToolbarControl;
	
    
    
	viewTag currentView;
	//   NSInteger currentFrame;  // show status item animation frame
	
	// status item data
	IBOutlet NSMatrix *statusBar;
	IBOutlet NSTextField *msgStatusBar;
	IBOutlet NSButton *statusBtn;
	
	// folder define
	SmsFolder smsfolder;
	PbFolder m_PbFolder;
	ChFolder chFolder;
	NSMutableDictionary *m_indexMap;
	NSButton *missCallButton;
	
	CComAgent comAgent;
    CComAgent *m_pCom;
	DialupMgr* dialupMgr;
	//	20120523 by foil
	VoiceMgr* voiceMgr;
	
	//others
	BOOL bConnected;
	int m_nSignal;
	int m_DialID;
    int		m_nAct;
	char m_ModemPath[256]; 
	char m_ComportPath[256];
	//	20120523 by foil
	char m_VoicePath[256];
	
	searchCritical search;
	
	NSArray *networkStatus;
	NSArray *networkStatusTip;
	
	NSThread *deviceThread;

	NSRect		mStatusBtnRect;
	NSRect		mMisscallBtnRect;
/************ 20131111 by Zhuwei **********************************************/
    NSTimer*    queryNetworkTypeTimer;
    NSString*   softwareVersion;
    BOOL        m_isChangeNetworkMode;
    int         flag;
/******************************************************************************/
/*********************20131223 by Zhang shao hua ******************************/
    HSButton*           btnSMS;
    HSButton*           btnContact;
    HSButton*           btnHistory;
    HSButton*           btnSetting;
    HSButton*           btnHelp;
    HSButton*           btnUssd;
    HSButton*           btnDialup;
   
/******************************************************************************/
}



//	!!!note: post/send message handler	///////////
- (void)wndMsgHandler: (id)arg;
- (void)HandleWndMsg: (void*)wnd 
			 Message: (unsigned int)msg
			  Param1: (long)wparam
			  Param2: (long)lparam
	   WaitUntilDone: (BOOL)bWait;

- (void)wndTimerHandler: (NSTimer*)timer;
- (NSTimer*)HandleWndTimer:(void*)hWnd
					Elapse:(unsigned int)nElapse
				   TimerID:(unsigned int)nIDEvent
				 TimerFunc:(void*)lpTimerFunc;
///////////////////////////////////////////////////
- (void) activeComPort:(NSString*)nspath;
- (void) activeModemPort:(NSString*)nspath;
- (void)showDefaultView;
- (void)setMenuState:(int)tag;
- (void)updateStatusIcon;
- (void)newMessageNotification;

// get view control;
- (callHistoryViewController*)getCallhistoryViewController;
- (SMSViewCtrl*)getSmsViewController;
- (pbViewControl*)getPbViewController;
-(settingsController*)getSettingsController;
- (IBAction)loadCallView:(id)sender;
- (IBAction)loadCallHistoryView:(id)sender;
- (IBAction)loadPhonebookView:(id)sender;
- (IBAction)loadSmsView:(id)sender;
- (IBAction)loadDialupView:(id)sender;
- (IBAction)loadStatisticsView:(id)sender;
- (IBAction)showSettingsWindow:(id)sender;
- (IBAction)showHelp:(id)sender;
- (IBAction)showUssd:(id)sender;
- (IBAction)onStatusBtn:(id)sender;
-(IBAction)showAbout:(id)sender;

- (void)setStatusText:(NSString*)stat;
- (void)setStatusInd:(int)stat;

- (NSString*)findOperName:(const CString&)opid;
-(NSDictionary*)findOperInfor:(const CString&)opid;

// connection relate method
- (void)initConnection;
- (void)stopConnection;
- (BOOL)getConnected;
- (CComAgent*)getComAgent;
- (SmsFolder*)getSmsFolder;
- (PbFolder*)getPbFolder;
- (ChFolder*)getChFolder;
- (char*)getModemPath;
- (VoiceMgr *)getVoiceMgr;
- (searchCritical *)getSearch;
- (void)CloseAllPort;

// file operation
- (void)writeDialupIniFile;
- (void)writeFile;
- (void)writeSMSIndexFile;
- (void)readFile;
- (void)readSMSIndexFile;
- (void)clearSMSHandsetFolder;
- (void)clearPbHandsetFolder;
- (void)clearHandsetFolder;

// common function
- (const char*)getAppPath;
- (NSString*)convertNumber:(CString)strNumber;
- (BOOL)pbFindFoler:(PbFolder*)pFoler withNumber:(CString&)strNumber andName:(CString&)strName;
- (BOOL)pbFindItem:(PbFolder*)pFolder withNUmber:(CString&)strNumber andName:(CString&)strName;

// sms job thread relate
//- (void)onSmsJobThread:(NSDictionary*)dict;
- (void)OnMessage:(long)Msg wParam:(long)wparam lParam:(long)lparam;
- (void)onUrcMsg:(WPARAM)wParam lParam:(LPARAM)lParam;

- (void)setSignal:(int)nSignal;
- (void)setConnection:(BOOL)bConn;
- (void)setSignalAndConnStatus:(int)signal andConn:(BOOL)bConn;
- (void) OnActChange: (int)act;
- (void) OnDialStatChange;
- (void)initBtnCtr:(BOOL)btnFlag;
- (void)hidePhonebook:(pbViewControl*)pb andMessage:(SMSViewCtrl*)sms;
- (void)showPhonebook:(pbViewControl*)pb andMessage:(SMSViewCtrl*)sms;

- (void)modifyMsgStatusBarData:(NSString*)info;

- (NSRect)newFrameForNewContentView:(NSView*)view;

// helper method
- (void)addView:(NSView*)view below:(NSView*)destView;
- (void)setAppMenuStatus:(BOOL)bEnable;

- (BOOL)searchName:(CString &)strName withNumber:(const CString &)strFind;
- (BOOL)pbSearchName:(CString&)strName withNumber:(const CString&)strFind inFolder:(PbFolder*)pFolder;
- (BOOL)pbSearchItem:(CString&)name withNumber:(const CString&)strFind inFolder:(PbFolder*)pFolder;

// set misscall button state
- (void)setMissCallStatus:(int)number;
- (IBAction)downloadMissCall:(id)sender;
//- (void)onTimechange:(NSNotification*)notification;
- (void)setMissCallAnimation;
- (void)setFlipAnimation;
@property (assign) IBOutlet NSButton *btnContact; 
@property (assign) IBOutlet NSButton *btnDialup;
@property (assign) IBOutlet NSButton *btnSMS;
@property (assign) IBOutlet NSButton *btnHelp;
@property (assign) IBOutlet NSButton *btnSetting;
@property (assign) IBOutlet NSButton *btnUssd;  
@property (assign) IBOutlet NSButton *btnHistory;
@property (assign) IBOutlet NSWindow *window;
@property (nonatomic, retain) IBOutlet callControl *callCtrl;
@property (nonatomic, retain) IBOutlet callHistoryViewController *chController;
@property (nonatomic, retain) IBOutlet SMSViewCtrl *smsController;
@property (retain) IBOutlet DialupViewCtrl *dialupController;
@property (retain) IBOutlet pbViewControl *pbController;
@property (retain) IBOutlet staticsViewController *staticsController;
@property (retain) IBOutlet settingsController *settingsCtrl;
@property (retain) IBOutlet UssdViewController *ussdViewController;
@property (readwrite, assign)viewTag currentView; 
@property (retain) DialupMgr* dialupMgr;
@property (retain) VoiceMgr* voiceMgr;
@property(readwrite, nonatomic) BOOL newMessage;
@property (nonatomic, retain) NSMutableDictionary *m_indexMap;

@property (assign) IBOutlet NSButton *missCallButton;

/************ 20131111 by Zhuwei **********************************************/
@property (retain) NSString* softwareVersion;
/******************************************************************************/

@end


void
MySleepCallBack( void * refCon, io_service_t service, natural_t messageType, void * messageArgument );




