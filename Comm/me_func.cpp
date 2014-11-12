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

typedef void (*ME_RspCallBack) (void *pData);

#define FILLREQUEST(a, b, c, d)	\
do{								\
	a->req.reply = me_respond_ready;\
	a->req.atproc  = c;			\
	a->req.nFsmState = FSM_STATE_START;\
	a->req.event = AT_RESPONSE;	\
	a->req.context = d;			\
	a->pRspObj = NULL;			\
}while(0)

void TimeoutProc(ME_HANDLE handle)
{
	PMEFSM pfsm = (PMEFSM)handle;
	if((pfsm->req.atproc != NULL) && (pfsm->req.nFsmState != FSM_STATE_DONE))
	{
		pfsm->req.event = AT_TIMEOUT;
		DebugOut(_T("time out proc -> todo\n"));
		pfsm->req.atproc(pfsm);
	}	
}
 
NTSTATUS TX_Callback(NTSTATUS status, int len, void *pData)
{
	PMEFSM pfsm = (PMEFSM)pData;
	
	pfsm->bWriteDone = TRUE;
	if(status != STATUS_SUCCESS)
	{
		pfsm->req.event = AT_WRITEFAIL;
		pfsm->req.atproc(pfsm);
	}
	else
	{
		if((pfsm->req.nFsmState == FSM_STATE_DONE) && (pfsm->req.atproc != NULL))
			pfsm->req.atproc(pfsm);
	}

	return 1;
}


ME_HANDLE ME_Init(void* ioproxy)
{
	PMEFSM pfsm = (PMEFSM )me_malloc(sizeof(MEFSM));
	pfsm->state = 0;
	pfsm->nHead = 0;
	pfsm->nTail = 0;
	pfsm->nReadIdx  = 0;
	pfsm->nWriteIdx = 0;
	pfsm->nUrcId = -1;
	pfsm->bWaitPdu = FALSE;
	pfsm->bReadPdu = FALSE; 
	pfsm->bWriteDone = FALSE;
	pfsm->nUrcState = FSM_STATE_START;
	pfsm->ioctl.ioproxy = ioproxy;
	pfsm->ioctl.notify = TX_Callback;
	pfsm->pRspObj = NULL;
	pfsm->req.atproc = NULL;
	pfsm->req.nFsmState = FSM_STATE_DONE;

	me_initlock(&pfsm->Lock);
	me_initlock(&pfsm->LockSending);


	pfsm->Timer.timer_cb = TimeoutProc;
	pfsm->Timer.pData = (void *)pfsm;
	me_inittimer(&pfsm->Timer);

	pfsm->urcproc = ME_URC_Parser;
	for(int i=0; i<ID_MAX; i++)
	{
		pfsm->urclist[i].reply = NULL;
		pfsm->urclist[i].context = NULL;
	}

	pfsm->req.hBlockEvt = CreateEvent(NULL, FALSE, FALSE, NULL);
	ResetEvent(pfsm->req.hBlockEvt);
	
	return (ME_HANDLE)pfsm;
}

BOOLEAN ME_DeInit(ME_HANDLE handle)
{
	if(handle != NULL)
	{
		PMEFSM pfsm = (PMEFSM)handle;
		
		if(pfsm->req.nFsmState != FSM_STATE_DONE)
			return FALSE;

		me_exittimer(&pfsm->Timer);
		me_free(handle);

		return TRUE;
	}
	
	return FALSE;
}

BOOLEAN	ME_Cancel(ME_HANDLE handle)
{
	if(handle != NULL)
	{
		PMEFSM pfsm = (PMEFSM)handle;
		
		me_lock(&pfsm->Lock); 
//		me_lock(&pfsm->LockSending); 
		if (pfsm->req.atproc != NULL)
		{
			pfsm->req.event = AT_CANCEL;
			pfsm->req.atproc(pfsm);
		}
//		me_unlock(&pfsm->LockSending); 
		me_unlock(&pfsm->Lock);

		return TRUE;
	}

	return FALSE;
}

NTSTATUS SendCommand(PMEFSM pfsm, const char *cmd)
{
	NTSTATUS status; 
	pfsm->bWriteDone = FALSE;

	status = me_send_at(pfsm->ioctl.ioproxy, cmd, pfsm->ioctl.notify, pfsm);

	return status;
}

NTSTATUS ME_Fill_FSM(ME_HANDLE handle, ME_Callback reply, void* context, 
					 ME_RspCallBack rspcb, const char *cmd, const char *exCmd, int timout)
{
	PMEFSM pfsm = NULL;
	
	if(handle == NULL)
		return STATUS_INVALID_PARAMETER;
	
	pfsm = (PMEFSM)handle;
//	me_lock(&pfsm->LockSending); 

	if(pfsm->req.nFsmState != FSM_STATE_DONE)
		return STATUS_INVALID_PARAMETER;
	
	FILLREQUEST(pfsm, reply, rspcb, context);
	
//	me_strcpy(pfsm->req.cmd, cmd);
	if(exCmd != NULL)
		me_strcpy(pfsm->req.expectedCmd, exCmd);	
	else
		pfsm->req.expectedCmd[0] = '\0';
	
	me_settimer(&pfsm->Timer, timout);
	
	if(STATUS_SUCCESS != SendCommand(pfsm, cmd))
		return 100;//ER_UNKNOWN;
//	me_unlock(&pfsm->LockSending); 

	return me_get_rettype(handle);
}

//AT+EPSB?
NTSTATUS ME_Query_ModemBearer(ME_HANDLE handle, ME_Callback reply, void* context)
{
	const char *cmd   = "AT+EPSB?\r";
	const char *exCmd = "+EPSB";

	return ME_Fill_FSM(handle, reply, context, ME_Query_ModemBearer_CB, cmd, exCmd, SHORT_TIMEOUT_ELAPSE);
}

//AT+EPBSE=?
NTSTATUS ME_Query_BandInfo(ME_HANDLE handle, ME_Callback reply, void* context)
{
	const char *cmd   = "AT+EPBSE=?\r";
	const char *exCmd = "+EPBSE";
	
	return ME_Fill_FSM(handle, reply, context, ME_Query_BandInfo_CB, cmd, exCmd, SHORT_TIMEOUT_ELAPSE);
}

//AT+CGSN
NTSTATUS ME_Query_IMEI(ME_HANDLE handle, ME_Callback reply, void* context)
{
	const char *cmd   = "AT+CGSN\r";
	return ME_Fill_FSM(handle, reply, context, ME_Query_IMEI_CB, cmd, NULL, SHORT_TIMEOUT_ELAPSE);
}

//AT+CGMI								
NTSTATUS ME_Query_FacInfo(ME_HANDLE handle, ME_Callback reply, void* context)
{
	const char *cmd   = "AT+CGMI\r";
	const char *exCmd = "+CGMI";
	
	return ME_Fill_FSM(handle, reply, context, ME_Query_FacInfo_CB, cmd, exCmd, SHORT_TIMEOUT_ELAPSE);
}

//AT+CGMM
NTSTATUS ME_Query_ModelInfo(ME_HANDLE handle, ME_Callback reply, void* context)
{
	const char *cmd   = "AT+CGMM\r";
	const char *exCmd = "+CGMM";
	
	return ME_Fill_FSM(handle, reply, context, ME_Query_ModelInfo_CB, cmd, exCmd, SHORT_TIMEOUT_ELAPSE);
}

//AT+CGMR
NTSTATUS ME_Query_Revision(ME_HANDLE handle, ME_Callback reply, void* context)
{
	const char *cmd   = "AT+CGMR\r";
	const char *exCmd = "+CGMR";
	
	return ME_Fill_FSM(handle, reply, context, ME_Query_Revision_CB, cmd, exCmd, SHORT_TIMEOUT_ELAPSE);
}

/************ 20131109 by Zhuwei **********************************************/
//AT+VERNO
NTSTATUS ME_Query_VERNO(ME_HANDLE handle, ME_Callback reply, void* context)
{
    const char *cmd   = "AT+VERNO?\r";
	const char *exCmd = "+VERNO";
	return ME_Fill_FSM(handle, reply, context, ME_Query_GetVERNO_CB, cmd, exCmd, SHORT_TIMEOUT_ELAPSE);

}
/******************************************************************************/

//ATE0Q0V1
NTSTATUS ME_Set_ModemFunc(ME_HANDLE handle, ME_Callback reply, void* context)
{
	const char *cmd = "ATE0\r";

	return ME_Fill_FSM(handle, reply, context, ME_Set_ModemFunc_CB, cmd, NULL, SHORT_TIMEOUT_ELAPSE);
}

//AT+CMEE=
NTSTATUS ME_Set_ErrorFmt(ME_HANDLE handle, int n, ME_Callback reply, void* context)
{
	char cmd[AT_CMD_LEN];
	me_sprintf(cmd, "AT+CMEE=%d\r", n);

	return ME_Fill_FSM(handle, reply, context, ME_Set_ModemFunc_CB, cmd, NULL, SHORT_TIMEOUT_ELAPSE);
}

//AT+CREG=
NTSTATUS ME_Set_NetwrokReg(ME_HANDLE handle, int n, ME_Callback reply, void* context)
{
	char cmd[AT_CMD_LEN];
	me_sprintf(cmd, "AT+CREG=%d\r", n);
	
	return ME_Fill_FSM(handle, reply, context, ME_Set_ModemFunc_CB, cmd, NULL, SHORT_TIMEOUT_ELAPSE);
}

//AT+CREG?
NTSTATUS ME_Query_NetwrokReg(ME_HANDLE handle, ME_Callback reply, void* context)
{
	const char *cmd   = "AT+CREG?\r";
	const char *exCmd = "+CREG";
	
	return ME_Fill_FSM(handle, reply, context, ME_Query_NetwrokReg_CB, cmd, exCmd, SHORT_TIMEOUT_ELAPSE);
}

//AT+CGREG=
NTSTATUS ME_Set_GPRSState(ME_HANDLE handle, int n, ME_Callback reply, void* context)
{
	char cmd[AT_CMD_LEN];
	me_sprintf(cmd, "AT+CGREG=%d\r", n);
	
	return ME_Fill_FSM(handle, reply, context, ME_Set_ModemFunc_CB, cmd, NULL, SHORT_TIMEOUT_ELAPSE);
}

//AT+CGREG?
NTSTATUS ME_Query_GPRSState(ME_HANDLE handle, ME_Callback reply, void* context)
{
	const char *cmd   = "AT+CGREG?\r";
	const char *exCmd = "+CGREG";
	
	return ME_Fill_FSM(handle, reply, context, ME_Query_GPRSState_CB, cmd, exCmd, SHORT_TIMEOUT_ELAPSE);
}

//AT+ECSQ=
NTSTATUS ME_Set_SignalLevel(ME_HANDLE handle, int n, ME_Callback reply, void* context)
{
	char cmd[AT_CMD_LEN];
	me_sprintf(cmd, "AT+ECSQ=%d\r", n);
	
	return ME_Fill_FSM(handle, reply, context, ME_Set_ModemFunc_CB, cmd, NULL, SHORT_TIMEOUT_ELAPSE);
}

//AT+ECSQ?
NTSTATUS ME_Query_SignalLevel(ME_HANDLE handle, ME_Callback reply, void* context)
{
	const char *cmd   = "AT+ECSQ?\r";
	const char *exCmd = "+ECSQ";
	
	return ME_Fill_FSM(handle, reply, context, ME_Query_SignalLevel_CB, cmd, exCmd, SHORT_TIMEOUT_ELAPSE);
}

//AT+CFUN=
NTSTATUS ME_Set_ModemFunctionality(ME_HANDLE handle, int fun, int rst, ME_Callback reply, void* context)
{
	char cmd[AT_CMD_LEN];

	if(rst < 0)
		me_sprintf(cmd, "AT+CFUN=%d\r", fun);
	else
		me_sprintf(cmd, "AT+CFUN=%d,%d\r", fun, rst);

	return ME_Fill_FSM(handle, reply, context, ME_Set_ModemFunc_CB, cmd, NULL, SHORT_TIMEOUT_ELAPSE*3);
}

//AT+CFUN?
NTSTATUS ME_Query_ModemFunctionality(ME_HANDLE handle, ME_Callback reply, void* context)
{
	const char *cmd   = "AT+CFUN?\r";
	const char *exCmd = "+CFUN";
	
	return ME_Fill_FSM(handle, reply, context, ME_Query_ModemFunctionality_CB, cmd, exCmd, SHORT_TIMEOUT_ELAPSE);
}

//AT+CPIN=
NTSTATUS ME_Set_PIN(ME_HANDLE handle, char *pin, char *newpin, ME_Callback reply, void* context)
{
	//AT+CPIN=<pin>[,<newpin>]

	char cmd[AT_CMD_LEN];

	if(newpin == NULL)
		me_sprintf(cmd, "AT+CPIN=\"%s\"\r", pin);
	else
		me_sprintf(cmd, "AT+CPIN=\"%s\",\"%s\"\r", pin, newpin);

	return ME_Fill_FSM(handle, reply, context, ME_Set_ModemFunc_CB, cmd, NULL, SHORT_TIMEOUT_ELAPSE);	
}

//AT+CPIN?
NTSTATUS ME_Query_PINState(ME_HANDLE handle, ME_Callback reply, void* context)
{
	const char *cmd   = "AT+CPIN?\r";
	const char *exCmd = "+CPIN";
	
	return ME_Fill_FSM(handle, reply, context, ME_Query_PINState_CB, cmd, exCmd, SHORT_TIMEOUT_ELAPSE);
}

//AT+EPINC?
NTSTATUS ME_Query_PINRemainNum(ME_HANDLE handle, ME_Callback reply, void* context)
{
	const char *cmd   = "AT+EPINC?\r";
	const char *exCmd = "+EPINC";
	
	return ME_Fill_FSM(handle, reply, context, ME_Query_PINRemainNum_CB, cmd, exCmd, SHORT_TIMEOUT_ELAPSE);
}

//AT+CIMI
NTSTATUS ME_Query_IMSI(ME_HANDLE handle, ME_Callback reply, void* context)
{
	const char *cmd   = "AT+CIMI\r";	
	return ME_Fill_FSM(handle, reply, context, ME_Query_IMSI_CB, cmd, NULL, SHORT_TIMEOUT_ELAPSE);
}

//AT+CNUM
NTSTATUS ME_Query_MSISDNInfo(ME_HANDLE handle, ME_Callback reply, void* context)
{
	const char *cmd   = "AT+CNUM\r";
	const char *exCmd = "+CNUM";
	
	return ME_Fill_FSM(handle, reply, context, ME_Query_MSISDNInfo_CB, cmd, exCmd, SHORT_TIMEOUT_ELAPSE);
}

//AT+CPWD=
NTSTATUS ME_Set_Password(ME_HANDLE handle, char *fac, char *oldpwd, char *newpwd, ME_Callback reply, void* context)
{
	char cmd[AT_CMD_LEN];
	me_sprintf(cmd, "AT+CPWD=\"%s\",\"%s\",\"%s\"\r", fac, oldpwd, newpwd);	
	
	return ME_Fill_FSM(handle, reply, context, ME_Set_ModemFunc_CB, cmd, NULL, LONG_TIMEOUT_ELAPSE);
}

//AT+CLCK=
NTSTATUS ME_Set_FacLock(ME_HANDLE handle, const char *fac, int mode, char *pwd, int classx, ME_Callback reply, void* context)
{
	//AT+CLCK=<fac>,<mode>[,<passwd>[,<class>]]

	char cmd[AT_CMD_LEN];
	char exCmd[EXP_CMD_LEN];

	if(pwd != NULL && classx >= 0)
		me_sprintf(cmd, "AT+CLCK=\"%s\",%d,\"%s\",%d\r", fac, mode, pwd, classx);
	else if(pwd != NULL)
		me_sprintf(cmd, "AT+CLCK=\"%s\",%d,\"%s\"\r", fac, mode, pwd);
	else
		me_sprintf(cmd, "AT+CLCK=\"%s\",%d\r", fac, mode);

	if(mode != 2)
		exCmd[0] = '\0';
	else
		me_strcpy(exCmd, "+CLCK");
		
	return ME_Fill_FSM(handle, reply, context, ME_Set_FacLock_CB, cmd, exCmd, SHORT_TIMEOUT_ELAPSE);
}

//AT+COPS=
NTSTATUS ME_Set_Operator(ME_HANDLE handle, int mode, int fmt, char *opern, int act, ME_Callback reply, void* context)
{
	char cmd[AT_CMD_LEN];
	int timeout;

	if(act != INVALID_PARA)
		me_sprintf(cmd, "AT+COPS=%d,%d,\"%s\",%d\r", mode, fmt, opern, act);
	else if((opern != NULL) && opern[0] != 0)	
		me_sprintf(cmd, "AT+COPS=%d,%d,\"%s\"\r", mode, fmt, opern);
	else if(fmt != INVALID_PARA)
		me_sprintf(cmd, "AT+COPS=%d,%d\r", mode, fmt);
	else
		me_sprintf(cmd, "AT+COPS=%d\r", mode);
	
	if (opern == NULL)
	{
		timeout = LONG_TIMEOUT_ELAPSE;
	}
	else
	{
		timeout = 5*60*1000;
	}

	return ME_Fill_FSM(handle, reply, context, ME_Set_ModemFunc_CB, cmd, NULL, timeout);
}

//AT+COPS?
NTSTATUS ME_Query_Operator(ME_HANDLE handle, ME_Callback reply, void* context)
{
	const char *cmd   = "AT+COPS?\r";
	const char *exCmd = "+COPS";
	
	return ME_Fill_FSM(handle, reply, context, ME_Query_Operator_CB, cmd, exCmd, LONG_TIMEOUT_ELAPSE);
}

//AT+COPS=?
NTSTATUS ME_Query_OperatorList(ME_HANDLE handle, ME_Callback reply, void *context)
{
	const char *cmd   = "AT+COPS=?\r";
	const char *exCmd = "+COPS";
	
	return ME_Fill_FSM(handle, reply, context, ME_Query_OperatorList_CB, cmd, exCmd, LONG_TIMEOUT_ELAPSE);
}

//AT+CGDCONT=
NTSTATUS ME_Set_PDPContext(ME_HANDLE handle, PDPCONTEXT *pdp, ME_Callback reply, void* context)
{
	//+CGDCONT=[<cid> [,<PDP_type> [,<APN> [,<PDP_addr> [,<d_comp> [,<h_comp> [,<pd1> [,...[,pdN]]]]]]]]]

	char cmd[AT_CMD_LEN];
	
	if(pdp->h_comp != INVALID_PARA)
		me_sprintf(cmd, "AT+CGDCONT=%d,\"%s\",\"%s\",\"%s\",%d,%d\r", pdp->cid, pdp->pdp_type, pdp->apn, pdp->pdp_addr, pdp->d_comp, pdp->h_comp);
	else if(pdp->d_comp != INVALID_PARA)
		me_sprintf(cmd, "AT+CGDCONT=%d,\"%s\",\"%s\",\"%s\",%d\r", pdp->cid, pdp->pdp_type, pdp->apn, pdp->pdp_addr, pdp->d_comp);
	else if(pdp->pdp_addr[0] != 0)
		me_sprintf(cmd, "AT+CGDCONT=%d,\"%s\",\"%s\",\"%s\"\r", pdp->cid, pdp->pdp_type, pdp->apn, pdp->pdp_addr);
	else if(pdp->apn[0] != 0)
		me_sprintf(cmd, "AT+CGDCONT=%d,\"%s\",\"%s\"\r", pdp->cid, pdp->pdp_type, pdp->apn);
	else if(pdp->pdp_type[0] != 0)
		me_sprintf(cmd, "AT+CGDCONT=%d,\"%s\"\r", pdp->cid, pdp->pdp_type);
	else if(pdp->cid != INVALID_PARA)
		me_sprintf(cmd, "AT+CGDCONT=%d\r", pdp->cid);

	return ME_Fill_FSM(handle, reply, context, ME_Set_ModemFunc_CB, cmd, NULL, SHORT_TIMEOUT_ELAPSE);
}

//AT+CGQREQ=
NTSTATUS ME_Set_ServiceProQuality(ME_HANDLE handle, SERVICEPRO *pro, ME_Callback reply, void* context)
{
	//+CGQREQ=[<cid> [,<precedence > [,<delay> [,<reliability.> [,<peak> [,<mean>]]]]]]

	char cmd[AT_CMD_LEN];

	if(pro->mean != INVALID_PARA)
		me_sprintf(cmd, "AT+CGQREQ=%d,%d,%d,%d,%d,%d\r", pro->cid, pro->precedence, pro->delay, pro->reliability, pro->peak, pro->mean);
	else if(pro->peak != INVALID_PARA)
		me_sprintf(cmd, "AT+CGQREQ=%d,%d,%d,%d,%d\r", pro->cid, pro->precedence, pro->delay, pro->reliability, pro->peak);
	else if(pro->reliability != INVALID_PARA)
		me_sprintf(cmd, "AT+CGQREQ=%d,%d,%d,%d\r", pro->cid, pro->precedence, pro->delay, pro->reliability);
	else if(pro->delay != INVALID_PARA)
		me_sprintf(cmd, "AT+CGQREQ=%d,%d,%d\r", pro->cid, pro->precedence, pro->delay);
	else if(pro->precedence != INVALID_PARA)
		me_sprintf(cmd, "AT+CGQREQ=%d,%d\r", pro->cid, pro->precedence);
	else if(pro->cid != INVALID_PARA)
		me_sprintf(cmd, "AT+CGQREQ=%d\r", pro->cid);
	
	return ME_Fill_FSM(handle, reply, context, ME_Set_ModemFunc_CB, cmd, NULL, SHORT_TIMEOUT_ELAPSE);
}

//AT+CGQMIN=
NTSTATUS ME_Set_MiniAcceptablePro(ME_HANDLE handle, SERVICEPRO *pro, ME_Callback reply, void* context)
{
	//+CGQMIN=[<cid> [,<precedence > [,<delay> [,<reliability.> [,<peak> [,<mean>]]]]]]

	char cmd[AT_CMD_LEN];
	
	if(pro->mean != INVALID_PARA)
		me_sprintf(cmd, "AT+CGQMIN=%d,%d,%d,%d,%d,%d\r", pro->cid, pro->precedence, pro->delay, pro->reliability, pro->peak, pro->mean);
	else if(pro->peak != INVALID_PARA)
		me_sprintf(cmd, "AT+CGQMIN=%d,%d,%d,%d,%d\r", pro->cid, pro->precedence, pro->delay, pro->reliability, pro->peak);
	else if(pro->reliability != INVALID_PARA)
		me_sprintf(cmd, "AT+CGQMIN=%d,%d,%d,%d\r", pro->cid, pro->precedence, pro->delay, pro->reliability);
	else if(pro->delay != INVALID_PARA)
		me_sprintf(cmd, "AT+CGQMIN=%d,%d,%d\r", pro->cid, pro->precedence, pro->delay);
	else if(pro->precedence != INVALID_PARA)
		me_sprintf(cmd, "AT+CGQMIN=%d,%d\r", pro->cid, pro->precedence);
	else if(pro->cid != INVALID_PARA)
		me_sprintf(cmd, "AT+CGQMIN=%d\r", pro->cid);
	
	return ME_Fill_FSM(handle, reply, context, ME_Set_ModemFunc_CB, cmd, NULL, SHORT_TIMEOUT_ELAPSE);

}

//AT+CGEREP=
NTSTATUS ME_Set_GPRSEventReporting(ME_HANDLE handle, int mode, int bfr, ME_Callback reply, void* context)
{
	char cmd[AT_CMD_LEN];
	me_sprintf(cmd, "AT+CGEREP=%d,%d\r", mode, bfr);

	return ME_Fill_FSM(handle, reply, context, ME_Set_ModemFunc_CB, cmd, NULL, SHORT_TIMEOUT_ELAPSE);
}

//AT+CGPRCO=
NTSTATUS ME_Set_PrcoCfg(ME_HANDLE handle, PRCOCFG *pCfg, ME_Callback reply, void* context)
{
	//+CGPRCO=<cid>,<user_name>,<passwd>,<DNS1>,<DNS2>,<Auth_type>

	char cmd[AT_CMD_LEN];
	me_sprintf(cmd, "AT+CGPRCO=%d,\"%s\",\"%s\",\"%s\", \"%s\",%d\r", 
		pCfg->cid, pCfg->name, pCfg->pwd, pCfg->dns1, pCfg->dns2, pCfg->auth_type);
	
	return ME_Fill_FSM(handle, reply, context, ME_Set_ModemFunc_CB, cmd, NULL, SHORT_TIMEOUT_ELAPSE);

}

//AT+CGPRCO?
NTSTATUS ME_Query_PrcoCfg(ME_HANDLE handle, ME_Callback reply, void* context)
{
	const char *cmd   = "AT+CGPRCO?\r";
	const char *exCmd = "+CGPRCO";
	
	return ME_Fill_FSM(handle, reply, context, ME_Query_PrcoCfg_CB, cmd, exCmd, SHORT_TIMEOUT_ELAPSE);
}

//AT+CGACT=
NTSTATUS ME_Set_PDPState(ME_HANDLE handle, int state, int cid, ME_Callback reply, void* context)
{
	//+CGACT=[<state> [,<cid>]]

	char cmd[AT_CMD_LEN];

	if(cid != INVALID_PARA)
		me_sprintf(cmd, "AT+CGACT=%d,%d\r", state, cid);
	else
		me_sprintf(cmd, "AT+CGACT=%d\r", state);
	
	return ME_Fill_FSM(handle, reply, context, ME_Set_ModemFunc_CB, cmd, NULL, LONG_TIMEOUT_ELAPSE);

}

//AT+CGACT?
NTSTATUS ME_Query_PDPState(ME_HANDLE handle, ME_Callback reply, void* context)
{
	const char *cmd   = "AT+CGACT?\r";
	const char *exCmd = "+CGACT";
	
	return ME_Fill_FSM(handle, reply, context, ME_Query_PDPState_CB, cmd, exCmd, SHORT_TIMEOUT_ELAPSE);
}

//AT+CGPADDR=
NTSTATUS ME_Query_PDPAddr(ME_HANDLE handle, int cid, ME_Callback reply, void* context)
{
	char cmd[AT_CMD_LEN];
	const char *exCmd = "+CGPADDR";
	
	me_sprintf(cmd, "AT+CGPADDR=%d\r", cid);
	return ME_Fill_FSM(handle, reply, context, ME_Query_PDPAddr_CB, cmd, exCmd, SHORT_TIMEOUT_ELAPSE);
}

//AT+CGDATA=
NTSTATUS ME_Set_DataState(ME_HANDLE handle, char *L2P, int cid, ME_Callback reply, void* context)
{
	//+CGDATA=[<L2P> ,[<cid>]]

	char cmd[AT_CMD_LEN];
	
	if(cid != INVALID_PARA)
		me_sprintf(cmd, "AT+CGDATA=\"%s\",%d\r", L2P, cid);
	else
		me_sprintf(cmd, "AT+CGDATA=\"%s\"\r", L2P);
	
	return ME_Fill_FSM(handle, reply, context, ME_Set_ModemFunc_CB, cmd, NULL, LONG_TIMEOUT_ELAPSE);
}

//AT+CSQ
NTSTATUS ME_Query_SignalQuality(ME_HANDLE handle, ME_Callback reply, void* context)
{
	const char *cmd   = "AT+CSQ\r";
	const char *exCmd = "+CSQ";
	
	return ME_Fill_FSM(handle, reply, context, ME_Query_SignalQuality_CB, cmd, exCmd, SHORT_TIMEOUT_ELAPSE);
}

//AT+CPOL=
NTSTATUS ME_Set_PreOperatorList(ME_HANDLE handle, PREOPERLIST *oplist, ME_Callback reply, void* context)
{
	//+CPOL=[<index>][, <format>[,<oper>[<GSM_AcT>,<GSM_compact_AcT>,<UTRAN_AcT>]]]

	char cmd[AT_CMD_LEN];
	
	if(oplist->index != INVALID_PARA)
	{
		if(oplist->gsm_act != INVALID_PARA)
			me_sprintf(cmd, "AT+CPOL=%d,%d,\"%s\",%d,%d,%d\r", 
			oplist->index, oplist->fmt, oplist->oper, oplist->gsm_act, oplist->gsm_compact_act, oplist->utran_act);
		else if(oplist->oper[0] != 0)
			me_sprintf(cmd, "AT+CPOL=%d,%d,\"%s\"\r", oplist->index, oplist->fmt, oplist->oper);
		else if(oplist->fmt != INVALID_PARA)
			me_sprintf(cmd, "AT+CPOL=%d,%d\r", oplist->index, oplist->fmt);
		else
			me_sprintf(cmd, "AT+CPOL=%d\r", oplist->index);
	}
	else
	{
		if(oplist->gsm_act != INVALID_PARA)
			me_sprintf(cmd, "AT+CPOL=,%d,\"%s\",%d,%d,%d\r", 
			oplist->fmt, oplist->oper, oplist->gsm_act, oplist->gsm_compact_act, oplist->utran_act);
		else if(oplist->oper[0] != 0)
			me_sprintf(cmd, "AT+CPOL=,%d,\"%s\"\r", oplist->fmt, oplist->oper);
		else if(oplist->fmt != INVALID_PARA)
			me_sprintf(cmd, "AT+CPOL=,%d\r", oplist->fmt);
	}
	
	return ME_Fill_FSM(handle, reply, context, ME_Set_ModemFunc_CB, cmd, NULL, SHORT_TIMEOUT_ELAPSE);
}

//AT+ESPN?
NTSTATUS ME_Query_SIMSPN(ME_HANDLE handle, ME_Callback reply, void* context)
{
	const char *cmd   = "AT+ESPN?\r";
	const char *exCmd = "+ESPN";
	
	return ME_Fill_FSM(handle, reply, context, ME_Query_SIMSPN_CB, cmd, exCmd, SHORT_TIMEOUT_ELAPSE);
}	

//AT+ICCID?
NTSTATUS ME_Query_SIMICCID(ME_HANDLE handle, ME_Callback reply, void* context)
{
	const char *cmd   = "AT+ICCID?\r"; 
	const char *exCmd = "+ICCID";
	
	return ME_Fill_FSM(handle, reply, context, ME_Query_SIMICCID_CB, cmd, exCmd, SHORT_TIMEOUT_ELAPSE);
}

//AT+ESIMS?
NTSTATUS ME_Query_SIMStatus(ME_HANDLE handle, ME_Callback reply, void* context)
{
	const char *cmd   = "AT+ESIMS?\r"; 
	const char *exCmd = "+ESIMS";
	
	return ME_Fill_FSM(handle, reply, context, ME_Query_ModemBearer_CB, cmd, exCmd, SHORT_TIMEOUT_ELAPSE);
}

//AT+ESIMS=
NTSTATUS ME_Set_SIMUrcState(ME_HANDLE handle, int mode, ME_Callback reply, void* context)
{
	char cmd[AT_CMD_LEN];
	me_sprintf(cmd, "AT+ESIMS=%d\r", mode);
	
	return ME_Fill_FSM(handle, reply, context, ME_Set_ModemFunc_CB, cmd, NULL, SHORT_TIMEOUT_ELAPSE);
}					

//AT+CGATT=
NTSTATUS ME_Set_PSState(ME_HANDLE handle, int state, ME_Callback reply, void* context)
{
	char cmd[AT_CMD_LEN];
	me_sprintf(cmd, "AT+CGATT=%d\r", state);
	
	return ME_Fill_FSM(handle, reply, context, ME_Set_ModemFunc_CB, cmd, NULL, LONG_TIMEOUT_ELAPSE);
}

//AT+CGATT?
NTSTATUS ME_Query_PSState(ME_HANDLE handle, ME_Callback reply, void* context)
{
	const char *cmd   = "AT+CGATT?\r";
	const char *exCmd = "+CGATT";
	
	return ME_Fill_FSM(handle, reply, context, ME_Query_PSState_CB, cmd, exCmd, SHORT_TIMEOUT_ELAPSE);
}

//AT+ESLN  //sync call log to sim card
NTSTATUS ME_Sync_CallLog(ME_HANDLE handle, ME_Callback reply, void* context)
{
	const char *cmd   = "AT+ESLN\r";
	
	return ME_Fill_FSM(handle, reply, context, ME_Set_ModemFunc_CB, cmd, NULL, SHORT_TIMEOUT_ELAPSE*3);
}		

//AT+MBPC=?
NTSTATUS ME_List_Provisioned_Contexts(ME_HANDLE handle, ME_Callback reply, void* context)
{
	const char *cmd   = "AT+MBPC=?\r";
	const char *exCmd = "+MBPC";
	
	return ME_Fill_FSM(handle, reply, context, ME_List_Provisioned_Contexts_CB, cmd, exCmd, SHORT_TIMEOUT_ELAPSE);
}

//AT+MBPC=
NTSTATUS ME_Write_Provisioned_Contexts(ME_HANDLE handle, SUB_WWANCONTEXT *pro, ME_Callback reply, void* context)
{
	char cmd[AT_CMD_LEN];
	
	if(pro->opcode == 1)
		me_sprintf(cmd, "AT+MBPC=1,%d,%d,%d,\"%s\",\"%s\",\"%s\",%d,%d,\"%s\"\r", 
		pro->entry_id, pro->context_id, pro->context_type, pro->access_string, pro->name, pro->pwd, pro->comp, pro->auth, pro->plmn);
	else
		me_sprintf(cmd, "AT+MBPC=0,%d\r", pro->entry_id);

	return ME_Fill_FSM(handle, reply, context, ME_Set_ModemFunc_CB, cmd, NULL, SHORT_TIMEOUT_ELAPSE);

}

//AT+MBPC?
NTSTATUS ME_Read_Provisioned_Contexts(ME_HANDLE handle, ME_Callback reply, void* context)
{
	const char *cmd   = "AT+MBPC?\r";
	const char *exCmd = "+MBPC";
	
	return ME_Fill_FSM(handle, reply, context, ME_Read_Provisioned_Contexts_CB, cmd, exCmd, SHORT_TIMEOUT_ELAPSE);
}

//AT+EGTYPE=
NTSTATUS ME_Set_GPRSAttachMode(ME_HANDLE handle, int mode, int act, ME_Callback reply, void* context)
{
	char cmd[AT_CMD_LEN];

	if (act >= 0)
		me_sprintf(cmd, "AT+EGTYPE=%d,%d\r", mode, act);
	else
		me_sprintf(cmd, "AT+EGTYPE=%d\r", mode);

	return ME_Fill_FSM(handle, reply, context, ME_Set_ModemFunc_CB, cmd, NULL, SHORT_TIMEOUT_ELAPSE);
}

//AT+EIND?
NTSTATUS ME_Query_Indication(ME_HANDLE handle, ME_Callback reply, void* context)
{
	const char *cmd   = "AT+EIND\r";
	const char *exCmd = "+EIND";
	
	return ME_Fill_FSM(handle, reply, context, ME_Query_Indication_CB, cmd, exCmd, SHORT_TIMEOUT_ELAPSE);
}

//AT+CMER=
NTSTATUS ME_Set_CMER(ME_HANDLE handle, int mode, int keyp, int disp, 
					 int ind, int bfr, int tscrn, ME_Callback reply, void* context)
{
    //AT+CMER=[<mode>[,<keyp>[,<disp>[,<ind>[,<bfr>][,<tscrn>]]]]]

	char cmd[AT_CMD_LEN];
	//char exCmd[EXP_CMD_LEN];
	const char *exCmd = "+CMER";

	if(mode<0)
		me_sprintf(cmd, "AT+CMER=\r");
	else if(keyp<0)
		me_sprintf(cmd, "AT+CMER=%d\r", mode);
	else if(disp<0)
		me_sprintf(cmd, "AT+CMER=%d,%d\r", mode, keyp);
	else if(ind<0)
		me_sprintf(cmd, "AT+CMER=%d,%d,%d\r", mode, keyp, disp);
	else if(bfr<0)
		me_sprintf(cmd, "AT+CMER=%d,%d,%d,%d\r", mode, keyp, disp, ind);
	else if(tscrn<0)
		me_sprintf(cmd, "AT+CMER=%d,%d,%d,%d,%d\r", mode, keyp, disp, ind, bfr);
	else
		me_sprintf(cmd, "AT+CMER=%d,%d,%d,%d,%d,%d\r", mode, keyp, disp, ind, bfr, tscrn);

	return ME_Fill_FSM(handle, reply, context, ME_Set_CMER_CB, cmd, exCmd, SHORT_TIMEOUT_ELAPSE);
}


//AT+CSCA=
NTSTATUS ME_Set_ServiceCenterAddress(ME_HANDLE handle, SMCS_ADDR *addr, ME_Callback reply, void* context)
{
	char cmd[AT_CMD_LEN];
	
	if(addr->tosca != INVALID_PARA)
		me_sprintf(cmd, "AT+CSCA=\"%s\",%d\r", addr->sca, addr->tosca);
	else
		me_sprintf(cmd, "AT+CSCA=\"%s\"\r", addr->sca);
	
	return ME_Fill_FSM(handle, reply, context, ME_Set_ModemFunc_CB, cmd, NULL, SHORT_TIMEOUT_ELAPSE);
}

//AT+CSCA?
NTSTATUS ME_Get_ServiceCenterAddress(ME_HANDLE handle, ME_Callback reply, void* context)
{
	const char *cmd   = "AT+CSCA?\r";
	const char *exCmd = "+CSCA";
	
	return ME_Fill_FSM(handle, reply, context, ME_Get_ServiceCenterAddress_CB, cmd, exCmd, SHORT_TIMEOUT_ELAPSE);
}

//AT+CNMI=
NTSTATUS ME_Set_MessageIndication(ME_HANDLE handle, SMS_INDICATION *pIndt, ME_Callback reply, void* context)
{
	char cmd[AT_CMD_LEN];
	
	if(pIndt->bfr != INVALID_PARA)
		me_sprintf(cmd, "AT+CNMI=%d,%d,%d,%d,%d\r", pIndt->mode, pIndt->mt, pIndt->bm, pIndt->ds, pIndt->bfr);
	else if(pIndt->ds != INVALID_PARA)
		me_sprintf(cmd, "AT+CNMI=%d,%d,%d,%d\r", pIndt->mode, pIndt->mt, pIndt->bm, pIndt->ds);
	else if(pIndt->bm != INVALID_PARA)
		me_sprintf(cmd, "AT+CNMI=%d,%d,%d\r", pIndt->mode, pIndt->mt, pIndt->bm);
	else if(pIndt->mt != INVALID_PARA)
		me_sprintf(cmd, "AT+CNMI=%d,%d\r", pIndt->mode, pIndt->mt);
	else
		me_sprintf(cmd, "AT+CNMI=%d\r", pIndt->mode);
	
	return ME_Fill_FSM(handle, reply, context, ME_Set_ModemFunc_CB, cmd, NULL, SHORT_TIMEOUT_ELAPSE);
}

//AT+CPMS=
NTSTATUS ME_Get_MessageMemoryState(ME_HANDLE handle, const char *mem12, const char *mem3, ME_Callback reply, void* context)
{
	char cmd[AT_CMD_LEN];
	const char *exCmd = "+CPMS";
	
	if(mem3 == NULL)
		me_sprintf(cmd, "AT+CPMS=\"%s\",\"%s\"\r", mem12, mem12);
	else
		me_sprintf(cmd, "AT+CPMS=\"%s\",\"%s\",\"%s\"\r", mem12, mem12, mem3);
	
	return ME_Fill_FSM(handle, reply, context, ME_Get_MessageMemoryState_CB, cmd, exCmd, SHORT_TIMEOUT_ELAPSE);
}

//AT+CMGD=
NTSTATUS ME_Delete_Message(ME_HANDLE handle, int idx, int flag, ME_Callback reply, void* context)
{
	char cmd[AT_CMD_LEN];

	if(flag != INVALID_PARA)
		me_sprintf(cmd, "AT+CMGD=%d,%d\r", idx, flag);
	else
		me_sprintf(cmd, "AT+CMGD=%d\r", idx);

	return ME_Fill_FSM(handle, reply, context, ME_Set_ModemFunc_CB, cmd, NULL, LONG_TIMEOUT_ELAPSE);

} 

// AT+CMGS=
NTSTATUS ME_Send_Message(ME_HANDLE handle, const char *pdu, int size, ME_Callback reply, void* context)
{
	PMEFSM pfsm = NULL;
	int pduLen = me_strlen(pdu);
	
	if(handle == NULL)
		return STATUS_INVALID_PARAMETER;
	
	pfsm = (PMEFSM)handle;
	if(pfsm->req.nFsmState != FSM_STATE_DONE)
		return STATUS_INVALID_PARAMETER;
	
	FILLREQUEST(pfsm, reply, ME_Send_Message_CB, context);
	
	char cmd[32];
	me_sprintf(cmd, "AT+CMGS=%d\r", size);

	me_strcpy(pfsm->pdu, pdu);
	pfsm->pdu[pduLen] = 0x1A;
	pfsm->pdu[pduLen+1] = 0;

	me_strcpy(pfsm->req.expectedCmd, "> ");
	pfsm->bWaitPdu = TRUE;
	
	me_settimer(&pfsm->Timer, SHORT_TIMEOUT_ELAPSE);

	if(STATUS_SUCCESS != SendCommand(pfsm, cmd))
		return 100;//ER_UNKNOWN;
	
	return me_get_rettype(handle);
}

// AT+CMGW=
NTSTATUS ME_Write_Message(ME_HANDLE handle, const char *pdu, int size, int stat, ME_Callback reply, void* context)
{
	PMEFSM pfsm = NULL;
	int pduLen = me_strlen(pdu);
	
	if(handle == NULL)
		return STATUS_INVALID_PARAMETER;
	
	pfsm = (PMEFSM)handle;
	if(pfsm->req.nFsmState != FSM_STATE_DONE)
		return STATUS_INVALID_PARAMETER;
	
	FILLREQUEST(pfsm, reply, ME_Write_Message_CB, context);
	
	char cmd[32];
	me_sprintf(cmd, "AT+CMGW=%d,%d\r", size, stat);

	me_strcpy(pfsm->pdu, pdu);
	pfsm->pdu[pduLen] = 0x1A;
	pfsm->pdu[pduLen+1] = 0;

	me_strcpy(pfsm->req.expectedCmd, "> ");
	pfsm->bWaitPdu = TRUE;
	
	me_settimer(&pfsm->Timer, SHORT_TIMEOUT_ELAPSE);
	
	if(STATUS_SUCCESS != SendCommand(pfsm, cmd))
		return 100;//ER_UNKNOWN;
	
	return me_get_rettype(handle);
}

//AT+CMGR=
NTSTATUS ME_Read_Message(ME_HANDLE handle, int idx, ME_Callback reply, void* context)
{
	char cmd[AT_CMD_LEN];
	const char *exCmd = "+CMGR";
	
	me_sprintf(cmd, "AT+CMGR=%d\r", idx);
	return ME_Fill_FSM(handle, reply, context, ME_Read_Message_CB, cmd, exCmd, SHORT_TIMEOUT_ELAPSE);
}

//AT+CMGL=
NTSTATUS ME_List_Message(ME_HANDLE handle, int stat, ME_Callback reply, void* context) 
{
	char cmd[AT_CMD_LEN];
	const char *exCmd = "+CMGL";
	
	me_sprintf(cmd, "AT+CMGL=%d\r", stat);
	return ME_Fill_FSM(handle, reply, context, ME_List_Message_CB, cmd, exCmd, LONG_TIMEOUT_ELAPSE);
}

//AT+CAPL
NTSTATUS ME_Abort_SearchOperator(ME_HANDLE handle, ME_Callback reply, void* context)
{
	const char *cmd   = "AT+CAPL\r";
	return ME_Fill_FSM(handle, reply, context, ME_Set_ModemFunc_CB, cmd, NULL, SHORT_TIMEOUT_ELAPSE);
}

//AT+CRSM
NTSTATUS ME_Read_Transparent_SIMFile(ME_HANDLE handle, int id, int len, ME_Callback reply, void* context)
{
	char cmd[AT_CMD_LEN];
	const char *exCmd = "+CRSM";

	me_sprintf(cmd, "AT+CRSM=176,%d,0,0,%d\r", id, len);
	return ME_Fill_FSM(handle, reply, context, ME_Read_Transparent_SIMFile_CB, cmd, exCmd, SHORT_TIMEOUT_ELAPSE);
}

//AT+ESLP
NTSTATUS ME_Set_SleepMode(ME_HANDLE handle, BOOLEAN bEnable, ME_Callback reply, void* context)
{
	char cmd[AT_CMD_LEN];
	
	me_sprintf(cmd, "AT+ESLP=%d\r", (int)bEnable);	
	return ME_Fill_FSM(handle, reply, context, ME_Set_ModemFunc_CB, cmd, NULL, SHORT_TIMEOUT_ELAPSE);
}		

//AT+CPMS?
NTSTATUS ME_Get_CurrentSMSMemType(ME_HANDLE handle, ME_Callback reply, void* context)
{
	const char *cmd =  "AT+CPMS?";
	const char *exCmd = "+CPMS";

	return ME_Fill_FSM(handle, reply, context, ME_Get_CurrentSMSMemType_CB, cmd, exCmd, SHORT_TIMEOUT_ELAPSE);
}

//AT+EQSI=
NTSTATUS ME_Get_SMSMemIndex(ME_HANDLE handle, const char *mem, ME_Callback reply, void* context)
{
	char cmd[AT_CMD_LEN];
	const char *exCmd = "+EQSI";
	
	me_sprintf(cmd, "AT+EQSI=\"%s\"\r", mem);
	return ME_Fill_FSM(handle, reply, context, ME_Get_SMSMemIndex_CB, cmd, exCmd, SHORT_TIMEOUT_ELAPSE);
}

//AT+CMGF=
NTSTATUS ME_Set_MessageFormat(ME_HANDLE handle, int mode, ME_Callback reply, void* context)
{
	char cmd[AT_CMD_LEN];
	
	me_sprintf(cmd, "AT+CMGF=%d\r", mode); 
	return ME_Fill_FSM(handle, reply, context, ME_Set_ModemFunc_CB, cmd, NULL, SHORT_TIMEOUT_ELAPSE);
}

//AT+ESMSS
NTSTATUS ME_Set_MessageStatus(ME_HANDLE handle, int mode, ME_Callback reply, void* context)
{
	char cmd[AT_CMD_LEN];

	me_sprintf(cmd, "AT+ESMSS=%d\r", mode);
	return ME_Fill_FSM(handle, reply, context, ME_Set_ModemFunc_CB, cmd, NULL, SHORT_TIMEOUT_ELAPSE);
}

//AT+CSCA
NTSTATUS ME_Set_MessageCA(ME_HANDLE handle, const char* sa, ME_Callback reply, void* context)
{
	char cmd[AT_CMD_LEN];

	me_sprintf(cmd, "AT+CSCA=\"%s\"\r", sa);

	return ME_Fill_FSM(handle, reply, context, ME_Set_ModemFunc_CB, cmd, NULL, SHORT_TIMEOUT_ELAPSE);
}

//AT+CPBS=?
NTSTATUS ME_Get_PbMemType(ME_HANDLE handle, ME_Callback reply, void* context)
{
	const char *cmd = "AT+CPBS=?\r";
	const char *exCmd = "+CPBS";

	return ME_Fill_FSM(handle, reply, context, ME_Get_PbMemType_CB, cmd, exCmd, SHORT_TIMEOUT_ELAPSE);

}

//AT+CPBS=
NTSTATUS ME_Set_PbMemType(ME_HANDLE handle, const char *mem, ME_Callback reply, void* context)
{
	char cmd[AT_CMD_LEN];
	
	me_sprintf(cmd, "AT+CPBS=\"%s\"\r", mem); 
	return ME_Fill_FSM(handle, reply, context, ME_Set_ModemFunc_CB, cmd, NULL, SHORT_TIMEOUT_ELAPSE);
	
}

//AT+CPBS?
NTSTATUS ME_Get_PbMemState(ME_HANDLE handle, ME_Callback reply, void* context)
{
	const char *cmd = "AT+CPBS?\r";
	const char *exCmd = "+CPBS";
	
	return ME_Fill_FSM(handle, reply, context, ME_Get_PbMemState_CB, cmd, exCmd, SHORT_TIMEOUT_ELAPSE);
}

//AT+CPBR=?
NTSTATUS ME_Get_PbMemInfo(ME_HANDLE handle, ME_Callback reply, void* context)
{
	const char *cmd = "AT+CPBR=?\r";
	const char *exCmd = "+CPBR";
	
	return ME_Fill_FSM(handle, reply, context, ME_Get_PbMemInfo_CB, cmd, exCmd, SHORT_TIMEOUT_ELAPSE);

}

//AT+CPBR=
NTSTATUS ME_Read_Pbk(ME_HANDLE handle, int idx, ME_Callback reply, void* context)
{
	char cmd[AT_CMD_LEN];
	const char *exCmd = "+CPBR";
	
	me_sprintf(cmd, "AT+CPBR=%d\r", idx); 
	return ME_Fill_FSM(handle, reply, context, ME_Read_Pbk_CB, cmd, exCmd, SHORT_TIMEOUT_ELAPSE);
}

//AT+CPBW=
NTSTATUS ME_Write_Pbk(ME_HANDLE handle, int idx, const char *str, ME_Callback reply, void* context)
{
	char cmd[AT_CMD_LEN];
	
	if(idx != -1)
		me_sprintf(cmd, "AT+CPBW=%d\r", idx);
	else
		me_sprintf(cmd, "AT+CPBW=%s\r", str);

	return ME_Fill_FSM(handle, reply, context, ME_Set_ModemFunc_CB, cmd, NULL, SHORT_TIMEOUT_ELAPSE);
}
/*
//AT+CIMI?
NTSTATUS ME_Get_UserIdentity(ME_HANDLE handle, ME_Callback reply, void* context)
{
	const char *cmd = "AT+CIMI?\r";
	const char *exCmd = "+CIMI";
	
	return ME_Fill_FSM(handle, reply, context, ME_Get_UserIdentity_CB, cmd, exCmd, SHORT_TIMEOUT_ELAPSE);

}
*/
//AT+ERAT
NTSTATUS ME_Query_RATnPS(ME_HANDLE handle, ME_Callback reply, void* context)
{
	const char *cmd = "AT+ERAT?\r";
	const char *exCmd = "+ERAT";
	
	return ME_Fill_FSM(handle, reply, context, ME_Query_RATnPS_CB, cmd, exCmd, LONG_TIMEOUT_ELAPSE);
}

//AT+CSCS=
NTSTATUS ME_Set_CharacterSet(ME_HANDLE handle, const char *charset, ME_Callback reply, void* context)
{
	char cmd[AT_CMD_LEN];
	const char *exCmd = "+CSCS";
	
	me_sprintf(cmd, "AT+CSCS=\"%s\"\r", charset); 
	return ME_Fill_FSM(handle, reply, context, ME_Set_ModemFunc_CB, cmd, exCmd, SHORT_TIMEOUT_ELAPSE);	
}

//AT
NTSTATUS ME_Query_Connection(ME_HANDLE handle, ME_Callback reply, void* context)
{
	return ME_Fill_FSM(handle, reply, context, ME_Set_ModemFunc_CB, "AT\r", NULL, SHORT_TIMEOUT_ELAPSE);	
}

//ATH
NTSTATUS ME_Terminate_Call(ME_HANDLE handle, ME_Callback reply, void* context)
{
	return ME_Fill_FSM(handle, reply, context, ME_Set_ModemFunc_CB, "ATH\r", NULL, SHORT_TIMEOUT_ELAPSE);
}

//ATD
NTSTATUS ME_MakeMO_Call(ME_HANDLE handle, const char *number, ME_Callback reply, void* context)
{
	char cmd[AT_CMD_LEN];
	me_sprintf(cmd,"ATD%s;\r", number);

	return ME_Fill_FSM(handle, reply, context, ME_Set_ModemFunc_CB, cmd, NULL, SHORT_TIMEOUT_ELAPSE);	
}

//ATA
NTSTATUS ME_AnswerMT_Call(ME_HANDLE handle, ME_Callback reply, void* context)
{
	return ME_Fill_FSM(handle, reply, context, ME_Set_ModemFunc_CB, "ATA\r", NULL, SHORT_TIMEOUT_ELAPSE);
}

//AT+EIND
NTSTATUS ME_Get_IndicationCtrl(ME_HANDLE handle, ME_Callback reply, void* context)
{
	const char *cmd   = "AT+EIND\r";
	const char *exCmd = "+EIND";

	return ME_Fill_FSM(handle, reply, context, ME_Get_IndicationCtrl_CB, cmd, exCmd, SHORT_TIMEOUT_ELAPSE);

}

//AT+EIND=
NTSTATUS ME_Set_IndicationCtrl(ME_HANDLE handle, int ind, ME_Callback reply, void* context)
{
	char cmd[AT_CMD_LEN];
	
	me_sprintf(cmd, "AT+EIND=%d\r", ind); 
	return ME_Fill_FSM(handle, reply, context, ME_Set_ModemFunc_CB, cmd, NULL, SHORT_TIMEOUT_ELAPSE);

}

//AT+CCLK
NTSTATUS ME_Set_TargetTime(ME_HANDLE handle, const char *time, ME_Callback reply, void* context)
{
	char cmd[AT_CMD_LEN];
	
	me_sprintf(cmd, "AT+CCLK=\"%s\"\r", time); 
	return ME_Fill_FSM(handle, reply, context, ME_Set_ModemFunc_CB, cmd, NULL, SHORT_TIMEOUT_ELAPSE);	

}

//AT+CMUT?
NTSTATUS ME_Get_MuteCtrl(ME_HANDLE handle, ME_Callback reply, void* context)
{
	const char *cmd = "AT+CMUT?\r";
	const char *exCmd = "+CMUT";

	return ME_Fill_FSM(handle, reply, context, ME_Get_MuteCtrl_CB, cmd, exCmd, SHORT_TIMEOUT_ELAPSE);
}

//AT+CLIP=
NTSTATUS ME_Set_CLIP(ME_HANDLE handle, int mode, ME_Callback reply, void* context)
{
	char cmd[AT_CMD_LEN];
	
	me_sprintf(cmd, "AT+CLIP=%d\r", mode); 
	return ME_Fill_FSM(handle, reply, context, ME_Set_ModemFunc_CB, cmd, NULL, SHORT_TIMEOUT_ELAPSE);	
}

//AT+CLIR=
NTSTATUS ME_Set_CLIR(ME_HANDLE handle, int mode, ME_Callback reply, void* context)
{
	char cmd[AT_CMD_LEN];
	
	me_sprintf(cmd, "AT+CLIR=%d\r", mode); 
	return ME_Fill_FSM(handle, reply, context, ME_Set_ModemFunc_CB, cmd, NULL, SHORT_TIMEOUT_ELAPSE);	
}

//AT+CLIR?
NTSTATUS ME_Get_CLIR(ME_HANDLE handle, ME_Callback reply, void* context)
{
	const char *cmd = "AT+ECUSD=1,1,\"*#31#\"\r"; //"AT+CLIR?\r"
	const char *exCmd = "+CLIR";

	return ME_Fill_FSM(handle, reply, context, ME_Get_CLIR_CB, cmd, exCmd, SHORT_TIMEOUT_ELAPSE);	
}

//AT+CVIB?
NTSTATUS ME_Get_VibratorMode(ME_HANDLE handle, ME_Callback reply, void* context)
{
	const char *cmd = "AT+CVIB?\r";
	const char *exCmd = "+CVIB";
	
	return ME_Fill_FSM(handle, reply, context, ME_Get_VibratorMode_CB, cmd, exCmd, SHORT_TIMEOUT_ELAPSE);	
}

//AT+CVIB=
NTSTATUS ME_Set_VibratorMode(ME_HANDLE handle, int mode, ME_Callback reply, void* context)
{
	char cmd[AT_CMD_LEN];
	
	me_sprintf(cmd, "AT+CVIB=%d\r", mode); 
	return ME_Fill_FSM(handle, reply, context, ME_Set_ModemFunc_CB, cmd, NULL, SHORT_TIMEOUT_ELAPSE);	
}

//AT+CMUT=
NTSTATUS ME_Set_MuteControl(ME_HANDLE handle, int mode, ME_Callback reply, void* context)
{
	char cmd[AT_CMD_LEN];
	
	me_sprintf(cmd, "AT+CMUT=%d\r", mode); 
	return ME_Fill_FSM(handle, reply, context, ME_Set_ModemFunc_CB, cmd, NULL, SHORT_TIMEOUT_ELAPSE);	
}

//AT+CRSL?
NTSTATUS ME_Get_RingLevel(ME_HANDLE handle, ME_Callback reply, void* context)
{
	const char *cmd = "AT+CRSL?\r";
	const char *exCmd = "+CRSL";
	
	return ME_Fill_FSM(handle, reply, context, ME_Get_RingLevel_CB, cmd, exCmd, SHORT_TIMEOUT_ELAPSE);	
}

//AT+CRSL=
NTSTATUS ME_Set_RingLevel(ME_HANDLE handle, int level, ME_Callback reply, void* context)
{
	char cmd[AT_CMD_LEN];
	
	me_sprintf(cmd, "AT+CRSL=%d\r", level); 
	return ME_Fill_FSM(handle, reply, context, ME_Set_ModemFunc_CB, cmd, NULL, SHORT_TIMEOUT_ELAPSE);	
}

//AT+CLVL?
NTSTATUS ME_Get_SpeakerVolume(ME_HANDLE handle, ME_Callback reply, void* context)
{
	const char *cmd = "AT+CLVL?\r";
	const char *exCmd = "+CLVL";
	
	return ME_Fill_FSM(handle, reply, context, ME_Get_SpeakerVolume_CB, cmd, exCmd, SHORT_TIMEOUT_ELAPSE);	
}

//AT+CLVL=
NTSTATUS ME_Set_SpeakerVolume(ME_HANDLE handle, int level, ME_Callback reply, void* context)
{
	char cmd[AT_CMD_LEN];
	
	me_sprintf(cmd, "AT+CLVL=%d\r", level); 
	return ME_Fill_FSM(handle, reply, context, ME_Set_ModemFunc_CB, cmd, NULL, SHORT_TIMEOUT_ELAPSE);	
}

//AT+CUSD=
NTSTATUS ME_Set_USSDSessionControl(ME_HANDLE handle, int n, const char *str, int dcs, ME_Callback reply, void* context)
{
	char cmd[AT_CMD_LEN];
	
	if(str != NULL)
		me_sprintf(cmd, "AT+CUSD=%d,\"%s\",%d\r", n, str, dcs); 
	else
		me_sprintf(cmd, "AT+CUSD=%d\r", n);

	return ME_Fill_FSM(handle, reply, context, ME_Set_ModemFunc_CB, cmd, NULL, SHORT_TIMEOUT_ELAPSE);	
}


//AT+ESWM=
NTSTATUS ME_Set_SwitchMode(ME_HANDLE handle, int op, int mode, ME_Callback reply, void *context)
{
	char cmd[AT_CMD_LEN];

	if(mode > -1)
		me_sprintf(cmd, "AT+ESWM=%d,%d\r", op, mode);
	else 
		me_sprintf(cmd, "AT+ESWM=%d\r", op);

	return ME_Fill_FSM(handle, reply, context, ME_Set_ModemFunc_CB, cmd, NULL, SHORT_TIMEOUT_ELAPSE);	
}

//AT+ERAT=
NTSTATUS ME_Set_NetworkType(ME_HANDLE handle, int type, ME_Callback reply, void* context)
{
	char cmd[AT_CMD_LEN];
	//	20120222 by foil - support prefer rat - lazy revise
//	me_sprintf(cmd, "AT+ERAT=%d\r", type);
	if (type == 0)		//	2G
		me_sprintf(cmd, "AT+ERAT=0,0\r");
	else if (type == 1)	//	3G
		me_sprintf(cmd, "AT+ERAT=1,0\r");
	else if (type == 2)	//	auto - no prefer
		me_sprintf(cmd, "AT+ERAT=2,0\r");
//	else if (type == 3)	//	prefer 2G
//		me_sprintf(cmd, "AT+ERAT=2,1\r");
	else if (type == 3)	//	prefer 3G
		me_sprintf(cmd, "AT+ERAT=2,2\r");

	return ME_Fill_FSM(handle, reply, context, ME_Set_ModemFunc_CB, cmd, NULL, LONG_TIMEOUT_ELAPSE);	
}


//AT+EPIN2?
NTSTATUS ME_Query_PIN2(ME_HANDLE handle, ME_Callback reply, void* context)
{
	const char *cmd = "AT+EPIN2?\r";
	const char *exCmd = "+EPIN2";

 	return ME_Fill_FSM(handle, reply, context, ME_Query_PIN2_CB, cmd, exCmd, SHORT_TIMEOUT_ELAPSE);	
}

//AT+EPIN2=
NTSTATUS ME_Validate_PIN2(ME_HANDLE handle, const char *puk2, const char *new_pin2, ME_Callback reply, void* context)
{
	char cmd[AT_CMD_LEN];

	if(new_pin2 != NULL)
		me_sprintf(cmd, "AT+EPIN2=\"%s\",\"%s\"\r", puk2, new_pin2);
	else
		me_sprintf(cmd, "AT+EPIN2=\"%s\"\r", puk2);

	return ME_Fill_FSM(handle, reply, context, ME_Set_ModemFunc_CB, cmd, NULL, SHORT_TIMEOUT_ELAPSE);	
}

//AT+CAOC=
NTSTATUS ME_Set_CallMeterValue(ME_HANDLE handle, int mode, ME_Callback reply, void* context)
{
	char cmd[AT_CMD_LEN];
	const char *exCmd = "+CAOC";
	int ret;

	me_sprintf(cmd, "AT+CAOC=%d\r", mode);
	if(mode == 0)
		ret = ME_Fill_FSM(handle, reply, context, ME_Set_CallMeterValue_CB, cmd, exCmd, SHORT_TIMEOUT_ELAPSE);	
	else
		ret = ME_Fill_FSM(handle, reply, context, ME_Set_ModemFunc_CB, cmd, NULL, SHORT_TIMEOUT_ELAPSE);

	return ret;
}

//AT+CCWE=
NTSTATUS ME_Set_CallMeterEvent(ME_HANDLE handle, int mode, ME_Callback reply, void* context)
{
	char cmd[AT_CMD_LEN];
	
	me_sprintf(cmd, "AT+CCWE=%d\r", mode);
	return ME_Fill_FSM(handle, reply, context, ME_Set_ModemFunc_CB, cmd, NULL, SHORT_TIMEOUT_ELAPSE);
}

//AT+CACM?
NTSTATUS ME_Get_ACMValue(ME_HANDLE handle, ME_Callback reply, void* context)
{
	const char *cmd = "AT+CACM?\r";
	const char *exCmd = "+CACM";

	return ME_Fill_FSM(handle, reply, context, ME_Get_ACMValue_CB, cmd, exCmd, SHORT_TIMEOUT_ELAPSE);
}

//AT+CACM=
NTSTATUS ME_Reset_ACMValue(ME_HANDLE handle, const char *pwd, ME_Callback reply, void* context)
{
	char cmd[AT_CMD_LEN];	
	me_sprintf(cmd, "AT+CACM=\"%s\"\r", pwd);
	
	return ME_Fill_FSM(handle, reply, context, ME_Set_ModemFunc_CB, cmd, NULL, SHORT_TIMEOUT_ELAPSE);
}

//AT+CAMM?
NTSTATUS ME_Get_MaxACMValue(ME_HANDLE handle, ME_Callback reply, void* context)
{
	const char *cmd = "AT+CAMM?\r";
	const char *exCmd = "+CAMM";
	
	return ME_Fill_FSM(handle, reply, context, ME_Get_MaxACMValue_CB, cmd, exCmd, SHORT_TIMEOUT_ELAPSE);
}

//AT+CAMM=
NTSTATUS ME_Set_MaxACMValue(ME_HANDLE handle, int maxacm, const char *pwd, ME_Callback reply, void* context)
{
	char cmd[AT_CMD_LEN];
	
	if(pwd == NULL)
		me_sprintf(cmd,"AT+CAMM=\"%06x\"\r", maxacm);
	else
		me_sprintf(cmd,"AT+CAMM=\"%06x\",\"%s\"\r", maxacm, pwd);
	
	return ME_Fill_FSM(handle, reply, context, ME_Set_ModemFunc_CB, cmd, NULL, SHORT_TIMEOUT_ELAPSE);
}

//AT+CPUC?
NTSTATUS ME_Get_PUCValue(ME_HANDLE handle, ME_Callback reply, void* context)
{
	const char *cmd = "AT+CPUC?\r";
	const char *exCmd = "+CPUC";

	return ME_Fill_FSM(handle, reply, context, ME_Get_PUCValue_CB, cmd, exCmd, SHORT_TIMEOUT_ELAPSE);
}

//AT+CPUC=
NTSTATUS ME_Set_PUCValue(ME_HANDLE handle, const char *currency, const char *ppu, const char *pwd, ME_Callback reply, void* context)
{
	char cmd[AT_CMD_LEN];
	
	if(pwd == NULL)
		me_sprintf(cmd,"AT+CPUC=\"%s\",\"%s\"\r", currency, ppu);
	else
		me_sprintf(cmd,"AT+CPUC=\"%s\",\"%s\",\"%s\"\r", currency, ppu, pwd);
	
	return ME_Fill_FSM(handle, reply, context, ME_Set_ModemFunc_CB, cmd, NULL, SHORT_TIMEOUT_ELAPSE);
}

//AT+CLCC
NTSTATUS ME_List_CurrentCall(ME_HANDLE handle, ME_Callback reply, void* context)
{
	const char *cmd = "AT+CLCC\r";
	const char *exCmd = "+CLCC";
	
	return ME_Fill_FSM(handle, reply, context, ME_List_CurrentCall_CB, cmd, exCmd, SHORT_TIMEOUT_ELAPSE);
}

//AT+CEER
NTSTATUS ME_Extend_ErrReport(ME_HANDLE handle, ME_Callback reply, void* context)
{
	const char *cmd = "AT+CEER\r";
	const char *exCmd = "+CEER";

	return ME_Fill_FSM(handle, reply, context, ME_Extend_ErrReport_CB, cmd, exCmd, SHORT_TIMEOUT_ELAPSE);
}

//AT+VTS
NTSTATUS ME_Transfer_DTMFTone(ME_HANDLE handle, char dtmf, ME_Callback reply, void* context)
{
	char cmd[AT_CMD_LEN];
	me_sprintf(cmd, "AT+VTS=%c\r", dtmf);	

	return ME_Fill_FSM(handle, reply, context, ME_Set_ModemFunc_CB, cmd, NULL, SHORT_TIMEOUT_ELAPSE);
}

//AT+CTFR=
NTSTATUS ME_Deflect_Call(ME_HANDLE handle, const char *num, int type, ME_Callback reply, void* context)
{
	char cmd[AT_CMD_LEN];

	if(type >= 0)
		me_sprintf(cmd, "AT+CTFR=\"%s\",%d\r", num, type);
	else
		me_sprintf(cmd, "AT+CTFR=\"%s\"\r", num);

	return ME_Fill_FSM(handle, reply, context, ME_Set_ModemFunc_CB, cmd, NULL, SHORT_TIMEOUT_ELAPSE); 
}

//AT+CHLD=
#define GET_BYTE(x, n) (((x)>>(n)*8) & 0xff)
NTSTATUS ME_Change_CallLine(ME_HANDLE handle, int code, int line, ME_Callback reply, void* context)
{
	char cmd[AT_CMD_LEN];

	int cd = GET_BYTE(code, 0);
	int ln = GET_BYTE(line, 0);
	
	if(ln > 0)
		me_sprintf(cmd, "AT+CHLD=%d%d", cd, ln);
	else
		me_sprintf(cmd, "AT+CHLD=%d", cd);
	
	for (int i=1; i<4; i++)
	{
		cd = GET_BYTE(code, i);
		ln = GET_BYTE(line, i);
		
		if (0 == cd)	// "+chld=0" must be in the lowest byte
			break;
		
		if (ln > 0)
			sprintf(cmd, "%s;+CHLD=%d%d", cmd, cd, ln);
		else
			sprintf(cmd, "%s;+CHLD=%d", cmd, cd);
	}
	
	strcat(cmd, "\r\n");

	return ME_Fill_FSM(handle, reply, context, ME_Set_ModemFunc_CB, cmd, NULL, SHORT_TIMEOUT_ELAPSE); 
}

//AT+CLCK= 0 or 1
NTSTATUS ME_Set_CallBarring(ME_HANDLE handle, const char *cmd, ME_Callback reply, void* context)
{
	return ME_Fill_FSM(handle, reply, context, ME_Set_FacLock_CB, cmd, NULL, LONG_TIMEOUT_ELAPSE);
}

//AT+CLCK=2
NTSTATUS ME_Get_CallBarring(ME_HANDLE handle, const char *cmd, ME_Callback reply, void* context)
{
	const char *exCmd = "+CLCK";
	return ME_Fill_FSM(handle, reply, context, ME_Set_FacLock_CB, cmd, exCmd, LONG_TIMEOUT_ELAPSE);	
}

//AT+CCFC= 0 or 1
NTSTATUS ME_Set_CallForward(ME_HANDLE handle, const char *cmd, ME_Callback reply, void* context)
{
	return ME_Fill_FSM(handle, reply, context, ME_Set_CallForward_CB, cmd, NULL, LONG_TIMEOUT_ELAPSE);
}

//AT+CCFC=2
NTSTATUS ME_Get_CallForward(ME_HANDLE handle, const char *cmd, ME_Callback reply, void* context)
{
	const char *exCmd = "+CCFC";
	return ME_Fill_FSM(handle, reply, context, ME_Set_CallForward_CB, cmd, exCmd, LONG_TIMEOUT_ELAPSE);	
}

//AT+CCWA= 0 or 1
NTSTATUS ME_Set_CallWaiting(ME_HANDLE handle, const char *cmd, ME_Callback reply, void* context)
{
	return ME_Fill_FSM(handle, reply, context, ME_Set_CallWaiting_CB, cmd, NULL, LONG_TIMEOUT_ELAPSE);
}

//AT+CCWA=2
NTSTATUS ME_Get_CallWaiting(ME_HANDLE handle, const char *cmd, ME_Callback reply, void* context)
{
	const char *exCmd = "+CCWA";
	return ME_Fill_FSM(handle, reply, context, ME_Set_CallWaiting_CB, cmd, exCmd, LONG_TIMEOUT_ELAPSE);	
}

//AT+CSSN=
NTSTATUS ME_Set_SSNotification(ME_HANDLE handle, int n, int m, ME_Callback reply, void* context)
{
	char cmd[AT_CMD_LEN];
	me_sprintf(cmd, "AT+CSSN=%d,%d\r", n, m);
	
	return ME_Fill_FSM(handle, reply, context, ME_Set_ModemFunc_CB, cmd, NULL, SHORT_TIMEOUT_ELAPSE); 
}


//AT+CRSM
NTSTATUS ME_Restricted_SIM(ME_HANDLE handle, int command, int fileid, int p1, int p2, int p3, ME_Callback reply, void* context)
{
	char cmd[AT_CMD_LEN];
	const char *exCmd = "+CRSM";

	if(p1 == -1)	
		me_sprintf(cmd,"AT+CRSM=%d,%d\r", command, fileid);
	else
		me_sprintf(cmd,"AT+CRSM=%d,%d,%d,%d,%d\r", command, fileid, p1, p2, p3);

	return ME_Fill_FSM(handle, reply, context, ME_Restricted_SIM_CB, cmd, exCmd, SHORT_TIMEOUT_ELAPSE); 	
}

//AT+CSCB?
NTSTATUS ME_Get_CBSInfo(ME_HANDLE handle, ME_Callback reply, void* context)
{
	const char *cmd = "AT+CSCB?\r";
	const char *exCmd = "+CSCB";
	
	return ME_Fill_FSM(handle, reply, context, ME_Get_CBSInfo_CB, cmd, exCmd, SHORT_TIMEOUT_ELAPSE);
}

//AT_CSCB=
NTSTATUS ME_Set_CBSInfo(ME_HANDLE handle, const char *cmd, ME_Callback reply, void* context)
{
	return ME_Fill_FSM(handle, reply, context, ME_Set_ModemFunc_CB, cmd, NULL, SHORT_TIMEOUT_ELAPSE);
}

//AT+ETWS?
NTSTATUS ME_Get_ETWSInfo(ME_HANDLE handle, ME_Callback reply, void* context)
{
	const char *cmd = "AT+ETWS?\r";
	const char *exCmd = "+ETWS";
	
	return ME_Fill_FSM(handle, reply, context, ME_Get_ETWSInfo_CB, cmd, exCmd, SHORT_TIMEOUT_ELAPSE);
}

//AT+ETWS=
NTSTATUS ME_Set_ETWSInfo(ME_HANDLE handle, int setting, ME_Callback reply, void* context)
{
	char cmd[AT_CMD_LEN];
	me_sprintf(cmd, "AT+ETWS=%d\r", setting);
	
	return ME_Fill_FSM(handle, reply, context, ME_Set_ModemFunc_CB, cmd, NULL, SHORT_TIMEOUT_ELAPSE);
}

NTSTATUS ME_Set_PSBearer(ME_HANDLE handle, int flag, ME_Callback reply, void* context)
{
	char cmd[AT_CMD_LEN];
	me_sprintf(cmd, "AT+PSBEARER=%d\r", flag);
	
	return ME_Fill_FSM(handle, reply, context, ME_Set_ModemFunc_CB, cmd, NULL, SHORT_TIMEOUT_ELAPSE);
}

//AT+EPIN1=
//AT+EPIN2=
NTSTATUS ME_UnblockPIN(ME_HANDLE handle, const char *puk, const char *pin, const char *type, ME_Callback reply, void* context)
{
	char cmd[AT_CMD_LEN];

	if(strcmp(type, "SC") == 0)
		me_sprintf(cmd, "AT+EPIN1=\"%s\",\"%s\"\r", puk, pin);
	else
		me_sprintf(cmd, "AT+EPIN2=\"%s\",\"%s\"\r", puk, pin);

	return ME_Fill_FSM(handle, reply, context, ME_Set_ModemFunc_CB, cmd, NULL, SHORT_TIMEOUT_ELAPSE);
}

#ifdef WIN32
//AT+STKTR=
NTSTATUS ME_STKResponse(ME_HANDLE handle, const char *str, ME_Callback reply, void* context)
{
	char cmd[AT_CMD_LEN];
	me_sprintf(cmd, "AT+STKTR=\"%s\"\r", str);

	return ME_Fill_FSM(handle, reply, context, ME_Set_ModemFunc_CB, cmd, NULL, SHORT_TIMEOUT_ELAPSE); 
}

//AT+STKENV=
NTSTATUS ME_STKEnvelope(ME_HANDLE handle, const char *str, ME_Callback reply, void* context)
{
	char cmd[AT_CMD_LEN];
	me_sprintf(cmd, "AT+STKENV=\"%s\"\r", str);
	
	return ME_Fill_FSM(handle, reply, context, ME_Set_ModemFunc_CB, cmd, NULL, SHORT_TIMEOUT_ELAPSE); 
}

//AT+STKCALL=
NTSTATUS ME_STKSetupCall(ME_HANDLE handle, int op, ME_Callback reply, void* context)
{
	char cmd[AT_CMD_LEN];
	me_sprintf(cmd, "AT+STKCALL=%d\r", op);
	
	return ME_Fill_FSM(handle, reply, context, ME_Set_ModemFunc_CB, cmd, NULL, SHORT_TIMEOUT_ELAPSE); 
}

//AT+STKSS=
NTSTATUS ME_STKSetupSS(ME_HANDLE handle, int op, ME_Callback reply, void* context)
{
	char cmd[AT_CMD_LEN];
	me_sprintf(cmd, "AT+STKSS=%d\r", op);
	
	return ME_Fill_FSM(handle, reply, context, ME_Set_ModemFunc_CB, cmd, NULL, SHORT_TIMEOUT_ELAPSE); 
}

//AT+STKUSSD=
NTSTATUS ME_STKSetupUSSD(ME_HANDLE handle, int op, ME_Callback reply, void* context)
{
	char cmd[AT_CMD_LEN];
	me_sprintf(cmd, "AT+STKUSSD=%d\r", op);
	
	return ME_Fill_FSM(handle, reply, context, ME_Set_ModemFunc_CB, cmd, NULL, SHORT_TIMEOUT_ELAPSE); 
}

//AT+STKSMS=
NTSTATUS ME_STKSendSMS(ME_HANDLE handle, int op, ME_Callback reply, void* context)
{
	char cmd[AT_CMD_LEN];
	me_sprintf(cmd, "AT+STKSMS=%d\r", op);
	
	return ME_Fill_FSM(handle, reply, context, ME_Set_ModemFunc_CB, cmd, NULL, SHORT_TIMEOUT_ELAPSE); 
}

//AT+STKDTMF=
NTSTATUS ME_STKSendDTMF(ME_HANDLE handle, int op, ME_Callback reply, void* context)
{
	char cmd[AT_CMD_LEN];
	me_sprintf(cmd, "AT+STKDTMF=%d\r", op);
	
	return ME_Fill_FSM(handle, reply, context, ME_Set_ModemFunc_CB, cmd, NULL, SHORT_TIMEOUT_ELAPSE); 
}

//AT+STKATCMD=
NTSTATUS ME_STKRunAtCmd(ME_HANDLE handle, int op, ME_Callback reply, void* context)
{
	char cmd[AT_CMD_LEN];
	me_sprintf(cmd, "AT+STKATCMD=%d\r", op);
	
	return ME_Fill_FSM(handle, reply, context, ME_Set_ModemFunc_CB, cmd, NULL, SHORT_TIMEOUT_ELAPSE); 
}

#endif //WIN32
/////////////////////////////////////////////////////URC////////////////////////////////////////////

void ME_RegisterURC(ME_HANDLE handle, int nID/*URC_ID*/, ME_Callback reply, void *context)
{
	PMEFSM pfsm;

	if(handle == NULL)
		return;
	pfsm = (PMEFSM)handle;

	for(int i=ID_ESIMS; i<ID_MAX; i++)
		pfsm->urclist[i].reply = reply;

}

/*
NTSTATUS ME_Recv_Data(NTSTATUS status, const PUCHAR pTxt, ULONG_PTR nLen, ME_HANDLE handle)
{
	if(STATUS_SUCCESS == status)
		DataCallBack(status, pTxt, (ULONG)nLen, handle);
	
	return status;
}*/

