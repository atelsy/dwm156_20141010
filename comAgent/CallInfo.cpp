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
*   CallInfo.cpp
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

// CallInfo.cpp: implementation of the CCall class.
//
//////////////////////////////////////////////////////////////////////
#ifdef WIN32
#include "StdAfx.h"
#include "customize.h"
#else
#include "Win2Mac.h"
#include "customize.h"
#endif

#include "CallInfo.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

#define TYPE_INTERNATIONAL 145


static BOOL CompareNum(const CString& strNum, const CString& strRef)
{
	_Size_T nLen1 = strNum.GetLength();
	_Size_T nLen2 = strRef.GetLength();

	if (nLen1 <= 0 || nLen2 <= 0)
		return (nLen1 == nLen2);

	_Size_T i1 = 0, i2 = 0;

	if (strNum[(_Size_T)0] == _T('+'))
		i1 ++;
	if (strRef[(_Size_T)0] == _T('+'))
		i2 ++;

	if (nLen1 - i1 != nLen2 - i2)
		return FALSE;

	while (i1 < nLen1 && i2 < nLen2)
	{
		if (strNum[i1] != strRef[i2])
			return FALSE;
		i1 ++;
		i2 ++;
	}

	return TRUE;
}

//////////////////////////////////////////////////////////////////////////

CCall::CCall()
{
	idx = -1;
	dir = MXC;
	status = DROPPED;
	mode = -1;
	mpty = 0;
	type = -1;
	
	m_tmConnect = 0;
	m_Duration = 0;

	m_tmStart = CTime::GetCurrentTime();

	m_pDtmf = NULL;
}

CCall::CCall(const CALL& call)
{
	idx = call.idx;
	dir = call.dir;
	status = call.status;
	mode = call.mode;
	mpty = call.mpty;
	number = call.number;
	type = call.type;
	alpha = call.alpha;

	if (TYPE_INTERNATIONAL == type)	// international
	{
		if (number.GetAt(0) != _T('+'))
			number = _T("+") + number;
	}

	m_tmStart = CTime::GetCurrentTime();

	if (IsConnected())
	{
		m_tmConnect = CTime::GetCurrentTime();
	}
	
	m_pDtmf = NULL;
}

CCall::CCall(const CCall& call)
{
	idx = call.idx;
	dir = call.dir;
	status = call.status;
	mode = call.mode;
	mpty = call.mpty;
	number = call.number;
	type = call.type;
	alpha = call.alpha;

	m_tmConnect = call.m_tmConnect;
	m_Duration = call.m_Duration;
	m_tmStart = call.m_tmStart;
	
	if (NULL != call.m_pDtmf)
		m_pDtmf = new CString(*(call.m_pDtmf));
	else
		m_pDtmf = NULL;
}

CCall::CCall(const CCall* pCall)
{
	if (NULL != pCall)
	{
		idx = pCall->idx;
		dir = pCall->dir;
		status = pCall->status;
		mode = pCall->mode;
		mpty = pCall->mpty;
		number = pCall->number;
		type = pCall->type;
		alpha = pCall->alpha;
		
		m_tmConnect = pCall->m_tmConnect;
		m_Duration = pCall->m_Duration;
		m_tmStart = pCall->m_tmStart;
	}
	else
	{
		idx = -1;
		dir = MXC;
		status = DROPPED;
		mode = -1;
		mpty = 0;
		type = -1;
		
		m_Duration = 0;
		m_tmStart = CTime::GetCurrentTime();
	}
	
	m_pDtmf = NULL;
}

CCall::~CCall()
{
	if (NULL != m_pDtmf)	
		delete m_pDtmf;
}

BOOL CCall::SetDtmf(const CString &strDtmf) const
{
	if (strDtmf.IsEmpty())
	{
		if (NULL != m_pDtmf &&
			&strDtmf != m_pDtmf)
		{
			delete m_pDtmf;
			m_pDtmf = NULL;
		}
		return TRUE;
	}

	if (NULL == m_pDtmf)
	{
		m_pDtmf = new CString(strDtmf);
	}
	else
	{
		*m_pDtmf = strDtmf;
	}

	return (NULL != m_pDtmf);
}

BOOL CCall::IsEmergency(const CString& strNum)
{
	const static TCHAR * szEmNums[] =
	{
		EMERGENCY_NUMBERS,
		NULL
	};

	int i = 0;

	while (szEmNums[i] != NULL)
	{
		if (CompareNum(strNum, szEmNums[i]))
			return TRUE;
		i ++;
	}
	
	return FALSE;
}

BOOL CCall::IsEmergency(void) const
{
	return IsEmergency(number);
}

BOOL CCall::IsOutgoing(void) const
{
	return (DIALING == status || ALERTING == status);
}

BOOL CCall::IsIncoming(void) const
{
	return (INCOMING == status);
}

BOOL CCall::IsWaiting(void) const
{
	return (WAITING == status);
}

BOOL CCall::IsRinging(void) const
{
	return (IsWaiting() || IsIncoming());
}

BOOL CCall::IsHangUp(void) const
{
	return (DROPPED == status);
}

BOOL CCall::IsMpty(void) const
{
	return (0 != mpty);
}

BOOL CCall::IsActive(void) const
{
	return (ACTIVE == status);
}

BOOL CCall::IsHeld(void) const
{
	return (HELD == status);
}

BOOL CCall::IsConnected(void) const
{
	if (IsActive() || IsHeld())
		return TRUE;

	if (IsEmergency() && IsOutgoing())
		return TRUE;

	return FALSE;
}

BOOL CCall::IsMoCall(void) const
{
	return (MOC == dir);
}

BOOL CCall::IsMtCall(void) const
{
	return (MTC == dir);
}

const CString& CCall::GetAlpha(void) const
{
	if (alpha.IsEmpty())
		return number;

	return alpha;
}

void CCall::SetDuration(const CTimeSpan& dura)
{
	m_Duration = dura;
	m_tmConnect = CTime::GetCurrentTime() - dura;
}

#ifndef IDS_TIME_FORMAT
#define IDS_TIME_FORMAT _T("%d:%02d:%02d")
#endif

BOOL CCall::GetDuration(CString& strDura) const
{
	if (!IsConnected())
	{
		strDura.Format(IDS_TIME_FORMAT, 0, 0, 0);
		return FALSE;
	}

	strDura.Format(IDS_TIME_FORMAT, 
		m_Duration.GetHours(),
		m_Duration.GetMinutes(),
		m_Duration.GetSeconds());

	return TRUE;
}

const CTimeSpan * CCall::UpdateDuration(void)
{
	if (!IsConnected())
		return NULL;

//	m_Duration = CTime::GetCurrentTime() - m_tmConnect;
	m_Duration += CTimeSpan(0, 0, 0, 1);

	return &m_Duration;
}

BOOL CCall::operator== (const CCall& call) const
{
	if (!Alike(&call))
		return FALSE;

	if (status != call.status)
		return FALSE;
	if (mpty != call.mpty)
		return FALSE;
	if (number != call.number)
		return FALSE;

	return TRUE;
}

BOOL CCall::operator!= (const CCall& call) const
{
	return !(*this == call);
}

BOOL CCall::Alike(const CCall *pCall) const
{
	if (NULL == pCall)
		return FALSE;

	if (idx != pCall->idx)
		return FALSE;
	if (dir != pCall->dir)
		return FALSE;
	if (mode != pCall->mode)
		return FALSE;

	return TRUE;
}

const CCall& CCall::operator= (const CCall& call)
{
	if (&call == this)
		return *this;

	idx = call.idx;
	dir = call.dir;
	status = call.status;
	mode = call.mode;
	mpty = call.mpty;
	type = call.type;
	number = call.number;
	alpha = call.alpha;
	
	m_Duration = call.m_Duration;
	
	m_tmConnect = call.m_tmConnect;

	if (call.m_tmStart < m_tmStart)
		m_tmStart = call.m_tmStart;
	
	if (NULL != m_pDtmf)
		delete m_pDtmf;

	if (NULL != call.m_pDtmf)
		m_pDtmf = new CString(*(call.m_pDtmf));
	else
		m_pDtmf = NULL;

	return *this;
}

const CCall& CCall::operator= (const CALL& call)
{
	idx = call.idx;
	dir = call.dir;
	status = call.status;
	mode = call.mode;
	mpty = call.mpty;
	type = call.type;
	number = call.number;
	alpha = call.alpha;
	
	if (TYPE_INTERNATIONAL == type)	// international
	{
		if ((!number.IsEmpty()) && 
			(number.GetAt(0) != _T('+')) )
		{
			number = _T("+") + number;
		}
	}

	if (IsConnected())
		m_tmConnect = CTime::GetCurrentTime();

	m_Duration = 0;
	m_tmStart = CTime::GetCurrentTime();
		
	return *this;
}

void CCall::UpdateInfo(const CCall& call)
{
	if (call.IsConnected())
	{
		if (!IsConnected())
			m_tmConnect = CTime::GetCurrentTime();
	}
	
	idx = call.idx;
	dir = call.dir;
	status = call.status;
	mode = call.mode;
	mpty = call.mpty;
	type = call.type;
	number = call.number;

	if (alpha.IsEmpty())
		alpha = call.alpha;
	
	if (m_tmStart > call.m_tmStart)
		m_tmStart = call.m_tmStart;

//	m_Duration = call.m_Duration;
	
//	m_tmConnect = call.m_tmConnect;
}
//////////////////////////////////////////////////////////////////////////

CCallList::CCallList()
{
	m_uStatus = 0;
	m_nMpty = 0;
}

CCallList::~CCallList()
{
	clear();
}

CALLPOS CCallList::GetFirstPos(void) const
{
	return begin();
}

const CCall * CCallList::GetNext(CALLPOS &pos) const
{
	if (pos != end())
	{
		const CCall *p = &(*pos);
		++ pos;
		return p;
	}

	return NULL;
}

CCall * CCallList::Get(short idx) const
{
	const_iterator it = begin();
	while(it != end())
	{
		if ((*it).idx == idx)
			return (CCall*)&(*it);
		++ it;
	}
	return NULL;
}

// to get the latest MO call by number
CCall* CCallList::Get(const CString& strNum) const
{
	CCall *p = NULL;
	CCall *tmp = NULL;
	const_iterator it = begin();

	while(it != end())
	{
		tmp = (CCall*)&(*it);
		if (tmp->IsMoCall())
		{
			if (CompareNum(tmp->number, strNum))
			{
				if (NULL == p || 
					tmp->GetStartTime() > p->GetStartTime())
				{
					p = tmp;
				}
			}
		}
		++ it;
	}

	return p;
}

BOOL CCallList::Pop(CCall* pCall)
{
	if (size() <= 0)
		return FALSE;

	*pCall = front();
	pop_front();

	return TRUE;
}

BOOL CCallList::Erase(short idx)
{
	iterator it = begin();
	while(it != end())
	{
		if ((*it).idx == idx)
		{
			erase(it);
			ReCalcGlobalStatus();
			return TRUE;
		}
		++ it;
	}
	return FALSE;
}

//*
BOOL CCallList::Append(const CCall& call)
{
	if (0 != call.mode)	// only voice calls
		return FALSE;

	push_back(call);
	UpdateGlobalStatus(call);
	return TRUE;
}
//*/

BOOL CCallList::Update(const CCall& call)
{
	CCall * p = Get(call.idx);

	if (NULL == p)
	{
		Append(call);
	}
	else
	{
		if (p->alpha.IsEmpty())
			p->alpha = call.alpha;
		if (call.GetDtmf())
			p->SetDtmf(*(call.GetDtmf()));

		if (call == *p)
			return FALSE;

		p->UpdateInfo(call);
		UpdateGlobalStatus(call);
	}
	
	return TRUE;
}

void CCallList::UpdateGlobalStatus(const CCall& call)
{
	UpdateGlobalStatus(call.status, call.mpty);
	if (call.IsEmergency())
		m_uStatus |= G_EMCALL;
}

void CCallList::UpdateGlobalStatus(short status, short mpty /*= 0*/)
{
	switch (status)
	{
	case CCall::ACTIVE:
		if (mpty == 1)
			m_uStatus |= G_MPTYACTIVE;
		else
			m_uStatus |= G_ACTIVE;
		break;
		
	case CCall::HELD:
		if (mpty == 1)
			m_uStatus |= G_MPTYHELD;
		else
			m_uStatus |= G_HELD;
		break;
		
	case CCall::DIALING:
		m_uStatus |= G_DIALING;
		break;

	case CCall::ALERTING:
		m_uStatus |= G_ALERTING;
		break;

	case CCall::INCOMING:
		m_uStatus |= G_INCOMING;
		break;

	case CCall::WAITING:
		m_uStatus |= G_WAITING;
		break;

	default:
		break;
	}
}

void CCallList::ReCalcGlobalStatus(void)
{
	m_nMpty = 0;
	m_uStatus = 0;
	iterator it = begin();
	while (it != end())
	{
		UpdateGlobalStatus(*it);
		if ((*it).mpty)
			m_nMpty ++;
		++ it;
	}
}

BOOL CCallList::Update(const CCallList& calllist)
{
	CCall *p1, *p2;
	int nCount = 0;
	size_t nTotal = size() + calllist.size();

	for (short i=0; nTotal>0 ; i++)
	{
		p1 = Get(i);
		p2 = calllist.Get(i);

		if (NULL == p1 && NULL == p2)
			continue;
		else if (NULL == p1 && NULL != p2)
		{
			Append(*p2);
			nTotal --;
			nCount ++;
		}
		else if (NULL == p2 && NULL != p1)
		{
			Erase(i);
			nTotal --;
			nCount ++;
		}
		else
		{
			if (p1->alpha.IsEmpty())
				p1->alpha = p2->alpha;

			if (*p1 != *p2)
			{
				p1->UpdateInfo(*p2);
				nCount ++;
			}

			nTotal -= 2;
		}
	}

	ReCalcGlobalStatus();
	return (nCount > 0);
}

void CCallList::UpdateDuration()
{
	iterator it = begin();
	while (it != end())
	{
		(*it).UpdateDuration();
		++ it;
	}
}

/*
// update list and get difference
BOOL CCallList::GetDiff(CCallList& target, const CCallList& ref)
{
	CCall *p1, *p2;
	int nTotal = size() + ref.size();

	target.clear();
	
	for (short i=0; nTotal>0 ; i++)
	{
		p1 = Get(i);
		p2 = ref.Get(i);

		if (NULL == p1 && NULL == p2)
			continue;
		else if (NULL == p1 && NULL != p2)
		{
			Append(*p2);
			target.Append(*p2);
			nTotal --;
		}
		else if (NULL == p2 && NULL != p1)
		{
			p1->status = CCall::DROPPED;
			target.Append(*p1);
			Erase(i);
			nTotal --;
		}
		else
		{
			if (*p1 != *p2)
			{
				p1->UpdateInfo(*p2);
				target.Append(*p1);
			}

			nTotal -= 2;
		}
	}

	ReCalcGlobalStatus();

	return (target.size() > 0);
}
//*/

BOOL CCallList::GetNewCalls(CCallList& target, const CCallList& ref) const
{
	const CCall *p1, *p2;
	const_iterator it = ref.begin();
	
	target.clear();
	
	while (it != ref.end())
	{
		p1 = &(*it);
		p2 = Get(p1->idx);

		if (!(p1->Alike(p2)))
			target.Append(*p1);
		else if (p2->IsActive() || p2->IsHeld())
		{
			if (p1->IsRinging() || p1->IsOutgoing())
				target.Append(*p1);
		}
		++ it;
	}

	return (target.size() > 0);
}

BOOL CCallList::GetLostCalls(CCallList& target, const CCallList& ref) const
{
	const CCall *p1, *p2;
	const_iterator it = begin();
	
	target.clear();
	
	while (it != end())
	{
		p1 = &(*it);
		p2 = ref.Get(p1->idx);

		if (!(p1->Alike(p2)))
			target.Append(*p1);
		else if (p1->IsActive() || p1->IsHeld())
		{
			if (p2->IsRinging() || p2->IsOutgoing())
				target.Append(*p1);
		}
		++ it;
	}

	return (target.size() > 0);
}

const CCall * CCallList::GetCall(short status) const
{
	const_iterator it = begin();
	while (it != end())
	{
		if ((*it).status == status)
			return &(*it);
		++ it;
	}
	return NULL;
}

const CCall * CCallList::GetIncomingCall() const
{
	return GetCall(CCall::INCOMING);
}

const CCall * CCallList::GetWaitingCall() const
{
	return GetCall(CCall::WAITING);
}

const CCall * CCallList::GetRingingCall() const
{
	const CCall *p = GetIncomingCall();
	if (NULL == p)
		p = GetWaitingCall();
	return p;
}

const CCall * CCallList::GetOutgoingCall() const
{
	const CCall *p = GetCall(CCall::DIALING);
	if (NULL == p)
		p = GetCall(CCall::ALERTING);
	return p;
}

BOOL CCallList::operator== (const CCallList& ref) const
{
	CCall *p1, *p2;
	size_t nTotal = size() + ref.size();
	
	for (short i=0; nTotal>0 ; i++)
	{
		p1 = Get(i);
		p2 = ref.Get(i);

		if (NULL != p1 && NULL != p2 && *p1 == *p2)
		{
			nTotal -= 2;
		}
		else
		{
			return FALSE;
		}
	}

	return TRUE;
}

const CCallList& CCallList::operator= (const CCallList& ref)
{
	if (this != &ref)
		Update((CCallList)ref);

	return *this;
}

//////////////////////////////////////////////////////////////////////////
// test funcitons
int CCallList::Status(short idx) const
{
	CCall *p = Get(idx);
	
	if (NULL == p)
		return -1;
	
	return (p->status);
}

BOOL CCallList::IsDialing(short idx /*= 0*/) const
{
	if (0 == idx)
		return ToBool(m_uStatus & G_DIALING);
	else
		return (Status(idx) == CCall::DIALING);
}

BOOL CCallList::IsAlerting(short idx /*= 0*/) const
{
	if (0 == idx)
		return ToBool(m_uStatus & G_ALERTING);
	else
		return (Status(idx) == CCall::ALERTING);
}

BOOL CCallList::IsOutgoing(short idx /*= 0*/) const
{
	return (IsDialing(idx) || IsAlerting(idx));
}

BOOL CCallList::IsIncoming(short idx /*= 0*/) const
{
	if (0 == idx)
		return ToBool(m_uStatus & G_INCOMING);
	else
		return (Status(idx) == CCall::INCOMING);
}

BOOL CCallList::IsWaiting(short idx /*= 0*/) const
{
	if (0 == idx)
		return ToBool(m_uStatus & G_WAITING);
	else
		return (Status(idx) == CCall::WAITING);
}

BOOL CCallList::IsRinging(short idx /*= 0*/) const
{
	return (IsIncoming() || IsWaiting());
}

BOOL CCallList::IsActive(short idx /*= 0*/) const
{
	if (0 == idx)
		return ToBool(m_uStatus & (G_ACTIVE|G_MPTYACTIVE));
	else
		return (Status(idx) == CCall::ACTIVE);
}

BOOL CCallList::IsHeld(short idx /*= 0*/) const
{
	if (0 == idx)
		return ToBool(m_uStatus & (G_HELD|G_MPTYHELD));
	else
		return (Status(idx) == CCall::HELD);
}

BOOL CCallList::IsMptyActive(short idx /*= 0*/) const
{
	if (m_nMpty <= 1)
		return FALSE;

	if (0 == idx)
	{
		return ToBool(m_uStatus & G_MPTYACTIVE);
	}
	else
	{
		CCall *p = Get(idx);
		if (NULL == p)
			return FALSE;

		return (p->status == CCall::ACTIVE && p->mpty == 1);
	}
}

BOOL CCallList::IsMptyHeld(short idx /*= 0*/) const
{
	if (m_nMpty <= 1)
		return FALSE;
	
	if (0 == idx)
	{
		return ToBool(m_uStatus & G_MPTYHELD);
	}
	else
	{
		CCall *p = Get(idx);
		if (NULL == p)
			return FALSE;

		return (p->status == CCall::HELD && p->mpty == 1);
	}
}

BOOL CCallList::IsDataCall(short idx /*= 0*/) const
{
//	if (0 == idx)
		return ToBool(m_uStatus & G_DATACALL);
//	else
//	{
//		return (mode(idx) == 1);
//	}
}

BOOL CCallList::IsEmCall(short idx /*= 0*/) const
{
	if (0 == idx)
		return ToBool(m_uStatus & G_EMCALL);
	else
	{
		CCall *p = Get(idx);
		if (NULL == p)
			return FALSE;

		return p->IsEmergency();
	}
}

BOOL CCallList::IsConnected(short idx /*= 0*/) const
{
	if (IsActive(idx) || IsHeld(idx))
		return TRUE;
	if (IsEmCall(idx) && IsOutgoing(idx))
		return TRUE;
	return FALSE;
}

BOOL CCallList::IsMpty(short idx /*= 0*/) const
{
	return (IsMptyActive(idx) || IsMptyHeld(idx));
}

BOOL CCallList::IsDtmfAvailable(short idx /*= 0*/) const
{
	if (IsActive(idx))
		return TRUE;

	// MAUI_01192432: '112' stays in "dialling"
	if (IsEmCall(idx) && IsOutgoing(idx)/*IsAlerting(idx)*/)
		return TRUE;

	return FALSE;
}

BOOL CCallList::JoinTest(void) const
{
	if (!IsActive())
		return FALSE;
	if (IsMptyHeld())
		return FALSE;

	return ToBool(m_uStatus & G_HELD);
}

// "single" here means one call or one conference call
// so, there may be several calls even when "single"
BOOL CCallList::IsSingle(void) const
{
	return !(IsActive() && IsHeld());
}

BOOL CCallList::IsIdle(void) const
{
	return (m_uStatus == 0);
}

// test functions
//////////////////////////////////////////////////////////////////////////
