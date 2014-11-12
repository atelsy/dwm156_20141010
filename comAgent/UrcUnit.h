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
*   UrcUnit.h
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

// UrcUnit.h: interface for the CUrcUnit class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_URCUNIT0_H__6E3BB62B_63CA_4379_82AE_D331062E4536__INCLUDED_)
#define AFX_URCUNIT0_H__6E3BB62B_63CA_4379_82AE_D331062E4536__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include <list>
#include "CallInfo.h"
#include "Types.h"

//////////////////////////////////////////////////////////////////////////
// urc mapping
// urc codes must be zero-based and continuous
// the most frequently-received urc should be place in front
enum urc_code_t
{
	URC_ECSQ = 0,
	URC_SYS,		// KEY: "+creg:", "+eind: 128"	// ***
	URC_RING,
	URC_NOCA,
	URC_CMTI,
	
	URC_CCWA,
	URC_CSSI,
	URC_CSSU,
	URC_SMS,		// KEY: "+eind: 1"
	URC_PBK,		// KEY: "+eind: 2"

	URC_CGREG,
	URC_PSBEARER,
	
	URC_CUSD,		// +cusd:
	URC_ERROR,		// +cme error:
	
	URC_STKPCI,

//	URC_CLIP,
//	URC_COLP,
	URC_CCCM,
	URC_CCWV,
	URC_CLCC,		// implemented by polling
	
	URC_CBM,		// +cbm
	URC_ETWS,		// +ETWS
	URC_CIEV,

	URC_MAX
};


//////////////////////////////////////////////////////////////////////////
// urc data, all types of urc data should derive from it
class CUrcValue : public CHeapObject
{
public:
	CUrcValue() : m_bDefault(TRUE)
	{
	}

	virtual ~CUrcValue()
	{
	}

public:
	
/*
	CUrcValue * Clone(void)
	{
		retain();
		return this;
	}
*/

	virtual BOOL Reset() = 0;

	BOOL IsDefault(void) const
	{
		return m_bDefault;
	}

	
protected:
	BOOL m_bDefault;
};


template <class _Tx>
class CUrcType : public CUrcValue
{
public:
	CUrcType()
	{
	}

	CUrcType(const CUrcType &ref)
	{
		m_value = ref.m_value;
		m_bDefault = ref.m_bDefault;
	}

	explicit CUrcType(const _Tx &ref)
	{
		m_value = ref;
		m_bDefault = FALSE;
		m_uCount = 1;
	}
	
	// if returns true, notify
	BOOL SetValue(const _Tx& value = _Tx());

	const _Tx& Value(void) const
	{
#ifdef _DEBUG
		ASSERT(typeid(*this) == typeid(CUrcType));
		ASSERT(m_isWatched > 0);
#endif
		return m_value;
	}

// 	virtual CUrcValue * Clone(void) const
// 	{
// 		return (new CUrcType(*this));
// 	}

	virtual BOOL Reset();

protected:
	inline _Tx Default()
	{
		return _Tx();
	}

protected:
	_Tx		m_value;

};


typedef CUrcType<DualWord>		CUrcSys;

typedef CUrcType<int>			CUrcInt;		// URC data type is integer

typedef CUrcType<BOOL>			CUrcFlag;		// URC data type is TRUE/FALSE

//typedef CUrcType<CString>		CUrcText;		// URC data type is string

typedef CUrcType<CSsnCode>		CUrcCssn;		// +cssi, +cssu

typedef CUrcType<CMsgInd>		CUrcMsg;		// +cmti

typedef CUrcType<CMsgInd>		CUrcStk;		// +stkcpi

typedef CUrcType<CUssdCode>		CUrcUssd;		// +cusd

typedef CUrcType<CCallList>		CUrcCalls;		// +clcc

typedef CUrcType<DualWord>		CUrcBearer;		// +psbearer

typedef CUrcType<CCbMsgInd>		CUrcCbMsg;		// +cbm

typedef CUrcType<CEtwsMsgInd>	CUrcEtwsMsg;	// +etws

typedef CUrcType<CMsgStatusInd>	CUrcMsgStatus;   // +CIEV


class CUrcEmpty : public CUrcFlag		// URC data is empty
{
public:
	virtual BOOL SetValue(const BOOL & /*dummy*/)
	{
		m_bDefault = FALSE;
		return TRUE;
	}

// 	virtual CUrcValue * Clone(void) const
// 	{
// 		return NULL;
// 	}
};

//////////////////////////////////////////////////////////////////////////
class CUrcWorker
{
public:
//	CUrcWorker();
	virtual ~CUrcWorker()
    {
    }
	
public:
	static CUrcWorker *Create(UINT uID);

	virtual CUrcValue *Alloc(const CUrcValue* = NULL) const = 0;

	virtual BOOL IsEvent() const
	{
		return TRUE;
	}
	
	/*
	return value:
		-1: AT result is not of the URC
		 0: AT result is wanted but value not changed
		 1: AT result is wanted and value is changed
	*/
	virtual int Decode(const ATResult *, CUrcValue*) const = 0;

public:
	static int	getEind(const ATResult* pResult);
	static WORD	convCreg(WORD creg, WORD stat);
};

template <int _code>
class CUrcConcreteWorker : public CUrcWorker
{
	// dummy class:
	// this class should never be used
	// but use the specializations all the time
};

//////////////////////////////////////////////////////////////////////////

class CUrcUnit
{
public:
	CUrcUnit();
	~CUrcUnit();

public:

	template <typename _Tx>
	BOOL Fill(const _Tx &val)
	{
		BOOL flag = FALSE;
		
		ASSERT(NULL != m_pValue);
		ASSERT(typeid(*m_pValue)==typeid(CUrcType<_Tx>));
		
		EnterCriticalSection(&m_theLock);
		
		flag = static_cast<CUrcType<_Tx>*>(m_pValue)->SetValue(val);
		
		LeaveCriticalSection(&m_theLock);
		
		if (flag || m_pWorker->IsEvent())
		{
			Broadcast();
		}
		
		return flag;
	}
	
	BOOL Register(HWND hWnd, UINT uMsg);
	
	void Deregister(void);
	void Deregister(HWND hWnd);
	
	BOOL Reset(void);

	BOOL Decode(const ATResult *pResult);

private:
	typedef std::list<WINCLIENT> ClientList;

private:
	BOOL Broadcast(void) const;
	
	BOOL Unicast(HWND hWnd, UINT uMsg) const;
	
private:
	static int				m_nCount;	// instance counter
	static CRITICAL_SECTION	m_theLock;
	
	UINT					m_uID;		// the unique ID of the urc
	
	CUrcValue *				m_pValue;	// current value of the urc
	CUrcWorker *			m_pWorker;
	
	ClientList				m_Clients;	// client list

};

#include "UrcUnit.inl"


#endif // !defined(AFX_URCUNIT0_H__6E3BB62B_63CA_4379_82AE_D331062E4536__INCLUDED_)
