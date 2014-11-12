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
#include "me_func_cb.h"
#include "me_parser.h"
#include "ATResult.h"
#include "glbdefs.h"

typedef enum rsp_type
{
	RSP_OK,
	RSP_ERROR,
	RSP_CMEERROR,
	RSP_CMSERROR,
	RSP_CONNECT
	
}RSPTYPE;

typedef enum rsp_parser_state
{
	PARSER_DOING,
	PARSER_DONE,
	PARSER_EXIT

}RSPPARSERSTATE;

const char *RSP_String[] = 
{
	"OK",
	"ERROR",
	"+CME ERROR:",
	"+CMS ERROR:",
	"CONNECT",
	NULL
};

URCTABLE urc_proc_t[] = 
{
	{ID_ECSQ,		ME_URC_ECSQ_Parser},
	{ID_CREG,		ME_URC_CREG_Parser},
	{ID_CGREG,		ME_URC_CGREG_Parser},
	{ID_PSBEARER,	ME_URC_PSBEARER_Parser},
	{ID_CGEV,		ME_URC_CGEV_Parser},
	{ID_EIND,		ME_URC_EIND_Parser},
	{ID_CMTI,		ME_URC_CMTI_Parser},
	{ID_RING,		ME_URC_RING_Parser},
	{ID_NOCA,		ME_URC_NOCA_Parser},
	{ID_CSSI,		ME_URC_CSSI_Parser},
	{ID_CSSU,		ME_URC_CSSU_Parser},
	{ID_CUSD,		ME_URC_CUSD_Parser},
	{ID_CCCM,		ME_URC_CCCM_Parser},
	{ID_CCWV,		ME_URC_CCWV_Parser},
	{ID_CCWA,		ME_URC_CCWA_Parser},
	{ID_CIEV,       ME_URC_CIEV_Parser},
//	{ID_PACSP,		ME_URC_PACSP_Parser},
	{ID_CMT,		ME_URC_CMT_Parser},
	{ID_CDS,		ME_URC_CDS_Parser},
	{ID_ETWS,		ME_URC_ETWS_Parser},
	{ID_CBM,		ME_URC_CBM_Parser},
	{ID_ESIMS,		ME_URC_ESIMS_Parser},
	{ID_STKPCI,		ME_URC_STKPCI_Parser},

	{ID_MAX, NULL}
};

int ME_URC_Parser(void *pData)
{
	PMEFSM pfsm = (PMEFSM)pData;
	int i = 0;
	
	if(pfsm == NULL)
		return 0;
	
	while(urc_proc_t[i].proc!= NULL)
	{
		if(urc_proc_t[i++].proc(pfsm) != 0)
			return 1;
	}
	
	return 0;
}

static int IsFinalResultCode(char* buf, unsigned int buflen)
{
	int i = 0;

	if ( buflen == 0 )
		return -1;
	
	while (RSP_String[i] != 0)
	{
		if (buflen < me_strlen(RSP_String[i]))
		{
			i++;
			continue;
		}
		
		if (me_memcmp(RSP_String[i], buf, me_strlen(RSP_String[i])) == 0 )
			return i;
		
		i++;
	}
	
	return -1;
}

static int IsExpectedCmd(PMEFSM pfsm)
{
	if(pfsm->req.expectedCmd[0] == 0)
		return 0;

	return me_memcmp(pfsm->req.expectedCmd, pfsm->ctldata, me_strlen(pfsm->req.expectedCmd));
}

static int GetErrorCode(char* buf, unsigned int buflen)
{
	int retval = IsFinalResultCode(buf, buflen);

	switch(retval)
	{
	case RSP_OK:
		retval = ER_OK;
		break;
	case RSP_ERROR:
		retval = ER_UNKNOWN;
		break;
	case RSP_CMEERROR:
		retval = ER_CMEERROR;
		break;
	case RSP_CMSERROR:
		retval = ER_CMEERROR;
		break;
	case RSP_CONNECT:
		retval = ER_OK;
		break;
	}
	
	return retval;
}

static void *CreateRspObj(PMEFSM pfsm, int size)
{
	PATRESULT pResult = NULL;

	if(pfsm->req.nFsmState == FSM_STATE_DONE)
		return pResult;

	if(pfsm->pRspObj == NULL)
	{
		pfsm->pRspObj = me_malloc(sizeof(ATRESULT));
		pResult = (PATRESULT)pfsm->pRspObj;
		pResult->pRet = NULL;	
		if(size != 0)
		{
			pResult->pRet = me_malloc(size);
			me_memset(pResult->pRet, 0, size);
		}
	}

	return pfsm->pRspObj;
}

static void DestroyRspObj(PATRESULT result)
{
	if(result != NULL)
	{
		if(result->pRet != NULL)
			me_free(result->pRet);	
		me_free(result);
	}
}

int GetCMEErrorNum(int err, PMEFSM pfsm)
{
	if(err == ER_CMEERROR)
	{
		int begin;
		begin = GetSignIdx(pfsm->ctldata, ':', 1);
		return GetInteger(pfsm->ctldata, begin, me_strlen(pfsm->ctldata));
	}
	
	return err;
}

static int AnalyingRespString(PMEFSM pfsm, int *errcode)
{
	int retval;

	if(pfsm->req.nFsmState == FSM_STATE_DONE)
		return PARSER_EXIT;

	switch(pfsm->req.event)
	{
		case AT_RESPONSE:
			{
				retval = GetErrorCode(pfsm->ctldata, sizeof(pfsm->ctldata));
				if(retval < 0)
				{
					if(!IsExpectedCmd(pfsm))
					{
						//parse data
						return PARSER_DOING;
					}
					else
					{
						//URC or other error response
						return PARSER_EXIT;
					}	
				}
				else
				{
					//final result code (e.g. OK, ERROR, +CME ERROR)
					*errcode = GetCMEErrorNum(retval, pfsm);
				}
			}
			break;
		case AT_TIMEOUT:
			{
				ME_Print("\r\n[error] time out\r\n");
				*errcode = ER_TIMEOUT;
			}
			break;
		case AT_WRITEFAIL:
			{
				ME_Print("\r\n[error] failed to write\r\n");
				*errcode = ER_UNKNOWN;
			}
			break;
		case AT_CANCEL:
			{
				ME_Print("\r\n[error] cancel operation\r\n");
				*errcode = ER_USERABORT;
			}
			break;
	}

	ATResult *pRet = (ATResult*)((PMEFSM)pfsm)->req.context;
	pRet->retType = *errcode;

	return PARSER_DONE;
}

void ME_Singleline_Response_Parser(void *pData, int size, PARSERFUNC func)
{
	PMEFSM pfsm = NULL;
	int errcode;
	
	pfsm = (PMEFSM)pData;	

	me_lock(&pfsm->Lock);	

	if(pfsm->req.nFsmState != FSM_STATE_DONE)
	{
		switch(AnalyingRespString(pfsm, &errcode))
		{
		case PARSER_DOING:
			{
				if(func != NULL)
				{
					func(pfsm, NULL);
				}
                                //notice there is NO break
			}
		case PARSER_EXIT: 
			{
				me_unlock(&pfsm->Lock);
				return;
			}
		case PARSER_DONE:
			{
				pfsm->req.nFsmState = FSM_STATE_DONE;
				me_killtimer(&pfsm->Timer, pfsm->req.event);				
				break;
			}
		}//end switch
		
	}
	
	if(pfsm->bWriteDone)
	{
//		me_lock(&pfsm->LockSending); 
		pfsm->req.atproc = NULL;	
//		me_unlock(&pfsm->LockSending); 

		pfsm->req.reply(pData);
	}

	me_unlock(&pfsm->Lock);
}

void ME_Multiline_Response_Parser(void *pData, int size, PARSERFUNC func)
{
	ME_Singleline_Response_Parser(pData, size, func);
}

//AT+CGREG?
void ME_Query_GPRSState_CB(void *pData)
{
	ME_Query_NetwrokReg_CB(pData);
}

//AT+CFUN?
void ME_Query_ModemFunctionality_CB(void *pData)
{
	ME_Query_ModemBearer_CB(pData);
}

//AT+ECSQ?
void ME_Query_SignalLevel_CB(void *pData)
{
	ME_Query_ModemBearer_CB(pData);
}

//AT+CGATT?
void ME_Query_PSState_CB(void *pData)
{
	ME_Query_ModemBearer_CB(pData);
}

//AT+EIND?
void ME_Query_Indication_CB(void *pData)
{
	ME_Query_ModemBearer_CB(pData);
}

//AT+EIND
void ME_Get_IndicationCtrl_CB(void *pData)
{
	ME_Query_ModemBearer_CB(pData);
}

//AT+CMUT?
void ME_Get_MuteCtrl_CB(void *pData)
{
	ME_Query_ModemBearer_CB(pData);
}

//AT+CVIB?
void ME_Get_VibratorMode_CB(void *pData)
{
	ME_Query_ModemBearer_CB(pData);
}

//AT+CRSL?
void ME_Get_RingLevel_CB(void *pData)
{
	ME_Query_ModemBearer_CB(pData);
}

//AT+CLVL?
void ME_Get_SpeakerVolume_CB(void *pData)
{
	ME_Query_ModemBearer_CB(pData);
}

//AT+ETWS?
void ME_Get_ETWSInfo_CB(void *pData)
{
	ME_Query_ModemBearer_CB(pData);
}

///////////////////////////////return OK/ERROR directly/////////////////////////////
/*
AT
ATH
ATE0Q0V1
AT+CMEE=
AT+CREG=
AT+CGREG=
AT+ECSQ=
AT+CFUN=
AT+CPIN=
AT+CPWD=
AT+COPS=
AT+CGDCONT=
AT+CGQREQ=
AT+CGQMIN=
AT+CGEREP=
AT+CGPRCO=
AT+CGDATA=
AT+CPOL=
AT+ESIMS=
AT+CGATT=
AT+CGACT=
AT+ESLN
AT+MBPC=
AT+EGTYPE=
AT+EGTYPE=
AT+CNMI=
AT+CMGD=
AT+CAPL
AT+CMGF=
AT+CPBS=
AT+CSCS=
AT+EIND=
AT+CCLK=
AT+CLIP=
AT+CLIR=
AT+CVIB=
AT+CMUT=
AT+CRSL=
AT+CLVL=
AT+CUSD=
AT+ESWM=
AT+ERAT=
AT+CAOC= 1 or 2
AT+CCWE=
AT+CACM=
AT+CAMM=
AT+VTS=
AT+CTFR=
AT+CHLD=
AT+CCWA= 1 or 2
AT+CCFC= 1 or 2
AT+CSSN=
AT_CSCB=
AT+ETWS=
AT+PSBEARER=
AT+EPIN1=
AT+EPIN2=
*/
void ME_Set_ModemFunc_CB(void *pData)
{
	ME_Singleline_Response_Parser(pData, 0, NULL);
}

void ME_Set_CMER_CB(void *pData)
{
	ME_Singleline_Response_Parser(pData, 0, NULL);
}

///////////////////////////Single line response///////////////////////////////////////////
//AT+CREG?
void ME_Query_NetwrokReg_CB(void *pData)
{
	ME_Singleline_Response_Parser(pData, sizeof(NETREG_INFO), ME_Query_NetwrokReg_Parser);
}

//AT+EPSB?
void ME_Query_ModemBearer_CB(void *pData)
{
	ME_Singleline_Response_Parser(pData, sizeof(INT_MODEMPARA), ME_Query_ModemBearer_Parser);
}

//AT+EPBSE=?
void ME_Query_BandInfo_CB(void *pData)
{
	ME_Singleline_Response_Parser(pData, sizeof(BAND_INFO), ME_Query_BandInfo_Parser);
}

//AT+CGSN
void ME_Query_IMEI_CB(void *pData)
{
	ME_Singleline_Response_Parser(pData, sizeof(STR_MODEMPARA), ME_Query_IMEI_Parser);
}

//AT+CGMI								
void ME_Query_FacInfo_CB(void *pData)
{
	ME_Singleline_Response_Parser(pData, sizeof(STR_MODEMPARA), ME_Common_Query_Parser);
}

//AT+CGMM
void ME_Query_ModelInfo_CB(void *pData)
{
	ME_Singleline_Response_Parser(pData, sizeof(STR_MODEMPARA), ME_Common_Query_Parser);
}

//AT+CGMR
void ME_Query_Revision_CB(void *pData)
{
	ME_Singleline_Response_Parser(pData, sizeof(STR_MODEMPARA), ME_Common_Query_Parser);
}

/************ 20131109 by Zhuwei **********************************************/
//AT+VERNO
void ME_Query_GetVERNO_CB(void *pData)
{
    ME_Singleline_Response_Parser(pData, sizeof(STR_MODEMPARA), ME_Common_Query_Parser);
}

/******************************************************************************/

//AT+CPIN?
void ME_Query_PINState_CB(void *pData)
{
	ME_Singleline_Response_Parser(pData, sizeof(STR_MODEMPARA), ME_Common_Query_Parser);
}

//AT+EPINC?
void ME_Query_PINRemainNum_CB(void *pData)
{
	ME_Singleline_Response_Parser(pData, sizeof(RAMAIN_NUM), ME_Query_PINRemainNum_Parser);
}

//AT+CIMI
void ME_Query_IMSI_CB(void *pData)
{
	ME_Singleline_Response_Parser(pData, sizeof(STR_MODEMPARA), ME_Query_IMSI_Parser);
}

//AT+COPS?
void ME_Query_Operator_CB(void *pData)
{
	ME_Singleline_Response_Parser(pData, sizeof(OPER_SEL), ME_Query_Operator_Parser);
}

//AT+CGPADDR=
void ME_Query_PDPAddr_CB(void *pData)
{
	ME_Singleline_Response_Parser(pData, sizeof(PDP_ADDR), ME_Query_PDPAddr_Parser);
}

//AT+ESPN?
void ME_Query_SIMSPN_CB(void *pData)
{
	ME_Singleline_Response_Parser(pData, sizeof(STR_MODEMPARA), ME_Common_Query_Parser);
}

//AT+ICCID?
void ME_Query_SIMICCID_CB(void *pData)
{
	ME_Singleline_Response_Parser(pData, sizeof(STR_MODEMPARA), ME_Common_Query_Parser);
}

//AT+CSQ
void ME_Query_SignalQuality_CB(void *pData)
{
	ME_Singleline_Response_Parser(pData, sizeof(SIGNAL_QUALITY), ME_Query_SignalQuality_Parser);
}

//AT+CSCA?
void ME_Get_ServiceCenterAddress_CB(void *pData)
{
	ME_Singleline_Response_Parser(pData, sizeof(SMCS_ADDR), ME_Get_ServiceCenterAddress_Parser);
}

//AT+CPMS=
void ME_Get_MessageMemoryState_CB(void *pData)
{
	ME_Singleline_Response_Parser(pData, sizeof(MSG_MEMSTATE), ME_Get_MessageMemoryState_Parser);
}

//AT+CRSM=
void ME_Read_Transparent_SIMFile_CB(void *pData)
{
	ME_Singleline_Response_Parser(pData, sizeof(SIM_FILE), ME_Read_Transparent_SIMFile_Parser);
}

//AT+CPMS?
void ME_Get_CurrentSMSMemType_CB(void *pData)
{
	ME_Singleline_Response_Parser(pData, 0, ME_Get_CurrentSMSMemType_Parser);
}

//AT+EQSI=
void ME_Get_SMSMemIndex_CB(void *pData)
{
	ME_Singleline_Response_Parser(pData, 0, ME_Get_SMSMemIndex_Parser);
}

//AT+CPBS=
void ME_Get_PbMemType_CB(void *pData)
{
	ME_Singleline_Response_Parser(pData, 0, ME_Get_PbMemType_Parser);
}

//AT+CPBS?
void ME_Get_PbMemState_CB(void *pData)
{
	ME_Singleline_Response_Parser(pData, 0, ME_Get_PbMemState_Parser);
}

//AT+CPBR=?
void ME_Get_PbMemInfo_CB(void *pData)
{
	ME_Singleline_Response_Parser(pData, 0, ME_Get_PbMemInfo_Parser);
}

//AT+CPBR=
void ME_Read_Pbk_CB(void *pData)
{
	ME_Singleline_Response_Parser(pData, 0, ME_Read_Pbk_Parser);
}
/*
//AT+CIMI?
void ME_Get_UserIdentity_CB(void *pData)
{
	ME_Singleline_Response_Parser(pData, 0, ME_Get_UserIdentity_Parser);
}
*/
//AT+ERAT
void ME_Query_RATnPS_CB(void *pData)
{
	ME_Singleline_Response_Parser(pData, 0, ME_Query_RATnPS_Parser);
}

//AT+CLIR?
void ME_Get_CLIR_CB(void *pData)
{
	ME_Singleline_Response_Parser(pData, 0, ME_Get_CLIR_Parser);
}

//AT+EPIN2?
void ME_Query_PIN2_CB(void *pData)
{
	ME_Singleline_Response_Parser(pData, 0, ME_Query_PIN2_Parser);
}

//AT+CAOC=0
void ME_Set_CallMeterValue_CB(void *pData)
{
	ME_Singleline_Response_Parser(pData, 0, ME_Query_PIN2_Parser);
}

//AT+CACM?
void ME_Get_ACMValue_CB(void *pData)
{
	ME_Singleline_Response_Parser(pData, 0, ME_Query_PIN2_Parser);
}

//AT+CAMM?
void ME_Get_MaxACMValue_CB(void *pData)
{
	ME_Singleline_Response_Parser(pData, 0, ME_Query_PIN2_Parser);
}

//AT+CPUC?
void ME_Get_PUCValue_CB(void *pData)
{
	ME_Singleline_Response_Parser(pData, 0, ME_Get_PUCValue_Parser);
}

//AT+CEER
void ME_Extend_ErrReport_CB(void *pData)
{
	ME_Singleline_Response_Parser(pData, 0, ME_Extend_ErrReport_Parser);
}

//AT+CRSM
void ME_Restricted_SIM_CB(void *pData)
{
	ME_Singleline_Response_Parser(pData, 0, ME_Restricted_SIM_Parser);
}

//AT+CSCB?
void ME_Get_CBSInfo_CB(void *pData)
{
	ME_Singleline_Response_Parser(pData, 0, ME_Get_CBSInfo_Parser);
}
/////////////////////////////Multiple line reponse//////////////////////////////////////

//AT+CGPRCO?
void ME_Query_PrcoCfg_CB(void *pData)
{
	ME_Multiline_Response_Parser(pData, sizeof(CGPRCO), ME_Query_PrcoCfg_Parser);
}

//AT+CGACT?
void ME_Query_PDPState_CB(void *pData)
{
	ME_Multiline_Response_Parser(pData, sizeof(PDP_CONTEXT), ME_Query_PDPState_Paser);
}

//AT+CNUM
void ME_Query_MSISDNInfo_CB(void *pData)
{
	ME_Multiline_Response_Parser(pData, sizeof(MSISDN_INFO), ME_Query_MSISDNInfo_Parser);
}

//AT+CLCK=
void ME_Set_FacLock_CB(void *pData)
{
	ME_Multiline_Response_Parser(pData, sizeof(FAC_LOCK), ME_Set_FacLock_Parser);
}

//AT+COPS=?
void ME_Query_OperatorList_CB(void *pData)
{
	ME_Multiline_Response_Parser(pData, sizeof(OPER_LIST), ME_Query_OperatorList_Parser);
}

//AT+MBPC=?
void ME_List_Provisioned_Contexts_CB(void *pData)
{
	ME_Multiline_Response_Parser(pData, sizeof(MB_CONTEXT), ME_List_Provisioned_Contexts_Parser);
}

//AT+MBPC?
void ME_Read_Provisioned_Contexts_CB(void *pData)
{
	ME_Multiline_Response_Parser(pData, sizeof(MB_CONTEXT), ME_List_Provisioned_Contexts_Parser);
}

//AT+CLCC
void ME_List_CurrentCall_CB(void *pData)
{
	ME_Multiline_Response_Parser(pData, 0, ME_List_CurrentCall_Parser);
}

//AT+CCFC
void ME_Set_CallForward_CB(void *pData)
{
	ME_Multiline_Response_Parser(pData, 0, ME_Set_CallForward_Parser);
}

//AT+CCWA
void ME_Set_CallWaiting_CB(void *pData)
{
	ME_Multiline_Response_Parser(pData, 0, ME_Set_CallWaiting_Parser);
}
//AT+CMGS=
void ME_Send_Message_CB(void *pData)
{
	PMEFSM pfsm = NULL;
	int errcode = 0;
	int state = 0;
	
	pfsm = (PMEFSM)pData;	
	me_lock(&pfsm->Lock);

	if(pfsm->req.nFsmState != FSM_STATE_DONE)
	{
		switch(pfsm->req.nFsmState)
		{
		case FSM_STATE_START:
			{	
				me_killtimer(&pfsm->Timer, pfsm->req.event);
				pfsm->req.nFsmState = FSM_STATE_STEP1;
				me_strcpy(pfsm->req.expectedCmd, "+CMGS");
				me_settimer(&pfsm->Timer, LONG_TIMEOUT_ELAPSE);
				me_send_pdu(pfsm->ioctl.ioproxy, pfsm->pdu, pfsm->ioctl.notify, pfsm);
				me_unlock(&pfsm->Lock);
				return;
			}
			break;
		case FSM_STATE_STEP1:
			{
				state = AnalyingRespString(pfsm, &errcode);
				if(state == PARSER_DOING)
				{
					ME_Query_ModemBearer_Parser(pfsm, NULL);

					me_unlock(&pfsm->Lock);
					return;
				}
				else if(state == PARSER_EXIT)
				{
					me_unlock(&pfsm->Lock);
					return;
				}
			}
			break;
		}
		
		me_killtimer(&pfsm->Timer, pfsm->req.event);
		pfsm->req.nFsmState = FSM_STATE_DONE;
	}

	if(pfsm->bWriteDone)
	{
//		me_lock(&pfsm->LockSending); 
		pfsm->req.atproc = NULL;	
//		me_unlock(&pfsm->LockSending); 

		pfsm->req.reply(pData);
	}
	
	me_unlock(&pfsm->Lock);
}

//AT+CMGS=
void ME_Write_Message_CB(void *pData)
{
	PMEFSM pfsm = NULL;
	int errcode = 0;
	int state = 0;
	
	pfsm = (PMEFSM)pData;	
	me_lock(&pfsm->Lock);
	
	if(pfsm->req.nFsmState != FSM_STATE_DONE)
	{
		switch(pfsm->req.nFsmState)
		{
		case FSM_STATE_START:
			{	
				me_killtimer(&pfsm->Timer, pfsm->req.event);
				pfsm->req.nFsmState = FSM_STATE_STEP1;
				me_strcpy(pfsm->req.expectedCmd, "+CMGW");
				me_settimer(&pfsm->Timer, LONG_TIMEOUT_ELAPSE);
				me_send_pdu(pfsm->ioctl.ioproxy, pfsm->pdu, pfsm->ioctl.notify, pfsm);
				me_unlock(&pfsm->Lock);
				return;
			}
			break;
		case FSM_STATE_STEP1:
			{
				state = AnalyingRespString(pfsm, &errcode);
				if(state == PARSER_DOING)
				{
					ME_Query_ModemBearer_Parser(pfsm, NULL);

					me_unlock(&pfsm->Lock);
					return;
				}
				else if(state == PARSER_EXIT)
				{
					me_unlock(&pfsm->Lock);
					return;
				}
			}
			break;
		}
		
		me_killtimer(&pfsm->Timer, pfsm->req.event);
		pfsm->req.nFsmState = FSM_STATE_DONE;
	}
	
	if(pfsm->bWriteDone)
	{
//		me_lock(&pfsm->LockSending); 
		pfsm->req.atproc = NULL;	
//		me_unlock(&pfsm->LockSending); 

		pfsm->req.reply(pData);
	}		

	me_unlock(&pfsm->Lock);
}

//AT+CMGR=
void ME_Read_Message_CB(void *pData)
{
	PMEFSM pfsm = NULL;
	int errcode = 0;
	int state = 0;

	pfsm = (PMEFSM)pData;	
	me_lock(&pfsm->Lock);

	if(pfsm->req.nFsmState != FSM_STATE_DONE)
	{
		switch(pfsm->req.nFsmState)
		{
		case FSM_STATE_START:
			{	
				state = AnalyingRespString(pfsm, &errcode);
				if(state == PARSER_DOING)
				{
					ME_Read_SMS_Parser(pfsm, NULL);

					pfsm->req.nFsmState = FSM_STATE_STEP1;
					pfsm->bReadPdu = TRUE;
					me_unlock(&pfsm->Lock);
					return;
				}
				else if(state == PARSER_EXIT)
				{
					me_unlock(&pfsm->Lock);
					return;
				}
			}
			break;
		case FSM_STATE_STEP1:
			{
				ME_Read_SMS_Parser(pfsm, NULL);

				me_unlock(&pfsm->Lock);
				pfsm->req.nFsmState = FSM_STATE_STEP2;
				pfsm->bReadPdu = FALSE;
				return;
			}
			break;
		case FSM_STATE_STEP2:
			{
				if(AnalyingRespString(pfsm, &errcode) == PARSER_EXIT)
				{
					me_unlock(&pfsm->Lock);
					return;
				}
			}	
		}

		me_killtimer(&pfsm->Timer, pfsm->req.event);
		pfsm->req.nFsmState = FSM_STATE_DONE;
	}

	if(pfsm->bWriteDone)
	{
//		me_lock(&pfsm->LockSending); 
		pfsm->req.atproc = NULL;	
//		me_unlock(&pfsm->LockSending); 

		pfsm->req.reply(pData);

	}
	me_unlock(&pfsm->Lock);
}

//AT+CMGL=
void ME_List_Message_CB(void *pData)
{
	PATRESULT result = NULL;
	PMEFSM pfsm = NULL;
	PLIST_MSG pListMsg = NULL;
	PLIST_MSG pNode = NULL;
	int errcode = 0;
	int state = 0;
	
	pfsm = (PMEFSM)pData;	
	me_lock(&pfsm->Lock);

	if(pfsm->req.nFsmState != FSM_STATE_DONE)
	{
		if((result = (PATRESULT)CreateRspObj(pfsm, sizeof(LIST_MSG))) == NULL)
		{
			me_unlock(&pfsm->Lock);
			return;
		}
		
		switch(pfsm->req.nFsmState)
		{
		case FSM_STATE_START:
			{	
				state = AnalyingRespString(pfsm, &errcode);
				if(state == PARSER_DOING)
				{
					pNode = (PLIST_MSG)result->pRet;
					if(pNode->tail == NULL)				
					{
						pNode->tail = pNode;
						pNode->next = NULL;
					}
					else
					{
						pNode = (PLIST_MSG)me_malloc(sizeof(LIST_MSG));
						//					ME_Print("malloc node addr = 0x%08x\n", pNode);
						memset(pNode, 0x00, sizeof(LIST_MSG));
						pNode->next = NULL;
						pNode->tail = pNode;
						
						pListMsg = (PLIST_MSG)result->pRet;
						pListMsg->tail->next = pNode;
						pListMsg->tail = pNode;
					}
					
					ME_List_SMS_Parser(pfsm, pNode);
					pfsm->req.nFsmState = FSM_STATE_STEP1;
					pfsm->bReadPdu = TRUE;
					me_unlock(&pfsm->Lock);
					return;
				}
				else if(state == PARSER_EXIT)
				{
					me_unlock(&pfsm->Lock);
					return;
				}
			}
			break;
		case FSM_STATE_STEP1:
			{
				pNode = ((PLIST_MSG)result->pRet)->tail;
				me_strcpy(pNode->Msg.pdu, pfsm->ctldata);
				me_unlock(&pfsm->Lock);
				pfsm->req.nFsmState = FSM_STATE_START;
				pfsm->bReadPdu = FALSE;
				return;
			}
			break;
		}
	
		me_killtimer(&pfsm->Timer, pfsm->req.event);
		result->err = errcode;
		pfsm->req.nFsmState = FSM_STATE_DONE;
	}

	if(pfsm->bWriteDone)
	{
		result = (PATRESULT)pfsm->pRspObj;
		pfsm->pRspObj = NULL;

//		me_lock(&pfsm->LockSending); 
		pfsm->req.atproc = NULL;	
//		me_unlock(&pfsm->LockSending); 

		pfsm->req.reply(pData);

		pListMsg = ((PLIST_MSG)result->pRet)->next;
		while(pListMsg != NULL)
		{
			pNode = pListMsg->next;
	//		ME_Print("free node addr = 0x%08x\n", pListMsg);
			me_free(pListMsg);
			pListMsg = pNode;
		}
		DestroyRspObj(result);
	}

	me_unlock(&pfsm->Lock);
}
