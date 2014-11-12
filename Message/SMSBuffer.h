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
 *  SMSBuffer.h
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


#ifndef MTK_SMSBUFFER_H
#define MTK_SMSBUFFER_H

//!!This header file is not public to this library!!

std::string GsmToISO8859latin1(std::string s);
std::string ISO8859latin1ToGsm(std::string s);

class SMSBuffer
{
public:
	
	SMSBuffer();
	SMSBuffer(const char *buf, int buflen);
	
	//encode
	void Set1Bit(const unsigned char b);
	void SetMultiBits(const unsigned int value, const unsigned char bit_num);
	void SetOctet(const unsigned char c);
	void ByteAlign();
	void SeptetAlign();
	void SetSeptetStartOctet();
	void SetAddr(GSMAddress &addr);
	void SetSCAddr(GSMAddress &addr);
	void SetTimeStamp(const GSMTimeStamp &ts);
	void SetString(const std::string &str);
	void SetSemiOctets(const std::string &str);
	void SetSemiOctetsInt(const unsigned int val, const unsigned char len);
	void SetSemiOctets_Addr(const std::string &str);
	void GetHexString(std::string &str);
	
	//decode
	bool Get1Bit();
	unsigned char GetOctet();
	eDataCodingScheme GetCodingScheme();
	
	std::string GetSemiOctets(const unsigned char len);
	unsigned int GetSemiOctetsInt(const unsigned char len);
	std::string GetSemiOctets_Addr(const unsigned char len);
	unsigned int GetMultiBits(const unsigned char bit_num);
	std::string GetString(const int len);
	GSMAddress GetAddr();
	GSMAddress GetSCAddr();
	GSMTimeStamp GetTimeStamp();
	bool GetTimeZone(unsigned short &value);
	
	
	int CurrentBufLen();
	int CopyRemainBuff(unsigned char *pbuf);
	
protected:
	
	unsigned char smsbuf[512];
	unsigned char *pcurbyte;
	unsigned char *pfinalbyte;
	unsigned char *pSeptetStartOctet;
	unsigned char curbit;
	
private:
	
};

#endif
