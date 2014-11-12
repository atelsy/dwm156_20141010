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
*   Types.h
*
* Project:
* --------
*   DCT
*
* Description:
* ------------
*   Type definitions for ComAgent
*
* Author:
* -------
*   Xiantao Han
*
*******************************************************************************/

// Types.h: interface for the CTypes class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_TYPES_H__7A30F2AB_F376_4A0F_9821_42FBF8F22C27__INCLUDED_)
#define AFX_TYPES_H__7A30F2AB_F376_4A0F_9821_42FBF8F22C27__INCLUDED_

#include "ATResult.h"
#include "CallInfo.h"
#ifdef WIN32
#include "customize.h"
#else
#include "../util/customize.h"
#endif

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000


enum sys_status_t
{
	SYS_NODEV = 0,	// no device
	SYS_DISC,		// disconnected
	SYS_CONN,		// connected
	SYS_CMUX,		// cmux ready
	SYS_ATREADY,	// AT ready
	
	SYS_NOSIM,		// SIM not inserted
	SYS_BADSIM,		// SIM blocked or destroyed
	
	SYS_SIMOK,		// SIM detected
	SYS_PINREQ,		// PIN waiting
	SYS_PUKREQ,		// PUK waiting
	SYS_PIN2REQ,	// PIN2 waiting
	SYS_PUK2REQ,	// PUK2 waiting
	
	SYS_PINREADY,	// PIN ready
	SYS_NONETWORK,	// Registration denied
	SYS_NETSEARCH,	// Searching for network
	SYS_NETREADY,	// network ready
	SYS_NETHOME,	// registered to home network
	SYS_NETROAMING	// registered, roaming
};

enum pin_type_t
{
	PWD_READY	= SYS_PINREADY,
	PWD_PIN		= SYS_PINREQ,
	PWD_PUK		= SYS_PUKREQ,
	PWD_PIN2	= SYS_PIN2REQ,
	PWD_PUK2	= SYS_PUK2REQ
};


// structure of at result and urc data
struct WINCLIENT	// windows call back
{
	HWND hWnd;
	UINT uMsg;
};


// at+ceer result
struct ERROREX
{
	int		cause;
	CString	report;
};

// at+cops? result
struct OPERINFO
{
	int		mode;
	CString	operId;
};

template <class _Tx>
class CHeapPointer
{
public:
	CHeapPointer(_Tx *obj)
	{
#ifdef _DEBUG
		if (obj != NULL)
		{
			++ obj->m_isWatched;
		}
#endif
		m_heapObj = obj;
	}

	~CHeapPointer()
	{
		if (NULL != m_heapObj)
		{
#ifdef _DEBUG
			-- m_heapObj->m_isWatched;
#endif
			m_heapObj->release();
		}
	}

/*
public:
	_Tx& operator*() const
	{
		return *m_heapObj;
	}

	_Tx* operator->() const
	{
		return m_heapObj;
	}
*/

private:
	_Tx * m_heapObj;
};

#define WATCH_POINTER(_ptr)		\
	CHeapPointer<CHeapObject> _watcher((CHeapObject*)(_ptr))

class CHeapObject
{
public:
	CHeapObject()
	{
		m_uCount = 1;
#ifdef _DEBUG
		m_isWatched = 0;
#endif
	}

protected:
	virtual ~CHeapObject()
	{
#ifdef _DEBUG
		ASSERT(m_uCount == 0);
		ASSERT(m_isWatched == 0);
#endif
	}

public:
	UINT retain()
	{
		return ++m_uCount;
	}

	UINT release()
	{
		if (--m_uCount == 0)
		{
			delete this;
			return 0;
		}
		else
		{
			return m_uCount;
		}
	}

protected:
	UINT m_uCount;

#ifdef _DEBUG
public:
	UINT m_isWatched;
#endif

};


class CResponse : public CHeapObject
{
public:
	CResponse() : m_lExt(0)
	{
	}

	CResponse(LONG lExt):m_lExt(lExt)
	{
	}

	virtual ~CResponse()
	{
	}
	
public:
	LONG m_lExt;	// user data, public
};

template <class _Tx>
class CRespType : public CResponse
{
public:
	CRespType(LONG lExt = 0)
		: CResponse(lExt)
	{
	}

	explicit CRespType(const _Tx &val, LONG lExt = 0) 
		: m_value(val), CResponse(lExt)
	{
	}

//	CRespType(LONG lExt) : CResponse(lExt) {}

	virtual ~CRespType()
	{
	}

public:
	const _Tx &Value() const
	{
#ifdef _DEBUG
		ASSERT(typeid(*this) == typeid(CRespType));
		ASSERT(m_isWatched > 0);
#endif
		return m_value;
	}

public:
	_Tx & Data()
	{
		ASSERT(typeid(*this) == typeid(CRespType));
		return m_value;
	}

protected:
	_Tx m_value;
};

enum ssi_code_t
{
	SSI_FORWARD_ALL_ON = 0,
	SSI_FORWARD_PART_ON,
	SSI_CALL_FORWARDED,
	SSI_CALL_WAITING,
	SSI_CUG_CALL,
	SSI_OUTGOING_BARRED,
	SSI_INCOMING_BARED,
	SSI_CLIR_REJECTED,
	SSI_CALL_DEFLECTED
};

enum ssu_code_t
{
	SSU_FORWARDED_CALL = 0,
	SSU_CUG_CALL,
	SSU_CALL_HELD,
	SSU_CALL_RETRIEVED,
	SSU_MPTY_ENTERED,
	SSU_HELDCALL_RELEASED,
	SSU_FORWARD_CHECK,
	SSU_TRANSFER_ALERTING,
	SSU_TRANSFER_CONNECTED,
	SSU_DEFLECTED_CALL,
	SSU_INCOMING_FORWARDED
};

class DualWord
{
public:
	WORD alpha;
	WORD beta;
	
	DualWord() :alpha(0), beta(0)
	{
	}

	DualWord(WORD _alpha, WORD _beta)
		:alpha(_alpha), beta(_beta)
	{
	}

	bool operator==(const DualWord &ref) const
	{
		return (alpha==ref.alpha && beta==ref.beta);
	}

};

class CSsnCode
{
public:
	CSsnCode();
	CSsnCode(short code, short index, short type);
	BOOL operator== (const CSsnCode& ref) const;
	const CSsnCode& operator= (const CSsnCode& ref);

public:
	short code;
	short index;
	short type;
	short unused;

	CString number;
};

enum cbs_code
{
	CBS_ALPHA_GSM,
	CBS_ALPHA_DATA,
	CBS_ALPHA_UCS2
};

class CUssdCode
{
public:
	CUssdCode();
	BOOL operator== (const CUssdCode& ref) const;
	const CUssdCode& operator= (const CUssdCode& ref);

	int Decode(
		const string	&src, 
		unsigned char	dcs);

public:
	short n;
	short dcs;

	CString str_urc;
};

class CMsgStatusInd
{
public:
	CMsgStatusInd();
	BOOL operator== (const CMsgStatusInd& ref) const;
	const CMsgStatusInd& operator= (const CMsgStatusInd& ref);

public:
	int ind;
	CString value;
};

class CMsgInd
{
public:
	CMsgInd();
	BOOL operator== (const CMsgInd& ref) const;
	const CMsgInd& operator= (const CMsgInd& ref);

public:
	int index;
	CString mem;
};

class CCbMsgInd
{
public:
	CCbMsgInd();
	BOOL operator== (const CCbMsgInd& ref)const;
	const CCbMsgInd& operator= (const CCbMsgInd& ref);
	
public:
	int length;
	string pdu;
};

class CEtwsMsgInd
{
public:
	CEtwsMsgInd();
	BOOL operator== (const CEtwsMsgInd& ref)const;
	const CEtwsMsgInd& operator= (const CEtwsMsgInd& ref);
	
public:
	unsigned short warningType;
	unsigned short msgId;
	unsigned short serialNum;
	string plmnId;
	string securityInfo;
};

//////////////////////////////////////////////////////////////////////////

typedef CRespType<int>						CRespInt;
typedef CRespType<BOOL>						CRespFlag;
typedef CRespType<CString>					CRespText;
typedef CRespType<ERROREX>					CRespErrex;
typedef CRespType<CCallList>				CRespCalls;
typedef CRespType<std::vector<int> >		CRespArray;
typedef CRespType<std::vector<CString> >	CRespMulLine;
typedef CRespType<CFSTRUCT>					CRespCFInfo;
typedef CRespType<DualWord>					CRespPinState;
typedef CRespType<DualWord>					CRespIndicate;
typedef CRespType<OPERINFO>					CRespOperInfo;
typedef CRespType<std::vector<OPERINFOEX> >	CRespOperList;
#ifdef _CBS_SUPPORT_
typedef CRespType<CBSInfo>					CRespCBSInfo;
#endif
#ifdef _ETWS_SUPPORT_
typedef CRespType<ETWSInfo>					CRespETWSInfo;
#endif


//////////////////////////////////////////////////////////////////////////

#endif // !defined(AFX_TYPES_H__7A30F2AB_F376_4A0F_9821_42FBF8F22C27__INCLUDED_)
