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
 *   GSMMisc.cpp
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
#include <time.h>
#include <stdlib.h>
#include "GSMMisc.h"
#include "glbdefs.h"

std::string GSMAddress::GetNumStr(void) const
{   
	return num_str;  
}

void GSMAddress::SetGSMAddress(const std::string str, eAddrType type)
{
	num_str = "";
	AddrType = type;
	for(int i = 0; i< str.length(); i++)
	{
		if(str[i]!=' ')
			num_str += str[i]; 
		if(!isdigit(str[i]) && str[i]!='+' && str[i]!='*' && str[i]!='#' && str[i]!='p' && str[i]!='w')
			AddrType = TON_ALPHANUMERIC;
	}
	
	if(num_str.length()>0 && num_str[0]=='+' && AddrType != TON_ALPHANUMERIC)
		AddrType = TON_INTERNATIONAL;
	if(AddrType==TON_INTERNATIONAL && num_str[0]!='+')
		num_str = "+" + num_str;
}

bool GSMAddress::operator<(const GSMAddress &rhs)
{
	if(num_str.length() != rhs.num_str.length())
	{
		int minnum = __min((int)num_str.length(), (int)rhs.num_str.length());
		std::string tempstr1= num_str;
		std::string tempstr2= rhs.num_str;
		
		if(num_str.length() > minnum)
			tempstr1 = num_str.substr(0, minnum);
		else if(rhs.num_str.length() > minnum)
			tempstr2 = rhs.num_str.substr(0, minnum);
		
		return tempstr1 < tempstr2;
	}
	return num_str < rhs.num_str;
}

void GSMTimeStamp::GettmStruct(tm &t)
{
	if(Year<80)
		t.tm_year = Year+100;
	else
		t.tm_year = Year;
	t.tm_mon = Month==0?Month:Month-1;
	t.tm_mday = Day;
	t.tm_hour = Hour;
	t.tm_min = Minute;
	t.tm_sec = Second;
}

void GSMTimeStamp::SettmStruct(const tm &t)
{
	if(t.tm_year>100)
		Year = t.tm_year - 100;
	else
		Year = t.tm_year;
	
	Month = t.tm_mon+1;
	Day = t.tm_mday;
	Hour = t.tm_hour;
	Minute = t.tm_min;
	Second = t.tm_sec; 
}

void GSMTimeStamp::SetCurrentTime()
{
	tm *pt;
	time_t ltime;
	tzset();
	int tz;
	tz = (int)timezone/60; //in minutes
	negativeTimezone = tz>0?false:true;
	Timezone = tz>0?(unsigned short)tz:(unsigned short)(tz*(-1));
	time( &ltime );
	pt = localtime( &ltime );
	SettmStruct(*pt);
}

std::string GSMTimeStamp::GetTimeString()
{
	char buf[128];
	tm t;
	GettmStruct(t);
	strftime(buf, 128, "%Y/%m/%d %X", &t);
	return std::string(buf);
}

void GSMTimeStamp::SetTimeString(const std::string str)
{
	const char *pBuf = str.c_str();
	int tmp1, tmp2, tmp3;
	
	sscanf(pBuf, "%hd/%02d/%02d", &Year, &tmp1, &tmp2);
	
	Month = (unsigned char)tmp1;
	Day = (unsigned char)tmp2;
	
	if (Year >= 100)
	{
		Year -= 1900;
		pBuf += 2;
	}
	
	pBuf += 9;
	sscanf(pBuf, "%02d:%02d:%02d", &tmp1, &tmp2, &tmp3);
	
	Hour = tmp1;
	Minute = tmp2;
	Second = tmp3;
	
	Timezone = timezone/60; //in minutes
}

bool GSMTimeStamp::operator<(const GSMTimeStamp &rhs)
{
	if(Year<rhs.Year)
		return true;
	else if(Year>rhs.Year) 
		return false;
	
	if(Month < rhs.Month)
		return true;
	else if(Month > rhs.Month)
		return false;
	
	if(Day < rhs.Day)
		return true;
	else if(Day > rhs.Day)
		return false;
	
	if(Hour < rhs.Hour)
		return true;
	else if(Day > rhs.Day)
		return false;
	
	if(Minute < rhs.Minute)
		return true;
	else if(Minute > rhs.Minute)
		return false;
	
	if(Second < rhs.Second)
		return true;
	else if(Second > rhs.Second)
		return false;  
	
	return true;
}

::ostream& operator<<(::ostream &os, const GSMAddress &ga)
{
	if( !os ) return os;
	
	//eAddrType AddrType;
	os.write((char *)&(ga.AddrType), sizeof(ga.AddrType));
	//eAddrNumberPlan AddrPlane;
	os.write((char *)&(ga.AddrPlane), sizeof(ga.AddrPlane));
	//string num_str;
	int str_len = (int)ga.num_str.size()+1;
	os.write((char*)&str_len, sizeof(str_len));
	os.write(ga.num_str.c_str(), str_len);
	
	return os;
}

::istream& operator>>(::istream &is, GSMAddress &ga)
{
	if( !is ) return is;
	int str_len;
	
	//eAddrType AddrType;
	is.read((char *)&(ga.AddrType), sizeof(ga.AddrType));
	//eAddrNumberPlan AddrPlane;
	is.read((char *)&(ga.AddrPlane), sizeof(ga.AddrPlane));
	//string num_str;
	is.read((char*)&str_len, sizeof(str_len));
	char *str = new char[str_len];
	is.read(str, str_len);
	ga.num_str = str;
	delete [] str;
	
	return is;
}

::ostream& operator<<(::ostream &os, const GSMTimeStamp &gt)
{
	if( !os ) return os;
	//unsigned short Timezone;
	os.write((char *)&(gt.Timezone), sizeof(gt.Timezone));
	//unsigned short Year;
	os.write((char *)&(gt.Year), sizeof(gt.Year));
	//unsigned char Month;
	os.write((char *)&(gt.Month), sizeof(gt.Month));
	//unsigned char Day;
	os.write((char *)&(gt.Day), sizeof(gt.Day));
	//unsigned char Hour;
	os.write((char *)&(gt.Hour), sizeof(gt.Hour));
	//unsigned char Minute;
	os.write((char *)&(gt.Minute), sizeof(gt.Minute));
	//unsigned char Second; 
	os.write((char *)&(gt.Second), sizeof(gt.Second));
	//bool negativeTimezone;
	os.write((char *)&(gt.negativeTimezone), sizeof(gt.negativeTimezone));
	
	return os;
	
}

::istream& operator>>(::istream &is, GSMTimeStamp &gt)
{
	if( !is ) return is;
	//unsigned short Timezone;
	is.read((char *)&(gt.Timezone), sizeof(gt.Timezone));
	//unsigned short Year;
	is.read((char *)&(gt.Year), sizeof(gt.Year));
	//unsigned char Month;
	is.read((char *)&(gt.Month), sizeof(gt.Month));
	//unsigned char Day;
	is.read((char *)&(gt.Day), sizeof(gt.Day));
	//unsigned char Hour;
	is.read((char *)&(gt.Hour), sizeof(gt.Hour));
	//unsigned char Minute;
	is.read((char *)&(gt.Minute), sizeof(gt.Minute));
	//unsigned char Second; 
	is.read((char *)&(gt.Second), sizeof(gt.Second));
	//bool negativeTimezone;
	is.read((char *)&(gt.negativeTimezone), sizeof(gt.negativeTimezone));
	
	return is; 
}

//ISO 8859 Latin-1 / GSM 7 bit undefined char
const int NOP = '_';

static unsigned char GsmToLatin1Table[] =
{
	//0x00 '@',  -,  '$',  -,   -,   -,   -,   -, 
	'@', 163, '$', 165, 232, 233, 249, 236,
	//0x08  -,   -,  LF,   -,   -,   CR,  -,   -, 
	242, 199,  10, 216, 248,  13, 197, 229,
	//0x10  -,  '_',  -,   -,   -,   -,   -,   -,
	NOP, '_', NOP, NOP, NOP, NOP, NOP, NOP, 
	//0x18  -,   -,   -,   -,   -,   -,   -,   -,
	NOP, NOP, NOP, NOP, 198, 230, 223, 201, 
	//0x20 ' ', '!', '"', '#', '?,  '%', '&', ''',
	' ', '!', '"', '#', 164, '%', '&', '\'',
	//0x28 '(', ')', '*', '+', ',', '-', '.', '/',
	'(', ')', '*', '+', ',', '-', '.', '/',
	//0x30 '0', '1', '2', '3', '4', '5', '6', '7',
	'0', '1', '2', '3', '4', '5', '6', '7',
	//0x38 '8', '9', ':', ';', '<', '=', '>', '?', 
	'8', '9', ':', ';', '<', '=', '>', '?', 
	//0x40  -,  'A', 'B', 'C', 'D', 'E', 'F', 'G', 
	161, 'A', 'B', 'C', 'D', 'E', 'F', 'G', 
	//0x48 'H', 'I', 'J', 'K', 'L', 'M', 'N', 'O', 
	'H', 'I', 'J', 'K', 'L', 'M', 'N', 'O',
	//0x50 'P', 'Q', 'R', 'S', 'T', 'U', 'V', 'W',
	'P', 'Q', 'R', 'S', 'T', 'U', 'V', 'W',
	//0x58 'X', 'Y', 'Z',  -,   -,   -,   -,   -, 
	'X', 'Y', 'Z', 196, 214, 209, 220, 167,
	//0x60  -,  'a', 'b', 'c', 'd', 'e', 'f', 'g',
	191, 'a', 'b', 'c', 'd', 'e', 'f', 'g',
	//0x68 'h', 'i', 'j', 'k', 'l', 'm', 'n', 'o', 
	'h', 'i', 'j', 'k', 'l', 'm', 'n', 'o', 
	//0x70 'p', 'q', 'r', 's', 't', 'u', 'v', 'w', 
	'p', 'q', 'r', 's', 't', 'u', 'v', 'w', 
	//0x78 'x', 'y', 'z',  -,   -,   -,   -,   -, 
	'x', 'y', 'z', 228, 246, 241, 252, 224
};

static unsigned char Latin1ToGsmTable[] = 
{
	//0x00  -,   -,   -,  -,   -,   -,   -,   -, 
	NOP, NOP, NOP, NOP, NOP, NOP, NOP, NOP,
	//0x08  -,   -,   LF,  -,   -,   CR,  -,   -, 
	NOP, NOP,  10, NOP, NOP,  13, NOP, NOP,
	//0x10  -,   -,   -,   -,   -,   -,   -,   -,
	NOP, NOP, NOP, NOP, NOP, NOP, NOP, NOP, 
	//0x18  -,   -,   -,   -,   -,   -,   -,   -,
	NOP, NOP, NOP, NOP, NOP, NOP, NOP, NOP, 
	//0x20 ' ', '!', '"', '#',  -,  '%', '&', ''',
	' ', '!', '"', '#', 0x2, '%', '&', '\'',
	//0x28 '(', ')', '*', '+', ',', '-', '.', '/',
	'(', ')', '*', '+', ',', '-', '.', '/',
	//0x30 '0', '1', '2', '3', '4', '5', '6', '7',
	'0', '1', '2', '3', '4', '5', '6', '7',
	//0x38 '8', '9', ':', ';', '<', '=', '>', '?', 
	'8', '9', ':', ';', '<', '=', '>', '?', 
	//0x40  -,  'A', 'B', 'C', 'D', 'E', 'F', 'G', 
	0x0, 'A', 'B', 'C', 'D', 'E', 'F', 'G', 
	//0x48 'H', 'I', 'J', 'K', 'L', 'M', 'N', 'O', 
	'H', 'I', 'J', 'K', 'L', 'M', 'N', 'O',
	//0x50 'P', 'Q', 'R', 'S', 'T', 'U', 'V', 'W',
	'P', 'Q', 'R', 'S', 'T', 'U', 'V', 'W',
	//0x58 'X', 'Y', 'Z',  -,   -,   -,   -,   -, 
	'X', 'Y', 'Z', NOP, NOP, NOP, NOP,0x11,
	//0x60  -,  'a', 'b', 'c', 'd', 'e', 'f', 'g',
	NOP, 'a', 'b', 'c', 'd', 'e', 'f', 'g',
	//0x68 'h', 'i', 'j', 'k', 'l', 'm', 'n', 'o', 
	'h', 'i', 'j', 'k', 'l', 'm', 'n', 'o', 
	//0x70 'p', 'q', 'r', 's', 't', 'u', 'v', 'w', 
	'p', 'q', 'r', 's', 't', 'u', 'v', 'w', 
	//0x78 'x', 'y', 'z',  -,   -,   -,   -,   -, 
	'x', 'y', 'z', NOP, NOP, NOP, NOP, NOP,
	//0x80  -,   -,   -,   -,   -,   -,   -,   -, 
	NOP, NOP, NOP, NOP, NOP, NOP, NOP, NOP,
	//0x88  -,   -,   -,   -,   -,   -,   -,   -, 
	NOP, NOP, NOP, NOP, NOP, NOP, NOP, NOP,
	//0x90  -,   -,   -,   -,   -,   -,   -,   -,
	NOP, NOP, NOP, NOP, NOP, NOP, NOP, NOP, 
	//0x98  -,   -,   -,   -,   -,   -,   -,   -,
	NOP, NOP, NOP, NOP, NOP, NOP, NOP, NOP, 
	//0xA0  -,   -,   -,   -,   -,   -,   -,   -,
	NOP, 0x40,NOP, 0x1, 0x24,0x3, NOP,0x5F,
	//0xA8  -,   -,   -,   -,   -,   -,   -,   -,
	NOP, NOP, NOP, NOP, NOP, NOP, NOP, NOP, 
	//0xB0  -,   -,   -,   -,   -,   -,   -,   -,
	NOP, NOP, NOP, NOP, NOP, NOP, NOP, NOP, 
	//0xB8  -,   -,   -,   -,   -,   -,   -,   -,
	NOP, NOP, NOP, NOP, NOP, NOP, NOP,0x60, 
	//0xC0 'A', 'A', 'A', 'A',  -,   -,   -,   -, 
	'A', 'A', 'A', 'A', 0x5B,0xE, 0x1C,0x9, 
	//0xC8 'E',  -,  'E', 'E', 'I', 'I', 'I', 'I', 
	'E',0x1F, 'E', 'E', 'I', 'I', 'I', 'I',
	//0xD0  -,  -,   'O', 'O', 'O', 'O',  -,   -,
	NOP,0x5D, 'O', 'O', 'O', 'O',0x5C, NOP,
	//0xD8  -,  'U', 'U', 'U',  -,   -,  'Y',  -, 
	0x0B,'U', 'U', 'U',0x5E, 'Y', NOP,0x1E,
	//0xE0  -,  'a', 'a', 'a',  -,   -,   -,   -,
	0x7F,'a', 'a', 'a',0x7B, 0xF,0x1D, 0x9,
	//0xE8  -,   -,  'e', 'e',  -,  'i', 'i', 'i', 
	0x4, 0x5, 'e', 'e', 0x7, 'i', 'i', 'i', 
	//0xF0  -,   -,   -,  'o', 'o', 'o',  -,   -, 
	NOP,0x7D, 0x8, 'o', 'o', 'o',0x7C, NOP, 
	//0xF8  -,   -,  'u', 'u',  -,  'y',  -,  'y', 
	0xC, 0x6, 'u', 'u',0x7E, 'y', NOP, 'y'
};

std::string GsmToISO8859latin1(std::string s)
{
	std::string latin1str(s.length(), 0);
	for (std::string::size_type i=0; i<s.length(); i++)
	{
		if((unsigned char)s[i] > 127)
			latin1str[i] = NOP;
		else 
			latin1str[i] = GsmToLatin1Table[s[i]];
	}
	return latin1str;
}

std::string ISO8859latin1ToGsm(std::string s)
{
	std::string gsmstr(s.length(), 0);
	for (std::string::size_type i=0; i<s.length(); i++)
	{
		unsigned char temp = (unsigned char)s[i];
		temp = Latin1ToGsmTable[(unsigned char)s[i]];
		gsmstr[i] = Latin1ToGsmTable[(unsigned char)s[i]];
	}
	return gsmstr;
}

//////////////////////////////////////////////////////////////////////////
const static TCHAR Latin1ToGsmTableEx[][2] =
{
	_T('['), 0x3c, 
	_T(']'), 0x3e, 
	_T('{'), 0x28, 
	_T('}'), 0x29, 
	_T('|'), 0x40, 
	_T('^'), 0x14, 
	_T('~'), 0x3d, 
	_T('\\'), 0x2f
};

TCHAR GetGsmTableEx(TCHAR ch)
{
	for (int i=0; i<sizeof(Latin1ToGsmTableEx)/(2*sizeof(TCHAR)); i++)
	{
		if (Latin1ToGsmTableEx[i][0] == ch)
		{
			return Latin1ToGsmTableEx[i][1];
		}
	}
	
	return 0x80;
}

TCHAR GetLatin1TableEx(TCHAR ch)
{
	for (int i=0; i<sizeof(Latin1ToGsmTableEx)/(2*sizeof(TCHAR)); i++)
	{
		if (Latin1ToGsmTableEx[i][1] == ch)
		{
			return Latin1ToGsmTableEx[i][0];
		}
	}
	
	return 0x80;
}

TCHAR GetGsmTable(TCHAR ch)
{
	if (ch > 0xff)
		return 0x80;
	else
		return (TCHAR)(Latin1ToGsmTable[ch]);
}

TCHAR GetLatin1Table(TCHAR ch)
{
	if (ch > 0x7f)
		return 0x80;
	else
		return (TCHAR)(GsmToLatin1Table[ch]);
}

