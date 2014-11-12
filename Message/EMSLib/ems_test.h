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
*   ems_test.h
*
* Project:
* --------
*   DCT
*
* Description:
* ------------
*   This file contains function prototypes of .
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


#ifndef _EMS_TEST_H
#define _EMS_TEST_H

/*******************************************************************************
 *
 *  Keep Old Change History.
 * 
 *******************************************************************************
 * 
 *Rev 1.7   Apr 10 2004 18:09:10   mtk00592
 * NVRAM code reorg: Data items renaming and movement
 * Resolution for 4799: [NVRAM][Enhancement]NVRAM code reorg: Data items renaming and movement
 * 
 *Rev 1.6   Oct 20 2003 13:57:14   mtk00435
 * for succeeding to compile in Pluto PC simulator (turn on EMS_ON_WIN32)
 * Resolution for 1417: [SMSAL][Add feature] Make EMS library stand-alone in WIN32
 * 
 *Rev 1.5   Sep 29 2003 14:32:00   mtk00435
 * define EMS_CONCAT_MSG_REF_FUNCTION
 * Resolution for 3009: [SMSAL][EMS] EMS buffer management
 * 
 *Rev 1.4   Sep 29 2003 14:30:32   mtk00435
 * support EMS buffer management
 * Resolution for 3009: [SMSAL][EMS] EMS buffer management
 * 
 *Rev 1.3   Sep 08 2003 11:24:06   mtk00435
 * add EMS_MAX_OBJECT
 * Resolution for 1155: support EMS
 * 
 *Rev 1.2   Aug 23 2003 18:49:12   mtk00435
 * 1. add 'error' in EMSInternalData
 * 2. add IS_EMS_DATA_NEED
 * 3. add EMS_MALLOC/EMS_MFREE
 * Resolution for 1155: support EMS
 * 
 *Rev 1.1   Mar 31 2003 20:11:16   mtk00435
 * make EMS library stand-alone in WIN32 environment
 * Resolution for 1417: [SMSAL][Add feature] Make EMS library stand-along in WIN32
 * 
 *Rev 1.0   Mar 04 2003 10:40:32   admin
 * Initial revision.
 *
 *******************************************************************************/


extern void EMSDataDump(EMSData *emsData);   
#endif /* _EMS_TEST_H */


