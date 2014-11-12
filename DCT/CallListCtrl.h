//
//  CallListCtrl.h
//  CocoaTry
//
//  Created by MBJ on 12-5-30.
//  Copyright 2012 MediaTek Inc. All rights reserved.
//

#import <Cocoa/Cocoa.h>


enum _cc_btn_pos
{
	CC_BTN_SWITCH = 0,
	CC_BTN_SWAP,
	CC_BTN_ANSWER,
	CC_BTN_JOIN,
	CC_BTN_EXTRACT,
	CC_BTN_REPLACE,
	CC_BTN_REJECT,
	CC_BTN_TERMINATE,
	CC_BTN_CUT_ACTIVE,
	CC_BTN_CUT_HELD,
	
	CC_BTN_MAX
};


class CCall;
class CCallList;

@class CallEngine;

@interface CallListCtrl
	: NSObject<
		NSTableViewDelegate, 
		NSTableViewDataSource, 
		NSMenuDelegate
	>
{
	
	IBOutlet NSTableView *	tableView;
	
	IBOutlet NSButton *		btnSwitch;
	IBOutlet NSButton *		btnSwap;
	IBOutlet NSButton *		btnAnswer;
	IBOutlet NSButton *		btnReplace;
	IBOutlet NSButton *		btnJoin;
	IBOutlet NSButton *		btnExtract;
	IBOutlet NSButton *		btnReject;
	IBOutlet NSButton *		btnTerminate;
	IBOutlet NSButton *		btnCutActive;
	IBOutlet NSButton *		btnCutHeld;
	
	IBOutlet NSObject *		target;
	IBOutlet NSMenu *		menu;

	const CCallList *		dataSource;
	
	CallEngine *			engine;
	short					hotCallIdx;
}


- (void)updateCalls:(const CCallList*)callList
		 withEngine:(CallEngine*)engine;

- (void)updateCtrl;

- (void)updateTick;

- (IBAction)onButtonClick:(id)sender;
- (IBAction)onContextMenu:(id)sender;

@end

