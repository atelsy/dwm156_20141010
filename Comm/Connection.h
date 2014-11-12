/*
 *  Connection.h
 *  Comm
 *
 *  Created by mbj on 11-11-21.
 *  Copyright 2011 MediaTek Inc. All rights reserved.
 *
 */
#ifndef MTK_CONNECTION_H
#define MTK_CONNECTION_H

#include "me_func.h"
#include "me_comm.h"
#include "ATResult.h"
#include "glbdefs.h"

#ifndef WIN32
#include "SerialPort.h"
#else
#include "..\comm\comm.h"
#include <vector>
#include <string>
#endif

#ifndef _MTK_SS_ECUSD_
#define _MTK_SS_ECUSD_
#endif

string UCS2toUTF8(const CString &src);
CString UTF8toUCS2(const unsigned char *src);
string UCS2toASCII(const CString &str);
string EncodeUCS2txt(const CString &strSrc);
CString DecodeUCS2txt(const string& ucs2txtname);

#ifdef WIN32
#define _Ssize_T	int
#define _Size_T		int
::ostream& operator<<(::ostream &os, const CString &str);
::istream& operator>>(::istream &is, CString &str);
#else
#define _Ssize_T	ssize_t
#define _Size_T		size_t
#endif

struct CFSTRUCT
{
public:
	CFSTRUCT(void);
	
	bool SetTime(int val, BYTE xclass = 0xff);
	bool SetNumber(const CString &num, BYTE xclass = 0xff);
	
	int GetTime(BYTE xclass = 0x01) const;
	CString GetNumber(BYTE xclass = 0x01) const;
	
	BYTE GetClass(void) const;
	BYTE GetType(void) const { return type; }
	
public:
	int		reason;
	BYTE	status;
	BYTE	xclass;
	
protected:
	BYTE	type;
	int		time;
	CString	number;
};

struct CBSInfo
{
public:
	BOOL bOn;
	BOOL bAllLang;
	BOOL bETWS;
	CString etws_channel;
	vector<CString> vec_channelID;
	vector<CString> vec_languages;
};

struct ETWSInfo
{
public:
	BOOL bEnableEtws;
	BOOL bSecurityCheck;
	BOOL bRecvTest;
};

// at+cops=? result
struct OPERINFOEX
{
//	short	status;
	short	AcT;
	CString operId;
	CString	long_name;
//	CString	short_name;
};


class Connection
{

public:
	Connection();
	~Connection();
	
	bool Conn_Init(char *port, ME_Callback reply);
	bool Conn_DeInit();
	bool Cancel();
	bool Exit();
	//ME_HANDLE GetHandle(){return m_medrv;}
	
	//SMS
	int GetCurrentSMSMemType(int idx, CString &mem);
	int SetCurrentSMSMemType(const CString &mem12, const CString &mem3 = _T(""));
	int GetTotalSMSNumber(const CString& memtype, int &total, int &used);
	int GetSMSMemStatus(const CString& memtype, std::vector<int> &vec);
	int GetSCAddress(CString &sca);
	int SetMessageFormat(int mode = 0);
	int SetSMSStatus(int mode = 0);
	int SetSCAddress(const CString& sca);
	int SetNewMsgInd(int mode, int mt, int bm, int ds, int bfr);
	
	bool CheckSupportSMStoInbox(void) const {return true;}
	
	//Phonebook
	int GetTotalPhonebookMemType(std::vector<CString> &vst);
	
	int SetCurrentPhonebookMemType(const CString& memstr);
	int GetTotalPhonebookNumber(const CString& memtype, int *pBuf, UINT uMask = 0xFF);

	bool CheckSupport0x81CharSet(void) const {return m_b0x81CharSet;}
	bool CheckSupportAdvPBItem(void) const {return m_b_advpbitem;}
	int InternalSupport0x81Charset(void);
	
	// Dial up
	int SetModemApn(const CString& apn);
	int GetOperator(CString &opname);
	int QueryRATnPS(int &curRat, int &gprsStat, int &ratMode, int &prefRat);
	
	// Power on / power off
	int PowerOn(int mode);
	int PowerOff(int mode);
	int SetSleepMode(bool bEnable);
	int SetPhoneFunctionality(int mode);
	
	// General
	int GetIndicationControl(int& ind);
	int SetIndicationControl(int ind);
	int SetNetworkRegInd(int gsm, int gprs);
	int GetNetworkStatus(int& status);
	int GetGPRSStatus(int& status);
	int SetSignalIndication(int flag);
	int SetPSBearer(int flag);

	int GetSelectedOperator(int format, int &mode, CString& operId);
	int GetOperatorList(std::vector<OPERINFOEX>& operlist);
	int SelectOperator(int mode, const CString &operID, int act);
	
	int SetErrorFormat(int mode = 0);
//	int SetTargetTime(const CTime &t);
	int SetDCEEcho(bool Isable = false);
	int SetCharacterSet(const char *charset);
	int Query(void);
	int QueryIMSI(CString& strIMSI);

	//sms
	int ReadSMS(int idx, ATResult &atret);
	int SendSMS(int length, char *pdu, ATResult &atret);
	int DeleteSMS(int idx, ATResult &atret);
	int WriteSMS(int length, int stat, char *pdu, ATResult &atret);

	//pbk
	int ReadPbk(int idx, ATResult &ATResult);
	int WritePbk(int idx, char *str, ATResult &atret);

	// SS&setting
	int GetVersionInfo(CString& strVer);
/************ 20131109 by Zhuwei **********************************************/
    int GetVERNO(CString& strVer);
/******************************************************************************/
    int QueryIMEI(CString& strIMEI);
  /************ 20140303 by shaohua **********************************************/
    int SetUccToUserName(int format);
/************ 20140303 by shaohua **********************************************/
	int GetSIMLock(BOOL &bLocked);
	int SetSIMLock(BOOL bLocked, const CString &pwd);
	
	int ChangePassword(const CString& facility, 
		const CString& old_pwd, const CString& new_pwd);

	int SetCLIP(int mode);
	int SetCOLP(int mode);
	int GetCLIR(int &n, int &m);
	int SetCLIR(int mode);

	int GetCallBarring(int& status, const char *facility, int xclass = 0);
	int SetCallBarring(const char *facility, BOOL bLock, 
		const CString& pwd, int xclass = 0);

	int SetCallForward(const CFSTRUCT &cfinfo);
	int GetCallForward(CFSTRUCT &cfinfo);

	int GetCallWaiting(int &nStatus, int xclass = 0);
	int SetCallWaiting(bool bEnable);
	int SetCallWaiting(int mode, int xclass = 0);
	int SetSSNotification(int n, int m);
	
	//USSD
	int USSDSessionControl(int n);	// set indicator or disconnect
	int USSDSessionControl(int n, const CString &str);	// session control
    
	//PIN Validation
	int QuerySIMStatus(int& Isable);
	int QueryPwdCount(int &count, int type);
	int QueryPINCode(std::string& code);
	int ValidationPIN(const CString& pin, const CString& new_pin);
	int QueryPIN2(std::string& pin2code);
	int ValidatePIN2(const CString& pin2, const CString& new_pin2);
	
	//Audio control
	int MuteControl(int mode);
	int GetMuteControl(int& Isable);
	int GetRingLevel(int& level);
	int SetRingLevel(int level);
	int SetVibratorMode(int mode);
	int GetVibratorMode(int& Isable);
	int GetSpeakerVolume(int& level);
	int SetSpeakerVolume(int level);
	
	//AOC
	int GetCallMeterValue(int &ccm);
	int SetCallMeterValue(bool bEnable);
	int SetCallMeterEvent(bool bEnable);
	int GetACMValue(int& acm);
	int ResetACMValue(const CString& password);
	int SetMaxACMValue(int maxacm, const CString &password);
	int GetMaxACMValue(int& maxacm);
	int GetPUCValue(CString& currency, CString &ppu);
	int SetPUCValue(const CString& currency, const CString &ppu, const CString &password);
	
	//call control
	int MakeMOCall(const CString& number);
	int AnswerMTCall(void);
	int TerminateCall(void);
	int ChangeCallLine(int code, int line = 0);
	int DeflectCall(const CString &number, int type = -1);
	int ListCurrentCall(std::vector<CALL>& vst);
	int ExtendErrReport(int& cause, CString& report);
	int TransferDTMFTone(char c);

	//CB and ETWS
	int GetLangID(string &);
	int GetCBSInfo(CBSInfo &cbInfo);
	int SetCBMode(BOOL bOn, BOOL bAllLang, const CString &dcs, const CString &channel);
	int GetETWSInfo(ETWSInfo &etwsInfo);
	int SetETWSInfo(BOOL bEnableEtws, BOOL bSecurityCheck, BOOL bRecvTest);

	int UnblockPIN(const char *type, const TCHAR *puk, const TCHAR *pin);

#ifdef _STK_SUPPORT_
	// STK
	int STKResponse(const CString &strCmd);
	int STKEnvelope(const CString &strCmd);
	int STKSetupCall(int option);
	int STKSetupSS(int option);
	int STKSetupUSSD(int option);
	int STKSendSMS(int option);
	int STKSendDTMF(int option);
	int STKRunAtCmd(int option);

	int QuerySIMFile(int fileId, int &fileLen, int &recLen);
	int ReadSIMRecord(int fileId, int recIdx, std::string &record);
	int ReadSIMFile(int fileId, std::string &content);
	int GetSIMImage(int imgId, HBITMAP &hImage);

	static int  SetPixel(BYTE *pBuf, BYTE r, BYTE g, BYTE b);
	static bool DecodeBasicImage(const char *p, BYTE *pBuf, UINT bufLen);
	static bool DecodeColorImage(const char *p, BYTE *pBuf, UINT bufLen);

#endif  //_STK_SUPPORT_

public:
	int SetLoggingMode(BOOL sdlog);
	int SetDebugPort(BOOL dport);

	int SetNetworkType(int type);    
	bool Init_Voice(char *port, DataNotify voice_cb, char *pExt = NULL);
	bool DeInit_Voice();
	void SpeechSender(const char *pData, int nLen);

private:

	//bool InternalSupportAdvPBItem(void);

	bool Internal_Conn_Init(char *port, ME_Callback reply);
	
	static bool ComposeCFCmd(char *szBuf, int mode, const CFSTRUCT &cfinfo);
	
#ifdef _MTK_SS_ECUSD_
	static bool ComposeCBCmd(char *szBuf, const char *fac, int mode, 
		int xclass, const CString &pwd);
#endif

private:

	ME_HANDLE m_medrv;

	CommModule *m_comm;  //comm module

	CommModule *m_voice;  //comm module for voice
	
	bool m_b0x81CharSet;
	bool m_b_advpbitem;
	
	//HANDLE m_hRWMutex;
	HANDLE m_hDeInitMutex;
	
	CString m_cur_memtype;	 //for sms
	CString m_cur_pbmemtype;  //for phone book
	std::string m_cur_charset;	// cscs		
};

#endif