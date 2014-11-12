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
*   Types.cpp
*
* Project:
* --------
*   DCT
*
* Description:
* ------------
*   Type implementions for ComAgent
*
* Author:
* -------
*   Xiantao Han
*
*******************************************************************************/

// Types.cpp: implementation of the CTypes class.
//
//////////////////////////////////////////////////////////////////////

#ifdef WIN32
#include "StdAfx.h"
#else
#include "Win2Mac.h"
#endif

#include "Types.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CSsnCode::CSsnCode()
{
	code = -1;
	index = -1;
	type = 0;
}

CSsnCode::CSsnCode(short code, short index, short type)
{
	this->code = code;
	this->index = index;
	this->type = type;
}

BOOL CSsnCode::operator== (const CSsnCode& ref) const
{
	return FALSE;	// cssi/cssu is an event
}

const CSsnCode& CSsnCode::operator= (const CSsnCode& ref)
{
	if (this != &ref)
	{
		code = ref.code;
		index = ref.index;
		type = ref.type;
		number = ref.number;
	}
	return *this;
}

//////////////////////////////////////////////////////////////////////////

CUssdCode::CUssdCode()
{
	n = -1;
	dcs = -1;
}

BOOL CUssdCode::operator== (const CUssdCode& ref) const
{
	return FALSE;	// ussd is an event
}

const CUssdCode& CUssdCode::operator= (const CUssdCode& ref)
{
	if (this != &ref)
	{
		n = ref.n;
		dcs = ref.dcs;
		str_urc = ref.str_urc;
	}
	return *this;
}

extern CString DecodeUCS2txt(const string&);

int CUssdCode::Decode(
	const string	&src, 
	unsigned char	dcs)
{
	switch (dcs/*GetCodeScheme(dcs)*/)
	{
	case CBS_ALPHA_UCS2:
		str_urc = DecodeUCS2txt(src);
		break;
		
	case CBS_ALPHA_DATA:
		str_urc = DecodeUCS2txt(src);
		break;
		
	default:
		str_urc = src.c_str();
		break;
	}
	
	size_t x = str_urc.Find(_T('\n'), 1);
	
	while (x != (size_t)-1)
	{
		if (str_urc.GetAt(x-1) != _T('\r'))
		{
			str_urc.Insert(x++, _T('\r'));
		}
		
		x = str_urc.Find(_T('\n'), x+1);
	}
	
	return 0;
}

//////////////////////////////////////////////////////////////////////////

CMsgStatusInd::CMsgStatusInd()
{
	ind = -1;
}

BOOL CMsgStatusInd::operator == (const CMsgStatusInd& ref)const
{
	return FALSE;
}

const CMsgStatusInd& CMsgStatusInd::operator = (const CMsgStatusInd& ref)
{
	if(this != &ref)
	{
		ind = ref.ind;
		value = ref.value;
	}

	return *this;
}

//////////////////////////////////////////////////////////////////////////

CMsgInd::CMsgInd()
{
	index = -1;
}

BOOL CMsgInd::operator== (const CMsgInd& ref) const
{
	return FALSE;	// msg ind is an event
}

const CMsgInd& CMsgInd::operator= (const CMsgInd& ref)
{
	if (this != &ref)
	{
		index = ref.index;
		mem = ref.mem;
	}
	return *this;
}
//////////////////////////////////////////////////////////////////////////
CCbMsgInd::CCbMsgInd()
{
	length = -1;
}

BOOL CCbMsgInd::operator==(const CCbMsgInd& ref)const
{
	return FALSE;  
}

const CCbMsgInd& CCbMsgInd::operator=(const CCbMsgInd& ref)
{
	if(this != &ref)
	{
		length = ref.length;
		pdu = ref.pdu;
	}
	
	return *this;
}

//////////////////////////////////////////////////////////////////////////
CEtwsMsgInd::CEtwsMsgInd()
{
	
}

BOOL CEtwsMsgInd::operator==(const CEtwsMsgInd& ref)const
{
	return FALSE;
}	

const CEtwsMsgInd& CEtwsMsgInd::operator=(const CEtwsMsgInd& ref)
{
	if(this != &ref)
	{
		warningType = ref.warningType;
		msgId = ref.msgId;
		plmnId = ref.plmnId;
		serialNum = ref.serialNum;
		securityInfo = ref.securityInfo;
	}
	
	return *this;
}
