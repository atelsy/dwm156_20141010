//
//  GeneralSettingsController.mm
//  DCT
//
//  Created by MBJ on 11-12-27.
//  Copyright 2011 MediaTek Inc. All rights reserved.
//

#import "GeneralSettingsController.h"
#import "DCTAppDelegate.h"

@implementation GeneralSettingsController

- (void)awakeFromNib
{
	NSString* str = LocalizeString(@"IDS_TABTITLE_GENERAL");
	NSTabView* tbv = (NSTabView*)[self view];
	[[tbv tabViewItemAtIndex:0]setLabel:str];
	str = LocalizeString(@"IDS_LABEL_SIMLOCK");
	[simlockLabel setStringValue:str];
	str = LocalizeString(@"IDS_LABEL_CHANGEPIN");
	[changepinLabel setStringValue:str];
	str = LocalizeString(@"IDS_LABEL_CHANGEPIN2");
	[changepin2Label setStringValue:str];
	
	[self SetCtrlWithURC];
	
	NSUserDefaults *defaults = [NSUserDefaults standardUserDefaults];
	
	NSNumber* number = (NSNumber*)[defaults valueForKey:DISCONNECT];
	[m_bDisconnectWhenExit setState:[number integerValue]];
	
	number = (NSNumber*)[defaults valueForKey:MINIMIZE];
	[m_bMinimizeToDock setState:[number integerValue]];
	
	number = (NSNumber*)[defaults valueForKey:AUTODIAL];
	[m_bAutoDialup setState:[number integerValue]];
}

- (void)SetCtrlWithURC
{
	DCTAppDelegate* dele = (DCTAppDelegate*)TheDelegate;
	NSString* str;
	if ([dele settingsCtrl]->m_bSimLock)
		str = LocalizeString(@"IDS_TITLE_UNLOCK");
	else
		str = LocalizeString(@"IDS_TITLE_LOCK");

	[changepinBtn setEnabled:[dele settingsCtrl]->m_bSimLock && [dele settingsCtrl]->m_bQuerySimlock];
	[simlockBtn setTitle:str];
	[changepin2Btn setEnabled:[dele settingsCtrl]->m_bQuerySimlock];
	[simlockBtn setEnabled:[dele settingsCtrl]->m_bQuerySimlock];
}

- (void)EnableCtrl:(BOOL)bEnable
{
	DCTAppDelegate* dele = (DCTAppDelegate*)TheDelegate;
	[changepinBtn setEnabled:(bEnable&&[dele settingsCtrl]->m_bSimLock)];
	[changepin2Btn setEnabled:bEnable];
	[simlockBtn setEnabled:bEnable];
}

-(IBAction)OnLanguageSelChange:(id)sender
{
}
-(IBAction)OnBtnSimLock:(id)sender
{
	DCTAppDelegate* dele = (DCTAppDelegate*)TheDelegate;

	[dele settingsCtrl].secDialog = [[verifyPwdDialog alloc]initWithWindowNibName:@"verifyPwdDialog"];
	
	if ([dele settingsCtrl]->m_bSimLock)
		[[dele settingsCtrl].secDialog setJob:SEC_UNLOCKSIM];
	else
		[[dele settingsCtrl].secDialog setJob:SEC_LOCKSIM];

	[NSApp runModalForWindow:[[dele settingsCtrl].secDialog window]];
	[[dele settingsCtrl].secDialog close];
	//[[dele settingsCtrl].secDialog release];
	[dele settingsCtrl].secDialog = nil;
	
	[self SetCtrlWithURC];
}
-(IBAction)OnBtnChangePin:(id)sender
{
	DCTAppDelegate* dele = (DCTAppDelegate*)TheDelegate;
	
	[dele settingsCtrl].secDialog = [[verifyPwdDialog alloc]initWithWindowNibName:@"verifyPwdDialog"];
	[[dele settingsCtrl].secDialog setJob:SEC_CHANGEPIN];
	
	[NSApp runModalForWindow:[[dele settingsCtrl].secDialog window]];
	[[dele settingsCtrl].secDialog close];
	//[[dele settingsCtrl].secDialog release];
	[dele settingsCtrl].secDialog = nil;
	
	[self SetCtrlWithURC];
}
-(IBAction)OnBtnChangePin2:(id)sender
{
	DCTAppDelegate* dele = (DCTAppDelegate*)TheDelegate;
	
	[dele settingsCtrl].secDialog = [[verifyPwdDialog alloc]initWithWindowNibName:@"verifyPwdDialog"];
	[[dele settingsCtrl].secDialog setJob:SEC_CHANGEPIN2];
	
	[NSApp runModalForWindow:[[dele settingsCtrl].secDialog window]];
	[[dele settingsCtrl].secDialog close];
//	[[dele settingsCtrl].secDialog release];
	[dele settingsCtrl].secDialog = nil;
	
	[self SetCtrlWithURC];
}

- (IBAction)clickDisconnect:(id)sender
{
	NSUserDefaults *defaults = [NSUserDefaults standardUserDefaults];
	[defaults setValue:[NSNumber numberWithInteger:[m_bDisconnectWhenExit state]] forKey:DISCONNECT];
	
}

- (IBAction)clickMini:(id)sender
{
	NSUserDefaults *defaults = [NSUserDefaults standardUserDefaults];
	[defaults setValue:[NSNumber numberWithInteger:[m_bMinimizeToDock state]] forKey:MINIMIZE];
}

- (IBAction)clickAutoDial:(id)sender;
{
	NSUserDefaults *defaults = [NSUserDefaults standardUserDefaults];
	[defaults setValue:[NSNumber numberWithInteger:[m_bAutoDialup state]] forKey:AUTODIAL];
}

@end
