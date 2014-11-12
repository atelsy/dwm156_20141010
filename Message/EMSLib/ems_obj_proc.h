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
*   ems_obj_proc.h
*
* Project:
* --------
*   DCT
*
* Description:
* ------------
*   This file is intends for SMS Library.
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
* 07 19 2012 fengping.yu
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


#ifndef _EMS_OBJ_PROC_H
#define _EMS_OBJ_PROC_H
 

extern EMS_OBJ_PACK_FUNCTION EMS_OBJ_PACK[NUM_OF_EMS_SUPPORTED_OBJ] ;


extern EMSObject *EMSObjectAllocate(kal_bool is_data_present);
extern kal_uint8 *EMSAllocObjPduBuff(kal_uint16 size, EMSObject *obj);
extern void EMSFreeObjPduBuff(kal_uint8 *pdu_ptr, EMSObject *obj);
extern void EMSHoldObjPduBuff(kal_uint8 *pdu_ptr );
extern void EMSUpdateObjPtr(EMSObject *obj );
extern void EMSUpdatePduBuffPtr(EMSObject *obj , kal_uint8 *new_pdu_ptr);

extern EMSTATUS ReleaseEMSObject (EMSObject *object);
extern void DeleteEMSObjectFromList(EMSData *emsData, EMSObject *Object);
extern void EMSSwapObject(EMSData *emsData, EMSObject *obj1, EMSObject *obj2);
extern EMSTATUS AddPicture(EMSObject *object,
kal_uint8 attribute, 
kal_uint8 vDim,
kal_uint8 hDim,
kal_uint8 bitsPerPixel, 
kal_uint16 pdu_length,
kal_uint8 *pdu);

extern EMSTATUS AddUsrDefAnim(EMSObject *object, 
   kal_uint8 attribute,
   kal_uint8 cFrame,
   kal_uint8 cRepeat,  
   kal_uint8 Duration,
   kal_uint8 vDim, 
   kal_uint8 hDim,
   kal_uint8 bitsPerPixel, 
   kal_uint16 pdu_length,
   kal_uint8 *pdu   );

extern EMSTATUS AddPreDefAnim(kal_uint8 PredefNo, EMSObject *object);

extern EMSTATUS AddUsrDefSnd(EMSObject   *object,
  kal_uint8   attribute, 
  kal_uint16  pdu_length, 
  kal_uint8   *pdu);

extern EMSTATUS AddPreDefSnd(kal_uint8 PredefNo, EMSObject *object);
extern void AddObjectIntoListByOffset(EMSData *emsData, EMSObject *newObj);
extern void AddObjectIntoList(EMSData *emsData, EMSObject *newObj);

extern EMSTATUS AddToObjQueue(EMSObjQueue **objQueue, EMSObject *newObj);
extern EMSObject* RemoveFromObjQueue(EMSObjQueue **objQueue);

extern kal_bool PackPredefSound(EMSData   *emsData,
 EMSObject *object, 							
							kal_bool  needPack);
extern kal_bool PackPredefAnimation(EMSData  *emsData,
 EMSObject*object, 
							kal_bool needPack);
extern kal_bool PackUserDefSound(EMSData  *emsData,
  EMSObject*object, 
							 kal_bool needPack);
extern kal_bool PackAnimation(EMSData  *emsData,
   EMSObject*object, 					
					kal_bool needPack);
extern kal_bool PackPicture(EMSData*emsData,
 EMSObject  *object, 					  
					  kal_bool   needPack);

extern kal_bool EMSCheckObjDistInd( EMSObject *object);
extern EMSTATUS EMSNeedObjDistInd (EMSData*emsData,
EMSObject  **object);
extern kal_bool EMSPackObjDistInd (EMSData*emsData,
EMSObject  *object, 					  
kal_bool   needPack);
extern EMSObject *CopyResuedObject(EMSData *emsData, EMSObject *obj);
extern EMSObject *CopyEMSObject(EMSObject *obj);

extern kal_bool EMSObjectCompare(kal_uint8 type, EMSObjData *objData1, 
 EMSObjData *objData2);

extern EMSTATUS EMSObjectCheck(kal_uint8 type, EMSObjData *objData, 
   kal_bool *isExtObj);
#ifdef __EMS_REL5__

extern kal_bool EMSPackExtObj(EMSData*emsData,
  EMSObject  *object, 					  
  kal_bool   needPack);
extern kal_bool EMSPackReusedObj(EMSData*emsData,
  EMSObject  *object, 					  
  kal_bool   needPack);

extern EMSTATUS AddVApp(EMSObject *object,
 kal_uint8 attribute, 
 kal_uint16 pdu_length,
 kal_uint8 *pdu);

extern kal_bool EMSFindRusedObjByObjData(EMSData *emsData, EMSObject *newObj, 
 kal_uint8 type, EMSObjData *objData) ;
extern kal_bool EMSFindRusedObj(EMSData *emsData,
 EMSObject *newObj, kal_uint8 type, EMSObjData *objData);
extern EMSObject *EMSFindRusedObjByRef(EMSData *emsData, kal_uint8 msg_ref);

extern EMSTATUS EMSObjHeaderCheck(EMSData *emsData, kal_uint8 iei, kal_uint8 ie_len, 
   kal_uint8 *data, kal_uint16 *obj_data_length);

extern EMSTATUS EMSFillExtObjIE (EMSObject *obj, kal_uint8 iei, 
 kal_uint8 *obj_buff, kal_uint16 obj_len);

extern EMSTATUS EMSEncodeExtObjIE(EMSData *emsData, EMSObject *obj, kal_bool needPack);
extern EMSTATUS EMSDecodeExtObjIE(EMSData *emsData, kal_uint16 len, kal_uint8 *data);

extern EMSTATUS EMSDecodeReusedExtObjIE (EMSData *emsData, kal_uint8 len, kal_uint8 *data);

extern EMSTATUS EMSCompressedObjIE(EMSData *emsData, kal_bool needPack) ;
extern EMSTATUS EMSDeCompressedObjIE(EMSData *emsData);
extern void EMSCheckExtTextOffset(EMSData *emsData);


extern EMSTATUS EMSGetExtObjLen(EMSObject *obj, kal_uint16 *length, kal_uint8 *iei_info);

extern kal_uint16 EMSGetExtObjRemainingSpace(EMSData *emsData, 
  EMSInternalData *i, 
  kal_uint16 ori_udh_len,
  kal_uint16 txt_len);
#endif
extern EMSTATUS EMSDecodeUDHObj(EMSObject **object, EMSData *emsData, 
   kal_uint8 iei, kal_uint8 ie_len, kal_uint8 *data);

extern void UpdateEMSObjOffset(EMSData  *emsData,
kal_bool isDel, 
kal_uint16  numOfOctet);
extern kal_uint16 getObjLength(EMSTYPE type, EMSObjData *objData, kal_uint16 text_len, kal_bool *isAllow);


#endif /* _EMS_OBJ_PROC_H */



