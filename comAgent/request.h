/*
 *  request.h
 *  DCT
 *
 *  Created by MBJ on 12-7-8.
 *  Copyright 2012 __MyCompanyName__. All rights reserved.
 *
 */

#ifndef _COMAGENT_REQUEST_H_
#define _COMAGENT_REQUEST_H_

#ifdef WIN32
#include "stdafx.h"
#else
#include "Win2Mac.h"
#endif

#include "customize.h"
#include "Connection.h"
#include "Types.h"
#include "CallInfo.h"
#include "SMSMessage.h"
#include "PhoneBookItem.h"

#include "UrcUnit.h"

class CBaseReq
{
public:
	CBaseReq()
	{
	}
	
	virtual ~CBaseReq()
	{
	}
	
public:
	void Init(HWND _hWnd, UINT _uMsg, short _step, LONG _lExt, bool _call = false)
	{
		hWnd = _hWnd;
		uMsg = _uMsg;
		step = _step;
		lExt = _lExt;
		clcc = (short)_call;
	}
	
	CResponse *Abort()
	{
		return AllocResp();
	}
	
	CResponse *Execute(Connection *conn, int *err)
	{
		CResponse *p = AllocResp();
		*err = Execute(conn, p);
		return p;
	}

	BOOL Respond(int err, CResponse *p) const;

	BOOL IsCallReq(void) const
	{
		return (clcc != 0);
	}

	static int ListCalls(Connection *conn, CCallList& calls);
	
protected:
	virtual CResponse *AllocResp()
	{
		return new CResponse(lExt);
	}
	
	virtual int Execute(Connection *, CResponse*) = 0;

protected:
	static const char* GetFacility(int fac);
	
	static int GetPwdType(const string& code);
	
	static int GetPwdRemain(Connection *conn, int pwdType);
	
	static int GetPwdState(Connection *conn, DualWord &pinStat, int fac);
	
	static int WaitNewCall(
		Connection *conn, 
		const CCallList &oldCalls, 
		CCallList &newCalls);

protected:
	HWND	hWnd;
	UINT	uMsg;
	LONG	lExt;
	short	step;
	short	clcc;

};


template <size_t _N>
struct _N_Struct
{
	INT32	nParam[_N];
};

template <size_t _S>
struct _S_Struct
{
	CString	sParam[_S];
};

template <size_t _P>
struct _P_Struct
{
	LPVOID	pParam[_P];
};

template <size_t _N, size_t _S>
struct _NS_Struct
{
	INT32	nParam[_N];
	CString	sParam[_S];
};

template <size_t _S, size_t _P>
struct _SP_Struct
{
	LPVOID	pParam[_P];
	CString	sParam[_S];
};

template <size_t _N, size_t _S, size_t _P>
struct _NSP_Struct
{
	INT32	nParam[_N];
	LPVOID	pParam[_P];
	CString	sParam[_S];
};

template <typename _Tx>
class CRequestT : public CBaseReq
{
protected:
	typedef _Tx MyType;
	
	virtual CResponse *AllocResp()
	{
		return new _Tx(lExt);
	}
};

template <typename _Dx>
class CRequestD : public CBaseReq
{
public:
	_Dx & Param()
	{
		return _param;
	}

protected:
	_Dx	_param;
};

template <typename _Dx, typename _Tx>
class CRequestDT : public CBaseReq
{
public:
	_Dx & Param()
	{
		return _param;
	}

protected:
	typedef _Tx MyType;
	
	virtual CResponse *AllocResp()
	{
		return new _Tx(lExt);
	}
	
protected:
	_Dx	_param;
};

class CReqPowerOn : public CRequestD<_N_Struct<1> >
{
protected:
	virtual int Execute(Connection*, CResponse*);
};

class CReqPowerOff : public CRequestD<_N_Struct<1> >
{
protected:
	virtual int Execute(Connection*, CResponse*);
};

class CReqInitBasic : public CBaseReq
{
protected:
	virtual int Execute(Connection*, CResponse*);
};

class CReqInitAdv : public CBaseReq
{
protected:	
	virtual int Execute(Connection*, CResponse*);
};

class CReqInitNet : public CBaseReq
{
protected:
	virtual int Execute(Connection*, CResponse*);
};

class CReqQueryPin : public CRequestDT<_N_Struct<1>, CRespPinState>
{
protected:
	virtual int Execute(Connection*, CResponse*);
};

class CReqPinValidate : public CRequestDT<_S_Struct<2>, CRespPinState>
{
protected:
	virtual int Execute(Connection*, CResponse*);
};

class CReqQuerySim : public CRequestT<CRespInt>
{
protected:
	virtual int Execute(Connection*, CResponse*);
};

class CReqQueryInd : public CRequestT<CRespIndicate>
{
protected:
	virtual int Execute(Connection*, CResponse*);
};

class CReqQueryCall : public CRequestT<CRespCalls>
{
protected:
	virtual int Execute(Connection*, CResponse*);
};

class CReqMakeCall : public CRequestDT<_NS_Struct<1,1>, CRespCalls>
{
protected:
	virtual int Execute(Connection*, CResponse*);
};

class CReqEndCall : public CRequestDT<_N_Struct<1>, CRespCalls>
{
protected:
	virtual int Execute(Connection*, CResponse*);
};

class CReqHoldCall : public CRequestDT<_N_Struct<2>, CRespCalls>
{
protected:
	virtual int Execute(Connection*, CResponse*);
};

class CReqAnswerCall : public CRequestT<CRespCalls>
{
protected:
	virtual int Execute(Connection*, CResponse*);
};

class CReqDeflectCall : public CRequestDT<_S_Struct<1>, CRespCalls>
{
protected:
	virtual int Execute(Connection*, CResponse*);
};

class CReqSendDtmf : public CRequestDT<_NS_Struct<1,1>, CRespCalls>
{
protected:
	virtual int Execute(Connection*, CResponse*);
};

class CReqExtErr : public CRequestT<CRespErrex>
{
protected:
	virtual int Execute(Connection*, CResponse*);
};

class CReqGetClir : public CRequestT<CRespInt>
{
protected:
	virtual int Execute(Connection*, CResponse*);
};

class CReqSetClir : public CRequestDT<_N_Struct<1>, CRespInt>
{
protected:
	virtual int Execute(Connection*, CResponse*);
};

class CReqQueryPin2 : public CRequestT<CRespPinState>
{
protected:
	virtual int Execute(Connection*, CResponse*);
};

class CReqPin2Validate : public CRequestDT<_S_Struct<2>, CRespPinState>
{
protected:
	virtual int Execute(Connection*, CResponse*);
};

class CReqGetSimLock : public CRequestT<CRespFlag>
{
protected:
	virtual int Execute(Connection*, CResponse*);
};

class CReqSetSimLock : public CRequestDT<_NS_Struct<1,1>, CRespPinState>
{
protected:
	virtual int Execute(Connection*, CResponse*);
};

class CReqGetFacLock : public CRequestDT<_N_Struct<2>, CRespInt>
{
protected:
	virtual int Execute(Connection*, CResponse*);
};

class CReqSetFacLock : public CRequestD<_NS_Struct<3,1> >
{
protected:
	virtual int Execute(Connection*, CResponse*);
};

class CReqChangePwd : public CRequestDT<_NS_Struct<1,2>, CRespPinState>
{
protected:
	virtual int Execute(Connection*, CResponse*);
};

class CReqUnblockPIN : public CRequestDT<_NS_Struct<1,2>, CRespPinState>
{
protected:
	virtual int Execute(Connection*, CResponse*);
};

class CReqReadCF : public CRequestDT<_N_Struct<2>, CRespCFInfo>
{
protected:
	virtual int Execute(Connection*, CResponse*);
};

class CReqWriteCF : public CRequestD<_NS_Struct<3,1> >
{
protected:
	virtual int Execute(Connection*, CResponse*);
};

class CReqReadCW : public CRequestDT<_N_Struct<1>, CRespInt>
{
protected:
	virtual int Execute(Connection*, CResponse*);
};

class CReqWriteCW : public CRequestD<_N_Struct<2> >
{
protected:
	virtual int Execute(Connection*, CResponse*);
};

#ifdef _CALL_COST_SUPPORT_
class CReqGetCallMeter : public CRequestT<CRespInt>
{
protected:
	virtual int Execute(Connection*, CResponse*);
};

class CReqGetAcmValue : public CRequestT<CRespInt>
{
protected:
	virtual int Execute(Connection*, CResponse*);
};

class CReqResetAcmValue : public CRequestD<_S_Struct<1> >
{
protected:
	virtual int Execute(Connection*, CResponse*);
};

class CReqReadAcmMax : public CRequestT<CRespInt>
{
protected:
	virtual int Execute(Connection*, CResponse*);
};

class CReqWriteAcmMax : public CRequestD<_NS_Struct<1,1> >
{
protected:
	virtual int Execute(Connection*, CResponse*);
};

class CReqGetPucValue : public CRequestT<CRespMulLine>
{
protected:
	virtual int Execute(Connection*, CResponse*);
};

class CReqSetPucValue : public CRequestD<_S_Struct<3> >
{
protected:
	virtual int Execute(Connection*, CResponse*);
};
#endif	// _CALL_COST_SUPPORT_

class CReqGetOperator : public CRequestDT<_N_Struct<1>, CRespOperInfo>
{
protected:
	virtual int Execute(Connection*, CResponse*);
};

class CReqListOperator : public CRequestT<CRespOperList>
{
protected:
	virtual int Execute(Connection*, CResponse*);
};

class CReqSetOperator : public CRequestD<_NS_Struct<1,1> >
{
protected:
	virtual int Execute(Connection*, CResponse*);
};

class CReqQueryGprsState : public CRequestT<CRespInt>
{
protected:
	virtual int Execute(Connection*, CResponse*);
};

class CReqGetVersion : public CRequestT<CRespText>
{
protected:
	virtual int Execute(Connection*, CResponse*);
};

/************ 20131109 by Zhuwei **********************************************/

class CReqGetVERNO : public CRequestT<CRespText>
{
protected:
	virtual int Execute(Connection*, CResponse*);
};

/******************************************************************************/

class CReqGetIMEI : public CRequestT<CRespText>
{
protected:
	virtual int Execute(Connection*, CResponse*);
};

class CReqGetIMSI : public CRequestT<CRespText>
{
protected:
	virtual int Execute(Connection*, CResponse*);
};

class CReqGetSimOwner : public CRequestT<CRespText>
{
protected:
	virtual int Execute(Connection*, CResponse*);
};

class CReqSetAPN : public CRequestD<_S_Struct<1> >
{
protected:
	virtual int Execute(Connection*, CResponse*);
};

class CReqQueryRatPs : public CRequestT<CRespArray>
{
protected:
	virtual int Execute(Connection*, CResponse*);
};

/*
class CReqUpdateTime : public CBaseReq
{
protected:
	virtual int Execute(Connection*, CResponse*);
};
*/

#ifdef _USSD_SUPPORT_
class CReqUssdCtrl : public CRequestD<_NS_Struct<1,1> >
{
protected:
	virtual int Execute(Connection*, CResponse*);
};

#endif	// _USSD_SUPPORT_

class CReqSendSms : public CRequestDT<_P_Struct<1>, CRespFlag>
{
protected:
	virtual int Execute(Connection*, CResponse*);
};

class CReqReadSms : public CRequestDT<_NSP_Struct<1,1,1>, CRespFlag>
{
protected:
	virtual int Execute(Connection*, CResponse*);
};

class CReqWriteSms : public CRequestDT<_SP_Struct<1,1>, CRespFlag>
{
protected:
	virtual int Execute(Connection*, CResponse*);
};

class CReqEraseSms : public CRequestDT<_P_Struct<1>, CRespFlag>
{
protected:
	virtual int Execute(Connection*, CResponse*);
};

class CReqReplaceSms : public CRequestDT<_P_Struct<1>, CRespFlag>
{
protected:
	virtual int Execute(Connection*, CResponse*);
};

class CReqSetSmsMem3 : public CRequestD<_S_Struct<1> >
{
protected:
	virtual int Execute(Connection*, CResponse*);
};

class CReqGetSmsMem3 : public CRequestT<CRespText>
{
protected:
	virtual int Execute(Connection*, CResponse*);
};

class CReqConfigSms : public CBaseReq
{
protected:
	virtual int Execute(Connection*, CResponse*);
};

class CReqCountSms : public CRequestDT<_S_Struct<1>, CRespArray>
{
protected:
	virtual int Execute(Connection*, CResponse*);
};

class CReqSetSmsStat : public CRequestD<_N_Struct<1> >
{
protected:
	virtual int Execute(Connection*, CResponse*);
};

class CReqSetSmsc : public CRequestD<_S_Struct<1> >
{
protected:
	virtual int Execute(Connection*, CResponse*);
};

#ifdef _CBS_SUPPORT_
class CReqGetCbsInfo : public CRequestDT<_NS_Struct<1,1>, CRespCBSInfo>
{
protected:
	virtual int Execute(Connection*, CResponse*);
	int SetETWSChannel(Connection*, const CBSInfo&);	
};

class CReqSetCbMode : public CRequestD<_NS_Struct<2,2> >
{
protected:
	virtual int Execute(Connection*, CResponse*);
};

class CReqConfigCbs : public CBaseReq
{
protected:
	virtual int Execute(Connection*, CResponse*);
};
#endif	// _CBS_SUPPORT_

#ifdef _ETWS_SUPPORT_
class CReqGetEtwsInfo : public CRequestT<CRespETWSInfo>
{
protected:
	virtual int Execute(Connection*, CResponse*);
};

class CReqSetEtwsInfo : public CRequestD<_N_Struct<3> >
{
protected:
	virtual int Execute(Connection*, CResponse*);
};
#endif	// _ETWS_SUPPORT_

class CReqReadPbk : public CRequestDT<_NSP_Struct<1,1,1>, CRespFlag>
{
protected:
	virtual int Execute(Connection*, CResponse*);
};

class CReqWritePbk : public CRequestDT<_SP_Struct<1,1>, CRespFlag>
{
protected:
	virtual int Execute(Connection*, CResponse*);
};

#ifdef _VCARD_SUPPORT_
class CReqReadAdvPbk : public CRequestDT<_P_Struct<1>, CRespFlag>
{
protected:
	virtual int Execute(Connection*, CResponse*);
};
#endif

class CReqErasePbk : public CRequestDT<_P_Struct<1>, CRespFlag>
{
protected:
	virtual int Execute(Connection*, CResponse*);
};

class CReqReplacePbk : public CRequestDT<_P_Struct<2>, CRespFlag>
{
protected:
	virtual int Execute(Connection*, CResponse*);
};

class CReqCountPbk : public CRequestDT<_S_Struct<1>, CRespArray>
{
protected:
	virtual int Execute(Connection*, CResponse*);
};

#ifdef _STK_SUPPORT_
class CReqStkRespond : public CRequestD<_S_Struct<1> >
{
protected:
	virtual int Execute(Connection*, CResponse*);
};

class CReqStkEnvelope : public CRequestD<_S_Struct<1> >
{
protected:
	virtual int Execute(Connection*, CResponse*);
};

class CReqStkCall : public CRequestDT<_N_Struct<1>, CRespCalls>
{
protected:
	virtual int Execute(Connection*, CResponse*);
};

class CReqStkSS : public CRequestD<_N_Struct<1> >
{
protected:
	virtual int Execute(Connection*, CResponse*);
};

class CReqStkUSSD : public CRequestD<_N_Struct<1> >
{
protected:
	virtual int Execute(Connection*, CResponse*);
};

class CReqStkSMS : public CRequestD<_N_Struct<1> >
{
protected:
	virtual int Execute(Connection*, CResponse*);
};

class CReqStkDTMF : public CRequestD<_N_Struct<1> >
{
protected:
	virtual int Execute(Connection*, CResponse*);
};

class CReqStkATCmd : public CRequestD<_N_Struct<1> >
{
protected:
	virtual int Execute(Connection*, CResponse*);
};

class CReqGetSimImage : public CRequestDT<_N_Struct<1>, CRespInt>
{
protected:
	virtual int Execute(Connection*, CResponse*);
};
#endif	// _STK_SUPPORT_

class CReqSetLogMode : public CRequestD<_N_Struct<1> >
{
protected:
	virtual int Execute(Connection*, CResponse*);
};

class CReqSetDbgPort : public CRequestD<_N_Struct<1> >
{
protected:
	virtual int Execute(Connection*, CResponse*);
};

class CReqSetNetWorkType : public CRequestD<_N_Struct<1> >
{
protected:
	virtual int Execute(Connection*, CResponse*);
};
#endif	// _COMAGENT_REQUEST_H_
