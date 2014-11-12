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
 *  GSMMisc.h
 *
 * Project:
 * --------
 *   DCT
 *
 * Description:
 * ------------
 *   Misc Class for SMS
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


#ifndef MTK_GSMMISC_H
#define MTK_GSMMISC_H

#include "Win2Mac.h"
#include <fstream.h>
#include <string>


using namespace std;

typedef enum
{
	NOT_PRESENT  = 0,
	PRESENT_ENHANCED = 1,
	PRESENT_RELATIVE = 2,
	PRESENT_ABSOLUTE = 3
	
}eTimePeriodFormat;

typedef enum
{
	DCS_DEFAULT  = 0x00,  //GSM 7-bit
	DCS_8BIT = 0x04,  //8-bit
	DCS_UCS2 = 0x08,  //UCS2 
	DCS_RESERVED = 0x0C   //reserved alphabet
	
}eDataCodingScheme;

typedef enum
{  
	TON_UNKNOWN = 0, 
	TON_INTERNATIONAL   = 1, 
	TON_NATIONAL= 2,
	TON_NETWORKSPECIFIC = 3, 
	TON_SUBSCRIBER  = 4,
	TON_ALPHANUMERIC= 5, 
	TON_ABBREVIATED = 6, 
	TON_RESERVED= 7
}eAddrType;  

typedef enum eAddrNumberPlan 
{
	NP_UNKNOW   = 0x00, 
	NP_ISDNTEL  = 0x01,
	NP_DATA = 0x03,
	NP_TELIX= 0x04,
	NP_NATIONAL = 0x08,
	NP_PRIVATE  = 0x09, 
	NP_RESERVED = 0x0F
	
}eAddrNumberPlan;

class GSMAddress
{
public:
	
	GSMAddress() : AddrType(TON_UNKNOWN), AddrPlane(NP_ISDNTEL){}
	void SetGSMAddress(const std::string str, eAddrType type = TON_UNKNOWN);
	std::string GetNumStr(void) const;
	
	eAddrType AddrType;
	eAddrNumberPlan AddrPlane;
	
	bool operator<(const GSMAddress &rhs);
	
private:
	std::string num_str;
	
	friend ::ostream& operator<<(::ostream &os, const GSMAddress &ga);
	friend ::istream& operator>>(::istream &is, GSMAddress &ga);
};

class GSMTimeStamp
{
public:
	
	GSMTimeStamp(): 
	Timezone(0),negativeTimezone(false),Year(0),Month(0),Day(0),Hour(0),Minute(0),Second(0){}
	void GettmStruct(tm &t);
	void SettmStruct(const tm &t);
	void SetCurrentTime();
	std::string GetTimeString();
	void SetTimeString(const std::string str);	
	
	unsigned short Timezone;
	unsigned short Year;
	unsigned char Month;
	unsigned char Day;
	unsigned char Hour;
	unsigned char Minute;
	unsigned char Second; 
	bool negativeTimezone;
	
	bool operator<(const GSMTimeStamp &rhs);
	
	friend ::ostream& operator<<(::ostream &os, const GSMTimeStamp &gt);
	friend ::istream& operator>>(::istream &is, GSMTimeStamp &gt);
	
};

#endif
