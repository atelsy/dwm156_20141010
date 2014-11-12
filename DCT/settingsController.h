//
//  settingsWindow.h
//  DCT
//
//  Created by MBJ on 11-12-21.
//  Copyright 2011 MediaTek Inc. All rights reserved.
//

#import <Cocoa/Cocoa.h>
#import "DialupSettingsController.h"
#import "NetworkSettingsController.h"
#import "GeneralSettingsController.h"
#import "smsSettingControl.h"
#import "verifyPwdDialog.h"
#import "Win2Mac.h"
#import "smsTextView.h"

typedef struct {
	NSString*			itemname;
	NSViewController*	itemcontroller;
} SETTINGITEM;

@interface settingsController : NSWindowController<NSWindowDelegate> 
{
	IBOutlet NSBox*			viewBox;
	IBOutlet NSTableView*	itemTable;
	NSMutableArray*			itemSource;
	NSViewController*		curController;
	IBOutlet NSButton*		btnOK;
	IBOutlet NSButton*		btnCancel;
	IBOutlet GeneralSettingsController*	generalSettingsController;
	IBOutlet DialupSettingsController*	dialupSettingsController;
	IBOutlet NetworkSettingsController*	networkSettingsController;
	IBOutlet smsSettingControl *smsSettingController;
	
	smsTextView *customFieldEditor;
		IBOutlet verifyPwdDialog*	secDialog;

@public
	//	for network settings:
	BOOL		m_bQueryNet;
	int			m_nAct;
	CString		m_strCellID;
	BOOL		m_bNetworkAuto;
	NSString*	m_strOperName;
	int			m_nRatMode;
	int			m_nPreferRat;
	
	//	for general settings:
	BOOL		m_bQuerySimlock;
	BOOL		m_bSimLock;
}

@property (retain) IBOutlet verifyPwdDialog* secDialog;
@property (retain) IBOutlet NetworkSettingsController*	networkSettingsController;
@property (retain) IBOutlet DialupSettingsController*	dialupSettingsController; 

- (IBAction)OnBtnOKClick:(id)sender;
- (IBAction)OnBtnCancelClick:(id)sender;

- (void)OnMessage:(long)Msg wParam:(long)wparam lParam:(long)lparam;
- (void)onUrcCallback:(long)wparam lParam:(long)lparam;
- (void)onRespCallback:(long)wparam lParam:(long)lparam;

- (BOOL)showPwdDialog:(int)pinType;


@end
