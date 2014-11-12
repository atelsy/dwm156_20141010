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
*   ems_segment_proc.h
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


#ifndef _EMS_SEGMENT_PROC_H
#define _EMS_SEGMENT_PROC_H
 
extern void EMSSetUDHI(EMSData *emsData);
extern kal_bool EMSChangeSegment(EMSData *ems, EMSInternalData *i);
extern void EMSFlushText(EMSData *ems, 
  EMSInternalData *i,
  kal_bool	needPack,
  kal_uint16 *udhLength, 
  kal_uint16 *txt_len,
  kal_uint16 *txt_len_in_char);
extern void EMSFlushAll(EMSData	   *ems,
 kal_uint16	newLength,
 kal_uint16	textEndOffset,					 
 kal_bool  needPack);

extern void EMSFlush(EMSData	   *ems,
 EMSObject*object, 
 kal_bool needPack);

extern EMSTATUS EMSInternalPack(EMSData *emsData, 
 kal_uint8  *TPUD[], 
 kal_bool	needPack);
extern void MakeConcatHdr(kal_uint8 *data, kal_uint8 conc_iei, kal_uint16 msg_ref, 
  kal_uint8 totalSeg, kal_uint8 seg);
extern void MakePortHdr(kal_uint8 *data, kal_uint16 src_port, kal_uint16 dst_port);
extern void updateEMSReminder(EMSData *emsData);
extern EMSTATUS EMSCalculateLength(EMSData *emsData);
extern EMSTATUS EMSDecodeUDH(EMSData *emsData, kal_uint8 *data, kal_uint16 text_len);
extern kal_bool EMSDetectObj(kal_uint8 *data);

#endif /* _EMS_SEGMENT_PROC_H */




