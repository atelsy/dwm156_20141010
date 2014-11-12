/*
 *  request.cpp
 *  DCT
 *
 *  Created by MBJ on 12-7-8.
 *  Copyright 2012 MediaTek Inc. All rights reserved.
 *
 */

#include "request.h"


#define _NPARAM(i)	_param.nParam[i]
#define _SPARAM(i)	_param.sParam[i]
#define _PPARAM(i)	_param.pParam[i]


BOOL CBaseReq::Respond(int err, CResponse *p) const
{
//	p->m_lExt = lExt;
	ASSERT(p->m_lExt == lExt);

	p->retain();

	if (!::PostMessage(hWnd, uMsg, MAKEWPARAM(step, err), (LPARAM)p))
	{
		p->release();
		return FALSE;
	}

	return TRUE;
}

const char* CBaseReq::GetFacility(int fac)
{
	static const char * szFac[FL_MAX] =
	{
		("SC"),
		("P2"),
		("AO"),
		("OI"),
		("OX"),
		("AI"),
		("IR"),
		("AB"),
		("AG"),
		("AC"),
		("PN"),
		("PU"),
		("PP"),
		("PC")
	};
	
	if (fac >= 0 && fac < FL_MAX)
		return szFac[fac];
	else
		return NULL;
}

int CBaseReq::GetPwdType(const string& code)
{
	if (code == ("READY"))
		return PWD_READY;
	else if (code == ("SIM PIN"))
		return PWD_PIN;
	else if (code == ("SIM PUK"))
		return PWD_PUK;
	else if (code == ("SIM PIN2"))
		return PWD_PIN2;
	else if (code == ("SIM PUK2"))
		return PWD_PUK2;
	else		// ignore other password		
		return -1;
}

int CBaseReq::GetPwdRemain(Connection *conn, int pwdType)
{
	int count = 0, err = ER_OK;
	
	switch (pwdType)
	{
		case PWD_PIN:
			err = conn->QueryPwdCount(count, 1);
			break;
		case PWD_PUK:
			err = conn->QueryPwdCount(count, 3);
			break;
		case PWD_PIN2:
			err = conn->QueryPwdCount(count, 2);
			break;
		case PWD_PUK2:
			err = conn->QueryPwdCount(count, 4);
			break;
		default:
			break;
	}
	return (ER_OK == err) ? count : -1;
}

int CBaseReq::GetPwdState(Connection *conn, DualWord &pinStat, int fac)
{
	int err = -1;
	int type = 0;
	int count = 0;
	
	string code;
	
	if (fac == FL_SC)
		err = conn->QueryPINCode(code);
	else if (fac == FL_P2)
		err = conn->QueryPIN2(code);
	else
		return 0;
	
	if (ER_OK == err)
	{
		type = GetPwdType(code);
		
		if (PWD_READY == type)
		{
			type = (FL_SC == fac) ? PWD_PIN : PWD_PIN2;
		}
		
		count = GetPwdRemain(conn, type);
	}
	
	pinStat.alpha = (WORD)type;
	pinStat.beta  = (WORD)count;
	
	return err;
}

int CBaseReq::ListCalls(Connection *conn, CCallList& calls)
{
	CCall unit;
	std::vector<CALL> vCalls;
	int err = conn->ListCurrentCall(vCalls);
	
	for (int i=0; i<vCalls.size(); i++)
	{
		unit = vCalls[i];
		calls.Append(unit);
	}
	return err;
}

int CBaseReq::WaitNewCall(
						  Connection *conn, 
						  const CCallList &oldCalls, 
						  CCallList &newCalls)
{
	int err = ER_OK;
	CCallList target;
	
	for (int i=0; i<10; i++)
	{
		Sleep(700 + i*50);
		
		target.Reset();
		newCalls.Reset();
		err = ListCalls(conn, newCalls);
		
		if (ER_OK != err)
			break;
		if (oldCalls.GetNewCalls(target, newCalls))
			break;
	}
	
	return err;
}

///////////////////////////

int CReqPowerOn::Execute(Connection *conn, CResponse*)
{
	int err = conn->SetSleepMode(false);
	
	if (ER_OK == err)
		err = conn->SetDCEEcho(false);
	if (ER_OK == err)
		err = conn->SetErrorFormat(1);	// AT+CMEE=1
	if (ER_OK == err)
		err = conn->PowerOn(_NPARAM(0));
	
	return err;
}

int CReqPowerOff::Execute(Connection *conn, CResponse*)
{
	int err = ER_OK;
	
	if (_NPARAM(0) >= 0)
	{
		err = conn->TerminateCall();		// ATH
		
		if (ER_TIMEOUT != err/* && ER_USERABORT != err*/)
			err = conn->PowerOff(_NPARAM(0));
	}

	conn->Conn_DeInit();

	return err;
}

int CReqInitBasic::Execute(Connection *conn, CResponse*)
{
	int err = conn->SetIndicationControl(0xff);	// at+eind=255
	
	if (ER_OK == err)
		err = conn->SetNetworkRegInd(2, 2);	// AT+CREG=2;+CGREG=2
	
	if (ER_OK == err)
		err = conn->SetSignalIndication(1);		// AT+ECSQ=1
	
        // if (ER_OK == err)
        
        //   err = conn->SetUccToUserName(2);   //AT+COPS=3,2
            

	if (ER_OK == err)
#ifndef _DEBUG
		err = conn->SetPSBearer(1);
#else
		/*err =*/ conn->SetPSBearer(1);
#endif
	
	if (ER_OK == err)
		err = conn->TerminateCall();		// ATH
	
	if (ER_OK == err)
	{
		err = conn->InternalSupport0x81Charset();	// AT+CSCS=0x81
		err = conn->SetCharacterSet("UCS2");	// AT+CSCS=UCS2
	}

    
    
// 	if (ER_OK == err)
// 		err = conn->SetTargetTime(CTime::GetCurrentTime());
	  return err;
}

int CReqInitAdv::Execute(Connection *conn, CResponse*)
{
	int err = ER_OK;
/*
	err = conn->SetNewMsgInd(2, 1, 0, 1, 1);	// AT+CNMI=2,1,0,1,1
	
	if (ER_OK == err)
		err = conn->SetMessageFormat(0);
	if (ER_OK == err)
		err = conn->SetCallWaiting(true);	// AT+CCWA=1
	if (ER_OK == err)
		err = conn->SetCLIP(1);		// AT+CLIP=1
	if (ER_OK == err)
		err = conn->SetCOLP(1);	// AT+COLP=1
*/
	err = conn->SetCallWaiting(true);	// AT+CCWA=1
	
	if (ER_OK == err)
		err = conn->SetSSNotification(1, 1);		// AT+CSSN=1,1
	
	return err;
}

int CReqInitNet::Execute(Connection *conn, CResponse*)
{
	int err = conn->USSDSessionControl(1);	// AT+CUSD=1
	
	if (ER_OK == err)
		err = conn->SetCallMeterValue(true);		// AT+CAOC=2
	if (ER_OK == err)
		err = conn->SetCallMeterEvent(true);		// AT+CCWE=1

//	if (ER_OK == err)
//		err = conn->USSDSessionControl(2);	// AT+CUSD=2
	
	return err;
}

//	AT+CPIN?
//	AT+EPINC?
int CReqQueryPin::Execute(Connection *conn, CResponse *p)
{
	DualWord &val = 
		static_cast<MyType*>(p)->Data();
	
	int pin = 0;
	int count = 0;
	int err = ER_OK;
	
	if (_NPARAM(0) != 0)
	{
		pin = _NPARAM(0);
		err = ER_OK;
	}
	else
	{
		string strPin;
		err = conn->QueryPINCode(strPin); //	AT+CPIN?
		pin = GetPwdType(strPin);
	}
	
	switch (err)
	{
		case ER_NOSIM:
			pin = SYS_NOSIM;
			err = ER_OK;
			break;
			
		case ER_SIMFAIL:	// error 13
		case ER_SIMWRONG:	// error 15
		case ER_SIMBLOCK:	// error 262
			pin = SYS_BADSIM;
			err = ER_OK;
			break;
			
		case ER_OK:
			count = GetPwdRemain(conn, pin);
			break;
			
		default:
			break;
	}
	
	if (count < 0 && ER_OK == err)
		err = ER_UNKNOWN;
	
	val.alpha = (WORD)pin;
	val.beta  = (WORD)count;
	
	return err;
}

//	AT+CPIN=<pin>
int CReqPinValidate::Execute(Connection *conn, CResponse *p)
{
	DualWord &val =
		static_cast<MyType*>(p)->Data();

	int err = conn->ValidationPIN(_SPARAM(0), _SPARAM(1));
	
	if (ER_OK != err &&
		ER_TIMEOUT != err )
	{
		string code;
		
		if (ER_OK == conn->QueryPINCode(code))
		{
			val.alpha = (WORD)GetPwdType(code);
			val.beta  = (WORD)GetPwdRemain(conn, val.alpha);
		}
	}
	
	return err;
}

//	AT+ESIMS?
int CReqQuerySim::Execute(Connection *conn, CResponse *p)
{
	// error 10/13/15/262 should be converted to sim statuses

	int &val = static_cast<MyType*>(p)->Data();
	
	int err = conn->QuerySIMStatus(val);
	
	switch (err)
	{
		case ER_OK:
			if (val != 0)
				val = SYS_SIMOK;
			else
				val = SYS_NOSIM;
			break;
			
		case ER_NOSIM:		// error 10
			val = SYS_NOSIM;
			break;
			
		case ER_SIMFAIL:	// error 13
		case ER_SIMWRONG:	// error 15
		case ER_SIMBLOCK:	// error 262
			val = SYS_BADSIM;
			break;
			
		default:
			val = 0;
			break;
	}
	
	return err;
}

//	AT+EIND?
//	AT+CREG?
int CReqQueryInd::Execute(Connection *conn, CResponse *p)
{
	DualWord &val =
		static_cast<MyType*>(p)->Data();

	int stat, eind = -1;
	
	int err = conn->GetNetworkStatus(stat);
	
	if (ER_OK == err)
	{
		err = conn->GetIndicationControl(eind);
	}
	
	val.alpha = (WORD)eind;
	val.beta  = (WORD)stat;
	
	return err;
}

//	AT+CLCC
int CReqQueryCall::Execute(Connection *conn, CResponse *p)
{
	CCallList &val =
		static_cast<MyType*>(p)->Data();
	
	return ListCalls(conn, val);
}

//	ATD<10086>;
int CReqMakeCall::Execute(Connection *conn, CResponse *p)
{
	CCallList &val =
		static_cast<MyType*>(p)->Data();
	
	CCallList calls;

	int err = ER_OK;
	
	if (_NPARAM(0))
	{
		err = ListCalls(conn, calls);
	
		if (ER_OK != err)
			return err;
	}
	
	err = conn->MakeMOCall(_SPARAM(0));
	
	if (_NPARAM(0))
	{
		// to make sure the new call is in the list
		if (ER_OK == err)
		{
			err = WaitNewCall(conn, calls, val);
		}
		else
		{
			err = ListCalls(conn, val);
		}
	}
	else
	{
		Sleep(2500);
	}
	
	return err;
}

//	ATH
int CReqEndCall::Execute(Connection *conn, CResponse *p)
{
	CCallList &val =
		static_cast<MyType*>(p)->Data();
	
	int err = conn->TerminateCall();		// ATH
	
	if (_NPARAM(0))
	{
		err = ListCalls(conn, val);
	}
	
	return err;
}

//	AT+CHLD=...
int CReqHoldCall::Execute(Connection *conn, CResponse *p)
{
	CCallList &val =
		static_cast<MyType*>(p)->Data();

	int err = conn->ChangeCallLine(_NPARAM(0), _NPARAM(1));	// AT+CHLD
	
	if (ER_OK == err || ER_TIMEOUT == err)
	{
		err = ListCalls(conn, val);
	}
	
	return err;
}

//	ATA
int CReqAnswerCall::Execute(Connection *conn, CResponse *p)
{
	CCallList &val =
		static_cast<MyType*>(p)->Data();
	
	int err = conn->AnswerMTCall();

//	if (ER_OK == err || ER_TIMEOUT == err)
	{
		err = ListCalls(conn, val);
	}
	
	return err;
}

//	AT+CTFR
int CReqDeflectCall::Execute(Connection *conn, CResponse *p)
{
	CCallList &val =
		static_cast<MyType*>(p)->Data();
	
	int err = conn->DeflectCall(_SPARAM(0));
	
//	if (ER_OK == err || ER_TIMEOUT == err)
	{
		err = ListCalls(conn, val);
	}
	
	return err;
}

//	AT+VTS=...
int CReqSendDtmf::Execute(Connection *conn, CResponse*)
{
	int err = ER_OK;
	
	if (_NPARAM(0) != 0)
	{
		err = conn->TransferDTMFTone((char)_NPARAM(0));
	}
	else
	{
		err = ER_OK;
		const CString &str = _SPARAM(0);
		
		for (_Size_T i=0; i<str.GetLength() && ER_OK == err; i++)
		{
			err = conn->TransferDTMFTone((char)str[i]);
		}
	}
	return err;
}

//	AT+CEER
int CReqExtErr::Execute(Connection *conn, CResponse *p)
{
	ERROREX &val =
		static_cast<MyType*>(p)->Data();
	
	val.cause = -1;
	
	return conn->ExtendErrReport(val.cause, val.report);
}

//	AT+CLIR?
int CReqGetClir::Execute(Connection *conn, CResponse *p)
{
	int &val = static_cast<MyType*>(p)->Data();
	
	int n = -1, m = -1;
	
	int err = conn->GetCLIR(n, m);
	
	if (ER_OK == err)
		val = MAKELONG(n, m);
	
	return err;
}

//	AT+CLIR=<n>
int CReqSetClir::Execute(Connection *conn, CResponse *p)
{
	int &val = static_cast<MyType*>(p)->Data();
	
	int n = _NPARAM(0);
	
	int err = conn->SetCLIR(n);
	
	if (ER_OK == err)
		val = n;
	
	return err;	
}

//	AT+EPIN2?
int CReqQueryPin2::Execute(Connection *conn, CResponse *p)
{
	DualWord &val =
		static_cast<MyType*>(p)->Data();
	
	string code;
	
	int err = conn->QueryPIN2(code);
	
	if (ER_OK == err)
	{
		int type = GetPwdType(code);
		int count = GetPwdRemain(conn, type);
		
		if (count < 0)
			err = ER_UNKNOWN;
		
		val.alpha = (WORD)type;
		val.beta  = (WORD)count;
	}
	
	return err;
}

//	AT+EPIN2=...
int CReqPin2Validate::Execute(Connection *conn, CResponse *p)
{
	DualWord &val =
		static_cast<MyType*>(p)->Data();
	
	int err = conn->ValidatePIN2(_SPARAM(0), _SPARAM(1));
	
	if (ER_OK != err)
	{
		/*err =*/ GetPwdState(conn, val, FL_P2);
	}
	
	return err;
}

int CReqGetSimLock::Execute(Connection *conn, CResponse *p)
{
	BOOL &val = static_cast<MyType*>(p)->Data();
	return conn->GetSIMLock(val);
}

int CReqSetSimLock::Execute(Connection *conn, CResponse *p)
{
	DualWord &val =
		static_cast<MyType*>(p)->Data();
	
	int err = conn->SetSIMLock(_NPARAM(0), _SPARAM(0));
	
	if (ER_OK != err)
	{
		/*err =*/ GetPwdState(conn, val, FL_SC);
	}
	
	return err;
}

//	AT+CLCK=...
int CReqGetFacLock::Execute(Connection *conn, CResponse *p)
{
	int &val = static_cast<MyType*>(p)->Data();
	
	const char *fac = GetFacility(_NPARAM(0));
	
	return conn->GetCallBarring(val, fac, _NPARAM(1));
}

//	AT+CLCK=...
int CReqSetFacLock::Execute(Connection *conn, CResponse*)
{
	return conn->SetCallBarring(
		GetFacility(_NPARAM(0)), 
		_NPARAM(1), _SPARAM(0), _NPARAM(2));
}

//	AT+CPWD=...
int CReqChangePwd::Execute(Connection *conn, CResponse *p)
{
	DualWord &val =
		static_cast<MyType*>(p)->Data();
	
	int type = _NPARAM(0);
	
	CString fac = GetFacility(type);
	
	int err = conn->ChangePassword(fac, _SPARAM(0), _SPARAM(1));
	
	if (ER_OK != err)
	{
		/*err =*/ GetPwdState(conn, val, type);
	}
	
	return err;
}

//	AT+EPIN1/2=...
int CReqUnblockPIN::Execute(Connection *conn, CResponse *p)
{
	DualWord &val =
		static_cast<MyType*>(p)->Data();
	
	int type = _NPARAM(0);
	
	const char *fac = (type==FL_SC) ? "SC" : "P2";
	
	int err = conn->UnblockPIN(fac, _SPARAM(0), _SPARAM(1));
	
	if (ER_OK != err)
	{
		/*err =*/ GetPwdState(conn, val, type);
	}
	
	return err;
}

//	AT+CCFC = <reason>, 2
int CReqReadCF::Execute(Connection *conn, CResponse *p)
{
	CFSTRUCT &val =
		static_cast<MyType*>(p)->Data();

	val.reason = _NPARAM(0);
	val.xclass = _NPARAM(1);
	
	return conn->GetCallForward(val);	
}

//	AT+CCFC=...
int CReqWriteCF::Execute(Connection *conn, CResponse*)
{
	CFSTRUCT cf_info;
	
	const int lTmp = _NPARAM(1);
	
	cf_info.status = (BYTE)LOWORD(lTmp);
	cf_info.xclass = (BYTE)HIWORD(lTmp);
	
	cf_info.reason = _NPARAM(0);
	cf_info.SetTime(_NPARAM(2));
	cf_info.SetNumber(_SPARAM(0));
	
	return conn->SetCallForward(cf_info);
}

int CReqReadCW::Execute(Connection *conn, CResponse *p)
{
	int &val = static_cast<MyType*>(p)->Data();
	return conn->GetCallWaiting(val, _NPARAM(0));
}

int CReqWriteCW::Execute(Connection *conn, CResponse*)
{
	return conn->SetCallWaiting(_NPARAM(0), _NPARAM(1));
}

#ifdef _CALL_COST_SUPPORT_
int CReqGetCallMeter::Execute(Connection *conn, CResponse *p)
{
	int &val = static_cast<MyType*>(p)->Data();
	return conn->GetCallMeterValue(val);
}

int CReqGetAcmValue::Execute(Connection *conn, CResponse *p)
{
	int &val = static_cast<MyType*>(p)->Data();
	return conn->GetACMValue(val);
}

int CReqResetAcmValue::Execute(Connection *conn, CResponse*)
{
	return conn->ResetACMValue(_SPARAM(0));
}

int CReqReadAcmMax::Execute(Connection *conn, CResponse *p)
{
	int &val = static_cast<MyType*>(p)->Data();
	return conn->GetMaxACMValue(val);
}

int CReqWriteAcmMax::Execute(Connection *conn, CResponse *p)
{
	return conn->SetMaxACMValue(_NPARAM(0), _SPARAM(0));
}

int CReqGetPucValue::Execute(Connection *conn, CResponse *p)
{
	std::vector<CString> &val =
		static_cast<MyType*>(p)->Data();
	
	CString currency, ppu;
	
	int err = conn->GetPUCValue(currency, ppu);
	
	val.push_back(currency);
	val.push_back(ppu);
	
	return err;
}

int CReqSetPucValue::Execute(Connection *conn, CResponse*)
{
	return conn->SetPUCValue(
		_SPARAM(0), _SPARAM(1), _SPARAM(2));
}
#endif	// _CALL_COST_SUPPORT_

//	AT+COPS?
int CReqGetOperator::Execute(Connection *conn, CResponse *p)
{
	OPERINFO &val = static_cast<MyType*>(p)->Data();
	return conn->GetSelectedOperator(0, val.mode, val.operId);
}

int CReqListOperator::Execute(Connection *conn, CResponse *p)
{
	std::vector<OPERINFOEX> &val =
		static_cast<MyType*>(p)->Data();

	return conn->GetOperatorList(val);
}

int CReqSetOperator::Execute(Connection *conn, CResponse*)
{
	return conn->SelectOperator(
		_NPARAM(0), _SPARAM(0), (int)lExt);
}

int CReqQueryGprsState::Execute(Connection *conn, CResponse *p)
{
	int &val = static_cast<MyType*>(p)->Data();
	return conn->GetGPRSStatus(val);
}

//	AT+CGMR
int CReqGetVersion::Execute(Connection *conn, CResponse *p)
{
	CString &val = static_cast<MyType*>(p)->Data();
	return conn->GetVersionInfo(val);
}

/************ 20131109 by Zhuwei **********************************************/

int CReqGetVERNO::Execute(Connection *conn, CResponse *p)
{
	CString &val = static_cast<MyType*>(p)->Data();
	return conn->GetVERNO(val);
}

/******************************************************************************/

//	AT+CGSN
int CReqGetIMEI::Execute(Connection *conn, CResponse *p)
{
	CString &val = static_cast<MyType*>(p)->Data();
	return conn->QueryIMEI(val);
}

int CReqGetIMSI::Execute(Connection *conn, CResponse *p)
{
	CString &val = static_cast<MyType*>(p)->Data();
	return conn->QueryIMSI(val);
}

//	AT+CIMI, AT+EOPN
int CReqGetSimOwner::Execute(Connection *conn, CResponse *p)
{
	CString &val = static_cast<MyType*>(p)->Data();
	return conn->GetOperator(val);
}

//	AT+CGDCONT
int CReqSetAPN::Execute(Connection *conn, CResponse*)
{
	return conn->SetModemApn(_SPARAM(0));
}

// AT+ERAT?
int CReqQueryRatPs::Execute(Connection *conn, CResponse *p)
{
	//	20110130 by foil
	//	CRespInt *p = static_cast<MyType*>(p)->Data();
	//	ON_ABORT(abort, p);
	
	//	int rat, ps;
	//	err = conn->QueryRATnPS(rat, ps);
	//	val = MAKELONG(rat, ps);
	
	std::vector<int> &val =
		static_cast<MyType*>(p)->Data();
	
	int curRat, gprsStat, ratMode, prefRat;
	
	int err = conn->QueryRATnPS(
		curRat, gprsStat, ratMode, prefRat);
	
	val.push_back(curRat);
	val.push_back(gprsStat);
	val.push_back(ratMode);
	val.push_back(prefRat);
	
	return err;
}

/*
int CReqUpdateTime::Execute(Connection *conn, CResponse*)
{
	return conn->SetTargetTime(CTime::GetCurrentTime());
}
*/

#ifdef _USSD_SUPPORT_
int CReqUssdCtrl::Execute(Connection *conn, CResponse* p)
{
	int err = ER_OK;
    
	const CString &strSec = _SPARAM(0);
	
	if (strSec.IsEmpty())
		err = conn->USSDSessionControl(_NPARAM(0));
	else
		err = conn->USSDSessionControl(_NPARAM(0), strSec);
	
	return err;
}

#endif	// _USSD_SUPPORT_
//////////////////////////////////////////////////////////////////////////
/*
//	AT+CLCC
CResponse * CConnEx::query_calls(int& err)
{
	m_pErr = &err;
	return query_calls(false);
}

//	AT
int CConnEx::conn_test(int n)
{
	// to try 'n' times if timeout
	
	for (int i=0; i<n; i++)
	{
		if (conn->Query() == ER_OK)
			return ER_OK;
	}
	
	return ER_TIMEOUT;
}
*/

//////////////////////////////////////////////////////////////////////////

int CReqSendSms::Execute(Connection *conn, CResponse *p)
{
	BOOL &val = static_cast<MyType*>(p)->Data();
	
	SMSMessage *pSms = (SMSMessage*)(_PPARAM(0));
	
	ASSERT (NULL != p);
	ASSERT (NULL != pSms);
	
	int err = ER_OK;
	
	val = pSms->send(conn, err);
	
	return err;
}

int CReqReadSms::Execute(Connection *conn, CResponse *p)
{
	BOOL &val = static_cast<MyType*>(p)->Data();
	
	SMSMessage *pSms = (SMSMessage*)(_PPARAM(0));
	
	ASSERT (NULL != p);
	ASSERT (NULL != pSms);
	
	int err = ER_OK;
	
	val = pSms->read(
			conn, 
			_SPARAM(0),	// memory type
			_NPARAM(0),	// index
			err);
	
	return err;
}

int CReqWriteSms::Execute(Connection *conn, CResponse *p)
{
	BOOL &val = static_cast<MyType*>(p)->Data();
	
	SMSMessage *pSms = (SMSMessage*)(_PPARAM(0));
	
	ASSERT (NULL != p);
	ASSERT (NULL != pSms);

	int err = ER_OK;
	
	val = pSms->write(conn, _SPARAM(0),	err);
	
	return err;
}

int CReqEraseSms::Execute(Connection *conn, CResponse *p)
{
	BOOL &val = static_cast<MyType*>(p)->Data();
	
	SMSMessage *pSms = (SMSMessage*)(_PPARAM(0));
	
	ASSERT (NULL != p);
	ASSERT (NULL != pSms);
	
	int err = ER_OK;
	
	val = pSms->EraseMsg(conn, err);
	
	return err;
}

int CReqReplaceSms::Execute(Connection *conn, CResponse *p)
{
	BOOL &val = static_cast<MyType*>(p)->Data();
	
	SMSMessage *pSms = (SMSMessage*)(_PPARAM(0));
	
	ASSERT (NULL != p);
	ASSERT (NULL != pSms);

	int err = ER_OK;
	
	val = pSms->ReplaceMsg(conn, err);
	
	return err;
}

int CReqSetSmsMem3::Execute(Connection *conn, CResponse*)
{
	return conn->SetCurrentSMSMemType(_T(""), _SPARAM(0));
}

int CReqGetSmsMem3::Execute(Connection *conn, CResponse *p)
{
	CString &val = static_cast<MyType*>(p)->Data();
	return conn->GetCurrentSMSMemType(3, val);
}

int CReqConfigSms::Execute(Connection *conn, CResponse*)
{
	int err = conn->SetNewMsgInd(2, 1, 2, 1, 1);	// AT+CNMI=2,1,2,1,1
	
	if (ER_OK == err)
		err = conn->SetMessageFormat(0);
	
	return err;
}

int CReqCountSms::Execute(Connection *conn, CResponse *p)
{
	std::vector<int> &val = static_cast<MyType*>(p)->Data();
	return conn->GetSMSMemStatus(_SPARAM(0), val);
}

int CReqSetSmsStat::Execute(Connection *conn, CResponse*)
{
	return conn->SetSMSStatus(_NPARAM(0));
}

int CReqSetSmsc::Execute(Connection *conn, CResponse*)
{
	return conn->SetSCAddress(_SPARAM(0));
}

#ifdef _CBS_SUPPORT_

int CReqGetCbsInfo::SetETWSChannel(Connection *conn, const CBSInfo &info)
{
	BOOL bOn = info.bOn;
	BOOL bAllLanguage = info.bAllLang;
	
	CString channelID, languages;
	int size = info.vec_channelID.size();
	BOOL bNew = FALSE;
	
	for(int i = 0; i < size; i++)
	{
		channelID += info.vec_channelID[i];
		channelID += _T(",");
	}
	
	if(channelID.GetLength() == 1)
	{
		channelID.Empty();
	}
	
	for(int k = 0; k < info.vec_languages.size() - 1; k++)
	{
		languages += info.vec_languages[k];
		languages += _T(",");
	}
	
	languages += info.vec_languages[info.vec_languages.size() - 1];
	
	if(info.bETWS && info.etws_channel.GetLength() > 0)
	{
		if(channelID.Find(info.etws_channel) == -1)
		{
			channelID += info.etws_channel;
			bNew = TRUE;
		}
		else
		{
			if (!channelID.IsEmpty())
			{
				channelID = channelID.Left(channelID.GetLength() - 1);
			}
		}
		
		if(!bOn || (bOn && bNew))
		{
		//	DebugOutA("\nSet CB Channel = ");
		//	DebugOut(channelID);
		//	DebugOutA("\n");
			return conn->SetCBMode(FALSE, TRUE, languages, channelID);
		}
	}
	
	return ER_OK;
}

int CReqGetCbsInfo::Execute(Connection *conn, CResponse *p)
{
	CBSInfo &val = static_cast<MyType*>(p)->Data();
	
	val.bETWS = (_NPARAM(0) != 0);
	val.etws_channel = _SPARAM(0);
	
	int err = conn->GetCBSInfo(val);
	
	if(val.bETWS  && err == ER_OK)
	{
		err = SetETWSChannel(conn, val);
	}
	
	return err;
}

int CReqSetCbMode::Execute(Connection *conn, CResponse*)
{
	return conn->SetCBMode(
				_NPARAM(0), _NPARAM(1), 
				_SPARAM(0), _SPARAM(1));	
}

int CReqConfigCbs::Execute(Connection *conn, CResponse*)
{
//	return conn->SetCBSMsgInd();
	
	return ER_OK;
}
#endif	// _CBS_SUPPORT_

#ifdef _ETWS_SUPPORT_
int CReqGetEtwsInfo::Execute(Connection *conn, CResponse *p)
{
	ETWSInfo &val = static_cast<MyType*>(p)->Data();
	return conn->GetETWSInfo(val);
}

int CReqSetEtwsInfo::Execute(Connection *conn, CResponse*)
{
	return conn->SetETWSInfo(_NPARAM(0), _NPARAM(1), _NPARAM(2));
}
#endif

int CReqReadPbk::Execute(Connection *conn, CResponse *p)
{
	BOOL &val = static_cast<MyType*>(p)->Data();
	
	PhoneBookItem *pPbk = (PhoneBookItem*)(_PPARAM(0));
	
	ASSERT (NULL != p);
	ASSERT (NULL != pPbk);
	
	int err = ER_OK;
	
	val = pPbk->read(
					 conn,
					 _SPARAM(0),	// memory type
					 _NPARAM(0),	// index
					 err);
	
	return err;
}

int CReqWritePbk::Execute(Connection *conn, CResponse *p)
{
	BOOL &val = static_cast<MyType*>(p)->Data();
	
	PhoneBookItem *pPbk = (PhoneBookItem*)(_PPARAM(0));
	
	ASSERT (NULL != p);
	ASSERT (NULL != pPbk);
	
	int err = ER_OK;
	
	val = pPbk->write(conn,
					  _SPARAM(0),	// memory type
					  err);
	
	return err;
}

#ifdef _VCARD_SUPPORT_
int CReqReadAdvPbk::Execute(Connection *conn, CResponse *p)
{
	BOOL &val = static_cast<MyType*>(p)->Data();
	
	PhoneBookItem *pPbk = (PhoneBookItem*)(_PPARAM(0));
	
	ASSERT (NULL != p);
	ASSERT (NULL != pPbk);
	
	int err = ER_OK;
	
	// no such thing as 'read_adv_items' yet...
	val = pPbk->read_adv_items(conn, err);
	
	return err;
}
#endif

int CReqErasePbk::Execute(Connection *conn, CResponse *p)
{
	BOOL &val = static_cast<MyType*>(p)->Data();
	
	PhoneBookItem *pPbk = (PhoneBookItem*)(_PPARAM(0));
	
	ASSERT (NULL != p);
	ASSERT (NULL != pPbk);
	
	int err = ER_OK;
	
	val = pPbk->EraseItem(conn, err);
	
	return err;
}

int CReqReplacePbk::Execute(Connection *conn, CResponse *p)
{
	BOOL &val = static_cast<MyType*>(p)->Data();
	
	PhoneBookItem *pPbk = (PhoneBookItem*)(_PPARAM(0));
	PhoneBookItem *pNewItem = (PhoneBookItem*)(_PPARAM(1));
	
	ASSERT (NULL != p);
	ASSERT (NULL != pPbk);
	ASSERT (NULL != pNewItem);
	
	int err = ER_OK;
	
	val = pPbk->ReplaceItem(conn, err, *pNewItem);
	
	delete pNewItem;	// possible leak
	
	return err;
}

int CReqCountPbk::Execute(Connection *conn, CResponse *p)
{
	std::vector<int> &val = static_cast<MyType*>(p)->Data();
	
	int buf[4] = { 0 };
	
	int err = conn->GetTotalPhonebookNumber(_SPARAM(0), buf, 0xff);
	
	val.push_back(buf[0]);	// total
	val.push_back(buf[2]);	// max num len
	val.push_back(buf[3]);	// max name len
	val.push_back(buf[1]);	// used
	
	return err;
}

//////////////////////////////////////////////////////////////////////////
#ifdef _STK_SUPPORT_
int CReqStkRespond::Execute(Connection *conn, CResponse*)
{
	return conn->STKResponse(_SPARAM(0));
}

int CReqStkEnvelope::Execute(Connection *conn, CResponse*)
{
	return conn->STKEnvelope(_SPARAM(0));
}

int CReqStkCall::Execute(Connection *conn, CResponse *p)
{
	CCallList &val = static_cast<MyType*>(p)->Data();
	
	int opt = HIWORD(_NPARAM(0));
	
	int err = ER_OK;

	if (0 != opt)
	{
		if (1 == opt || 2 == opt)
			err = conn->ChangeCallLine(opt, 0);
		else
			err = conn->TerminateCall();	// hang up all
		
		if (ER_OK != err)
		{
			conn->STKSetupCall(0x20);
			return err;
		}
	}
	
	opt = LOWORD(_NPARAM(0));
	
	CCallList calls;

	if (0 == opt || 4 == opt)
	{
		err = ListCalls(conn, calls);
		if (ER_OK != err)
		{
			conn->STKSetupCall(0x20);
			return err;
		}
	}
	
	err = conn->STKSetupCall(opt);
	
	// to make sure the new call is in the list
	if (0 == opt || 4 == opt)
	{
		if (ER_OK == err)
			err = WaitNewCall(conn, calls, val);
		else
			err = ListCalls(conn, val);
	}
	
	return err;
}

int CReqStkSS::Execute(Connection *conn, CResponse*)
{
	return conn->STKSetupSS(_NPARAM(0));
}

int CReqStkUSSD::Execute(Connection *conn, CResponse*)
{
	return conn->STKSetupUSSD(_NPARAM(0));
}

int CReqStkSMS::Execute(Connection *conn, CResponse*)
{
	return conn->STKSendSMS(_NPARAM(0));
}

int CReqStkDTMF::Execute(Connection *conn, CResponse*)
{
	return conn->STKSendDTMF(_NPARAM(0));
}

int CReqStkATCmd::Execute(Connection *conn, CResponse*)
{
	return conn->STKRunAtCmd(_NPARAM(0));
}

int CReqGetSimImage::Execute(Connection *conn, CResponse *p)
{
	int &val = static_cast<MyType*>(p)->Data();
	return conn->GetSIMImage(_NPARAM(0), (HBITMAP&)(val));
}
#endif	// _STK_SUPPORT_

//20110107 by foil
int CReqSetLogMode::Execute(Connection *conn, CResponse*)
{
	return conn->SetLoggingMode(_NPARAM(0) != 0);
}

int CReqSetDbgPort::Execute(Connection *conn, CResponse*)
{
	return conn->SetDebugPort(_NPARAM(0) != 0);
}

//	20110130 by foil
int CReqSetNetWorkType::Execute(Connection *conn, CResponse*)
{
	return conn->SetNetworkType(_NPARAM(0));
}

/******************************************************************************/

//////////////////////////////////////////////////////////////////////////
