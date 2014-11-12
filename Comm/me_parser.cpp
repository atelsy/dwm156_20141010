/*****************************************************************************
*  Copyright Statement:
*  --------------------
*  This software is protected by Copyright and the information contained
*  herein is confidential. The software may not be copied and the information
*  contained herein may not be used or disclosed except with the written
*  permission of MediaTek Inc. (C) 2011
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

#include "me_comm.h"
#include "ATResult.h"
#include "me_parser.h"

////////////////////////////////////////////////////////////////////////
//response parser
static void AddExpectedCmd(void *pfsm, ATParamLst &paraLst)
{
	ATParamElem elem;
	
	elem.type = AT_STRING;
	elem.str_value = string(((PMEFSM)pfsm)->req.expectedCmd);	
	paraLst.eleLst.push_back(elem);
}

void ME_Common_Query_Parser(void *pfsm, void *pObj)
{
	char *rawdata = ((PMEFSM)pfsm)->ctldata;

	char buf[128];
	ATParamElem elem;
	ATParamLst paraLst;
	ATResult *pRet = (ATResult*)((PMEFSM)pfsm)->req.context;
	
	AddExpectedCmd(pfsm, paraLst);
	
	elem.type = AT_STRING;
	GetString(rawdata, buf, GetSignIdx(rawdata, ':', 1), me_strlen(rawdata));
	elem.str_value = string(buf);
	paraLst.eleLst.push_back(elem);
	
	pRet->resultLst.push_back(paraLst);

}

void ME_Query_ModemBearer_Parser(void *pfsm, void *pObj)
{
	int begin;
	char *rawdata = ((PMEFSM)pfsm)->ctldata;

	ATParamElem elem;
	ATParamLst paraLst;
	ATResult *pRet = (ATResult*)((PMEFSM)pfsm)->req.context;

	AddExpectedCmd(pfsm, paraLst);

	begin = GetSignIdx(rawdata, ':', 1);
	elem.type = AT_INTEGER;
	elem.int_value = GetInteger(rawdata, begin, me_strlen(rawdata));
	paraLst.eleLst.push_back(elem);
	
	pRet->resultLst.push_back(paraLst);
}

void ME_Query_BandInfo_Parser(void *pfsm, void *pObj)
{
	int begin, end;
	char *rawdata = ((PMEFSM)pfsm)->ctldata;
	PBAND_INFO rspobj = (PBAND_INFO)pObj;
	
	begin = GetSignIdx(rawdata, ':', 1);
	end	  = GetSignIdx(rawdata, ',', 1);
	rspobj->gsm_band  = GetInteger(rawdata, begin, end);
	rspobj->umts_band = GetInteger(rawdata, end, me_strlen(rawdata));
}

void ME_Query_IMEI_Parser(void *pfsm, void *pObj)
{
	char *rawdata = ((PMEFSM)pfsm)->ctldata;

	ATParamElem elem;
	ATParamLst paraLst;
	ATResult *pRet = (ATResult*)((PMEFSM)pfsm)->req.context;
	
	elem.type = AT_STRING;
	elem.str_value = string(rawdata);
	paraLst.eleLst.push_back(elem);

	pRet->resultLst.push_back(paraLst);

}

void ME_Query_NetwrokReg_Parser(void *pfsm, void *pObj)
{
	//+CREG: <n>,<stat>[,<lac>,<ci>[,<Act>]]
	int begin, end;
	char *rawdata = ((PMEFSM)pfsm)->ctldata;

	ATParamElem elem;
	ATParamLst paraLst;
	ATResult *pRet = (ATResult*)((PMEFSM)pfsm)->req.context;
	
	AddExpectedCmd(pfsm, paraLst);

	begin = GetSignIdx(rawdata, ':', 1);
	end	  = GetSignIdx(rawdata, ',', 1);
	elem.type = AT_INTEGER;
	elem.int_value = GetInteger(rawdata, begin, end);
	paraLst.eleLst.push_back(elem);
		
	elem.type = AT_INTEGER;
	if(GetSignIdx(rawdata, ',', 2) < 0)
		elem.int_value = GetInteger(rawdata, end, me_strlen(rawdata));
	else
		elem.int_value = GetInteger(rawdata, end, GetSignIdx(rawdata, ',', 2));
	paraLst.eleLst.push_back(elem);
	
	begin = GetSignIdx(rawdata, '\"', 1);
	if(begin == -1)
	{
		pRet->resultLst.push_back(paraLst);
		return;
	}
	
	end = GetSignIdx(rawdata, '\"', 2);
	if(end - begin > 1)
	{
		char lac[8];
		GetString(rawdata, lac, begin,  end);
		elem.type = AT_STRING;
		elem.str_value = string(lac);
		paraLst.eleLst.push_back(elem);
	}
	
	begin = GetSignIdx(rawdata, '\"', 3);
	end   = GetSignIdx(rawdata, '\"', 4);
	if(end - begin > 1)
	{
		char ci[8];
		GetString(rawdata, ci, begin, end);
		elem.type = AT_STRING;
		elem.str_value = string(ci);
		paraLst.eleLst.push_back(elem);
	}
	
	begin = GetSignIdx(rawdata, ',', 4);
	if(begin != -1)
	{
		elem.type = AT_INTEGER;
		elem.int_value = GetInteger(rawdata, begin, me_strlen(rawdata));
		paraLst.eleLst.push_back(elem);
	}
	
	pRet->resultLst.push_back(paraLst);
  		
}

void ME_Query_PINRemainNum_Parser(void *pfsm, void *pObj)
{
	int begin, end;
	char *rawdata = ((PMEFSM)pfsm)->ctldata;

	ATParamElem elem;
	ATParamLst paraLst;
	ATResult *pRet = (ATResult*)((PMEFSM)pfsm)->req.context;
	
	AddExpectedCmd(pfsm, paraLst);
	
	begin = GetSignIdx(rawdata, ':', 1);
	end	  = GetSignIdx(rawdata, ',', 1);
	elem.type = AT_INTEGER;
	elem.int_value = GetInteger(rawdata, begin, end);
	paraLst.eleLst.push_back(elem);
	
	begin = end + 1;
	end	  = GetSignIdx(rawdata, ',', 2);
	elem.type = AT_INTEGER;
	elem.int_value = GetInteger(rawdata, begin, end);
	paraLst.eleLst.push_back(elem);

	begin = end + 1;
	end	  = GetSignIdx(rawdata, ',', 3);
	elem.type = AT_INTEGER;
	elem.int_value = GetInteger(rawdata, begin, end);
	paraLst.eleLst.push_back(elem);

	elem.type = AT_INTEGER;
	elem.int_value = GetInteger(rawdata, end+1, me_strlen(rawdata));
	paraLst.eleLst.push_back(elem);

	pRet->resultLst.push_back(paraLst);	

}

void ME_Query_MSISDNInfo_Parser(void *pfsm, void *pObj)
{
	//+CNUM: [<alpha1>],<number1>,<type1>[,<speed>,<service>[,<itc>]]
	int begin, end;
	int idx = 1;
	char *rawdata = ((PMEFSM)pfsm)->ctldata;
	PMSISDN_INFO rspobj = (PMSISDN_INFO)pObj;
	
	if(rspobj->list >= ME_EXPECT_SIZE)
		return;
	
	rspobj->msisdn[rspobj->list].alphax[0] = '\0';
	rspobj->msisdn[rspobj->list].number[0] = '\0';
	rspobj->msisdn[rspobj->list].type  = -1;
	rspobj->msisdn[rspobj->list].speed = -1;
	rspobj->msisdn[rspobj->list].type  = -1;
	rspobj->msisdn[rspobj->list].itc   = -1;
	
	//alpha
	begin = GetSignIdx(rawdata, '\"', 1);
	end	  = GetSignIdx(rawdata, ',', 1);
	if(begin < end)
	{
		end	  = GetSignIdx(rawdata, '\"', 2);
		if(end - begin > 1)
			GetString(rawdata, rspobj->msisdn[rspobj->list].alphax, begin, end);
		idx = 3;
	}
	
	//number
	begin = GetSignIdx(rawdata, '\"', idx);
	end = GetSignIdx(rawdata, '\"', idx+1);
	if(end - begin > 1)
		GetString(rawdata, rspobj->msisdn[rspobj->list].number, begin, end);
	
	//type
	begin = GetSignIdx(rawdata, ',', 2);
	end = GetSignIdx(rawdata, ',', 3);
	if(end == -1)
	{
		rspobj->msisdn[rspobj->list].type = GetInteger(rawdata, begin, me_strlen(rawdata));
		rspobj->list++;
		return;
	}
	else
		rspobj->msisdn[rspobj->list].type = GetInteger(rawdata, begin, end);
	
	//speed	
	begin = end;
	end = GetSignIdx(rawdata, ',', 4);
	if(end - begin > 1)
		rspobj->msisdn[rspobj->list].speed = GetInteger(rawdata, begin, end);
	
	
	//service
	begin = end;
	end = GetSignIdx(rawdata, ',', 5);
	if(end == -1)
	{
		rspobj->msisdn[rspobj->list].service = GetInteger(rawdata, begin, me_strlen(rawdata));
		rspobj->list++;
		return;	
	}
	else
		rspobj->msisdn[rspobj->list].service = GetInteger(rawdata, begin, end);
	
	//itc
	begin = end;
	rspobj->msisdn[rspobj->list].itc = GetInteger(rawdata, begin, me_strlen(rawdata));
	rspobj->list++;
}

void ME_Set_FacLock_Parser(void  *pfsm, void *pObj)
{
	int begin, end;	
	char *rawdata = ((PMEFSM)pfsm)->ctldata;

	ATParamElem elem;
	ATParamLst paraLst;
	ATResult *pRet = (ATResult*)((PMEFSM)pfsm)->req.context;
	
	AddExpectedCmd(pfsm, paraLst);
	
	begin = GetSignIdx(rawdata, ':', 1);
	end	  = GetSignIdx(rawdata, ',', 1);
	if(end == -1)
	{
		elem.type = AT_INTEGER;
		elem.int_value = GetInteger(rawdata, begin, me_strlen(rawdata));
		paraLst.eleLst.push_back(elem);
	}
	else
	{
		elem.type = AT_INTEGER;
		elem.int_value = GetInteger(rawdata, begin, end);
		paraLst.eleLst.push_back(elem);

		elem.type = AT_INTEGER;
		elem.int_value = GetInteger(rawdata, end, me_strlen(rawdata));
		paraLst.eleLst.push_back(elem);
	}

	pRet->resultLst.push_back(paraLst);	

}

void ME_Query_OperatorList_Parser(void *pfsm, void *pObj)
{
	//+COPS: [list of supported (<stat>,long alphanumeric <oper>,short alphanumeric <oper>,numeric <oper>,[,<Act>])s][,,(list of supported <mode>s),(list of supported <format>s)]
	//+COPS: (2,"China Mobile","CMCC","46000",0),(3,"China Unicom","CU-GSM","46001",2),(3,"China Unicom","CU-GSM","46001",0),,(0-3),(0-2)

	int begin, end;
	char *rawdata = ((PMEFSM)pfsm)->ctldata;
	
	char buf[128];
	ATParamElem elem, paraElem;
	ATParamLst paraLst;
	ATResult *pRet = (ATResult*)((PMEFSM)pfsm)->req.context;
	
	AddExpectedCmd(pfsm, paraLst);

	char *dst, *src;
	int bStart = 0, quo = 0;
  
	src = dst = (char*)me_malloc(me_strlen(rawdata));
  
	while(*rawdata != '\0')
	{
		if((*rawdata == '(') && (bStart == 0))
		{
			*dst++ = *rawdata++;
			bStart = 1;
			continue;
		}
		
		if(bStart)
		{
			if(*rawdata == ')')
			{
				if(quo%2 != 0)
				{
					*dst++ = *rawdata;
					continue;
				}
				*dst++ = ')';
				*dst = '\0';
				bStart = 0;
				quo = 0;
				dst = src;
				//parse 
				if(GetSignCount(src, '\"') > 1)
				{ 
					paraElem.paramLst.clear();

					end	= GetSignIdx(src, ',', 1);
					elem.type = AT_INTEGER;
					elem.int_value = GetInteger(src, 0, end);;
					paraElem.paramLst.push_back(elem);

					begin = GetSignIdx(src, '\"', 1);
					end = GetSignIdx(src, '\"', 2);
					GetString(src, buf, begin, end);
					elem.type = AT_STRING;
					elem.str_value = string(buf);
					paraElem.paramLst.push_back(elem);

					begin = GetSignIdx(src, '\"', 3);
					end = GetSignIdx(src, '\"', 4);
					GetString(src, buf, begin, end);
					elem.type = AT_STRING;
					elem.str_value = string(buf);
					paraElem.paramLst.push_back(elem);
					
					begin = GetSignIdx(src, '\"', 5);
					end = GetSignIdx(src, '\"', 6);
					GetString(src, buf, begin, end);
					elem.type = AT_STRING;
					elem.str_value = string(buf);
					paraElem.paramLst.push_back(elem);

					begin = GetSignIdx(src, ',', 4);
					if(begin > 0)
					{
						elem.type = AT_INTEGER;
						elem.int_value = GetInteger(src, begin, me_strlen(src)-1);
						paraElem.paramLst.push_back(elem);
					}
					
					paraLst.eleLst.push_back(paraElem);					
 
				}
				else
				{
					end = GetSignIdx(src, '-', 1);
					if(end > 0)
					{ 
						elem.type = AT_INTEGER;
						elem.int_range_begin = GetInteger(src, 0, end);
						elem.int_range_end   = GetInteger(src, end, me_strlen(src));
						paraLst.eleLst.push_back(elem);	
 
					}
				}
			}
			else
			{
				if(*rawdata == '\"')
					quo++;
				*dst++ = *rawdata;
			}
		}	
		rawdata++;
	}

	me_free(src);

	pRet->resultLst.push_back(paraLst);	


}

void ME_List_CurrentCall_Parser(void *pfsm, void *pObj)
{
	//+CLCC: <id1>,<dir>,<stat>,<mode>,<mpty>[,<number>,<type>[,<alpha>]]
	int begin, end;
	char buf[128];
	char *rawdata = ((PMEFSM)pfsm)->ctldata;

	ATParamElem elem;
	ATParamLst paraLst;
	ATResult *pRet = (ATResult*)((PMEFSM)pfsm)->req.context;
	
	AddExpectedCmd(pfsm, paraLst);
	
	//id1
	begin = GetSignIdx(rawdata, ':', 1);
	end	  = GetSignIdx(rawdata, ',', 1);
	elem.type = AT_INTEGER;
	elem.int_value = GetInteger(rawdata, begin, end);
	paraLst.eleLst.push_back(elem);

	//dir
	begin = end;
	end	  = GetSignIdx(rawdata, ',', 2);
	elem.type = AT_INTEGER;
	elem.int_value = GetInteger(rawdata, begin, end);
	paraLst.eleLst.push_back(elem);

	//stat
	begin = end;
	end	  = GetSignIdx(rawdata, ',', 3);
	elem.type = AT_INTEGER;
	elem.int_value = GetInteger(rawdata, begin, end);
	paraLst.eleLst.push_back(elem);

	//mode
	begin = end;
	end	  = GetSignIdx(rawdata, ',', 4);
	elem.type = AT_INTEGER;
	elem.int_value = GetInteger(rawdata, begin, end);
	paraLst.eleLst.push_back(elem);

	//mpty
	begin = end;
	end	  = GetSignIdx(rawdata, ',', 5);
	if(end == -1)
	{
		elem.type = AT_INTEGER;
		elem.int_value = GetInteger(rawdata, begin, me_strlen(rawdata));
		paraLst.eleLst.push_back(elem);
		pRet->resultLst.push_back(paraLst);	
		return;
	}
	elem.type = AT_INTEGER;
	elem.int_value = GetInteger(rawdata, begin, end);
	paraLst.eleLst.push_back(elem);

	//number
	begin = GetSignIdx(rawdata, '\"', 1);
	end	  = GetSignIdx(rawdata, '\"', 2);
	elem.type = AT_STRING;
	GetString(rawdata, buf, begin, end);
	elem.str_value = string(buf);
	paraLst.eleLst.push_back(elem);

	//type
	begin = GetSignIdx(rawdata, ',', 6);
	end	  = GetSignIdx(rawdata, ',', 7);
	if(end == -1)
	{
		elem.type = AT_INTEGER;
		elem.int_value = GetInteger(rawdata, begin, me_strlen(rawdata));
		paraLst.eleLst.push_back(elem);
		pRet->resultLst.push_back(paraLst);	
		return;
	}
	elem.type = AT_INTEGER;
	elem.int_value = GetInteger(rawdata, begin, end);
	paraLst.eleLst.push_back(elem);
	
	//alpha
	begin = GetSignIdx(rawdata, '\"', 3);
	elem.type = AT_STRING;
	GetString(rawdata, buf, begin, me_strlen(rawdata)-1);
	elem.str_value = string(buf);
	paraLst.eleLst.push_back(elem);

	pRet->resultLst.push_back(paraLst);	
}

void ME_Set_CallForward_Parser(void *pfsm, void *pObj)
{
	// +CCFC: <status>,<class>[,<number>,<type>[,<subaddr>,<satype>[,<time>]]]
	int begin, end;
	char buf[128];
	char *rawdata = ((PMEFSM)pfsm)->ctldata;
	
	ATParamElem elem;
	ATParamLst paraLst;
	ATResult *pRet = (ATResult*)((PMEFSM)pfsm)->req.context;
	
	AddExpectedCmd(pfsm, paraLst);
	
	//status
	begin = GetSignIdx(rawdata, ':', 1);
	end	  = GetSignIdx(rawdata, ',', 1);
	elem.type = AT_INTEGER;
	elem.int_value = GetInteger(rawdata, begin, end);
	paraLst.eleLst.push_back(elem);

	//class
	begin = end;
	end	  = GetSignIdx(rawdata, ',', 2);
	if(end == -1)
	{
		end = me_strlen(rawdata);
		elem.type = AT_INTEGER;
		elem.int_value = GetInteger(rawdata, begin, end);
		paraLst.eleLst.push_back(elem);
		pRet->resultLst.push_back(paraLst);	
		return;
	}
	elem.type = AT_INTEGER;
	elem.int_value = GetInteger(rawdata, begin, end);
	paraLst.eleLst.push_back(elem);
	
	//number
	begin = GetSignIdx(rawdata, '\"', 1);
	end   = GetSignIdx(rawdata, '\"', 2);
	elem.type = AT_INTEGER;
	GetString(rawdata, buf, begin, end);
	elem.str_value = string(buf);
	paraLst.eleLst.push_back(elem);

	//type
	begin = GetSignIdx(rawdata, ',', 3);
	end = GetSignIdx(rawdata, ',', 4);
	if(end == -1)
	{
		end = me_strlen(rawdata);
		elem.type = AT_INTEGER;
		elem.int_value = GetInteger(rawdata, begin, end);
		paraLst.eleLst.push_back(elem);
		pRet->resultLst.push_back(paraLst);	
		return;
	}
	elem.type = AT_INTEGER;
	elem.int_value = GetInteger(rawdata, begin, end);
	paraLst.eleLst.push_back(elem);

	//subaddr
	begin = GetSignIdx(rawdata, '\"', 3);
	end   = GetSignIdx(rawdata, '\"', 4);
	elem.type = AT_STRING;
	if((begin == -1) || (begin - end == 1))
	{
		buf[0] = 0;
		buf[1] = 0;
	}
	else
		GetString(rawdata, buf, begin, end);
	elem.str_value = string(buf);
	paraLst.eleLst.push_back(elem);

	//satype
	begin = GetSignIdx(rawdata, ',', 5);
	end = GetSignIdx(rawdata, ',', 6);
	if(end == -1)
	{
		end = me_strlen(rawdata);
		elem.type = AT_INTEGER;
		elem.int_value = GetInteger(rawdata, begin, end);
		paraLst.eleLst.push_back(elem);
		pRet->resultLst.push_back(paraLst);	
		return;
	}
	elem.type = AT_INTEGER;
	elem.int_value = GetInteger(rawdata, begin, end);
	paraLst.eleLst.push_back(elem);

	//time
	elem.type = AT_INTEGER;
	elem.int_value = GetInteger(rawdata, end, me_strlen(rawdata));
	paraLst.eleLst.push_back(elem);

	pRet->resultLst.push_back(paraLst);	
}

void ME_Set_CallWaiting_Parser(void *pfsm, void *pObj)
{
	//+CCWA: <status>,<class1>
	int begin, end;
	char *rawdata = ((PMEFSM)pfsm)->ctldata;
	
	ATParamElem elem;
	ATParamLst paraLst;
	ATResult *pRet = (ATResult*)((PMEFSM)pfsm)->req.context;
	
	AddExpectedCmd(pfsm, paraLst);
	
	//status
	begin = GetSignIdx(rawdata, ':', 1);
	end	  = GetSignIdx(rawdata, ',', 1);
	elem.type = AT_INTEGER;
	elem.int_value = GetInteger(rawdata, begin, end);
	paraLst.eleLst.push_back(elem);
	
	//class
	elem.type = AT_INTEGER;
	elem.int_value = GetInteger(rawdata, end, me_strlen(rawdata));
	paraLst.eleLst.push_back(elem);

	pRet->resultLst.push_back(paraLst);	
}

void ME_Query_Operator_Parser(void *pfsm, void *pObj)
{
	//+COPS: <mode>[,<format>,<oper>]
	int begin, end;
	char *rawdata = ((PMEFSM)pfsm)->ctldata;
 
	char oper[32];
	ATParamElem elem;
	ATParamLst paraLst;
	ATResult *pRet = (ATResult*)((PMEFSM)pfsm)->req.context;

	AddExpectedCmd(pfsm, paraLst);
	
	begin = GetSignIdx(rawdata, ':', 1);
	end = GetSignIdx(rawdata, ',', 1);
	if((end - begin) < 1)
	{
		elem.type = AT_INTEGER;
		elem.int_value = GetInteger(rawdata, begin, me_strlen(rawdata));
		paraLst.eleLst.push_back(elem);
		pRet->resultLst.push_back(paraLst);	
		return;
	}

	elem.type = AT_INTEGER;
	elem.int_value = GetInteger(rawdata, begin, end);
	paraLst.eleLst.push_back(elem);

	begin = end;
	end = GetSignIdx(rawdata, ',', 2);
	elem.type = AT_INTEGER;
	elem.int_value = GetInteger(rawdata, begin, end);
	paraLst.eleLst.push_back(elem);

	GetString(rawdata, oper, GetSignIdx(rawdata, '\"', 1), me_strlen(rawdata)-1);
	elem.type = AT_STRING;
	elem.str_value = string(oper);
	paraLst.eleLst.push_back(elem);

	pRet->resultLst.push_back(paraLst);	
 
}

void ME_Query_PDPState_Paser(void *pfsm, void *pObj)
{
	//+CGACT: <cid>, <state>
	//[<CR><LF>+CGACT: <cid>, <state>
	//[...]]
	
	int begin, end;	
	char *rawdata = ((PMEFSM)pfsm)->ctldata;
	PPDP_CONTEXT rspobj = (PPDP_CONTEXT)pObj;
	
	if(rspobj->list >= ME_EXPECT_SIZE)
		return;
	
	begin = GetSignIdx(rawdata, ':', 1);
	end	  = GetSignIdx(rawdata, ',', 1);
	
	rspobj->context[rspobj->list].cid = GetInteger(rawdata, begin, end);
	rspobj->context[rspobj->list].state   = GetInteger(rawdata, end, me_strlen(rawdata));	
	rspobj->list ++;
}

void ME_Query_PrcoCfg_Parser(void *pfsm, void *pObj)
{
	//+CGPRCO: <cid>,<DNS1>,<DNS2>
	int begin, end;
	char *rawdata = ((PMEFSM)pfsm)->ctldata;
	PCGPRCO rspobj = (PCGPRCO)pObj;
	
	if(rspobj->list >= ME_EXPECT_SIZE)
		return;

	begin = GetSignIdx(rawdata, ':', 1);
	end = GetSignIdx(rawdata, ',', 1);
	
	rspobj->cfg[rspobj->list].cid = GetInteger(rawdata, begin, end);
	GetString(rawdata, rspobj->cfg[rspobj->list].dns1, end, GetSignIdx(rawdata, ',', 2));
	GetString(rawdata, rspobj->cfg[rspobj->list].dns2, GetSignIdx(rawdata, ',', 2), me_strlen(rawdata));
	rspobj->list++;
}

void ME_Query_PDPAddr_Parser(void *pfsm, void *pObj)
{
	//+CGPADDR: <cid>,<PDP_addr>
	int begin, end;
	char *rawdata = ((PMEFSM)pfsm)->ctldata;
	PPDP_ADDR rspobj = (PPDP_ADDR)pObj;
	
	begin = GetSignIdx(rawdata, ':', 1);
	end = GetSignIdx(rawdata, ',', 1);
	
	rspobj->cid = GetInteger(rawdata, begin, end);

	begin = GetSignIdx(rawdata, '\"', 1);
	end = GetSignIdx(rawdata, '\"', 2);
	GetString(rawdata, rspobj->pdp_addr, begin, end);
}

void ME_Query_SignalQuality_Parser(void *pfsm, void *pObj)
{
	int begin, end;
	char *rawdata = ((PMEFSM)pfsm)->ctldata;
	PSIGNAL_QUALITY rspobj = (PSIGNAL_QUALITY)pObj;
	
	begin = GetSignIdx(rawdata, ':', 1);
	end	  = GetSignIdx(rawdata, ',', 1);

	rspobj->rssi  = GetInteger(rawdata, begin, end);
	rspobj->ber   = GetInteger(rawdata, end, me_strlen(rawdata));
}

void ME_Query_IMSI_Parser(void *pfsm, void *pObj)
{
	char *rawdata = ((PMEFSM)pfsm)->ctldata;
 
	ATParamElem elem; 
	ATParamLst paraLst; 
	ATResult *pRet = (ATResult*)((PMEFSM)pfsm)->req.context; 
	
	elem.type = AT_STRING; 
	elem.str_value = string(rawdata); 
	paraLst.eleLst.push_back(elem); 
	
	pRet->resultLst.push_back(paraLst); 
 
}

void ME_Read_SMS_Parser(void *pfsm, void *pObj)
{
	int begin, end;
	char *rawdata = ((PMEFSM)pfsm)->ctldata;
 
	char alpha[16];
	ATParamElem elem;
	ATParamLst paraLst;
	ATResult *pRet = (ATResult*)((PMEFSM)pfsm)->req.context;

	int nStep = ((PMEFSM)pfsm)->req.nFsmState;
	if(nStep == FSM_STATE_START)
	{
		AddExpectedCmd(pfsm, paraLst);
	
		begin = GetSignIdx(rawdata, ':', 1);
		end	  = GetSignIdx(rawdata, ',', 1);
		elem.type = AT_INTEGER;
		elem.int_value = GetInteger(rawdata, begin, end);
		paraLst.eleLst.push_back(elem);

		begin = GetSignIdx(rawdata, '\"', 1);
		if(begin != -1)
		{
			end = GetSignIdx(rawdata, '\"', 2);
			GetString(rawdata, alpha, begin, end);
		}
		else
			alpha[0] = 0;

		elem.type = AT_STRING;
		elem.str_value = string(alpha);
		paraLst.eleLst.push_back(elem);

		begin = GetSignIdx(rawdata, ',', 2);
		elem.type = AT_INTEGER;
		elem.int_value = GetInteger(rawdata, begin, me_strlen(rawdata));
		paraLst.eleLst.push_back(elem)	;	
	
		pRet->resultLst.push_back(paraLst);
	}
	else
	{
		elem.type = AT_STRING;
		elem.str_value = string(rawdata);
		pRet->resultLst[0].eleLst.push_back(elem);
	}
 
}

void ME_List_SMS_Parser(void *pfsm, void *pObj)
{
	int begin, end;
	char *rawdata = ((PMEFSM)pfsm)->ctldata;
	PLIST_MSG rspobj = (PLIST_MSG)pObj;

	begin = GetSignIdx(rawdata, ':', 1);
	end	  = GetSignIdx(rawdata, ',', 1);
	rspobj->Msg.index  = GetInteger(rawdata, begin, end);
	
	begin = end;
	end   = GetSignIdx(rawdata, ',', 2);
	rspobj->Msg.stat  = GetInteger(rawdata, begin, end);

	begin = GetSignIdx(rawdata, '\"', 1);
	if(begin != -1)
	{
		end = GetSignIdx(rawdata, '\"', 2);
		GetString(rawdata, rspobj->Msg.alpha, begin, end);
	}
	
	begin = GetSignIdx(rawdata, ',', 3);
	rspobj->Msg.length = GetInteger(rawdata, begin, me_strlen(rawdata));
}


void ME_List_Provisioned_Contexts_Parser(void *pfsm, void *pObj)
{
	//+MBPC: <defined>, <entry_id>, <context_id>, <context_type>, <access_string>, <username>, <passwd>, <comp>, <auth>, <plmn>
	int begin, end;	
	char *rawdata = ((PMEFSM)pfsm)->ctldata;
	PMB_CONTEXT rspobj = (PMB_CONTEXT)pObj;
	
	if(rspobj->list >= ME_EXPECT_SIZE)
		return;
	
	begin = GetSignIdx(rawdata, ':', 1);
	end	  = GetSignIdx(rawdata, ',', 1);	
	rspobj->context[rspobj->list].opcode = GetInteger(rawdata, begin, end);

	begin = end;
	end = GetSignIdx(rawdata, ',', 2);
	rspobj->context[rspobj->list].entry_id = GetInteger(rawdata, begin, end);
	
	begin = end;
	end = GetSignIdx(rawdata, ',', 3);
	rspobj->context[rspobj->list].context_id = GetInteger(rawdata, begin, end);

	begin = end;
	end = GetSignIdx(rawdata, ',', 4);
	rspobj->context[rspobj->list].context_type = GetInteger(rawdata, begin, end);

	begin = GetSignIdx(rawdata, '\"', 1);
	end = GetSignIdx(rawdata, '\"', 2);
	GetString(rawdata, rspobj->context[rspobj->list].access_string, begin, end);

	begin = GetSignIdx(rawdata, '\"', 3);
	end = GetSignIdx(rawdata, '\"', 4);
	GetString(rawdata, rspobj->context[rspobj->list].name, begin, end);

	begin = GetSignIdx(rawdata, '\"', 5);
	end = GetSignIdx(rawdata, '\"', 6);
	GetString(rawdata, rspobj->context[rspobj->list].pwd, begin, end);

	begin = GetSignIdx(rawdata, ',', 7);
	end	  = GetSignIdx(rawdata, ',', 8);	
	rspobj->context[rspobj->list].comp = GetInteger(rawdata, begin, end);

	begin = end;
	end = GetSignIdx(rawdata, ',', 9);
	rspobj->context[rspobj->list].auth = GetInteger(rawdata, begin, end);

	begin = GetSignIdx(rawdata, '\"', 7);
	end = GetSignIdx(rawdata, '\"', 8);
	GetString(rawdata, rspobj->context[rspobj->list].plmn, begin, end);

	rspobj->list ++;
}

void ME_Get_ServiceCenterAddress_Parser(void *pfsm, void *pObj)
{
	int begin;
	char *rawdata = ((PMEFSM)pfsm)->ctldata;
 
	char sca[32];
	ATParamElem elem;
	ATParamLst paraLst;
	ATResult *pRet = (ATResult*)((PMEFSM)pfsm)->req.context;

	AddExpectedCmd(pfsm, paraLst);

	GetString(rawdata, sca, GetSignIdx(rawdata, '\"', 1), GetSignIdx(rawdata, '\"', 2));
	elem.type = AT_STRING;
	elem.str_value = string(sca);
	paraLst.eleLst.push_back(elem);

	begin = GetSignIdx(rawdata, ',', 1);
	if(begin != -1)
	{
		elem.type = AT_INTEGER;
		elem.int_value = GetInteger(rawdata, begin, me_strlen(rawdata));
		paraLst.eleLst.push_back(elem);
	}
	
	pRet->resultLst.push_back(paraLst);
 
}

void ME_Get_MessageMemoryState_Parser(void *pfsm, void *pObj)
{
	int begin, end;
	char *rawdata = ((PMEFSM)pfsm)->ctldata;
 
	ATParamElem elem;
	ATParamLst paraLst;
	ATResult *pRet = (ATResult*)((PMEFSM)pfsm)->req.context;

	AddExpectedCmd(pfsm, paraLst);

	begin = GetSignIdx(rawdata, ':', 1);
	end	  = GetSignIdx(rawdata, ',', 1);
	elem.type = AT_INTEGER;
	elem.int_value = GetInteger(rawdata, begin, end);
	paraLst.eleLst.push_back(elem);

	begin = end;
	end = GetSignIdx(rawdata, ',', 2);
	elem.type = AT_INTEGER;
	elem.int_value = GetInteger(rawdata, begin, end);
	paraLst.eleLst.push_back(elem);

	pRet->resultLst.push_back(paraLst);
 
}

void ME_Read_Transparent_SIMFile_Parser(void *pfsm, void *pObj)
{
	int begin, end;
	char *rawdata = ((PMEFSM)pfsm)->ctldata;
	PSIM_FILE rspobj = (PSIM_FILE)pObj;

	begin = GetSignIdx(rawdata, ':', 1);
	end	  = GetSignIdx(rawdata, ',', 1);
	rspobj->sw1  = GetInteger(rawdata, begin, end);
	
	begin = end;
	end = GetSignIdx(rawdata, ',', 2);
	if(end == -1)
	{
		rspobj->sw2 = GetInteger(rawdata, begin, me_strlen(rawdata));
		rspobj->resp[0] = 0;
		return;	
	}
	rspobj->sw2 = GetInteger(rawdata, begin, end);

	begin = end;
	GetString(rawdata, rspobj->resp, begin, me_strlen(rawdata));
}

void ME_Get_CurrentSMSMemType_Parser(void *pfsm, void *pObj)
{
	//+CPMS: <mem1>, <used1>, <total1>, <mem2>, <used2>, <total2>, <mem3>, <used3>, <total3>
	int begin, end;
	char mem[8];
	char *rawdata = ((PMEFSM)pfsm)->ctldata;

	ATParamElem elem;
	ATParamLst paraLst;
	ATResult *pRet = (ATResult*)((PMEFSM)pfsm)->req.context;

	AddExpectedCmd(pfsm, paraLst);
	
	//mem1
	begin = GetSignIdx(rawdata, '\"', 1);
	end   = GetSignIdx(rawdata, '\"', 2);
	GetString(rawdata, mem, begin, end);
	elem.type = AT_STRING;
	elem.str_value = string(mem);
	paraLst.eleLst.push_back(elem);
	
	//used1
	begin = GetSignIdx(rawdata, ',', 1);
	end   = GetSignIdx(rawdata, ',', 2);
	elem.type = AT_INTEGER;
	elem.int_value = GetInteger(rawdata, begin, end);
	paraLst.eleLst.push_back(elem);

	//total1
	begin = end;
	end	  = GetSignIdx(rawdata, ',', 3);
	elem.type = AT_INTEGER;
	elem.int_value = GetInteger(rawdata, begin, end);
	paraLst.eleLst.push_back(elem);

	//mem2
	begin = GetSignIdx(rawdata, '\"', 3);
	end   = GetSignIdx(rawdata, '\"', 4);
	GetString(rawdata, mem, begin, end);
	elem.type = AT_STRING;
	elem.str_value = string(mem);
	paraLst.eleLst.push_back(elem);
	
	//used2
	begin = GetSignIdx(rawdata, ',', 4);
	end   = GetSignIdx(rawdata, ',', 5);
	elem.type = AT_INTEGER;
	elem.int_value = GetInteger(rawdata, begin, end);
	paraLst.eleLst.push_back(elem);
	
	//total2
	begin = end;
	end	  = GetSignIdx(rawdata, ',', 6);
	elem.type = AT_INTEGER;
	elem.int_value = GetInteger(rawdata, begin, end);
	paraLst.eleLst.push_back(elem);

	//mem3
	begin = GetSignIdx(rawdata, '\"', 5);
	end   = GetSignIdx(rawdata, '\"', 6);
	GetString(rawdata, mem, begin, end);
	elem.type = AT_STRING;
	elem.str_value = string(mem);
	paraLst.eleLst.push_back(elem);
	
	//used3
	begin = GetSignIdx(rawdata, ',', 7);
	end   = GetSignIdx(rawdata, ',', 8);
	elem.type = AT_INTEGER;
	elem.int_value = GetInteger(rawdata, begin, end);
	paraLst.eleLst.push_back(elem);
	
	//total3
	begin = end;
	elem.type = AT_INTEGER;
	elem.int_value = GetInteger(rawdata, begin, me_strlen(rawdata));
	paraLst.eleLst.push_back(elem);

	pRet->resultLst.push_back(paraLst);
}

void ME_Get_SMSMemIndex_Parser(void *pfsm, void *pObj)
{
	//+EQSI: <mem>,<idx0>,<idx1>,<used>
	int begin, end;
	char mem[8];
	char *rawdata = ((PMEFSM)pfsm)->ctldata;
	
	ATParamElem elem;
	ATParamLst paraLst;
	ATResult *pRet = (ATResult*)((PMEFSM)pfsm)->req.context;

	AddExpectedCmd(pfsm, paraLst);

	begin = GetSignIdx(rawdata, '\"', 1);
	if(begin == -1)
	{
		begin = GetSignIdx(rawdata, ':', 1);
		end   = GetSignIdx(rawdata, ',', 1);
	}
	else
		end = GetSignIdx(rawdata, '\"', 2);

	GetString(rawdata, mem, begin, end);
	elem.type = AT_STRING;
	elem.str_value = string(mem);
	paraLst.eleLst.push_back(elem);

	begin = GetSignIdx(rawdata, ',', 1);
	end   = GetSignIdx(rawdata, ',', 2);
	elem.type = AT_INTEGER;
	elem.int_value = GetInteger(rawdata, begin, end);
	paraLst.eleLst.push_back(elem);

	begin = end;
	end   = GetSignIdx(rawdata, ',', 3);
	elem.type = AT_INTEGER;
	elem.int_value = GetInteger(rawdata, begin, end);
	paraLst.eleLst.push_back(elem);

	begin = end;
	elem.type = AT_INTEGER;
	elem.int_value = GetInteger(rawdata, begin, me_strlen(rawdata));
	paraLst.eleLst.push_back(elem);

	pRet->resultLst.push_back(paraLst);
}

void ME_Get_PbMemType_Parser(void *pfsm, void *pObj)
{
	//+CPBS: ("ME","SM","LD","MC","RC","FD","DC","ON")

	int begin, end, count;
	char mem[8];
	char *rawdata = ((PMEFSM)pfsm)->ctldata;

	ATParamElem elem, paraElem;
	ATParamLst paraLst;
	ATResult *pRet = (ATResult*)((PMEFSM)pfsm)->req.context;
	
	AddExpectedCmd(pfsm, paraLst);

	count = GetSignCount(rawdata, ',');
	for(int i=0; i<count+1; i++)
	{
		begin = GetSignIdx(rawdata, '\"', i*2+1);
		end   = GetSignIdx(rawdata, '\"', (i+1)*2);
		GetString(rawdata, mem, begin, end);
		elem.type = AT_STRING;
		elem.str_value = string(mem);
		paraElem.paramLst.push_back(elem);
	}

	paraLst.eleLst.push_back(paraElem);
	pRet->resultLst.push_back(paraLst);		
}

void ME_Get_PbMemState_Parser(void *pfsm, void *pObj)
{
	//+CPBS: "SM", 0, 200
	int begin, end;
	char mem[8];
	char *rawdata = ((PMEFSM)pfsm)->ctldata;
	
	ATParamElem elem;
	ATParamLst paraLst;
	ATResult *pRet = (ATResult*)((PMEFSM)pfsm)->req.context;

	AddExpectedCmd(pfsm, paraLst);
	
	begin = GetSignIdx(rawdata, '\"', 1);
	end   = GetSignIdx(rawdata, '\"', 2);
	GetString(rawdata, mem, begin, end);
	elem.type = AT_STRING;
	elem.str_value = string(mem);
	paraLst.eleLst.push_back(elem);

	end   = GetSignIdx(rawdata, ',', 2);
	elem.type = AT_INTEGER;
	elem.int_value = GetInteger(rawdata, GetSignIdx(rawdata, ',', 1), end);
	paraLst.eleLst.push_back(elem);

	elem.type = AT_INTEGER;
	elem.int_value = GetInteger(rawdata, end, me_strlen(rawdata));
	paraLst.eleLst.push_back(elem);

	pRet->resultLst.push_back(paraLst);		
}

void ME_Get_PbMemInfo_Parser(void *pfsm, void *pObj)
{
	//+CPBR: (1-200), 40, 14
	int begin, end;
	char *rawdata = ((PMEFSM)pfsm)->ctldata;
	
	ATParamElem elem;
	ATParamLst paraLst;
	ATResult *pRet = (ATResult*)((PMEFSM)pfsm)->req.context;
	
	AddExpectedCmd(pfsm, paraLst);
	
	begin = GetSignIdx(rawdata, '(', 1);
	end   = GetSignIdx(rawdata, '-', 1);
	elem.type = AT_INTEGER;

	if(end != -1)
	{
		elem.int_range_begin = GetInteger(rawdata, begin, end);
		begin = end;
	}
	else
		elem.int_range_begin = 0;

	end   = GetSignIdx(rawdata, ')', 1);
	elem.int_range_end = GetInteger(rawdata, begin, end);
	paraLst.eleLst.push_back(elem);
	
	begin = GetSignIdx(rawdata, ',', 1);
	end   = GetSignIdx(rawdata, ',', 2);
	elem.type = AT_INTEGER;
	elem.int_value = GetInteger(rawdata, begin, end);
	paraLst.eleLst.push_back(elem);
	
	elem.type = AT_INTEGER;
	elem.int_value = GetInteger(rawdata, end, me_strlen(rawdata));
	paraLst.eleLst.push_back(elem);
	
	pRet->resultLst.push_back(paraLst);	
}

void ME_Read_Pbk_Parser(void *pfsm, void *pObj)
{
	//+CPBR: 1, "81800818", 129, "006100620063"<, time>
	char buf[256];
	char *rawdata = ((PMEFSM)pfsm)->ctldata;
	
	ATParamElem elem;
	ATParamLst paraLst;
	ATResult *pRet = (ATResult*)((PMEFSM)pfsm)->req.context;

	AddExpectedCmd(pfsm, paraLst);

	elem.type = AT_INTEGER;
	elem.int_value = GetInteger(rawdata, GetSignIdx(rawdata, ':', 1), GetSignIdx(rawdata, ',', 1));
	paraLst.eleLst.push_back(elem);

	GetString(rawdata, buf, GetSignIdx(rawdata, '\"', 1), GetSignIdx(rawdata, '\"', 2));
	elem.type = AT_STRING;
	elem.str_value = string(buf);
	paraLst.eleLst.push_back(elem);

	elem.type = AT_INTEGER;
	elem.int_value = GetInteger(rawdata, GetSignIdx(rawdata, ',', 2), GetSignIdx(rawdata, ',', 3));
	paraLst.eleLst.push_back(elem);

	if(GetSignCount(rawdata, ',') == 3)
	{
		// for Phonebook
		GetString(rawdata, buf, GetSignIdx(rawdata, '\"', 3), me_strlen(rawdata)-1);
		elem.type = AT_STRING;
		elem.str_value = string(buf);
		paraLst.eleLst.push_back(elem);
	}
	else
	{
		//for Call history
		elem.type = AT_STRING;
		elem.str_value = "";
		paraLst.eleLst.push_back(elem);
		
		GetString(rawdata, buf, GetSignIdx(rawdata, '\"', 5), me_strlen(rawdata)-1);
		elem.type = AT_STRING;
		elem.str_value = string(buf);
		paraLst.eleLst.push_back(elem);
	}
	
	pRet->resultLst.push_back(paraLst);	
}

/*
void ME_Get_UserIdentity_Parser(void *pfsm, void *pObj)
{
	//+CIMI: "460029010410574"
	char buf[256];
	char *rawdata = ((PMEFSM)pfsm)->ctldata;
	
	ATParamElem elem;
	ATParamLst paraLst;
	ATResult *pRet = (ATResult*)((PMEFSM)pfsm)->req.context;
	
	AddExpectedCmd(pfsm, paraLst);

	elem.type = AT_STRING;
	GetString(rawdata, buf, GetSignIdx(rawdata, '\"', 1), GetSignIdx(rawdata, '\"', 2));
	elem.str_value = string(buf);
	paraLst.eleLst.push_back(elem);

	pRet->resultLst.push_back(paraLst);	
}
*/

void ME_Query_RATnPS_Parser(void *pfsm, void *pObj)
{
	//+ERAT: 255, 0, 2, 2
	int begin, end;
	char *rawdata = ((PMEFSM)pfsm)->ctldata;
	
	ATParamElem elem;
	ATParamLst paraLst;
	ATResult *pRet = (ATResult*)((PMEFSM)pfsm)->req.context;

	AddExpectedCmd(pfsm, paraLst);
	
	end = GetSignIdx(rawdata, ',', 1);
	elem.type = AT_INTEGER;
	elem.int_value = GetInteger(rawdata, GetSignIdx(rawdata, ':', 1), end);
	paraLst.eleLst.push_back(elem);

	begin = end;
	end = GetSignIdx(rawdata, ',', 2);
	elem.type = AT_INTEGER;
	elem.int_value = GetInteger(rawdata, begin, end);
	paraLst.eleLst.push_back(elem);

	begin = end;
	end = GetSignIdx(rawdata, ',', 3);
	if(end == -1)
	{
		end = me_strlen(rawdata);
		elem.type = AT_INTEGER;
		elem.int_value = GetInteger(rawdata, begin, end);
		paraLst.eleLst.push_back(elem);
		pRet->resultLst.push_back(paraLst);
		return;
	}

	elem.type = AT_INTEGER;
	elem.int_value = GetInteger(rawdata, begin, end);
	paraLst.eleLst.push_back(elem);

	begin = end;
	elem.type = AT_INTEGER;
	elem.int_value = GetInteger(rawdata, begin, me_strlen(rawdata));
	paraLst.eleLst.push_back(elem);

	pRet->resultLst.push_back(paraLst);
}

void ME_Get_CLIR_Parser(void *pfsm, void *pObj)
{
	//+CLIR: <n>,<m>
	int begin, end;
	char *rawdata = ((PMEFSM)pfsm)->ctldata;
	
	ATParamElem elem;
	ATParamLst paraLst;
	ATResult *pRet = (ATResult*)((PMEFSM)pfsm)->req.context;
	
	AddExpectedCmd(pfsm, paraLst);
	
	end = GetSignIdx(rawdata, ',', 1);
	elem.type = AT_INTEGER;
	elem.int_value = GetInteger(rawdata, GetSignIdx(rawdata, ':', 1), end);
	paraLst.eleLst.push_back(elem);
	
	begin = end;
	elem.type = AT_INTEGER;
	elem.int_value = GetInteger(rawdata, begin, me_strlen(rawdata));
	paraLst.eleLst.push_back(elem);
	
	pRet->resultLst.push_back(paraLst);
}

void ME_Query_PIN2_Parser(void *pfsm, void *pObj)
{
	//+EPIN2: <code>
	int begin;
	char buf[128];
	char *rawdata = ((PMEFSM)pfsm)->ctldata;
	
	ATParamElem elem;
	ATParamLst paraLst;
	ATResult *pRet = (ATResult*)((PMEFSM)pfsm)->req.context;
	
	AddExpectedCmd(pfsm, paraLst);

	begin = GetSignIdx(rawdata, '\"', 1);
	elem.type = AT_STRING;
	GetString(rawdata, buf, begin, me_strlen(rawdata)-1);
	elem.str_value = string(buf);
	paraLst.eleLst.push_back(elem);

	pRet->resultLst.push_back(paraLst);
}

void ME_Get_PUCValue_Parser(void *pfsm, void *pObj)
{
	//+CPUC: <currency>,<ppu>
	int begin, end;
	char buf[128];
	char *rawdata = ((PMEFSM)pfsm)->ctldata;
	
	ATParamElem elem;
	ATParamLst paraLst;
	ATResult *pRet = (ATResult*)((PMEFSM)pfsm)->req.context;
	
	AddExpectedCmd(pfsm, paraLst);
	
	begin = GetSignIdx(rawdata, '\"', 1);
	end   = GetSignIdx(rawdata, '\"', 2);
	elem.type = AT_STRING;
	if(begin - end == 1)
	{
		buf[0] = 0;
		buf[1] = 0;
	}
	else
		GetString(rawdata, buf, begin, end);
	elem.str_value = string(buf);
	paraLst.eleLst.push_back(elem);

	begin = GetSignIdx(rawdata, '\"', 3);
	elem.type = AT_STRING;
	GetString(rawdata, buf, begin, me_strlen(rawdata)-1);
	elem.str_value = string(buf);
	paraLst.eleLst.push_back(elem);

	pRet->resultLst.push_back(paraLst);
}

void ME_Extend_ErrReport_Parser(void *pfsm, void *pObj)
{
	//+CEER: <cause>, <report>
	int begin, end;
	char buf[128];
	char *rawdata = ((PMEFSM)pfsm)->ctldata;
	
	ATParamElem elem;
	ATParamLst paraLst;
	ATResult *pRet = (ATResult*)((PMEFSM)pfsm)->req.context;
	
	AddExpectedCmd(pfsm, paraLst);

	begin = GetSignIdx(rawdata, ':', 1);
	end = GetSignIdx(rawdata, ',', 1);
	elem.type = AT_INTEGER;
	elem.int_value = GetInteger(rawdata, begin, end);
	paraLst.eleLst.push_back(elem);

	begin = GetSignIdx(rawdata, '\"', 1);
	if(begin == -1)
	{
		begin = end;
		end = me_strlen(rawdata);
	}
	else
		end = GetSignIdx(rawdata, '\"', 2);
	elem.type = AT_STRING;
	GetString(rawdata, buf, begin, end);
	elem.str_value = string(buf);
	paraLst.eleLst.push_back(elem);

	pRet->resultLst.push_back(paraLst);
}

void ME_Restricted_SIM_Parser(void *pfsm, void *pObj)
{
	//+CRSM: <sw1>,<sw2>[,<response>]
	int begin, end;
	char buf[128];
	char *rawdata = ((PMEFSM)pfsm)->ctldata;
	
	ATParamElem elem;
	ATParamLst paraLst;
	ATResult *pRet = (ATResult*)((PMEFSM)pfsm)->req.context;
	
	AddExpectedCmd(pfsm, paraLst);
	
	begin = GetSignIdx(rawdata, ':', 1);
	end = GetSignIdx(rawdata, ',', 1);
	elem.type = AT_INTEGER;
	elem.int_value = GetInteger(rawdata, begin, end);
	paraLst.eleLst.push_back(elem);

	begin = end;
	end = GetSignIdx(rawdata, ',', 1);
	if(end == -1)
	{
		elem.type = AT_INTEGER;
		elem.int_value = GetInteger(rawdata, begin, me_strlen(rawdata));
		paraLst.eleLst.push_back(elem);
		pRet->resultLst.push_back(paraLst);
		return;
	}
	elem.type = AT_INTEGER;
	elem.int_value = GetInteger(rawdata, begin, end);
	paraLst.eleLst.push_back(elem);

	begin = GetSignIdx(rawdata, '\"', 1);
	end = GetSignIdx(rawdata, '\"', 2);
	if(end - begin == 1)
	{
		buf[0] = 0;
		buf[1] = 1;
	}
	else
		GetString(rawdata, buf, begin, end);
	elem.type = AT_STRING;
	elem.str_value = string(buf);
	paraLst.eleLst.push_back(elem);

	pRet->resultLst.push_back(paraLst);
}

void ME_Get_CBSInfo_Parser(void *pfsm, void *pObj)
{
	//+CSCB: <mode>,<mids>,<dcss>[,<flag>]
	int begin, end;//, length;
	char buf[CTL_DATA_LEN];
	char *rawdata = ((PMEFSM)pfsm)->ctldata;
	
	ATParamElem elem;
	ATParamLst paraLst;
	ATResult *pRet = (ATResult*)((PMEFSM)pfsm)->req.context;
	
	AddExpectedCmd(pfsm, paraLst);
	
	begin = GetSignIdx(rawdata, ':', 1);
	end = GetSignIdx(rawdata, ',', 1);
	elem.type = AT_INTEGER;
	elem.int_value = GetInteger(rawdata, begin, end);
	paraLst.eleLst.push_back(elem);

	begin = GetSignIdx(rawdata, '\"', 1);
	end = GetSignIdx(rawdata, '\"', 2);
	if(end - begin == 1)
	{
		buf[0] = 0;
		buf[1] = 0;
	}
	else
	{
		GetString(rawdata, buf, begin, end);
	}
	elem.type = AT_STRING;
	elem.str_value = string(buf);
	paraLst.eleLst.push_back(elem);

	begin = GetSignIdx(rawdata, '\"', 3);
	end = GetSignIdx(rawdata, '\"', 4);
//	length = end - begin;
	if(end - begin == 1)
	{
		buf[0] = 0;
		buf[1] = 0;
	}
	else
	{
		GetString(rawdata, buf, begin, end);
	}
	elem.type = AT_STRING;
	elem.str_value = string(buf);
	paraLst.eleLst.push_back(elem);

	begin = GetSignIdx(rawdata+end, ',', 1); 
	if(begin != -1) 
	{ 
		elem.type = AT_INTEGER; 
		elem.int_value = GetInteger(rawdata+end, begin, me_strlen(rawdata+end)); 
		paraLst.eleLst.push_back(elem); 
	}
	
	pRet->resultLst.push_back(paraLst);	
}

//response parser
////////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////////////
//urc parser
int ME_URC_ESIMS_Parser(void *pData)
{
	//+ESIMS: <SIM_INSERTED>				  //AT response 
	//+ESIMS: <SIM_INSERTED>,<LOCK_REQUIRED>  //URC

//	int begin, end;
	PMEFSM pfsm = NULL;
//	PATRESULT	pResult = NULL;
//	PSIM_STATE	pURC = NULL;

	pfsm = (PMEFSM)pData;
	if(me_memcmp("+ESIMS", pfsm->ctldata, me_strlen("+ESIMS")) == 0)
	{
		if(GetSignCount(pfsm->ctldata, ',') != 1)
			return 0;  // it is AT response

		if(pfsm->urclist[ID_ESIMS].reply == NULL)
			return 1;  //not register, return directly

 
		ATResult atret;
		// to parse
		pfsm->urclist[ID_ESIMS].reply((void*)&atret);
 

		return 1;
	}

	return 0;
}

int ME_URC_ECSQ_Parser(void *pData)
{
	// +ECSQ: <flag>		 //AT response
	// +ECSQ: <rssi>, <ber>  //URC

	int begin, end;
	PMEFSM pfsm = NULL;
	
	
	pfsm = (PMEFSM)pData;
	if(me_memcmp("+ECSQ", pfsm->ctldata, me_strlen("+ECSQ")) == 0)
	{
		if(GetSignCount(pfsm->ctldata, ',') == 0)
			return 0;  // it is AT response

		if(pfsm->urclist[ID_ECSQ].reply == NULL)
			return 1;
 
		ATResult atret;
		ATParamElem elem;
		ATParamLst paraLst;
		char *rawdata = ((PMEFSM)pfsm)->ctldata;

		elem.type = AT_STRING;
		elem.str_value = string("+ECSQ");
		paraLst.eleLst.push_back(elem);

		begin = GetSignIdx(rawdata, ':', 1);
		end	  = GetSignIdx(rawdata, ',', 1);
		elem.type = AT_INTEGER;
		elem.int_value = GetInteger(rawdata, begin, end);
		paraLst.eleLst.push_back(elem);

		begin = end;
		elem.type = AT_INTEGER;
		if(GetSignCount(rawdata, ',') > 1)
			end = GetSignIdx(rawdata, ',', 2);
		else 
			end = me_strlen(rawdata);
		elem.int_value = GetInteger(rawdata, begin, end);
		paraLst.eleLst.push_back(elem);

		atret.resultLst.push_back(paraLst);
		pfsm->urclist[ID_ECSQ].reply((void*)&atret);
 
		return 1;
	}

	return 0;
}

int ME_URC_Network_Parser(int id, void *pData)
{
	//+CREG: <n>,<stat>[,<lac>,<ci>[,<act>]]	//AT response
	//+CREG: <stat>[,<lac>,<ci>[,<act>]]		//URC
	
	//+CGREG: <n>,<stat>[,<lac>,<ci>[,<act>]]   //AT response
	//+CGREG: <stat>[,<lac>,<ci>[,<act>]]		//URC

	int count, begin, end;	
	PMEFSM pfsm = NULL;

	char urc[8];

	if(id == ID_CREG)
		me_strcpy(urc, "+CREG");
	else
		me_strcpy(urc, "+CGREG");

	pfsm = (PMEFSM)pData;
	if(me_memcmp(urc, pfsm->ctldata, me_strlen(urc)) == 0)
	{
		count = GetSignCount(pfsm->ctldata, ',');
		if((count == 1)||(count == 4))  // it is AT response
			return 0;  
		
		if(count == 3)
		{
			begin = GetSignIdx(pfsm->ctldata, ',', 3);
			count = GetSignCount(pfsm->ctldata+begin, '\"');
			if(count > 1) //<ci> is a string parameter, it has quotation marks.
				return 0; // if <ci> is fourth parameter, it is AT response.
		}
		
		if(pfsm->urclist[id].reply == NULL)
			return 1;
 
		char buf[80];
		ATResult atret;
		ATParamElem elem;
		ATParamLst paraLst;
		char *rawdata = ((PMEFSM)pfsm)->ctldata;
		
		elem.type = AT_STRING;
		if(id == ID_CREG)
			elem.str_value = string("+CREG");
		else
			elem.str_value = string("+CGREG");
		paraLst.eleLst.push_back(elem);
		
		begin = GetSignIdx(pfsm->ctldata, ':', 1);
		end	  = GetSignIdx(rawdata, ',', 1);
		if(end == -1)
			end = me_strlen(rawdata);
		elem.type = AT_INTEGER;
		elem.int_value = GetInteger(rawdata, begin, end);
		paraLst.eleLst.push_back(elem);

		begin = GetSignIdx(rawdata, '\"', 1);
		if(begin != -1)
		{
			elem.type = AT_STRING;
			GetString(rawdata, buf, begin,  GetSignIdx(rawdata, '\"', 2));	
			elem.str_value = string(buf);
			paraLst.eleLst.push_back(elem);

			begin = GetSignIdx(rawdata, '\"', 3);
			if(begin != -1)
			{
				elem.type = AT_STRING;
				GetString(rawdata, buf, begin,  GetSignIdx(rawdata, '\"', 4));	
				elem.str_value = string(buf);
				paraLst.eleLst.push_back(elem);
			}
		}
		
		begin = GetSignIdx(rawdata, ',', 3);
		if(begin != -1)
		{
			elem.type = AT_INTEGER;
			elem.int_value = GetInteger(rawdata, begin, me_strlen(rawdata));
			paraLst.eleLst.push_back(elem);
		}

		atret.resultLst.push_back(paraLst);
		pfsm->urclist[id].reply((void*)&atret);
 
		return 1;
	}

	return 0;
}

int ME_URC_CREG_Parser(void *pData)
{
	return ME_URC_Network_Parser(ID_CREG, pData);
}

int ME_URC_CGREG_Parser(void *pData)
{
	return ME_URC_Network_Parser(ID_CGREG, pData);
}

int ME_URC_CGEV_Parser(void *pData)
{
	/**********************************  
	+CGEV: REJECT <PDP_type>, <PDP_addr>
	+CGEV: NW REACT <PDP_type>, <PDP_addr>, [<cid>]
	+CGEV: NW DEACT <PDP_type>, <PDP_addr>, [<cid>]
	+CGEV: ME DEACT <PDP_type>, <PDP_addr>, [<cid>]
	+CGEV: NW DETACH
	+CGEV: ME DETACH
	+CGEV: NW CLASS <class>
	+CGEV: ME CLASS <class>
	**********************************/
	PMEFSM pfsm = NULL;
	
	char *rawdata;
	
	pfsm = (PMEFSM)pData;
	rawdata = pfsm->ctldata;

	if(me_memcmp("+CGEV", rawdata, me_strlen("+CGEV")) == 0)
	{
		if(pfsm->urclist[ID_CGEV].reply == NULL)
			return 1;
 
		ATResult atret;
		//to parse
		pfsm->urclist[ID_CGEV].reply((void*)&atret);
 
		return 1;
	}

	return 0;
}

int ME_URC_EIND_Parser(void *pData)
{
	//+EIND: <flag>

	int begin, end;
	PMEFSM pfsm = NULL;
	
	pfsm = (PMEFSM)pData;

	if(me_memcmp("+EIND", pfsm->ctldata, me_strlen("+EIND")) == 0)
	{
		if(pfsm->req.expectedCmd[0] != 0)
		{
			if(!me_memcmp(pfsm->req.expectedCmd, pfsm->ctldata, me_strlen(pfsm->req.expectedCmd)))
				return 0; //maybe it's AT+EIND? response.
		}

		if(pfsm->urclist[ID_EIND].reply == NULL)
			return 1;

 
		ATResult atret;
		ATParamElem elem;
		ATParamLst paraLst;
		char *rawdata = ((PMEFSM)pfsm)->ctldata;

		elem.type = AT_STRING;
		elem.str_value = string("+EIND");
		paraLst.eleLst.push_back(elem);

		begin = GetSignIdx(rawdata, ':', 1);
		elem.type = AT_INTEGER;
		if(GetSignCount(rawdata, ',') > 0)
			end = GetSignIdx(rawdata, ',', 1);
		else 
			end = me_strlen(rawdata);
		elem.int_value = GetInteger(rawdata, begin, end);
		paraLst.eleLst.push_back(elem);
		
		atret.resultLst.push_back(paraLst);
		pfsm->urclist[ID_EIND].reply((void*)&atret);
 
		return 1;
	}

	return 0;
}


int ME_URC_CIEV_Parser(void *pData)
{ 
	//+CIEV: <ind>,<value1>[,<value2>,бн]
    //ind is a integer
	//value is string

	int begin, end;
	PMEFSM pfsm = NULL;
	
	pfsm = (PMEFSM)pData;
	if(me_memcmp("+CIEV", pfsm->ctldata, me_strlen("+CIEV")) == 0)
	{
		if(pfsm->urclist[ID_CIEV].reply == NULL)
			return 1;
 
		char buf[80];
		ATResult atret;
		ATParamElem elem;
		ATParamLst paraLst;
		char *rawdata = ((PMEFSM)pfsm)->ctldata;
		
		elem.type = AT_STRING;
		elem.str_value = string("+CIEV");
		paraLst.eleLst.push_back(elem);


		begin = GetSignIdx(rawdata, ':', 1);
		end = GetSignIdx(rawdata, ',', 1); 
		elem.type = AT_INTEGER;
		elem.int_value = GetInteger(rawdata, begin, end);
		paraLst.eleLst.push_back(elem);
	
		bool finish=false;
		for(int i=1; i<20; i++)  //20 is a guard value, prevent unexpected infinite loop
		{
			begin = GetSignIdx(rawdata, ',', i);
			end = GetSignIdx(rawdata, ',', i+1); 
			if(end == -1)
			{
				finish = true;
				end = me_strlen(rawdata);
			}

			GetString(rawdata, buf, begin, me_strlen(rawdata));
			elem.type = AT_STRING;
			elem.str_value = string(buf);
			paraLst.eleLst.push_back(elem);

			if(finish)
				break;
		}

		atret.resultLst.push_back(paraLst);
		pfsm->urclist[ID_CIEV].reply((void*)&atret);	
 
		return 1;
	}

	return 0;
}

int ME_URC_CMTI_Parser(void *pData)
{
	//+CMTI: <mem>,<index>
	
	int begin, end;
	PMEFSM pfsm = NULL;
	
	pfsm = (PMEFSM)pData;
	if(me_memcmp("+CMTI", pfsm->ctldata, me_strlen("+CMTI")) == 0)
	{
		if(pfsm->urclist[ID_CMTI].reply == NULL)
			return 1;
 
		char buf[80];
		ATResult atret;
		ATParamElem elem;
		ATParamLst paraLst;
		char *rawdata = ((PMEFSM)pfsm)->ctldata;
		
		elem.type = AT_STRING;
		elem.str_value = string("+CMTI");
		paraLst.eleLst.push_back(elem);


		begin = GetSignIdx(rawdata, '\"', 1);
		end = GetSignIdx(rawdata, '\"', 2);
		GetString(rawdata, buf, begin, end);
		elem.type = AT_STRING;
		elem.str_value = string(buf);
		paraLst.eleLst.push_back(elem);
		
		begin = GetSignIdx(rawdata, ',', 1);
		elem.type = AT_INTEGER;
		elem.int_value  = GetInteger(rawdata, begin, me_strlen(rawdata));
		paraLst.eleLst.push_back(elem);

		atret.resultLst.push_back(paraLst);
		pfsm->urclist[ID_CMTI].reply((void*)&atret);	
 
		return 1;
	}

	return 0;
}

int ME_URC_SMS_Parser(int id, void *pData)
{
	//+CMT: <length><CR><LF><pdu>
	//+CDS: <length><CR><LF><pdu>
	//+CBM: <length><CR><LF><pdu>
	
	int begin;
	PMEFSM pfsm = NULL;
	PATRESULT	pResult = NULL;
	PMSG_CONTENT	pURC = NULL;
	char urc[8];
	
	if(id == ID_CDS)
		me_strcpy(urc, "+CDS");
	else if(id == ID_CMT)
		me_strcpy(urc, "+CMT");
	else
		me_strcpy(urc, "+CBM");
	
	pfsm = (PMEFSM)pData;

	if(pfsm->nUrcState == FSM_STATE_START)
	{
		if(me_memcmp(urc, pfsm->ctldata, me_strlen(urc)) == 0)
		{
			if(pfsm->urclist[id].reply != NULL)
			{
				pResult = (PATRESULT)me_malloc(sizeof(ATRESULT));
				pURC = (PMSG_CONTENT)me_malloc(sizeof(MSG_CONTENT));
				pfsm->pUrcObj = pResult;
				pResult->pRet = pURC;	
				
				begin = GetSignIdx(pfsm->ctldata, ':', 1);
				pURC->length = GetInteger(pfsm->ctldata, begin, me_strlen(pfsm->ctldata));
			}

			pfsm->nUrcState = FSM_STATE_STEP1;
			pfsm->bReadPdu = TRUE;
			pfsm->nUrcId = id;

			return 1;
		}
	}
	else if(pfsm->nUrcState == FSM_STATE_STEP1)
	{
		if(pfsm->nUrcId == id)
		{
			if(pfsm->urclist[id].reply != NULL)
			{
				pResult = (PATRESULT)pfsm->pUrcObj;
				pURC = (PMSG_CONTENT)pResult->pRet;
				me_strcpy(pURC->pdu, pfsm->ctldata);				
 
				ATResult atret;
				ATParamElem elem;
				ATParamLst paraLst;
					
				elem.type = AT_STRING;
				if(id == ID_CDS)
					elem.str_value = string("+CDS");
				else if(id == ID_CMT)
					elem.str_value = string("+CMT");
				else
					elem.str_value = string("+CBM");

				paraLst.eleLst.push_back(elem);
				
				elem.type = AT_INTEGER;
				elem.int_value = pURC->length;
				paraLst.eleLst.push_back(elem);

				elem.type = AT_STRING;
				elem.str_value = string(pURC->pdu);
				paraLst.eleLst.push_back(elem);				
				
				atret.resultLst.push_back(paraLst);				
				pfsm->urclist[id].reply((void*)&atret);	
 
				me_free(pURC);
				me_free(pResult);
			}

			pfsm->bReadPdu = FALSE;
			pfsm->nUrcState = FSM_STATE_START;
			pfsm->pUrcObj = NULL;
			pfsm->nUrcId = -1;
			return 1;
		}
	}

	return 0;
}

int ME_URC_CMT_Parser(void *pData)
{
	return ME_URC_SMS_Parser(ID_CMT, pData);
}

int ME_URC_CDS_Parser(void *pData)
{
	return ME_URC_SMS_Parser(ID_CDS, pData);	
}

int ME_URC_CBM_Parser(void *pData)
{
	return ME_URC_SMS_Parser(ID_CBM, pData);	
}

int ME_URC_RING_Parser(void *pData)
{
	PMEFSM pfsm = (PMEFSM)pData;

	if(me_memcmp("RING", pfsm->ctldata, me_strlen("RING")) == 0)
	{
		if(pfsm->urclist[ID_RING].reply == NULL)
			return 1;
	
		ATResult atret;
		ATParamElem elem;
		ATParamLst paraLst;
		
		elem.type = AT_STRING;
		elem.str_value = string("RING");
		paraLst.eleLst.push_back(elem);

		atret.resultLst.push_back(paraLst);				
		pfsm->urclist[ID_RING].reply((void*)&atret);	

		return 1;
	}
	
	return 0;	
}

int ME_URC_NOCA_Parser(void *pData)
{
	PMEFSM pfsm = (PMEFSM)pData;
	
	if(me_memcmp("NO CARRIER", pfsm->ctldata, me_strlen("NO CARRIER")) == 0)
	{
		if(pfsm->urclist[ID_NOCA].reply == NULL)
			return 1;

		ATResult atret;
		ATParamElem elem;
		ATParamLst paraLst;
		
		elem.type = AT_STRING;
		elem.str_value = string("NO CARRIER");
		paraLst.eleLst.push_back(elem);
		
		atret.resultLst.push_back(paraLst);				
		pfsm->urclist[ID_NOCA].reply((void*)&atret);	

		return 1;
	}
	
	return 0;	
}

int ME_URC_CSSI_Parser(void *pData)
{
	//+CSSI: <code1> [,<index>]

	int begin, end;
	PMEFSM pfsm = (PMEFSM)pData;
	char *rawdata = pfsm->ctldata;
	
	if(me_memcmp("+CSSI", rawdata, me_strlen("+CSSI")) == 0)
	{
		if(pfsm->urclist[ID_CSSI].reply == NULL)
			return 1;

		ATResult atret;
		ATParamElem elem;
		ATParamLst paraLst;

		elem.type = AT_STRING;
		elem.str_value = string("+CSSI");
		paraLst.eleLst.push_back(elem);	
		
		begin = GetSignIdx(rawdata, ':', 1);
		end   = GetSignIdx(rawdata, ',', 1);
		if(end == -1)
			end = me_strlen(rawdata);
		elem.type = AT_INTEGER;
		elem.int_value = GetInteger(rawdata, begin, end);
		paraLst.eleLst.push_back(elem);

		begin = GetSignIdx(rawdata, ',', 1);
		if(begin != -1)
		{
			elem.type = AT_INTEGER;
			elem.int_value = GetInteger(rawdata, begin, me_strlen(rawdata));
			paraLst.eleLst.push_back(elem);
		}

		atret.resultLst.push_back(paraLst);				
		pfsm->urclist[ID_CSSI].reply((void*)&atret);	
	
		return 1;
	}

	return 0;	
}

int ME_URC_CSSU_Parser(void *pData)
{
	//+CSSU: <code2>[,<index>[,<number>,<type>[,<subaddr>,<satype>]]]
	int begin, end;
	PMEFSM pfsm = (PMEFSM)pData;
	char *rawdata = pfsm->ctldata;
	
	if(me_memcmp("+CSSU", rawdata, me_strlen("+CSSU")) == 0)
	{
		if(pfsm->urclist[ID_CSSU].reply == NULL)
			return 1;

		char buf[128];
		ATResult atret;
		ATParamElem elem;
		ATParamLst paraLst;
		
		elem.type = AT_STRING;
		elem.str_value = string("+CSSU");
		paraLst.eleLst.push_back(elem);	

		begin = GetSignIdx(rawdata, ':', 1);
		end   = GetSignIdx(rawdata, ',', 1);
		if(end == -1)
			end = me_strlen(rawdata);
		elem.type = AT_INTEGER;
		elem.int_value = GetInteger(rawdata, begin, end);
		paraLst.eleLst.push_back(elem);
		
		begin = GetSignIdx(rawdata, ',', 1);
		if(begin != -1)
		{
			end = GetSignIdx(rawdata, ',', 2);
			if(end == -1)
				end = me_strlen(rawdata);
			elem.type = AT_INTEGER;
			elem.int_value = GetInteger(rawdata, begin, end);
			paraLst.eleLst.push_back(elem);
		}

		begin = GetSignIdx(rawdata, '\"', 1);
		if(begin != -1)
		{
			end = GetSignIdx(rawdata, '\"', 2);
			elem.type = AT_STRING;
			GetString(rawdata, buf, begin, end);
			elem.str_value = string(buf);
			paraLst.eleLst.push_back(elem);			
		}

		begin = GetSignIdx(rawdata, ',', 3);
		if(begin != -1)
		{
			end = GetSignIdx(rawdata, ',', 4);
			if(end == -1)
				end = me_strlen(rawdata);
			elem.type = AT_INTEGER;
			elem.int_value = GetInteger(rawdata, begin, end);
			paraLst.eleLst.push_back(elem);				
		}

		atret.resultLst.push_back(paraLst);				
		pfsm->urclist[ID_CSSU].reply((void*)&atret);

		return 1;	
	}

	return 0;	
}

int ME_URC_CUSD_Parser(void *pData)
{
	// +CUSD: <m>[,<str>,<dcs>]

	int begin, end;
	PMEFSM pfsm = (PMEFSM)pData;
	char *rawdata = pfsm->ctldata;

	if(me_memcmp("+CUSD", rawdata, me_strlen("+CUSD")) == 0)
	{
		if(pfsm->urclist[ID_CUSD].reply == NULL)
			return 1;

		char buf[CTL_DATA_LEN];
		ATResult atret;
		ATParamElem elem;
		ATParamLst paraLst;
		
		elem.type = AT_STRING;
		elem.str_value = string("+CUSD");
		paraLst.eleLst.push_back(elem);	

		begin = GetSignIdx(rawdata, ':', 1);
		end   = GetSignIdx(rawdata, ',', 1);
		if(end == -1)
			end = me_strlen(rawdata);
		elem.type = AT_INTEGER;
		elem.int_value = GetInteger(rawdata, begin, end);
		paraLst.eleLst.push_back(elem);

		begin = GetSignIdx(rawdata, '\"', 1);
		if(begin != -1)
		{
			end = GetSignIdx(rawdata, '\"', 2);
			GetString(rawdata, buf, begin, end);
			elem.type = AT_STRING;
			elem.str_value = string(buf);
			paraLst.eleLst.push_back(elem);

			rawdata += end;
			begin = GetSignIdx(rawdata, ',', 1);
			elem.type = AT_INTEGER;
			elem.int_value = GetInteger(rawdata, begin, me_strlen(rawdata));
			paraLst.eleLst.push_back(elem);
		}

		atret.resultLst.push_back(paraLst);				
		pfsm->urclist[ID_CUSD].reply((void*)&atret);	
		
		return 1;
	}

	return 0;	
}

int ME_URC_CCCM_Parser(void *pData)
{
	//+CCCM: <ccm>
	
	PMEFSM pfsm = (PMEFSM)pData;
	char *rawdata = pfsm->ctldata;

	if(me_memcmp("+CCCM", rawdata, me_strlen("+CCCM")) == 0)
	{
		if(pfsm->urclist[ID_CCCM].reply == NULL)
			return 1;

		char buf[128];
		ATResult atret;
		ATParamElem elem;
		ATParamLst paraLst;
		
		elem.type = AT_STRING;
		elem.str_value = string("+CCCM");
		paraLst.eleLst.push_back(elem);
		
		elem.type = AT_STRING;
		GetString(rawdata, buf, GetSignIdx(rawdata, '\"',1), GetSignIdx(rawdata, '\"',2));
		elem.str_value = string(buf);
		paraLst.eleLst.push_back(elem);
		
		atret.resultLst.push_back(paraLst);				
		pfsm->urclist[ID_CCCM].reply((void*)&atret);	
		
		return 1;
	}
	
	return 0;
}

int ME_URC_CCWV_Parser(void *pData)
{
	PMEFSM pfsm = (PMEFSM)pData;
	
	if(me_memcmp("+CCWV", pfsm->ctldata, me_strlen("+CCWV")) == 0)
	{
		if(pfsm->urclist[ID_CCWV].reply == NULL)
			return 1;

		ATResult atret;
		ATParamElem elem;
		ATParamLst paraLst;
		
		elem.type = AT_STRING;
		elem.str_value = string("+CCWV");
		paraLst.eleLst.push_back(elem);
		
		atret.resultLst.push_back(paraLst);				
		pfsm->urclist[ID_CCWV].reply((void*)&atret);	
		
		return 1;
	}
	
	return 0;	
}

int ME_URC_CCWA_Parser(void *pData)
{
	PMEFSM pfsm = (PMEFSM)pData;
	
	if(me_memcmp("+CCWA", pfsm->ctldata, me_strlen("+CCWA")) == 0)
	{
		if(GetSignCount(pfsm->ctldata, ',') != 2)
			return 0; //AT response
		
		if(pfsm->urclist[ID_CCWA].reply == NULL)
			return 1;

		ATResult atret;
		ATParamElem elem;
		ATParamLst paraLst;
		
		elem.type = AT_STRING;
		elem.str_value = string("+CCWA");
		paraLst.eleLst.push_back(elem);
		
		atret.resultLst.push_back(paraLst);
		pfsm->urclist[ID_CCWA].reply((void*)&atret);
		
		return 1;
	}

	return 0;	
}

int ME_URC_STKPCI_Parser(void *pData)
{
	int begin, end;
	PMEFSM pfsm = (PMEFSM)pData;
	char *rawdata = pfsm->ctldata;
	
	if(me_memcmp("+STKPCI", pfsm->ctldata, me_strlen("+STKPCI")) == 0)
	{
		if(pfsm->urclist[ID_STKPCI].reply == NULL)
			return 1;

		ATResult atret;
		ATParamElem elem;
		ATParamLst paraLst;
		char buf[CTL_DATA_LEN];
		
		elem.type = AT_STRING;
		elem.str_value = string("+STKPCI");
		paraLst.eleLst.push_back(elem);

		begin = GetSignIdx(rawdata, ':', 1);
		end   = GetSignIdx(rawdata, ',', 1);
		elem.type = AT_INTEGER;
		elem.int_value = GetInteger(rawdata, begin, end);
		paraLst.eleLst.push_back(elem);

		begin = GetSignIdx(rawdata, '\"', 1);
		end   = GetSignIdx(rawdata, '\"', 2);

		if (begin >= 0 && end > begin)
		{
			elem.type = AT_STRING;
			GetString(rawdata, buf, begin ,end);
			elem.str_value = string(buf);
			paraLst.eleLst.push_back(elem);
		}
		
		atret.resultLst.push_back(paraLst);				
		pfsm->urclist[ID_STKPCI].reply((void*)&atret);	
		
		return 1;
	}
	
	return 0;	
}

int ME_URC_PACSP_Parser(void *pData)
{
	return 0;	
}

int ME_URC_ETWS_Parser(void *pData)
{
	//+ETWS: <msgid>,<plmnid>,<num>,<type>[,<sec>]
	int begin, end;
	PMEFSM pfsm = (PMEFSM)pData;
	char *rawdata = pfsm->ctldata;
	
	if(me_memcmp("+ETWS", rawdata, me_strlen("+ETWS")) == 0)
	{
		if(GetSignCount(pfsm->ctldata, ',') < 3) //AT response has one parameter
			return 0;
			
		if(pfsm->urclist[ID_ETWS].reply == NULL)
			return 1;

		char buf[CTL_DATA_LEN];
		ATResult atret;
		ATParamElem elem;
		ATParamLst paraLst;
		
		elem.type = AT_STRING;
		elem.str_value = string("+ETWS");
		paraLst.eleLst.push_back(elem);	

		begin = GetSignIdx(rawdata, ':', 1);
		end   = GetSignIdx(rawdata, ',', 1);
		elem.type = AT_INTEGER;
		elem.int_value = GetInteger(rawdata, begin, end);
		paraLst.eleLst.push_back(elem);

		begin = end;
		end = GetSignIdx(rawdata, ',', 2);
		elem.type = AT_INTEGER;
		elem.int_value = GetInteger(rawdata, begin, end);
		paraLst.eleLst.push_back(elem);

		begin = end;
		end = GetSignIdx(rawdata, ',', 3);
		elem.type = AT_INTEGER;
		elem.int_value = GetInteger(rawdata, begin, end);
		paraLst.eleLst.push_back(elem);
		
		begin = GetSignIdx(rawdata, '\"', 1);
		end = GetSignIdx(rawdata, '\"', 2);
		elem.type = AT_STRING;
		GetString(rawdata, buf, begin ,end);
		elem.str_value = string(buf);
		paraLst.eleLst.push_back(elem);

		begin = GetSignIdx(rawdata, '\"', 3);
		if(begin != -1)
		{
			elem.type = AT_STRING;
			GetString(rawdata, buf, begin, GetSignIdx(rawdata, '\"',4));
			elem.str_value = string(buf);
			paraLst.eleLst.push_back(elem);
		}
		
		atret.resultLst.push_back(paraLst);				
		pfsm->urclist[ID_ETWS].reply((void*)&atret);	

		return 1;
	}

	return 0;
}

int ME_URC_PSBEARER_Parser(void* pData)
{
	//+PSBEARER: <cell_data_speed_support>, <max_data_bearer_capability>

	int begin, end;
	PMEFSM pfsm = (PMEFSM)pData;
	char *rawdata = pfsm->ctldata;
	
	if(me_memcmp("+PSBEARER", pfsm->ctldata, me_strlen("+PSBEARER")) == 0)
	{
		if(pfsm->urclist[ID_PSBEARER].reply == NULL)
			return 1;
		
		ATResult atret;
		ATParamElem elem;
		ATParamLst paraLst;
		
		elem.type = AT_STRING;
		elem.str_value = string("+PSBEARER");
		paraLst.eleLst.push_back(elem);

		begin = GetSignIdx(rawdata, ':', 1);
		end   = GetSignIdx(rawdata, ',', 1);
		elem.type = AT_INTEGER;
		elem.int_value = GetInteger(rawdata, begin, end);
		paraLst.eleLst.push_back(elem);
		
		elem.type = AT_INTEGER;
		elem.int_value = GetInteger(rawdata, end, me_strlen(rawdata));
		paraLst.eleLst.push_back(elem);

		atret.resultLst.push_back(paraLst);
		pfsm->urclist[ID_PSBEARER].reply((void*)&atret);
		
		return 1;
	}

	return 0;
}

//urc parser
////////////////////////////////////////////////////////////////////////

