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
 *  SMSMessage.h
 *
 * Project:
 * --------
 *   DCT
 *
 * Description:
 * ------------
 *   SMS Message Class Header
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


#ifndef MTK_SMSMESSAGE_H
#define MTK_SMSMESSAGE_H

#include "BaseItem.h"
#include "GSMMisc.h"
#include "FolderState.h"
#include "kal_non_specific_general_types.h"
#include "ems.h"
#include "ems_defs.h"
#include "customer_ps_inc.h"
#include "custom_ems_context.h"
#include "smsal_l4c_enum.h"

#define MAX_CONCATENATED_MSG 10  //max concatenated msg

class CString;

typedef enum
{
	REC_UNREAD = 1,
	REC_READ = 2,  //InBox
	STO_UNSENT = 4,  //OutBox
	STO_SENT = 8,
	ALL = 16,
	PC_SENT = 32,
	UNKNOW = 64
} eMemStatus;

typedef enum
{
	SMS_DELIVER = 0, 
	SMS_DELIVER_REPORT = 0,
	SMS_SUBMIT = 1,
	SMS_SUBMIT_REPORT = 1,
	SMS_STATUS_REPORT = 2, 
	SMS_COMMAND = 2,
	SMS_UNSPECIFIED
} eMessageType;

typedef enum
{
	SMS_SEND_SUCESSFUL	= 0,
	SMS_SEND_FAILED,
	SMS_SEND_UNKNOW
}eSendStatus;

typedef enum
{
	SMS_DELETE_FROM_HANDSET_INBOX		= 0,
	SMS_DELETE_FROM_HANDSET_OUTBOX,
	SMS_DELETE_FROM_PC_INBOX,
	SMS_DELETE_FROM_PC_OUTBOX,
	SMS_DELETE_FROM_SENTBOX,
	SMS_DELETE_FROM_PC_DRAFT,
	SMS_DELETE_FROM_PERSONALFOLDER,
	SMS_DELETE_FROM_NONE,
}eDelMsgOrigin;

class SMSMessage : public BaseItem
{
public:
	
	SMSMessage();
	~SMSMessage();
	
	SMSMessage(const SMSMessage &rhs);		
	SMSMessage & operator=(const SMSMessage &rhs);
	
	//virtual functions
	virtual bool send(Connection *pct, int &cms_error);
	virtual bool read(Connection *pct, CString memtype, int index, int &cms_error);
	virtual bool write(Connection *pct, CString memtype, int &cms_error);
	
	//method functions 
	void SetMsg(const string &str, bool needUCS2);
	void SetMsg(const wstring &wstr, bool needUCS2);
	void SetMsg(EMSData *pEmsData);
	void GetMsg(string &str);
	void GetMsg(wstring &wstr);
	void GetMsg(EMSData **pEmsData);
	
	bool EraseMsg(Connection *pct, int &cms_error);
	bool ReplaceMsg(Connection *pct, int &cms_error);
	
	eMemStatus GetStatus();
	void SetStatus(const eMemStatus es);
	unsigned char GetStandardStatusInt();
	void SetStandardStatusInt(unsigned char stat);  
	
	eSendStatus GetMsgSendStatus();
	void SetMsgSendStatus(const eSendStatus es);
	
	eDelMsgOrigin GetMsgDeleteOrigin();
	void SetMsgDeleteOrigin(const eDelMsgOrigin eo);
	
	int GetMsgIndex();
	void SetMsgIndex(int index);
	
	std::list<int> GetMsgIndexList();
	
	void GetMemType(CString &str);
	bool SetMemType(const CString str);
	
	void GetTime(tm &t);
	void SetTime(const tm t);
	
	void GetAddressNumber(CString &str);
	void SetAddressNumber(const CString &str);
	void SetUserDataHeader(unsigned char *buf, int len);
	bool isUCS2() {return m_DataCodingScheme==DCS_UCS2;}
	bool isEMS();
	
	bool GetConcatenateIDbuf(long &cs);
	bool ConcatenateMessage(SMSMessage *psm);
	
	bool UnPackEMSData();
	bool PackEMSData();	
	
	//	bool Import(CString &str);
	//	bool Export(FILE *fd);
	
	void SetTimeStamp();
	void SetSortKey(efolder_sort_key key);
	bool operator<(const SMSMessage &rhs);
	
protected:
	
	bool InternalEraseMsg(Connection *pct, CString memtype, int &cms_error);
	bool SendOneMsg(Connection *pct, int &cms_error);
	int  WriteOneMsg(Connection *pct, CString memtype, int &cms_error);
	int  GetSCABufLen();
	bool Encode_SMS_SUBMIT(string &hexstr);
	bool Encode_SMS_DELIVER(string &hexstr);
	bool Decode_SMS(string pdustr);
	bool Decode_SMS_SUBMIT(const char *tbuf, int buflen);
	bool Decode_SMS_DELIVER(const char *tbuf, int buflen);   
	bool sendEMSData(Connection *pct, int &cms_error);
	bool writeEMSData(Connection *pct, CString memtype, int &cms_error);
	
	void SetMsgSubmit();
	void SetMsgDelivery();
	
	static efolder_sort_key sort_key;
	static unsigned char ConcatMsgID;
	
	std::list<int> m_index_list;   //temp message index list of the sms in mobile
	
	string  message_text;   //message content, char  
	wstring message_wtext;  //message content, wchar
	string  userdataheader; //message header,  char
	
	CString memtype_str; //memory type in the mobile
	eMemStatus msgstatus;   //message status in the mobile
	
	GSMAddress m_SCAddress; //SCAddress for the message, default none
	GSMAddress m_DestAddr;  //Dest addr
	
	GSMTimeStamp m_SMSTimeStamp; //The time stamp of the this message
	
	GSMTimeStamp m_ValidOrScaTimeStamp;  //SCA TimeStamp for delivery, valid for submit
	
	//TS23.040 Submit, Delivery pdu item
	eMessageType m_Msg_Type_Indicator;
	bool m_RejectDuplicates;
	bool m_ReplyPath;
	bool m_StatusReportReqInd;
	unsigned char m_MessageReference;
	unsigned char m_ProtocolID;
	eTimePeriodFormat m_ValidPeriodFormat;
	eDataCodingScheme m_DataCodingScheme;
	unsigned short m_ValidRelativePeriod;
	
	bool m_MoreMessagesToSend;
	
	bool m_UserDataHeaderIndicator;
	
	// add for sms send and recovery
	eSendStatus		m_MsgSendStatus;
	eDelMsgOrigin	m_MsgDeleteOrigin;
	int				m_index;
	
	std::list<string> m_TPUD_list;
	EMSData *m_pEmsData;
	
private:
	
	friend ::ostream& operator<<(::ostream &os, SMSMessage &sms);
	friend ::istream& operator>>(::istream &is, SMSMessage &sms);
};

::ostream& operator<<(::ostream &os, SMSMessage &sms);
::istream& operator>>(::istream &is, SMSMessage &sms);

::ostream& operator<<(::ostream &os, const folder_state_struct &fs);
::istream& operator>>(::istream &is, folder_state_struct &fs);


::ostream& operator<<(::ostream &os, const CString &str);
::istream& operator>>(::istream &is, CString &str);

//::ostream& operator<<(::ostream &os, SMSStatusIndexMap &status);
//::istream& operator>>(::istream &is, SMSStatusIndexMap &status);

#endif
