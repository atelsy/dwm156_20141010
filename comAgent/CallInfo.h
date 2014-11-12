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
*   CallInfo.h
*
* Project:
* --------
*   DCT
*
* Description:
* ------------
*   Information of calls
*
* Author:
* -------
*   Xiantao Han
*
*******************************************************************************/

// CallInfo.h: interface for the CCall class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_CALL_H__65FE92F4_8C66_4937_BA50_D9A1E11B19C1__INCLUDED_)
#define AFX_CALL_H__65FE92F4_8C66_4937_BA50_D9A1E11B19C1__INCLUDED_

#include "Connection.h"

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

enum call_clear_cause_t
{
	CAUSE_TERMINATED = 512,
	CAUSE_TRANSFERRED,
	CAUSE_DEFLECTED,
	CAUSE_REPLACED,
	CAUSE_FAILURE
};

class CCall : public CALL
{
public:
	CCall();
	CCall(const CCall& call);
	CCall(const CCall* pCall);
	CCall(const CALL& call);
	virtual ~CCall();

	static BOOL IsEmergency(const CString& strNum);

	BOOL IsEmergency(void) const;
	BOOL IsOutgoing(void) const;
	BOOL IsIncoming(void) const;
	BOOL IsWaiting(void) const;
	BOOL IsRinging(void) const;
	BOOL IsHangUp(void) const;
	BOOL IsMpty(void) const;
	BOOL IsActive(void) const;
	BOOL IsHeld(void) const;
	BOOL IsConnected(void) const;

	BOOL IsMoCall(void) const;
	BOOL IsMtCall(void) const;

	BOOL SetDtmf(const CString &strDtmf) const;
	const CString *GetDtmf(void) const { return m_pDtmf; }

	const CString& GetAlpha(void) const;
	
	BOOL GetDuration(CString& strDura) const;
	const CTimeSpan& GetDuration(void) const { return m_Duration; }
	const CTime& GetConnectTime(void) const { return m_tmConnect; }
	const CTime& GetStartTime(void) const { return m_tmStart; }

	void SetDuration(const CTimeSpan& dura);
	const CTimeSpan * UpdateDuration(void);

	void UpdateInfo(const CCall& call);

	BOOL Alike(const CCall *pCall) const;
	BOOL operator== (const CCall& call) const;
	BOOL operator!= (const CCall& call) const;

	const CCall& operator= (const CCall& call);
	const CCall& operator= (const CALL& call);

public:
	enum dir_t { MOC = 0, MTC, MXC	};
	enum stat_t
	{
		ACTIVE = 0, 
		HELD,
		DIALING,
		ALERTING,
		INCOMING,
		WAITING,
		DROPPED
	};

protected:
	CTime		m_tmStart;
	CTime 		m_tmConnect;
	CTimeSpan	m_Duration;	

	mutable CString	*m_pDtmf;
};

//////////////////////////////////////////////////////////////////////////

#include <list>

typedef std::list<CCall> CALLLIST;
typedef CALLLIST::const_iterator CALLPOS;

class CCallList : protected CALLLIST
{
public:
	CCallList();
//	CCallList(const CCallList&);
	virtual ~CCallList();
	
	size_t  Size(void) const { return size(); }
	
	CALLPOS GetFirstPos(void) const;
	const CCall* GetNext(CALLPOS&) const;

	CCall* Get(short idx) const;
	CCall* Get(const CString& strNum) const;

	BOOL Pop(CCall* pCall);

	BOOL Append(const CCall& call);
	BOOL Update(const CCall& call);
	BOOL Update(const CCallList& calllist);

//	BOOL GetDiff(CCallList& target, const CCallList& ref);
//	BOOL GetChangedCalls(CCallList& target, CCallList& ref);
	BOOL GetNewCalls(CCallList& target, const CCallList& ref) const;
	BOOL GetLostCalls(CCallList& target, const CCallList& ref) const;

	const CCall * GetIncomingCall() const;
	const CCall * GetWaitingCall() const;
	const CCall * GetRingingCall() const;
	const CCall * GetOutgoingCall() const;

	BOOL Erase(short idx);
	void Reset(void) { clear(); }
	
	void UpdateDuration(void);
//	const CTimeSpan * UpdateDuration(short idx);
//	BOOL UpdateStat(short idx, short stat);

	UINT GetGlobalStatus(void) const { return m_uStatus; }
	UINT GetMptyCount(void) const { return m_nMpty; }

	BOOL operator== (const CCallList& ref) const;
	const CCallList& operator= (const CCallList& ref);

	void UpdateGlobalStatus(short status, short mpty = 0);
	void ReCalcGlobalStatus(void);

	BOOL IsDialing(short idx = 0) const;
	BOOL IsAlerting(short idx = 0) const;
	BOOL IsOutgoing(short idx = 0) const;
	BOOL IsIncoming(short idx = 0) const;
	BOOL IsWaiting(short idx = 0) const;
	BOOL IsRinging(short idx = 0) const;
	BOOL IsActive(short idx = 0) const;
	BOOL IsHeld(short idx = 0) const;
	BOOL IsMptyActive(short idx = 0) const;
	BOOL IsMptyHeld(short idx = 0) const;
	BOOL IsDataCall(short idx = 0) const;
	BOOL IsEmCall(short idx = 0) const;
	BOOL IsConnected(short idx = 0) const;
	BOOL IsMpty(short idx = 0) const;
	BOOL IsSingle(void) const;
	BOOL IsDtmfAvailable(short idx = 0) const;
	BOOL JoinTest(void) const;
	BOOL IsIdle(void) const;

protected:
	BOOL ToBool(UINT u) const
	{
		return (u) ? TRUE : FALSE;
	}

	int  Status(short idx) const;

	const CCall * GetCall(short status) const;

	void UpdateGlobalStatus(const CCall& call);

public:
	enum call_stat_t
	{
		G_DIALING		= 0x0001,
		G_ALERTING		= 0x0002,
		G_INCOMING		= 0x0004,
		G_WAITING		= 0x0008,
		G_ACTIVE		= 0x0010,
		G_HELD			= 0x0020,
		G_MPTYACTIVE	= 0x0040,	// active conference call
		G_MPTYHELD		= 0x0080,	// held conference call
		G_DATACALL		= 0x0100,	// data call
		G_EMCALL		= 0x0200	// emergency call
	};


protected:
	UINT m_uStatus;	// global status
	int  m_nMpty;	// mpty count

//	mutable const_iterator m_it;

};

#endif // !defined(AFX_CALL_H__65FE92F4_8C66_4937_BA50_D9A1E11B19C1__INCLUDED_)
