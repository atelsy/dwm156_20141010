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
 *   SMSBuffer.cpp
 *
 * Project:
 * --------
 *   DCT
 *
 * Description:
 * ------------
 *   Buffer Class for SMS
 *
 * Author:
 * -------
 *   Fengping Yu
 *
 *******************************************************************************/
#include "GSMMisc.h"
#include "SMSBuffer.h"

SMSBuffer::SMSBuffer()
{
	pcurbyte = smsbuf;
	curbit = 0;
	memset(smsbuf, 0, sizeof(smsbuf)); 
	pfinalbyte = pcurbyte + sizeof(smsbuf);
}

SMSBuffer::SMSBuffer(const char *buf, int buflen)
{
	memset(smsbuf, 0, sizeof(smsbuf));
	if(buflen > sizeof(smsbuf))
		buflen = sizeof(smsbuf);
	memcpy(smsbuf, buf, buflen);
	pcurbyte = smsbuf;
	curbit = 0;
	pfinalbyte = pcurbyte+buflen;
}

void SMSBuffer::Set1Bit(const unsigned char b)
{
	if(pcurbyte > pfinalbyte)
		return;
	if(b!=0)
		*pcurbyte |= (1 << curbit);
	curbit++;
	if(curbit == 8)
	{
		curbit=0;
		pcurbyte++;
	}
}

void SMSBuffer::SetMultiBits(const unsigned int value, const unsigned char bit_num)
{
	for(int i=0; i<bit_num; i++)
		Set1Bit((value & (1<<i)) != 0);
}

void SMSBuffer::SetOctet(const unsigned char c)
{
	ByteAlign();
	*pcurbyte=c;
	pcurbyte++;
}   

void SMSBuffer::ByteAlign()
{
	if(pcurbyte > pfinalbyte)
		return;
	if(curbit!=0)
	{
		curbit=0;
		pcurbyte++;
	}
}

void SMSBuffer::SetSeptetStartOctet()
{
	ByteAlign();
	pSeptetStartOctet = pcurbyte;
}

void SMSBuffer::SeptetAlign()
{
	if(pcurbyte > pfinalbyte)
		return;
	while (((pcurbyte - pSeptetStartOctet) * 8 + curbit) % 7 != 0) 
		Set1Bit(0);
}

void SMSBuffer::SetAddr(GSMAddress &addr)
{
	ByteAlign();
	std::string numstr = addr.GetNumStr();
	
	//remove '+' for TON_INTERNATIONAL type
	if(numstr.length()>0 && addr.AddrType == TON_INTERNATIONAL)
		numstr = numstr.substr(1, numstr.length() - 1);
	
	int len = (int)numstr.length();
	//set Address length
	if (addr.AddrType == TON_ALPHANUMERIC)  
		SetOctet((len * 7 + 6) / 8 * 2);
	else
		SetOctet(len);
	
	//set Address Plane and Type
	SetMultiBits(addr.AddrPlane, 4);
	SetMultiBits(addr.AddrType, 3);
	Set1Bit(1);
	
	//set Address Value
	if (len > 0)
	{
		if (addr.AddrType == TON_ALPHANUMERIC)
		{
			SetSeptetStartOctet();
			SetString(ISO8859latin1ToGsm(numstr));
			ByteAlign(); 
		}
		else
		{
			// [STP100000974] deal with phone address with '+', '*', '#', 'p' and 'w'
			SetSemiOctets_Addr(numstr);
		}
	}
}

void SMSBuffer::SetSCAddr(GSMAddress &addr)
{
	ByteAlign();
	std::string numstr = addr.GetNumStr();
	
	//remove '+' for TON_INTERNATIONAL type
	if(numstr.length()>0 && addr.AddrType == TON_INTERNATIONAL)
		numstr = numstr.substr(1, numstr.length() - 1);
	
	int len = (int)numstr.length();
	if(len==0)
	{
		SetOctet(0); //use default SCA for SMS Message
		return;
	}
	//set len
	SetOctet(len / 2 + len % 2 + 1);
	
	//set Address Plane and Type
	SetMultiBits(addr.AddrPlane, 4);
	SetMultiBits(addr.AddrType, 3);
	Set1Bit(1);
	
	//set Address Value
	if (addr.AddrType == TON_ALPHANUMERIC)
	{
		SetString(ISO8859latin1ToGsm(numstr));
		ByteAlign();
	}
	else
	{
		// [STP100000974] deal with phone address with '+', '*', '#', 'p' and 'w'
		SetSemiOctets_Addr(numstr);
	}
}

void SMSBuffer::SetTimeStamp(const GSMTimeStamp &ts)
{
	SetSemiOctetsInt(ts.Year, 2);
	SetSemiOctetsInt(ts.Month, 2);
	SetSemiOctetsInt(ts.Day, 2);
	SetSemiOctetsInt(ts.Hour, 2);
	SetSemiOctetsInt(ts.Minute, 2);
	SetSemiOctetsInt(ts.Second, 2);
	SetSemiOctetsInt(ts.Timezone / 15, 2);
	if(ts.negativeTimezone==false)   //set the bit of the negative time zone!
		*(pcurbyte - 1) |= 8;
}

void SMSBuffer::SetString(const std::string &str)
{
	SeptetAlign(); //align to Septet Boundary!
	for (int i = 0; i < str.length(); i++)
	{
		unsigned char c = str[i];
		//write 7 bit of char!
		for(int j = 0; j < 7; j++)
			Set1Bit( ((1 << j) & c) != 0 );
	}
}

void SMSBuffer::SetSemiOctetsInt(const unsigned int val, const unsigned char len)
{
	char tempbuf[128];
	sprintf(tempbuf, "%d", val);
	std::string str=tempbuf;
	while(str.length()<len)
		str = "0" + str;
	SetSemiOctets(str);
}

void SMSBuffer::SetSemiOctets(const std::string &str)
{
	ByteAlign();
	
	for (int i = 0; i < str.length(); i++)
	{
		//  unsigned char c = str[i];
		
		if(curbit == 0)
		{
			*pcurbyte = str[i] - '0';
			curbit = 4;
		}
		else
		{
			*pcurbyte |= ((str[i] - '0') << 4);
			pcurbyte++;
			curbit = 0;
		}
	}
	
	//write 1111 if curbit is 4
	if(curbit == 4)
	{
		*pcurbyte |= 0xf0;
		pcurbyte++;
		curbit = 0;
	}
}

void SMSBuffer::SetSemiOctets_Addr(const std::string &str)
{
	ByteAlign();
	
	for (int i = 0; i < str.length(); i++)
	{
		unsigned char c ;//= str[i];
		
		if(curbit == 0)
		{
			if(str[i] == '*'){
				c = 0xa;
			}else if(str[i] == '#'){
				c = 0xb;
			}else if(str[i] == 'p'){
				c = 0xc;
			}else if(str[i] == 'w'){
				c = 0xd;
			}else if(str[i] == '+'){
				c = 0xe;
			}else{
				c = str[i] - '0';
			}
			*pcurbyte = c;
			curbit = 4;
		}
		else
		{
			if(str[i] == '*'){
				c = 0xa;
			}else if(str[i] == '#'){
				c = 0xb;
			}else if(str[i] == 'p'){
				c = 0xc;
			}else if(str[i] == 'w'){
				c = 0xd;
			}else if(str[i] == '+'){
				c = 0xe;
			}else{
				c = str[i] - '0';
			}
			*pcurbyte |= (c << 4);
			pcurbyte++;
			curbit = 0;
		}
	}
	
	//write 1111 if curbit is 4
	if(curbit == 4)
	{
		*pcurbyte |= 0xf0;
		pcurbyte++;
		curbit = 0;
	}
}

void SMSBuffer::GetHexString(std::string &str)
{
	str = "";
	ByteAlign();
	int byte_num = (int)(pcurbyte - smsbuf);
	char c[10];
	for(int i=0; i< byte_num ; i++)
	{
		sprintf(c, "%02X", smsbuf[i]);
		str += c;
	}
}

bool SMSBuffer::Get1Bit()
{   
	if(pcurbyte > pfinalbyte)
		return false;
	
	bool value = (*pcurbyte >> curbit) & 1;
	curbit++;
	if(curbit == 8)
	{
		curbit = 0;
		pcurbyte++;
	}
	return value;
}

unsigned int SMSBuffer::GetMultiBits(const unsigned char bit_num)
{
	unsigned char value = 0;
	for(int i=0; i<bit_num; i++)
		value |= (Get1Bit() << i);
	return value;
}

unsigned char SMSBuffer::GetOctet()
{
	ByteAlign();
	unsigned char value = *pcurbyte;
	pcurbyte++;
	return value;
}

eDataCodingScheme SMSBuffer::GetCodingScheme()
{
	unsigned char bit0 = Get1Bit();
	unsigned char bit1 = Get1Bit();
	unsigned char bit2 = Get1Bit();
	unsigned char bit3 = Get1Bit();
	unsigned char bit4 = Get1Bit();
	unsigned char bit5 = Get1Bit();
	unsigned char bit6 = Get1Bit();
	unsigned char bit7 = Get1Bit();
	
	if((bit0+bit1+bit2+bit3+bit4+bit5+bit6+bit7)==0)  //GSM default 7bit
		return DCS_DEFAULT;
	
	if((bit7+bit6)==0 || (bit7==0 && bit6==1))  
	{
		if(bit3==0 && bit2==0)
			return DCS_DEFAULT;
		else if(bit3==0 && bit2==1)
			return DCS_8BIT;
		else if( bit3==1 && bit2==0)
			return DCS_UCS2;
	}
	else if(bit7==1 && bit6==1 && bit5==0 && (bit4==0||bit4==1))
		return DCS_DEFAULT;
	else if(bit7==1 && bit6==1 && bit5==1 && bit4==0)
		return DCS_UCS2;
	else if(bit7==1 && bit6==1 && bit5==1 && bit4==1)
	{
		if(bit2==0)
			return DCS_DEFAULT;
		else
			return DCS_8BIT;
	}
	return DCS_DEFAULT;
}

std::string SMSBuffer::GetSemiOctets(const unsigned char len)
{
	std::string str;
	ByteAlign();
	for(int i=0; i<len; i++)
	{
		if (curbit == 0)
		{
			str += ('0'+ (*pcurbyte & 0xf));
			curbit = 4;
		}
		else
		{   
			//1111 is useless
			if ((*pcurbyte & 0xf0) != 0xf0)
				str += ('0'+(*pcurbyte >> 4));
			curbit = 0;
			pcurbyte++;
		}
	}
	ByteAlign();
	return str;
}

std::string SMSBuffer::GetSemiOctets_Addr(const unsigned char len)
{
	std::string str;
	ByteAlign();
	for(int i=0; i<len; i++)
	{
		unsigned char c;
		if (curbit == 0)
		{
			c = *pcurbyte & 0xf;
			if(c == 0xa){
				str += '*';
			}else if(c == 0xb){
				str += '#';
			}else if(c == 0xc){
				str += 'p';
			}else if(c == 0xd){
				str += 'w';
			}else if(c == 0xe){
				str += '+';
			}else{
				str += ('0'+ (*pcurbyte & 0xf));
			}
			curbit = 4;
		}
		else
		{   
			c = *pcurbyte >> 4;
			//1111 is useless
			if ((*pcurbyte & 0xf0) != 0xf0){
				if(c == 0xa){
					str += '*';
				}else if(c == 0xb){
					str += '#';
				}else if(c == 0xc){
					str += 'p';
				}else if(c == 0xd){
					str += 'w';
				}else if(c == 0xe){
					str += '+';
				}else{
					str += ('0'+(*pcurbyte >> 4));
				}
			}
			curbit = 0;
			pcurbyte++;
		}
	}
	ByteAlign();
	return str;
}

unsigned int SMSBuffer::GetSemiOctetsInt(const unsigned char len)
{
	ByteAlign();
	unsigned int value=0;
	for(int i=0; i<len; i++)
	{
		if (curbit == 0)
		{
			value = value * 10 + (*pcurbyte & 0xf);
			curbit = 4;
		}
		else
		{
			//1111 is useless
			if ((*pcurbyte & 0xf0) != 0xf0)
				value = value * 10 + (*pcurbyte >> 4);
			curbit = 0;
			pcurbyte++;
		}
	}
	ByteAlign();
	return value;
}

std::string SMSBuffer::GetString(const int len)
{
	std::string str="";
	
	if(len<=0 || len>sizeof(smsbuf))
		return str;
	
	SeptetAlign();  //align to 7 bit boundary
	for(int i=0; i<len; i++)
	{
		unsigned char c = 0;
		for (unsigned short j = 0; j < 7; j++)
			c |= (Get1Bit() << j);
		str += c;
	}
	return str;
}

GSMAddress SMSBuffer::GetAddr()
{
	GSMAddress addr;
	ByteAlign();
	unsigned char addrlen = GetOctet();
	addr.AddrPlane = (eAddrNumberPlan)GetMultiBits(4); 
	addr.AddrType = (eAddrType)GetMultiBits(3); 
	
	std::string numstr;
	if (addr.AddrType == TON_ALPHANUMERIC)
	{
		SetSeptetStartOctet();
		numstr = GsmToISO8859latin1(GetString(addrlen * 4 / 7));
		ByteAlign();
	}
	else
	{
		// [STP100000974] deal with phone address with '+', '*', '#', 'p' and 'w'
		numstr = GetSemiOctets_Addr(addrlen);
	}
	addr.SetGSMAddress(numstr, addr.AddrType);
	return addr;
}

GSMAddress SMSBuffer::GetSCAddr()
{
	GSMAddress addr;
	ByteAlign();
	unsigned char addrlen = GetOctet();
	if(addrlen == 0)  //default SCA
		return addr;
	addr.AddrPlane = (eAddrNumberPlan)GetMultiBits(4); 
	addr.AddrType = (eAddrType)GetMultiBits(3); 
	
	std::string numstr;
	if (addr.AddrType == TON_ALPHANUMERIC)
	{
		numstr = GsmToISO8859latin1(GetString(addrlen * 4 / 7));
		ByteAlign();
	}
	else
	{
		// [STP100000974] deal with phone address with '+', '*', '#', 'p' and 'w'
		numstr = GetSemiOctets_Addr((addrlen-1)*2);
	}
	addr.SetGSMAddress(numstr);
	return addr;
}

bool SMSBuffer::GetTimeZone(unsigned short &value)
{
	bool isneg = 0;
	ByteAlign();
	for (int i = 0; i < 2; i++)
	{
		if (curbit == 0)
		{
			if (i == 0)
			{
				//check if negtive time zone
				if((*pcurbyte & 0x8) == 0)
					isneg = 1;
				else
					isneg = 0;
				value = value*10 + (*pcurbyte & 0x7);
			}
			else
				value = value*10 + (*pcurbyte & 0xf);
			curbit = 4;
		}
		else  //bit 4
		{
			value = value*10 + (*pcurbyte >> 4);
			curbit = 0;
			pcurbyte++;
		}
	}
	ByteAlign();
	value = value * 15;
	return isneg;
}


GSMTimeStamp SMSBuffer::GetTimeStamp()
{
	GSMTimeStamp ts;
	ts.Year = GetSemiOctetsInt(2);
	ts.Month = GetSemiOctetsInt(2);
	ts.Day = GetSemiOctetsInt(2);
	ts.Hour = GetSemiOctetsInt(2);
	ts.Minute = GetSemiOctetsInt(2);
	ts.Second = GetSemiOctetsInt(2);
	ts.negativeTimezone = GetTimeZone(ts.Timezone);
	
	return ts;
}

int SMSBuffer::CurrentBufLen()
{
	ByteAlign();
	return (int)(pcurbyte-smsbuf);
}

int SMSBuffer::CopyRemainBuff(unsigned char *pbuf)
{
	memcpy(pbuf, pcurbyte, pfinalbyte-pcurbyte+1);
	return (int)(pfinalbyte-pcurbyte+1);
}
