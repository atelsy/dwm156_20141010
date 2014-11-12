//
//  CallEngine.h
//  DCT
//
//  Created by MBJ on 12-6-20.
//  Copyright 2012 MediaTek Inc. All rights reserved.
//

#import <Cocoa/Cocoa.h>
#import "CallClient.h"


@interface CallEngine : NSObject {

	BOOL			noCarrier;

	CCallList		callListData;
	
	CComAgent *		comObj;
	
	id<CallClient>	client;
	
	NSInteger		pending;

}

//@property(assign) CComAgent* comObj;

- (id)initWithContext:(id<CallClient>)client
	   comAgentObject:(CComAgent*)comAgent;

- (const CCallList*)callList;

- (BOOL)isPending;

////////////////
/*
- (CAERR)PinValidate:(const CString&)strPin
		   winNewPin:(const CString&)newPin;
*/

- (CAERR)MoCall:(const CString&)strDial
	   withDtmf:(const CString&)strDtmf
		andName:(const CString&)strName;

- (CAERR)SendDTMF:(const CString&)strDtmf;

- (CAERR)Deflect:(const CString&)strNum;

- (CAERR)Terminate:(int)nIdx;

- (CAERR)Extract:(int)nIdx;

- (CAERR)Answer;

- (CAERR)Replace;

- (CAERR)HangUp;

- (CAERR)ReleaseHeld;

- (CAERR)Transfer;

//- (CAERR)Swap;

- (CAERR)Switch;

- (CAERR)Join;

- (CAERR)Reject;

- (void)updateDuration;

// interfaces for UI
//////////////////////////////////////////////////////////////////////////

- (CAERR)MoCallTest;

- (CAERR)HangUpTest;

//- (CAERR)SwapTest;

- (CAERR)SwitchTest;

- (CAERR)DeflectTest;

- (CAERR)TransferTest;

- (CAERR)TerminateTest:(int)nIdx;

- (CAERR)ReleaseHeldTest;

- (CAERR)ReleaseActiveTest;

- (CAERR)JoinTest;

- (CAERR)ExtractTest:(int)nIdx;

- (CAERR)RejectTest;

- (CAERR)AnswerTest;

- (CAERR)ReplaceTest;

- (BOOL)DtmfTest:(short)idx;

@end
