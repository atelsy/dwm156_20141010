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

#ifndef _ME_FUNC_H
#define _ME_FUNC_H

#include "me_glbdef.h"
#include "me_osal.h"
	
//Mobile engine control
ME_HANDLE ME_Init(void* ioproxy);
BOOLEAN ME_DeInit(ME_HANDLE handle);
BOOLEAN	ME_Cancel(ME_HANDLE handle);

//register URC calback function.
void ME_RegisterURC(ME_HANDLE handle, int nID/*URC_ID*/, ME_Callback reply, void *context);
//receive IRQ data
//NTSTATUS ME_Recv_Data(NTSTATUS status, const PUCHAR pTxt, ULONG_PTR nLen, ME_HANDLE handle);

//AT command control
//AT+EPSB?
NTSTATUS ME_Query_ModemBearer(ME_HANDLE handle, ME_Callback reply, void* context);

//AT+EPBSE=?
NTSTATUS ME_Query_BandInfo(ME_HANDLE handle, ME_Callback reply, void* context);

//AT+CGSN
NTSTATUS ME_Query_IMEI(ME_HANDLE handle, ME_Callback reply, void* context);

//AT+CGMI
NTSTATUS ME_Query_FacInfo(ME_HANDLE handle, ME_Callback reply, void* context);

//AT+CGMM
NTSTATUS ME_Query_ModelInfo(ME_HANDLE handle, ME_Callback reply, void* context);

//AT+CGMR
NTSTATUS ME_Query_Revision(ME_HANDLE handle, ME_Callback reply, void* context);

/************ 20131109 by Zhuwei **********************************************/
//AT+VERNO
NTSTATUS ME_Query_VERNO(ME_HANDLE handle, ME_Callback reply, void* context);
/******************************************************************************/

//ATE0Q0V1
NTSTATUS ME_Set_ModemFunc(ME_HANDLE handle, ME_Callback reply, void* context);

//AT+CMEE=
NTSTATUS ME_Set_ErrorFmt(ME_HANDLE handle, int n, ME_Callback reply, void* context);

//AT+CMER
NTSTATUS ME_Set_CMER(ME_HANDLE handle, int mode, int keyp, int disp, 
					 int ind, int bfr, int tscrn, ME_Callback reply, void* context);

//AT+CREG=
NTSTATUS ME_Set_NetwrokReg(ME_HANDLE handle, int n, ME_Callback reply, void* context);

//AT+CREG?
NTSTATUS ME_Query_NetwrokReg(ME_HANDLE handle, ME_Callback reply, void* context);

//AT+CGREG=
NTSTATUS ME_Set_GPRSState(ME_HANDLE handle, int n, ME_Callback reply, void* context);

//AT+CGREG?
NTSTATUS ME_Query_GPRSState(ME_HANDLE handle, ME_Callback reply, void* context);

//AT+ECSQ=
NTSTATUS ME_Set_SignalLevel(ME_HANDLE handle, int n, ME_Callback reply, void* context);

//AT+ECSQ?
NTSTATUS ME_Query_SignalLevel(ME_HANDLE handle, ME_Callback reply, void* context);

//AT+CFUN=
NTSTATUS ME_Set_ModemFunctionality(ME_HANDLE handle, int fun, int rst, ME_Callback reply, void* context);

//AT+CFUN?
NTSTATUS ME_Query_ModemFunctionality(ME_HANDLE handle, ME_Callback reply, void* context);

//AT+CPIN=
NTSTATUS ME_Set_PIN(ME_HANDLE handle, char *pin, char *newpin, ME_Callback reply, void* context);

//AT+CPIN?
NTSTATUS ME_Query_PINState(ME_HANDLE handle, ME_Callback reply, void* context);

//AT+EPINC?
NTSTATUS ME_Query_PINRemainNum(ME_HANDLE handle, ME_Callback reply, void* context);

//AT+CIMI
NTSTATUS ME_Query_IMSI(ME_HANDLE handle, ME_Callback reply, void* context);

//AT+CNUM
NTSTATUS ME_Query_MSISDNInfo(ME_HANDLE handle, ME_Callback reply, void* context);

//AT+CPWD=
NTSTATUS ME_Set_Password(ME_HANDLE handle, char* fac, char *oldpwd, char *newpwd, ME_Callback reply, void* context);

//AT+CLCK=
NTSTATUS ME_Set_FacLock(ME_HANDLE handle, const char* fac, int mode, char *pwd, int classx, ME_Callback reply, void* context);

//AT+COPS=
NTSTATUS ME_Set_Operator(ME_HANDLE handle, int mode, int fmt, char *opern, int act, ME_Callback reply, void *context);

//AT+COPS?
NTSTATUS ME_Query_Operator(ME_HANDLE handle, ME_Callback reply, void* context);

//AT+COPS=?
NTSTATUS ME_Query_OperatorList(ME_HANDLE handle, ME_Callback reply, void *context);

//AT+CGDCONT=
NTSTATUS ME_Set_PDPContext(ME_HANDLE handle, PDPCONTEXT *pdp, ME_Callback reply, void* context);

//AT+CGQREQ=
NTSTATUS ME_Set_ServiceProQuality(ME_HANDLE handle, SERVICEPRO *pro, ME_Callback reply, void* context);

//AT+CGQMIN=
NTSTATUS ME_Set_MiniAcceptablePro(ME_HANDLE handle, SERVICEPRO *pro, ME_Callback reply, void* context);

//AT+CGEREP=
NTSTATUS ME_Set_GPRSEventReporting(ME_HANDLE handle, int mode, int bfr, ME_Callback reply, void* context);

//AT+CGPRCO=
NTSTATUS ME_Set_PrcoCfg(ME_HANDLE handle, PRCOCFG *pCfg, ME_Callback reply, void* context);

//AT+CGPRCO?
NTSTATUS ME_Query_PrcoCfg(ME_HANDLE handle, ME_Callback reply, void* context);

//AT+CGACT=
NTSTATUS ME_Set_PDPState(ME_HANDLE handle, int state, int cid, ME_Callback reply, void* context);

//AT+CGACT?
NTSTATUS ME_Query_PDPState(ME_HANDLE handle, ME_Callback reply, void* context);

//AT+CGPADDR=
NTSTATUS ME_Query_PDPAddr(ME_HANDLE handle, int cid, ME_Callback reply, void* context);

//AT+CGDATA=
NTSTATUS ME_Set_DataState(ME_HANDLE handle, char *L2P, int cid, ME_Callback reply, void* context);

//AT+CSQ
NTSTATUS ME_Query_SignalQuality(ME_HANDLE handle, ME_Callback reply, void* context);

//AT+CPOL=
NTSTATUS ME_Set_PreOperatorList(ME_HANDLE handle, PREOPERLIST *oplist, ME_Callback reply, void* context);

//AT+ESPN?
NTSTATUS ME_Query_SIMSPN(ME_HANDLE handle, ME_Callback reply, void* context);

//AT+ICCID?
NTSTATUS ME_Query_SIMICCID(ME_HANDLE handle, ME_Callback reply, void* context);

//AT+ESIMS=
NTSTATUS ME_Set_SIMUrcState(ME_HANDLE handle, int mode, ME_Callback reply, void* context);

//AT+ESIMS?
NTSTATUS ME_Query_SIMStatus(ME_HANDLE handle, ME_Callback reply, void* context);

//AT+CGATT=
NTSTATUS ME_Set_PSState(ME_HANDLE handle, int state, ME_Callback reply, void* context);

//AT+CGATT?
NTSTATUS ME_Query_PSState(ME_HANDLE handle, ME_Callback reply, void* context);

//AT+ESLN
NTSTATUS ME_Sync_CallLog(ME_HANDLE handle, ME_Callback reply, void* context);

//AT+MBPC=?
NTSTATUS ME_List_Provisioned_Contexts(ME_HANDLE handle, ME_Callback reply, void* context);

//AT+MBPC=
NTSTATUS ME_Write_Provisioned_Contexts(ME_HANDLE handle, SUB_WWANCONTEXT *pro, ME_Callback reply, void* context);

//AT+MBPC?
NTSTATUS ME_Read_Provisioned_Contexts(ME_HANDLE handle, ME_Callback reply, void* context);

//AT+EGTYPE=
NTSTATUS ME_Set_GPRSAttachMode(ME_HANDLE handle, int mode, int act, ME_Callback reply, void* context);

//AT+EIND?
NTSTATUS ME_Query_Indication(ME_HANDLE handle, ME_Callback reply, void* context);

//AT+CSCA=
NTSTATUS ME_Set_ServiceCenterAddress(ME_HANDLE handle, SMCS_ADDR *addr, ME_Callback reply, void* context);

//AT+CSCA?
NTSTATUS ME_Get_ServiceCenterAddress(ME_HANDLE handle, ME_Callback reply, void* context);

//AT+CNMI=
NTSTATUS ME_Set_MessageIndication(ME_HANDLE handle, SMS_INDICATION *pIndt, ME_Callback reply, void* context);

//AT+CPMS=
NTSTATUS ME_Get_MessageMemoryState(ME_HANDLE handle, const char *mem12, const char *mem3, ME_Callback reply, void* context);

//AT+CMGD=
NTSTATUS ME_Delete_Message(ME_HANDLE handle, int idx, int flag, ME_Callback reply, void* context); 

//AT+CMGW=
NTSTATUS ME_Write_Message(ME_HANDLE handle, const char *pdu, int size, int stat, ME_Callback reply, void* context);

// AT+CMGS=
NTSTATUS ME_Send_Message(ME_HANDLE handle, const char *pdu, int size, ME_Callback reply, void* context);

//AT+CMGR=
NTSTATUS ME_Read_Message(ME_HANDLE handle, int idx, ME_Callback reply, void* context);

//AT+CMGL=
NTSTATUS ME_List_Message(ME_HANDLE handle, int stat, ME_Callback reply, void* context);

//AT+CAPL
NTSTATUS ME_Abort_SearchOperator(ME_HANDLE handle, ME_Callback reply, void* context);

//AT+CRSM
NTSTATUS ME_Read_Transparent_SIMFile(ME_HANDLE handle, int id, int len, ME_Callback reply, void* context);

//AT+ESLP
NTSTATUS ME_Set_SleepMode(ME_HANDLE handle, BOOLEAN bEnable, ME_Callback reply, void* context);

//AT+CPMS?
NTSTATUS ME_Get_CurrentSMSMemType(ME_HANDLE handle, ME_Callback reply, void* context);

//AT+EQSI?
NTSTATUS ME_Get_SMSMemIndex(ME_HANDLE handle, const char *mem, ME_Callback reply, void* context);

//AT+CMGF=
NTSTATUS ME_Set_MessageFormat(ME_HANDLE handle, int mode, ME_Callback reply, void* context);

//AT+ESMSS
NTSTATUS ME_Set_MessageStatus(ME_HANDLE handle, int mode, ME_Callback reply, void* context);

//AT+CSCA
NTSTATUS ME_Set_MessageCA(ME_HANDLE handle, const char* sa, ME_Callback reply, void* context);

//AT+CPBS=?
NTSTATUS ME_Get_PbMemType(ME_HANDLE handle, ME_Callback reply, void* context);

//AT+CPBS=
NTSTATUS ME_Set_PbMemType(ME_HANDLE handle, const char *mem, ME_Callback reply, void* context);

//AT+CPBS?
NTSTATUS ME_Get_PbMemState(ME_HANDLE handle, ME_Callback reply, void* context);

//AT+CPBR=?
NTSTATUS ME_Get_PbMemInfo(ME_HANDLE handle, ME_Callback reply, void* context);

//AT+CPBR=
NTSTATUS ME_Read_Pbk(ME_HANDLE handle, int idx, ME_Callback reply, void* context);

//AT+CPBW=
NTSTATUS ME_Write_Pbk(ME_HANDLE handle, int idx, const char *str, ME_Callback reply, void* context);
/*
//AT+CIMI?
NTSTATUS ME_Get_UserIdentity(ME_HANDLE handle, ME_Callback reply, void* context);
*/
//AT+ERAT
NTSTATUS ME_Query_RATnPS(ME_HANDLE handle, ME_Callback reply, void* context);

//AT+CSCS=
NTSTATUS ME_Set_CharacterSet(ME_HANDLE handle, const char *charset, ME_Callback reply, void* context);

//AT
NTSTATUS ME_Query_Connection(ME_HANDLE handle, ME_Callback reply, void* context);

//ATH
NTSTATUS ME_Terminate_Call(ME_HANDLE handle, ME_Callback reply, void* context);

//AT+EIND
NTSTATUS ME_Get_IndicationCtrl(ME_HANDLE handle, ME_Callback reply, void* context);

//AT+EIND=
NTSTATUS ME_Set_IndicationCtrl(ME_HANDLE handle, int ind, ME_Callback reply, void* context);

//AT+CCLK
NTSTATUS ME_Set_TargetTime(ME_HANDLE handle, const char *time, ME_Callback reply, void* context);

//AT+CMUT?
NTSTATUS ME_Get_MuteCtrl(ME_HANDLE handle, ME_Callback reply, void* context);

//AT+CLIP
NTSTATUS ME_Set_CLIP(ME_HANDLE handle, int mode, ME_Callback reply, void* context);

//AT+CLIR=
NTSTATUS ME_Set_CLIR(ME_HANDLE handle, int mode, ME_Callback reply, void* context);

//AT+CLIR?
NTSTATUS ME_Get_CLIR(ME_HANDLE handle, ME_Callback reply, void* context);

//AT+CVIB?
NTSTATUS ME_Get_VibratorMode(ME_HANDLE handle, ME_Callback reply, void* context);

//AT+CVIB=
NTSTATUS ME_Set_VibratorMode(ME_HANDLE handle, int mode, ME_Callback reply, void* context);

//AT+CMUT=
NTSTATUS ME_Set_MuteControl(ME_HANDLE handle, int mode, ME_Callback reply, void* context);

//AT+CRSL?
NTSTATUS ME_Get_RingLevel(ME_HANDLE handle, ME_Callback reply, void* context);

//AT+CRSL=
NTSTATUS ME_Set_RingLevel(ME_HANDLE handle, int level, ME_Callback reply, void* context);

//AT+CLVL?
NTSTATUS ME_Get_SpeakerVolume(ME_HANDLE handle, ME_Callback reply, void* context);

//AT+CLVL=
NTSTATUS ME_Set_SpeakerVolume(ME_HANDLE handle, int level, ME_Callback reply, void* context);

//AT+CUSD=
NTSTATUS ME_Set_USSDSessionControl(ME_HANDLE handle, int n, const char *str, int dcs, ME_Callback reply, void* context);
//AT+ESWM=
NTSTATUS ME_Set_SwitchMode(ME_HANDLE handle, int op, int mode, ME_Callback reply, void *context);

//AT+ERAT=
NTSTATUS ME_Set_NetworkType(ME_HANDLE handle, int type, ME_Callback reply, void* context);
//AT+EPIN2?
NTSTATUS ME_Query_PIN2(ME_HANDLE handle, ME_Callback reply, void* context);

//AT+EPIN2=
NTSTATUS ME_Validate_PIN2(ME_HANDLE handle, const char *puk2, const char *new_pin2, ME_Callback reply, void* context);

//ATD
NTSTATUS ME_MakeMO_Call(ME_HANDLE handle, const char *number, ME_Callback reply, void* context);

//ATA
NTSTATUS ME_AnswerMT_Call(ME_HANDLE handle, ME_Callback reply, void* context);

//AT+CAOC=
NTSTATUS ME_Set_CallMeterValue(ME_HANDLE handle, int mode, ME_Callback reply, void* context);

//AT+CCWE=
NTSTATUS ME_Set_CallMeterEvent(ME_HANDLE handle, int mode, ME_Callback reply, void* context);

//AT+CACM?
NTSTATUS ME_Get_ACMValue(ME_HANDLE handle, ME_Callback reply, void* context);

//AT+CACM=
NTSTATUS ME_Reset_ACMValue(ME_HANDLE handle, const char *pwd, ME_Callback reply, void* context);

//AT+CAMM?
NTSTATUS ME_Get_MaxACMValue(ME_HANDLE handle, ME_Callback reply, void* context);

//AT+CAMM=
NTSTATUS ME_Set_MaxACMValue(ME_HANDLE handle, int maxacm, const char *pwd, ME_Callback reply, void* context);

//AT+CPUC?
NTSTATUS ME_Get_PUCValue(ME_HANDLE handle, ME_Callback reply, void* context);

//AT+CPUC=
NTSTATUS ME_Set_PUCValue(ME_HANDLE handle, const char *currency, const char *ppu, const char *pwd, ME_Callback reply, void* context);

//AT+CLCC
NTSTATUS ME_List_CurrentCall(ME_HANDLE handle, ME_Callback reply, void* context);

//AT+CEER
NTSTATUS ME_Extend_ErrReport(ME_HANDLE handle, ME_Callback reply, void* context);

//AT+VTS
NTSTATUS ME_Transfer_DTMFTone(ME_HANDLE handle, char demf, ME_Callback reply, void* context);

//AT+CTFR=
NTSTATUS ME_Deflect_Call(ME_HANDLE handle, const char *num, int type, ME_Callback reply, void* context);

//AT+CHLD=
NTSTATUS ME_Change_CallLine(ME_HANDLE handle, int code, int line, ME_Callback reply, void* context);

//AT+CLCK= 0 or 1
NTSTATUS ME_Set_CallBarring(ME_HANDLE handle, const char *cmd, ME_Callback reply, void* context);

//AT+CLCK=2
NTSTATUS ME_Get_CallBarring(ME_HANDLE handle, const char *cmd, ME_Callback reply, void* context);

//AT+CCFC= 0 or 1
NTSTATUS ME_Set_CallForward(ME_HANDLE handle, const char *cmd, ME_Callback reply, void* context);

//AT+CCFC=2
NTSTATUS ME_Get_CallForward(ME_HANDLE handle, const char *cmd, ME_Callback reply, void* context);

//AT+CCWA= 0 or 1
NTSTATUS ME_Set_CallWaiting(ME_HANDLE handle, const char *cmd, ME_Callback reply, void* context);

//AT+CCWA=2
NTSTATUS ME_Get_CallWaiting(ME_HANDLE handle, const char *cmd, ME_Callback reply, void* context);

//AT+CSSN=
NTSTATUS ME_Set_SSNotification(ME_HANDLE handle, int n, int m, ME_Callback reply, void* context);

//AT+CRSM
NTSTATUS ME_Restricted_SIM(ME_HANDLE handle, int command, int fileid, int p1, int p2, int p3, ME_Callback reply, void* context);

//AT+CSCB?
NTSTATUS ME_Get_CBSInfo(ME_HANDLE handle, ME_Callback reply, void* context);

//AT+CSCB=
NTSTATUS ME_Set_CBSInfo(ME_HANDLE handle, const char *cmd, ME_Callback reply, void* context);

//AT+ETWS?
NTSTATUS ME_Get_ETWSInfo(ME_HANDLE handle, ME_Callback reply, void* context);

//AT+ETWS=
NTSTATUS ME_Set_ETWSInfo(ME_HANDLE handle, int setting, ME_Callback reply, void* context);

//AT+PSBEARER
NTSTATUS ME_Set_PSBearer(ME_HANDLE handle, int flag, ME_Callback reply, void* context);

//AT+EPIN1=
//AT+EPIN2=
NTSTATUS ME_UnblockPIN(ME_HANDLE handle, const char *puk, const char *pin, const char *type, ME_Callback reply, void* context);

#ifdef WIN32
//AT+STKTR=
NTSTATUS ME_STKResponse(ME_HANDLE handle, const char *str, ME_Callback reply, void* context);

//AT+STKENV=
NTSTATUS ME_STKEnvelope(ME_HANDLE handle, const char *str, ME_Callback reply, void* context);

//AT+STKCALL=
NTSTATUS ME_STKSetupCall(ME_HANDLE handle, int op, ME_Callback reply, void* context);

//AT+STKSS=
NTSTATUS ME_STKSetupSS(ME_HANDLE handle, int op, ME_Callback reply, void* context);

//AT+STKUSSD=
NTSTATUS ME_STKSetupUSSD(ME_HANDLE handle, int op, ME_Callback reply, void* context);

//AT+STKSMS=
NTSTATUS ME_STKSendSMS(ME_HANDLE handle, int op, ME_Callback reply, void* context);

//AT+STKDTMF=
NTSTATUS ME_STKSendDTMF(ME_HANDLE handle, int op, ME_Callback reply, void* context);

//AT+STKATCMD=
NTSTATUS ME_STKRunAtCmd(ME_HANDLE handle, int op, ME_Callback reply, void* context);

#endif //WIN32

#endif