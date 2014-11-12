//
//  GeneralSettingsController.h
//  DCT
//
//  Created by MBJ on 11-12-27.
//  Copyright 2011 MediaTek Inc. All rights reserved.
//

#import <Cocoa/Cocoa.h>

#define DISCONNECT @"DisconnectDialupconnectionWhenExitTool"
#define MINIMIZE   @"MinizeToDockWhenCloseTool"
#define AUTODIAL @"AutoDialAfterToolStartup"

@interface GeneralSettingsController : NSViewController {

	IBOutlet NSTextField*	simlockLabel;
	IBOutlet NSButton*		simlockBtn;
	IBOutlet NSTextField*	changepinLabel;
	IBOutlet NSButton*		changepinBtn;
	IBOutlet NSTextField*	changepin2Label;
	IBOutlet NSButton*		changepin2Btn;

	IBOutlet NSButton *m_bDisconnectWhenExit;
	IBOutlet NSButton *m_bMinimizeToDock;
	IBOutlet NSButton *m_bAutoDialup;
}

-(IBAction)OnLanguageSelChange:(id)sender;
-(IBAction)OnBtnSimLock:(id)sender;
-(IBAction)OnBtnChangePin:(id)sender;
-(IBAction)OnBtnChangePin2:(id)sender;

- (IBAction)clickDisconnect:(id)sender;
- (IBAction)clickMini:(id)sender;
- (IBAction)clickAutoDial:(id)sender;

- (void)SetCtrlWithURC;
- (void)EnableCtrl:(BOOL)bEnable;

@end
