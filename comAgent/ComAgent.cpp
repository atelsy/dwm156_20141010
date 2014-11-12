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
*   ComAgent.cpp
*
* Project:
* --------
*   DCT
*
* Description:
* ------------
*   ComAgent module
*
* Author:
* -------
*   Xiantao Han
*
*******************************************************************************/

// ComAgent.cpp : implementation file
//

#ifdef WIN32
#include "StdAfx.h"
#else
#include "Win2Mac.h"
#endif

#include "ComAgent.h"

#ifdef WIN32
#include <process.h>

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif
#endif	// WIN32

#define _CMUXINIT_ASYNC_

#define PIN_DELAY			500
#define SEARCHING_DELAY		(1*60*1000)	// 1 minute

#ifdef PIN_DELAY
#define TID_QUERYPIN		(PL_MAX+10)
#endif

#ifdef SEARCHING_DELAY
#define TID_SEARCHING		(PL_MAX+20)
#endif

#define SETTIMER(id, elp)	\
	SetTimer((id), (elp), NULL)

#define LOCK()		EnterCriticalSection(&m_theLock)
#define UNLOCK()	LeaveCriticalSection(&m_theLock)


#define MakePollingReq(i)	((CBaseReq*)((i)+1))
#define MakePollingIdx(p)	(UINT)(((ULONG)p) - 1)
#define IsPollingReq(p)		(((ULONG)p) <= PL_MAX)

/////////////////////////////////////////////////////////////////////////////
// CComAgent

//////////////////////////////////////////////////////////////////////////
// static variables
CComAgent *CComAgent::m_pSelf = NULL;
Connection CComAgent::m_conx;
// static variables
//////////////////////////////////////////////////////////////////////////

CComAgent::CComAgent()
{
	ASSERT(m_pSelf == NULL);	// single instance only

	m_nSysState	= SYS_NODEV;
	m_hEvtStart	= NULL;
	m_hEvtStop	= NULL;
	m_hThread	= NULL;

	m_pSelf		= this;
	
	InitializeCriticalSection(&m_theLock);
	
	memset(m_PollingQ, 0, sizeof(m_PollingQ));
}

CComAgent::~CComAgent()
{
	DestroyWindow();
	m_pSelf = NULL;
}


BEGIN_MESSAGE_MAP(CComAgent, CWnd)
	//{{AFX_MSG_MAP(CComAgent)
	ON_MESSAGE(WM_POLLING, OnPolling)
	ON_MESSAGE(WM_URCRESP, OnUrcResp)
	ON_MESSAGE(WM_REQUEST, OnRequest)
	ON_WM_CLOSE()
	ON_WM_TIMER()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CComAgent message handlers

BOOL CComAgent::Create(CWnd *pParent)
{
	BOOL flag = CWnd::Create(
		NULL, NULL, WS_CHILD, 
		CRect(0,0,0,0), pParent, 0);
	
	ShowWindow(SW_HIDE);
	
	return flag;
}

// restart polling timer
BOOL CComAgent::ResetPolling(UINT uID, UINT nElapse)
{
	const static UINT uPlElapse[] = 
	{
		4000,
		1000,	// at+clcc
		5000	// at
	};
	
	if (0 == nElapse)
		nElapse = uPlElapse[uID];

	if (!StopPolling(uID))
		return FALSE;
	if (m_port.IsEmpty())	// device removed
		return FALSE;

	return SETTIMER(uID, nElapse);
}

// stop polling timer
BOOL CComAgent::StopPolling(UINT uID)
{
	if (uID >= PL_MAX)
		return FALSE;

	KillTimer(uID);
	AbortPolling(uID);

	return TRUE;
}

// clear all urc clients
void CComAgent::DeregisterURC(void)
{
	for (int i=0; i<URC_MAX; i++)
		m_UrcList[i].Deregister();
}

// polling timer, to begin polling
void CComAgent::OnTimer(UINT nIDEvent) 
{
	KillTimer(nIDEvent);
	
	switch(nIDEvent)
	{
	case PL_CONN:
		if (SYS_DISC == m_nSysState)
		{
#ifdef _CMUXINIT_ASYNC_
			PollingRequest(PL_CMUXINIT);
#else
			InitConnection(m_port, UrcCallback);
			ResetPolling(PL_CONN);
#endif
		}
		else if (m_nSysState >= SYS_CONN)	// after AT ready
		{
			PollingRequest(PL_CONN);	// to detect disconnection
		}

		break;

	case PL_CLCC:
		PollingRequest(nIDEvent);
		break;

	// add more polling if necessary

#ifdef PIN_DELAY
	case (TID_QUERYPIN):
	case (TID_QUERYPIN+1):
	case (TID_QUERYPIN+2):
		QueryPIN(m_hWnd, WM_REQUEST, 
			IR_QUERYPIN + nIDEvent - TID_QUERYPIN);
		break;
#endif

#ifdef SEARCHING_DELAY
	case TID_SEARCHING:	// abort searching
		if (SYS_NETSEARCH == m_nSysState)
			SetSysState(SYS_NONETWORK);
		break;
#endif

	default:
		break;
	}
	CWnd::OnTimer(nIDEvent);
}

// Polling request result message
// to generate URC from polling result
LRESULT CComAgent::OnPolling(WPARAM wParam, LPARAM lParam)
{
	UINT  uID = (UINT)LOWORD(wParam);
	short err = (short)HIWORD(wParam);

	WATCH_POINTER(lParam);

	switch (uID)
	{
	case PL_CONN:
		if ((short)ER_OK != err)
		{
			if (m_nSysState >= SYS_ATREADY)
				SetSysState(SYS_DISC);
		}
		else if (m_nSysState == SYS_CMUX/*m_nSysState < SYS_ATREADY*/)
		{
			// polling to detect "at ready"
			SetSysState(SYS_ATREADY);
		}		
		ResetPolling(PL_CONN);
		break;

	case PL_CLCC:
		if (ER_OK == err)
		{
			ASSERT(lParam != 0);

			CRespCalls *p = (CRespCalls*)lParam;

			SetCallInd(p->Value());

			// stop polling when there're no calls
			if (p->Value().Size() <= 0)
				break;
			else if (p->Value().IsOutgoing())
			{
				ResetPolling(PL_CLCC);	// quick scan
				break;
			}
		}
		ResetPolling(PL_CLCC, 5000);
		break;

	default:
		break;
	}

	return 0;
}

// to stop request thread before exiting
void CComAgent::OnClose()
{
 	AbortAll();

	SetEvent(m_hEvtStop);
	if (WAIT_TIMEOUT == WaitForSingleObject(m_hThread, 5000))
	{
		TerminateThread(m_hThread, 0);
		DebugOut(_T("\n#===============================#\n"));
		DebugOut(_T(" Warning: job thread terminated!"));
		DebugOut(_T("\n#===============================#\n"));
	}

 	CloseHandle(m_hThread);

	CloseHandle(m_hEvtStart);
	CloseHandle(m_hEvtStop);
	
	DeleteCriticalSection(&m_theLock);
	
	m_hEvtStart = NULL;
	m_hEvtStop = NULL;
	m_hThread = NULL;

	m_conx.Exit();

	CWnd::OnClose();
}


#ifdef _ENABLE_LOG_

#define CaseToText(_c)	\
	case _c: return _T(#_c)

static const TCHAR *SysStateText(int stat)
{
	switch(LOWORD(stat))
	{
	CaseToText(SYS_NODEV);
	CaseToText(SYS_DISC);
	CaseToText(SYS_CONN);
	CaseToText(SYS_CMUX);
	CaseToText(SYS_ATREADY);

	CaseToText(SYS_NOSIM);
	CaseToText(SYS_BADSIM);

	CaseToText(SYS_SIMOK);
	CaseToText(SYS_PINREQ);
	CaseToText(SYS_PUKREQ);
	CaseToText(SYS_PIN2REQ);
	CaseToText(SYS_PUK2REQ);

	CaseToText(SYS_PINREADY);
	CaseToText(SYS_NONETWORK);
	CaseToText(SYS_NETSEARCH);
	CaseToText(SYS_NETREADY);
	CaseToText(SYS_NETHOME);
	CaseToText(SYS_NETROAMING);
	
	default:
		break;
	}

	return _T("SYS_STATE_@@");
}
#endif	// _ENABLE_LOG_

// internal response to some special urcs
// further actions when receiving a URC
LRESULT CComAgent::OnUrcResp(WPARAM wParam, LPARAM lParam)
{
	WATCH_POINTER(lParam);

	switch(wParam)
	{
	case URC_SYS:
		
	//	ASSERT(m_nSysState != ((CUrcSys*)lParam)->Value().alpha);

		m_nSysState = ((CUrcSys*)lParam)->Value().alpha;

		DebugOut(
			_T("\n>>>>>>>>>>>>>> system state: %s\n"), 
			SysStateText(m_nSysState));
		
		switch(m_nSysState)
		{
		case SYS_NODEV:	// disconnected, trying to reconnect
		case SYS_DISC:	// disconnected, trying to reconnect
			Reset();
			break;

		case SYS_CONN:	// connected, trying to open CMUX
			// "SYS_CMUX" is useless to users
			// so it's unnecessary to notify
			m_nSysState = SYS_CMUX;	// cmux ready

			// now wait for "+eind:128"
			// if this URC is missed,
			// the process will STOP here.
			// so we start polling even if
			// maybe "at" is not ready yet
			PollingRequest(PL_CONN);
			
			break;

		case SYS_ATREADY:
			PowerOn(m_hWnd, WM_REQUEST, IR_POWERON);	// power-on procedure
		//	SetLoggingMode(m_hWnd, WM_REQUEST, IR_LOGGINGMODE); // 20110106 by foil
			break;
			
		case SYS_SIMOK:	// this status may be skipped
			QueryPIN(m_hWnd, WM_REQUEST, IR_QUERYPIN);
			break;

		case SYS_PINREADY:
			InitTarget1(m_hWnd, WM_REQUEST, IR_INIT1);

			// at+eind?, at+creg?
			// we could have missed "+eind:1/2" & "+creg:*"
			QueryInd(m_hWnd, WM_REQUEST, IR_QUERYIND); 

			break;
		
#ifdef SEARCHING_DELAY	// to limit searching time
		case SYS_NETSEARCH:
			SETTIMER(TID_SEARCHING, SEARCHING_DELAY);
			break;
#endif

		case SYS_NETHOME:
		case SYS_NETROAMING:
			InitTarget2(m_hWnd, WM_REQUEST, IR_INIT2);
#ifdef SEARCHING_DELAY
			KillTimer(TID_SEARCHING);
#endif
			break;

		default:
			break;
		}
		
		break;

	case URC_CCWA:
	case URC_RING:
	case URC_NOCA:
    case URC_PSBEARER:
    case URC_CUSD:
		QueryCall(m_hWnd, WM_POLLING, PL_CLCC);
		break;
		
	default:
		break;
	}

	return 0;
}

// register this callback to smslib.
// it is called in urc thread.
// OR call it when a certain event occurs
void CComAgent::UrcCallback(void * pData)
{
	const ATResult *pResult = (ATResult*)pData;

	if (NULL == pResult)
	{
		// SMSLib doesn't parse "+eind:128" outside CMUX mode.
		// In the case, "pResult" could be NULL.
		// And this indicator means connection is set up.

		m_pSelf->SetSysState(SYS_CONN);
		return;
	}

	if (m_pSelf->m_nSysState < SYS_CONN)
		return;

	for(int i=0; i<URC_MAX; i++)
	{
		if (m_pSelf->m_UrcList[i].Decode(pResult))
		{
			break;
		}
	}
	
	// reset connection polling ("ResetPolling" is not a static func)
	m_pSelf->PostMessage(WM_POLLING, PL_CONN, 0);	
}

// to put a request into the queue with high priority
CAERR CComAgent::HighRequest(CBaseReq *pReq)
{
	m_HighQ.push(pReq);
	Start();
	return CA_OK;
}

// to put a request into the queue with normal priority
CAERR CComAgent::NormalRequest(CBaseReq *pReq)
{
	m_NormalQ.push(pReq);
	Start();
	return CA_OK;
}

// to add a polling request
CAERR CComAgent::PollingRequest(UINT uID)
{
	if (uID > PL_MAX)
		return CA_BADPARAM;

	if (PL_CLCC == uID && m_nSysState < SYS_PINREADY)
		return CA_NOTALLOWED;

	LOCK();
	m_PollingQ[uID] = TRUE;
	UNLOCK();

	Start();
	
	return CA_OK;
}

// to get a request in order of priority
CBaseReq * CComAgent::PopRequest(void)
{
	CBaseReq *p = m_HighQ.pop();

	LOCK();
	if (p == NULL && m_PollingQ[PL_CLCC])
	{
		m_PollingQ[PL_CLCC] = FALSE;
		p = MakePollingReq(PL_CLCC);
	}

	if (p == NULL)
	{
		p = m_NormalQ.pop();
	}

	if (p == NULL)
	{
		for (int i=0; i<PL_MAX; i++)
		{
			if (m_PollingQ[i])
			{
				m_PollingQ[i] = FALSE;
				p = MakePollingReq(i);
				break;
			}
		}
	}
	UNLOCK();

	return p;
}

// to send final result of a request to the caller
BOOL CComAgent::Respond(const CBaseReq *pReq, int err_code, CResponse * pResp /*= NULL*/)
{
	ASSERT(NULL != pReq);

	pReq->Respond(err_code, pResp);
	
	if (pReq->IsCallReq())	// to update CLCC result
	{
		if (ER_OK == err_code)
		{
			pResp->retain();
			
			PostMessage(WM_POLLING, 
				MAKEWPARAM(PL_CLCC, err_code), 
				(LPARAM)pResp);
		}
		else
		{
			QueryCall(m_hWnd, WM_POLLING, PL_CLCC);
		}
	}
	
	pResp->release();

	return TRUE;
}

#if 0
// to remove a request from queue
BOOL CComAgent::AbortRequest(REQHandler proc, HWND hWnd /*= NULL*/)
{
	REQUEST req;

	req.reqProc = proc;
	req.client.hWnd = hWnd;

	if (m_HighQ.pick(&req) || m_NormalQ.pick(&req))	// get and erase
	{
		CResponse *p = m_conn.Abort(req);
		Respond(req, ER_USERABORT, p);
		return TRUE;
	}
// 	else if (GetCurRequest() == uID)
// 	{
// 		flag = m_conx.Cancel();
// 	}

	return FALSE;
}
#endif

// to remove a polling request
BOOL CComAgent::AbortPolling(UINT uID)
{
	LOCK();
	m_PollingQ[uID] = FALSE;
	UNLOCK();

	return TRUE;
}

// to abort all requests
void CComAgent::AbortAll(void)
{
	LOCK();

	m_HighQ.clear();
	m_NormalQ.clear();

	UNLOCK();

	for (int i=0; i<PL_MAX; i++)
	{
		AbortPolling(i);
	}
	
	KillTimer(PL_MAX);
	m_conx.Cancel();
}

BOOL CComAgent::Reset(void)
{
	AbortAll();
	ResetPolling(PL_CONN);

//	m_conx.Conn_DeInit();

	// reset URC value
	ResetUrc(URC_SMS);
	ResetUrc(URC_PBK);
	ResetUrc(URC_ECSQ);
//	ResetUrc(URC_CLIP);
//	ResetUrc(URC_COLP);
	ResetUrc(URC_CLCC);
//	ResetUrc(URC_CGREG);	// no need to reset as an event

	return TRUE;
}

void CComAgent::ResetUrc(UINT uID)
{
	m_UrcList[uID].Reset();
}

// to start request thread
BOOL CComAgent::Start(void)
{
	if (NULL == m_hThread)
	{
		if (NULL == m_hEvtStart)
			m_hEvtStart = CreateEvent(NULL, FALSE, FALSE, NULL);
		if (NULL == m_hEvtStop)
			m_hEvtStop = CreateEvent(NULL, FALSE, FALSE, NULL);
		
		m_hThread = (HANDLE)_beginthreadex(NULL, 0, RequestProc, 
								   (LPVOID)this, 0, NULL);
	}
	
	SetEvent(m_hEvtStart);

	return (NULL != m_hThread);
}

// request thread proc
UINT CALLBACK CComAgent::RequestProc(void *p)
{
	ASSERT (NULL != p);

	CComAgent *pAgent = (CComAgent*)p;

	const HANDLE hEvents[] = 
	{
		pAgent->m_hEvtStart, 
		pAgent->m_hEvtStop
	};

	CBaseReq *pReq = NULL;

	short err, nCount = 0;

	while (TRUE)
	{
		switch (WaitForMultipleObjects(2, hEvents, FALSE, INFINITE))
		{
		case WAIT_OBJECT_0:	// start
			while (TRUE)
			{
				pReq = pAgent->PopRequest();

				if (NULL == pReq)
				{
					break;
				}
				else if (IsPollingReq(pReq))
				{
					err = pAgent->Polling(MakePollingIdx(pReq));
				}
				else
				{
					err = pAgent->Execute(pReq);
					delete pReq;
				}

				if (ER_TIMEOUT == err)
				{
					// too many timeouts!
					if (++nCount >= 5)
					{
						nCount = 0;
						pAgent->SetSysState(SYS_DISC);
					}
				}
				else
				{
					nCount = 0;
				}

				if (WaitForSingleObject(hEvents[1], 0)==WAIT_OBJECT_0)
				{
					DebugOut(_T("[ComAgent]: request thread, busy stop\n"));
					return 0;
				}
			}
			break;

		case WAIT_OBJECT_0+1:	// stop
			DebugOut(_T("[ComAgent]: request thread, idle stop ...\n"));
			return 0;

		default:
			break;
		}
	}
	return 0;
}

// to execute a request
// running in the request thread
int CComAgent::Execute(CBaseReq *pReq)
{
	int err = ER_OK;

	CResponse *p = pReq->Execute(&m_conx, &err);

	Respond(pReq, err, p);

	if (ER_TIMEOUT != err &&
		ER_USERABORT != err)
	{
	//	ResetPolling(PL_CONN);	// not in main thread
		PostMessage(WM_POLLING, PL_CONN, 0);
	}
	
	return err;
}

// to execute a polling request
// running in the request thread
int CComAgent::Polling(UINT uID)
{
	int err = ER_OK;

	switch (uID)
	{
	case PL_CONN:
		err = TestConnection();
		PostMessage(WM_POLLING, MAKEWPARAM(uID, err), 0);
		break;
		
	case PL_CLCC:
		{
			CResponse *p = QueryCall(err);

			if (NULL != p)
			{
			//	p->retain();
				PostMessage(WM_POLLING, MAKEWPARAM(uID, err), (LPARAM)p);
			//	p->release();
			}

			if (ER_TIMEOUT != err && ER_USERABORT != err)
			{
			//	ResetPolling(PL_CONN);	// not in main thread
				PostMessage(WM_POLLING, PL_CONN, 0);
			}
		}
		break;

#ifdef _CMUXINIT_ASYNC_
	case PL_CMUXINIT:
		// init at-mode and try to send "at"
		StopPolling(PL_CMUXINIT);
		StopPolling(PL_CONN);
		InitConnection(m_port, UrcCallback);
		StopPolling(PL_CMUXINIT);
		ResetPolling(PL_CONN);
		break;
#endif

	default:
		break;
	}
	
	return err;
}

// internal request result message
// to generate URC from request result
LRESULT CComAgent::OnRequest(WPARAM wParam, LPARAM lParam)
{
	short code = LOWORD(wParam);
	short err  = HIWORD(wParam);

	WATCH_POINTER(lParam);
	
	switch (code)
	{
	/************** power-on result ***************/
	case (IR_POWERON):
	case (IR_POWERON+1):
	case (IR_POWERON+2):	// retry 3 times
		if (ER_OK == err)
		{
			DebugOut(_T("[ComAgent]: power on, ok ^^\n"));
			InitTarget0(m_hWnd, WM_REQUEST, IR_INIT0);		// basic settings
		}
		else if (code < IR_POWERON+2)
		{
			PowerOn(m_hWnd, WM_REQUEST, code+1);
		}
		else
		{
			DebugOut(_T("[ComAgent]: power on failed :(\n"));
			SetSysState(SYS_DISC);
		}
		break;
	/************** power-on result ***************/
		
	/**** init stage 1 (before PIN validation) ****/
	case (IR_INIT0):
	case (IR_INIT0+1):
	case (IR_INIT0+2):
		if (ER_OK == err)
		{
			DebugOut(_T("[ComAgent]: init stage 1, ok ^^\n"));
#ifdef PIN_DELAY
			SETTIMER(TID_QUERYPIN, /*PIN_DELAY*/5*1000);
			DebugOut(_T("[ComAgent]: delay 5 sec for next step ...\n"));
#else
		// the command "+ESIMS" is skipped.
		// use the error code of "+CPIN" to detect SIM instead
		//	QuerySIM(m_hWnd, WM_REQUEST, IR_QUERYSIM);
			QueryPIN(m_hWnd, WM_REQUEST, IR_QUERYPIN);
#endif
		}
		else if (code < IR_INIT0+2)	// retry
		{
			InitTarget0(m_hWnd, WM_REQUEST, code+1);
		}
		else
		{
			DebugOut(_T("[ComAgent]: init stage 1 failed :(\n"));
			SetSysState(SYS_DISC);
		}
		break;
	/**** init stage 1 (before PIN validation) ****/

	/**** init stage 2 (after PIN validation) *****/
	case (IR_INIT1):
	case (IR_INIT1+1):
	case (IR_INIT1+2):
		if (ER_OK == err)
			DebugOut(_T("[ComAgent]: init stage 2, ok ^^\n"));
		else if (code < IR_INIT1+2)	// retry
			InitTarget1(m_hWnd, WM_REQUEST, code+1);
		else
			DebugOut(_T("[ComAgent]: init stage 2 failed :(\n"));
		break;
	/**** init stage 2 (after PIN validation) *****/

	case IR_INIT2:
		if (ER_OK != err)
			DebugOut(_T("[ComAgent]: init stage 3, failed :(\n"));
		else
			DebugOut(_T("[ComAgent]: init stage 3, ok ^^\n"));
		break;

// 	case IR_QUERYSIM:
	case (IR_QUERYPIN):
	case (IR_QUERYPIN+1):
	case (IR_QUERYPIN+2):
		if (ER_OK == err && 0 != lParam)
		{
			const DualWord &pinState = 
				((CRespPinState*)lParam)->Value();
			
			if (pinState.alpha == PWD_PUK && pinState.beta == 0)
				SetSysState(SYS_BADSIM);
			else if (pinState.alpha != SYS_PINREADY || m_nSysState < SYS_PINREADY)
				SetSysState(pinState.alpha, pinState.beta);
		}
		else if (m_nSysState >= SYS_ATREADY)
		{
			if (code < IR_QUERYPIN+2)
			{
#ifdef PIN_DELAY
				SETTIMER(TID_QUERYPIN + (code+1-IR_QUERYPIN), 4*PIN_DELAY);
#else
				QueryPIN(m_hWnd, WM_REQUEST, code+1);
#endif
			}
			else
			{
				DebugOut(_T("[ComAgent]: query PIN failed :(\n"));
				SetSysState(SYS_BADSIM);
			}
		}
		break;

	case (IR_QUERYIND):
	case (IR_QUERYIND+1):
	case (IR_QUERYIND+2):
		if (ER_OK == err)
		{
			ASSERT(lParam != 0);
			CRespIndicate *p = (CRespIndicate*)lParam;
			SetIndicator(p->Value().alpha, p->Value().beta);
		}
		else if (code < IR_QUERYIND+2)
		{
			QueryInd(m_hWnd, WM_REQUEST, code+1);
		}
		break;

	case IR_POWEROFF:
		{
			CBaseReq *pReq = (CBaseReq*)(((CResponse*)lParam)->m_lExt);

			PostMessage(WM_CLOSE, 0, 0);	// close ComAgent itself

			Respond(pReq, err, pReq->Abort());

			delete pReq;
		}
		break;

#ifdef _STK_SUPPORT_
	case IR_SIMRESET:	// powered off, try to connect again
		if (m_nSysState > SYS_DISC)
			SetSysState(SYS_DISC);
		break;
#endif

	default:
		break;
	}
	
	return 0;
}

// to change system state (according to request result)
void CComAgent::SetSysState(WORD wState, WORD wInfo)
{
	if (m_nSysState < SYS_ATREADY && 
		wState > SYS_ATREADY)
	{
		return;
	}
	else if (m_nSysState < SYS_PINREADY && 
			 wState > SYS_PINREADY)
	{
		return;
	}
// 	else if (m_nSysState > SYS_PINREADY && 
// 		wState == SYS_PINREADY)
// 	{
// 		return;	// ???
// 	}
	else if (m_nSysState >= SYS_PINREADY && 
			 wState < SYS_PINREADY)
	{	// if PIN gets locked, operator would be lost
		ResetUrc(URC_PBK);
		ResetUrc(URC_SMS);
		ResetUrc(URC_CLCC);
	}
	
	m_UrcList[URC_SYS].Fill(DualWord(wState, wInfo));
}

// to change sms/pbk/reg state (according to request result)
void CComAgent::SetIndicator(WORD eind, WORD creg)
{
	creg = CUrcWorker::convCreg(creg, m_nSysState);

	if (m_nSysState < creg)
		SetSysState(creg);	// new value is checked inside

	if ((eind & 0x01) != 0)
	{
		m_UrcList[URC_SMS].Fill((BOOL)TRUE);
	}
	
	if ((eind & 0x02) != 0)
	{
		m_UrcList[URC_PBK].Fill((BOOL)TRUE);
	}
}

// to change call state (according to "cclc" result)
void CComAgent::SetCallInd(const CCallList& calls)
{
	m_UrcList[URC_CLCC].Fill(calls);
}

//////////////////////////////////////////////////////////////////////////
// user interfaces
//////////////////////////////////////////////////////////////////////////

// parent window should be main frame
// call "Initialize" to setup connection or modify params
BOOL CComAgent::Initialize(const CString& port)
{	
	if (!IsWindow(m_hWnd))
	{
		VERIFY ( Create(AfxGetMainWnd()) );
		
		// Register special URCs. 
		// ComAgent has to be the first client.
		RegisterURC(URC_SYS,  m_hWnd, WM_URCRESP);
		RegisterURC(URC_CCWA, m_hWnd, WM_URCRESP);
		RegisterURC(URC_RING, m_hWnd, WM_URCRESP);
		RegisterURC(URC_NOCA, m_hWnd, WM_URCRESP);
	}

	if (port.IsEmpty())
		return TRUE;
	
	m_port = port;

	if (m_nSysState < SYS_CONN)
	{
		// begin polling "at"
		// CMUX try to send "at", 
		// and wait for "ok" or "+eind:128"
		// both are taken as URC: SYS_CONN
#ifdef _CMUXINIT_ASYNC_
		PollingRequest(PL_CMUXINIT);
#else
		InitConnection(m_port, UrcCallback);
		ResetPolling(PL_CONN);
#endif
	}
	else	// user is trying to change CMUX parameters
			// in this case, the module isn't unplugged
			// we will miss most of the expected indicators
	{
	//	SetSysState(SYS_DISC);

		// disconnect first,
		// then in response of "SYS_DISC",
		// try to reconnect with new params
	}
	SetSysState(SYS_DISC);

	return TRUE;
}

void CComAgent::StopDevice(void)
{
	SetSysState(SYS_NODEV);
	
	if (IsWindow(m_hWnd))
		AbortAll();

	// this is fast enough to be used in the main thread
	m_conx.Conn_DeInit();

	DebugOut(_T("\n<><><> device(%s) stopped <><><>\n"), (LPCTSTR)m_port);

	m_port.Empty();

	SetSysState(SYS_NODEV);
}

// let me know who cares what
BOOL CComAgent::RegisterURC(UINT uID, HWND hWnd, UINT uMsg)
{
	ASSERT(uID < URC_MAX);
	
	if (!IsWindow(m_hWnd))
		return FALSE;
	
	// to register & notify the current value
	m_UrcList[uID].Register(hWnd, uMsg);

	return TRUE;
}

void CComAgent::DeregisterURC(UINT uID, HWND hWnd)
{
	ASSERT(uID < URC_MAX);
	m_UrcList[uID].Deregister(hWnd);
}

#ifdef _STK_SUPPORT_
void CComAgent::SimRefresh(bool reset, int nExt)
{
	if (m_nSysState <= SYS_ATREADY)
		return;

	if (reset)	// SIM Reset
	{
		PowerOff(m_hWnd, WM_REQUEST, IR_SIMRESET);
	}
	else if (m_nSysState >= SYS_PINREADY)
	{
		if (-1 == nExt || URC_PBK == nExt)
			ResetUrc(URC_PBK);
		if (-1 == nExt || URC_SMS == nExt)
			ResetUrc(URC_SMS);
		
		// todo: wait for +EIND:1/2...
	}
}
#endif

BOOL CComAgent::InitConnection(const CString& port, ME_Callback reply)
{
	m_conx.Conn_DeInit();
	
#ifdef WIN32
	return (BOOL)m_conx.Conn_Init(
		(char*)UCS2toASCII(port).c_str(), reply);
#else
	char cport[64];
	wcstombs(cport, port, 64);
	return (BOOL)m_conx.Conn_Init(cport, reply);
#endif
}

int CComAgent::TestConnection(void)
{
	for (int i=0; i<3; i++)
	{
		if (m_conx.Query() == ER_OK)
		{
			return ER_OK;
		}
	}
	return ER_TIMEOUT;
}

CResponse * CComAgent::QueryCall(int &err)
{
	CCallList calls;
	
	err = CBaseReq::ListCalls(&m_conx, calls);

	if (ER_OK == err)
	{
		return new CRespCalls(calls);
	}

	return NULL;
}
//////////////////////////////////////////////////////////////////////////
