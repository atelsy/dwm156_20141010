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
*   ems_txt_proc.h
*
* Project:
* --------
*   DCT
*
* Description:
* ------------
*   This file is intends for SMS Library.
*
* Author:
* -------
*   Fengping Yu
*
*==============================================================================
*   HISTORY
* Below this line, this part is controlled by PVCS VM. DO NOT MODIFY!!
*------------------------------------------------------------------------------
* $Revision:$
* $Modtime:$
* $Log:$
* 
* 07 19 2012 fengping.yu
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


 
#ifndef _EMS_TXT_FMT_PROC_H
#define _EMS_TXT_FMT_PROC_H
 
extern void EMSDeleteText(EMSData *emsData, kal_uint16 bytes);
extern void SetCurrTxtFmtObj(EMSData *emsData);
extern kal_bool MergeTF(EMSData *emsData, EMSObject *prev, EMSObject *next);
extern void BwCancelCurrentTF(EMSData *emsData, kal_bool needCancel);
extern kal_bool TextFormatCmp(EMSTextFormat *tf1, EMSTextFormat *tf2);
extern EMSObject *AddTextFmtObjByOffset(EMSData *emsData, kal_uint16 offsetToText, EMSTextFormat *txt_fmt);
extern EMSObject *AddTextFormatObject(EMSData *emsData, EMSTextFormat *txt_fmt);
extern kal_bool needSplitTextFormat(EMSData *emsData);
extern kal_bool isTFValid(EMSTextFormat *txt_fmt);
extern kal_bool isDefaultTF(EMSTextFormat *txt_fmt);
extern EMSTATUS AddTextFormat(EMSData *emsData, EMSTextFormat *txt_fmt, kal_uint16 length);
extern EMSTATUS ReAllocateTextBuffer(EMSData *emsData, kal_uint16 newBufferSize, 
 kal_uint16 num_byte);
extern kal_bool GetTxtFmtRemainingSpace(EMSData *emsData, 
 EMSInternalData *i, 
 kal_uint16 ori_udh_len,
 kal_uint16 udh_len,
 kal_uint16 txt_len,
 EMSTextFormat *txt_fmt,
 kal_uint16   *udLength,
 kal_uint16  *numOfChar, 
 kal_uint16  *numOfOctet);
extern kal_bool PackTextFormat(EMSData*emsData,
EMSObject  *object,
kal_bool   needPack);
extern void EMSTextCpy(kal_uint8  *dest, 
kal_uint8  *src, 
kal_uint16 len, 
kal_uint8  dcs, 
kal_bool   isPack);
extern kal_bool DecodeTFAttribute(kal_uint8 ie_len, 
   kal_uint8 *data, 
   EMSTextFormat *tf);


#endif /* _EMS_TXT_FMT_PROC_H */






