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

#ifndef _ME_AT_PARSER_H
#define _ME_AT_PARSER_H

//AT parser
void ME_Common_Query_Parser(void *pfsm, void *pObj);
void ME_Query_ModemBearer_Parser(void *pfsm, void *pObj);
void ME_Query_BandInfo_Parser(void *pfsm, void *pObj);
void ME_Query_IMEI_Parser(void *pfsm, void *pObj);
void ME_Query_NetwrokReg_Parser(void *pfsm, void *pObj);
void ME_Query_PINRemainNum_Parser(void *pfsm, void *pObj);
void ME_Query_MSISDNInfo_Parser(void *pfsm, void *pObj);
void ME_Set_FacLock_Parser(void  *pfsm, void *pObj);
void ME_Query_OperatorList_Parser(void *pfsm, void *pObj);
void ME_Query_Operator_Parser(void *pfsm, void *pObj);
void ME_Query_PDPState_Paser(void *pfsm, void *pObj);
void ME_Query_PrcoCfg_Parser(void *pfsm, void *pObj);
void ME_Query_PDPAddr_Parser(void *pfsm, void *pObj);
void ME_Query_SignalQuality_Parser(void *pfsm, void *pObj);
void ME_Query_IMSI_Parser(void *pfsm, void *pObj);
void ME_Read_SMS_Parser(void *pfsm, void *pObj);
void ME_List_SMS_Parser(void *pfsm, void *pObj);
void ME_List_Provisioned_Contexts_Parser(void *pfsm, void *pObj);
void ME_Get_ServiceCenterAddress_Parser(void *pfsm, void *pObj);
void ME_Get_MessageMemoryState_Parser(void *pfsm, void *pObj);
void ME_Read_Transparent_SIMFile_Parser(void *pfsm, void *pObj);
void ME_Get_CurrentSMSMemType_Parser(void *pfsm, void *pObj);
void ME_Get_SMSMemIndex_Parser(void *pfsm, void *pObj);
void ME_Get_PbMemType_Parser(void *pfsm, void *pObj);
void ME_Get_PbMemState_Parser(void *pfsm, void *pObj);
void ME_Get_PbMemInfo_Parser(void *pfsm, void *pObj);
void ME_Read_Pbk_Parser(void *pfsm, void *pObj);
//void ME_Get_UserIdentity_Parser(void *pfsm, void *pObj);
void ME_Query_RATnPS_Parser(void *pfsm, void *pObj);
void ME_Get_CLIR_Parser(void *pfsm, void *pObj);
void ME_Query_PIN2_Parser(void *pfsm, void *pObj);
void ME_Get_PUCValue_Parser(void *pfsm, void *pObj);
void ME_List_CurrentCall_Parser(void *pfsm, void *pObj);
void ME_Extend_ErrReport_Parser(void *pfsm, void *pObj);
void ME_Set_CallForward_Parser(void *pfsm, void *pObj);
void ME_Set_CallWaiting_Parser(void *pfsm, void *pObj);
void ME_Restricted_SIM_Parser(void *pfsm, void *pObj);
void ME_Get_CBSInfo_Parser(void *pfsm, void *pObj);

//URC parser
int ME_URC_ESIMS_Parser(void *pData);
int ME_URC_CREG_Parser(void *pData);
int ME_URC_ECSQ_Parser(void *pData);
int ME_URC_CGREG_Parser(void *pData);
int ME_URC_CGEV_Parser(void *pData);
int ME_URC_EIND_Parser(void *pData);
int ME_URC_CMTI_Parser(void *pData);
int ME_URC_CMT_Parser(void *pData);
int ME_URC_CDS_Parser(void *pData);
int ME_URC_RING_Parser(void *pData);
int ME_URC_NOCA_Parser(void *pData);
int ME_URC_CSSI_Parser(void *pData);
int ME_URC_CSSU_Parser(void *pData);
int ME_URC_CUSD_Parser(void *pData);
int ME_URC_CCCM_Parser(void *pData);
int ME_URC_CCWV_Parser(void *pData);
int ME_URC_CCWA_Parser(void *pData);
int ME_URC_STKPCI_Parser(void *pData);
int ME_URC_PACSP_Parser(void *pData);
int ME_URC_ETWS_Parser(void *pData);
int ME_URC_PSBEARER_Parser(void* pData);
int ME_URC_CBM_Parser(void *pData);
int ME_URC_CIEV_Parser(void *pData);

#endif