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
*   ATResult.cpp
*
* Project:
* --------
*   DCT
*
* Description:
* ------------
*	 This module implement the AT Command respond's results.
*
* Author:
* -------
*   Gang Wei
*
*******************************************************************************/



#include "ATResult.h"


ATParamElem::ATParamElem()
{

}


ATParamElem::~ATParamElem()
{

}

ATParamLst::ATParamLst()
{
}

ATParamLst::~ATParamLst()
{
}


ATResult::ATResult()
{
  this->retType = 0;
}


ATResult::~ATResult()
{

}

void ATResult::clear(void)
{
	resultLst.clear();
	this->retType = 0;
}

bool ATResult::check_key(const char* szKey) const
{
	if (NULL == szKey)
		return false;

	string key;
	get_string(key, 0, 0);
	return (key == szKey);
}

bool ATResult::get_string(string& str, int line, int row) const
{
	if (line >= resultLst.size())
		return false;
	if (row >= resultLst[line].eleLst.size())
		return false;
	if (resultLst[line].eleLst[row].type != AT_STRING)
		return false;
	
	str = resultLst[line].eleLst[row].str_value;
	return true;
}

bool ATResult::get_string(CString& str, int line, int row) const
{
	str.Empty();
	if (line >= resultLst.size())
		return false;
	if (row >= resultLst[line].eleLst.size())
		return false;
	if (resultLst[line].eleLst[row].type != AT_STRING)
		return false;
	
	str = resultLst[line].eleLst[row].str_value.c_str(); // to unicode
	return true;
}

int ATResult::get_integer(int line, int row) const
{
	if (line >= resultLst.size())
		return -1;
	if (row >= resultLst[line].eleLst.size())
		return -1;
	if (resultLst[line].eleLst[row].type != AT_INTEGER)
		return -1;
	
	return resultLst[line].eleLst[row].int_value;
}
