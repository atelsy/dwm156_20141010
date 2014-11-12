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
*   ComAgent.h
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

#if !defined(AFX_COMAGENT_H__34AD2023_C4C1_43EF_BF3D_8C0780AC88E3__INCLUDED_)
#define AFX_COMAGENT_H__34AD2023_C4C1_43EF_BF3D_8C0780AC88E3__INCLUDED_

#include "RequestQ.h"
#include "UrcUnit.h"

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// ComAgent.h : header file
//


enum CAERR
{
	CA_OK = 0,
		
	// 1-256: reserved
		
	CA_NOTALLOWED = 1024,
	CA_NOSENSE,
	CA_BADPARAM,
	CA_NOMEMORY,
		
	CA_CALLFULL,
	CA_RESTRICTED,	// no permission
	CA_NONETWORK,
		
	CA_UNKNOWN
};

/////////////////////////////////////////////////////////////////////////////
// CComAgent window

class CComAgent : protected CWnd
{
// Construction
public:
	CComAgent();

// Attributes
public:

// Operations
public:
	BOOL Initialize(const CString& port);
	
	int  GetSysState(void) const { return m_nSysState; }

	static const Connection * GetConnection() { return &m_conx; }

#ifdef _STK_SUPPORT_
	void SimRefresh(bool reset, int nExt);
#endif

	BOOL RegisterURC(UINT uID, HWND hWnd, UINT uMsg);
	void DeregisterURC(UINT uID, HWND hWnd);

	void StopDevice(void);

	CAERR Close(HWND hWnd, UINT uMsg, short step = 0, 
		UINT (*last_req_cb)(void) = NULL, LONG lExt = 0);
	
	CAERR Suspend(HWND hWnd, UINT uMsg, short step = 0, 
		UINT (*last_req_cb)(void) = NULL, LONG lExt = 0);

	CAERR PinValidate(HWND hWnd, UINT uMsg, short step, 
		const CString& strPin, const CString& newPin = CString(), 
		LONG lExt = 0);

	CAERR Dial(HWND hWnd, UINT uMsg, short step, 
		const CString& strNum, LONG lExt = 0);
	CAERR ChangeCallLine(HWND hWnd, UINT uMsg, short step, 
		int code, int line = 0, LONG lExt = 0);	
	CAERR Deflect(HWND hWnd, UINT uMsg, short step, 
		const CString& strNum, LONG lExt = 0);

	CAERR Answer(HWND hWnd, UINT uMsg, short step = 0, LONG lExt = 0);
	CAERR HangUp(HWND hWnd, UINT uMsg, short step = 0, LONG lExt = 0);

	CAERR SendDTMF(HWND hWnd, UINT uMsg, short step, 
		char chDTMF, LONG lExt = 0);
	CAERR SendDTMF(HWND hWnd, UINT uMsg, short step, 
		const CString& strDTMF, LONG lExt = 0);
	
	CAERR GetExtendErr(HWND hWnd, UINT uMsg, short step = 0, LONG lExt = 0);

	CAERR SetClirScheme(HWND hWnd, UINT uMsg, short step,
		int nScheme, LONG lExt = 0);
	CAERR GetClirScheme(HWND hWnd, UINT uMsg, short step = 0, LONG lExt = 0);
	
	CAERR ChangePassword(HWND hWnd, UINT uMsg, short step,
		int facility, const CString& oldpwd, 
		const CString& newpwd, LONG lExt = 0);
	
	CAERR GetSIMLock(HWND hWnd, UINT uMsg, short step, LONG lExt = 0);
	CAERR SetSIMLock(HWND hWnd, UINT uMsg, short step, BOOL bLock, 
		const CString& pwd, LONG lExt = 0);

	CAERR GetCallBarring(HWND hWnd, UINT uMsg, short step,
		int facility, int nClass = 0, LONG lExt = 0);
	CAERR SetCallBarring(HWND hWnd, UINT uMsg, short step,
		int faclity, BOOL bLock, const CString& pwd, 
		int nClass = 0, LONG lExt = 0);

	CAERR QueryPIN(HWND hWnd, UINT uMsg, short step = 0, 
		int nPinType = 0, LONG lExt = 0);

	CAERR QueryPIN2(HWND hWnd, UINT uMsg, short step = 0, LONG lExt = 0);
	CAERR PIN2Validate(HWND hWnd, UINT uMsg, short step,
		const CString& strPin2, const CString& newPin2 = CString(), 
		LONG lExt = 0);
	
	CAERR ReadCallForward(HWND hWnd, UINT uMsg, short step, 
		int nReason, int nClass = 0, LONG lExt = 0);
	CAERR WriteCallForward(HWND hWnd, UINT uMsg, short step, 
		const CFSTRUCT &cf_info, LONG lExt = 0);

	CAERR ReadCallWaiting(HWND hWnd, UINT uMsg, short step = 0, 
		int nClass = 0, LONG lExt = 0);
	CAERR WriteCallWaiting(HWND hWnd, UINT uMsg, short step, 
		BOOL bEnable, int nClass = 0, LONG lExt = 0);

#ifdef _CALL_COST_SUPPORT_
	CAERR GetCallMeter(HWND hWnd, UINT uMsg, short step = 0, LONG lExt = 0);
	CAERR GetAcmValue(HWND hWnd, UINT uMsg, short step = 0, LONG lExt = 0);
	CAERR ResetAcmValue(HWND hWnd, UINT uMsg, short step,
		const CString &strPwd, LONG lExt = 0);

	CAERR ReadAcmMax(HWND hWnd, UINT uMsg, short step = 0, LONG lExt = 0);
	CAERR WriteAcmMax(HWND hWnd, UINT uMsg, short step, 
		int nMax, const CString &strPwd, LONG lExt = 0);

	CAERR GetPucValue(HWND hWnd, UINT uMsg, short step = 0, LONG lExt = 0);
	CAERR SetPucValue(HWND hWnd, UINT uMsg, short step, 
		const CString &currency, const CString &ppu, 
		const CString &strPwd, LONG lExt = 0);
#endif	// _CALL_COST_SUPPORT_

	CAERR QueryOperator(HWND hWnd, UINT uMsg, short step, int format = 2, LONG lExt = 0);

	CAERR GetVersionInfo(HWND hWnd, UINT uMsg, short step = 0, LONG lExt = 0);
    
/************ 20131109 by Zhuwei **********************************************/
    CAERR GetVERNO(HWND hWnd, UINT uMsg, short step = 0, LONG lExt = 0);
/******************************************************************************/
    
    CAERR QueryIMEI(HWND hWnd, UINT uMsg, short step = 0, LONG lExt = 0);
	CAERR QueryIMSI(HWND hWnd, UINT uMsg, short step = 0, LONG lExt = 0);
	CAERR QuerySimOwner(HWND hWnd, UINT uMsg, short step = 0, LONG lExt = 0);
	CAERR SetModemAPN(HWND hWnd, UINT uMsg, short step, 
		const CString& strApn, LONG lExt = 0);
	CAERR QueryPSStatus(HWND hWnd, UINT uMsg, short step = 0, LONG lExt = 0);

//	CAERR UpdateTime(HWND hWnd, UINT uMsg, short step = 0, LONG lExt = 0);

#ifdef _USSD_SUPPORT_
	CAERR UssdControl(HWND hWnd, UINT uMsg, short step, int n,
		const CString &strReq, LONG lExt = 0);
#endif

	CAERR SendSMS(HWND hWnd, UINT uMsg, short step, 
		SMSMessage *pSms, LONG lExt = 0);
	CAERR ReadSMS(HWND hWnd, UINT uMsg, short step, 
		SMSMessage *pSms, const CString& strMem, 
		int nIdx, LONG lExt = 0);
	CAERR WriteSMS(HWND hWnd, UINT uMsg, short step, 
		SMSMessage *pSms, const CString& strMem, 
		LONG lExt = 0);
	CAERR EraseSMS(HWND hWnd, UINT uMsg, short step, 
		SMSMessage *pSms, LONG lExt = 0);
	CAERR ReplaceSMS(HWND hWnd, UINT uMsg, short step, 
		SMSMessage *pSms, LONG lExt = 0);
	CAERR SetSMSMem3(HWND hWnd, UINT uMsg, short step, 
		const CString& strMem1, LONG lExt = 0);
	CAERR GetSMSMem3(HWND hWnd, UINT uMsg, short step, LONG lExt = 0);
	CAERR ConfigSMS(HWND hWnd, UINT uMsg, short step, LONG lExt = 0);
	CAERR GetSMSNumber(HWND hWnd, UINT uMsg, short step, 
		const CString& strMem, LONG lExt = 0);
	CAERR SetSMSC(HWND hWnd, UINT uMsg, short step, 
		const CString &sc, LONG lExt = 0);

#ifdef _CBS_SUPPORT_
	CAERR GetCBSInfo(HWND hWnd, UINT uMsg, short step, 
		BOOL bETWS = FALSE, const CString& channelID = _T(""), LONG lExt = 0);
	CAERR SetCBMode(HWND hWnd, UINT uMsg, short step, BOOL bOn, 
		const CString& dcs, const  CString& channel, BOOL bAllLang, LONG lExt = 0);
#endif

#ifdef _ETWS_SUPPORT_
	CAERR GetETWSInfo(HWND hWnd, UINT uMsg, short step, LONG lExt = 0);
	CAERR SetETWSInfo(HWND hWnd, UINT uMsg, short step, BOOL bAbleEtws, 
		BOOL bSecurityCheck, BOOL bRecvTest, LONG lExt = 0);
#endif

	CAERR ReadPBK(HWND hWnd, UINT uMsg, short step, 
		PhoneBookItem *pPbk, const CString& strMem, 
		int nIdx, LONG lExt = 0);
	CAERR WritePBK(HWND hWnd, UINT uMsg, short step, 
		PhoneBookItem *pPbk, const CString& strMem, 
		LONG lExt = 0);
#ifdef _VCARD_SUPPORT_
	CAERR ReadAdvPBK(HWND hWnd, UINT uMsg, short step, 
		PhoneBookItem *pPbk, LONG lExt = 0);
#endif
	CAERR ErasePBK(HWND hWnd, UINT uMsg, short step, 
		PhoneBookItem *pPbk, LONG lExt = 0);
	CAERR ReplacePBK(HWND hWnd, UINT uMsg, short step, 
		PhoneBookItem *pPbk, const PhoneBookItem& newItem, 
		LONG lExt = 0);
	CAERR GetPBKNumber(HWND hWnd, UINT uMsg, short step, 
		const CString& strMem, LONG lExt = 0);

#ifdef _STK_SUPPORT_
	// STK
	CAERR STKResponse(HWND hWnd, UINT uMsg, short step, 
		const CString &strCmd, LONG lExt = 0);
	CAERR STKEnvelope(HWND hWnd, UINT uMsg, short step, 
		const CString &strCmd, LONG lExt = 0);
	CAERR STKSetupCall(HWND hWnd, UINT uMsg, short step, 
		int option, LONG lExt = 0);
	CAERR STKSetupSS(HWND hWnd, UINT uMsg, short step, 
		int option, LONG lExt = 0);
	CAERR STKSetupUSSD(HWND hWnd, UINT uMsg, short step, 
		int option, LONG lExt = 0);
	CAERR STKSendSMS(HWND hWnd, UINT uMsg, short step, 
		int option, LONG lExt = 0);
	CAERR STKSendDTMF(HWND hWnd, UINT uMsg, short step, 
		int option, LONG lExt = 0);
	CAERR STKRunAtCmd(HWND hWnd, UINT uMsg, short step, 
		int option, LONG lExt = 0);
	CAERR GetSIMImage(HWND hWnd, UINT uMsg, short step, 
		int imgId, LONG lExt = 0);
#endif	// _STK_SUPPORT_

	CAERR SetNetworkType(HWND hWnd, UINT uMsg, short step, 
		int option, LONG lExt = 0);
	
	CAERR GetOperList(HWND hWnd, UINT uMsg, 
		short step = 0, LONG lExt = 0);

	CAERR SwitchDbgPort(HWND hWnd, UINT uMsg, short step, 
		BOOL bFlag, LONG lExt = 0);

	CAERR SwitchSDLogging(HWND hWnd, UINT uMsg, short step, 
		BOOL bFlag, LONG lExt = 0);

	CAERR SelectOperator(HWND hWnd, UINT uMsg, short step, 
		int mode, const CString &oper = _T(""), LONG lExt = 0);
	
	CAERR QueryGprsState(HWND hWnd, UINT uMsg, short step, LONG lExt = 0);

	CAERR UnblockPIN(HWND hWnd, UINT uMsg, short step, 
		int facility, LPCTSTR puk, LPCTSTR pin, LONG lExt = 0);

	CAERR PowerOff(HWND hWnd, UINT uMsg, short step = 0, LONG lExt = 0);
	
    
// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CComAgent)
	//}}AFX_VIRTUAL

// Implementation
public:
	virtual ~CComAgent();

	// Generated message map functions
protected:
	//{{AFX_MSG(CComAgent)
	afx_msg LRESULT OnPolling(WPARAM wParam, LPARAM lParam);
	afx_msg LRESULT OnUrcResp(WPARAM wParam, LPARAM lParam);
	afx_msg LRESULT OnRequest(WPARAM wParam, LPARAM lParam);
	afx_msg void OnTimer(UINT nIDEvent);
	afx_msg void OnClose();
	//}}AFX_MSG

	BOOL Reset(void);
	BOOL Start(void);
	BOOL Create(CWnd *pParent); 

//	BOOL SetTimer(UINT nIDEvent, UINT nElapse);

	BOOL ResetPolling(UINT uID, UINT nElapse = 0);
	BOOL StopPolling(UINT uID);

	void DeregisterURC(void);

	static UINT CALLBACK RequestProc(void *p);
	static void UrcCallback(void * pData);

	void ResetUrc(UINT uID);

	CBaseReq * PopRequest(void);

	int Execute(CBaseReq *pReq);
	int Polling(UINT uID);

	CAERR HighRequest(CBaseReq *pReq);
	CAERR NormalRequest(CBaseReq *pReq);
	CAERR PollingRequest(UINT uID);

	BOOL AbortPolling(UINT uID);
//	BOOL AbortRequest(REQHandler proc, HWND hWnd = NULL);
	void AbortAll(void);
	
// 	void ClearRequests(void);

	void SetSysState(WORD wState, WORD wInfo = 0);
	void SetIndicator(WORD eind, WORD creg);

	void SetCallInd(const CCallList& calls);

	BOOL Respond(const CBaseReq *pReq, int err_code, CResponse * pResp = NULL);


	CAERR PowerOn(HWND hWnd, UINT uMsg, short step, 
		BOOL bRF = TRUE, LONG lExt = 0);
	
	CAERR InitTarget0(HWND hWnd, UINT uMsg, short step = 0, LONG lExt = 0);
	CAERR InitTarget1(HWND hWnd, UINT uMsg, short step = 0, LONG lExt = 0);
	CAERR InitTarget2(HWND hWnd, UINT uMsg, short step = 0, LONG lExt = 0);

	CAERR QuerySIM(HWND hWnd, UINT uMsg, short step = 0, LONG lExt = 0);
	CAERR QueryInd(HWND hWnd, UINT uMsg, short step = 0, LONG lExt = 0);

	CAERR QueryCall(HWND hWnd, UINT uMsg, short step = 0, LONG lExt = 0);


	//////////////////////////////////////////////////////////////////////////
	BOOL InitConnection(const CString& port, ME_Callback reply);
	int  TestConnection(void);
	CResponse * QueryCall(int&);
	//////////////////////////////////////////////////////////////////////////

	DECLARE_MESSAGE_MAP()

protected:
	enum user_msg_t
	{
		WM_POLLING = WM_USER+101,	// polling result message
		WM_REQUEST,					// internal request
		WM_URCRESP					// internal response to urc
	};
	
	// Internal request
	enum internal_req_t
	{
		IR_POWERON	= 0,	// power on
		IR_INIT0	= 10,	// init target, 1st stage
		IR_INIT1	= 20,	// init target, 2nd stage
		IR_INIT2	= 26,	// init target, 3rd stage
		IR_QUERYSIM	= 30,	// query sim
		IR_QUERYPIN	= 40,	// query pin
		IR_QUERYIND	= 50,	// query eind
		IR_POWEROFF	= 60,	// power off
		IR_SIMRESET = 70	// sim reset
	};
	
	enum polling_id_t
	{
		PL_CMUXINIT = 0,
		PL_CLCC,		// polling, at+clcc

		PL_CONN,		// polling, at

		PL_MAX
	};

	
	int					m_nSysState;

	CUrcUnit			m_UrcList[URC_MAX];

	CRequestQue			m_HighQ;
	CRequestQue			m_NormalQ;
	BOOL				m_PollingQ[PL_MAX];	// not a real queue

	HANDLE				m_hEvtStart;
	HANDLE				m_hEvtStop;
	HANDLE				m_hThread;
	CRITICAL_SECTION	m_theLock;

	CString				m_port;

	static Connection	m_conx;
	static CComAgent *	m_pSelf;
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_COMAGENT_H__34AD2023_C4C1_43EF_BF3D_8C0780AC88E3__INCLUDED_)
