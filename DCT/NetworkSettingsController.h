//
//  NetworkSettingsController.h
//  DCT
//
//  Created by MBJ on 11-12-23.
//  Copyright 2011 MediaTek Inc. All rights reserved.
//

#import <Cocoa/Cocoa.h>
#import "promptDialog.h"

@interface NetworkSettingsController : NSViewController {
	IBOutlet NSTextField*	networkTypeLabel;
//	IBOutlet NSComboBox*	networkTypeBox;
	IBOutlet NSPopUpButton* networkTypeBox;
	IBOutlet NSTextField*	curOperLabel;
	IBOutlet NSTextField*	curOperator;
	IBOutlet NSTextField*	curCellIDLabel;
	IBOutlet NSTextField*	curCellID;

	IBOutlet NSMatrix*		srchregMatrix;
	IBOutlet NSTableView*	plmnList;
	IBOutlet NSButton*		refreshBtn;
	IBOutlet NSButton*		registerBtn;
	
	NSMutableArray*			plmnSource;
	promptDialog*			refreshDlg;
	NSInteger		m_nOldType;
	BOOL			m_bChangeType;
}

- (IBAction)OnNetworkTypeSelChange:(id)sender;
- (IBAction)OnRadioMatrixChange:(id)sender;
- (IBAction)OnBtnRefreshClick:(id)sender;
- (IBAction)OnBtnRegisterClick:(id)sender;
- (void)OnMessage:(long)Msg wParam:(long)wparam lParam:(long)lparam;
/****************************20131120 by zhang shao hua clear NSTableView Data ************/
//- (void)clearData;
- (void)SetCtrlWithURC;
- (void)EnableCtrl:(BOOL)bEnable;
- (NSString*)ParseAct:(long)act;

/************ 20131120 by Zhuwei **********************************************/

- (void)clearSource;

/******************************************************************************/

@end
