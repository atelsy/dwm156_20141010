//
//  callControl.h
//  DCT
//
//  Created by mtk on 12-6-6.
//  Copyright (c) 2012 MediaTek Inc. All rights reserved.
//
#import <Cocoa/Cocoa.h>
#import "KeypadCtrl.h"
#import "CallListCtrl.h"
#import "CallEngine.h"
#import "CallClient.h"


class CCall;
class CString;

@class CallPopup;

@interface callControl 
	: NSViewController<KeyPadClient, CallClient>
{
	IBOutlet KeypadCtrl *keyPadCtrl;
	IBOutlet CallListCtrl *callListCtrl;

	IBOutlet NSButton * tipsBtn;
	
	NSTimer *			tickTimer;
	NSTimer *			dtmfTimer;
	NSTimer *			tipsTimer;
	
	SEL					tipsProc;
	
	CallEngine *		engine;
	
	CallPopup *			popupWin;
	NSSound *			ringTone;
	
	CString				strForDtmf;
	short				idxForDtmf;
	
	BOOL				voicePlaying;

}

@property (assign) IBOutlet KeypadCtrl *keyPadCtrl;
@property (assign) IBOutlet CallListCtrl *callListCtrl;


- (void)makeCall:(NSString*)strDial;

- (void)makeCall:(NSString*)strDial
		withName:(NSString*)strName;

- (void)onCallCtrl:(NSInteger)code
			 index:(int)idx;

- (void)endSession;

- (void)HangupAll;

///////////////////////

@end


@interface callControl(Private)

- (NSTimer*)setTimer:(NSTimeInterval)ti
			selector:(SEL)aSelector
			 repeats:(BOOL)yesOrNo;

- (void)onTickTimer:(NSTimer*)timer;
- (void)startTickTimer;
- (void)stopTickTimer;

- (void)buildRing:(NSString*)name;
- (void)startRing:(NSString*)name;
- (void)stopRing;

- (void)showPopup:(const CCall*)pCall;
- (void)hidePopup;

- (BOOL)showNotify:(NSString*)strMsg
		 withOwner:(unichar)owner;

- (BOOL)showConfirm:(NSString*)strMsg
		  withOwner:(unichar)owner
		andSelector:(SEL)selector;

- (void)dismissTips;
- (void)dismissTips:(unichar)owner;

- (void)onTipsTimer:(NSTimer*)timer;

- (IBAction)onTipsBtn:(id)sender;

- (void)writeCallLog:(const CCall&)call
		   withCause:(int)cause;

- (void)writeMultiCallLog:(const CCallList&)calls
				withCause:(int)cause;

- (void)errorMessage:(NSInteger)code
		   withError:(CAERR)err;

- (void)failureMessage:(NSInteger)code;

- (void)callSummary:(const CString&)alpha
		  withCause:(int)cause;

- (void)multiCallSummary:(const CCallList*)calls
			   withCause:(int)cause;

- (BOOL)searchName:(CString&)name
		  byNumber:(const CString&)number
		   andDTMF:(const CString&)dtmf;


@end
