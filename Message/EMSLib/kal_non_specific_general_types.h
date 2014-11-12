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
*   kal_non_specific_general_types.h
*
* Project:
* --------
*   DCT
*
* Description:
* ------------
*   This file provides general types definations that are not specific
*   to any os
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



/*********************************************************************
   (C) _____ (year of first publication) Sasken communication
   Technologies Limited, All rights reserved.
*   This file provides a template for .c files. This space
*   should be used to describe the file contents
*   Component-specific prefix : xxxx
*********************************************************************/

#ifndef _KAL_NON_SPECIFIC_GENERAL_TYPES_H
#define _KAL_NON_SPECIFIC_GENERAL_TYPES_H

/*
#ifdef WIN32
#include "windows.h"
#endif
*/

/*******************************************************************************
 * Type Definitions
 *******************************************************************************/
#ifdef KAL_ON_OSCAR

#ifndef UINT8
typedef unsigned char UINT8;
#endif

#ifndef UINT16
typedef unsigned short UINT16;
#endif

#ifndef UINT32
typedef unsigned int UINT32;
#endif

#ifndef INT
typedef int INT;
#endif

#endif //KAL_ON_OSCAR

typedef unsigned char   kal_uint8;
typedef signed char kal_int8;
typedef char		kal_char;
typedef unsigned short  kal_wchar;

typedef unsigned short int  kal_uint16;
typedef signed short int	kal_int16;

typedef unsigned int	kal_uint32;
typedef signed int  kal_int32;

#ifdef WIN32
   //typedef ULONG64  kal_uint64;
   //typedef LONG64   kal_int64;
   typedef unsigned __int64  kal_uint64;
   typedef __int64   kal_int64;
#else
   typedef unsigned long long   kal_uint64;
   typedef signed long long kal_int64;
#endif

typedef enum {
   KAL_FALSE,
   KAL_TRUE
} kal_bool;

#endif  /* _KAL_NON_SPECIFIC_GENERAL_TYPES_H */


