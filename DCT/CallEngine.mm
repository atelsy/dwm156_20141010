//
//  client.mm
//  DCT
//
//  Created by MBJ on 12-6-20.
//  Copyright 2012 MediaTek Inc. All rights reserved.
//

#import "CallEngine.h"

enum req_step_t
{
	CC_MOCALL = 0,
	CC_EMCALL,
	CC_HANGUP,
	CC_ANSWER,
	CC_SWAP,
	CC_SWITCH,
	CC_DEFLECT,
	CC_JOIN,
	CC_EXTRACT,
	CC_REJECT,
	CC_REPLACE,
	CC_RELEASEHELD,
	CC_TRANSFER,	
	CC_DTMF,
	CC_CALLLOST
//	CC_PINVALID,
//	CC_OPERATOR

};


#define WM_URC_NOTIFY	54007
#define WM_CMD_ECHO		54008

#define CALL_MASK_NEW	0x01
#define CALL_MASK_LOST	0x02
#define CALL_MASK_ALL	(CALL_MASK_NEW|CALL_MASK_LOST)

#define isNetReady(obj)	\
	((obj)->GetSysState() >= SYS_NETREADY)


@interface CallEngine(Private)

- (CAERR)Pending:(CAERR)err;

- (void)unPending;

- (void)onCallLostErr:(const CCallList*)callList
			withCause:(int)cause;

- (BOOL)updateCalls:(const CCallList&)allCalls;

- (void)onCallStatus:(const CCallList&)calls 
			withMask:(NSUInteger)uMask;

- (void)onMocall:(CRespCalls*)pResp
	   withError:(int)err;

- (void)onEmcall:(CRespCalls*)pResp
	   withError:(int)err;

- (void)onUrcNotify:(long)urcCode
		   withInfo:(long)lParam;

- (void)onCmdEcho:(long)wParam
		   lParam:(long)lParam;

- (void)OnMessage:(long)Msg 
		   wParam:(long)wParam 
		   lParam:(long)lParam;
@end


@implementation CallEngine(Private)

- (CAERR)Pending:(CAERR)err
{
	ASSERT(pending >= 0);
	
	if (CA_OK == err)
	{
		if ((++pending) == 1)
		{
			[client onUpdateCtrl];
		}
	}
	return err;
}

- (void)unPending
{
	if ((--pending) == 0)
	{
		[client onUpdateCtrl];
	}
	
	ASSERT(pending >= 0);
}

- (void)onCallLostErr:(const CCallList*)callList
			withCause:(int)cause
{
	[client onCallLost:callList
			 withCause:cause];
}

- (BOOL)updateCalls:(const CCallList&)allCalls
{
	BOOL isIncoming = callListData.IsIncoming();
	BOOL isWaiting  = callListData.IsWaiting();
	BOOL isOutgoing = callListData.IsOutgoing();
	
	if (callListData.Update(allCalls))
	{
		[client onCallUpdated:callListData];
		[client onUpdateCtrl];
		
		if (isIncoming && !callListData.IsIncoming())
		{
			[client onIncomingEnd];
		}
		if (isWaiting && !callListData.IsWaiting())
		{
			[client onWaitingEnd];
		}
		if (isOutgoing && !callListData.IsOutgoing())
		{
			[client onOutgoingEnd];
		}
		
		return YES;
	}
	
	return NO;
}

- (void)onCallStatus:(const CCallList&)calls 
			withMask:(NSUInteger)uMask
{
    //dwm156
    return;
    
	CCall		theCall;
	CCallList	refList;
	
	//	1. get lost calls. 
	//	if any, get extended error code,
	//	then notify screen manager
	if (uMask & CALL_MASK_LOST)
	{
		if (callListData.GetLostCalls(refList, calls))
		{
			CCallList *p = new CCallList;
			
			*p = refList;
			
			CAERR err = comObj->GetExtendErr(
				self, WM_CMD_ECHO, CC_CALLLOST, (LONG)p);
			
			if (CA_OK != err)
			{
				[self onCallLostErr:p
						  withCause:-1];
			}
		}
	}
	
	//	2. get new calls. (mainly incoming)
	//	if any, notify screen manager
	if (uMask & CALL_MASK_NEW)
	{
		refList.Reset();
		
		if (callListData.GetNewCalls(refList, calls))
		{
			// incoming + waiting + alerting <= 2
			ASSERT(refList.Size() <= 2);
			
			while (refList.Pop(&theCall))
			{
				BOOL toUpdate = NO;
				
				if (theCall.IsIncoming())
				{
					toUpdate = [client onIncomingCall:&theCall];
				}
				else if (theCall.IsWaiting())
				{
					toUpdate = [client onWaitingCall:&theCall];
				}
				else if (theCall.IsOutgoing())
				{
					[client onOutgoingCall:&theCall];
				}

				// UI may have changed "alpha" of the new call
				if (toUpdate)
				{
					((CCallList&)calls).Update(theCall);
				}
			}
		}
	}
	
	//	3. get changed calls. (active, held, waiting)
	//	if any, notify screen manager
	[self updateCalls:calls];
}

- (void)onMocall:(CRespCalls*)pResp
	   withError:(int)err
{
	CCallList newCalls;
	CCall *pCall = (CCall*)(pResp->m_lExt);
	const CCallList *pAllCalls = &(pResp->Value());
	
	ASSERT (NULL != pCall);
	
	if (ER_USERABORT != err)
	{
		callListData.GetNewCalls(newCalls, *pAllCalls);
	}
	
	// the outgoing call was not put into the call list
	// because its index was unknown yet
	// so now it's time to add it to the list
	if (newCalls.Size() > 0)
	{
		CCall *pNew = NULL;
		
		CALLPOS pos = newCalls.GetFirstPos();
		
		while (NULL != (pNew=(CCall*)(newCalls.GetNext(pos))))
		{
			if (pNew->IsMoCall())
				break;
		}
		
		if (NULL != pNew)
		{
			if (pNew->alpha.IsEmpty())
				pNew->alpha = pCall->alpha;
			if (pCall->GetDtmf())
				pNew->SetDtmf(*(pCall->GetDtmf()));
			
			pCall->idx = pNew->idx;
			((CCallList*)pAllCalls)->Update(*pNew);
			
			[self onCallStatus:*pAllCalls
					  withMask:CALL_MASK_ALL];
		//	[self updateCalls:*pAllCalls];
			
			[client onCallDialed:pNew
					   withError:err];
			
			delete pCall;
			
			return;
		}
	}
	
	if (ER_OK == err)
	{
		[self onCallStatus:*pAllCalls
				  withMask:CALL_MASK_ALL];
	//	[self updateCalls:*pAllCalls];
	//	return;
	}
	else
	{
		callListData.ReCalcGlobalStatus();
	}
	
	/*
	 if (ER_USERABORT != err && ER_TIMEOUT != err)
	 {
	 //	AT+CEER
	 CAERR ca_err = comObj->GetExtendErr(m_hWnd, 
	 WM_ATRESPONSE, CC_MOCALLEX, pResp->m_lExt);
	 
	 if (CA_OK == ca_err)
	 return;
	 }
	 */
	
	[client onCallDialed:pCall
			   withError:-1];
	
	delete pCall;	
}

// 'AT+CLCC' is not available for EM call
- (void)onEmcall:(CRespCalls*)pResp
	   withError:(int)err
{
	CCall *pCall = (CCall*)(pResp->m_lExt);
	ASSERT (NULL != pCall);
	
	if (ER_OK == err && noCarrier)
		err = ER_UNKNOWN;
	
	if((ER_OK == err) && (comObj->GetSysState() > SYS_ATREADY))
	{
		// the outgoing call was not put into the call list
		// because its index was unknown yet
		// so now it's time to add it to the list

		ASSERT(callListData.Size() == 0);
		
		pCall->idx = 1;	// only 1 call possible
		pCall->status = CCall::ALERTING;
		pCall->SetDuration(0);
		
		CCallList allCalls;
		allCalls.Append(*pCall);
		
		[self updateCalls:allCalls];
		
		[client onCallDialed:pCall
				   withError:err];
	}
	else
	{
		[client onCallDialed:pCall
				 withError:-1];
		
		callListData.ReCalcGlobalStatus();
	}
	
	delete pCall;
}

- (void)onUrcNotify:(long)urcCode
		   withInfo:(long)lParam
{
	WATCH_POINTER(lParam);
	
	switch (urcCode)
	{
		case URC_SYS:
            if (((CUrcSys*)lParam)->Value().alpha <= SYS_DISC)
            {
                [self onCallStatus:CCallList()
                          withMask:CALL_MASK_LOST];
            }
			[client onSystemState:((CUrcSys*)lParam)->Value()];
			break;
		case URC_CLCC:
			[self onCallStatus:((CUrcCalls*)lParam)->Value()
					  withMask:CALL_MASK_ALL];
			break;
		case URC_NOCA:
			noCarrier = YES;
			if (callListData.Size() == 1 && callListData.IsEmCall())
			{
				[self onCallStatus:CCallList()
						  withMask:CALL_MASK_LOST];	// emergency call lost
			}
			break;
		case URC_CSSI:
			[client onSSINotify:((CUrcCssn*)lParam)->Value()];
			break;
		case URC_CSSU:
			[client onSSUNotify:((CUrcCssn*)lParam)->Value()];
			break;
		default:
			break;
	}
}

- (void)onCmdEcho:(long)wParam
		   lParam:(long)lParam
{
    
	WATCH_POINTER(lParam);
	
	short step = LOWORD(wParam);
	short err  = HIWORD(wParam);
	
	switch (step)
	{
		case CC_MOCALL:
			[self onMocall:(CRespCalls*)lParam
				 withError:err];
			break;
			
		case CC_EMCALL:
			[self onEmcall:(CRespCalls*)lParam
				 withError:err];
			break;
	/*
		case CC_OPERATOR:
			 {
			 CRespText *p = (CRespText*)lParam;
			 
			 int count = HIWORD(p->m_lExt);
			 int state = LOWORD(p->m_lExt);
			 
			 if (ER_OK == err && !(p->Value().IsEmpty()))
			 {
			 [self onNetworkStatus:state
			 withOperator:p->Value()];
			 }
			 else if (count >= 3)
			 {
			 [self onNetworkStatus:state
			 withOperator:_T("")];
			 }
			 else
			 {
			 CAERR err = comObj->QueryOperator(
			 m_hWnd, WM_ATRESPONSE, CC_OPERATOR, 
			 MAKElParam(state, count+1));
			 if (CA_OK != err)
			 {
			 [self onNetworkStatus:state
			 withOperator:_T("")];
			 }
			 }
			 }
			 break;
		*/
		case CC_CALLLOST:
		{
			CRespErrex *p = (CRespErrex*)lParam;
			
			[self onCallLostErr:(CCallList*)(p->m_lExt)
					  withCause:p->Value().cause];
			
			break;
		}
			
		case CC_SWAP:
			if (ER_OK == err)
			{
				CRespCalls *p = (CRespCalls*)lParam;
				[self onCallStatus:p->Value()
						  withMask:CALL_MASK_ALL];
			//	updateCalls(p->Value());
			}
			[self unPending];
			[client onCallSwapped:err];
			break;
			
		case CC_SWITCH:
			if (ER_OK == err)
			{
				CRespCalls *p = (CRespCalls*)lParam;
				[self onCallStatus:p->Value()
						  withMask:CALL_MASK_ALL];
			//	[self updateCalls:p->Value()];
			}
			[self unPending];
			[client onCallSwitched:err];
			break;
			
		case CC_JOIN:
			if (ER_OK == err)
			{
				CRespCalls *p = (CRespCalls*)lParam;
				[self onCallStatus:p->Value()
						  withMask:CALL_MASK_ALL];
			//	[self updateCalls:p->Value()];
			}
			[self unPending];
			[client onCallJoined:err];
			break;
			
		case CC_EXTRACT:
		{
			CRespCalls *p = (CRespCalls*)lParam;
			
			CCall theCall(callListData.Get(p->m_lExt));
			
			if (ER_OK == err)
			{
				[self onCallStatus:p->Value()
						  withMask:CALL_MASK_ALL];
			//	[self updateCalls:p->Value()];
			}
			
			[self unPending];

			[client onCallExtracted:&theCall
						  withError:err];
			
			break;
		}
			
		case CC_DEFLECT:
		{
			CRespCalls *p = (CRespCalls*)lParam;
			CString *pStr = (CString*)(p->m_lExt);
			
			CCall theCall(callListData.GetRingingCall());
			
			if (ER_OK == err)
			{
				[self onCallStatus:p->Value()
						  withMask:CALL_MASK_NEW];
			//	[self updateCalls:p->Value()];
			}
			
			[self unPending];

			[client onCallDeflected:&theCall
						   toNumber:*pStr
						  withError:err];
			
			delete pStr;
			
			break;
		}
			
		case CC_TRANSFER:
		{
			CRespCalls *p = (CRespCalls*)lParam;
			CCallList refList;
			
			if (ER_OK == err)
			{
				callListData.GetLostCalls(refList, p->Value());
				[self onCallStatus:p->Value()
						  withMask:CALL_MASK_NEW];
			//	[self updateCalls:p->Value()];
			}
			
			[self unPending];

			[client onCallTransferred:refList
							withError:err];
			
			break;
		}
			
		case CC_REJECT:	// what to do?
		case CC_RELEASEHELD:
		case CC_HANGUP:
		{
			CCallList refList;
			CRespCalls *p = (CRespCalls*)lParam;
			
			if (ER_OK == err)
			{
				if (comObj->GetSysState() >= SYS_NETREADY)
				{
					callListData.GetLostCalls(refList, p->Value());
					[self onCallStatus:p->Value()
							  withMask:CALL_MASK_NEW];
				//	[self updateCalls:p->Value());
				}
				else
				{
					refList = callListData;		// all terminated (only 1 in fact)
					[self updateCalls:CCallList()];	// no calls now
				}
			}
			
			[self unPending];

			[client onCallTerminated:refList
						   withError:err];
			
			break;
		}
			
		case CC_REPLACE:
		{
			CCallList refList;
			CCall theCall(callListData.GetWaitingCall());
			CRespCalls *p = (CRespCalls*)lParam;
			
			if (ER_OK == err)
			{
				callListData.GetLostCalls(refList, p->Value());
				[self onCallStatus:p->Value()
						  withMask:CALL_MASK_NEW];
			//	[self updateCalls:p->Value()];
			}
			
			[self unPending];

			[client onCallReplaced:refList
						  withCall:&theCall
						  andError:err];
			
			break;
		}
			
		case CC_ANSWER:
		{
			CRespCalls *p = (CRespCalls*)lParam;
			
			CCall theCall(callListData.GetRingingCall());
			
			if (ER_OK == err)
			{
				[self onCallStatus:p->Value()
						  withMask:CALL_MASK_ALL];
			//	[self updateCalls:p->Value()];
			}
			
			[self unPending];

			[client onCallAnswered:&theCall
						 withError:err];
			
			break;
		}
			
		case CC_DTMF:
			[client onDTMFSent:err];
			break;
/*
		case CC_PINVALID:
			if (ER_WRONGPWD == err ||
				ER_PUKREQ == err)
			{
				CRespInt *p = (CRespInt*)lParam;
				int nStat = p->Value();
				[self onPinState:err
						withType:LOWORD(nStat)
					   andRemain:HIWORD(nStat)];
			}
			else
			{
				[self onPinState:err
						withType:0
					   andRemain:0];	// password right or not
			}
			break;
*/
		default:
			break;
	}
}

- (void)OnMessage:(long)Msg 
		   wParam:(long)wParam 
		   lParam:(long)lParam
{
	if (Msg == WM_URC_NOTIFY)
	{
		[self onUrcNotify:wParam
				 withInfo:lParam];
	}
	else if (Msg == WM_CMD_ECHO)
	{
		[self onCmdEcho:wParam
				 lParam:lParam];
	}
}

@end
// end of category


@implementation CallEngine

- (id)initWithContext:(id<CallClient>)theClient
	   comAgentObject:(CComAgent*)comAgent
{
	if (self = [super init])
	{
		noCarrier	= NO;
		comObj		= comAgent;
		client		= theClient;
		pending		= 0;
		
		comObj->RegisterURC(URC_SYS , self, WM_URC_NOTIFY);
		comObj->RegisterURC(URC_CCWV, self, WM_URC_NOTIFY);
		comObj->RegisterURC(URC_CSSI, self, WM_URC_NOTIFY);
		comObj->RegisterURC(URC_CSSU, self, WM_URC_NOTIFY);
		comObj->RegisterURC(URC_CLCC, self, WM_URC_NOTIFY);
		comObj->RegisterURC(URC_NOCA, self, WM_URC_NOTIFY);
	}
	return self;
}

- (const CCallList*)callList
{
	return &callListData;
}

- (BOOL)isPending
{
	return (pending > 0);
}
/*
- (CAERR)PinValidate:(const CString&)strPin
		   winNewPin:(const CString&)newPin
{
	ASSERT (comObj->GetSysState() == SYS_PINREQ ||
			comObj->GetSysState() == SYS_PUKREQ);
	
	return comObj->PinValidate(
				self, WM_CMD_ECHO, CC_PINVALID, strPin, newPin);
}
*/

- (CAERR)MoCall:(const CString&)strDial
	   withDtmf:(const CString&)strDtmf
		andName:(const CString&)strName
{
	CCall *pCall	= new CCall;
	
	pCall->idx		= 0;
	pCall->dir		= CCall::MOC;
	pCall->status	= CCall::DIALING;
	pCall->mode		= 0;	// voice call
	pCall->mpty		= 0;
	pCall->type		= 0;	// ???
	pCall->alpha	= strName;
	pCall->number	= strDial;
	
	pCall->SetDtmf(strDtmf);
	
	noCarrier = NO;
	
	CAERR err = comObj->Dial(
		self, WM_CMD_ECHO, 
		isNetReady(comObj) ? CC_MOCALL : CC_EMCALL, 
		strDial, (LONG)pCall);
	
	if (CA_OK != err)
	{
		delete pCall;
		return err;
	}
	
	callListData.UpdateGlobalStatus(CCall::DIALING);
	
	return CA_OK;
}

- (CAERR)Answer
{
	CAERR err = comObj->Answer(
		self, WM_CMD_ECHO, CC_ANSWER);
	
	return [self Pending:err];
}

- (CAERR)Deflect:(const CString&)strNum
{
	CString *pStr = new CString(strNum);
	
	CAERR err = comObj->Deflect(
		self, WM_CMD_ECHO, CC_DEFLECT, strNum, (LONG)pStr);
	
	if (CA_OK != err)
		delete pStr;
	
	return [self Pending:err];
}

- (CAERR)Replace
{
	CAERR err = comObj->ChangeCallLine(
		self, WM_CMD_ECHO, CC_REPLACE, 1);
	
	return [self Pending:err];
}

- (CAERR)HangUp
{
	CAERR err = comObj->HangUp(
		self, WM_CMD_ECHO, CC_HANGUP, 0);
	
	return [self Pending:err];
}

- (CAERR)Terminate:(int)nIdx
{
	if (callListData.IsOutgoing(nIdx))
//	if (callListData.IsEmCall(nIdx)) // +CHLD doesn't work for ECC
	{
		return [self HangUp];
	}
	else
	{
		CAERR err;
		
		err = comObj->ChangeCallLine(
			self, WM_CMD_ECHO, CC_HANGUP, 1, nIdx, nIdx);
		
		return [self Pending:err];
	}
}

// release all held calls
- (CAERR)ReleaseHeld
{
	CAERR err = comObj->ChangeCallLine(
		self, WM_CMD_ECHO, CC_RELEASEHELD, 0);
	
	return [self Pending:err];
}

// ???
- (CAERR)Transfer
{
	CAERR err = comObj->ChangeCallLine(
		self, WM_CMD_ECHO, CC_TRANSFER, 4);
	
	return [self Pending:err];
}
/*
- (CAERR)Swap
{
	CAERR err = comObj->ChangeCallLine(
		self, WM_CMD_ECHO, CC_SWAP, 2);
	
	return [self Pending:err];
}
*/
- (CAERR)Switch
{
	CAERR err = comObj->ChangeCallLine(
		self, WM_CMD_ECHO, CC_SWITCH, 2);
	
	return [self Pending:err];
}

// split a call and hold conference
- (CAERR)Extract:(int)nIdx
{
	CAERR err = comObj->ChangeCallLine(
		self, WM_CMD_ECHO, CC_EXTRACT, 2, nIdx, nIdx);
	
	return [self Pending:err];
}

// add active call to conference
- (CAERR)Join
{
	CAERR err;
	
	if (callListData.IsMptyHeld())
	{
		err = comObj->ChangeCallLine(
			self, WM_CMD_ECHO, CC_JOIN, 0x02 | (0x03 << 8));
	}
	else
	{
		err = comObj->ChangeCallLine(
			self, WM_CMD_ECHO, CC_JOIN, 3);
	}
	
	return [self Pending:err];
}

// user defined user busy
// to reject an incoming or waiting call
// but the calling party gets a message of "busy"
- (CAERR)Reject
{
	CAERR err = comObj->ChangeCallLine(
		self, WM_CMD_ECHO, CC_REJECT, 0);
	
	return [self Pending:err];
}

- (CAERR)SendDTMF:(const CString&)strDtmf
{
	return comObj->SendDTMF(
		self, WM_CMD_ECHO, CC_DTMF, strDtmf);
}

- (void)updateDuration
{
	callListData.UpdateDuration();
}
// interfaces for UI
//////////////////////////////////////////////////////////////////////////


// logic test
- (CAERR)MoCallTest
{
	if ([self isPending])
		return CA_NOSENSE;
	
	if (callListData.IsOutgoing())
		return CA_NOTALLOWED;
	if (callListData.IsIncoming())
		return CA_NOTALLOWED;
	if (!callListData.IsSingle())
		return CA_CALLFULL;
	
	return CA_OK;
}

- (CAERR)HangUpTest
{
	if ([self isPending])
		return CA_NOSENSE;
	
	if (callListData.IsIdle())
		return CA_NOSENSE;
	
	return CA_OK;
}
/*
- (CAERR)SwapTest
{
	if ([self isPending])
		return CA_NOSENSE;
	
	// "AT+CHLD=2" : Places all active calls on hold, and
	//				 accepts the other (waiting or held) call
	if (callListData.IsOutgoing() || 
		callListData.IsWaiting())
	{
		return CA_NOTALLOWED;
	}
	if (callListData.IsActive() && 
		callListData.IsHeld())
	{
		return CA_OK;
	}
	
	return CA_NOTALLOWED;
}

- (CAERR)SwitchTest
{
	if ([self isPending])
		return CA_NOSENSE;
	
	// "AT+CHLD=2" : Places all active calls on hold, and
	//				 accepts the other (waiting or held) call
	if (callListData.IsOutgoing())
		return CA_NOTALLOWED;
// 	if (callListData.IsRinging())
// 		return CA_NOTALLOWED;
	if (callListData.IsActive())
	{
		if (callListData.IsHeld())
			return CA_NOTALLOWED;
		else
			return CA_OK;
	}
	else if (callListData.IsHeld())
	{
		return CA_OK;
	}
	
	return CA_NOTALLOWED;
}
*/

- (CAERR)SwitchTest
{
	if ([self isPending])
		return CA_NOSENSE;
	
	// "AT+CHLD=2" : Places all active calls on hold, and
	//				 accepts the other (waiting or held) call
	if (callListData.IsOutgoing() || 
		callListData.IsWaiting())
	{
		return CA_NOTALLOWED;
	}
	
	if (callListData.IsActive() || 
		callListData.IsHeld())
	{
		return CA_OK;
	}
	
	return CA_NOTALLOWED;
}

- (CAERR)DeflectTest
{
	if ([self isPending])
		return CA_NOSENSE;
	
	if (callListData.IsOutgoing())
		return CA_NOTALLOWED;
	if (callListData.IsRinging())
		return CA_OK;
	
	return CA_NOTALLOWED;
}

- (CAERR)TransferTest
{
	if ([self isPending])
		return CA_NOSENSE;
	
	if (callListData.IsOutgoing())
		return CA_NOTALLOWED;
	
// 	if (callListData.IsMpty())		// todo: to be confirmed
// 		return CA_NOTALLOWED;
	
	if (callListData.IsActive() && 
		callListData.IsHeld())
	{
		return CA_OK;
	}
	
	return CA_NOTALLOWED;
}

- (CAERR)TerminateTest:(int)nIdx
{
/*	if (nIdx == 0)
	{
		return [self ReleaseActiveTest];
	}
*/
	if (nIdx < 1)
		return CA_NOSENSE;
	
	if ([self isPending])
		return CA_NOSENSE;
	
	// ATH in this case
	if (callListData.IsOutgoing(nIdx))
		return CA_OK;
	
	// "AT+CHLD=1X" : Releases the specific active call X	
	if (callListData.IsActive(nIdx))
		return CA_OK;
	else if (callListData.IsEmCall(nIdx))
		return CA_OK;
	else
		return CA_NOTALLOWED;
}

- (CAERR)ReleaseHeldTest
{
	if ([self isPending])
		return CA_NOSENSE;
	
	// "AT+CHLD=0" : Release all held calls,
	//				 or set UDUB for a waiting call
	if (callListData.IsOutgoing())
	{
		return CA_NOTALLOWED;
	}
	if (callListData.IsHeld() && 
		!callListData.IsWaiting())
	{
		return CA_OK;
	}
	
	return CA_NOSENSE;
}

- (CAERR)ReleaseActiveTest
{
	if ([self isPending])
		return CA_NOSENSE;
	
	// "AT+CHLD=1" : Releases all active calls and 
	//				 accept the other (waiting or held) call
	if (callListData.IsWaiting())
		return CA_NOTALLOWED;
	
	if (callListData.IsActive())
		return CA_OK;
	
	return CA_NOTALLOWED;
}

- (CAERR)JoinTest
{
	if ([self isPending])
		return CA_NOSENSE;
	
	// "AT+CHLD=3" : Adds a held call to the conversation
	// 	if (callListData.IsMptyHeld())
	// 		return CA_NOTALLOWED;
	if (callListData.IsSingle())
		return CA_NOTALLOWED;
	if (callListData.GetMptyCount() >= 5)
		return CA_CALLFULL;
	
	return CA_OK;
}

- (CAERR)ExtractTest:(int)nIdx
{
	if ([self isPending])
		return CA_NOSENSE;
	
	if (nIdx < 1)
		return CA_NOSENSE;
	
	// "AT+CHLD=2X" : Places all active calls, 
	//				  except call X, on hold
	if (callListData.IsMptyActive(nIdx) && 
		callListData.IsSingle())
	{
		return CA_OK;
	}
	else
	{
		return CA_NOTALLOWED;
	}
}

- (CAERR)RejectTest
{
	if ([self isPending])
		return CA_NOSENSE;
	
	// "AT+CHLD=0" : Release all held calls,
	//				 or set UDUB for a waiting call
	if (callListData.IsOutgoing())
		return CA_NOTALLOWED;
	if (callListData.IsRinging())
		return CA_OK;
	
	return CA_NOSENSE;
}

- (CAERR)AnswerTest
{
	if ([self isPending])
		return CA_NOSENSE;
	
	// ATA : Answers an incoming / waiting call
	if (callListData.IsOutgoing())
		return CA_NOTALLOWED;
	if (!callListData.IsSingle())
		return CA_CALLFULL;
	if (callListData.IsRinging())
		return CA_OK;
	
	return CA_NOSENSE;
}

- (CAERR)ReplaceTest
{
	if ([self isPending])
		return CA_NOSENSE;
	
	// "AT+CHLD=1" : Releases all active calls and 
	//				 accept the other (waiting or held) call
	if (callListData.IsOutgoing())
	{
		return CA_NOTALLOWED;
	}
	if (callListData.IsWaiting() && 
		callListData.IsActive())
	{
		return CA_OK;
	}
	
	return CA_NOTALLOWED;
}

- (BOOL)DtmfTest:(short)idx
{
	return callListData.IsDtmfAvailable(idx);
}

@end