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

#ifndef _ME_GLBDEF_CB_H
#define _ME_GLBDEF_CB_H

typedef void (*PARSERFUNC) (void *pfsm, void *pObj);
int ME_URC_Parser(void *pData);

void ME_Query_ModemBearer_CB(void *pData);
void ME_Query_BandInfo_CB(void *pData);
void ME_Query_IMEI_CB(void *pData);
void ME_Query_FacInfo_CB(void *pData);
void ME_Query_ModelInfo_CB(void *pData);	
void ME_Query_Revision_CB(void *pData);
/************ 20131109 by Zhuwei **********************************************/
void ME_Query_GetVERNO_CB(void *pData);
/******************************************************************************/
void ME_Set_ModemFunc_CB(void *pData);
void ME_Query_NetwrokReg_CB(void *pData);
void ME_Query_GPRSState_CB(void *pData);
void ME_Query_SignalLevel_CB(void *pData);
void ME_Query_ModemFunctionality_CB(void *pData);
void ME_Query_PINState_CB(void *pData);	
void ME_Query_PINRemainNum_CB(void *pData);
void ME_Query_IMSI_CB(void *pData);
void ME_Query_MSISDNInfo_CB(void *pData);
void ME_Set_FacLock_CB(void *pData);
void ME_Query_Operator_CB(void *pData);
void ME_Query_OperatorList_CB(void *pData);
void ME_Query_PrcoCfg_CB(void *pData);
void ME_Query_PDPState_CB(void *pData);
void ME_Query_PDPAddr_CB(void *pData);
void ME_Query_SignalQuality_CB(void *pData);
void ME_Query_SIMSPN_CB(void *pData);
void ME_Query_SIMICCID_CB(void *pData);
void ME_Query_PSState_CB(void *pData);
void ME_List_Provisioned_Contexts_CB(void *pData);
void ME_Read_Provisioned_Contexts_CB(void *pData);
void ME_Query_Indication_CB(void *pData);
void ME_Get_ServiceCenterAddress_CB(void *pData);
void ME_Get_MessageMemoryState_CB(void *pData);
void ME_Send_Message_CB(void *pData);
void ME_Write_Message_CB(void *pData);
void ME_Read_Message_CB(void *pData);
void ME_List_Message_CB(void *pData);
void ME_Read_Transparent_SIMFile_CB(void *pData);
void ME_Get_CurrentSMSMemType_CB(void *pData);
void ME_Get_SMSMemIndex_CB(void *pData);
void ME_Get_PbMemType_CB(void *pData);
void ME_Get_PbMemState_CB(void *pData);
void ME_Get_PbMemInfo_CB(void *pData);
void ME_Read_Pbk_CB(void *pData);
//void ME_Get_UserIdentity_CB(void *pData);
void ME_Query_RATnPS_CB(void *pData);
void ME_Get_IndicationCtrl_CB(void *pData);
void ME_Get_MuteCtrl_CB(void *pData);
void ME_Get_CLIR_CB(void *pData);
void ME_Get_VibratorMode_CB(void *pData);
void ME_Get_RingLevel_CB(void *pData);
void ME_Get_SpeakerVolume_CB(void *pData);
void ME_Query_PIN2_CB(void *pData);
void ME_Set_CallMeterValue_CB(void *pData);
void ME_Get_ACMValue_CB(void *pData);
void ME_Get_MaxACMValue_CB(void *pData);
void ME_Get_PUCValue_CB(void *pData);
void ME_List_CurrentCall_CB(void *pData);
void ME_Extend_ErrReport_CB(void *pData);
void ME_Set_CallForward_CB(void *pData);
void ME_Set_CallWaiting_CB(void *pData);
void ME_Restricted_SIM_CB(void *pData);
void ME_Get_CBSInfo_CB(void *pData);
void ME_Get_ETWSInfo_CB(void *pData);
void ME_Set_CMER_CB(void *pData);


#endif