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
 *	SMSMessage.cpp
 *
 * Project:
 * --------
 *	DCT
 *
 * Description:
 * ------------
 *	SMSMessage.cpp
 *
 * Author:
 * -------
 *	Fengping Yu
 *
 *==============================================================================
 *			HISTORY
 * Below this line, this part is controlled by PVCS VM. DO NOT MODIFY!!
 *------------------------------------------------------------------------------
 * $Revision:$
 * $Modtime:$
 * $Log:$
 * 
 * 09 24 2012 fengping.yu
 * [STP100004315]  check in code
 * .
 * 
 * 08 29 2012 fengping.yu
 * [STP100004315]  check in code
 * .
 *
 * 01 21 2011 gang.wei
 * [STP100004317]  Data Card Tool source code check in
 * .
 *------------------------------------------------------------------------------
 * Upper this line, this part is controlled by PVCS VM. DO NOT MODIFY!!
 *==============================================================================
 *******************************************************************************/
#include <time.h>
#include "Win2Mac.h"
#include "SMSMessage.h"
#include "SMSBuffer.h"


efolder_sort_key SMSMessage::sort_key;
unsigned char SMSMessage::ConcatMsgID;

SMSMessage::SMSMessage()
{
	msgstatus = UNKNOW;
	
	m_Msg_Type_Indicator = SMS_SUBMIT;
	m_RejectDuplicates = true;
	m_ValidRelativePeriod = 167; // 1 Day
	//m_ValidRelativePeriod = 168;	 // 2 Day
	m_ValidPeriodFormat = PRESENT_RELATIVE;
	m_ReplyPath = false;
	m_StatusReportReqInd = false;
	m_MessageReference = 0;
	m_ProtocolID = 0;
	m_DataCodingScheme = DCS_DEFAULT;
	m_MoreMessagesToSend = false;
	m_UserDataHeaderIndicator = false;
	
	m_SMSTimeStamp.SetCurrentTime();
	m_ValidOrScaTimeStamp.SetCurrentTime();
	
	m_MsgSendStatus = SMS_SEND_UNKNOW;
	m_MsgDeleteOrigin = SMS_DELETE_FROM_NONE;
	m_index = -1;
	
	m_pEmsData = NULL;	   
}

SMSMessage::~SMSMessage()
{
	if(m_pEmsData)
	{
		ReleaseEMSData(m_pEmsData);
		delete m_pEmsData;
		m_pEmsData = NULL;
	}
}

SMSMessage &SMSMessage::operator =(const SMSMessage &rhs)
{
	m_index_list = rhs.m_index_list;
	message_text = rhs.message_text;
	message_wtext = rhs.message_wtext;
	userdataheader = rhs.userdataheader;	
	memtype_str = rhs.memtype_str;
	msgstatus = rhs.msgstatus;
	m_SCAddress = rhs.m_SCAddress; 
	m_DestAddr = rhs.m_DestAddr;
	m_SMSTimeStamp = rhs.m_SMSTimeStamp;
	m_ValidOrScaTimeStamp = rhs.m_ValidOrScaTimeStamp;	  
	m_Msg_Type_Indicator = rhs.m_Msg_Type_Indicator;
	m_RejectDuplicates = rhs.m_RejectDuplicates;
	m_ReplyPath = rhs.m_ReplyPath;
	m_StatusReportReqInd = rhs.m_StatusReportReqInd;
	m_MessageReference = rhs.m_MessageReference;
	m_ProtocolID = rhs.m_ProtocolID;
	m_ValidPeriodFormat = rhs.m_ValidPeriodFormat;
	m_DataCodingScheme = rhs.m_DataCodingScheme;
	m_ValidRelativePeriod = rhs.m_ValidRelativePeriod;	  
	m_MoreMessagesToSend = rhs.m_MoreMessagesToSend;
	m_UserDataHeaderIndicator = rhs.m_UserDataHeaderIndicator;
	m_TPUD_list = rhs.m_TPUD_list;
	m_MsgSendStatus = rhs.m_MsgSendStatus;
	m_MsgDeleteOrigin = rhs.m_MsgDeleteOrigin;
	m_index = rhs.m_index;
	
	if(m_pEmsData)	  
		ReleaseEMSData(m_pEmsData);
	else
		m_pEmsData = new EMSData;
	
	//	  InitializeEMSData(m_pEmsData, m_DataCodingScheme);	
	//	  CopyEMSData(m_pEmsData, rhs.m_pEmsData);
	if(rhs.m_pEmsData){
		InitializeEMSData(m_pEmsData, m_DataCodingScheme);	  
		CopyEMSData(m_pEmsData, rhs.m_pEmsData);
	}else{
		delete m_pEmsData;
		m_pEmsData = NULL;
	}
	return *this;
}

SMSMessage::SMSMessage(const SMSMessage &rhs)
{
	m_index_list = rhs.m_index_list;
	message_text = rhs.message_text;
	message_wtext = rhs.message_wtext;
	userdataheader = rhs.userdataheader;	
	memtype_str = rhs.memtype_str;
	msgstatus = rhs.msgstatus;
	m_SCAddress = rhs.m_SCAddress; 
	m_DestAddr = rhs.m_DestAddr;
	m_SMSTimeStamp = rhs.m_SMSTimeStamp;
	m_ValidOrScaTimeStamp = rhs.m_ValidOrScaTimeStamp;	  
	m_Msg_Type_Indicator = rhs.m_Msg_Type_Indicator;
	m_RejectDuplicates = rhs.m_RejectDuplicates;
	m_ReplyPath = rhs.m_ReplyPath;
	m_StatusReportReqInd = rhs.m_StatusReportReqInd;
	m_MessageReference = rhs.m_MessageReference;
	m_ProtocolID = rhs.m_ProtocolID;
	m_ValidPeriodFormat = rhs.m_ValidPeriodFormat;
	m_DataCodingScheme = rhs.m_DataCodingScheme;
	m_ValidRelativePeriod = rhs.m_ValidRelativePeriod;	  
	m_MoreMessagesToSend = rhs.m_MoreMessagesToSend;
	m_UserDataHeaderIndicator = rhs.m_UserDataHeaderIndicator;
	m_TPUD_list = rhs.m_TPUD_list;	
	m_MsgSendStatus = rhs.m_MsgSendStatus;
	m_MsgDeleteOrigin = rhs.m_MsgDeleteOrigin;
	m_index = rhs.m_index;
	
	m_pEmsData = new EMSData;
	InitializeEMSData(m_pEmsData, m_DataCodingScheme);	  
	CopyEMSData(m_pEmsData, rhs.m_pEmsData);	
}

bool SMSMessage::UnPackEMSData()
{	 
	if(m_pEmsData)
	{
		ReleaseEMSData(m_pEmsData);
		delete m_pEmsData;
	}
	m_pEmsData = new EMSData;
	InitializeEMSData(m_pEmsData, m_DataCodingScheme);
	
	kal_uint8 *tpud[255];
	kal_uint8 tpudLen[255];
	kal_uint8 buf[255][160];
	
	int i=0;	
	unsigned char num = m_TPUD_list.size();
	std::list<string>::iterator iter = m_TPUD_list.begin();
	while(iter!=m_TPUD_list.end())
	{			
		memcpy(&(buf[i][0]), iter->data(), iter->size());
		tpud[i]=&(buf[i][0]);
		tpudLen[i]=iter->size();
		iter++;
		i++;
	}
	m_TPUD_list.clear();
	if(m_UserDataHeaderIndicator)
		m_pEmsData->udhi = KAL_TRUE;
	EMSTATUS status = EMSUnPack(m_pEmsData, m_pEmsData->udhi, num, tpud, tpudLen);
	if(status==EMS_OK)
	{
		/*
		 if(m_pEmsData->dcs==SMSAL_UCS2_DCS)  //change the endian!
		 {																		 
		 for(int i=0;i<m_pEmsData->textLength/2;i++)
		 ((wchar_t*) m_pEmsData->textBuffer)[i] = ntohs(((wchar_t*) m_pEmsData->textBuffer)[i]);
		 }
		 */
		return true;
	}
	else
		return false;
}

bool SMSMessage::PackEMSData()
{	 
	unsigned char num;
	kal_uint8 *tpud[MAX_CONCATENATED_MSG];
	kal_uint8 tpudLen[MAX_CONCATENATED_MSG];
	kal_uint8 buf[MAX_CONCATENATED_MSG][160];
	
	if(m_pEmsData->Reminder.requiredSegment >  MAX_CONCATENATED_MSG)
		return false;
	
	for(int i=0;i<MAX_CONCATENATED_MSG;i++)
	{
		tpud[i]=&(buf[i][0]);
		tpudLen[i]=160;
	}		 
	
	m_TPUD_list.clear();
	
	/*
	 if(m_pEmsData->dcs==SMSAL_UCS2_DCS)   //change the endian!
	 {																		 
	 for(int i=0;i<m_pEmsData->textLength/2;i++) 
	 ((wchar_t*) m_pEmsData->textBuffer)[i] = htons(((wchar_t*) m_pEmsData->textBuffer)[i]);
	 }
	 */
	
	if(EMSPack(m_pEmsData, 1, &num, tpud, tpudLen)==EMS_OK)
	{
		for(int i=0;i<num;i++)	
		{
			m_TPUD_list.push_back(string((char*)(&tpud[i][0]), tpudLen[i]));
		}
		
		return true;
	}
	return false;
}

bool SMSMessage::sendEMSData(Connection *pct, int &cms_error)
{	 
	if(!PackEMSData())
		return false;
	
	bool ret=true;
	std::list<string> temp_TPUD_list = m_TPUD_list;
	m_TPUD_list.clear();
	std::list<string>::iterator iter = temp_TPUD_list.begin();
	while(iter!=temp_TPUD_list.end())
	{			 
		m_TPUD_list.push_back(*iter);
		if(!SendOneMsg(pct, cms_error))
		{
			ret=false;
			break;
		}
		m_TPUD_list.clear();
		iter++; 	   
	}
	m_TPUD_list.clear();		
	return ret;
}


bool SMSMessage::writeEMSData(Connection *pct, CString memtype, int &cms_error)
{
	if(!PackEMSData())
	{
		cms_error = ER_UNKNOWN;
		return false;
	}
	
	int total_sm = -1, used_sm = 0;
	int total_me = -1, used_me = 0;
	
	cms_error = pct->GetTotalSMSNumber(_T("SM"), total_sm, used_sm);
	if(ER_OK != cms_error)
		return false;
	
	
	cms_error = pct->GetTotalSMSNumber(_T("ME"), total_me, used_me);
	if(ER_OK != cms_error)
		return false;
	
	if(memtype == _T("SM"))
	{
		if(used_sm+m_TPUD_list.size() > total_sm)  //full
			return false;	//memtype = _T("MT"); 	   
	}
	
	
 	if(memtype == _T("ME"))
	{
		if(used_me+m_TPUD_list.size() > total_me)  //full
			memtype = _T("MT"); 	   
	}
	
	cms_error = pct->SetCurrentSMSMemType(memtype);
	if(ER_OK != cms_error)
		return false;
	
	bool ret=true;
	int ret_index=-1;
	std::list<string> temp_TPUD_list = m_TPUD_list;
	
	m_index_list.clear();
	m_TPUD_list.clear();
	std::list<string>::iterator iter = temp_TPUD_list.begin();
	while(iter!=temp_TPUD_list.end())
	{
		m_TPUD_list.push_back(*iter);
		ret_index = WriteOneMsg(pct, memtype, cms_error);
		if(ret_index!=-1)
			m_index_list.push_back(ret_index);
		else
		{
			InternalEraseMsg(pct, memtype, cms_error);
			ret=false;
			break;
		}
		m_TPUD_list.clear();
		iter++;
	}
	m_TPUD_list.clear();
	if(ret)
	{
		if (_T("MT") == memtype)
		{
			if(ret_index > total_sm)
				memtype = _T("ME");
			else
				memtype = _T("SM");
		}
		memtype_str = memtype;
	}
	return ret;
}


bool SMSMessage::send(Connection *pct, int &cms_error)
{
	cms_error = ER_UNKNOWN;
	if(pct == NULL)
		return false;
	
	if(m_pEmsData)
		return sendEMSData(pct, cms_error);
	
	//calculate the msg number
	//	int maxconctextlen = 0;
	int msgnum = 1;
	string text;
	wstring wtext;
	SMSMessage msg = *this;  //assign	 
	
	if(m_DataCodingScheme == DCS_DEFAULT)
	{
		GetMsg(text);
		if(text.length() > 160)
			msgnum = (int)(text.length() + 153-1)/153;
		if(msgnum > MAX_CONCATENATED_MSG)
			return false;
	}
	else if(m_DataCodingScheme == DCS_UCS2)
	{
		GetMsg(wtext);
		if(wtext.length() > 70)
			msgnum = (int)(wtext.length() + 67-1)/67;
		if(msgnum > MAX_CONCATENATED_MSG)
			return false;
	}
	else 
		return false;
	
	m_SMSTimeStamp.SetCurrentTime();	
	if(msgnum==1)
	{
		if(!SendOneMsg(pct, cms_error))
			return false;
	}
	else
	{
		//static variable;	  
		ConcatMsgID += (rand()+1)%256;
		SMSMessage *pmsg = &msg;  //working msg pointer.
		for(int i=0;i<msgnum;i++)
		{
			if(m_DataCodingScheme == DCS_UCS2)
			{
				pmsg->SetMsg(wtext.substr(0, 67), true);
				wtext.erase(0, 67);
			}
			else
			{
				pmsg->SetMsg(text.substr(0, 153), false);
				text.erase(0, 153);
			}
			unsigned char udh[] = {0, 3, ConcatMsgID, msgnum, i+1};
			pmsg->SetUserDataHeader(udh , sizeof(udh)); //set UDH Information Element
			if(!pmsg->SendOneMsg(pct, cms_error))
				return false;
		}
	}
	//modify the state
	msgstatus = PC_SENT;
	return true;
}

bool SMSMessage::SendOneMsg(Connection *pct, int &cms_error)
{			 
	string pdu;
	//encode the pdu
	Encode_SMS_SUBMIT(pdu);
	
	cms_error = ER_UNKNOWN;
	
	ATResult atret; 	   
	if(ER_OK == pct->SendSMS((int)pdu.length()/2 - GetSCABufLen(), (char*)pdu.c_str(), atret))
	{
		if(atret.resultLst.size() > 0)	//+CMGS: <mr>, [ackpdu]
		{
			if(atret.resultLst[0].eleLst[0].str_value == "ERROR")
			{
				cms_error = ER_UNKNOWN;
			}
			else if(atret.resultLst[0].eleLst[0].str_value == "+CMS ERROR")
				cms_error = atret.resultLst[0].eleLst[1].int_value;
			else
			{
				msgstatus = PC_SENT; //modify the state
				cms_error = ER_OK;
				return true;
			}			 
		}
	}
	
	if (AT_TIMEOUT == atret.retType)
		cms_error = ER_TIMEOUT;
	
	return false;
}

bool SMSMessage::read(Connection *pct, CString memtype, int index, int &cms_error)
{
	cms_error = ER_UNKNOWN;
	if(pct == NULL || index < 0 || memtype.GetLength()==0)
		return false;	 
	
	cms_error = pct->SetCurrentSMSMemType(memtype);
	if(ER_OK != cms_error)
		return false;
	
	ATResult atret; 	   
	if(	ER_OK == pct->ReadSMS(index, atret))
	{
		if(atret.resultLst.size() > 0 && atret.resultLst[0].eleLst.size()>=4)   //+CMGR: <stat>, [alpha], <length>, <pdu>
		{			 
			if(atret.resultLst[0].eleLst[0].str_value == "+CMGR")
			{
				int pdu_count= (int)atret.resultLst[0].eleLst.size()-1;				  
				Decode_SMS(atret.resultLst[0].eleLst[pdu_count].str_value);
				//save the state and index
				SetStandardStatusInt(atret.resultLst[0].eleLst[1].int_value);
				m_index_list.clear();
				m_index_list.push_back(index);
				memtype_str=memtype;
				cms_error = ER_OK;
				return true;
			}
		}
		else if(atret.resultLst.size()>0)
		{
			if(atret.resultLst[0].eleLst[0].str_value == "+CMS ERROR")
				cms_error = atret.resultLst[0].eleLst[1].int_value;
		}
	}
	
	if (AT_TIMEOUT == atret.retType)
		cms_error = ER_TIMEOUT;
	
	return false;
}

bool SMSMessage::write(Connection *pct, CString memtype, int &cms_error)
{
	cms_error = ER_UNKNOWN;
	if(pct == NULL)
		return false;  
	
	if(pct->CheckSupportSMStoInbox())		
	{
		if(msgstatus==REC_UNREAD || msgstatus==REC_READ)
			SetMsgDelivery();
		else
			SetMsgSubmit();
	}
	else
		SetMsgSubmit();  //set all to outbox!
	
	if(m_pEmsData)	//use emsdata
		return writeEMSData(pct, memtype, cms_error);
	
	cms_error = pct->SetCurrentSMSMemType(_T("MT"));
	if(ER_OK != cms_error)
		return false;
	
	//calculate the msg number
	//	int maxconctextlen = 0;
	unsigned char msgnum = 1;
	string text;
	wstring wtext;
	SMSMessage msg = *this;   //assign	  
	
	if(m_DataCodingScheme == DCS_DEFAULT)
	{
		GetMsg(text);
		if(text.length() > 160)
			msgnum = (text.length() + 153-1)/153;		 
	}
	else if(m_DataCodingScheme == DCS_UCS2)
	{
		GetMsg(wtext);
		if(wtext.length() > 70)
			msgnum = (wtext.length() + 67-1)/67;		
	}
	else 
		return false;
	
	if(msgnum > MAX_CONCATENATED_MSG)
		return false;
	
	//reset the index
	m_index_list.clear();
	int ret_index=-1;	 
	if(msgnum==1)
	{
		ret_index = WriteOneMsg(pct, memtype,cms_error);
		if(ret_index!=-1)		 
			m_index_list.push_back(ret_index);
		else 
			return false;
	}
	else
	{
		//static variable;	  
		ConcatMsgID += (rand()+1)%256;	  
		SMSMessage *pmsg = &msg;  //working msg pointer.		
		for(int i=1;i<=msgnum;i++)
		{
			if(m_DataCodingScheme == DCS_UCS2)
			{
				pmsg->SetMsg(wtext.substr(0, 67), true);
				wtext.erase(0, 67);
			}
			else
			{
				pmsg->SetMsg(text.substr(0, 153), false);
				text.erase(0, 153);
			}			 
			unsigned char udh[] = {0, 3, ConcatMsgID, msgnum, i};
			pmsg->SetUserDataHeader(udh, sizeof(udh)); //set UDH Information Element
			ret_index = pmsg->WriteOneMsg(pct, memtype, cms_error);
			if(ret_index != -1)
				m_index_list.push_back(ret_index);
			else
			{
				InternalEraseMsg(pct, memtype, cms_error);
				return false;
			}
		}
	}	 
	
	int total_sm, used_sm;
	if(ER_OK == pct->GetTotalSMSNumber(_T("SM"), total_sm, used_sm))
	{
		if(ret_index > total_sm)
			memtype = _T("ME");
		else
			memtype = _T("SM");
	}
	
	memtype_str = memtype;
	
	/////check here!!!!!!
	//msgstatus = STO_UNSENT;  //set to sto_unsent
	
	return true;
}

int SMSMessage::WriteOneMsg(Connection *pct, CString memtype, int &cms_error)
{
	string pdu;  
	int stat;
	//encode the pdu	
	if(m_Msg_Type_Indicator == SMS_DELIVER)
		Encode_SMS_DELIVER(pdu);
	else if(m_Msg_Type_Indicator == SMS_SUBMIT)
		Encode_SMS_SUBMIT(pdu);
	else 
		return -1;
	
	if(pct->CheckSupportSMStoInbox() && (msgstatus==REC_UNREAD||msgstatus==REC_READ))			
		stat = 1;
	else
		stat = 2;
	
	ATResult atret; 	   
	if(ER_OK == pct->WriteSMS((int)pdu.length()/2 - GetSCABufLen(), stat, (char*)pdu.c_str(), atret))
	{				 
		string str;
		atret.get_string(str, 0, 0);
		if (str == "+CMGW")
		{
			cms_error = ER_OK;
			return atret.get_integer(0, 1);
		}
		else if(str == "+CMS ERROR")
			cms_error = atret.get_integer(0, 1);
		else
			cms_error = ER_UNKNOWN;
	}
	
	if (AT_TIMEOUT == atret.retType)
		cms_error = ER_TIMEOUT;
	
	return -1;
}

void SMSMessage::SetMsg(const string &str, bool needUCS2)
{
	message_text = str;
	message_wtext = L""; 
	if(needUCS2 == true)
		m_DataCodingScheme = DCS_UCS2;
}

void SMSMessage::SetMsg(const wstring &wstr, bool needUCS2)
{
	message_wtext = wstr;
	message_text = "";
	if(needUCS2 == true)	
		m_DataCodingScheme = DCS_UCS2;
	else
	{
		//we force to save the non-UCS2 in single byte format!
		char tmpbuf[160*MAX_CONCATENATED_MSG];	//max of available GSM7 chars		
		setlocale(LC_ALL, ".ACP");
		int n = (int)wcstombs(tmpbuf, wstr.c_str(), sizeof(tmpbuf));
		tmpbuf[n] = 0;
		//	WideCharToMultiByte(CP_ACP, 0, wstr.c_str(), -1, tmpbuf,  sizeof(tmpbuf), NULL, NULL);
		message_text = tmpbuf;
		message_wtext = L"";
	}
}

void SMSMessage::SetMsg(EMSData *pEmsData)
{
	m_pEmsData = pEmsData;
	m_DataCodingScheme = (eDataCodingScheme) m_pEmsData->dcs;
}

void SMSMessage::GetMsg(EMSData **pEmsData)
{
	*pEmsData = m_pEmsData;
}

void SMSMessage::GetMsg(string &str)
{
	str ="";
	if(message_text.length() == 0)
	{
		if(message_wtext.length()!=0)
		{
			char tmpbuf[160*MAX_CONCATENATED_MSG];	//max of available GSM7 chars						 
			WideCharToMultiByte(CP_ACP, 0, message_wtext.c_str(), -1, tmpbuf,  sizeof(tmpbuf), NULL, NULL);
			str = tmpbuf;
		}
	}
	else
		str = message_text;
}

void SMSMessage::GetMsg(wstring &wstr)
{
	wstr = L"";
	if(message_wtext.length() == 0)
	{
		if(message_text.length()!=0)
		{
			wchar_t tmpwbuf[17085];  //max of available UCS2 wchars 		   
			MultiByteToWideChar(CP_ACP, MB_PRECOMPOSED, message_text.c_str(), -1, tmpwbuf,	sizeof(tmpwbuf)/2);
			wstr = tmpwbuf;
		}
	}
	else
		wstr = message_wtext;
}

bool SMSMessage::isEMS()
{
	if(m_pEmsData==NULL)
		return false;
	if(m_pEmsData->listHead == NULL)
		return false;
	return true;
}

bool SMSMessage::InternalEraseMsg(Connection *pct, CString memtype, int &cms_error)
{
	cms_error = ER_UNKNOWN;
	ATResult atret;
	
	if(m_index_list.size()==0)
		return true;
	
	//if(ER_OK != pct->SetCurrentSMSMemType(memtype))
	cms_error = pct->SetCurrentSMSMemType(_T("MT"));
	if(ER_OK != cms_error)
		return false;
	
	//bool ret=false;
	std::list<int>::const_iterator iter=m_index_list.begin();
	while(iter != m_index_list.end())
	{
		atret.clear();
		cms_error = pct->DeleteSMS(*iter, atret);;
		if (ER_OK != cms_error)
			break;
		
		iter++;
	}
	
	if (ER_OK == cms_error)
		m_index_list.clear();
	return (ER_OK == cms_error);
}

bool SMSMessage::EraseMsg(Connection *pct, int &cms_error)
{	 
	return InternalEraseMsg(pct, memtype_str, cms_error);
}

bool SMSMessage::ReplaceMsg(Connection *pct, int &cms_error)
{
	if(!EraseMsg(pct, cms_error))
		return false;	 
	return write(pct, memtype_str, cms_error);		  
}

eMemStatus SMSMessage::GetStatus()
{
	return msgstatus;
}

unsigned char SMSMessage::GetStandardStatusInt()
{	 
	switch (msgstatus)
	{
		case REC_UNREAD:
			return 0;
		case REC_READ:
			return 1;
		case STO_UNSENT:
			return 2;
		case STO_SENT:
			return 3;
		case ALL:
			return 4;
		default:
			return 2;	 
	}
}

void SMSMessage::SetStandardStatusInt(unsigned char stat)
{
	switch (stat)
	{
		case 0:
			msgstatus = REC_UNREAD;
			break;
		case 1:
			msgstatus = REC_READ;
			break;
		case 2:
			msgstatus = STO_UNSENT;
			break;
		case 3:
			msgstatus = STO_SENT;
			break;
		case 4:
			msgstatus = ALL;
			break;
		default:
			msgstatus = REC_UNREAD;
	}
}

void SMSMessage::SetStatus(const eMemStatus es)
{
	msgstatus = es;
}

eSendStatus SMSMessage::GetMsgSendStatus()
{
	return m_MsgSendStatus;
}

void SMSMessage::SetMsgSendStatus(const eSendStatus es)
{
	m_MsgSendStatus = es;
}

eDelMsgOrigin SMSMessage::GetMsgDeleteOrigin()
{
	return m_MsgDeleteOrigin;
}

void SMSMessage::SetMsgDeleteOrigin(const eDelMsgOrigin eo)
{
	m_MsgDeleteOrigin = eo;
}

int SMSMessage::GetMsgIndex()
{
	return m_index;
}

void SMSMessage::SetMsgIndex(int index)
{
	m_index = index;
}

std::list<int> SMSMessage::GetMsgIndexList()
{
	return m_index_list;
}

void SMSMessage::GetMemType(CString &str)
{
	str = memtype_str;	  
}

bool SMSMessage::SetMemType(const CString str)
{	 
	memtype_str = str;
	return true;
}

void SMSMessage::GetTime(tm &t)
{		 
	m_SMSTimeStamp.GettmStruct(t);	 
}

void SMSMessage::SetTime(const tm t)
{
	m_SMSTimeStamp.SettmStruct(t);	  
}

void SMSMessage::GetAddressNumber(CString &str)
{
	str = m_DestAddr.GetNumStr().c_str();
}

void SMSMessage::SetAddressNumber(const CString &str)
{
	m_DestAddr.SetGSMAddress(UCS2toASCII(str));
}

void SMSMessage::SetMsgSubmit()
{
	GSMAddress EmptySCAddress;
	m_SCAddress = EmptySCAddress;
	
	m_Msg_Type_Indicator = SMS_SUBMIT;
}

void SMSMessage::SetMsgDelivery()
{
	GSMAddress EmptySCAddress;
	m_SCAddress = EmptySCAddress;
	
	m_Msg_Type_Indicator = SMS_DELIVER;
}

int SMSMessage::GetSCABufLen()
{	 
	SMSBuffer sbuf;
	
	sbuf.SetSCAddr(m_SCAddress);	
	return sbuf.CurrentBufLen();	
}

void SMSMessage::SetUserDataHeader(unsigned char *buf, int len)
{		
	//remove the Concatenate Information-Elemenat-Data in the userdata header if exist
	if(userdataheader.length() > 0)
	{		 
		int step=0;
		for(int i=0;i<userdataheader.length();i=i+step)
		{
			step = userdataheader[i+1] + 2;  //2 = id+data_length
			if(userdataheader[i]==0 && userdataheader[i+1]==3 && userdataheader.length()>=i+5)
				userdataheader=userdataheader.substr(i+5, userdataheader.length());
			
			//enhance msg reference id
			if(userdataheader[i]==8 && userdataheader[i+1]==4 && userdataheader.length()>=i+6)
				userdataheader=userdataheader.substr(i+6, userdataheader.length());
		}
	}
	
	if(userdataheader.size()<len)
		userdataheader.resize(len); 	   
	
	for(int i=0; i<len; i++)
		userdataheader[i]=buf[i];
}

bool SMSMessage::GetConcatenateIDbuf(long &cs)
{
	if(userdataheader.length() < 5)
		return false;
	
	cs=0;
	int step=0;
	for(int i=0;i<userdataheader.length();i=i+step)
	{
		step = userdataheader[i+1] + 2;  //2 = id+data_length
		if(userdataheader[i]==0x00 && userdataheader[i+1]==0x03)
		{
			//long 4 byte: ID0-IDS-Total-Count
			char *pcs= (char *) &cs;
			pcs[0] = userdataheader[i+2];			 
			pcs[2] = userdataheader[i+3];
			pcs[3] = userdataheader[i+4];
			
			//use more data to differ the ID						
			if(GetStandardStatusInt()==1)
				pcs[1] = 1;
			else
				pcs[1] = 2;
			
			if(pcs[2]==0 || pcs[3]==0 || pcs[3]>pcs[2]) //count must not be zero
				continue;
			
			return true;
		}	 
		
		//enhanced msg reference id
		if(userdataheader[i]==0x08 && userdataheader[i+1]==0x04 && userdataheader.size()>=6)		
		{
			//long 4 byte: ID0-IDS-Total-Count
			char *pcs= (char *) &cs;
			pcs[0] = userdataheader[i+2];
			pcs[1] = userdataheader[i+3];
			pcs[2] = userdataheader[i+4];
			pcs[3] = userdataheader[i+5];
			
			if(pcs[2]==0 || pcs[3]==0 || pcs[3]>pcs[2]) //count must not be zero
				continue;
			
			return true;
		}	 
	}
	return false;
}

bool SMSMessage::ConcatenateMessage(SMSMessage *psm)
{
	//add index
	if(psm->m_index_list.size()==0)
		return false;
	m_index_list.push_back(*(psm->m_index_list.begin()));
	
	if(psm->m_TPUD_list.size() > 0)
		m_TPUD_list.push_back(*(psm->m_TPUD_list.begin()));
	
	//concatenate the text
	if(message_text.length()!=0)
	{
		string str;
		psm->GetMsg(str);
		message_text+=str;
	}
	else
	{
		wstring wstr;
		psm->GetMsg(wstr);
		message_wtext+=wstr;
	}
	
	return true;
}

bool SMSMessage::Decode_SMS(string pdustr)
{
	char tbuf[512];
	int buflen, temp, i;
	
	memset(tbuf, 0, sizeof(tbuf));
	
	for(i=0; i<pdustr.length(); i++)	//transfer the chars to standard unsigned char format
	{  
		if (pdustr[i] >= '0' && pdustr[i] <= '9')
			temp = pdustr[i] - '0';
		else if (pdustr[i] >= 'A' && pdustr[i] <= 'F')
			temp = pdustr[i] - 'A' + 10;
		else					
			return false;
		
		if((i%2)==0)   //4-7 bit
			tbuf[i/2]=temp<<4; 
		else		   //0-3 bit
			tbuf[i/2]=tbuf[i/2]+temp;
	}
	buflen = (i-1)/2+1;
	
	SMSBuffer sbuf(tbuf, buflen);
	sbuf.GetSCAddr();
	
	//decode the message type 
	m_Msg_Type_Indicator = (eMessageType)sbuf.GetMultiBits(2);
	
	if(m_Msg_Type_Indicator == SMS_DELIVER)
		return Decode_SMS_DELIVER(tbuf, buflen);
	else if(m_Msg_Type_Indicator == SMS_SUBMIT)
		return Decode_SMS_SUBMIT(tbuf, buflen);
	else
		return false;  //only support SMS_DELIVER SMS_SUBMIT
}

bool SMSMessage::Encode_SMS_SUBMIT(string &hexstr)
{
	SMSBuffer sbuf;
	
	static unsigned char messageRefrenceCount=0;
	m_MessageReference = messageRefrenceCount++;
	
	//set value
	m_Msg_Type_Indicator = SMS_SUBMIT;	  
	
	//process user data header and content 
	unsigned char udhlen=0;
	string udc;
	
	if(m_TPUD_list.size()==1)  //Data is pack from EMSData
	{				 
		string str = *(m_TPUD_list.begin());
		if(m_pEmsData->udhi)
		{
			udhlen = str[0]; //udh len
			userdataheader = string(str.data()+1, udhlen); //udh 
			udc = string(str.data()+1+udhlen, str.size()-udhlen-1); //data, already in big-endian
		}
		else
			udc = str;
	}		 
	else  //process the text
	{
		udhlen=userdataheader.length();
		if(m_DataCodingScheme == DCS_UCS2)	//convert single-byte buffer to unicode
		{
			wchar_t tmpwbuf[256];
			memset(tmpwbuf, 0, sizeof(tmpwbuf));
			if(message_wtext.length()==0)		 
				MultiByteToWideChar(CP_ACP, MB_PRECOMPOSED, message_text.c_str(),(int)message_text.length(), tmpwbuf,  sizeof(tmpwbuf)/2);
			else
				wcscpy(tmpwbuf, message_wtext.c_str()); 			   
			
			//change to net order
			int tmpwlen = (int)wcslen(tmpwbuf);
			for (int i = 0; i < tmpwlen; i++)
				tmpwbuf[i] = htons(tmpwbuf[i]);  //change the endian!
			udc = string((char *)tmpwbuf, tmpwlen*2);
		}
		else
			udc = message_text;
	}
	
	//write Empty SCAddress
	//sbuf.SetSCAddr(m_SCAddress);
	GSMAddress EmptySCAddress;
	m_SCAddress = EmptySCAddress;
	sbuf.SetSCAddr(m_SCAddress);
	
	//write TS23.040 SMS_SUBMIT
	sbuf.SetMultiBits(m_Msg_Type_Indicator, 2);
	sbuf.Set1Bit(m_RejectDuplicates);
	
	if(m_ValidPeriodFormat == PRESENT_ENHANCED)   //Not Support
		m_ValidPeriodFormat = NOT_PRESENT;
	
	sbuf.SetMultiBits(m_ValidPeriodFormat, 2);
	m_StatusReportReqInd = false;
	sbuf.Set1Bit(m_StatusReportReqInd);
	if(udhlen > 0)
	{
		m_UserDataHeaderIndicator=true;
		sbuf.Set1Bit(m_UserDataHeaderIndicator);
	}
	m_ReplyPath = false;
	sbuf.Set1Bit(m_ReplyPath);
	sbuf.SetOctet(messageRefrenceCount);	 
	
	sbuf.SetAddr(m_DestAddr);		  
	sbuf.SetOctet(m_ProtocolID);	
	sbuf.SetOctet(m_DataCodingScheme);
	
	if(m_ValidPeriodFormat == PRESENT_ABSOLUTE)  //Set Time Stamp!	 
		sbuf.SetTimeStamp(m_ValidOrScaTimeStamp);	 
	else if(m_ValidPeriodFormat == PRESENT_RELATIVE)
		sbuf.SetOctet(m_ValidRelativePeriod);  
	
	//process userdata length !!		  
	if(udhlen == 0)
		sbuf.SetOctet(udc.length());
	else
	{
		if(m_DataCodingScheme==DCS_DEFAULT) //number of septets!!			 
			sbuf.SetOctet(((1+udhlen)*8+6)/7 + udc.length());
		else
			sbuf.SetOctet(1+udhlen + udc.length());
	}			 
	
	//Start Set UserData Header
	//set the septet point!(for compatible with earlier phase mobile!)
	sbuf.SetSeptetStartOctet();
	if(udhlen > 0)
	{
		sbuf.SetOctet(udhlen);	 //userdata length in octet
		for(int i=0; i < udhlen; i++)
			sbuf.SetOctet(userdataheader[i]);
	}
	
	//Start Set UserData
	if(m_DataCodingScheme==DCS_DEFAULT) //set septets!!
		sbuf.SetString(ISO8859latin1ToGsm(udc));
	else
	{
		for(int i=0; i < udc.length(); i++)
			sbuf.SetOctet(udc[i]);
	}	 
	
	sbuf.GetHexString(hexstr);
	return true;
}

bool SMSMessage::Encode_SMS_DELIVER(string &hexstr)
{
	SMSBuffer sbuf;
	//set value
	m_Msg_Type_Indicator = SMS_DELIVER;
	
	//process user data header and content 
	unsigned char udhlen=0;
	string udc;
	
	if(m_TPUD_list.size()==1)  //Data is pack from EMSData
	{				 
		string str = *(m_TPUD_list.begin());
		if(m_pEmsData->udhi)
		{
			udhlen = str[0]; //udh len
			userdataheader = string(str.data()+1, udhlen); //udh 
			udc = string(str.data()+1+udhlen, str.size()-udhlen-1); //data, already in big-endian
		}
		else
			udc = str;
	}
	else  //process the text
	{
		udhlen = userdataheader.length();
		if(m_DataCodingScheme == DCS_UCS2)	//convert single-byte buffer to unicode
		{
			wchar_t tmpwbuf[256];
			memset(tmpwbuf, 0, sizeof(tmpwbuf));
			if(message_wtext.length()==0)		 
				MultiByteToWideChar(CP_ACP, MB_PRECOMPOSED, message_text.c_str(), (int)message_text.length(), tmpwbuf,  sizeof(tmpwbuf)/2);
			else
				wcscpy(tmpwbuf, message_wtext.c_str()); 			   
			
			//change to net order
			int tmpwlen = (int)wcslen(tmpwbuf);
			for (int i = 0; i < tmpwlen; i++)
				tmpwbuf[i] = htons(tmpwbuf[i]);  //change the endian!
			udc = string((char *)tmpwbuf, tmpwlen*2);
		}
		else
			udc = message_text;
	}
	
	//write SCAddress
	sbuf.SetSCAddr(m_SCAddress);
	
	//write TS23.040 SMS_DELIVER
	sbuf.SetMultiBits(m_Msg_Type_Indicator, 2);
	sbuf.Set1Bit(m_MoreMessagesToSend); 
	sbuf.Set1Bit(0);  //bit3
	sbuf.Set1Bit(0);  //bit4	
	sbuf.Set1Bit(m_StatusReportReqInd);
	if(udhlen>0)
	{
		m_UserDataHeaderIndicator=true;
		sbuf.Set1Bit(m_UserDataHeaderIndicator);  //UserDataHeaderIndicator
	}
	sbuf.Set1Bit(m_ReplyPath);
	//	  sbuf.SetOctet(m_MessageReference++);	   
	
	sbuf.SetAddr(m_DestAddr);		  
	sbuf.SetOctet(m_ProtocolID);	
	sbuf.SetOctet(m_DataCodingScheme);			  
	sbuf.SetTimeStamp(m_ValidOrScaTimeStamp); //set SCA timestamp		
	
	//process userdata length !!		
	if(udhlen == 0)
		sbuf.SetOctet(udc.length());
	else
	{
		if(m_DataCodingScheme==DCS_DEFAULT) //number of septets!!			 
			sbuf.SetOctet(((1+udhlen)*8+6)/7 + udc.length());
		else
			sbuf.SetOctet(1+udhlen + udc.length());
	}	 
	
	//Start Set UserData Header
	//set the septet point!
	sbuf.SetSeptetStartOctet();
	if(udhlen > 0)
	{
		sbuf.SetOctet(udhlen);	//userdata length in octet
		for(int i=0; i < udhlen; i++)
			sbuf.SetOctet(userdataheader[i]);
	}
	
	//Start Set UserData
	if(m_DataCodingScheme==DCS_DEFAULT) //set septets!! 		   
		sbuf.SetString(ISO8859latin1ToGsm(udc));
	else
	{
		for(int i=0; i < udc.length(); i++)
			sbuf.SetOctet(udc[i]);
	}	 
	
	sbuf.GetHexString(hexstr);	  
	return true;
}

bool SMSMessage::Decode_SMS_SUBMIT(const char *tbuf, int buflen)
{
	SMSBuffer sbuf(tbuf, buflen);	 
	unsigned char udhlen=0; //user data header len
	int udclen = 0;
	int udlen=0;  //hdh+udc
	unsigned char tempbuf[512]; 	  
	
	//clear the buffer first
	message_text = "";
	message_wtext = L"";
	userdataheader = "";
	
	m_SCAddress = sbuf.GetSCAddr();
	
	//write TS23.040 SMS_SUBMIT
	m_Msg_Type_Indicator = (eMessageType)sbuf.GetMultiBits(2);
	m_RejectDuplicates = sbuf.Get1Bit();	
	m_ValidPeriodFormat = (eTimePeriodFormat) sbuf.GetMultiBits(2);
	m_StatusReportReqInd = sbuf.Get1Bit();
	m_UserDataHeaderIndicator = sbuf.Get1Bit();  //UserDataHeaderIndicator
	m_ReplyPath = sbuf.Get1Bit();
	m_MessageReference = sbuf.GetOctet();
	
	m_DestAddr = sbuf.GetAddr();		 
	m_ProtocolID = sbuf.GetOctet();
	m_DataCodingScheme = (eDataCodingScheme)sbuf.GetCodingScheme();
	
	m_SMSTimeStamp.SetCurrentTime();
	m_ValidOrScaTimeStamp.SetCurrentTime();
	if(m_ValidPeriodFormat == PRESENT_ABSOLUTE)//Get Time Stamp!		
		m_ValidOrScaTimeStamp = sbuf.GetTimeStamp();	
	else if(m_ValidPeriodFormat == PRESENT_RELATIVE) //Get Octet!
		m_ValidRelativePeriod = sbuf.GetOctet();	
	else if(m_ValidPeriodFormat == PRESENT_ENHANCED) //read 7 octets
	{
		sbuf.GetOctet(); sbuf.GetOctet(); sbuf.GetOctet();
		sbuf.GetOctet(); sbuf.GetOctet(); sbuf.GetOctet();
		sbuf.GetOctet();
	}	
	
	//process userdata length !!		
	udlen = sbuf.GetOctet();	
	udclen = udlen; 	   
	
	//Start Get UserData Header
	//set the septet point!
	sbuf.SetSeptetStartOctet();
	if(m_UserDataHeaderIndicator)
	{
		udhlen = sbuf.GetOctet();				 
		for(int i=0; i < udhlen; i++)
			tempbuf[i] = sbuf.GetOctet();
		
		userdataheader = string((char *)tempbuf, udhlen);
		if(m_DataCodingScheme==DCS_DEFAULT)  //add 1 for the header len
			udclen = udlen - ((udhlen + 1) * 8 + 6) / 7;
		else
			udclen = udlen - udhlen - 1;
		if(udclen < 0)
			udclen = udlen;
	}
	
	//Start Get UserData
	m_TPUD_list.clear();
	if(m_DataCodingScheme==DCS_DEFAULT) //Get septets!!  
	{
		message_text = GsmToISO8859latin1(sbuf.GetString(udclen));						
		memcpy(tempbuf, message_text.c_str(), message_text.size());
	}
	else if(m_DataCodingScheme==DCS_UCS2)
	{		 
		for(int i=0; i<udclen; i++)
			tempbuf[i] = sbuf.GetOctet();
		
		wchar_t wbuf[512];
		memset(wbuf, 0, sizeof(wbuf));
		memcpy((unsigned char *)wbuf, tempbuf, udclen);
		for(int i=0; i<wcslen(wbuf); i++)
			wbuf[i]=ntohs(wbuf[i]);  //change the endian!
		message_wtext = wbuf;		 
	}
	else
	{
		for(int i=0; i<udclen; i++)
			tempbuf[i] = sbuf.GetOctet();
		message_text = GsmToISO8859latin1((char *)tempbuf); 			   
	}			 
	
	//for EMSData Unpack
	char tpud[512];
	if(userdataheader.size() > 0)
	{
		tpud[0] = userdataheader.size();  //udh len
		memcpy(tpud+1, userdataheader.data(), udhlen);	 //udh
		memcpy(tpud+1+udhlen, tempbuf, udclen);   //data
		m_TPUD_list.push_back(string(tpud, 1+udhlen+udclen));
	}
	else	
		m_TPUD_list.push_back(string((char*)tempbuf, udclen));	  
	
	return true;
}

bool SMSMessage::Decode_SMS_DELIVER(const char *tbuf, int buflen)
{
	SMSBuffer sbuf(tbuf, buflen);	 
	unsigned char udhlen=0; //user data header len
	int udclen = 0;
	int udlen=0;  //hdh+udc
	unsigned char tempbuf[512]; 	  
	
	//clear the buffer first
	message_text = "";
	message_wtext = L"";
	userdataheader = "";
	
	m_SCAddress = sbuf.GetSCAddr();
	
	//write TS23.040 SMS_SUBMIT
	m_Msg_Type_Indicator = (eMessageType)sbuf.GetMultiBits(2);
	m_MoreMessagesToSend = sbuf.Get1Bit();
	sbuf.Get1Bit();  //bit3
	sbuf.Get1Bit();  //bit4
	m_StatusReportReqInd = sbuf.Get1Bit();
	
	m_UserDataHeaderIndicator = sbuf.Get1Bit();  //UserDataHeaderIndicator
	m_ReplyPath = sbuf.Get1Bit();	 
	
	m_DestAddr = sbuf.GetAddr();		 
	m_ProtocolID = sbuf.GetOctet();
	m_DataCodingScheme = (eDataCodingScheme)sbuf.GetCodingScheme();
	
	m_ValidOrScaTimeStamp = sbuf.GetTimeStamp();	//Get Time Stamp	
	m_SMSTimeStamp = m_ValidOrScaTimeStamp;
	
	//process userdata length !!		
	udlen = sbuf.GetOctet();	
	udclen = udlen; 	   
	
	//Start Get UserData Header
	//set the septet point!
	sbuf.SetSeptetStartOctet();
	if(m_UserDataHeaderIndicator)
	{
		udhlen = sbuf.GetOctet();		 
		for(int i=0; i < udhlen; i++)
			tempbuf[i] = sbuf.GetOctet();
		userdataheader = string((char *)tempbuf, udhlen);
		if(m_DataCodingScheme==DCS_DEFAULT)  //add 1 for the header len
			udclen = udlen - ((udhlen + 1) * 8 + 6) / 7;
		else
			udclen = udlen - udhlen - 1;		  
		if(udclen < 0)
			udclen = udlen;
	}		 
	
	//Start Get UserData
	m_TPUD_list.clear();
	if(m_DataCodingScheme==DCS_DEFAULT) //Get septets!! 	   
	{
		message_text = GsmToISO8859latin1(sbuf.GetString(udclen));
		memcpy(tempbuf, message_text.c_str(), message_text.size());
	}
	else if(m_DataCodingScheme==DCS_UCS2)
	{		 
		for(int i=0; i<udclen; i++)
			tempbuf[i] = sbuf.GetOctet();
		
		wchar_t wbuf[512];
		memset(wbuf, 0, sizeof(wbuf));
		memcpy((unsigned char *)wbuf, tempbuf, udclen);
		for(int i=0; i<wcslen(wbuf); i++)
			wbuf[i]=ntohs(wbuf[i]);  //change the endian!
		message_wtext = wbuf;				 
	}
	else
	{
		for(int i=0; i < udclen; i++)
			tempbuf[i] = sbuf.GetOctet();
		message_text = GsmToISO8859latin1((char *)tempbuf); 			  
	}			 
	
	//for EMSData Unpack
	char tpud[512];
	if(userdataheader.size() > 0)
	{
		tpud[0] = userdataheader.size();  //udh len
		memcpy(tpud+1, userdataheader.data(), udhlen);	 //udh
		memcpy(tpud+1+udhlen, tempbuf, udclen);   //data
		m_TPUD_list.push_back(string(tpud, 1+udhlen+udclen));
	}
	else	
		m_TPUD_list.push_back(string((char*)tempbuf, udclen));
	
	return true;
}

bool SMSMessage::operator<(const SMSMessage &rhs)
{
	switch(sort_key)
	{
		case BY_PHONENUMBER:
			return m_DestAddr < rhs.m_DestAddr;
		case BY_TIME:
		default:
			return m_SMSTimeStamp < rhs.m_SMSTimeStamp;
	}
}

void SMSMessage::SetSortKey(efolder_sort_key key)
{
	sort_key = key;
}

::ostream& operator<<(::ostream &os, SMSMessage &sms)
{
	if( !os ) return os;
	int str_len;
	
	//string  message_text;   
	str_len = (int)sms.message_text.size()+1;
	os.write((char*)&str_len, sizeof(str_len));
	os.write(sms.message_text.c_str(), str_len);
	
	//wstring message_wtext;
	str_len = (int)sms.message_wtext.size()+1;
	os.write((char*)&str_len, sizeof(str_len));
	os.write((char *)sms.message_wtext.c_str(), str_len*2);  //wchar_t
	
	//string  userdataheader;
	str_len = (int)sms.userdataheader.size()+1;
	os.write((char*)&str_len, sizeof(str_len));
	os.write(sms.userdataheader.c_str(), str_len);
	
	//CString memtype_str;
	os << sms.memtype_str;
	
	//eMemStatus msgstatus;
	os.write((char *)&(sms.msgstatus), sizeof(sms.msgstatus));
	
	os.write((char*)&(sms.m_MsgSendStatus), sizeof(sms.m_MsgSendStatus));
	
	os.write((char*)&(sms.m_MsgDeleteOrigin), sizeof(sms.m_MsgDeleteOrigin));
	
	os.write((char*)&(sms.m_index), sizeof(sms.m_index));
	
	//GSMAddress m_SCAddress;
	os << sms.m_SCAddress;
	
	//GSMAddress m_DestAddr;
	os << sms.m_DestAddr;
	
	//GSMTimeStamp m_SMSTimeStamp; 
	os << sms.m_SMSTimeStamp;
	
	//GSMTimeStamp m_ValidOrScaTimeStamp;
	os << sms.m_ValidOrScaTimeStamp;
	
	//eMessageType m_Msg_Type_Indicator;
	os.write((char *)&(sms.m_Msg_Type_Indicator), sizeof(sms.m_Msg_Type_Indicator));	
	
	//bool m_RejectDuplicates;
	os.write((char *)&(sms.m_RejectDuplicates), sizeof(sms.m_RejectDuplicates));	
	
	//bool m_ReplyPath;
	os.write((char *)&(sms.m_ReplyPath), sizeof(sms.m_ReplyPath));	  
	
	//bool m_StatusReportReqInd;
	os.write((char *)&(sms.m_StatusReportReqInd), sizeof(sms.m_StatusReportReqInd));	
	
	//unsigned char m_MessageReference;
	os.write((char *)&(sms.m_MessageReference), sizeof(sms.m_MessageReference));	
	
	//unsigned char m_ProtocolID;
	os.write((char *)&(sms.m_ProtocolID), sizeof(sms.m_ProtocolID));	
	
	//eTimePeriodFormat m_ValidPeriodFormat;
	os.write((char *)&(sms.m_ValidPeriodFormat), sizeof(sms.m_ValidPeriodFormat));	  
	
	//eDataCodingScheme m_DataCodingScheme;
	os.write((char *)&(sms.m_DataCodingScheme), sizeof(sms.m_DataCodingScheme));	
	
	//unsigned short m_ValidRelativePeriod;
	os.write((char *)&(sms.m_ValidRelativePeriod), sizeof(sms.m_ValidRelativePeriod));	  
	
	//bool m_MoreMessagesToSend;
	os.write((char *)&(sms.m_MoreMessagesToSend), sizeof(sms.m_MoreMessagesToSend)); 
	
	//bool m_UserDataHeaderIndicator;
	if(sms.m_pEmsData->udhi==KAL_TRUE)
		sms.m_UserDataHeaderIndicator=true;
	
	os.write((char *)&(sms.m_UserDataHeaderIndicator), sizeof(sms.m_UserDataHeaderIndicator)); 
	
	
	//m_TPUD_list
	sms.PackEMSData();
	std::list<string>::iterator iter = sms.m_TPUD_list.begin();
	int nbrofentry = (int)sms.m_TPUD_list.size();
	os.write((char*)&nbrofentry, sizeof(nbrofentry));
	while(iter!=sms.m_TPUD_list.end())
	{
		int data_len = (int)(*iter).size();
		os.write((char*)&data_len, sizeof(data_len));
		os.write((*iter).data(), data_len); 			   
		iter++; 	   
	}
	sms.m_TPUD_list.clear();
	
	return os;
}

::istream& operator>>(::istream &is, SMSMessage &sms)
{
	if( !is ) return is;
	
	int str_len;
	char *str;
	wchar_t *wstr;
	
	//string  message_text;
	is.read((char*)&str_len, sizeof(str_len));
	str = new char[str_len];
	is.read(str, str_len);
	sms.message_text = str;
	delete [] str;
	
	//wstring message_wtext;
	is.read((char*)&str_len, sizeof(str_len));
	wstr = new wchar_t[str_len];
	is.read((char *)wstr, str_len*2);  //wchar_t
	sms.message_wtext = wstr;
	delete [] wstr;
	
	//string  userdataheader;
	is.read((char*)&str_len, sizeof(str_len));
	str = new char[str_len];
	is.read(str, str_len);
	sms.userdataheader = str;
	delete [] str;
	
	//string memtype_str;
	is >> sms.memtype_str; 
	
	//eMemStatus msgstatus;
	is.read((char *)&(sms.msgstatus), sizeof(sms.msgstatus));	
	
	is.read((char*)&(sms.m_MsgSendStatus), sizeof(sms.m_MsgSendStatus));
	
	is.read((char*)&(sms.m_MsgDeleteOrigin), sizeof(sms.m_MsgDeleteOrigin));
	
	is.read((char*)&(sms.m_index), sizeof(sms.m_index));
	
	//GSMAddress m_SCAddress;
	is >> sms.m_SCAddress;
	
	//GSMAddress m_DestAddr;
	is >> sms.m_DestAddr;
	
	//GSMTimeStamp m_SMSTimeStamp; 
	is >> sms.m_SMSTimeStamp;
	
	//GSMTimeStamp m_ValidOrScaTimeStamp;
	is >> sms.m_ValidOrScaTimeStamp;
	
	//eMessageType m_Msg_Type_Indicator;
	is.read((char *)&(sms.m_Msg_Type_Indicator), sizeof(sms.m_Msg_Type_Indicator));
	
	//bool m_RejectDuplicates;
	is.read((char *)&(sms.m_RejectDuplicates), sizeof(sms.m_RejectDuplicates));
	
	//bool m_ReplyPath;
	is.read((char *)&(sms.m_ReplyPath), sizeof(sms.m_ReplyPath));	 
	
	//bool m_StatusReportReqInd;
	is.read((char *)&(sms.m_StatusReportReqInd), sizeof(sms.m_StatusReportReqInd));
	
	//unsigned char m_MessageReference;
	is.read((char *)&(sms.m_MessageReference), sizeof(sms.m_MessageReference));
	
	//unsigned char m_ProtocolID;
	is.read((char *)&(sms.m_ProtocolID), sizeof(sms.m_ProtocolID));
	
	//eTimePeriodFormat m_ValidPeriodFormat;
	is.read((char *)&(sms.m_ValidPeriodFormat), sizeof(sms.m_ValidPeriodFormat));	 
	
	//eDataCodingScheme m_DataCodingScheme;
	is.read((char *)&(sms.m_DataCodingScheme), sizeof(sms.m_DataCodingScheme));
	
	//unsigned short m_ValidRelativePeriod;
	is.read((char *)&(sms.m_ValidRelativePeriod), sizeof(sms.m_ValidRelativePeriod));	 
	
	//bool m_MoreMessagesToSend;
	is.read((char *)&(sms.m_MoreMessagesToSend), sizeof(sms.m_MoreMessagesToSend));
	
	//bool m_UserDataHeaderIndicator
	is.read((char *)&(sms.m_UserDataHeaderIndicator), sizeof(sms.m_UserDataHeaderIndicator));	 
	
	//m_TPUD_list
	int nbrofentry, data_len;
	sms.m_TPUD_list.clear();
	is.read((char*)&nbrofentry, sizeof(nbrofentry));	
	for(; nbrofentry>0; nbrofentry--)
	{
		is.read((char*)&data_len, sizeof(data_len));
		char *str = new char[data_len];
		is.read(str, data_len);
		sms.m_TPUD_list.push_back(string(str, data_len));	 
		delete [] str;
	}
	sms.UnPackEMSData();
	
	return is;
}
/*
 
 bool SMSMessage::Export(FILE *fd)
 {
 _TCHAR tempbuf[4096];
 if(m_pEmsData == NULL || m_pEmsData->textLength==0)
 return true;	
 
 // Reconvert Text
 unsigned char *pTemp = new unsigned char[m_pEmsData->textLength];	
 int nLen2 = m_pEmsData->textLength / 2;
 memcpy(pTemp, m_pEmsData->textBuffer, m_pEmsData->textLength);	
 // Reconvert Text
 for(int i=0;i<nLen2;i++) 
 ((wchar_t*)pTemp)[i] = ntohs(((wchar_t*)pTemp)[i]);
 
 //Convert "" -> " , "\r\n" for csv file
 wstring temp_wstr = wstring((wchar_t *)pTemp, nLen2);
 wstring new_wstr;
 for(i=0;i<temp_wstr.size();i++)
 {
 if(temp_wstr[i] == _L('\r') || temp_wstr[i] == _L('\n'))
 {
 }
 else
 {
 new_wstr += temp_wstr[i];
 if(temp_wstr[i] == _L('\"'))
 new_wstr += _L('\"');
 }		
 }
 
 // Get String	
 #ifdef _UNICODE
 CString msg_str = new_wstr.c_str(); 			
 #else
 char *pBuf;
 int nLen = WideCharToMultiByte(CP_ACP, 0, new_wstr.c_str(), new_wstr.size(), NULL, 0, 0, 0);
 pBuf = new char[nLen + 1];	
 WideCharToMultiByte(CP_ACP, 0, new_wstr.c_str(), new_wstr.size(), pBuf, nLen, 0, 0);
 pBuf[nLen] = '\0';
 CString msg_str = pBuf; 			
 delete [] pBuf; 
 #endif		
 
 // Delete Temp Buffer		
 delete [] pTemp;	
 
 string telnum;
 telnum = m_DestAddr.GetNumStr();
 // use \t instead of , in order to let excel can open unicode text file
 //	_stprintf(tempbuf, _T("\"%s\",\"%s\",\"%s\"\r\n"), CString(telnum.c_str()), CString(m_SMSTimeStamp.GetTimeString().c_str()), msg_str);
 _stprintf(tempbuf, _T("\"%s\"\t\"%s\"\t\"%s\"\r\n"), CString(telnum.c_str()), CString(m_SMSTimeStamp.GetTimeString().c_str()), msg_str);
 
 #ifdef _UNICODE
 int ts = 2;
 #else
 int ts = 1;
 #endif	
 
 if(fwrite(tempbuf, ts, _tcslen(tempbuf), fd) != _tcslen(tempbuf))
 return false;
 return true;
 }
 
 bool SMSMessage::Import(CString &str)
 {	 
 int pos1 = str.Find(_T('"'));
 if(pos1 == -1 || pos1 == str.GetLength()-1)
 return false;	 
 
 int pos2 = str.Find(_T('"'), pos1+1);
 if(pos2 == -1 || pos2 == str.GetLength()-1)
 return false;	 
 
 int pos3 = str.Find(_T('"'), pos2+1);
 if(pos3 == -1 || pos3 == str.GetLength()-1)
 return false;	 
 
 int pos4 = str.Find(_T('"'), pos3+1);
 if(pos4 == -1 || pos4 == str.GetLength()-1)
 return false;	 
 
 int pos5 = str.Find(_T('"'), pos4+1);
 if(pos5 == -1 || pos5 == str.GetLength()-1)
 return false;	 
 
 int pos6 = str.ReverseFind(_T('"'));
 if(pos6 == -1 || pos6 == str.GetLength()-1)
 return false;	 
 
 m_DestAddr.SetGSMAddress(UCS2toASCII(str.Mid(pos1+1, pos2-pos1-1)));   //don't count "
 m_SMSTimeStamp.SetTimeString(UCS2toASCII(str.Mid(pos3+1, pos4-pos3-1)));
 
 CString msg_str = str.Mid(pos5+1, pos6-pos5-1);
 
 #ifdef _UNICODE
 //replace "" -> "	
 msg_str.Replace(_L("\"\""), _L("\""));
 wstring temp_wstr = (LPCTSTR)msg_str;
 #else
 //convert to WideChar
 wchar_t tmpwbuf[17085];  //max of available UCS2 wchars 			
 MultiByteToWideChar(CP_ACP, MB_PRECOMPOSED, msg_str, -1, tmpwbuf,  sizeof(tmpwbuf)/2);
 
 //replace "" -> "	
 wstring::size_type s=0;
 wstring temp_wstr = tmpwbuf;
 while((s = temp_wstr.find(_L("\"\""), s)) != wstring::npos)
 {
 temp_wstr.replace(s, 2, _L("\""));
 s=s+1;
 }		
 #endif
 
 //change to net order	 
 for (int i=0; i<temp_wstr.size(); i++)
 temp_wstr[i] = htons(temp_wstr[i]);  //change the endian!
 
 
 if(m_pEmsData)
 ReleaseEMSData(m_pEmsData);
 else
 m_pEmsData = new EMSData;
 InitializeEMSData(m_pEmsData, m_DataCodingScheme);
 
 EMSTextFormat tf;
 ResetTextFormat(&tf);
 AddString(m_pEmsData, (unsigned char*)temp_wstr.c_str(), temp_wstr.size(), &tf);
 memtype_str = "PC";
 
 bool b = false;
 //check if use unicode
 #ifdef _UNICODE 
 for(i=0;i<msg_str.GetLength();i++)
 {
 if((unsigned short)msg_str[i]>(unsigned short)127)
 b = true;
 }
 if(b==false)
 {
 int Len = WideCharToMultiByte(CP_ACP, 0, (LPCTSTR)msg_str, -1, NULL, 0, NULL, NULL);
 if(Len!=msg_str.GetLength())
 b=true;
 }
 #else
 for(i=0;i<msg_str.GetLength();i++)
 {
 if((unsigned char)msg_str[i]>(unsigned char)127)
 b = true;
 }	
 if(b==false)
 {
 int wLen = MultiByteToWideChar(CP_ACP, 0, (LPCSTR)msg_str, msg_str.GetLength(), NULL, 0);
 if(wLen!=msg_str.GetLength())
 b=true;
 }
 #endif	
 
 if(b)
 {
 m_DataCodingScheme = DCS_UCS2;
 EMSSetDCS(m_pEmsData, SMSAL_UCS2_DCS);
 }
 else
 {
 m_DataCodingScheme = DCS_DEFAULT;
 EMSSetDCS(m_pEmsData, SMSAL_DEFAULT_DCS);
 }
 
 return true;
 }
 */

void SMSMessage::SetTimeStamp()
{
	m_SMSTimeStamp.SetCurrentTime();
}
