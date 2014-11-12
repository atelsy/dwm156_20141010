//
//  smsReaderController.m
//  DCT
//
//  Created by Fengping Yu on 10/25/11.
//  Copyright 2011 MediaTek Inc. All rights reserved.
//

#import "smsReaderController.h"
#import "DCTAppDelegate.h"
#import "nsstrex.h"


@implementation smsReaderController

- (id)initWithWindow:(NSWindow *)window
{
	self = [super initWithWindow:window];
	if (self) 
	{
		// Initialization code here.
	}
	
	return self;
}

- (void)dealloc
{
	[super dealloc];
}

- (void)windowDidLoad
{
	[super windowDidLoad];
	
	// Implement this method to handle any initialization after your window controller's window has been loaded from its nib file.
	[m_ctlEditMessage initDefaultParameters];
	CString from;
	
	if(m_pMsg)
	{
		EMSData *pEmsData;
		m_pMsg->GetMsg(&pEmsData);
		[m_ctlEditMessage setEmsData:pEmsData withSelFolder:NO];
		
		m_pMsg->GetAddressNumber(from);
		[m_time setStringValue: [utility getTime:m_pMsg]];
		
		[m_number setStringValue:[NSString getStringFromWchar:(LPCTSTR)from]];
	}
}

- (void)initWithAddress:(NSString *)from
				   time:(NSString *)time
	  andMessageContent:(NSString *)message
				 forMsg:(SMSMessage *)pmsg
			   andIndex:(int)index
{
	m_strFrom = from;
	m_strTime = time;
	m_strMessage = message;
	m_pMsg = pmsg;
	m_nIndex = index;
}

- (SMSMessage*)initMessage
{
	SMSMessage *pMsg = [utility newMessage];
	if(pMsg && m_pMsg)
		*pMsg = *m_pMsg;
	
	return pMsg;
}

#pragma mark --
#pragma mark toolbar message
#pragma mark--

- (IBAction)onSmsNew:(id)sender
{ 
	[[self window] performClose:nil];
	
	[[TheDelegate getSmsViewController] smsNew:nil];
}

- (IBAction)onSmsReply:(id)sender
{
	[[self window] performClose:nil];
	
	[[TheDelegate getSmsViewController] smsReply:nil];
}

- (IBAction)onSmsForward:(id)sender
{
	[[self window] performClose:nil];
	
	[[TheDelegate getSmsViewController] smsForward:nil];
}

#pragma mark--
#pragma mark delegage method
#pragma mark--

- (BOOL)validateToolbarItem:(NSToolbarItem *)theItem
{
	if([theItem tag] == REPLY_TAG || [theItem tag] == FORWARD_TAG)
	{
		if([[TheDelegate getSmsViewController] getSimStatus] == YES)
			return YES;
		else
			return NO;
	}
	
	return YES;
}

- (void)windowWillClose:(NSNotification *)notification
{
	[[NSNotificationCenter defaultCenter] postNotificationName:WM_SMS_DESTROYDIALOG object:self userInfo:[NSDictionary dictionaryWithObjectsAndKeys:[NSNumber numberWithInt:SMS_DLG_READER],ITEM_MSGID, nil]];
}

@end
