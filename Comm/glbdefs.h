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
*   glbdefs.h
*
* Project:
* --------
*   DCT
*
* Description:
* ------------
*   Global definitions
*
* Author:
* -------
*   Fengping Yu
*
*==============================================================================
*           HISTORY
* Below this line, this part is controlled by PVCS VM. DO NOT MODIFY!!
*------------------------------------------------------------------------------
* $Revision:$
* $Modtime:$
* $Log:$
* 
* 10 09 2012 fengping.yu
* [STP100004315]  check in code
* .
*
*------------------------------------------------------------------------------
* Upper this line, this part is controlled by PVCS VM. DO NOT MODIFY!!
*==============================================================================
*******************************************************************************/

// global definitions

#ifndef _GLB_DEFS_H_
#define _GLB_DEFS_H_

#ifndef WIN32
#include "Win2Mac.h"

#define __min(a, b) ((a) < (b) ? (a) : (b))
#define __max(a, b) ((a) > (b) ? (a) : (b))
#endif

enum facility_lock_t
{
	FL_SC = 0,	// PIN
	FL_P2,		// PIN2
	FL_AO,
	FL_OI,
	FL_OX,
	FL_AI,
	FL_IR,
	FL_AB,
	FL_AG,
	FL_AC,
	FL_PN,
	FL_PU,
	FL_PP,
	FL_PC,

	FL_MAX
};

//////////////////////////////////////////////////////////////////////////

class CALL
{
public:
	short			idx;
	short			dir;
	short			status;
	short			mode;
	short			mpty;
	CString			number;
	short			type;
	mutable CString	alpha;
};

enum cf_reason_t
{
	CF_UNCONDICTIONAL	= 0,
	CF_MOBILEBUSY		= 1,
	CF_NOREPLY			= 2,
	CF_NOTREACHABLE		= 3,
	CF_ALLFORWARDING	= 4,
	CF_ALLCONDICTIONAL	= 5
};

enum service_class_t
{
	CLS_VOICE	= 1,
	CLS_DATA	= 2,
	CLS_FAX		= 4,
	CLS_SMS		= 8,
	CLS_DCS		= 16,
	CLS_DCAS	= 32,
	CLS_PACKET	= 64,
	CLS_PAD		= 128
};

//////////////////////////////////////////////////////////////////////////
// error code (16 bit)
enum err_code_t
{
	// +cme error:
	ER_OK			= 0,
	ER_NOALLOW		= 3,
	ER_NOSUPPORT	= 4,
	ER_NOSIM		= 10,
	ER_PINREQ		= 11,
	ER_PUKREQ		= 12,
	ER_SIMFAIL		= 13,
	ER_SIMBUSY		= 14,
	ER_SIMWRONG		= 15,
	ER_WRONGPWD		= 16,
	ER_PIN2REQ		= 17,
	ER_PUK2REQ		= 18,
	ER_BADINDEX		= 21,
	ER_LONGTEXT		= 24,
	ER_BADTEXT		= 25,
	ER_LONGNUM		= 26,
	ER_BADNUM		= 27,
	ER_NOSERVICE	= 30,
	ER_EMCALLONLY	= 32,
	ER_UNKNOWN		= 100,
	ER_TRYLATER		= 256,
	ER_CALLBARRED	= 257,
	ER_SSNOTEXE		= 261,
	ER_SIMBLOCK		= 262,
	
	// todo: +cms error:

	ER_TIMEOUT		= 0x7f00,
	ER_NOCONNECTION,
	ER_USERABORT,
	ER_CMEERROR
};
// error code

//////////////////////////////////////////////////////////////////////////

#endif
