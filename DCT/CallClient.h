//
//  CallClient.h
//  DCT
//
//  Created by MBJ on 12-6-20.
//  Copyright 2012 MediaTek Inc. All rights reserved.
//
#import "Win2Mac.h"
#import "ComAgent.h"
#import <Cocoa/Cocoa.h>

@protocol CallClient

- (void)onSystemState:(const DualWord&)nState;

- (BOOL)onIncomingCall:(const CCall*)pCall;
- (void)onOutgoingCall:(const CCall*)pCall;
- (BOOL)onWaitingCall:(const CCall*)pCall;

- (void)onIncomingEnd;
- (void)onOutgoingEnd;
- (void)onWaitingEnd;

- (void)onCallUpdated:(const CCallList&)allCalls;


- (void)onSSINotify:(const CSsnCode&)ssiCode;

- (void)onSSUNotify:(const CSsnCode&)ssuCode;

- (void)onCallDialed:(const CCall*)pCall
		   withError:(int)err;

- (void)onCallLost:(const CCallList*)calls
		 withCause:(int)cause;

- (void)onCallSwitched:(int)err;

- (void)onCallJoined:(int)err;

- (void)onCallSwapped:(int)err;

- (void)onCallAnswered:(const CCall*)pCall
			 withError:(int)err;

- (void)onCallExtracted:(const CCall*)pCall
			  withError:(int)err;

- (void)onCallReplaced:(const CCallList&)lostCalls
			  withCall:(const CCall*)pCall
			  andError:(int)err;

- (void)onCallTerminated:(const CCallList&)lostCalls
			   withError:(int)err;

- (void)onCallTransferred:(const CCallList&)lostCalls
				withError:(int)err;

- (void)onCallDeflected:(const CCall*)pCall
			   toNumber:(const CString&)strNum
			  withError:(int)err;

- (void)onDTMFSent:(int)err;
/*
- (void)onDisconnected:(int)nStat;

- (void)onConnecting;

- (void)onEmergencyOnly:(int)nReason;

- (void)onPinState:(short)err
		  withType:(short)nType
		 andRemain:(short)nRemain;

- (void)onNetworkStatus:(int)nStat
		   withOperator:(const CString&)strOp;
*/
//- (void)onUpdateDuration;

- (void)onUpdateCtrl;

@end
