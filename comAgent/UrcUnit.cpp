/*****************************************************************************
*  Copyright Statement:
*  --------------------
*  This software is protected by Copyright and the information contained
*  herein is confidential. The software may not be copied and the information
*  contained herein may not be used or disclosed except with the written
*  permission of MediaTek Inc. (C) 2012
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
*   UrcUnit.cpp
*
* Project:
* --------
*   DCT
*
* Description:
* ------------
*   URC management module
*
* Author:
* -------
*   Xiantao Han
*
*******************************************************************************/

// UrcUnit.cpp: implementation of the CUrcUnit class.
//
//////////////////////////////////////////////////////////////////////

#ifdef WIN32
#include "stdafx.h"
#else
#include "Win2Mac.h"
#endif

#include "UrcUnit.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

#define LOCK()		EnterCriticalSection(&m_theLock)
#define UNLOCK()	LeaveCriticalSection(&m_theLock)

int CUrcUnit::m_nCount = 0;
CRITICAL_SECTION CUrcUnit::m_theLock;

CUrcUnit::CUrcUnit()
{
	if (0 == m_nCount)
	{
		InitializeCriticalSection(&m_theLock);
	}

	m_uID = m_nCount++;

	ASSERT(m_uID < URC_MAX);

	m_pWorker = CUrcWorker::Create(m_uID);
	m_pValue = m_pWorker->Alloc();
}

CUrcUnit::~CUrcUnit()
{
	m_Clients.clear();
	
	-- m_nCount;
	
	if (m_nCount <= 0)
	{
		DeleteCriticalSection(&m_theLock);
	}

	m_pValue->release();

	delete m_pWorker;
}

BOOL CUrcUnit::Register(HWND hWnd, UINT uMsg)
{
	WINCLIENT client = {hWnd, uMsg};

	LOCK();

	m_Clients.push_back(client);
	
	UNLOCK();

	Unicast(hWnd, uMsg);
	
	return TRUE;
}

void CUrcUnit::Deregister(void)
{
	LOCK();
	m_Clients.clear();
	UNLOCK();
}

void CUrcUnit::Deregister(HWND hWnd)
{
	LOCK();
	
	ClientList::iterator it = m_Clients.begin();

	while (it != m_Clients.end())
	{
		if (it->hWnd == hWnd)
		{
			m_Clients.erase(it);
			break;
		}
		++ it;
	}

	UNLOCK();
}

BOOL CUrcUnit::Reset(void)
{
	ASSERT(m_pValue != NULL);

	if (m_pWorker->IsEvent())
		return FALSE;
	
//	if (NULL == m_pValue)
//		return NULL;
	
	LOCK();
	
	BOOL flag = m_pValue->Reset();

	UNLOCK();
	
	if (flag)
	{
		Broadcast();
	}

	return flag;
}

// set new value
BOOL CUrcUnit::Decode(const ATResult * pResult)
{
	int choice = -1;

	ASSERT (NULL != m_pValue);
	
	LOCK();

	choice = m_pWorker->Decode(pResult, m_pValue);

	UNLOCK();
	
	if (choice == 1 || (choice == 0 && m_pWorker->IsEvent()))
	{
		Broadcast();
	}

	return (choice != -1);
}

// notify the new client, though the value is not changed at the time
BOOL CUrcUnit::Unicast(HWND hWnd, UINT uMsg) const
{
	// the value is still unknown if (m_pValue is default)
	if (!m_pValue->IsDefault() && 
		!m_pWorker->IsEvent() )
	{
		CUrcValue *p = m_pWorker->Alloc(m_pValue);
	//	if (p != NULL) p->retain();
		PostMessage(hWnd, uMsg, m_uID, (LPARAM)p);
	//	if (p != NULL) p->release();
		return TRUE;
	}
	
	return FALSE;
}

// notify users, may be called in another thread
BOOL CUrcUnit::Broadcast(void) const
{
	LOCK();

	CUrcValue *p = m_pWorker->Alloc(m_pValue);
	
	ClientList::const_iterator it = m_Clients.begin();

	while (it != m_Clients.end())
	{
		p->retain();

		PostMessage(
			it->hWnd, 
			it->uMsg, 
			(WPARAM)m_uID, 
			(LPARAM)p
			);

		++ it;
	}
	
	p->release();

	UNLOCK();
	
	return TRUE;
}

//////////////////////////////////////////////////////////////////////////

int CUrcWorker::getEind(const ATResult* pResult)
{
	if (pResult->check_key("+EIND"))
		return pResult->get_integer(0, 1);
	else
		return -1;
}

WORD CUrcWorker::convCreg(WORD creg, WORD stat)
{
	// if the status "PINREADY" is skipped,
	// some of the initialization will be missed.
	if (stat < SYS_PINREADY)
	{
		return stat;
	}

	switch (creg)
	{
	case 0:
	case 3:
	case 4:	// UNKNOWN
		return SYS_NONETWORK;
		
	case 1:
		return SYS_NETHOME;
		
	case 2:
		return SYS_NETSEARCH;
		
	case 5:
		return SYS_NETROAMING;
		
	default:
		break;
	}
	
	return stat;
}

#define CreateConcreteUrc(id)	\
	case id: return new CUrcConcreteWorker<id>

CUrcWorker *CUrcWorker::Create(UINT uID)
{
	switch(uID)
	{
	CreateConcreteUrc(URC_ECSQ);
	CreateConcreteUrc(URC_SYS);
	CreateConcreteUrc(URC_RING);	
	CreateConcreteUrc(URC_NOCA);	
	CreateConcreteUrc(URC_CMTI);
	CreateConcreteUrc(URC_CUSD);	
	CreateConcreteUrc(URC_CCWA);	
	CreateConcreteUrc(URC_CCCM);
	CreateConcreteUrc(URC_CCWV);
//	CreateConcreteUrc(URC_CLIP);
//	CreateConcreteUrc(URC_COLP);
	CreateConcreteUrc(URC_CSSI);
	CreateConcreteUrc(URC_CSSU);
	CreateConcreteUrc(URC_CLCC);
	CreateConcreteUrc(URC_SMS);
	CreateConcreteUrc(URC_PBK);
	CreateConcreteUrc(URC_CGREG);
	CreateConcreteUrc(URC_ERROR);
	CreateConcreteUrc(URC_STKPCI);
	CreateConcreteUrc(URC_PSBEARER);
	CreateConcreteUrc(URC_CBM);
	CreateConcreteUrc(URC_ETWS);
	CreateConcreteUrc(URC_CIEV);

	default:
		ASSERT(false);
		return NULL;
	}
}
