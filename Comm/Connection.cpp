/*
 *  Connection.cpp
 *  Comm
 *
 *  Created by mbj on 11-11-21.
 *  Copyright 2011 MediaTek Inc. All rights reserved.
 *
 */

#include "Connection.h"
#include "ATResult.h"
#include "me_osal.h"

///////////////////////////////////////////////////////////////////////////
//functions for UTF8 --> UCS2 from Maui

/* Note that we do not handle UTF-8 BOM here. */
static int widget_utf8_to_ucs2_char(WORD &dest, const BYTE *utf8)
{
	static const BYTE widget_utf8_bytes_per_character[16] = 
	{
		1, 1, 1, 1, 1, 1, 1, 1,
		0, 0, 0, 0, 2, 2, 3, 0 /* we don't support UCS4 */
	};

	BYTE c = utf8[0];
	int count = widget_utf8_bytes_per_character[c>>4];
	
	switch(count)
	{
	case 1:
		dest = c;
		break;
	case 2:
		dest = ((WORD) (c & 0x1f) << 6) | (WORD) (utf8[1] ^ 0x80);
		break;
	case 3:
		dest = ((WORD) (c & 0x0f) << 12)
			| ((WORD) (utf8[1] ^ 0x80) << 6)
			|  (WORD) (utf8[2] ^ 0x80);
		break;
	case 0:
		break;
	default:
		ASSERT(false);
		break;
	}
	
	return count;
}

static int widget_ucs2_to_utf8_char(BYTE *utf8, WORD ucs2)
{
	int count;

	if (ucs2 < (WORD) 0x80)
		count = 1;
	else if (ucs2 < (WORD) 0x800)
		count = 2;
	else
		count = 3;

	switch (count) 
	{
	case 3: utf8[2] = 0x80 | (ucs2 & 0x3f); ucs2 = ucs2 >> 6; ucs2 |= 0x800;
	case 2: utf8[1] = 0x80 | (ucs2 & 0x3f); ucs2 = ucs2 >> 6; ucs2 |= 0xc0;
	case 1: utf8[0] = (BYTE)ucs2;
	}

	utf8[count] = '\0';

	return count;
}

string UCS2toASCII(const CString &str)
{
	string result;

	for (_Size_T i=0; i<str.GetLength(); i++)
	{
		ASSERT (str[i] < 0x80);
		result += (char)(str[i]);
	}

	return result;
}

CString UTF8toUCS2(const BYTE *src)
{
	int cnt;
	WORD wCode = 0;

	CString dest;

	while('\0' != *src)
	{
		cnt = widget_utf8_to_ucs2_char(wCode, src);
		if (cnt == 0) // abnormal case
		{
			break;			
		}
		else // normal case
		{
			dest += (TCHAR)wCode;
			src += cnt;
		}
	}

	return dest;
}

string UCS2toUTF8(const CString &src)
{
	string dest;

	BYTE szBuf[8] = {0};
	size_t len = src.GetLength();
	
	for (_Size_T i=0; i<len; i++)
	{
		widget_ucs2_to_utf8_char(szBuf, src[i]);
		dest += (char*)szBuf;
	}
	return dest;
}

string EncodeUCS2txt(const CString &strSrc)
{
	_Size_T i, len;
	string retstr;
	char temptxt[8] = "";
	
	len = strSrc.GetLength();
	for(i=0; i<len; i++)  //fill data
	{
		sprintf(temptxt, "%04x", (int)strSrc[i]);
		retstr += temptxt;
	}

	return retstr;
}

static BYTE HexToNum(char ch)
{
	if (ch >= '0' && ch <= '9')
		return (ch - '0');
	else if (ch >= 'A' && ch <= 'F')
		return (ch - 'A' + 10);
	else				   
		throw "error";
	return 0;
}

CString DecodeUCS2txt(const string& ucs2txtname)
{
	CString strDst;

	BYTE temp1, temp2;
	int len = (int)ucs2txtname.size()/4;
	
	try
	{
		for(int j=0; j<len; j++) //per word
		{
			temp1 = HexToNum(ucs2txtname[4*j]);
			temp1 = (temp1 << 4) + HexToNum(ucs2txtname[4*j+1]);
			
			temp2 = HexToNum(ucs2txtname[4*j+2]);
			temp2 = (temp2 << 4) + HexToNum(ucs2txtname[4*j+3]);
			
			strDst += MAKEWORD(temp2, temp1);
		}
	}
	catch (...)
	{
		strDst = ucs2txtname.c_str();	
	}

	return strDst;
}

static void FillID(std::vector<CString>& idList, CString str)
{
	_Ssize_T index;
	
	while (!str.IsEmpty())
	{
		index = str.Find(_T(","));

		if (index > 0)
		{
			idList.push_back(str.Left(index));
			str = str.Mid(index + 1);
		}
		else
		{
			idList.push_back(str);
			break;			
		}
	}
}

///////////////////////////////////////////////////////////////////////////
//class Connection

Connection::Connection()
{
	m_comm = NULL;
	m_b0x81CharSet = false;
	m_medrv = NULL;
	m_voice = NULL;
	m_hDeInitMutex = CreateMutex(NULL, FALSE, NULL);
}

Connection::~Connection()
{
	CloseHandle(m_hDeInitMutex);

	if (m_voice != NULL)
		delete m_voice;
}

//////////////////////////////////////////////////////////////////////////

CFSTRUCT::CFSTRUCT(void)
{
	reason	= 0;
	status	= false;
	xclass	= 0;
	type	= 0;
//	satype	= 0;
	time	= 0;
}

BYTE CFSTRUCT::GetClass(void) const
{
	if (status && 0 == xclass)
	{
		return 0x07;	// default class
	}
	return xclass;
}

bool CFSTRUCT::SetTime(int val, BYTE xclass)
{
	if (val > 30)
		val = 30;
	else if (val < 0)
		val = 0;

	val /= 5;
	
	for (int i=0; i<8; i++)
	{
		if (xclass & (0x01<<i))
		{
			time |= (val << (i*4));
		}
	}
	
	return true;
}

int CFSTRUCT::GetTime(BYTE xclass) const
{
	for (int i=0; i<8; i++)
	{
		if (xclass & (0x01<<i))
		{
			return ((time >> (i*4)) & 0x0f) * 5;
		}
	}
	return 0;
}

CString CFSTRUCT::GetNumber(BYTE xclass) const
{
	_Ssize_T pos = -1;
	CString strRes;
	CString strTmp = number;
	for (int i=0; i<8; i++)
	{
		pos = strTmp.Find(_T(';'));
		if (pos != -1)
		{
			strRes = strTmp.Left(pos);
			strTmp = strTmp.Mid(pos+1);
		}
		if (xclass & (0x01<<i))
		{
			break;
		}
	}
	return strRes;
}

bool CFSTRUCT::SetNumber(const CString &num, BYTE xclass)
{
	CString strTmp;
	
	for (_Size_T i=0; i<8; i++)
	{
		if (xclass & (0x01<<i))
			strTmp += num;
		else
			strTmp += GetNumber(0x01 << i);

		strTmp += _T(';');
	}

	number = strTmp;

	if (!num.IsEmpty() && num[(_Size_T)0] == _T('+'))
		type = 145;
	else
		type = 129;
	
	return true;
}
#ifdef WIN32
::ostream& operator<<(::ostream &os, const CString &str)
{
#ifdef _UNICODE
	int str_len = str.GetLength()+1;
	os.write((char*)&str_len, sizeof(str_len));
	os.write((char*)(LPCTSTR)str, str_len*2);	
#else
	int str_len = str.GetLength()+1;
	os.write((char*)&str_len, sizeof(str_len));
	os.write((LPCTSTR)str, str_len);
#endif
	
	return os;
}

::istream& operator>>(::istream &is, CString &str)
{
	int str_len;
	
#ifdef _UNICODE
	is.read((char*)&str_len, sizeof(str_len));
	_TCHAR *buf = new _TCHAR[str_len];
	is.read((char*)buf, str_len*2);
	str = buf;
	delete [] buf;				 
#else
	is.read((char*)&str_len, sizeof(str_len));
	_TCHAR *buf = new _TCHAR[str_len];
	is.read((char*)buf, str_len);
	str = buf;
	delete [] buf;
#endif
	
	return is;
}
#endif  //WIN32

///////////////////////////////////////////////////////////////////////////
//com functions
bool Connection::Internal_Conn_Init(char *port, ME_Callback reply)
{
	if(m_comm != NULL)
		Conn_DeInit();
	
	try 
	{
		m_comm = new CommModule;
	}
	catch(...)
	{
		m_comm = NULL;
		return false;
	}
	
	if(INVALID_HANDLE_VALUE == m_comm->Init_Port(port))
	{
		Conn_DeInit();
		return false;
	}
	
#ifdef WIN32
	m_comm->EnableLog(TRUE);
#endif
	
	m_medrv = ME_Init((void*)m_comm);
	if(m_medrv == NULL)
	{
		Conn_DeInit();
		return false;
	}
	
	m_comm->SetCallBack(DataCallBack, m_medrv);
	ME_RegisterURC(m_medrv, 0, reply, NULL);
	
	m_cur_pbmemtype.Empty();
	m_cur_memtype.Empty();
	m_cur_charset.erase();
	
	if(ER_OK == Query())
		reply(NULL);
	else
	{
		Conn_DeInit();
		return false;
	}
	
	return true;		
}

bool Connection::Conn_Init(char *port, ME_Callback reply)
{
	WaitForSingleObject(m_hDeInitMutex, INFINITE);
	bool ret = Internal_Conn_Init(port, reply);
	ReleaseMutex(m_hDeInitMutex);

	return ret;
}

bool Connection::Conn_DeInit()
{
	WaitForSingleObject(m_hDeInitMutex, INFINITE);
	if(m_comm != NULL)
	{
		m_comm->Deinit_Port();
		delete m_comm;
		m_comm = NULL;
	}
	
	if(m_medrv != NULL)
	{
		ME_DeInit(m_medrv);
		m_medrv = NULL;
	}
	ReleaseMutex(m_hDeInitMutex);
	return true;	
}

bool Connection::Cancel()
{
	return ME_Cancel(m_medrv);
}

bool Connection::Exit()
{
	return Conn_DeInit();
}

bool Connection::Init_Voice(char *port, DataNotify voice_cb, char *pExt)
{
	if(m_voice != NULL)
		DeInit_Voice();
	
	try 
	{
		m_voice = new CommModule;
	}
	catch(...)
	{
		m_voice = NULL;
		return false;
	}
	
	if(!m_voice->Init_Port(port))
	{
		DeInit_Voice();
		return false;
	}
	
	m_voice->SetCallBack(voice_cb, pExt);

	return true;
}

bool Connection::DeInit_Voice()
{
	if(m_voice != NULL)
	{
		m_voice->Deinit_Port();
		delete m_voice;
		m_voice = NULL;
	}

	return true;
}

void Connection::SpeechSender(const char *pData, int nLen)
{
	if(m_voice == NULL)
		return;
	
	m_voice->WriteData(pData, nLen, 1);
}

//////////////////////////////////////////////////////////////////////////
// sms commands
int Connection::SetCurrentSMSMemType(const CString &mem12, const CString &mem3)
{	 
	bool ready = false;
	string szMem, szMem3;
	int ret;
	ATResult atret;

	if((mem12 == m_cur_memtype) && (mem3.IsEmpty()))
		ready = true;
	
	if (ready)
		return ER_OK;
	

	if (!mem12.IsEmpty())
		szMem = UCS2toASCII(mem12);
	else if (!m_cur_memtype.IsEmpty())
		szMem = UCS2toASCII(m_cur_memtype);
	else
		szMem = "SM";

	if(!mem3.IsEmpty())
	{
		szMem3 = UCS2toASCII(mem3).c_str();
		ret = ME_Get_MessageMemoryState(m_medrv, (char*)szMem.c_str(), (char*)szMem3.c_str(), NULL, &atret);
	}
	else
	{
		ret = ME_Get_MessageMemoryState(m_medrv, (char*)szMem.c_str(), NULL, NULL, &atret);		
	}

	if (ER_OK == ret)
		m_cur_memtype = szMem.c_str();
	else
		m_cur_memtype.Empty();
	
	return ret;
}

int Connection::GetCurrentSMSMemType(int idx, CString &mem)
{
	ASSERT(idx <= 3 && idx >= 1);
	ATResult atret;

	int err = ME_Get_CurrentSMSMemType(m_medrv, NULL, &atret);
	
	if (ER_OK != err)
		return err;
	
	if (!atret.check_key("+CPMS"))
		return ER_UNKNOWN;

	if (!atret.get_string(mem, 0, 3*idx-3))
		return ER_UNKNOWN;	
	
	return ER_OK;
}

int Connection::GetTotalSMSNumber(const CString& memtype, int &total, int &used)
{
	ATResult atret;
	const string szMem = UCS2toASCII(memtype);	

	m_cur_memtype.Empty();
	int ret = ME_Get_MessageMemoryState(m_medrv, (char*)szMem.c_str(), NULL, NULL, &atret);
	
	if (ER_OK == ret)
	{	
		//+CPMS: <used1>, <total1>
		if (atret.resultLst[0].eleLst.size() >= 3&&
			atret.check_key("+CPMS") )
		{
			used =	atret.get_integer(0, 1);
			total = atret.get_integer(0, 2);
			m_cur_memtype = memtype;
		}
		else
			ret = ER_UNKNOWN;
	}
	
	return ret;
}

int Connection::GetSMSMemStatus(const CString& memtype, std::vector<int> &vec)
{
	ATResult atret;

	vec.clear();
	int err = ME_Get_SMSMemIndex(m_medrv, (char*)UCS2toASCII(memtype).c_str(), NULL, &atret);
	
	int idx0 = 1, idx1 = 1, used = 0;
	if (ER_OK == err)
	{
		//+EQSI: <mem>,<idx0>,<idx1>,<used>
		if (atret.resultLst[0].eleLst.size() >= 4 &&
			atret.check_key("+EQSI") )
		{
			idx0 = atret.get_integer(0, 2);
			idx1 = atret.get_integer(0, 3);
			used = atret.get_integer(0, 4);
		}
	}
	else
	{
		err = GetTotalSMSNumber(memtype, idx1, used);
	}
	
	if (ER_OK != err)
		return err;
	if (idx1 < idx0 || idx0 < 0 || used < 0)
		return ER_UNKNOWN;
	
	vec.push_back(idx0);	// default: start from 1
	vec.push_back(idx1);
	vec.push_back(used);

	return ER_OK;
}

int Connection::GetSCAddress(CString &sca)
{
	sca.Empty();
	
	ATResult atret;	
	int ret = ME_Get_ServiceCenterAddress(m_medrv, NULL, &atret);
	
	if(ER_OK != ret)
		return ret;
	
	if (!atret.check_key("+CSCA"))
		return ER_UNKNOWN;
	
	atret.get_string(sca, 0, 1);
	
	if(atret.get_integer(0, 2) == 145)  //international
		sca = _T("+") + sca;
	
	return ER_OK;
}

int Connection::SetMessageFormat(int mode)
{
	ATResult atret;		
	return ME_Set_MessageFormat(m_medrv, mode, NULL, &atret);
}

int Connection::SetSMSStatus(int mode /* = 0 */)
{
	ATResult atret;

	return ME_Set_MessageStatus(m_medrv, mode, NULL, &atret);
}

int Connection::SetSCAddress(const CString& sca)
{
	ATResult atret;

	return ME_Set_MessageCA(m_medrv, (char*)UCS2toASCII(sca).c_str(), NULL, &atret);
}

int Connection::SetNewMsgInd(int mode, int mt, int bm, int ds, int bfr)
{
	ATResult atret;
	SMS_INDICATION idc;
	
	idc.mode = mode;
	idc.mt   = mt;
	idc.bm   = bm;
	idc.ds   = ds;
	idc.bfr  = bfr;

	return ME_Set_MessageIndication(m_medrv, &idc, NULL, &atret);
}

// sms commands
//////////////////////////////////////////////////////////////////////////

/////////////////////////////////////////////////////////////////////////
// pbk commands
int Connection::GetTotalPhonebookMemType(std::vector<CString> &vst)
{

	ATResult atret;
	int ret = ME_Get_PbMemType(m_medrv, NULL, &atret);

	if(ER_OK != ret)
		return ret;

	if(!atret.check_key("+CPBS"))
		return ER_UNKNOWN;
	
	for(int i=0; i<atret.resultLst[0].eleLst[1].paramLst.size(); i++)
		vst.push_back(atret.resultLst[0].eleLst[1].paramLst[i].str_value.c_str());

	return ER_OK;
}

int Connection::SetCurrentPhonebookMemType(const CString& memstr)
{	 
	if (memstr == m_cur_pbmemtype)
		return ER_OK;
	
	ATResult atret;
	int ret = ME_Set_PbMemType(m_medrv, (char*)UCS2toASCII(memstr).c_str(), NULL, &atret);
	
	if (ER_OK == ret)
		m_cur_pbmemtype = memstr;
	else
		m_cur_pbmemtype.Empty();
	
	return ret;
}

int Connection::GetTotalPhonebookNumber(const CString& memtype, int *pBuf, UINT uMask /*= 0xFF*/)
{
	ASSERT ((NULL != pBuf) && (uMask & 0xff));
	int ret = SetCurrentPhonebookMemType(memtype);
	
	if (ER_OK != ret)
		return ret;
	
	ATResult atret;
	
	pBuf[0] = 100;	// total
	pBuf[1] = 0;	// used
	pBuf[2] = 20;	// num len
	pBuf[3] = 20;	// name len
	
	if (0x03 != uMask)	// 0x03 means only "total" and "used" are concerned. just use "at+cpbs?"
	{
		ret = ME_Get_PbMemInfo(m_medrv, NULL, &atret);
		
		if(ER_OK != ret)
			return ret;
		
		if(!atret.check_key("+CPBR"))
			return ER_UNKNOWN;

		int x = (int)atret.resultLst[0].eleLst.size();					 
		pBuf[0] = atret.resultLst[0].eleLst[1].int_range_end;
		if(pBuf[0]<0)
			pBuf[0] = atret.resultLst[0].eleLst[1].int_value;
		
		if (x > 3)
		{
			pBuf[2] = atret.resultLst[0].eleLst[2].int_value;
			pBuf[2] = max(pBuf[2], 10);
			pBuf[3] = atret.resultLst[0].eleLst[3].int_value;
			pBuf[3] = max(pBuf[3], 10);
		}
	}
	
	if ((uMask & 0x02) != 0)	// query "used"
	{
		atret.clear();
		ret = ME_Get_PbMemState(m_medrv, NULL, &atret);
		
		if(ER_OK != ret)
			return ret;
		
		if(!atret.check_key("+CPBS"))
			return ER_UNKNOWN;
		
		int n = atret.get_integer(0, 2);
		pBuf[1] = max(0, n);
		
		n = atret.get_integer(0, 3);
		if (n > 0) pBuf[0] = n;
	}

	return ER_OK;
}
// pbk commands
//////////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////////
// dial up
int Connection::SetModemApn(const CString& apn)
{
	ATResult atret;
	PDPCONTEXT context;

	context.cid = 1;
	strcpy(context.pdp_type, "IP");
	strcpy(context.apn, UCS2toASCII(apn).c_str());
	context.pdp_addr[0] = 0;
	context.d_comp = 0;
	context.h_comp = 0;

	return ME_Set_PDPContext(m_medrv, &context, NULL, &atret);
}

int Connection::GetOperator(CString &opname)
{
	int ret = QueryIMSI(opname);

	if (ret != ER_OK)
		return ret;
	
	opname = opname.Left(5);	// 5 or 6 ???

	return ER_OK;
}

int Connection::QueryRATnPS(int &curRat, int &gprsStat, int &ratMode, int &prefRat)
{
	ATResult atret;

	int ret = ME_Query_RATnPS(m_medrv, NULL, &atret);

	if(ER_OK != ret)
		return ret;
	
	if(!atret.check_key("+ERAT"))
		return ER_UNKNOWN;
	
	curRat = atret.get_integer(0, 1);
	gprsStat = atret.get_integer(0, 2);
	ratMode = atret.get_integer(0, 3);
	prefRat = atret.get_integer(0, 4);
	
	if (curRat < 0 || gprsStat < 0 || ratMode < 0)
		return ER_UNKNOWN;

	if (curRat == 255 || ratMode == 255)
		return ER_UNKNOWN;
	
	return ER_OK;
}

// dial up
//////////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////////
// power-on procedure
int Connection::PowerOn(int mode)
{
	ATResult atret;
	int ret = ME_Query_ModemFunctionality(m_medrv, NULL, &atret);
	
	if(ER_OK != ret)
		return ret;
	
#ifndef WIN32
	if(atret.get_integer(0, 1) == 1)
    {
		SetPhoneFunctionality(4);
	}
#endif
	
	///requested by fengping, add by Changshun, 20121008
	ME_Set_CMER(m_medrv, 1, 0, 0, 1, -1, -1, NULL, &atret);
	///

	return SetPhoneFunctionality(mode);
}

int Connection::PowerOff(int mode)
{
	ATResult atret;
	int err = 0;
	
#ifndef WIN32
	err = SetPhoneFunctionality(mode);
#endif
	
	//	20110914 by foil
	if (ER_TIMEOUT != err/* && ER_USERABORT != err*/)
		err = SetSleepMode(true);
	
	if (ER_TIMEOUT != err/* && ER_USERABORT != err*/)
		err = ME_Sync_CallLog(m_medrv, NULL, &atret);
	
	return err;
}

int Connection::SetSleepMode(bool bEnable)
{
	ATResult atret;
	return ME_Set_SleepMode(m_medrv, bEnable, NULL, &atret);
}

int Connection::SetPhoneFunctionality(int mode)
{
	ATResult atret;
	return ME_Set_ModemFunctionality(m_medrv, mode, -1, NULL, &atret);
}
// power-on procedure
//////////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////////
// general
int Connection::InternalSupport0x81Charset(void)
{
	string charset(m_cur_charset);
	
	if (ER_OK == SetCharacterSet("UCS2_0x81"))
		m_b0x81CharSet = true;
	
	m_cur_charset.erase();
	
	return SetCharacterSet(charset.c_str());
}

int Connection::SetCharacterSet(const char *charset)
{
	ATResult atret;
	if (NULL == charset || charset[0] == '\0')
		return ER_UNKNOWN;
	if (charset == m_cur_charset)
		return ER_OK;
	
	int ret = ME_Set_CharacterSet(m_medrv, (char*)charset, NULL, &atret);
	
	if (ER_OK == ret)
		m_cur_charset = charset;
	else
		m_cur_charset.erase();
	
	return ret;
}

int Connection::GetNetworkStatus(int& status)
{
	ATResult atret;	
	int ret = ME_Query_NetwrokReg(m_medrv, NULL, &atret);
	if(ER_OK != ret)
		return ret;
	
	if(!atret.check_key("+CREG"))
		return ER_UNKNOWN;
	
	ASSERT(atret.get_integer(0, 1) > 0);	// urc enabled
	status = atret.get_integer(0, 2);
	
	return ER_OK; 
}

int Connection::GetGPRSStatus(int& status)
{
	ATResult atret;	
	int ret = ME_Query_GPRSState(m_medrv, NULL, &atret);
	if(ER_OK != ret)
		return ret;
	
	if(!atret.check_key("+CGREG"))
		return ER_UNKNOWN;
	
	ASSERT(atret.get_integer(0, 1) > 0);	// urc enabled
	status = atret.get_integer(0, 5);
	
	return ER_OK; 
}

int Connection::SetNetworkRegInd(int gsm, int gprs)
{	
	ATResult atret;
	int ret;

	ret = ME_Set_NetwrokReg(m_medrv, gsm, NULL, &atret);

	if (ret != ER_OK)
	{
		return ret;
	}

	atret.clear();
	return ME_Set_GPRSState(m_medrv, gprs, NULL, &atret);
}

int Connection::SetSignalIndication(int flag)
{
	ATResult atret;
	return ME_Set_SignalLevel(m_medrv, flag, NULL, &atret);
}

int Connection::SetPSBearer(int flag)
{
	ATResult atret;
	return ME_Set_PSBearer(m_medrv, flag, NULL, &atret);
}

int Connection::GetSelectedOperator(int format, int &mode, CString& operId)
{
	ATResult atret;
	
	int ret = ME_Set_Operator(m_medrv, 3, format, NULL, -1, NULL, &atret);
	if (ER_OK != ret)
		return ret;
	
	atret.clear();
	ret = ME_Query_Operator(m_medrv, NULL, &atret);
	
	if(ER_OK != ret)
		return ret;
	
	if(!atret.check_key("+COPS"))
		return ER_UNKNOWN;
	
	mode = atret.get_integer(0, 1);

	if (!atret.get_string(operId, 0, 3))
	{
		operId.Empty();
		return ER_UNKNOWN;
	}
	
	return ER_OK;
}
/***********************20140303 by shao hua *************/


int Connection::SetUccToUserName(int format)
{
	ATResult atret;
	
	int ret = ME_Set_Operator(m_medrv, 3, format, NULL, -1, NULL, &atret);
	if (ER_OK != ret)
		return ret;
	
	return ER_OK;
	
}
/***********************2014 0303 by shao hua *************/
int Connection::GetOperatorList(std::vector<OPERINFOEX> &operList)
{
	ATResult atret;

	// AT+COPS=?
	int ret = ME_Query_OperatorList(m_medrv, NULL, &atret);

	if(ER_OK != ret)
		return ret;
	
	if(!atret.check_key("+COPS"))
		return ER_UNKNOWN;
	
	OPERINFOEX operInfo;
	
	int row = (int)atret.resultLst[0].eleLst.size();

	for (int j = 1; j < row; j ++)
	{
		int z = (int)atret.resultLst[0].eleLst[j].paramLst.size();

		if (z > 4)
		{
			if (atret.resultLst[0].eleLst[j].paramLst[1].type == AT_STRING)
				operInfo.long_name = atret.resultLst[0].eleLst[j].paramLst[1].str_value.c_str();
			else
				operInfo.long_name.Empty();

			if (atret.resultLst[0].eleLst[j].paramLst[3].type == AT_STRING)
				operInfo.operId = atret.resultLst[0].eleLst[j].paramLst[3].str_value.c_str();
			else
				operInfo.operId.Empty();
			
			if (atret.resultLst[0].eleLst[j].paramLst[4].type == AT_INTEGER)
				operInfo.AcT = atret.resultLst[0].eleLst[j].paramLst[4].int_value;
			else
				operInfo.AcT = -1;
			
			operList.push_back(operInfo);
		}
	}
	
	return ER_OK;
}

int Connection::SelectOperator(int mode, const CString &operID, int act)
{
	ATResult atret;
	
	if (mode == 0)	//	auto selection
	{
		int ret = ME_Set_Operator(m_medrv, 0, -1, NULL, -1, NULL, &atret);
		if (ER_OK != ret)
			return ret;
	}
	else if (mode == 1)
	{
		//	manual select by operator's long name.	
		//	must change oper to single bytes first.

		//  note: need to implement
		char* coper = new char[operID.GetLength()+1];	

		_wcstombsz(coper, operID, operID.GetLength()+1);

		int ret = ME_Set_Operator(m_medrv, 1, 2, coper, act, NULL, &atret);
		delete[] coper;

		if(ER_OK != ret)
			return ret;
	}
	
	return ER_OK;
}
int Connection::SetErrorFormat(int mode)
{
	ATResult atret;
	return ME_Set_ErrorFmt(m_medrv, mode, NULL, &atret);
}

int Connection::Query(void)
{
	ATResult atret;
	return ME_Query_Connection(m_medrv, NULL, &atret);
}

int Connection::SetDCEEcho(bool Isable)
{
	ATResult atret;
	return ME_Set_ModemFunc(m_medrv, NULL, &atret);
}

int Connection::GetIndicationControl(int& ind)
{
	ATResult atret;
	
	if (ER_OK == ME_Get_IndicationCtrl(m_medrv, NULL, &atret))
	{
		ind = atret.get_integer(0, 1);
		if (ind < 0)
			return ER_UNKNOWN;
	}
	
	return ER_OK;
}

int Connection::SetIndicationControl(int ind)
{
	ATResult atret;
	return ME_Set_IndicationCtrl(m_medrv, ind, NULL, &atret);
}

/*
int Connection::SetTargetTime(const CTime &t)
{
	ATResult atret;
	char tempbuf[64];
	
	sprintf(tempbuf,"%02d/%02d/%02d,%02d:%02d:%02d",
		t.GetYear() % 100, t.GetMonth(), t.GetDay(),
		t.GetHour(), t.GetMinute(), t.GetSecond());
	
	return ME_Set_TargetTime(m_medrv, tempbuf, NULL, &atret);	
}
*/
// general
//////////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////////
// sms
int Connection::ReadSMS(int idx, ATResult &atret)
{
	return ME_Read_Message(m_medrv, idx, NULL, &atret);
}

int Connection::SendSMS(int length, char *pdu, ATResult &atret)
{
	return  ME_Send_Message(m_medrv, pdu, length, NULL, &atret);
}

int Connection::DeleteSMS(int idx, ATResult &atret)
{
	return ME_Delete_Message(m_medrv, idx, -1, NULL, &atret);
}

int Connection::WriteSMS(int length, int stat, char *pdu, ATResult &atret)
{
	return ME_Write_Message(m_medrv, pdu, length, stat, NULL, &atret);
}
// sms
//////////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////////
// pbk

int Connection::ReadPbk(int idx, ATResult &atret)
{
	return ME_Read_Pbk(m_medrv, idx, NULL, &atret);
}
int Connection::WritePbk(int idx, char *str, ATResult &atret)
{
	return ME_Write_Pbk(m_medrv, idx, str, NULL, &atret);
}
// pbk
//////////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////////
// ss & setting
int Connection::QueryIMSI(CString& strIMSI)
{
	ATResult atret;
	int ret = ME_Query_IMSI(m_medrv, NULL, &atret);

	if (ER_OK != ret)
	{
		return ret;
	}

	if(atret.resultLst.size() >= 1)
	{
		atret.get_string(strIMSI, 0, 0);
	}
	else
	{
		strIMSI.Empty();
	}

	return ER_OK;
}

int Connection::QueryIMEI(CString& strIMEI)
{
	ATResult atret;
	int ret = ME_Query_IMEI(m_medrv, NULL, &atret);

	if(ER_OK != ret)
		return ret;
	
	if (atret.resultLst.size() >= 1)
	{
		/*
		at+cgsn
		135790246811220
		OK
		*/
		atret.get_string(strIMEI, 0, 0);
	}
	else
	{
		/*
		at+cgsn
		OK
		*/
		strIMEI.Empty();
	}
	
	return ER_OK;

}

int Connection::GetVersionInfo(CString& strVer)
{
	ATResult atret;

	int ret = ME_Query_VERNO(m_medrv, NULL, &atret);
	if(ER_OK != ret)
		return ret;

	if (!atret.get_string(strVer, 0, 1))
		return ER_UNKNOWN;
	
	return ER_OK;
}

/************ 20131109 by Zhuwei **********************************************/
int Connection::GetVERNO(CString& strVer)
{
    ATResult atret;
    
	int ret = ME_Query_VERNO(m_medrv, NULL, &atret);
	if(ER_OK != ret)
		return ret;
    
	if (!atret.get_string(strVer, 0, 1))
		return ER_UNKNOWN;
	
	return ER_OK;
}
/******************************************************************************/

int Connection::GetSIMLock(BOOL &bLocked)
{
	ATResult atret;
	int err = ME_Set_FacLock(m_medrv, "SC", 2, NULL, -1, NULL, &atret);
	
	if(ER_OK != err)
		return err;

	if (!atret.check_key("+CLCK"))
		return ER_UNKNOWN;

	if (atret.get_integer(0, 1) > 0)
		bLocked = TRUE;
	else
		bLocked = FALSE;
	
	return ER_OK;
}

int Connection::SetSIMLock(BOOL bLocked, const CString &pwd)
{
	ATResult atret;
	return ME_Set_FacLock(m_medrv, "SC", (bLocked ? 1 : 0), (char*)UCS2toASCII(pwd).c_str(), -1, NULL, &atret);
}

#ifdef _MTK_SS_ECUSD_
bool Connection::ComposeCBCmd(char *szBuf, const char *fac, int mode, 
							  int xclass, const CString &pwd)
{
	ASSERT (NULL != szBuf);
	ASSERT (NULL != fac);
	ASSERT (mode >= 0);
	ASSERT (mode <= 2);
	char *pStr = szBuf;

	static const char *szMode[] = { "#", "*", "*#" };

	static const char *szFac[][2] =
	{
		"33",  "AO",	// BAOC (Bar All Outgoing Calls)
		"331", "OI",	// BOIC (Bar Outgoing International Calls)
		"332", "OX",	// BOIC-exHC (Bar Outgoing International Calls except to Home Country)
		"35",  "AI",	// BAIC (Bar All Incoming Calls)
		"351", "IR",	// BIC-Roam (Bar Incoming Calls when Roaming outside the home country)
		"330", "AB",	// All Barring services (applicable only for <mode>=0)
		"333", "AG",	// All outGoing barring services (applicable only for <mode>=0)
		"353", "AC"		// All inComing barring services (applicable only for <mode>=0)
	};

	for (int i=0; i<sizeof(szFac)/(2*sizeof(char*)); i++)
	{
		if (stricmp(fac, szFac[i][1]) == 0)
		{
			sprintf(pStr, "AT+ECUSD=1,1,\"%s%s", szMode[mode], szFac[i][0]);
			break;
		}
	}

	pStr += strlen(pStr);

	if (!pwd.IsEmpty())
		sprintf(pStr, "*%s", UCS2toASCII(pwd).c_str());
	else if (xclass > 0)
		sprintf(pStr, "*0000");	// empty pwd not accepted by target

	pStr += strlen(pStr);

	if (xclass > 0)
		sprintf(pStr, "*%d", xclass);

	strcat(pStr, "#\"\r");

	return true;
}
#endif

int Connection::GetCallBarring(int& status, const char *facility, int xclass)
{
	if (NULL == facility)
		return ER_UNKNOWN;

	char tempbuf[32];
	status = 0;

#ifndef _MTK_SS_ECUSD_
	if(xclass > 0)
		sprintf(tempbuf, "AT+CLCK=\"%s\",2,,%d\r", facility, xclass);
	else
		sprintf(tempbuf, "AT+CLCK=\"%s\",2\r", facility);
#else
	ComposeCBCmd(tempbuf, facility, 2, xclass, _T(""));
#endif

	ATResult atret;
	int err = ME_Get_CallBarring(m_medrv, tempbuf, NULL, &atret);

	
	if(ER_OK != err)
		return err;

	for(int i = 0; i < atret.resultLst.size(); i++)
	{
		if(atret.resultLst[i].eleLst[0].str_value != "+CLCK")
		{
			return ER_UNKNOWN;
		}
		if(atret.get_integer(i, 1) == 1)
		{
			int nClass = atret.get_integer(i, 2);
			if (nClass >= 0)
				status |= nClass;
			else
				status = max(1, xclass);
		}
	}
	
	return ER_OK;
}

int Connection::SetCallBarring(const char *facility, BOOL bLock, 
				   const CString& pwd, int xclass)
{
	if (NULL == facility)
		return ER_UNKNOWN;

	char tempbuf[128];
	ATResult atret;
	ASSERT (!pwd.IsEmpty());

#ifndef _MTK_SS_ECUSD_
	if(xclass > 0)
	{
		sprintf(tempbuf, "AT+CLCK=\"%s\",%d,\"%s\",%d\r", 
			facility, (bLock ? 1 : 0), UCS2toASCII(pwd).c_str(), xclass);
	}
	else
	{
		sprintf(tempbuf, "AT+CLCK=\"%s\",%d,\"%s\"\r", 
			facility, (bLock ? 1 : 0), UCS2toASCII(pwd).c_str());
	}
#else
	ComposeCBCmd(tempbuf, facility, (bLock ? 1 : 0), xclass, pwd);
#endif
	
	return ME_Set_CallBarring(m_medrv, tempbuf, NULL, &atret);
}

int Connection::ChangePassword(const CString& facility, 
							   const CString& old_pwd, 
							   const CString& new_pwd)
{
	ATResult atret;
	return ME_Set_Password(m_medrv,
		(char*)UCS2toASCII(facility).c_str(),
		(char*)UCS2toASCII(old_pwd).c_str(),
		(char*)UCS2toASCII(new_pwd).c_str(),
		NULL, &atret);
}

int Connection::SetCLIP(int mode)
{
/*	//no used
	char tempbuf[32];
	sprintf(tempbuf,"AT+CLIP=%d\r", mode);	
	return SetCommand(tempbuf, "+CLIP");
*/
	return ER_OK;
}

int Connection::SetCOLP(int mode)
{
/*  //no used
	char tempbuf[32];
	sprintf(tempbuf,"AT+COLP=\r");
	return SetCommand(tempbuf, "+COLP");
*/
	return ER_OK;
}

int Connection::GetCLIR(int& n, int& m)
{

	ATResult atret;

	int ret = ME_Get_CLIR(m_medrv, NULL, &atret);
	if (ER_OK != ret)
		return ret;

	if(!atret.check_key("+CLIR"))
		return ER_UNKNOWN;

	n = atret.get_integer(0, 1);
	m = atret.get_integer(0, 2);		
	
	return ER_OK;
}

int Connection::SetCLIR(int mode)
{
	ATResult atret;
	return ME_Set_CLIR(m_medrv, mode, NULL, &atret);
}

#ifndef _MTK_SS_ECUSD_
bool Connection::ComposeCFCmd(char *szBuf, int mode, const CFSTRUCT &cfinfo)
{
	ASSERT (NULL != szBuf);
	char *pStr = szBuf;

	sprintf(pStr, "AT+CCFC=%d,%d", cfinfo.reason, mode);
	pStr += strlen(pStr);

	const CString strNum = cfinfo.GetNumber();

	if (!strNum.IsEmpty())
	{
		sprintf(pStr, ",\"%s\"", UCS2toASCII(strNum).c_str());
		pStr += strlen(pStr);

		if (cfinfo.GetType() != 0)
			sprintf(pStr, ",%d", (int)(cfinfo.GetType()));
		else if (cfinfo.GetTime() != 0 || cfinfo.xclass != 0)
			sprintf(pStr, ",");

		pStr += strlen(pStr);

		if (cfinfo.xclass != 0)
			sprintf(pStr, ",%d", (int)(cfinfo.xclass));
		else if (cfinfo.GetTime() != 0)
			sprintf(pStr, ",");

		pStr += strlen(pStr);

		if (cfinfo.GetTime() != 0)
		{
			sprintf(pStr, ",,,%d", cfinfo.GetTime());
			pStr += strlen(pStr);
		}
	}
	else if (cfinfo.xclass != 0)
	{
		sprintf(pStr, ",,,%d", (int)(cfinfo.xclass));
		pStr += strlen(pStr);
	}

	sprintf(pStr, "\r");
	
	return true;
}
#else
bool Connection::ComposeCFCmd(char *szBuf, int mode, const CFSTRUCT &cfinfo)
{
	ASSERT (NULL != szBuf);
	ASSERT (mode >= 0);
	ASSERT (mode <= 4);
	ASSERT (cfinfo.reason >= 0);
	ASSERT (cfinfo.reason <= 5);

	char *pStr = szBuf;

	static const char *szMode[] = { "#", "*", "*#", "**", "##" };

	static const char *szReason[] =
	{
		"21",	// 0	unconditional
		"67",	// 1	mobile busy
		"61",	// 2	no reply
		"62",	// 3	not reachable
		"002",	// 4	all call forwarding (refer 3GPP TS 22.030 [19])
		"004"	// 5	all conditional call forwarding (refer 3GPP TS 22.030 [19])
	};

	sprintf(pStr, "AT+ECUSD=1,1,\"%s%s", szMode[mode], szReason[cfinfo.reason]);
	pStr += strlen(pStr);

	const CString strNum = cfinfo.GetNumber();

	if (!strNum.IsEmpty())
	{
		sprintf(pStr, "*%s", UCS2toASCII(strNum).c_str());
		pStr += strlen(pStr);

		if (cfinfo.xclass > 0)
			sprintf(pStr, "*%d", (int)(cfinfo.xclass));
		else if (cfinfo.GetTime() != 0)
			sprintf(pStr, "*");
		
		pStr += strlen(pStr);
		
		if (cfinfo.GetTime() != 0)
		{
			sprintf(pStr, "*%d", cfinfo.GetTime());
			pStr += strlen(pStr);
		}
	}
	else if (cfinfo.xclass > 0)
	{
		sprintf(pStr, "**%d", (int)(cfinfo.xclass));
		pStr += strlen(pStr);
	}

	sprintf(pStr, "#\"\r");
	
	return true;
}
#endif

int Connection::GetCallForward(CFSTRUCT &cfinfo)
{
	char tempbuf[32];

	ComposeCFCmd(tempbuf, 2, cfinfo);
	cfinfo.xclass = 0;

	ATResult atret;
	int ret = ME_Get_CallForward(m_medrv, tempbuf, NULL, &atret);
	
	if(ER_OK != ret)
		return ret;
	
	// +CCFC: <status>,<class>[,<number>,<type>[,<subaddr>,<satype>[,<time>]]]

	//////////////////////////////////////////////////////////////////////////
	for(int i = 0; i < atret.resultLst.size(); i++)
	{
		const vector<ATParamElem> &eleLst = atret.resultLst[i].eleLst;
	
		if (eleLst.size() < 3 ||
			eleLst[0].str_value != "+CCFC")
		{
			ret = ER_UNKNOWN;
			break;
		}

		const bool status = (eleLst[1].int_value > 0);
		const BYTE xclass = (BYTE)(eleLst[2].int_value);
		if (status && xclass > 0 && eleLst.size() > 3)
		{
			cfinfo.xclass |= xclass;
			cfinfo.SetNumber(eleLst[3].str_value.c_str(), xclass);
			if (eleLst.size() > 7)
				cfinfo.SetTime(eleLst[7].int_value, xclass);
		}
	}
	//////////////////////////////////////////////////////////////////////////

	cfinfo.status = (cfinfo.xclass != 0);

	return ret;
}

int Connection::SetCallForward(const CFSTRUCT &cfinfo)
{
	char tempbuf[512];
	ATResult atret;

	ComposeCFCmd(tempbuf, cfinfo.status, cfinfo);
	return ME_Set_CallForward(m_medrv, tempbuf, NULL, &atret);
}

int Connection::GetCallWaiting(int &nStatus, int xclass)
{
	char tempbuf[32];

#ifndef _MTK_SS_ECUSD_
	if (xclass > 0)
		sprintf(tempbuf, "AT+CCWA=1,2,%d\r", xclass);
	else
		sprintf(tempbuf, "AT+CCWA=1,2\r");
#else
	if (xclass > 0)
		sprintf(tempbuf, "AT+ECUSD=1,1,\"*#43*%d\"\r", xclass);
	else
		sprintf(tempbuf, "AT+ECUSD=1,1,\"*#43#\"\r");
#endif

	ATResult atret;
	int ret = ME_Get_CallWaiting(m_medrv, tempbuf, NULL, &atret);
	if(ER_OK != ret)
		return ret;

	size_t size = atret.resultLst.size();
	if (size < 1)
		return ER_UNKNOWN;

	nStatus = 0;
	for(int i=0; i<size; i++)
	{
		if (atret.resultLst[i].eleLst.size() < 3)
			return ER_UNKNOWN;
		if (atret.resultLst[i].eleLst[0].str_value != "+CCWA")
			return ER_UNKNOWN;

		if (atret.resultLst[i].eleLst[1].int_value != 0)
			nStatus |= atret.resultLst[i].eleLst[2].int_value;
	}
	
	return ret;
}

int Connection::SetCallWaiting(bool bEnable)
{
	char tempbuf[32];
	ATResult atret;

	sprintf(tempbuf, "AT+CCWA=%d\r", bEnable ? 1 : 0);
	return ME_Set_CallWaiting(m_medrv, tempbuf, NULL, &atret);
}

int Connection::SetCallWaiting(int mode, int xclass)
{
	char tempbuf[32];
	ATResult atret;

#ifndef _MTK_SS_ECUSD_
	if(xclass > 0)
		sprintf(tempbuf, "AT+CCWA=1,%d,%d\r", mode, xclass);
	else 
		sprintf(tempbuf, "AT+CCWA=1,%d\r", mode);
#else
	if(xclass > 0)
		sprintf(tempbuf, "AT+ECUSD=1,1,\"%s43*%d#\"\r", (mode ? "*" : "#"), xclass);
	else 
		sprintf(tempbuf, "AT+ECUSD=1,1,\"%s43#\"\r", (mode ? "*" : "#"));
#endif

	return ME_Set_CallWaiting(m_medrv, tempbuf, NULL, &atret);
}

int Connection::SetSSNotification(int n, int m)
{
	ATResult atret;
	return ME_Set_SSNotification(m_medrv, n, m, NULL, &atret);
}
// ss & setting
//////////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////////
// STK
#ifdef _STK_SUPPORT_

int Connection::STKResponse(const CString &strCmd)
{
	ATResult atret;
	return ME_STKResponse(m_medrv, (char*)UCS2toASCII(strCmd).c_str(), NULL, &atret);
}

int Connection::STKEnvelope(const CString &strCmd)
{
	ATResult atret;
	return ME_STKEnvelope(m_medrv, (char*)UCS2toASCII(strCmd).c_str(), NULL, &atret);
}

int Connection::STKSetupCall(int option)
{
	ATResult atret;
	return ME_STKSetupCall(m_medrv, option, NULL, &atret);
}

int Connection::STKSetupSS(int option)
{
	ATResult atret;
	return ME_STKSetupSS(m_medrv, option, NULL, &atret);
}

int Connection::STKSetupUSSD(int option)
{
	ATResult atret;
	return ME_STKSetupUSSD(m_medrv, option, NULL, &atret);
}

int Connection::STKSendSMS(int option)
{
	ATResult atret;
	return ME_STKSendSMS(m_medrv, option, NULL, &atret);
}

int Connection::STKSendDTMF(int option)
{
	ATResult atret;
	return ME_STKSendDTMF(m_medrv, option, NULL, &atret);
}

int Connection::STKRunAtCmd(int option)
{
	ATResult atret;
	return ME_STKRunAtCmd(m_medrv, option, NULL, &atret);
}

//---------------------------------------------------//
int Connection::QuerySIMFile(int fileId, int &fileLen, int &recLen)
{
	ATResult atret;

	fileLen = 0;
	recLen  = 0;

	// get response [3GPP TS 11.11 9.2.18]
	// AT+CRSM=192,<file id>,0,0,<lgth>

	//sprintf(tempbuf,"AT+CRSM=192,%d\r", fileId);

	// response format [3GPP TS 11.11 9.2.1]
	// byte 3-4: file size
	// byte 15: length of record

	int err = ME_Restricted_SIM(m_medrv, 192, fileId, -1, -1, -1, NULL, &atret);;

	if (ER_OK != err)
		return err;

	string strResp;
	atret.get_string(strResp, 0, 3);

	const char *p = strResp.c_str();
	if (strlen(p) < 8)
		return ER_UNKNOWN;

	sscanf(p+4, "%04x", &fileLen);

	if (strlen(p) >= 30)
	{
		sscanf(p+28, "%02x", &recLen);
	}

	return ER_OK;
}

int Connection::ReadSIMRecord(int fileId, int recIdx, string &record)
{
#ifdef _STK_SIMULATE_
	if (0x4f20 == fileId && recIdx >0 && recIdx <= 5)
	{
		// default SIM EF(img) [3GPP TS 51.010-4 27.22.2A]
		const char *p[5] =
		{
			"010808114F040000000AFFFFFFFFFFFFFFFFFFFF",
			"010808214F0200000016FFFFFFFFFFFFFFFFFFFF",
			"011810114F0300000032FFFFFFFFFFFFFFFFFFFF",
			"012E28114F01000000E8FFFFFFFFFFFFFFFFFFFF",
			"010505114F0500000008FFFFFFFFFFFFFFFF"
		};
		
		record = p[recIdx-1];
		return ER_OK;
	}
#endif

	int fileLen = 0;
	int recLen  = 0;

	int err = QuerySIMFile(fileId, fileLen, recLen);

	if (ER_OK != err)
		return err;
	if (fileLen <= 0 || recLen <= 0)
		return ER_UNKNOWN;
	if (recIdx > fileLen/recLen)
		return ER_UNKNOWN;

	ATResult atret;

	// read record [3GPP TS 11.11 9.2.5]
	// AT+CRSM=178,<file id>,<rec.no.>,<mode>,<lgth>
	// <mode> = 4: absolute mode

//	atret.expectedCommand = "+CRSM";
//	sprintf(tempbuf,"AT+CRSM=178,%d,%d,4,%d\r", fileId, recIdx, recLen);

	err = ME_Restricted_SIM(m_medrv, 178, fileId, recIdx, 4, recLen, NULL, &atret);

	if (ER_OK != err)
		return err;

	if (!atret.get_string(record, 0, 3))
		return ER_UNKNOWN;

	return ER_OK;
}

int Connection::ReadSIMFile(int fileId, string &content)
{
#ifdef _STK_SIMULATE_
	if (fileId >= 0x4f01 && fileId <= 0x4f05)
	{
		// default SIM images [3GPP TS 51.010-4 27.22.2A]
		const char *p[5] =
		{
			"2E280000000000000001FF800000000FFF0000000077FE00"
			"000001BFF800000006FFE00000001A03800000006BF6BC00"
			"0001AFD838000006BF602000001AFD804000006BF6008000"
			"01A01F02000006FFE40400001BFF901000006DEE40400001"
			"BFF90100006FFFE40400001BFF901000006FFE40400001BF"
			"F901000006FFE60400001BFF881000006FFE20400001BFF8"
			"66000006FFE0F000001BFF808000007FFE00000003000C00"
			"00001FFFF800000000000000000000000000000000000000"
			"1C210844EE0048C431922001251145508007144515438012"
			"711C4D08004A2489322001C89E244EE0",

			"080802030016AAAA800285428142814281528002AAAAFF00"
			"0000FF000000FF",

			"1810FFFFFF8000018000018000018F3CF189208189208189"
			"20F18920118920118920118F3CF1800001800001800001FF"
			"FFFF",

			"0808FF03A59999A5C3FF",

			"0505FEEBBFFFFFFF"
		};
		
		content = p[fileId-0x4f01];
		return ER_OK;
	}
#endif

	int fileLen = 0;
	int recLen  = 0;

	int err = QuerySIMFile(fileId, fileLen, recLen);

	if (ER_OK != err)
		return err;
	if (fileLen <= 0 || recLen > 0)
		return ER_UNKNOWN;


	ATResult atret;
	string strTmp;

	// read binary [3GPP TS 11.11 9.2.3]
	// AT+CRSM=176,<file id>,<offset high>,<offset low>,<lgth>
	// <lgth> = 0-255, but 0 stands for 256
	
	for (int i=0; fileLen > 0; i++)
	{
		recLen = (fileLen >= 256) ? 0 : fileLen;

		//atret.expectedCommand = "+CRSM";
		//sprintf(tempbuf,"AT+CRSM=176,%d,%d,0,%d\r", fileId, i, recLen);
		
		err = ME_Restricted_SIM(m_medrv, 176, fileId, i, 0, recLen, NULL, &atret);
		
		if (ER_OK != err)
			return err;
		
		if (!atret.get_string(strTmp, 0, 3))
			return ER_UNKNOWN;
		
		content += strTmp;
		fileLen -= 256;

		atret.clear();
	}

	return ER_OK;
}

int Connection::GetSIMImage(int imgId, HBITMAP &hImage)
{
	string data;

	int err = ReadSIMRecord(0x4f20, imgId, data);

	if (ER_OK != err)
		return err;
	if (data.length() < 10*2)
		return ER_UNKNOWN;

	const char *p = data.c_str() + 2;

	int w, h, coding, instId, offset, len;
	sscanf(p, "%02x%02x%02x%04x%04x%04x", 
		&w, &h, &coding, &instId, &offset, &len);

	data = "";
	err = ReadSIMFile(instId, data);

	if (ER_OK != err)
		return err;
	if (data.length() < (offset+len)*2)
		return ER_UNKNOWN;

	p = data.c_str() + offset*2;
	sscanf(p, "%02x%02x", &w, &h);	// this is the real dimension

	if (w <= 0 || h <= 0)
		return ER_UNKNOWN;

	p += 2*2;

	bool bRes = false;
	BYTE *pBuf = new BYTE[w*h*4+4];

	if (0x11 == coding)
		bRes = DecodeBasicImage(p, pBuf, w*h*4);
	else if (0x21 == coding)
		bRes = DecodeColorImage(p, pBuf, w*h*4);

	if (bRes)
		hImage = CreateBitmap(w, h, 1, 32, pBuf);
	
	delete[] pBuf;

	return (bRes ? ER_OK : ER_UNKNOWN);
}

int Connection::SetPixel(BYTE *pBuf, BYTE r, BYTE g, BYTE b)
{
	pBuf[0] = r;
	pBuf[1] = g;
	pBuf[2] = b;
	pBuf[3] = 0;
	return 4;
}

// refer to [3GPP TS 11.11 Annex G]
bool Connection::DecodeBasicImage(const char *p, BYTE *pBuf, UINT bufLen)
{
	int i;

	UINT nByte = 0;
	BYTE *pCur = pBuf;

	while (('\0' != *p) && (pCur-pBuf < bufLen))
	{
		sscanf(p, "%02x", &nByte);
		
		for (i=0; i<8 && (pCur-pBuf < bufLen); i++)
		{
			if ((nByte & (0x80 >> i)) != 0)
				pCur += SetPixel(pCur, 0xff, 0xff, 0xff);	// white?
			else
				pCur += SetPixel(pCur, 0, 0, 0);			// black?
		}
		
		p += 2;
	}
	
	*pCur = '\0';

	return (pCur-pBuf == bufLen);
}

// refer to [3GPP TS 11.11 Annex G]
bool Connection::DecodeColorImage(const char *p, BYTE *pBuf, UINT bufLen)
{
	const char *pCLUT = p;
	UINT bpr, nCLUT, offset;

	sscanf(p, "%02x%02x%04x", &bpr, &nCLUT, &offset);
	ASSERT (bpr <= 8);

	if (0 == nCLUT)
		nCLUT = 256;
	if (offset <= 6)
		return false;

	pCLUT += (offset-2)*2;	// offset to the beginning of the EF
	p += 4*2;

	if (strlen(pCLUT) < nCLUT*2*3)
		return false;

	UINT nTmp = 0;
	UINT nByte = 0;
	BYTE *pCur = pBuf;

	int R, G, B;
	int freeBit = 8*sizeof(UINT);
	const UINT mask = ~((~((UINT)0)) >> bpr);

	while (('\0' != *p) && (pCur-pBuf < bufLen))
	{
		while ((freeBit >= 8) && (sscanf(p, "%02x", &nTmp) > 0))
		{
			freeBit -= 8;
			nTmp = (nTmp & 0xff) << freeBit;
			nByte |= nTmp;
			p += 2;
		}

		while ((8*sizeof(UINT) - freeBit >= bpr) && (pCur-pBuf < bufLen))
		{
			nTmp = (nByte & mask) >> (8*sizeof(UINT)-bpr);
			ASSERT (nTmp < nCLUT);

			sscanf(pCLUT+3*nTmp*2, "%02x%02x%02x", &R, &G, &B);
			pCur += SetPixel(pCur, R, G, B);

			nByte <<= bpr;
			freeBit += bpr;	
		}		
	}

	*pCur = '\0';

	return (pCur-pBuf == bufLen);
}

#endif // _STK_SUPPORT_
//---------------------------------------------------//

// STK
//////////////////////////////////////////////////////////////////////////


//////////////////////////////////////////////////////////////////////////
// audio control

int Connection::GetMuteControl(int& Isable)
{
	ATResult atret;
	
	int ret = ME_Get_MuteCtrl(m_medrv, NULL, &atret);
	
	if( ER_OK == ret) 
	{
		if(atret.check_key("+CMUT"))
			Isable = atret.get_integer(0, 1);
		else
			ret = ER_UNKNOWN;
	}
	
	return ret; 	
}

int Connection::GetVibratorMode(int& Isable)
{	
	ATResult atret;
	int ret = ME_Get_VibratorMode(m_medrv, NULL, &atret);

	if(ER_OK == ret)
	{ 
		if(atret.check_key("+CVIB"))
			Isable = atret.get_integer(0, 1);
		else
			ret = ER_UNKNOWN;
	}
	
	return ret;
}

int Connection::MuteControl(int mode)
{
	ATResult atret;
	return ME_Set_MuteControl(m_medrv, mode, NULL, &atret);
}

int Connection::GetRingLevel(int& level)
{
	ATResult atret;

	int ret = ME_Get_RingLevel(m_medrv, NULL, &atret);;
	if (ER_OK == ret) 
	{
		if(atret.check_key("+CRSL"))
			level = atret.get_integer(0, 1);	
		else
			ret = ER_UNKNOWN;
	}
	
	return ret;
}

int Connection::SetRingLevel(int level)
{
	ATResult atret;
	return ME_Set_RingLevel(m_medrv, level, NULL, &atret);
}

int Connection::SetVibratorMode(int mode)
{
	ATResult atret;
	return ME_Set_VibratorMode(m_medrv, mode, NULL, &atret);
}

int Connection::GetSpeakerVolume(int& level)
{
	ATResult atret;
	int ret = ME_Get_SpeakerVolume(m_medrv, NULL, &atret);

	if(ER_OK == ret) 
	{
		if(atret.check_key("+CLVL"))
			level = atret.get_integer(0, 1);	
		else
			ret = ER_UNKNOWN;
	}
	
	return ret;

}

int Connection::SetSpeakerVolume(int level)
{
	ATResult atret;
	return ME_Set_SpeakerVolume(m_medrv, level, NULL, &atret);
}

// audio control
//////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////
// ussd

static bool GetCodeScheme(const CString &strCode)
{
	for (_Size_T i=0; i<strCode.GetLength(); i++)
	{
		if (strCode[i] >= 0x80)
			return true;
		if (_tcschr(_T("\\;`\""), strCode[i]) != NULL)
			return true;
	}
	
	return false;	// true for UCS2, false for GSM
}

int Connection::USSDSessionControl(int n)
{
	ATResult atret;
	return ME_Set_USSDSessionControl(m_medrv, n, NULL, -1, NULL, &atret);

}

int Connection::USSDSessionControl(int n, const CString &str)
{
	int dcs;
	string strReq;
	ATResult atret;

	if (!GetCodeScheme(str))
	{
		dcs = 15;	// gsm
		strReq = UCS2toASCII(str);
	}
	else
	{
		dcs = 72;	// ucs2
		strReq = EncodeUCS2txt(str);
	}

	if (strReq.length() > 200)	// ?????? target limit = 182
		return ER_UNKNOWN;

	return ME_Set_USSDSessionControl(m_medrv, n, (char*)strReq.c_str(), dcs, NULL, &atret);
}
// ussd
//////////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////////
// sim & password
int Connection::QuerySIMStatus(int& Isable)
{
	ATResult atret;
	
	int ret = ME_Query_SIMStatus(m_medrv, NULL, &atret);
	if(ER_OK == ret)
	{
		if(atret.check_key("+ESIMS"))
			Isable = atret.get_integer(0, 1);
		else
			ret = ER_UNKNOWN;
	}
	
	return ret;
}

int Connection::QueryPwdCount(int &count, int type)
{
	// type: 1 - pin, 2 - pin2, 3 - puk, 4 - puk2
	if (type < 1 || type > 4)
		return ER_UNKNOWN;

	ATResult atret;
	int ret = ME_Query_PINRemainNum(m_medrv, NULL, &atret);

	if (ER_OK == ret) 
	{
		if(atret.check_key("+EPINC"))
			count = atret.get_integer(0, type);
		else
			ret = ER_UNKNOWN;				
	}
	
	return ret;
}

int Connection::QueryPINCode(string& code)
{
	ATResult atret;

	int ret = ME_Query_PINState(m_medrv, NULL, &atret);

	if (ER_OK == ret)
	{
		if(atret.check_key("+CPIN"))
			atret.get_string(code, 0, 1);
		else
			ret = ER_UNKNOWN;
	}
	
	return ret;
}

int Connection::ValidationPIN(const CString& pin, const CString& new_pin)
{
	ATResult atret;
	int ret;

	if(!new_pin.IsEmpty())
		ret = ME_Set_PIN(m_medrv, (char*)UCS2toASCII(pin).c_str(), (char*)UCS2toASCII(new_pin).c_str(), NULL, &atret);
	else
		ret = ME_Set_PIN(m_medrv, (char*)UCS2toASCII(pin).c_str(), NULL, NULL, &atret);

	return ret;
}

int Connection::QueryPIN2(string& pin2code)
{
	ATResult atret;
	int ret = ME_Query_PIN2(m_medrv, NULL, &atret);
	if (ER_OK == ret)
	{
		if(atret.check_key("+EPIN2"))
			atret.get_string(pin2code, 0, 1);
		else
			ret = ER_UNKNOWN;
	}
	return ret;
}

int Connection::ValidatePIN2(const CString& puk2, const CString& new_pin2)
{
	ATResult atret;
	int ret;

	if(!new_pin2.IsEmpty())
	{
		ret = ME_Validate_PIN2(m_medrv, (char*)UCS2toASCII(puk2).c_str(), 
			(char*)UCS2toASCII(new_pin2).c_str(), NULL, &atret);
	}
	else
	{
		ret = ME_Validate_PIN2(m_medrv, (char*)UCS2toASCII(puk2).c_str(), 
			NULL, NULL, &atret);
	}
	
	return ret;
}
// sim & password
//////////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////////
// aoc
int Connection::GetCallMeterValue(int &ccm)
{
	ATResult atret;
	int ret = ME_Set_CallMeterValue(m_medrv, 0, NULL, &atret);
	if (ER_OK != ret)
		return ret;
	
	if (!atret.check_key("+CAOC"))
		return ER_UNKNOWN;
	
	sscanf(atret.resultLst[0].eleLst[1].str_value.c_str(), "%x", &ccm);
				
	return ER_OK;
}

int Connection::SetCallMeterValue(bool bEnable)
{
	ATResult atret;
	return ME_Set_CallMeterValue(m_medrv, (bEnable ? 2 : 1), NULL, &atret);
}

int Connection::SetCallMeterEvent(bool bEnable)
{
	ATResult atret;
	return ME_Set_CallMeterEvent(m_medrv, (bEnable ? 1 : 0), NULL, &atret);
}

int Connection::GetACMValue(int& acm)
{
	ATResult atret;

	int ret = ME_Get_ACMValue(m_medrv, NULL, &atret);
	if (ER_OK != ret)
		return ret;
	
	if(!atret.check_key("+CACM"))
		return ER_UNKNOWN;
	
	sscanf(atret.resultLst[0].eleLst[1].str_value.c_str(), "%x", &acm);
	
	return ER_OK;
}

int Connection::ResetACMValue(const CString& password)
{
	ATResult atret;
	return ME_Reset_ACMValue(m_medrv, (char*)UCS2toASCII(password).c_str(), NULL, &atret);
}

int Connection::GetMaxACMValue(int& maxacm)
{
	ATResult atret;
	
	int ret = ME_Get_MaxACMValue(m_medrv, NULL, &atret);
	if (ER_OK !=ret)
		return ret;
	
	if (!atret.check_key("+CAMM"))
		return ER_UNKNOWN;

	sscanf(atret.resultLst[0].eleLst[1].str_value.c_str(), "%x", &maxacm);
	
	return ER_OK;
}

int Connection::SetMaxACMValue(int maxacm, const CString &password)
{
	int ret;
	ATResult atret;

	if(password.IsEmpty())
		ret = ME_Set_MaxACMValue(m_medrv, maxacm, NULL, NULL, &atret);
	else
		ret = ME_Set_MaxACMValue(m_medrv, maxacm, (char*)UCS2toASCII(password).c_str(), NULL, &atret);

	return ret;
}

int Connection::GetPUCValue(CString &currency, CString &ppu)
{
	ATResult  atret;
	int ret = ME_Get_PUCValue(m_medrv, NULL, &atret);
	
	if (ER_OK != ret)
		return ret;

	if(!atret.check_key("+CPUC"))
		return ER_UNKNOWN;

	currency = DecodeUCS2txt(atret.resultLst[0].eleLst[1].str_value.c_str());
	atret.get_string(ppu, 0, 2);
	
	return ER_OK;
}

int Connection::SetPUCValue(const CString& currency, const CString &ppu, const CString &password)
{
	int ret;
	ATResult atret;

	if(password.IsEmpty())
	{
		ret = ME_Set_PUCValue(m_medrv, (char*)EncodeUCS2txt(currency).c_str(), 
		(char*)UCS2toASCII(ppu).c_str(), NULL, NULL, &atret);
	}
	else
	{
		ret = ME_Set_PUCValue(m_medrv, (char*)EncodeUCS2txt(currency).c_str(), 
		(char*)UCS2toASCII(ppu).c_str(), (char*)UCS2toASCII(password).c_str(), NULL, &atret);
	}

	return ret;
}
// aoc
//////////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////////
// call control
int Connection::MakeMOCall(const CString& number)
{
	ATResult atret;
	int ret = ME_MakeMO_Call(m_medrv, (char*)UCS2toASCII(number).c_str(), NULL, &atret);

	if (ER_USERABORT == ret)
	{
		TerminateCall();
	}
	
	return ret;
}

int Connection::AnswerMTCall(void)
{
	ATResult atret;
	return ME_AnswerMT_Call(m_medrv, NULL, &atret);
}

int Connection::TerminateCall(void)
{
	ATResult atret;
	return ME_Terminate_Call(m_medrv, NULL, &atret);
}

int Connection::ChangeCallLine(int code, int line)
{
	ATResult atret;
	return ME_Change_CallLine(m_medrv, code, line, NULL, &atret);
}

int Connection::DeflectCall(const CString &number, int type)
{
	ATResult atret;
	return ME_Deflect_Call(m_medrv, (char*)UCS2toASCII(number).c_str(), type, NULL, &atret);
}

int Connection::ListCurrentCall(std::vector<CALL>& vst)
{
	CALL temp;
	ATResult atret;
	
	int ret = ME_List_CurrentCall(m_medrv, NULL, &atret);
	if(ER_OK != ret)
		return ret;
	
	size_t size = atret.resultLst.size();

//	if(size <= 0)
//		return ER_UNKNOWN;
	
//	for(int i = 0; i < size -1; i++)
	for(int i = 0; i < size; i++)
	{
		if(atret.resultLst[i].eleLst.size() <= 0)
			return ER_UNKNOWN;
		
		if(atret.resultLst[i].eleLst[0].str_value == "+CLCC")
		{
			temp.idx = atret.get_integer(i, 1);
			temp.dir = atret.get_integer(i, 2);
			temp.status = atret.get_integer(i, 3);
			temp.mode = atret.get_integer(i, 4);
			temp.mpty = atret.get_integer(i, 5);
			atret.get_string(temp.number, i, 6);
			temp.type = atret.get_integer(i, 7);
			atret.get_string(temp.alpha, i, 8);
			vst.push_back(temp);
		}
		else
		{
			return ER_UNKNOWN;
		}
	}				
	
	return ret;
}

int Connection::ExtendErrReport(int& cause, CString& report)
{
	ATResult  atret;

	int ret = ME_Extend_ErrReport(m_medrv, NULL, &atret);
	if (ER_OK == ret) 
	{
		if(atret.check_key("+CEER"))
		{
			cause = atret.get_integer(0, 1);
			atret.get_string(report, 0, 2);
		}
		else
			ret = ER_UNKNOWN;
	}
	
	return ret;
}

int Connection::TransferDTMFTone(char dtmf)
{
	ATResult atret;
	return ME_Transfer_DTMFTone(m_medrv, dtmf, NULL, &atret);

}
// call control
//////////////////////////////////////////////////////////////////////////

int Connection::SetDebugPort(BOOL dport)
{
	ATResult atret;
	return ME_Set_SwitchMode(m_medrv, 1, dport ? 1 : 0, NULL, &atret);
}

int Connection::SetLoggingMode(BOOL sdlog)
{
	ATResult atret;
	return ME_Set_SwitchMode(m_medrv, 2, sdlog ? 0 : 1, NULL, &atret);
}

int Connection::SetNetworkType(int type)
{
	ATResult atret;

	return ME_Set_NetworkType(m_medrv, type, NULL, &atret);
}

//CB and EWTS
int Connection::GetLangID(string &str)
{	
	ATResult atret;
	
	int ret = ME_Get_CBSInfo(m_medrv, NULL, &atret);
	
	if(ER_OK != ret)
		return ret;
	
	if(!atret.check_key("+CSCB"))
		return ER_UNKNOWN;

	atret.get_string(str, 0, 3);

	return ER_OK;
}

int Connection::GetCBSInfo(CBSInfo &cbInfo)
{
	 ATResult atret;

	 int ret = ME_Get_CBSInfo(m_medrv, NULL, &atret);

	 if(ER_OK != ret)
		 return ret;

	 if(!atret.check_key("+CSCB"))
		 return ER_UNKNOWN;


	cbInfo.bOn = atret.get_integer(0, 1) ? FALSE : TRUE;
	CString strIds, strLangIDS;
	atret.get_string(strIds, 0, 2);
	atret.get_string(strLangIDS, 0, 3);
	 
	FillID(cbInfo.vec_channelID, strIds);
	FillID(cbInfo.vec_languages, strLangIDS);

	if(atret.resultLst[0].eleLst.size() > 3)
		cbInfo.bAllLang = atret.get_integer(0, 4) ? TRUE : FALSE;
	else
		cbInfo.bAllLang = FALSE;

	return ER_OK;
}

int Connection::SetCBMode(BOOL bOn, BOOL bAllLang, const CString &dcs, const CString &channel)
{
	ATResult atret;
	char tempbuf[1024];
	char interBuf[1024];
	int on = bOn ? 1:0;

	if(bAllLang)
	{
		if(!channel.IsEmpty())
		{
			sprintf(tempbuf, "AT+CSCB=%d,\"%s\",\"\"\r", on, UCS2toASCII(channel).c_str());
			sprintf(interBuf, "AT+CSCB=%d,\"%s\",\"\",1\r", on, UCS2toASCII(channel).c_str());
		}
		else
		{
			sprintf(tempbuf, "AT+CSCB=%d, \"\",\"\"\r", on);
			sprintf(interBuf, "AT+CSCB=%d, \"\",\"\",1\r", on);
		}

		ME_Set_CBSInfo(m_medrv, tempbuf, NULL, &atret);

		atret.clear();
		return ME_Set_CBSInfo(m_medrv, interBuf, NULL, &atret);
	}
	
	string LangID;
	
	int ret = GetLangID(LangID);

	if (ER_OK != ret)
	{
		return ret;
	}

	if(!dcs.IsEmpty())
	{
		if(!channel.IsEmpty())
		{	
			sprintf(interBuf, "AT+CSCB=1,\"%s\",\"%s\"\r", 
				UCS2toASCII(channel).c_str(), LangID.c_str());

			sprintf(tempbuf, "AT+CSCB=%d,\"%s\",\"%s\"\r", on, 
				UCS2toASCII(channel).c_str(), UCS2toASCII(dcs).c_str());
		}
		else
		{	
			sprintf(interBuf, "AT+CSCB=1, \"\", \"%s\"\r", LangID.c_str());
			sprintf(tempbuf, "AT+CSCB=%d, \"\", \"%s\"\r", on, UCS2toASCII(dcs).c_str());
		}
	}
	else
	{
		if(!channel.IsEmpty())
		{	
			sprintf(interBuf, "AT+CSCB=1,\"%s\",\"\"\r", LangID.c_str());
			sprintf(tempbuf, "AT+CSCB=%d, \"%s\",\"\"\r", on, UCS2toASCII(channel).c_str());
		}
		else
		{
			sprintf(interBuf, "AT+CSCB=1\r");
			sprintf(tempbuf, "AT+CSCB=%d\r", on);
		}
	}
	
	if(on == 0)
		ME_Set_CBSInfo(m_medrv, interBuf, NULL, &atret);

	atret.clear();
	
	return ME_Set_CBSInfo(m_medrv, tempbuf, NULL, &atret);
}

int Connection::GetETWSInfo(ETWSInfo &etwsInfo)
{
	ATResult atret;
	
	int ret = ME_Get_ETWSInfo(m_medrv, NULL, &atret);
	
	if(ER_OK != ret)
		return ret;
	
	if(!atret.check_key("+ETWS"))   // target now return ETWS, need check
		return ER_UNKNOWN;
	
	unsigned char setting = atret.get_integer(0, 1);
	
	etwsInfo.bEnableEtws = setting & 0x01 ? TRUE : FALSE;
	etwsInfo.bSecurityCheck = setting & 0x02 ? TRUE : FALSE;
	etwsInfo.bRecvTest = setting & 0x04 ? TRUE : FALSE;
	
	return ER_OK;
}

int Connection::SetETWSInfo(BOOL bEnableEtws, BOOL bSecurityCheck, BOOL bRecvTest)
{
	ATResult atret;
	int settings = 0;
	
	if(bEnableEtws)
		settings |= 0x01;
	
	if(bSecurityCheck)
		settings |= 0x02;
	
	if(bRecvTest)
		settings |= 0x04;
	
	return ME_Set_ETWSInfo(m_medrv, settings, NULL, &atret);
}

int Connection::UnblockPIN(const char *type, const TCHAR *puk, const TCHAR *pin)
{
	ATResult atret;

	return ME_UnblockPIN(
		m_medrv, UCS2toASCII(puk).c_str(), 
		UCS2toASCII(pin).c_str(), type, NULL, &atret);
}
