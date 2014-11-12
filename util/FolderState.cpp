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
*   FolderState.cpp
*
* Project:
* --------
*   DCT
*
* Description:
* ------------
*   FolderState.h
*
* Author:
* -------
*   Fengping Yu
*
*==============================================================================
*		   HISTORY
* Below this line, this part is controlled by PVCS VM. DO NOT MODIFY!!
*------------------------------------------------------------------------------
* $Revision:$
* $Modtime:$
* $Log:$
* 
* 08 29 2012 fengping.yu
* [STP100004315]  check in code
* .
 *
 * 01 10 2011 gang.wei
 * [STP100004317]  Data Card Tool source code check in
 * .
*
*------------------------------------------------------------------------------
* Upper this line, this part is controlled by PVCS VM. DO NOT MODIFY!!
*==============================================================================
*******************************************************************************/
#include "Win2Mac.h"
#include <fstream.h>
#include "FolderState.h"

using namespace::std;

::ostream& operator<<(::ostream &os, const folder_state_struct &fs)
{
	if( !os ) return os;
	os.write((char *)&(fs.nType), sizeof(fs.nType));
	os.write((char *)&(fs.nState), sizeof(fs.nState));
	os.write((char *)&(fs.nImage), sizeof(fs.nImage));
	os.write((char *)&(fs.nSelImage), sizeof(fs.nSelImage));

	os.write((char *)&(fs.bSubfolder), sizeof(fs.bSubfolder));
	os.write((char *)&(fs.bModify), sizeof(fs.bModify));
	os.write((char *)&(fs.bMessage), sizeof(fs.bMessage));
	os.write((char *)&(fs.bEnable), sizeof(fs.bEnable));
	os.write((char *)&(fs.bSave), sizeof(fs.bSave));			
	return os;
}
	
::istream& operator>>(::istream &is, folder_state_struct &fs)
{
	if( !is ) return is;
	is.read((char *)&(fs.nType), sizeof(fs.nType));
	is.read((char *)&(fs.nState), sizeof(fs.nState));
	is.read((char *)&(fs.nImage), sizeof(fs.nImage));
	is.read((char *)&(fs.nSelImage), sizeof(fs.nSelImage));

	is.read((char *)&(fs.bSubfolder), sizeof(fs.bSubfolder));
	is.read((char *)&(fs.bModify), sizeof(fs.bModify));
	is.read((char *)&(fs.bMessage), sizeof(fs.bMessage));
	is.read((char *)&(fs.bEnable), sizeof(fs.bEnable));
	is.read((char *)&(fs.bSave), sizeof(fs.bSave));		

	return is;	
}	

::ostream& operator<<(::ostream &os, const CString &str)
{
	size_t str_len = str.GetLength()+1;
	os.write((char*)&str_len, sizeof(str_len));
	os.write((char*)(LPCTSTR)str, str_len*sizeof(TCHAR));	
	
	return os;
}

::istream& operator>>(::istream &is, CString &str)
{
	size_t str_len;
	
	is.read((char*)&str_len, sizeof(str_len));
	TCHAR *buf = new TCHAR[str_len];
	is.read((char*)buf, str_len*sizeof(TCHAR));
	str = buf;
	delete [] buf;
	
	return is;
}
