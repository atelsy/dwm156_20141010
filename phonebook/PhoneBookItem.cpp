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
*	PhoneBookItem.cpp
*
* Project:
* --------
*	DCT
*
* Description:
* ------------
*	Phonebook Item Class
*
* Author:
* -------
*	Fengping Yu
*
*******************************************************************************/


#include "PhoneBookItem.h"
#include "Win2Mac.h"

efolder_sort_key PhoneBookItem::sort_key;

static BYTE Get0x81Offset(WORD base, WORD code)
{
	if(code < 0x80)
		return (BYTE)code;
	else
		return (BYTE)(code - base + 0x80);
}

/*static */WORD Get0x81Base(TCHAR code)
{
	if ((code >= 0x0E00) && (code <= 0x0E5F))	/* Thai 0E00 ~ 0E5F */
	{
		return 0x0E00;
	}
	if ((code >= 0x0080) && (code <= 0x00FF))	/* C1 Controls and Latin-1 Supplement */
	{
		return 0x0080;
	}
	if ((code >= 0x0100) && (code <= 0x017F))	/* Latin Extended - A */
	{
		return 0x0100;
	}
	if ((code >= 0x0180) && (code <= 0x01FF))	/* Latin Extended - B (Partial) */
	{
		return 0x0180;
	}
	if ((code >= 0x0400) && (code <= 0x047F))	/* Russian 0400 ~ 047F */
	{
		return 0x0400;
	}
	if ((code >= 0x0600) && (code <= 0x067F))	/* Arabic 0600 ~ 06FF */
	{
		return 0x0600;
	}
	if ((code >= 0x0980) && (code <= 0x09FF))	/* Bengali 0980 ~ 09FF */
	{
		return 0x0980;
	}
	if ((code >= 0x0A00) && (code <= 0x0A7F))	/* Punjabi 0A00 ~ 0A7F */
	{
		return 0x0A00;
	}
	if ((code >= 0x0B00) && (code <= 0x0B7F))	/* Oriya 0B00 ~ 0B7F */
	{
		return 0x0B00;
	}
	if ((code >= 0x0B80) && (code <= 0x0BFF))	/* Tamil 0B80 ~ 0BFF */
	{
		return 0x0B80;
	}
	if ((code >= 0x0C00) && (code <= 0x0C7F))	/* Telugu 0C00 ~ 0C7F */
	{
		return 0x0C00;
	}
	
	return 0x0000;
}

static bool Encode0x81(const CString &strSrc, string &result)
{
	BYTE i = 0;
	WORD base = 0;
	WORD code = 0;
	
	ASSERT (strSrc.GetLength() < 255);
	
	result.erase();
	
	/*Find first UCS2 character as base*/
	for (i=0; i<strSrc.GetLength(); i++)
	{
		code = strSrc.GetAt(i);
		if (code >= 0x80)
		{
			base = Get0x81Base(code);
			break;
		}
	}
	
	/*Check if whole string can be encoded as 0x81.*/
	if(base > 0)
	{
		char szBuf[8] = "";
		sprintf(szBuf, "81%02x%02x", (BYTE)strSrc.GetLength(), (BYTE)(base >> 7));
		result = szBuf;
		
		for (i=0; i<strSrc.GetLength(); i++)
		{
			code = strSrc.GetAt(i);
			if((code >= 0x80) && (Get0x81Base(code) != base))
			{
				result.erase();
				return false;
			}
			sprintf(szBuf, "%02x", Get0x81Offset(base, code));
			result += szBuf;
		}
		return true;
	}
	
	return false;
}
//---------------------------------------------------------------------------------------

PhoneBookItem::PhoneBookItem()
{
	m_index = 1;
	m_bCharSet= PBCHS_UCS2;
	memtype_str = _T("PC");
}

bool PhoneBookItem::read(Connection *pct, CString memtype, int index, int &cms_error)
{ 
	cms_error = ER_UNKNOWN;
	
	if(pct == NULL || index < 0 || memtype.IsEmpty())
		return false;
	
	cms_error = pct->SetCurrentPhonebookMemType(memtype);
	if (ER_OK != cms_error)
		return false;
	
	// just to make sure
	cms_error = pct->SetCharacterSet("UCS2");
	if (ER_OK != cms_error)
		return false;
	ATResult atret; 	   
	cms_error = pct->ReadPbk(index, atret);
	if (ER_OK != cms_error)
		return false;
	
	if(atret.resultLst.size() > 0 && atret.resultLst[0].eleLst.size() >= 5)   //+CPBR: <index>, <number>, <type>, <text>
	{ 		   
		if(atret.resultLst[0].eleLst[0].str_value == "+CPBR")
		{												 
			m_index = atret.resultLst[0].eleLst[1].int_value;				 
			if(atret.resultLst[0].eleLst[3].int_value == 145)
				m_addr.SetGSMAddress(atret.resultLst[0].eleLst[2].str_value, TON_INTERNATIONAL);
			else
				m_addr.SetGSMAddress(atret.resultLst[0].eleLst[2].str_value);
			//save the name , single byte only now.
			
			m_name = DecodeUCS2txt(atret.resultLst[0].eleLst[4].str_value);
			memtype_str = memtype;
			
			string atstr;
			if (atret.get_string(atstr, 0, 5))
				m_CHTimeStamp.SetTimeString(atstr);
			
			return true;
		}
	}
	
	cms_error = ER_UNKNOWN;
	return false;
}

bool PhoneBookItem::compose_write_cmd(char *pBuf, const string &name) const
{
	int ton = 129;
	string numstr = m_addr.GetNumStr().c_str();
	
	if(m_addr.AddrType == TON_INTERNATIONAL)
	{
		ton = 145;
		if(numstr[0] == '+')
			numstr.erase((int)0, (int)1);
	}
	
	if (m_index > 0)
	{
		sprintf(pBuf, "%d", m_index);
		pBuf += strlen(pBuf);
	}
	
	if(m_CHTimeStamp.Year == 0)
		sprintf(pBuf, ",\"%s\",%d,\"%s\"", numstr.c_str(), ton, name.c_str());
	else {
		sprintf(pBuf, ",\"%s\",%d,\"%s\",\"%02d/%02d/%02d,%02d:%02d:%02d\"", numstr.c_str(), ton, name.c_str(),
				m_CHTimeStamp.Year, m_CHTimeStamp.Month, m_CHTimeStamp.Day,
				m_CHTimeStamp.Hour, m_CHTimeStamp.Minute, m_CHTimeStamp.Second);
	}

	pBuf += strlen(pBuf);
	
	return true;
}

BYTE PhoneBookItem::encode_name(Connection *pct, string &result) const
{
	switch(m_bCharSet)
	{
	case PBCHS_DEFAULT:
		pct->SetCharacterSet("IRA");
		result = UCS2toASCII(GetName());
		return PBCHS_DEFAULT;

	case PBCHS_0x81:
		if(pct->CheckSupport0x81CharSet())
		{
			if(ER_OK == pct->SetCharacterSet("UCS2_0x81"))
			{
				if(Encode0x81(GetName(), result))
				{
					return PBCHS_0x81;
				}
			}
		}	// no break
	
	case PBCHS_UCS2:
	default:
		pct->SetCharacterSet("UCS2");
		result = EncodeUCS2txt(GetName());
		return PBCHS_UCS2;
	}
}

bool PhoneBookItem::write(Connection *pct, CString memtype, int &cms_error)
{ 
	cms_error = ER_UNKNOWN;

	if(pct == NULL || memtype.GetLength()==0)
		return false;	 
	
	cms_error = pct->SetCurrentPhonebookMemType(memtype);
	if(ER_OK != cms_error)
		return false;		 
	
	char tempbuf[512];
	string namestr;

	encode_name(pct, namestr);
	compose_write_cmd(tempbuf, namestr);

	ATResult atret;
	cms_error = pct->WritePbk(-1, tempbuf, atret);
	pct->SetCharacterSet("UCS2");
	
	if(ER_OK == cms_error)
	{
		memtype_str=memtype;
#ifdef _VCARD_SUPPORT_
		if(pct->CheckSupportAdvPBItem())
		{
			if(memtype == _T("ME") && m_adv_pb_item.HasData())
				return write_adv_items(pct, cms_error);
		}
#endif
//		if(memtype == _T("SM"))
		m_adv_pb_item.Clear();

		return true;
	}
	
	return false;
}

#ifdef _VCARD_SUPPORT_
bool PhoneBookItem::read_adv_items(Connection *pct, int &cms_error) //get advanced items
{	
	//read command
	cms_error = ER_UNKNOWN;

	if(pct == NULL || m_index < 0 || memtype_str!=_T("ME"))
		return false;		
	
	cms_error = pct->SetCurrentPhonebookMemType("ME");
	if(ER_OK != cms_error)
		return false;		 
	
	char tempbuf[64];
	sprintf(tempbuf, "AT+EVCARD=1,%d\r", m_index);

	ATResult atret; 	   
	atret.expectedCommand="+EVCARD";

	cms_error = pct->SendCommand(tempbuf, TIME_OUT_MEDIUM, atret);
	if (ER_OK != cms_error)
		return false;
	
	if(atret.resultLst[0].eleLst.size()>0)	 //+EVCARD: "<file_path>"....OK..
	{			 
		if(atret.resultLst[0].eleLst[0].str_value == "+EVCARD")
		{												 
			m_adv_pb_item.Clear();
			int size = atret.resultLst[0].eleLst.size();
			if(atret.resultLst[0].eleLst.size() < 2)
			{
				cms_error = ER_UNKNOWN;
				return false;
			}
			if(atret.resultLst[0].eleLst[1].str_value != "Error")
			{
				//parse vcard
				CString filename = DecodeUCS2txt(atret.resultLst[0].eleLst[1].str_value);
				if(filename.GetLength() != 0)
				{
					CString tempfile = GetAppPath() + _T("%s\\temppb.vcf");
					if(TFSReadTargetFile(pct, filename, tempfile))
					{
						if(input_vcard(tempfile))
							return true;
					}
				}
			}
		}
	}

	return false;
}

bool PhoneBookItem::write_adv_items(Connection *pct, int &cms_error) //write advanced items
{	
	cms_error = ER_UNKNOWN; 

	if(pct == NULL || m_index < 0 || memtype_str!=_T("ME"))
		return false;		
	
	cms_error = pct->SetCurrentPhonebookMemType(_T("ME"));
	if(ER_OK != cms_error)
		return false;		 
	
	cms_error = ER_UNKNOWN;

	//output the file
	CString tempfile = GetAppPath() + _T("%s\\temppb.vcf");
	if(!output_vcardfile(tempfile))
		return false;
	
	//wirte to target		
	std::vector<CString> drv_str;
	std::vector<unsigned long> drv_avail_size;
	
	if(TFSGetDriveInfo(pct, drv_str, drv_avail_size)==false)
		return false;

	CString target_file;
	for(int i=0;i<drv_str.size();i++)
	{
		target_file = drv_str[i]+_T('\\')+_T("temppb.vcf");
		if(TFSWriteTargetFile(pct,target_file, tempfile)==false)
			continue;	//try next disk!	
		break;
	}
	
	if (i == drv_str.size()) //can't write to target!
		return false;
	
	//write AT Command
	char tempbuf[1024];
	sprintf(tempbuf, "AT+EVCARD=0,%d,\"%s\"\r", 
		m_index, (EncodeUCS2txt(target_file)).c_str());

	cms_error = pct->SetCommand(tempbuf);

	if(ER_OK == cms_error)
	{
		//delete the target temp files
		TFSRemoveTargetFile(pct, target_file);
		return true;
	}

	return false;
}
#endif	// _VCARD_SUPPORT_

int PhoneBookItem::GetIndex() const
{
	return m_index;
}

void PhoneBookItem::SetIndex(int index)
{
	m_index = index;
}

void PhoneBookItem::SetPhone(const CString &str_name, const CString &number, BYTE chs)
{	  
	m_name = str_name;
	m_addr.SetGSMAddress(UCS2toASCII(number));
	m_bCharSet = chs;
}

void PhoneBookItem::GetPhone(CString &str_name, CString &str_number)
{	 
	str_number = m_addr.GetNumStr().c_str();
	str_name = GetName();
}

void PhoneBookItem::GetTime(tm &t)
{
	m_CHTimeStamp.GettmStruct(t);
}

void PhoneBookItem::SetTime(const tm t)
{
	m_CHTimeStamp.SettmStruct(t);
}

const CString &PhoneBookItem::GetMemType(void) const
{
	return memtype_str;	  
}

bool PhoneBookItem::SetMemType(const CString str)
{	 
	memtype_str = str;
	return true;
}

bool PhoneBookItem::EraseItem(Connection *pct, int &err)
{
	err = pct->SetCurrentPhonebookMemType(memtype_str);
	
	if(ER_OK != err)
		return false;
	
	ATResult atret;
	err = pct->WritePbk(m_index, NULL, atret);

	if (ER_OK != err && ER_TIMEOUT != err)
	{
		atret.clear();
		if (ER_OK == pct->ReadPbk(m_index, atret))
		{
			if (!atret.check_key("+CPBR"))
				err = ER_OK;	// deleted
		}
	}

	return (ER_OK == err);
}

bool PhoneBookItem::ReplaceItem(Connection *pct, int &cms_error, PhoneBookItem &newitem)
{
	if(newitem.memtype_str != memtype_str)
	{
		if(!EraseItem(pct, cms_error))
			return false;
	}
	
	if(newitem.write(pct, newitem.memtype_str, cms_error))	  
	{
		*this = newitem;
		return true;
	}
	else
	{
		write(pct, memtype_str, cms_error);
		return false;
	}
}

CString PhoneBookItem::GetName() const
{
	if(m_name.GetLength()!=0)
		return m_name;
	return _T("");
}

void PhoneBookItem::SetSortKey(efolder_sort_key key)
{
	sort_key = key;
}

bool PhoneBookItem::operator<(const PhoneBookItem &rhs)
{
	switch(sort_key)
	{
	case BY_PHONENUMBER:
		return m_addr < rhs.m_addr;

	case BY_NAME:
	default:
		CString str1 = GetName();
		CString str2 = rhs.GetName();
		
		size_t minnum = __min(str1.GetLength(), str2.GetLength());
		
		if(str1.GetLength() > minnum)
			str1 = str1.Mid(0, minnum);
		else if(str2.GetLength() > minnum)
			str2 = str2.Mid(0, minnum); 			  
		return str1 < str2;
	}
}

#ifdef _VCARD_SUPPORT_
/*
bool PhoneBookItem::Export(FILE *fd)
{
	_TCHAR tempbuf[2048];
	
	CString name, telnum;
	GetPhone(name, telnum);
	// use \t instead of , in order to let excel can open unicode text file
	//	_stprintf(tempbuf, _T("\"%s\",\"%s\"\r\n"), name, telnum);
	_stprintf(tempbuf, _T("\"%s\"\t\"%s\"\r\n"), name, telnum);
	
#ifdef _UNICODE
	int ts = 2;
#else
	int ts = 1;
#endif	
	
	if(fwrite(tempbuf, ts, _tcslen(tempbuf), fd) != _tcslen(tempbuf))
		return false;
	return true;
}
*/

bool PhoneBookItem::output_vcardfile(const CString &filename)
{
	FILE *fd = _tfopen(filename, _T("wb"));
	if(fd == NULL)
		return false;
	
	/////BEGIN:VCARD		
	fprintf(fd, "BEGIN:VCARD\r\n");
	/////VERSION:2.1
	fprintf(fd, "VERSION:2.1\r\n"); 
	/////N;CHARSET=UTF-8:
	fprintf(fd, "N;CHARSET=UTF-8:%s\r\n", UCS2toUTF8(m_name).c_str());
	/////FN;CHARSET=UTF-8:
	fprintf(fd, "FN;CHARSET=UTF-8:%s\r\n", UCS2toUTF8(m_name).c_str());
	/////TEL;CELL;VOICE:
	fprintf(fd, "TEL;CELL;VOICE:%s\r\n", m_addr.GetNumStr().c_str());
	/////TEL;HOME;VOICE:
	if(m_adv_pb_item.m_home_num.GetLength())
		fprintf(fd, "TEL;HOME;VOICE:%s\r\n", UCS2toASCII(m_adv_pb_item.m_home_num).c_str());
	/////TEL;WORK;VOICE:
	if(m_adv_pb_item.m_office_num.GetLength())
		fprintf(fd, "TEL;WORK;VOICE:%s\r\n", UCS2toASCII(m_adv_pb_item.m_office_num).c_str());
	/////TEL;FAX:
	if(m_adv_pb_item.m_fax_num.GetLength())
		fprintf(fd, "TEL;FAX;VOICE:%s\r\n", UCS2toASCII(m_adv_pb_item.m_fax_num).c_str());
	/////EMAIL:
	if(m_adv_pb_item.m_email.GetLength())
		fprintf(fd, "EMAIL:%s\r\n", UCS2toASCII(m_adv_pb_item.m_email).c_str());
	/////ORG;CHARSET=UTF-8:
	if(m_adv_pb_item.m_company_name.GetLength())
		fprintf(fd, "ORG;CHARSET=UTF-8:%s\r\n", UCS2toUTF8(m_adv_pb_item.m_company_name).c_str());
	/////BDY
	if((m_adv_pb_item.m_birth_year != 0)||(m_adv_pb_item.m_birth_month != 0)||(m_adv_pb_item.m_birth_day != 0))
		fprintf(fd, "BDAY:%04d%02d%02d\r\n", m_adv_pb_item.m_birth_year, m_adv_pb_item.m_birth_month, m_adv_pb_item.m_birth_day);
	/////END:VCARD
	fprintf(fd, "END:VCARD\r\n");
	
	fclose(fd);
	return true;
}

static string get_value_from_vcard(string &vcard, string property)
{
	string::size_type pos = vcard.find(property);
	if(pos == string::npos)
		return "";
	
	string ret_str;
	for(int i=pos+property.size(); i<vcard.size()-2; i++)
	{
		if(vcard[i]==0x0d && vcard[i+1]==0x0a){
			// [STP100000865] Long lines of text can be split into a multiple-line 
			// representation using the RFC 822 "folding" technique. (CRLF)
			if(vcard[i+2]==0x3d){
				i+=2;
				continue;
			}else{
				break;
			}
		}
		ret_str += vcard[i];
	}	
	return ret_str;
}

static string quoted_printable_to_utf8(string str)
{
	string ret_str;
	for(int i=0;i<str.size();i++)
	{
		if(str[i] == '=' && i+2<str.size())
		{
			unsigned char a1=0, a2=0;
			if (str[i+1] >= '0' && str[i+1] <= '9')
				a1 = str[i+1] - '0';
			else
				a1 = str[i+1] - 'A' + 10;
			
			if (str[i+2] >= '0' && str[i+2] <= '9')
				a2 = str[i+2] - '0';
			else
				a2 = str[i+2] - 'A' + 10;
			
			a1 = (a1<<4) + a2;
			ret_str += a1;
			i+=2;
		}
		else
			ret_str += str[i];
	}
	
	return ret_str;
}

bool PhoneBookItem::input_vcard(const CString &filename)
{
	FILE *fd = _tfopen(filename, _T("rb"));
	if(fd == NULL)
		return false;
	
	string str;
	unsigned char c;
	while(fread(&c, 1, 1, fd))
	{
		str += c;
	}
	fclose(fd);
	
	int n = strncmp(str.c_str(), "BEGIN:VCARD", strlen("BEGIN:VCARD"));
	if(strncmp(str.c_str(), "BEGIN:VCARD", strlen("BEGIN:VCARD"))!=0)
		return false;
	
	m_adv_pb_item.m_home_num = get_value_from_vcard(str, "TEL;HOME;VOICE:").c_str();
	m_adv_pb_item.m_company_name = UTF8toUCS2((unsigned char*)quoted_printable_to_utf8(get_value_from_vcard(str, "ORG;CHARSET=UTF-8;ENCODING=QUOTED-PRINTABLE:")).c_str());
	m_adv_pb_item.m_email = get_value_from_vcard(str, "EMAIL:").c_str();
	m_adv_pb_item.m_office_num = get_value_from_vcard(str, "TEL;WORK;VOICE:").c_str();
	m_adv_pb_item.m_fax_num = get_value_from_vcard(str, "TEL;FAX:").c_str();				
	string bday_str = get_value_from_vcard(str, "BDAY:").c_str();
	int year, month, day;
	if(bday_str.size()){
		sscanf(bday_str.c_str(), "%04d%02d%02d", &year, &month, &day);
		
		m_adv_pb_item.m_birth_year = year;
		m_adv_pb_item.m_birth_month = month;
		m_adv_pb_item.m_birth_day = day;
	}
	return true;
}
#endif	// _VCARD_SUPPORT_

::ostream& operator<<(::ostream &os, const Advanced_PBItem_Struct &pbitem)
{
	if( !os ) return os;	
	
	os << pbitem.m_home_num;
	os << pbitem.m_company_name;
	os << pbitem.m_email;
	os << pbitem.m_office_num;
	os << pbitem.m_fax_num;
	os << pbitem.m_title;
	os << pbitem.m_url;
	os << pbitem.m_address;
	os << pbitem.m_note;
	
	os.write((char *)&pbitem.m_birth_year, sizeof(pbitem.m_birth_year));
	os.write((char *)&pbitem.m_birth_month, sizeof(pbitem.m_birth_month));
	os.write((char *)&pbitem.m_birth_day, sizeof(pbitem.m_birth_day));
	
	return os;
}

::istream& operator>>(::istream &is, Advanced_PBItem_Struct &pbitem)
{
	if( !is ) return is;	
	
	is >> pbitem.m_home_num;
	is >> pbitem.m_company_name;
	is >> pbitem.m_email;
	is >> pbitem.m_office_num;
	is >> pbitem.m_fax_num;
	is >> pbitem.m_title;
	is >> pbitem.m_url;
	is >> pbitem.m_address;
	is >> pbitem.m_note;
	
	is.read((char *)&pbitem.m_birth_year, sizeof(pbitem.m_birth_year));
	is.read((char *)&pbitem.m_birth_month, sizeof(pbitem.m_birth_month));
	is.read((char *)&pbitem.m_birth_day, sizeof(pbitem.m_birth_day));
	
	return is;
}

::ostream& operator<<(::ostream &os, const PhoneBookItem &pb)
{
	if( !os ) return os;	
	
	//GSMAddress m_addr;
	os << pb.m_addr;
	
	//CString m_name;
	os << pb.m_name;
	
	//int m_index;	  
	os.write((char *)&pb.m_index, sizeof(pb.m_index));
	
	//CString memtype_str
	os << pb.memtype_str;
	
	//Advanced_PBItem_Struct m_adv_pb_item;
	os << pb.m_adv_pb_item;
	
	//unsigned char m_bCharSet;
	os.write((char *)&pb.m_bCharSet, sizeof(pb.m_bCharSet));
	
	return os;
}

::istream& operator>>(::istream &is, PhoneBookItem &pb)
{
	if( !is ) return is;			
	
	//GSMAddress m_addr;
	is >> pb.m_addr;
	
	//CString m_name;
	is >> pb.m_name;		  
	
	//int m_index;	  
	is.read((char *)&pb.m_index, sizeof(pb.m_index));
	
	//CString memtype_str
	is >> pb.memtype_str;
	
	//Advanced_PBItem_Struct m_adv_pb_item;
	is >> pb.m_adv_pb_item;
	
	//unsigned char m_bCharSet;
	is.read((char *)&pb.m_bCharSet, sizeof(pb.m_bCharSet));
	
	return is;
}
