//
//  callControl.mm
//  DCT
//
//  Created by mtk on 12-6-6.
//  Copyright (c) 2012 MediaTek Inc. All rights reserved.
//

#import "Win2Mac.h"
#import "DCTAppDelegate.h"
#import "callPopup.h"
#import "MmiString.h"
#import "nsstrex.h"


#define TIP_TYPE_NOTIFY		0x000000
#define TIP_TYPE_CONFIRM	0x010000

#define IS_TIP_NOTIFY(tag)	\
	(((tag) & 0xff0000) == TIP_TYPE_NOTIFY)

#define IS_TIP_CONFIRM(tag)	\
	(((tag) & 0xff0000) == TIP_TYPE_CONFIRM)

#define OWNER_OF_TIP(tag)	\
	((tag)&0xffff)

#define MAKE_NOTIFY_OWNER(owr)	\
	((owr) | TIP_TYPE_NOTIFY)

#define MAKE_CONFIRM_OWNER(owr)	\
	((owr) | TIP_TYPE_CONFIRM)

#define TIP_OWNER_CSSN	'c'
#define TIP_OWNER_CALL	'l'

///////////////

@implementation callControl

@synthesize callListCtrl, keyPadCtrl;

- (id)initWithNibName:(NSString *)nibNameOrNil 
			   bundle:(NSBundle *)nibBundleOrNil
{
	self = [super initWithNibName:nibNameOrNil 
						   bundle:nibBundleOrNil];
	
	if (self)
	{
		// Initialization code here.
		CComAgent *comObj = 
			[(DCTAppDelegate*)TheDelegate getComAgent];
		
		engine = [[CallEngine alloc]initWithContext:self
									 comAgentObject:comObj];
		
		tickTimer = nil;
		dtmfTimer = nil;
		tipsTimer = nil;
		tipsProc  = nil;
		
		ringTone = nil;
		popupWin = nil;
		
		idxForDtmf = -1;
		
		voicePlaying = NO;
	}
	
	return self;
}

- (void)dealloc
{
	[tickTimer invalidate];
	[dtmfTimer invalidate];
	
	[ringTone release];
	[popupWin release];
	
	[super dealloc];
}

- (void)awakeFromNib
{
}

- (void)onKeyPressed:(char)keyCode 
		  withString:(NSString *)mmiStr
{
	switch (keyCode)
	{
		case KEY_CODE_SEND:
			[keyPadCtrl selectAll];
			[self makeCall:mmiStr];
			break;
			
		case KEY_CODE_END:
			[keyPadCtrl selectAll];
			[engine HangUp];
			break;
			
		case KEY_CODE_FRIEND:
			[TheDelegate loadPhonebookView:nil];
			break;
		
		case KEY_CODE_P:
		case KEY_CODE_W:
		case KEY_CODE_PLUS:
		case KEY_CODE_DEL:
			break;
			
		default:
			if ([engine DtmfTest:0])
			{
				[engine SendDTMF:[NSString NSStringToCString:mmiStr]];
			}
			break;
	}
}


- (void)onSystemState:(const DualWord&)state
{
	NSLog(@"onSystemState: %d", state.alpha);
	
	if (state.alpha >= SYS_NETREADY)
	{
		[keyPadCtrl enable:KEY_MASK_SEND|KEY_MASK_EDIT|KEY_MASK_FRIEND];
	}
	else if (state.alpha > SYS_ATREADY)
	{
		[keyPadCtrl enable:KEY_MASK_SEND|KEY_MASK_EDIT|KEY_MASK_FRIEND];
	}
	else if (state.alpha >= SYS_CONN)
	{
		[keyPadCtrl enable:0];
	}
	else
	{
		[keyPadCtrl enable:0];
	}
}

- (BOOL)onIncomingCall:(const CCall*)pCall
{
	CString strName;
	BOOL	toUpdate = NO;
	
	[self searchName:strName
			byNumber:pCall->number
			 andDTMF:_T("")];
	
	if (pCall->alpha != strName)
	{
		pCall->alpha = strName;
		toUpdate = YES;
	}
	
	[self showPopup:pCall];
	[self startRing:@"incoming"];

	return toUpdate;
}

- (void)onOutgoingCall:(const CCall*)pCall
{
}

- (BOOL)onWaitingCall:(const CCall*)pCall
{
	CString strName;
	BOOL	toUpdate = NO;
	
	[self searchName:strName
			byNumber:pCall->number
			 andDTMF:_T("")];
	
	if (pCall->alpha != strName)
	{
		pCall->alpha = strName;
		toUpdate = YES;
	}
	
	[self showPopup:pCall];
	[self startRing:@"waiting"];
	
	return toUpdate;
}

- (void)onIncomingEnd
{
	[self hidePopup];
	[self stopRing];
}

- (void)onOutgoingEnd
{
}

- (void)onWaitingEnd
{
	[self hidePopup];
	[self stopRing];
}

- (void)onCallUpdated:(const CCallList&)allCalls
{
	[callListCtrl updateCalls:&allCalls
				   withEngine:engine];

	if (tickTimer == nil)
	{
		if (allCalls.IsActive() || allCalls.IsEmCall())
		{
			[self startTickTimer];
		}
	}
	else
	{
		if (!allCalls.IsActive() && !allCalls.IsEmCall())
		{
			[self stopTickTimer];
		}
	}
	
	if (allCalls.Size() > 0)
	{
		if (!voicePlaying)
		{
			[TheDelegate.voiceMgr StartPlay];
			[TheDelegate.voiceMgr StartCapture];
			voicePlaying = YES;
		}
	}
	else if (voicePlaying)
	{
		[TheDelegate.voiceMgr StopCapture];
		[TheDelegate.voiceMgr StopPlay];
		voicePlaying = NO;
	}
	
	[self checkOnDtmf];
}

- (void)onSSINotify:(const CSsnCode&)ssiCode
{
	NSString *noteID = nil;
	
	switch(ssiCode.code)
	{
		case SSI_FORWARD_ALL_ON:
			noteID = @"IDS_SSI_FORWARD_ON";
			break;
			
		case SSI_FORWARD_PART_ON:
			noteID = @"IDS_SSI_FORWARD_PART_ON";
			break;
			
		case SSI_CALL_FORWARDED:
			noteID = @"IDS_SSI_CALL_FORWARDED";
			break;
			
		case SSI_CALL_WAITING:
			noteID = @"IDS_SSI_CALL_WAITING";
			break;
			
		case SSI_CUG_CALL:
			noteID = @"IDS_SSI_CUG_CALL";
			break;
			
		case SSI_OUTGOING_BARRED:
			noteID = @"IDS_SSI_OUTGOING_BARRED";
			break;
			
		case SSI_INCOMING_BARED:
			noteID = @"IDS_SSI_INCOMING_BARRED";
			break;
			
		case SSI_CLIR_REJECTED:
			noteID = @"IDS_SSI_CLIR_REJECTED";
			break;
			
		case SSI_CALL_DEFLECTED:
			noteID = @"IDS_SSI_CALL_DEFLECTED";
			break;
			
		default:
			break;
	}
	
	if (noteID != nil)
	{
		[self showNotify:LoadString(noteID)
			   withOwner:TIP_OWNER_CSSN];
	}
}

- (void)onSSUNotify:(const CSsnCode&)ssuCode
{
	NSString *noteID = nil;
	
	switch(ssuCode.code)
	{
		case SSU_FORWARDED_CALL:
			noteID = @"IDS_SSU_FORWARDED_CALL";
			break;
			
		case SSU_CUG_CALL:
			noteID = @"IDS_SSI_CUG_CALL";
			break;
			
		case SSU_CALL_HELD:
			noteID = @"IDS_SSU_CALL_HELD";
			break;
			
		case SSU_CALL_RETRIEVED:
			noteID = @"IDS_SSU_CALL_RETRIEVED";
			break;
			
		case SSU_MPTY_ENTERED:
			noteID = @"IDS_SSU_MPTY_ENTERED";
			break;
			
	// 	case SSU_HELDCALL_RELEASED:
	// 		noteID = @"IDS_SSU_HELD_RELEASED";
	// 		break;
			
		case SSU_FORWARD_CHECK:
			break;
			
		case SSU_TRANSFER_ALERTING:
			noteID = @"IDS_SSU_TRANSFER_ALERTING";
			break;
			
		case SSU_TRANSFER_CONNECTED:
			noteID = @"IDS_SSU_TRANSFER_CONNECTED";
			break;
			
		case SSU_DEFLECTED_CALL:
			noteID = @"IDS_SSU_DEFLECTED_CALL";
			break;
			
		case SSU_INCOMING_FORWARDED:
			noteID = @"IDS_SSU_INCOMING_FORWARDED";
			break;
			
		default:
			break;
	}
	
	if (noteID != nil)
	{
		[self showNotify:LoadString(noteID)
			   withOwner:TIP_OWNER_CSSN];
	}
}

- (void)onCallDialed:(const CCall*)pCall
		   withError:(int)err
{
	[self closeDtmf];

	[self dismissTips:TIP_OWNER_CALL];
	
	if (err == ER_OK)
	{
		idxForDtmf = pCall->idx;
		
		if ([self setupDtmf])
		{
			[self beginDtmf];
		}
	}
	else
	{
		[self writeCallLog:*pCall
				 withCause:0];
		
		[self failureMessage:-1];
	}
}

- (void)onCallLost:(const CCallList*)calls
		 withCause:(int)cause
{
	[self multiCallSummary:calls
				 withCause:cause];
	
	[self writeMultiCallLog:*calls
				  withCause:0];	// cause?
}

- (void)onCallSwitched:(int)err
{
	if (ER_OK != err)
	{
		[self failureMessage:CC_BTN_SWITCH];
	}
}

- (void)onCallJoined:(int)err
{
	if (ER_OK != err)
	{
		[self failureMessage:CC_BTN_JOIN];
	}
}

- (void)onCallSwapped:(int)err
{
	if (ER_OK != err)
	{
		[self failureMessage:CC_BTN_SWAP];
	}
}

- (void)onCallAnswered:(const CCall*)pCall
			 withError:(int)err
{
	if (ER_OK != err)
	{
		[self failureMessage:CC_BTN_ANSWER];
	}
}

- (void)onCallExtracted:(const CCall*)pCall
			  withError:(int)err
{
	if (ER_OK != err)
	{
		[self failureMessage:CC_BTN_EXTRACT];
	}
}

- (void)onCallReplaced:(const CCallList&)lostCalls
			  withCall:(const CCall*)pCall
			  andError:(int)err
{
	if (ER_OK == err)
	{
		[self writeMultiCallLog:lostCalls
					  withCause:CAUSE_REPLACED];
	}
	else
	{
		[self failureMessage:CC_BTN_REPLACE];
	}
}

- (void)onCallTerminated:(const CCallList&)lostCalls
			   withError:(int)err
{
	if (ER_OK == err)
	{
		[self writeMultiCallLog:lostCalls
					  withCause:CAUSE_TERMINATED];
		
		[self multiCallSummary:&lostCalls
					 withCause:CAUSE_TERMINATED];
	}
	else
	{
		[self failureMessage:CC_BTN_TERMINATE];
	}
}

- (void)onCallTransferred:(const CCallList&)lostCalls
				withError:(int)err
{
	if (ER_OK == err)
	{
		[self writeMultiCallLog:lostCalls
					  withCause:CAUSE_TRANSFERRED];
	}
	else
	{
	//	[self failureMessage:CC_BTN_TRANSFER];
	}
}
	
- (void)onCallDeflected:(const CCall*)pCall
			   toNumber:(const CString&)strNum
			  withError:(int)err
{
	if (ER_OK == err)
	{
		[self writeCallLog:pCall
				 withCause:CAUSE_DEFLECTED];
	}
	else
	{
	//	[self failureMessage:CC_BTN_DEFLECT];
	}
}

- (void)onDTMFSent:(int)err
{
	[self beginDtmf];
}

- (void)onUpdateCtrl
{
	[callListCtrl updateCtrl];
	
	if (popupWin != nil)
	{
		[popupWin updateCtrl];
	}
}
///////////////////////

- (void)onCallCtrl:(NSInteger)code
			 index:(int)idx
{
	CAERR err = CA_OK;
	
	switch (code)
	{
		case CC_BTN_SWITCH:
		case CC_BTN_SWAP:
			err = [engine Switch];
			break;
		case CC_BTN_ANSWER:
			if ([engine callList]->IsIncoming())
				err = [engine Answer];
			else
				err = [engine Switch];
			break;
		case CC_BTN_JOIN:
			err = [engine Join];
			break;
		case CC_BTN_EXTRACT:
			err = [engine Extract:idx];
			break;
		case CC_BTN_REPLACE:
			err = [engine Replace];
			break;
		case CC_BTN_REJECT:
			err = [engine Reject];
			break;
		case CC_BTN_TERMINATE:
			err = [engine Terminate:idx];
			break;
		case CC_BTN_CUT_ACTIVE:
			err = [engine Terminate:0];
			break;
		case CC_BTN_CUT_HELD:
			err = [engine ReleaseHeld];
			break;
		default:
			break;
	}
	
	if (err != CA_OK)
	{
		[self errorMessage:code
				 withError:err];
	}
}

- (void)makeCall:(NSString*)strDial
		withName:(NSString*)strName
{
	CString strForDial;
	CString strForName;
	
	[TheDelegate loadCallView:nil];
	[keyPadCtrl setText:strDial];

	[self clearDtmf];
	
	if (([strDial length] > 40)	|| // ???
		![self crackDialString:strDial
					   forDial:strForDial
					   forDtmf:strForDtmf])
	{
		[self showNotify:LoadString(@"IDS_CC_BADSTRING")
			   withOwner:TIP_OWNER_CALL];
		return;
	}
	
	if (strName != nil)
	{
		strForName = [NSString NSStringToCString:strName];
	}
	else
	{
		[self searchName:strForName
				byNumber:strForDial
				 andDTMF:strForDtmf];		
	}
	
	CAERR err = [engine MoCall:strForDial
					  withDtmf:strForDtmf
					   andName:strForName];

	if (err == CA_OK)
	{
		NSString *strAlp = (strForName.IsEmpty()) ?
			strDial : [NSString CStringToNSString:strForName];

		NSString *strFmt = LoadString(@"IDS_TIPS_DIALING");
		NSString *strMsg = [NSString stringWithFormat:strFmt, strAlp];
		
		[self showNotify:strMsg
			   withOwner:TIP_OWNER_CALL];
	}
	else
	{
		[self errorMessage:-1
				 withError:err];
	}
}

- (void)makeCall:(NSString*)strDial
{
	[self makeCall:strDial
		  withName:nil];
}

- (void)endSession
{
	if ([engine callList]->Size() > 0)
	{
		[self writeMultiCallLog:*[engine callList]
					  withCause:CAUSE_TERMINATED];
	}
}

- (void)HangupAll
{
	[engine HangUp];

}

@end

// private part

@implementation callControl(Private)

- (NSTimer*)setTimer:(NSTimeInterval)ti
			selector:(SEL)aSelector
			 repeats:(BOOL)yesOrNo
{
	NSTimer *timer =
		[NSTimer scheduledTimerWithTimeInterval:ti
										 target:self
									   selector:aSelector
									   userInfo:nil
										repeats:yesOrNo];
	
	NSRunLoop *theLoop = [NSRunLoop currentRunLoop];
	
	[theLoop addTimer:timer forMode:NSDefaultRunLoopMode];
	[theLoop addTimer:timer forMode:NSModalPanelRunLoopMode];
	[theLoop addTimer:timer forMode:NSEventTrackingRunLoopMode];
	
	return timer;
}

- (void)onTickTimer:(NSTimer*)timer
{
	[engine updateDuration];
	[callListCtrl updateTick];
}

- (void)startTickTimer
{
	ASSERT(tickTimer == nil);
	
	tickTimer = [self setTimer:1.0
					  selector:@selector(onTickTimer:)
					   repeats:YES];
}

- (void)stopTickTimer
{
	[tickTimer invalidate];
//	[tickTimer release];
	tickTimer = nil;
}

- (void)buildRing:(NSString*)name
{
//	ASSERT(ringTone == nil);
	
	NSString *wavPath =[[NSBundle mainBundle] pathForResource:name 
													   ofType:@"wav"];
	
	ringTone = [[[NSSound alloc] initWithContentsOfFile:wavPath
											byReference:NO] retain];
	
	[ringTone setName:name];
	
	if ([name isEqualToString:@"incoming"])
	{
		[ringTone setLoops:YES];
	}
}

- (void)startRing:(NSString*)name
{
	if (ringTone != nil)
	{
		[ringTone stop];
		
		if (![[ringTone name] isEqualToString:name])
		{
			[ringTone release];
			[self buildRing:name];
		}
	}
	else
	{
		[self buildRing:name];
	}
	
	[ringTone play];
}

- (void)stopRing
{
	[ringTone stop];
}

- (void)showPopup:(const CCall*)pCall
{
	ASSERT(popupWin == nil);
	
	popupWin = 
		[[[CallPopup alloc] initWithWindowNibName:@"callPopup"] retain];
	
	[popupWin bindCall:pCall
			 andEngine:engine];
	
	NSRect rcPop = [popupWin.window frame];
	
	NSRect rcScr = [[NSScreen mainScreen] visibleFrame];
	
	rcPop.origin.x = 
		rcScr.origin.x + rcScr.size.width - rcPop.size.width;
	
	rcPop.origin.y = 
		rcScr.origin.y + rcScr.size.height - rcPop.size.height;
	
	[popupWin showWindow:self
				  origin:rcPop.origin];
}

- (void)hidePopup
{
	[popupWin.window close];
	[popupWin release];
	popupWin = nil;
}

- (BOOL)showNotify:(NSString*)strMsg
		 withOwner:(unichar)owner
{
	if ([tipsBtn isHidden] ||
		IS_TIP_NOTIFY([tipsBtn tag]) ||
		OWNER_OF_TIP([tipsBtn tag]) == owner)
	{
		[tipsBtn setTag:MAKE_NOTIFY_OWNER(owner)];
		[tipsBtn setTitle:strMsg];
		[tipsBtn setHidden:NO];
	/*	
		NSAttributedString *coloredString;
		coloredString = [[NSAttributedString alloc] initWithString:@"Test"
														attributes:[NSDictionary dictionaryWithObject:[NSColor redColor]	
																							   forKey:NSForegroundColorAttributeName] ];
		[tipsBtn setAttributedTitle:coloredString];
	*/
		[tipsTimer invalidate];

		tipsTimer = [self setTimer:5.0
						  selector:@selector(onTipsTimer:)
						   repeats:NO];
		
		return YES;
	}
	
	return NO;
}

- (BOOL)showConfirm:(NSString*)strMsg
		  withOwner:(unichar)owner
		andSelector:(SEL)selector
{
	if (IS_TIP_CONFIRM([tipsBtn tag]))
	{
		return NO;
	}
	
	[self dismissTips];	
	
	[tipsBtn setTag:MAKE_CONFIRM_OWNER(owner)];
	[tipsBtn setTitle:strMsg];
	[tipsBtn setHidden:NO];
	
	tipsProc = selector;
	
	return YES;
}

- (void)dismissTips
{
	[tipsTimer invalidate];	
	
	[tipsBtn setHidden:YES];
	
	[tipsBtn setTag:0];
	
	tipsTimer = nil;
	tipsProc = nil;	
}

- (void)dismissTips:(unichar)owner
{
	if (owner == OWNER_OF_TIP([tipsBtn tag]))
	{
		[self dismissTips];
	}
}

- (void)onTipsTimer:(NSTimer*)timer
{
	[self dismissTips];
}

- (IBAction)onTipsBtn:(id)sender
{
	if (tipsProc != nil)
	{
		NSNumber *num = [NSNumber numberWithBool:YES];
		
		[self performSelector:tipsProc
				   withObject:num];
	}

	[self dismissTips];
}

- (void)writeCallLog:(const CCall&)call
		   withCause:(int)cause
{
	callHistoryViewController *ch =
		[(DCTAppDelegate*)TheDelegate chController];
	
	if (call.IsRinging())
	{
		if (CAUSE_TERMINATED == cause ||
			CAUSE_DEFLECTED == cause)
		{
			CCall alias(call);
			alias.status = CCall::DROPPED;
			[ch insertItem:alias];
			return;
		}
	}
	
	[ch insertItem:call];
}

- (void)writeMultiCallLog:(const CCallList&)calls
				withCause:(int)cause
{
	const CCall *p = NULL;
	
	CALLPOS pos = calls.GetFirstPos();
	
	while (NULL != (p=calls.GetNext(pos)))
	{
		[self writeCallLog:*p
				 withCause:cause];
	}
}

- (void)errorMessage:(NSInteger)code
			withError:(CAERR)err
{
	// show notification according to "err"
	
	NSString *str = nil;
	
	switch (err)
	{
		case CA_CALLFULL:
			str = LoadString(@"IDS_CC_CALLFULL");
			break;
		case CA_RESTRICTED:
			str = LoadString(@"IDS_CC_RESTRICTED");
			break;
		case CA_NOTALLOWED:
			str = LoadString(@"IDS_CC_NOTALLOWED");
			break;
		case CA_NONETWORK:
			str = LoadString(@"IDS_CC_RESTRICTED");
			break;
		case CA_OK:
			return;
		default:
			break;
	}

	if (str == nil)
	{
		[self failureMessage:code];
	}
	else
	{
		[self showNotify:str
			   withOwner:TIP_OWNER_CALL];
	}
}

- (void)failureMessage:(NSInteger)code
{
	NSString *strCmd = nil;
	
	switch (code)
	{
		case CC_BTN_SWITCH:
		case CC_BTN_SWAP:
			strCmd = LoadString(@"IDS_CC_CMD_SWITCH");
			break;
		case CC_BTN_ANSWER:
			strCmd = LoadString(@"IDS_CC_CMD_ANSWER");
			break;
		case CC_BTN_JOIN:
			strCmd = LoadString(@"IDS_CC_CMD_JOIN");
			break;
		case CC_BTN_EXTRACT:
			strCmd = LoadString(@"IDS_CC_CMD_EXTRACT");
			break;
		case CC_BTN_REPLACE:
			strCmd = LoadString(@"IDS_CC_CMD_REPLACE");
			break;
		case CC_BTN_REJECT:
			strCmd = LoadString(@"IDS_CC_CMD_REJECT");
			break;
		case CC_BTN_TERMINATE:
		case CC_BTN_CUT_ACTIVE:
		case CC_BTN_CUT_HELD:
			strCmd = LoadString(@"IDS_CC_CMD_RELEASE");
			break;
		default:
			strCmd = LoadString(@"IDS_CC_CMD_CALL");
			break;
	}

	NSString *str = [NSString stringWithFormat:@"%@: %@", 
					 strCmd, LoadString(@"IDS_CC_FAILURE")];
	
	[self showNotify:str
		   withOwner:TIP_OWNER_CALL];
}

- (void)multiCallSummary:(const CCallList*)calls
			   withCause:(int)cause
{
	CALLPOS pos = calls->GetFirstPos();
	
	const CCall *p = NULL;
	
	CString alpha;
	
	while ((p=calls->GetNext(pos)) != NULL)
	{
		if (alpha.GetLength() > 64)
		{
			alpha += _T("...");
			break;
		}
		else if (!alpha.IsEmpty())
		{
			alpha += _T(", ");
		}
		
		alpha += p->GetAlpha();
	}
	
	[self callSummary:alpha
			withCause:cause];
}

- (void)callSummary:(const CString&)alpha
		  withCause:(int)cause
{
	NSString *str = nil;
	
	switch (cause)
	{
		case 0:
		case 16:
		case 31:
			str = LoadString(@"IDS_CC_NORMALCLEAR");
			break;
		case 1:
			str = LoadString(@"IDS_CC_NUMUNASSIGN");
			break;
		case 8:
			str = LoadString(@"IDS_CC_OPBARRED");
			break;
		case 17:
			str = LoadString(@"IDS_CC_USERBUSY");
			break;
		case 18:
			str = LoadString(@"IDS_CC_NORESPOND");
			break;
		case 19:
			str = LoadString(@"IDS_CC_NOANSWER");
			break;
		case 21:
			str = LoadString(@"IDS_CC_REJECTED");
			break;
		case 22:
			str = LoadString(@"IDS_CC_NUMCHG");
			break;
		case 28:
			str = LoadString(@"IDS_CC_BADFORMAT");
			break;
		case 29:
			str = LoadString(@"IDS_CC_FACREJECT");
			break;
		case 38:
			str = LoadString(@"IDS_CC_NETERR");
			break;
		case 50:
			str = LoadString(@"IDS_CC_FACNOTSUB");
			break;
		case 55:
			str = LoadString(@"IDS_CC_BARREDCUG");
			break;
		case 68:
			str = LoadString(@"IDS_CC_ACMREACH");
			break;
		case 69:
			str = LoadString(@"IDS_CC_FACINVALID");
			break;
		case 87:
			str = LoadString(@"IDS_CC_CUGEXCLUDE");
			break;
		case CAUSE_TERMINATED:
			str = LoadString(@"IDS_CC_TERMINATED");
			break;
		case CAUSE_TRANSFERRED:
			str = LoadString(@"IDS_CC_TRANSFERRED");
			break;
		case CAUSE_DEFLECTED:
			str = LoadString(@"IDS_CC_DEFLECTED");
			break;
		case CAUSE_REPLACED:
			str = LoadString(@"IDS_CC_TERMINATED");
			break;
		default:
			str = LoadString(@"IDS_CC_CALLLOST");
			break;
	}
	
	NSString *strAlp = [NSString CStringToNSString:alpha];
	NSString *strTxt = [NSString stringWithFormat:@"%@: %@", str, strAlp];
	
	[self showNotify:strTxt
		   withOwner:TIP_OWNER_CALL];
}

- (BOOL)searchName:(CString&)name
		  byNumber:(const CString&)number
		   andDTMF:(const CString&)dtmf
{
	name.Empty();
	
	if ([TheDelegate searchName:name withNumber:number])
	{
		return YES;
	}
	else if (!dtmf.IsEmpty())
	{
		return [TheDelegate searchName:name
							withNumber:number+dtmf];
	}
	return NO;
}

@end
