//
//  ExitDialog.mm
//  DCT
//
//  Created by MBJ on 12-2-29.
//  Copyright 2012 MediaTek Inc. All rights reserved.
//

#import "ExitDialog.h"
#import "DCTAppDelegate.h"

#define WM_DISCONNECTDIAL	WM_USER + 6
#define WM_CONFIRMED		WM_USER + 7


static UINT EndSessionCB()
{
	[[TheDelegate callCtrl] endSession];
	return 0;
}

@implementation ExitDialog

- (void)windowDidLoad
{
	[super windowDidLoad];
	[[self window] setTitle:LocalizeString(@"IDS_PROMPT_CAPTION_TOOL")];
	
	DCTAppDelegate* dele = (DCTAppDelegate*)TheDelegate;
	NSUserDefaults *defaults = [NSUserDefaults standardUserDefaults];
	NSNumber *number = (NSNumber*)[defaults valueForKey:DISCONNECT];
	if ([number intValue] == NSOnState && 
		(dele.dialupMgr->curConnStatus == kSCNetworkConnectionConnected ||
		 dele.dialupMgr->curConnStatus == kSCNetworkConnectionConnecting))
	{
		[promptMsg setStringValue:LocalizeString(@"IDS_PROMPT_DISCONN")];
		dele.dialupMgr->dialupReceiver = self;
		[dele.dialupMgr DoJob];
	}
	else
	{
		[promptMsg setStringValue:LocalizeString(@"IDS_PROMPT_TOOLEXIT")];
		CAERR err = [dele getComAgent]->Close(self, WM_CONFIRMED, 0, EndSessionCB);
		if (err == CA_OK)
			return;
		PostMessage(self, WM_CONFIRMED, 0, 0);
	}
}

- (void)OnMessage:(long)Msg wParam:(long)wparam lParam:(long)lparam
{
	if (Msg == WM_DIALUP_STATUS) {
		if (wparam == kSCNetworkConnectionDisconnected)
		{
			DCTAppDelegate* dele = (DCTAppDelegate*)TheDelegate;
			[promptMsg setStringValue:LocalizeString(@"IDS_PROMPT_TOOLEXIT")];
			CAERR err = [dele getComAgent]->Close(self, WM_CONFIRMED, 0);
			if (err == CA_OK)
				return;
			PostMessage(self, WM_CONFIRMED, 0, 0);
		}
	}
	else if (Msg == WM_CONFIRMED)
	{
	//	[NSApp stopModal];
		[NSApp abortModal];
		//	test
		NSLog(@"modal stopped.\n");
	}
}

@end
