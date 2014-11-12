/*****************************************************************************
*  Copyright Statement:
*  --------------------
*  This software is protected by Copyright and the information contained
*  herein is confidential. The software may not be copied and the information
*  contained herein may not be used or disclosed except with the written
*  permission of MediaTek Inc. (C) 2007
*
*  BY OPENING THIS FILE, BUYER HEREBY UNEQUIVOCALLY ACKNOWLEDGES AND AGREES
*  THAT THE SOFTWARE/FIRMWARE AND ITS DOCUMENTATIONS ("MEDIATEK SOFTWARE")
*  RECEIVED FROM MEDIATEK AND/OR ITS REPRESENTATIVES ARE PROVIDED TO BUYER ON
*  AN "AS-IS" BASIS ONLY. MEDIATEK EXPRESSLY DISCLAIMS ANY AND ALL WARRANTIES,
*  EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE IMPLIED WARRANTIES OF
*  MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE OR NONINFRINGEMENT.
*  NEITHER DOES MEDIATEK PROVIDE ANY WARRANTY WHATSOEVER WITH RESPECT TO THE
*  SOFTWARE OF ANY THIRD PARTY WHICH MAY BE USED BY, INCORPORATED IN, OR
*  SUPPLIED WITH THE MEDIATEK SOFTWARE, AND BUYER AGREES TO LOOK ONLY TO SUCH
*  THIRD PARTY FOR ANY WARRANTY CLAIM RELATING THERETO. MEDIATEK SHALL ALSO
*  NOT BE RESPONSIBLE FOR ANY MEDIATEK SOFTWARE RELEASES MADE TO BUYER'S
*  SPECIFICATION OR TO CONFORM TO A PARTICULAR STANDARD OR OPEN FORUM.
*
*  BUYER'S SOLE AND EXCLUSIVE REMEDY AND MEDIATEK'S ENTIRE AND CUMULATIVE
*  LIABILITY WITH RESPECT TO THE MEDIATEK SOFTWARE RELEASED HEREUNDER WILL BE,
*  AT MEDIATEK'S OPTION, TO REVISE OR REPLACE THE MEDIATEK SOFTWARE AT ISSUE,
*  OR REFUND ANY SOFTWARE LICENSE FEES OR SERVICE CHARGE PAID BY BUYER TO
*  MEDIATEK FOR SUCH MEDIATEK SOFTWARE AT ISSUE.
*
*  THE TRANSACTION CONTEMPLATED HEREUNDER SHALL BE CONSTRUED IN ACCORDANCE
*  WITH THE LAWS OF THE STATE OF CALIFORNIA, USA, EXCLUDING ITS CONFLICT OF
*  LAWS PRINCIPLES.  ANY DISPUTES, CONTROVERSIES OR CLAIMS ARISING THEREOF AND
*  RELATED THERETO SHALL BE SETTLED BY ARBITRATION IN SAN FRANCISCO, CA, UNDER
*  THE RULES OF THE INTERNATIONAL CHAMBER OF COMMERCE (ICC).
*
*****************************************************************************/

/*******************************************************************************
* Filename:
* ---------
*   requests.cpp
*
* Project:
* --------
*   DCT
*
* Description:
* ------------
*   Implementions of ComAgent requests
*
* Author:
* -------
*   Xiantao Han
*
*******************************************************************************/

#ifdef WIN32
#include "StdAfx.h"
#else
#include "Win2Mac.h"
#endif

#include "ComAgent.h"


#define CreateRequest(_type, _tag)			\
	_type *_tag = new _type;				\
	_tag->Init(hWnd, uMsg, step, lExt)

#define CreateRequestEx(_type, _tag, _clcc)	\
	_type *_tag = new _type;				\
	_tag->Init(hWnd, uMsg, step, lExt, _clcc)

#define CreateCallRequest(_type, _tag)		\
	CreateRequestEx(_type, _tag, true)

#define _NPARAM(p, i)	p->Param().nParam[i]
#define _SPARAM(p, i)	p->Param().sParam[i]
#define _PPARAM(p, i)	p->Param().pParam[i]

//////////////////////////////////////////////////////////////////////////
// internal request
CAERR CComAgent::PowerOn(HWND hWnd, UINT uMsg, short step, 
				BOOL bRF /* = TRUE*/, LONG lExt /*= 0*/)
{
	if (m_nSysState != SYS_ATREADY)
		return CA_NOTALLOWED;

	CreateRequest(CReqPowerOn, p);

	_NPARAM(p, 0) = bRF ? 1 : 4;

	// Of course it's of high priority
	return HighRequest(p);
}

CAERR CComAgent::SwitchDbgPort(
	HWND hWnd, UINT uMsg, short step, BOOL bFlag, LONG lExt)
{
	if (m_nSysState < SYS_ATREADY)
		return CA_NOTALLOWED;
	
	CreateRequest(CReqSetDbgPort, p);
	
	_NPARAM(p, 0) = bFlag ? 1 : 0;
	
	return NormalRequest(p);
}

CAERR CComAgent::SwitchSDLogging(
	HWND hWnd, UINT uMsg, short step, BOOL bFlag, LONG lExt)
{
	if (m_nSysState < SYS_ATREADY)
		return CA_NOTALLOWED;

	CreateRequest(CReqSetLogMode, p);
		
	_NPARAM(p, 0) = bFlag ? 1 : 0;

	return NormalRequest(p);
}

//
CAERR CComAgent::PowerOff(
	HWND hWnd, UINT uMsg, short step /*= 0*/, LONG lExt /*= 0*/)
{
//	AbortAll();
	
	CreateRequest(CReqPowerOff, p);
	
	if (m_nSysState < SYS_ATREADY)
	{
		_NPARAM(p, 0) = -1;
	}
	else
	{
		_NPARAM(p, 0) = 4;	// AT+CUFN=4
	}

	return NormalRequest(p);
}

// initialize target, before PIN validation
CAERR CComAgent::InitTarget0(HWND hWnd, UINT uMsg, short step, LONG lExt /*= 0*/)
{
/*
	// basic setting
	AT+ELSP=0
	ATE0
	AT+CMEE=1
	AT+CSCS=UCS2 ???

	// reset calls
	ATH
	
	// enable urc
	AT+CREG=2		// +CREG:
	AT+ECSQ=1		// +ECSQ:
*/

	if (m_nSysState < SYS_ATREADY)
		return CA_NOTALLOWED;

	CreateRequest(CReqInitBasic, p);

	// init is of high priority
	return HighRequest(p);
}

// initialize target, after PIN validation
CAERR CComAgent::InitTarget1(HWND hWnd, UINT uMsg, short step, LONG lExt /*= 0*/)
{
/*
	// sms setting
	AT+CMGF=0

	// enable urc
	AT+CNMI=...		// +CMTI:
	AT+CAOC=1		// +CCCM:
	AT+CCWE=1		// +CCWV:
	AT+CLIP=1		// +CLIP:
	AT+COLP=1		// +COLP:
	AT+CSSN=1,1		// +CSSI:, +CSSU:
*/

	if (m_nSysState < SYS_PINREADY)
		return CA_NOTALLOWED;

	CreateRequest(CReqInitAdv, p);

	// init is of high priority
	return HighRequest(p);
}

// initialize target, after network ready
CAERR CComAgent::InitTarget2(HWND hWnd, UINT uMsg, short step, LONG lExt /*= 0*/)
{
/*
	// reset USSD
	AT+CUSD=2

	// enable urc
	AT+CUSD=1		// +CUSD:
*/

	if (m_nSysState < SYS_NETREADY)
		return CA_NOTALLOWED;

	CreateRequest(CReqInitNet, p);

	// init is of high priority
	return HighRequest(p);
}

// at+esims?
CAERR CComAgent::QuerySIM(HWND hWnd, UINT uMsg, short step, LONG lExt /*= 0*/)
{
	if (m_nSysState < SYS_ATREADY)
		return CA_NOTALLOWED;

	CreateRequest(CReqQuerySim, p);

	return NormalRequest(p);
}

// at+cpin?
CAERR CComAgent::QueryPIN(HWND hWnd, UINT uMsg, short step, int nPinType, LONG lExt /*= 0*/)
{
	if (m_nSysState < SYS_ATREADY)
		return CA_NOTALLOWED;

	CreateRequest(CReqQueryPin, p);

	_NPARAM(p, 0) = nPinType;

	return NormalRequest(p);
}

// at+eind?, at+creg?
CAERR CComAgent::QueryInd(HWND hWnd, UINT uMsg, short step, LONG lExt /*= 0*/)
{
	if (m_nSysState < SYS_PINREADY)
		return CA_NOTALLOWED;

	CreateRequest(CReqQueryInd, p);

	return NormalRequest(p);
}

// at+clcc, immediately after CC requests
CAERR CComAgent::QueryCall(HWND hWnd, UINT uMsg, short step, LONG lExt /*= 0*/)
{
	if (m_nSysState < SYS_PINREADY)
		return CA_NOTALLOWED;

	CreateRequest(CReqQueryCall, p);

	ResetPolling(PL_CLCC);

	return HighRequest(p);
}

// internal request
//////////////////////////////////////////////////////////////////////////

// User requests
CAERR CComAgent::PinValidate(HWND hWnd, UINT uMsg, short step, 
							 const CString& strPin, 
							 const CString& newPin /*= CString()*/, 
							 LONG lExt /*= 0*/)
{
	if (!newPin.IsEmpty())
	{
		if (m_nSysState != SYS_PUKREQ)
			return CA_NOTALLOWED;
	}
	else if (m_nSysState != SYS_PINREQ)
	{
		return CA_NOTALLOWED;
	}

	CreateRequest(CReqPinValidate, p);

	_SPARAM(p, 0) = strPin;
	_SPARAM(p, 1) = newPin;

	NormalRequest(p);

	// after pin validation, 
	// waiting password could change,
	// and remaining tries too.
	// so we add an internal request here.
	QueryPIN(m_hWnd, WM_REQUEST, IR_QUERYPIN);

	return CA_OK;
}

//////////////////////////////////////////////////////////////////////////
// CC requests
CAERR CComAgent::Dial(HWND hWnd, UINT uMsg, short step, 
					  const CString& strNum, LONG lExt /*= 0*/)
{
	if (m_nSysState <= SYS_ATREADY)
	{
		return CA_NOTALLOWED;
	}
	else if (m_nSysState < SYS_NETREADY)
	{
		if (!CCall::IsEmergency(strNum))
			return CA_NONETWORK;
	}

	CreateRequestEx(CReqMakeCall, p, (m_nSysState>=SYS_NETREADY));
	
	_SPARAM(p, 0) = strNum;
	_NPARAM(p, 0) = 0;
	
	if (m_nSysState < SYS_NETREADY)
	{
		if (_T('+') == strNum[(_Size_T)0])
			_SPARAM(p, 0) = strNum.Mid(1);
	}
	else
	{
		_NPARAM(p, 0) = 1;
	}

	return HighRequest(p);
}

CAERR CComAgent::Answer(HWND hWnd, UINT uMsg, short step /*= 0*/, LONG lExt /*= 0*/)
{
	CreateCallRequest(CReqAnswerCall, p);
	return HighRequest(p);
}

/*
	AT+CHLD=...
	0		UDUB a waiting call or
			release all held calls
	1		terminate all active calls and 
			accept a waiting call or 
			activate a held call
	1x		terminate a call (x)
	2		hold all active calls and 
			accept a waiting call or
			activate a held call
	2x		hold all calls except x
	3		add a held call to the active calls
*/
CAERR CComAgent::ChangeCallLine(HWND hWnd, UINT uMsg, short step, 
						int code, int line /*= 0*/, LONG lExt /*= 0*/)
{
	if (m_nSysState < SYS_NETREADY )
	{
		return CA_NOTALLOWED;
	}

	CreateCallRequest(CReqHoldCall, p);

	_NPARAM(p, 0) = code;
	_NPARAM(p, 1) = line;

	return HighRequest(p);
}

// MAYBE "at+chld" can not be used before PIN
// thus "ATH" is needed to hang up emergency calls
CAERR CComAgent::HangUp(HWND hWnd, UINT uMsg, 
					short step /*= 0*/, LONG lExt /*= 0*/)
{
	if (m_nSysState < SYS_ATREADY )
	{
		return CA_NOTALLOWED;
	}
	
	CReqEndCall *p = new CReqEndCall;

	p->Init(hWnd, uMsg, step, lExt, (m_nSysState >= SYS_NETREADY));

	_NPARAM(p, 0) = (m_nSysState >= SYS_NETREADY);

	return HighRequest(p);
}

CAERR CComAgent::Deflect(HWND hWnd, UINT uMsg, short step, 
					const CString& strNum, LONG lExt /*= 0*/)
{
	CreateCallRequest(CReqDeflectCall, p);

	_SPARAM(p, 0) = strNum;

	return HighRequest(p);
}

CAERR CComAgent::SendDTMF(HWND hWnd, UINT uMsg, short step, 
					char chDTMF, LONG lExt /*= 0*/)
{
	if (m_nSysState < SYS_ATREADY)
		return CA_NOTALLOWED;

	CreateRequest(CReqSendDtmf, p);

	_NPARAM(p, 0) = chDTMF;
	
	return HighRequest(p);
}

CAERR CComAgent::SendDTMF(HWND hWnd, UINT uMsg, short step, 
					const CString& strDTMF, LONG lExt /*= 0*/)
{
	if (m_nSysState < SYS_ATREADY)
		return CA_NOTALLOWED;

	CreateRequest(CReqSendDtmf, p);

	_NPARAM(p, 0) = 0;
	_SPARAM(p, 0) = strDTMF;
	
	return HighRequest(p);
}

CAERR CComAgent::GetExtendErr(HWND hWnd, UINT uMsg, 
					short step /*= 0*/, LONG lExt /*= 0*/)
{
	if (m_nSysState < SYS_ATREADY)
		return CA_NOTALLOWED;

	CreateRequest(CReqExtErr, p);

	return HighRequest(p);
}
//////////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////////
// security setting
CAERR CComAgent::SetClirScheme(HWND hWnd, UINT uMsg, short step,
							   int nScheme, LONG lExt /*= 0*/)
{
	if (m_nSysState < SYS_NETREADY)
		return CA_NOTALLOWED;

	CreateRequest(CReqSetClir, p);

	_NPARAM(p, 0) = nScheme;
	
	return NormalRequest(p);
}

CAERR CComAgent::GetClirScheme(HWND hWnd, UINT uMsg, short step, LONG lExt /*= 0*/)
{
	if (m_nSysState < SYS_NETREADY)
		return CA_NOTALLOWED;

	CreateRequest(CReqGetClir, p);

	return NormalRequest(p);
}

CAERR CComAgent::ChangePassword(HWND hWnd, UINT uMsg, short step,
					int facility, const CString& oldpwd, 
					const CString& newpwd, LONG lExt /*= 0*/)
{
	if (m_nSysState < SYS_PINREADY)
		return CA_NOTALLOWED;

	CreateRequest(CReqChangePwd, p);

	_NPARAM(p, 0) = facility;
	_SPARAM(p, 0) = oldpwd;
	_SPARAM(p, 1) = newpwd;

	NormalRequest(p);
	
	if (FL_SC == facility)
		QueryPIN(m_hWnd, WM_REQUEST, IR_QUERYPIN);

	return CA_OK;
}

CAERR CComAgent::UnblockPIN(HWND hWnd, UINT uMsg, short step, 
				 int facility, LPCTSTR puk, LPCTSTR pin, LONG lExt)
{
	if (m_nSysState <= SYS_PINREADY)
		return CA_NOTALLOWED;
	
	CreateRequest(CReqUnblockPIN, p);
	
	_NPARAM(p, 0) = facility;
	_SPARAM(p, 0) = puk;
	_SPARAM(p, 1) = pin;
	
	NormalRequest(p);
	
	if (FL_SC == facility)
		QueryPIN(m_hWnd, WM_REQUEST, IR_QUERYPIN);
	
	return CA_OK;
}

CAERR CComAgent::GetSIMLock(HWND hWnd, UINT uMsg, short step, LONG lExt)
{
	if (m_nSysState < SYS_PINREADY)
		return CA_NOTALLOWED;

	CreateRequest(CReqGetSimLock, p);
	
	return NormalRequest(p);
}

CAERR CComAgent::SetSIMLock(HWND hWnd, UINT uMsg, short step, BOOL bLock, 
							const CString& pwd, LONG lExt)
{
	if (m_nSysState < SYS_PINREADY)
		return CA_NOTALLOWED;

	CreateRequest(CReqSetSimLock, p);

	_NPARAM(p, 0) = bLock;
	_SPARAM(p, 0) = pwd;
	
	NormalRequest(p);

	// wrong pwd may change system state
	QueryPIN(m_hWnd, WM_REQUEST, IR_QUERYPIN);

	return CA_OK;
}

CAERR CComAgent::GetCallBarring(HWND hWnd, UINT uMsg, short step,
					int facility, int nClass /*= -1*/, LONG lExt /*= 0*/)
{
	if (m_nSysState < SYS_PINREADY)
		return CA_NOTALLOWED;

	CreateRequest(CReqGetFacLock, p);

	_NPARAM(p, 0) = facility;
	_NPARAM(p, 1) = nClass;
	
	return NormalRequest(p);
}

CAERR CComAgent::SetCallBarring(HWND hWnd, UINT uMsg, short step,
					int facility, BOOL bLock, const CString& pwd, 
					int nClass /*= -1*/, LONG lExt /*= 0*/)
{
	if (m_nSysState < SYS_PINREADY)
		return CA_NOTALLOWED;

	CreateRequest(CReqSetFacLock, p);

	_NPARAM(p, 0) = facility;
	_SPARAM(p, 0) = pwd;
	_NPARAM(p, 1) = bLock;
	_NPARAM(p, 2) = nClass;

	return NormalRequest(p);
}

CAERR CComAgent::QueryPIN2(HWND hWnd, UINT uMsg, short step, LONG lExt /*= 0*/)
{
	if (m_nSysState < SYS_PINREADY)
		return CA_NOTALLOWED;

	CreateRequest(CReqQueryPin2, p);

	return NormalRequest(p);
}

CAERR CComAgent::PIN2Validate(HWND hWnd, UINT uMsg, short step,
					const CString& strPin2, const CString& newPin2 /*= CString()*/, 
					LONG lExt /*= 0*/)
{
	if (m_nSysState < SYS_PINREADY)
		return CA_NOTALLOWED;

	CreateRequest(CReqPin2Validate, p);

	_SPARAM(p, 0) = strPin2;
	_SPARAM(p, 1) = newPin2;

	return NormalRequest(p);
}

CAERR CComAgent::ReadCallForward(HWND hWnd, UINT uMsg, short step, 
								 int nReason, int nClass, LONG lExt)
{
	if (m_nSysState < SYS_NETREADY)
		return CA_NOTALLOWED;
	
	CreateRequest(CReqReadCF, p);

	_NPARAM(p, 0) = nReason;
	_NPARAM(p, 1) = nClass;
	
	return NormalRequest(p);	
}

CAERR CComAgent::WriteCallForward(HWND hWnd, UINT uMsg, short step, 
							const CFSTRUCT &cf_info, LONG lExt)
{
	if (m_nSysState < SYS_NETREADY)
		return CA_NOTALLOWED;
	
	CreateRequest(CReqWriteCF, p);

	_NPARAM(p, 0) = cf_info.reason;
	_NPARAM(p, 1) = MAKELONG(cf_info.status, cf_info.xclass);
	_NPARAM(p, 2) = cf_info.GetTime();
	_SPARAM(p, 0) = cf_info.GetNumber();
	
	return NormalRequest(p);	
}

CAERR CComAgent::ReadCallWaiting(HWND hWnd, UINT uMsg, short step, int nClass, LONG lExt)
{
	if (m_nSysState < SYS_NETREADY)
		return CA_NOTALLOWED;
	
	CreateRequest(CReqReadCW, p);

	_NPARAM(p, 0) = nClass;
	
	return NormalRequest(p);	
}

CAERR CComAgent::WriteCallWaiting(HWND hWnd, UINT uMsg, short step, 
								  BOOL bEnable, int nClass, LONG lExt)
{
	if (m_nSysState < SYS_NETREADY)
		return CA_NOTALLOWED;
	
	CreateRequest(CReqWriteCW, p);

	_NPARAM(p, 0) = bEnable ? 1 : 0;
	_NPARAM(p, 1) = nClass;
	
	return NormalRequest(p);	
}

#ifdef _CALL_COST_SUPPORT_
CAERR CComAgent::GetCallMeter(HWND hWnd, UINT uMsg, short step, LONG lExt)
{
	if (m_nSysState < SYS_PINREADY)
		return CA_NOTALLOWED;

	CreateRequest(CReqGetCallMeter, p);
	
	return NormalRequest(p);	
}

CAERR CComAgent::GetAcmValue(HWND hWnd, UINT uMsg, short step, LONG lExt)
{
	if (m_nSysState < SYS_PINREADY)
		return CA_NOTALLOWED;
	
	CreateRequest(CReqGetAcmValue, p);
	
	return NormalRequest(p);	
}

CAERR CComAgent::ResetAcmValue(HWND hWnd, UINT uMsg, short step,
							   const CString &strPwd, LONG lExt)
{
	if (m_nSysState < SYS_PINREADY)
		return CA_NOTALLOWED;
	
	CreateRequest(CReqResetAcmValue, p);

	_SPARAM(p, 0) = strPwd;
	
	return NormalRequest(p);
}

CAERR CComAgent::ReadAcmMax(HWND hWnd, UINT uMsg, short step, LONG lExt)
{
	if (m_nSysState < SYS_PINREADY)
		return CA_NOTALLOWED;
	
	CreateRequest(CReqReadAcmMax, p);
	
	return NormalRequest(p);	
}

CAERR CComAgent::WriteAcmMax(HWND hWnd, UINT uMsg, short step, 
							 int nMax, const CString &strPwd, LONG lExt)
{
	if (m_nSysState < SYS_PINREADY)
		return CA_NOTALLOWED;
	
	CreateRequest(CReqWriteAcmMax, p);

	_NPARAM(p, 0) = nMax;
	_SPARAM(p, 0) = strPwd;
	
	return NormalRequest(p);	
}

CAERR CComAgent::GetPucValue(HWND hWnd, UINT uMsg, short step, LONG lExt)
{
	if (m_nSysState < SYS_PINREADY)
		return CA_NOTALLOWED;
	
	CreateRequest(CReqGetPucValue, p);
	
	return NormalRequest(p);	
}

CAERR CComAgent::SetPucValue(HWND hWnd, UINT uMsg, short step, 
							 const CString &currency, const CString &ppu, 
							 const CString &strPwd, LONG lExt)
{
	if (m_nSysState < SYS_PINREADY)
		return CA_NOTALLOWED;
	
	CreateRequest(CReqSetPucValue, p);

	_SPARAM(p, 0) = currency;
	_SPARAM(p, 1) = ppu;
	_SPARAM(p, 2) = strPwd;
	
	return NormalRequest(p);
}
#endif	// _CALL_COST_SUPPORT_

CAERR CComAgent::QueryOperator(HWND hWnd, UINT uMsg, short step, int format, LONG lExt)
{
	if (m_nSysState < SYS_SIMOK)
		return CA_NOTALLOWED;

	CreateRequest(CReqGetOperator, p);

	_NPARAM(p, 0) = format;

	return HighRequest(p);
}

CAERR CComAgent::GetOperList(HWND hWnd, UINT uMsg, short step, LONG lExt)
{
	if (m_nSysState < SYS_ATREADY)
		return CA_NOTALLOWED;
	
	CreateRequest(CReqListOperator, p);
	
	return NormalRequest(p);
}

CAERR CComAgent::SelectOperator(
	HWND hWnd, UINT uMsg, short step, int mode, 
	const CString &oper, LONG lExt)
{
	if (m_nSysState < SYS_ATREADY)
		return CA_NOTALLOWED;
	
	CreateRequest(CReqSetOperator, p);

	_NPARAM(p, 0) = mode;
	_SPARAM(p, 0) = oper;
	
	return NormalRequest(p);
}

CAERR CComAgent::QueryGprsState(HWND hWnd, UINT uMsg, short step, LONG lExt)
{
	if (m_nSysState < SYS_ATREADY)
		return CA_NOTALLOWED;
	
	CreateRequest(CReqQueryGprsState, p);
	
	return NormalRequest(p);
}

CAERR CComAgent::QueryIMEI(HWND hWnd, UINT uMsg, short step, LONG lExt)
{
	if (m_nSysState < SYS_ATREADY)
		return CA_NOTALLOWED;

	CreateRequest(CReqGetIMEI, p);

	return NormalRequest(p);
}

CAERR CComAgent::QueryIMSI(HWND hWnd, UINT uMsg, short step /* = 0 */, LONG lExt /* = 0 */)
{
	if(m_nSysState < SYS_ATREADY)
		return CA_NOTALLOWED;

	CreateRequest(CReqGetIMSI, p);

	return NormalRequest(p);
}

CAERR CComAgent::GetVersionInfo(HWND hWnd, UINT uMsg, short step, LONG lExt)
{
	if (m_nSysState < SYS_ATREADY)
		return CA_NOTALLOWED;
	
	CreateRequest(CReqGetVersion, p);
	
	return NormalRequest(p);
}

/************ 20131109 by Zhuwei **********************************************/
CAERR CComAgent::GetVERNO(HWND hWnd, UINT uMsg, short step, LONG lExt)
{
	if (m_nSysState < SYS_ATREADY)
		return CA_NOTALLOWED;
	
	CreateRequest(CReqGetVERNO, p);
	
	return NormalRequest(p);
}
/******************************************************************************/

CAERR CComAgent::QuerySimOwner(HWND hWnd, UINT uMsg, short step, LONG lExt)
{
	if (m_nSysState < SYS_PINREADY)
		return CA_NOTALLOWED;

	CreateRequest(CReqGetSimOwner, p);
	
	return NormalRequest(p);
}

CAERR CComAgent::SetModemAPN(HWND hWnd, UINT uMsg, short step, 
							 const CString& strApn, LONG lExt)
{
	if (m_nSysState < SYS_NETREADY)
		return CA_NOTALLOWED;

	CreateRequest(CReqSetAPN, p);

	_SPARAM(p, 0) = strApn;
	
	return NormalRequest(p);
}

CAERR CComAgent::QueryPSStatus(HWND hWnd, UINT uMsg, short step, LONG lExt)
{
	//	20110218 by foil
    //if (m_nSysState < SYS_NETREADY)
	if (m_nSysState < SYS_ATREADY)
		return CA_NOTALLOWED;

	CreateRequest(CReqQueryRatPs, p);

	return NormalRequest(p);
}

//	20110130 by foil
CAERR CComAgent::SetNetworkType(HWND hWnd, UINT uMsg, short step, int option, LONG lExt)
{
	if (m_nSysState < SYS_PINREADY)
		return CA_NOTALLOWED;
	
	CreateRequest(CReqSetNetWorkType, p);

	_NPARAM(p, 0) = option;

	return NormalRequest(p);
}

/*
CAERR CComAgent::UpdateTime(HWND hWnd, UINT uMsg, short step, LONG lExt)
{
	if (m_nSysState < SYS_ATREADY)
		return CA_NOTALLOWED;
	
	CreateRequest(CReqUpdateTime, p);
	
	return NormalRequest(p);
}
*/

#ifdef _USSD_SUPPORT_
CAERR CComAgent::UssdControl(HWND hWnd, UINT uMsg, short step, int n,
							 const CString &strReq, LONG lExt)
{
	if (m_nSysState < SYS_NETREADY)
		return CA_NOTALLOWED;
	
	CreateRequest(CReqUssdCtrl, p);

	_NPARAM(p, 0) = n;
	_SPARAM(p, 0) = strReq;
	
	return NormalRequest(p);
}

#endif	// _USSD_SUPPORT_

//////////////////////////////////////////////////////////////////////////
CAERR CComAgent::SendSMS(HWND hWnd, UINT uMsg, short step, 
						 SMSMessage* pSms, LONG lExt)
{
	if (m_nSysState < SYS_NETREADY)
		return CA_NONETWORK;

	CreateRequest(CReqSendSms, p);

	_PPARAM(p, 0) = pSms;
	
	return NormalRequest(p);
}

CAERR CComAgent::ReadSMS(HWND hWnd, UINT uMsg, short step, 
						 SMSMessage* pSms, const CString& strMem, 
						 int nIdx, LONG lExt)
{
//	if (m_nSysState < SYS_PINREADY)
	if (m_nSysState < SYS_ATREADY)	// the urc could come before pin_ready
		return CA_NOTALLOWED;

	CreateRequest(CReqReadSms, p);

	_PPARAM(p, 0) = pSms;
	_NPARAM(p, 0) = nIdx;
	_SPARAM(p, 0) = strMem;
	
	return NormalRequest(p);
}

CAERR CComAgent::WriteSMS(HWND hWnd, UINT uMsg, short step, 
						  SMSMessage* pSms, const CString& strMem, 
						  LONG lExt)
{
	if (m_nSysState < SYS_ATREADY)
		return CA_NOTALLOWED;

	CreateRequest(CReqWriteSms, p);

	_PPARAM(p, 0) = pSms;
	_SPARAM(p, 0) = strMem;
	
	return NormalRequest(p);	
}

CAERR CComAgent::EraseSMS(HWND hWnd, UINT uMsg, short step, 
						  SMSMessage* pSms, LONG lExt)
{
	if (m_nSysState < SYS_ATREADY)
		return CA_NOTALLOWED;

	CreateRequest(CReqEraseSms, p);

	_PPARAM(p, 0) = pSms;
	
	return NormalRequest(p);
	
}

CAERR CComAgent::ReplaceSMS(HWND hWnd, UINT uMsg, short step, 
							SMSMessage* pSms, LONG lExt)
{
	if (m_nSysState < SYS_ATREADY)
		return CA_NOTALLOWED;

	CreateRequest(CReqReplaceSms, p);

	_PPARAM(p, 0) = pSms;
	
	return NormalRequest(p);	
}

CAERR CComAgent::SetSMSMem3(HWND hWnd, UINT uMsg, short step, 
						   const CString& strMem1, LONG lExt)
{
	if (m_nSysState < SYS_ATREADY)
		return CA_NOTALLOWED;

	CreateRequest(CReqSetSmsMem3, p);

	_SPARAM(p, 0) = strMem1;
	
	return NormalRequest(p);	
}

CAERR CComAgent::GetSMSMem3(HWND hWnd, UINT uMsg, short step, LONG lExt)
{
	if (m_nSysState < SYS_ATREADY)
		return CA_NOTALLOWED;

	CreateRequest(CReqGetSmsMem3, p);
	
	return NormalRequest(p);
}

CAERR CComAgent::ConfigSMS(HWND hWnd, UINT uMsg, short step, LONG lExt)
{
	if (m_nSysState < SYS_ATREADY)
		return CA_NOTALLOWED;
	
	CreateRequest(CReqConfigSms, p);
	
	return NormalRequest(p);
}

CAERR CComAgent::GetSMSNumber(HWND hWnd, UINT uMsg, short step, 
							  const CString& strMem, LONG lExt)
{
	if (m_nSysState < SYS_ATREADY)
		return CA_NOTALLOWED;

	CreateRequest(CReqCountSms, p);

	_SPARAM(p, 0) = strMem;
	
	return NormalRequest(p);
}

CAERR CComAgent::SetSMSC(HWND hWnd, UINT uMsg, short step, const CString &sc, LONG lExt /* = 0 */)
{
	if(m_nSysState < SYS_ATREADY)
		return CA_NOTALLOWED;
	
	CreateRequest(CReqSetSmsc, p);

	_SPARAM(p, 0) = sc;
	
	return NormalRequest(p);
}

#ifdef _CBS_SUPPORT_
CAERR CComAgent::GetCBSInfo(
	HWND hWnd, UINT uMsg, short step, BOOL bETWS, 
	const CString& channelID, LONG lExt /* = 0 */)
{
	if(m_nSysState < SYS_ATREADY)
		return CA_NOTALLOWED;

	CreateRequest(CReqGetCbsInfo, p);

	_NPARAM(p, 0) = bETWS ? 1: 0;
	_SPARAM(p, 0) = channelID;

	return NormalRequest(p);
}

CAERR CComAgent::SetCBMode(
	HWND hWnd, UINT uMsg, short step, BOOL bOn, 
	const CString& dcs, const CString& channel, 
	BOOL bAllLang, LONG lExt /* = 0 */)
{
	if(m_nSysState < SYS_ATREADY)
		return CA_NOTALLOWED;

	CreateRequest(CReqSetCbMode, p);

	_NPARAM(p, 0) = bOn ? 0 : 1;
	_NPARAM(p, 1) = bAllLang ? 1 : 0;
	_SPARAM(p, 0) = dcs;
	_SPARAM(p, 1) = channel;

	return NormalRequest(p);
}
#endif

#ifdef _ETWS_SUPPORT_
CAERR CComAgent::GetETWSInfo(HWND hWnd, UINT uMsg, short step, LONG lExt /* = 0 */)
{
	if(m_nSysState < SYS_ATREADY)
		return CA_NOTALLOWED;

	CreateRequest(CReqGetEtwsInfo, p);

	return NormalRequest(p);
}

CAERR CComAgent::SetETWSInfo(
	HWND hWnd, UINT uMsg, short step, BOOL bAbleEtws, 
	BOOL bSecurityCheck, BOOL bRecvTest, LONG lExt)
{
	if(m_nSysState < SYS_ATREADY)
		return CA_NOTALLOWED;

	CreateRequest(CReqSetEtwsInfo, p);

	_NPARAM(p, 0) = bAbleEtws;
	_NPARAM(p, 1) = bSecurityCheck;
	_NPARAM(p, 2) = bRecvTest;

	return NormalRequest(p);
}
#endif

CAERR CComAgent::ReadPBK(HWND hWnd, UINT uMsg, short step, 
						 PhoneBookItem *pPbk, const CString& strMem, 
						 int nIdx, LONG lExt)
{
	if (m_nSysState < SYS_ATREADY)
		return CA_NOTALLOWED;
	
	CreateRequest(CReqReadPbk, p);
	
	_PPARAM(p, 0) = pPbk;
	_NPARAM(p, 0) = nIdx;
	_SPARAM(p, 0) = strMem;
	
	return NormalRequest(p);	
}

CAERR CComAgent::WritePBK(HWND hWnd, UINT uMsg, short step, 
						  PhoneBookItem *pPbk, const CString& strMem, 
						  LONG lExt)
{
	if (m_nSysState < SYS_ATREADY)
		return CA_NOTALLOWED;
	
	CreateRequest(CReqWritePbk, p);
	
	_PPARAM(p, 0) = pPbk;
	_SPARAM(p, 0) = strMem;
	
	return NormalRequest(p);	
}

#ifdef _VCARD_SUPPORT_
CAERR CComAgent::ReadAdvPBK(HWND hWnd, UINT uMsg, short step, 
							PhoneBookItem *pPbk, LONG lExt)
{
	if (m_nSysState < SYS_ATREADY)
		return CA_NOTALLOWED;
	
	CreateRequest(CReqReadAdvPbk, p);
	
	_PPARAM(p, 0) = pPbk;
	
	return NormalRequest(p);	
}
#endif

CAERR CComAgent::ErasePBK(HWND hWnd, UINT uMsg, short step, 
						  PhoneBookItem *pPbk, LONG lExt)
{
	if (m_nSysState < SYS_ATREADY)
		return CA_NOTALLOWED;
	
	CreateRequest(CReqErasePbk, p);
	
	_PPARAM(p, 0) = pPbk;
	
	return NormalRequest(p);	
}

CAERR CComAgent::ReplacePBK(HWND hWnd, UINT uMsg, short step, 
							PhoneBookItem *pPbk, 
							const PhoneBookItem& newItem, LONG lExt)
{
	if (m_nSysState < SYS_ATREADY)
		return CA_NOTALLOWED;
	
	CreateRequest(CReqReplacePbk, p);
	
	_PPARAM(p, 0) = pPbk;
	_PPARAM(p, 1) = new PhoneBookItem(newItem);
	
	return NormalRequest(p);
}

CAERR CComAgent::GetPBKNumber(HWND hWnd, UINT uMsg, short step, 
							  const CString& strMem, LONG lExt)
{
	if (m_nSysState < SYS_ATREADY)
		return CA_NOTALLOWED;

	CreateRequest(CReqCountPbk, p);
	
	_SPARAM(p, 0) = strMem;

	return NormalRequest(p);
}

//////////////////////////////////////////////////////////////////////////

#ifdef _STK_SUPPORT_
//////////////////////////////////////////////////////////////////////////
// STK
CAERR CComAgent::STKResponse(HWND hWnd, UINT uMsg, short step, 
							 const CString &strCmd, LONG lExt)
{
	if (m_nSysState < SYS_ATREADY)
		return CA_NOTALLOWED;
	
	CreateRequest(CReqStkRespond, p);
	
	_SPARAM(p, 0) = strCmd;
	
	return NormalRequest(p);
}

CAERR CComAgent::STKEnvelope(HWND hWnd, UINT uMsg, short step, 
							 const CString &strCmd, LONG lExt)
{
	if (m_nSysState < SYS_ATREADY)
		return CA_NOTALLOWED;
	
	CreateRequest(CReqStkEnvelope, p);

	_SPARAM(p, 0) = strCmd;
	
	return NormalRequest(p);
}

CAERR CComAgent::STKSetupCall(HWND hWnd, UINT uMsg, short step, 
							  int option, LONG lExt)
{
	if (m_nSysState < SYS_ATREADY)
		return CA_NOTALLOWED;
	
	CReqStkCall *p = new CReqStkCall;

	p->Init(hWnd, uMsg, step, lExt, (0 == option||4 == option));

	_NPARAM(p, 0) = option;
	
	return NormalRequest(p);
}

CAERR CComAgent::STKSetupSS(HWND hWnd, UINT uMsg, short step, 
							int option, LONG lExt)
{
	if (m_nSysState < SYS_ATREADY)
		return CA_NOTALLOWED;
	
	CreateRequest(CReqStkSS, p);
	
	_NPARAM(p, 0) = option;
	
	return NormalRequest(p);
}

CAERR CComAgent::STKSetupUSSD(HWND hWnd, UINT uMsg, short step, 
							  int option, LONG lExt)
{
	if (m_nSysState < SYS_ATREADY)
		return CA_NOTALLOWED;
	
	CreateRequest(CReqStkUSSD, p);
	
	_NPARAM(p, 0) = option;
	
	return NormalRequest(p);
}

CAERR CComAgent::STKSendSMS(HWND hWnd, UINT uMsg, short step, 
							int option, LONG lExt)
{
	if (m_nSysState < SYS_ATREADY)
		return CA_NOTALLOWED;
	
	CreateRequest(CReqStkSMS, p);
	
	_NPARAM(p, 0) = option;
	
	return NormalRequest(p);
}

CAERR CComAgent::STKSendDTMF(HWND hWnd, UINT uMsg, short step, 
							 int option, LONG lExt)
{
	if (m_nSysState < SYS_ATREADY)
		return CA_NOTALLOWED;
	
	CreateRequest(CReqStkDTMF, p);
	
	_NPARAM(p, 0) = option;
	
	return NormalRequest(p);
}

CAERR CComAgent::STKRunAtCmd(HWND hWnd, UINT uMsg, short step, 
							 int option, LONG lExt)
{
	if (m_nSysState < SYS_ATREADY)
		return CA_NOTALLOWED;
	
	CreateRequest(CReqStkATCmd, p);
	
	_NPARAM(p, 0) = option;
	
	return NormalRequest(p);
}

CAERR CComAgent::GetSIMImage(HWND hWnd, UINT uMsg, short step, 
							 int imgId, LONG lExt)
{
#ifndef _STK_SIMULATE_
	if (m_nSysState < SYS_ATREADY)
		return CA_NOTALLOWED;
#endif

	CreateRequest(CReqGetSimImage, p);
	
	_NPARAM(p, 0) = imgId;
	
	return NormalRequest(p);
}
//////////////////////////////////////////////////////////////////////////
#endif	// _STK_SUPPORT_

//////////////////////////////////////////////////////////////////////////
// power-off procedure
CAERR CComAgent::Close(
	HWND hWnd, UINT uMsg, short step, 
	UINT (*last_req_cb)(void), LONG lExt)
{
	Reset();
	DeregisterURC();
	StopPolling(PL_CONN);
	StopPolling(PL_CMUXINIT);

	if(last_req_cb)
	{
		last_req_cb();
	}

	CreateRequest(CReqInitBasic, p);	// not a real request

	CAERR err = PowerOff(m_hWnd, WM_REQUEST, IR_POWEROFF, (LONG)p);

	if (err != CA_OK)
	{
		delete p;
		return err;
	}

	// to reject future requests
	m_nSysState = SYS_CMUX;

	return CA_OK;
}

CAERR CComAgent::Suspend(
	HWND hWnd, UINT uMsg, short step, 
	UINT (*last_req_cb)(void), LONG lExt)
{
	Reset();
//	DeregisterURC();
	StopPolling(PL_CONN);
	StopPolling(PL_CMUXINIT);
	
	if(last_req_cb)
	{
		last_req_cb();
	}
	
	CAERR err = PowerOff(hWnd, uMsg, step, lExt);
	
	if (err != CA_OK)
	{
		return err;
	}
	
	// to reject future requests
	m_nSysState = SYS_CMUX;
	
	return CA_OK;
}

