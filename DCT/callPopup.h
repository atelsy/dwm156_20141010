//
//  callPopup.h
//  DCT
//
//  Created by MBJ on 12-6-13.
//  Copyright 2012 MediaTek Inc. All rights reserved.
//

#import <Cocoa/Cocoa.h>

class CCall;

@class CallEngine;
@class callControl;

@interface CallPopup : NSWindowController
{
	IBOutlet NSTextField *label;
	IBOutlet NSTextField *alpha;
	IBOutlet NSTextField *number;
	
	IBOutlet NSImageView *iconIncm;
//	IBOutlet NSImageView *iconWait;
	
	IBOutlet NSButton *btnAnswer;
	IBOutlet NSButton *btnReject;
	
	CCall *			theCall;
	CallEngine *	engine;
	callControl *	owner;
}

- (IBAction)onAnswer:(id)sender;
- (IBAction)onReject:(id)sender;

- (void)bindCall:(const CCall*)pCall
	   andEngine:(CallEngine*)engine;

- (void)showWindow:(callControl*)owner
			origin:(NSPoint)origin;

- (void)updateCtrl;

//- (void)onAwakeFromNib:(void *)context;
//- (NSInteger)livingPeriod;

@end
