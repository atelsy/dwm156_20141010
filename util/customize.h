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
*  customize.h
*
* Project:
* --------
*   DCT
*
* Description:
* ------------
*   Customizable definitions.
*
* Author:
* -------
*   Xiantao Han
*
*==============================================================================
*           HISTORY
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

#ifndef _DCT_CUSTOMIZE_H_
#define _DCT_CUSTOMIZE_H_

#ifdef WIN32

//////////////////////////////////////////////////////////////////////////

#define FRAME_CLASSNAME		_T("#MTK-DCT-FRAME-CLASS#")
#define LOADER_WNDCLASS		_T("#MTK-DCT-LOADER-CLASS#")

//////////////////////////////////////////////////////////////////////////

#define FRAMEWORK_WIDTH		830					// width of main frame
#define FRAMEWORK_HEIGHT	550					// height of main frame

#define LCD_WIDTH			240					// width of LCD pane
#define LCD_RECT			21,39,21+197,39+241	// LCD screen position and size
#define LCD_TEXT_COLOR		RGB(255,0,128)		// LCD text color

#define GLB_BKCOLOR			RGB(219,234,239)	// dialog background color

//////////////////////////////////////////////////////////////////////////

#define DCT_COLOR_MENU_BK			RGB(100,201,230)	// checked menu item background color
#define DCT_COLOR_MENU_DISABLE_BK	RGB(213,214,216)	// disabled menu item background color
#define DCT_COLOR_MENU_DISABLE_TEXT	RGB(148,150,152)	// disabled menu item text color
#define DCT_COLOR_MENUBAR_BK		RGB(196,212,237)	// menu bar/item background color
#define DCT_COLOR_MENUBAR_SELECT_BK	RGB(0,170,194)		// selected menu item background color
#define DCT_COLOR_MENU_SELECT_BK	RGB(0,150,233)		// selected main menu background color (only for GRADIENT)
#define DCT_COLOR_WHITE				RGB(255,255,255)	// white
#define DCT_COLOR_BLACK				RGB(0,0,0)			// black

#define MENU_IMGWIDTH		16			// menu image width
#define MENU_IMGHEIGHT		16			// menu image height
#define MENU_IMGPADDING		6			// menu image padding
#define MENU_TEXTPADDING	8			// menu text padding

#define _DCT_MENU_SHOW_GRADIENT_		// to draw background gradiently (DCT_COLOR_MENU_BK to DCT_COLOR_WHITE)
#define _DCT_MENU_SHOW_CHECK_BITMAP_	// to show check image
//#define _DCT_MENU_SHOW_SHADOW_		// to show shadow

#ifdef _DCT_MENU_SHOW_SHADOW_
#define MENU_TEXTPADDING_MNUBR 4		// shadow padding
#define MENU_SM_CXSHADOW 4
#endif

#endif	// WIN32
//////////////////////////////////////////////////////////////////////////

#ifdef WIN32
#define _CBS_SUPPORT_
#define _ETWS_SUPPORT_
//#define _USSD_SUPPORT_
#define _CALL_COST_SUPPORT_
#endif

// #define _STK_SUPPORT_
// #define _STK_SIMULATE_
// #define _VCARD_SUPPORT_

#define _USSD_SUPPORT_

#define _MTK_SS_ECUSD_


#ifdef _STK_SUPPORT_

#define STK_SHORT_TIMEOUT	5		// 5 seconds [timeout for STK "auto-clear" message]
#define STK_DEFAULT_TIMEOUT	(5*60)	// 5 minutes [timeout for STK "wait for user to clear" message]

#endif	// _STK_SUPPORT_

//////////////////////////////////////////////////////////////////////////

#define EMERGENCY_NUMBERS \
	_T("112"), \
	_T("911"), \
	_T("999")

//////////////////////////////////////////////////////////////////////////

#endif
