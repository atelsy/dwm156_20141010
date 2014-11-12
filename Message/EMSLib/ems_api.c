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
* ems_api.c
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


#define EMS_API_C

#include "kal_non_specific_general_types.h"
#include <stdio.h>
//#include <malloc.h> /* malloc, free */
#include <memory.h> /* memcpy, memset */
#include <assert.h> /* assert */

#define ASSERT assert

#include "smsal_l4c_enum.h"
#include "ems.h"
#include "ems_defs.h"
#include "ems_enums.h"
#include "ems_context.h"
#include "ems_init.h"
#include "ems_utils.h"
#include "ems_obj_proc.h"
#include "ems_txt_proc.h"
#include "ems_position_proc.h"
#include "ems_segment_proc.h"
#include "customer_ps_inc.h"
#include "custom_ems_context.h"

 /* 
 * EMS APIs
 */
EMSTATUS EMSInitialize(void)
{ 
   // modify by PhoneSuite
   // kal_uint8 i;
   int i;

   if(EMS_CNTX(isInit) == KAL_TRUE)
  return EMS_ALREADY_INITIALIZED;

   EMS_CNTX(isInit) = KAL_TRUE;
   EMS_CNTX(concat_msg_ref) = 0;
   EMS_CNTX(max_seg_num)= ems_max_seg_num();

   EMS_CNTX(activeEMSDataID) = 0xffffffff; /* invalid one */

   for(i=0;i<EMS_MAX_EMS_DATA;i++)
   {
  EMS_CNTX(isEMSDataUsed[i]) = KAL_FALSE;
  EMS_CNTX(textBuffer[i])= ems_get_text_buff_ptr(i);

  EMS_CNTX(internalData[i]).TPUDLen   = ems_get_TPUDLen_ptr(i);
  EMS_CNTX(internalData[i]).UDHL  = ems_get_UDHL_ptr(i);
   }

#ifndef __SLIM_EMS__
   EMSPduMemPoolInitialize();
#endif

   return EMS_OK;

} /* endf of EMSInitialize */

EMSTATUS EMSDeInitialize(void)
{
   EMS_INIT_CHECK;
   EMS_CNTX(isInit) = KAL_FALSE;
   return EMS_OK;
} /* end of EMSDeInitialize */



/*****************************************************************************
* FUNCTION
*  InitializeEMSData
* DESCRIPTION
*   This function initializes the EMS Data.
*
* PARAMETERS
*  a  IN/OUT  emsData
*  b  IN  dcs
* RETURNS
*  EMSTATUS
* GLOBALS AFFECTED
*  none
*****************************************************************************/
EMSTATUS InitializeEMSData(EMSData *emsData, kal_uint8 dcs)
{
   EMSInternalData *internal;
   
   EMS_INIT_CHECK;

   if(EMSGetEMSDataResource(emsData) == KAL_FALSE)
  return EMS_EMS_DATA_EXCEEDED;

   /* check data coding scheme */
   if((dcs != SMSAL_DEFAULT_DCS) &&
  (dcs != SMSAL_UCS2_DCS) &&
  (dcs != SMSAL_8BIT_DCS))
   {
  return EMS_INVALID_DCS;
   }

   emsData->CurrentTextFormatObj = NULL;   
   emsData->listHead = NULL;
   emsData->listTail = NULL;
   emsData->textLength   = 0;   
   emsData->dcs  = dcs;  
   emsData->isConcat = KAL_FALSE;
   emsData->udhi = KAL_FALSE;   
   emsData->ps_dcs   = dcs;  

   emsData->PortNum.isPortNumSet = KAL_FALSE;
   emsData->PortNum.src_port = 0;
   emsData->PortNum.dst_port = 0;

   internal = (EMSInternalData*)emsData->internal;

   /* reset internal data */
   /* ems_mem_set(emsData->internal, 0, sizeof(EMSInternalData));  */
   internal->totalTxtFmtOctet= 0;
   internal->totalObjOctet   = 0;
   internal->totalExtObjOctet= 0;
   internal->numOfEMSObject  = 0;
   internal->numOfExtObject  = 0;

   internal->CurrentConcatMsgRef = 0;

#ifdef __EMS_REL5__
   internal->needReCompress  = KAL_FALSE ;
   internal->forceCompress   = KAL_FALSE ;
   internal->CompressIELen   = 0;

   internal->UnpackSegNum= 0;
   internal->MissSegInCrossIE= KAL_FALSE;

   internal->ObjDistIndCount = 0 ;
   internal->ExtObjRefNum= 0 ;
   internal->RefNumOfSwitchObj= 0xff ;
   internal->isObjSwitched= KAL_FALSE ;

   internal->PBuff.BufType   = EMS_PBUFF_NONE;
   internal->PBuff.BufPtr= NULL;
   internal->PBuff.BufLen= 0;
   internal->PBuff.BufOffset = 0;
#endif


   emsData->Reminder.isSegChange = KAL_FALSE;
   emsData->Reminder.requiredSegment = 1;

   UpdateLenInfo(emsData);
   
   /* reset current posistion */
   ResetCurrentPosition(emsData);

   return EMS_OK;

} /* end of InitializeEMSData */

/*****************************************************************************
* FUNCTION
*  ReleaseEMSData
* DESCRIPTION
*   This function releases the EMS Data, including text buffer, object list.
*
* PARAMETERS
*  a  IN/OUT  emsData
* RETURNS
*  EMSTATUS
* GLOBALS AFFECTED
*  none
*****************************************************************************/
EMSTATUS ReleaseEMSData (EMSData *data)
{
#ifndef __SLIM_EMS__
   EMSObject   *prev;
#endif
   EMSInternalData *intr;
   EMSObject   *obj = data->listTail;
   
   EMS_DATA_CHECK(data);

   if(data == NULL)
  return EMS_NULL_POINTER;  

   ASSERT(data->internal != NULL);

 //  intr = (EMSInternalData*)data->internal;
   SET_ACTIVE_EMS_ID(data->id);


#ifdef __SLIM_EMS__
   EMS_ASSERT(obj == NULL);
#else
   /* free EMS object(s) */
   /* Free from tail to avoid memory moving of pdu buffer */
   while(obj != NULL)
   {
  prev = obj->prev;
  ReleaseEMSObject(obj);
  obj = prev;
   }
   
   if(data->internal != NULL)
   {
  intr = (EMSInternalData*)data->internal;

  if(intr->notFinishedObject != NULL)
 ReleaseEMSObject(intr->notFinishedObject);

   }
  
#ifdef __EMS_REL5__
   EMSFreePBuff(&(intr->PBuff));
#endif
#endif

   data->internal = NULL;
   data->CurrentTextFormatObj = NULL;
   data->listHead = NULL;
   data->listTail = NULL;
   data->textBuffer   = NULL;
   data->textBufferSize = 0;
   data->textLength = 0;   

   EMSFreeEMSDataResource(data);

   return EMS_OK;
   
} /* end of ReleaseEMSData */



/*****************************************************************************
* FUNCTION
*  ResetCurrentPosition
* DESCRIPTION
*   This function moves current position to the HEAD.
*
* PARAMETERS
*  a  IN/OUT  emsData
* RETURNS
*  none
* GLOBALS AFFECTED
*  none
*****************************************************************************/
void ResetCurrentPosition(EMSData *emsData)
{
   if(IS_EMS_INIT == KAL_FALSE ||
  IS_EMS_DATA_VALID(emsData) == KAL_FALSE) return;

   if(emsData != NULL)
   {
  ASSERT(emsData->internal != NULL);

  emsData->CurrentPosition.OffsetToText = 0;
  emsData->CurrentPosition.Object = NULL;
  emsData->CurrentTextFormatObj   = NULL;
   }
} /* end of ResetCurrentPosition */

/*****************************************************************************
* FUNCTION
*  GoToEndPosition
* DESCRIPTION
*   This function moves current position to the END.
*
* PARAMETERS
*  a  IN/OUT  emsData
* RETURNS
*  none
* GLOBALS AFFECTED
*  none
*****************************************************************************/
void GoToEndPosition(EMSData *emsData)
{
   if(IS_EMS_INIT == KAL_FALSE) return;

   if(emsData == NULL) return;

   ASSERT(emsData->internal != NULL);

   /* move offset to end of text. */
   emsData->CurrentPosition.OffsetToText = emsData->textLength;;
   /* point to last object. */
   emsData->CurrentPosition.Object = emsData->listTail;
   
   if(emsData->CurrentPosition.Object != NULL)
   {
  if(emsData->CurrentPosition.Object->Type == EMS_TYPE_TEXT_FORMAT)
  {
 /* set current TF obj. */
 emsData->CurrentTextFormatObj = emsData->CurrentPosition.Object;
  }
  else
  {
 /* find the closest TF obj, and check does offset fall in TF obj. */
 SetCurrTxtFmtObj(emsData);
  }  
   }

   /* check does offset fall in TF obj. */
   /* if not fall in, set CurrentTextFormatObj = NULL  */
   if(emsData->CurrentTextFormatObj != NULL &&
  CURR_TXT_OFFSET(emsData) > (EMS_OBJ_OFFSET(emsData->CurrentTextFormatObj)+
  TXT_FMT_LEN(emsData->CurrentTextFormatObj)) )
   {
  emsData->CurrentTextFormatObj = NULL;
   } 

} /* end of GoToEndPosition */



/*****************************************************************************
* FUNCTION
*  BackwardCurrentPosition
* DESCRIPTION
*   This function backwards current position by n steps.
*
* PARAMETERS
*  a  IN  emsData
*  b  IN  steps (to be moved)
*  c  IN  needCancel, indicate whether to remove text or object
* RETURNS
*  steps actual moved
* GLOBALS AFFECTED
*  none
*****************************************************************************/
kal_uint16 BackwardCurrentPosition(EMSData *emsData, kal_uint16 steps)
{
   return BwCancelCurrentPosition(emsData, steps, KAL_FALSE, KAL_FALSE);	
} /* end of BackwardCurrentPosition */

/*****************************************************************************
* FUNCTION
*  CancelCurrentPosition
* DESCRIPTION
*   This function cancels current position by n steps.
*
* PARAMETERS
*  a  IN  emsData
*  b  IN  steps (to be moved)
*  c  IN  needCancel, indicate whether to remove text or object
* RETURNS
*  steps actual moved
* GLOBALS AFFECTED
*  none
*****************************************************************************/
kal_uint16 CancelCurrentPosition(EMSData *emsData, kal_uint16 steps)
{
   EMSTATUS ret;
   kal_uint16 actualCancel;

   EMS_DBG_(   
  kal_bool inEndPos=KAL_FALSE;
  if(((kal_uint32*)emsData->CurrentPosition.Object == (kal_uint32*)emsData->listTail) &&
 (emsData->CurrentPosition.OffsetToText == emsData->textLength))
   inEndPos = KAL_TRUE;   
   )

   actualCancel = BwCancelCurrentPosition(emsData, steps, KAL_TRUE, KAL_FALSE);

   EMS_DBG_(
  if(steps > actualCancel)  
  {/* all ems contents MUST be deleted */
 ASSERT(emsData->CurrentPosition.Object == NULL);
 ASSERT(emsData->CurrentPosition.OffsetToText == 0);
 if(inEndPos == KAL_TRUE)   
 { ASSERT(emsData->textLength == 0);
ASSERT(((EMSInternalData*) emsData->internal)->numOfEMSObject == 0); } }
   )

   UpdateLenInfo(emsData);
   EMSSetUDHI(emsData);

   ret=EMSCalculateLength(emsData);
#ifdef __EMS_REL5__
   if (ret != EMS_OK)
   {
  EMSInternalData *internal = (EMSInternalData*)emsData->internal;

  if (internal->isObjSwitched == KAL_TRUE)
  {
 EMSObject *obj_tmp, *obj = emsData->listHead;

 EMS_ASSERT(internal->RefNumOfSwitchObj != 0xff);
 while (obj != NULL)
 {
if ((IS_TXT_FMT_OBJ (obj) != KAL_TRUE) &&
  (EXT_OBJ_INFO_P(obj, isExtObj)== KAL_TRUE) &&
  (EXT_OBJ_INFO_P(obj, ExtObjRef)== internal->RefNumOfSwitchObj))
{
   obj_tmp = obj->next;
   DeleteEMSObjectFromList(emsData, obj);
   obj = obj_tmp;
   continue;
}

obj = obj->next;
 }

 ret=EMSCalculateLength(emsData);
  }

  if(EMS_CNTX(isCompressSupp) == KAL_TRUE)
  {
 if (ret == EMS_NO_SPACE)
 {		
internal->forceCompress  = KAL_TRUE;
internal->needReCompress = KAL_TRUE;
ret=EMSCalculateLength(emsData);
 }
  }

  ASSERT(ret==EMS_OK);
   }
   /* reset */
   ((EMSInternalData*) emsData->internal)->RefNumOfSwitchObj = 0xff;
   ((EMSInternalData*) emsData->internal)->isObjSwitched = KAL_FALSE;
#else
   if (ret!=EMS_OK)
  ASSERT(ret==EMS_OK);
#endif

   EMSAddNullTerminator(emsData->textBuffer + emsData->textLength);

   return actualCancel;

} /* end of CancelCurrentPosition */

/*****************************************************************************
* FUNCTION
*  ForwardCurrentPosition
* DESCRIPTION
*   This function forwards and cancels (if required) text or object.
*
* PARAMETERS
*  a  IN  emsData
*  b  IN  steps (to be moved)
* RETURNS
*  steps actual moved
* GLOBALS AFFECTED
*  none
*****************************************************************************/
kal_uint16 ForwardCurrentPosition(EMSData *emsData, kal_uint16 steps)
{
	kal_uint16 remainingStep = steps;
	kal_uint16 maxStep, maxStepInOctet;
	kal_uint16 nextObjOffset;
   EMSObject *obj;
   EMSObject *curr_obj;
   kal_bool  isTFexist;

   if(IS_EMS_INIT == KAL_FALSE || IS_EMS_DATA_VALID(emsData) == KAL_FALSE) return 0;

   /* sanity check */
   if((emsData == NULL) || (steps == 0)) return 0;

   SET_ACTIVE_EMS_ID(emsData->id);

   ASSERT(emsData->internal != NULL);

   while(remainingStep > 0)
   {
  curr_obj = emsData->CurrentPosition.Object;

  if(curr_obj != NULL && IS_TXT_FMT_OBJ(curr_obj) == KAL_TRUE)
 isTFexist = KAL_TRUE;
  else
 isTFexist = KAL_FALSE;
   
  if(((kal_uint32*)curr_obj == (kal_uint32*)emsData->listTail) &&
 (emsData->CurrentPosition.OffsetToText == emsData->textLength))
  {
 /* already reached the END */
 break;
  }	
		
  if(curr_obj == NULL)   
  	obj = emsData->listHead;  
  else
  	obj = curr_obj->next;					
				
  if(obj != NULL)
			nextObjOffset = obj->OffsetToText;
  else
  	nextObjOffset = emsData->textLength;
  
  /* in case that MMI doesn't call EMS library properly, 
   * we turn off this assertion in production version.
   */
  EMS_ASSERT(nextObjOffset >= emsData->CurrentPosition.OffsetToText);
  if(nextObjOffset < emsData->CurrentPosition.OffsetToText) 
  {
 return (steps - remainingStep);	
  }

  maxStepInOctet = nextObjOffset - emsData->CurrentPosition.OffsetToText;
  maxStep = maxStepInOctet/2;

  if(maxStep >= remainingStep)		
  {
 maxStep = remainingStep;
 maxStepInOctet = maxStep*2;
  }
		
  if(maxStep == 0)
  {
  	 /* in case that MMI doesn't call EMS library properly, 
  * we turn off this assertion in production version.
  */
 EMS_ASSERT( obj != NULL );
 if( obj == NULL )
 {
return (steps - remainingStep);	
 }

 /* forward object part */
 if(IS_TXT_FMT_OBJ(obj) == KAL_FALSE) 
remainingStep--; 
 else
emsData->CurrentTextFormatObj = obj;

 emsData->CurrentPosition.Object = obj;
 
  }
  else
  {
 /* forward for text part */ 
	  remainingStep -= maxStep;
	  emsData->CurrentPosition.OffsetToText += maxStepInOctet;  
 
 /* the current position is in the range of text format object 
  * if not, change the current position */
 if(isTFexist == KAL_TRUE)
 {
if(emsData->CurrentPosition.OffsetToText > (emsData->CurrentPosition.Object->OffsetToText+EMS_P(emsData->CurrentPosition.Object->data,text_format.textLength)))
{ 
   SWITCH_TO_NEXT_EMS_OBJ(emsData);   
}
 }
  }
   } /* while */

   /* if current object is text format, assign current text format object */
   if(emsData->CurrentPosition.Object != NULL &&
  IS_TXT_FMT_OBJ(emsData->CurrentPosition.Object))
   {
  emsData->CurrentTextFormatObj = emsData->CurrentPosition.Object;
   }

   /* eg, 
* "bold" sound "normal"
* ^ -- current position*
* in this example, 
*  current_text_fmt --> bold
*  current_obj  --> sound, but 
*  current_position --> normal
* thus, current_text_fmt shall be set to NULL
*/
   if(emsData->CurrentTextFormatObj != NULL)
   { 
  if(CURR_TXT_OFFSET(emsData) > (EMS_OBJ_OFFSET(emsData->CurrentTextFormatObj)+TXT_FMT_LEN(emsData->CurrentTextFormatObj)))   
 emsData->CurrentTextFormatObj = NULL;  
   }
   else
   {
  SetCurrTxtFmtObj(emsData);   
   }

   /* 12/07/2003, Kevin
* fix issue that text format precedes other objects (with the same offset)
*/
   /* Robert comment: for following case  */
   /*format : <snd> bold  */
   /*  ^  */
   /*object list: A(bold) -- B(snd)   */
   /* The case should not occurs if optimize text format. */
   /* But there are some case which can't be changed to optimized text format. */
   /* This checking shall be kept. */
   if(emsData->CurrentTextFormatObj != NULL && IS_CURRENT_OBJ_PRESENT(emsData) == KAL_TRUE)
   {
  if((IS_TXT_FMT_OBJ(emsData->CurrentPosition.Object) == KAL_FALSE) &&
 (emsData->CurrentPosition.OffsetToText == EMS_OBJ_OFFSET(emsData->CurrentTextFormatObj)) &&
 (emsData->CurrentPosition.OffsetToText == EMS_OBJ_OFFSET(emsData->CurrentPosition.Object)))
  {
 emsData->CurrentTextFormatObj = NULL;
  }
   }

   return (steps - remainingStep); /* return steps actual moved */
} /* end of ForwardCurrentPosition */


/*****************************************************************************
* FUNCTION
*  ResetTextFormat
* DESCRIPTION
*   This function resets a text format structure. 
*
* PARAMETERS
*  a  IN/OUT  txt_fmt
* RETURNS
*  none
* GLOBALS AFFECTED
*  none
*****************************************************************************/
void ResetTextFormat(EMSTextFormat *txt_fmt)
{
   txt_fmt->Alignment = EMS_ALIGN_LANG_DEP;
   txt_fmt->FontSize = EMS_FONTSIZE_NORMAL;
   txt_fmt->isBold = KAL_FALSE;
   txt_fmt->isItalic= KAL_FALSE;
   txt_fmt->isUnderline = KAL_FALSE;
   txt_fmt->isStrikethrough = KAL_FALSE;
	
   txt_fmt->fgColor = EMS_BLACK;
   txt_fmt->bgColor = EMS_WHITE;

} /* end of ResetTextFormat */



/*****************************************************************************
* FUNCTION
*  AddObject
* DESCRIPTION
*   This function adds an object.
*
* PARAMETERS
*  a  IN/OUT  emsData
*  b  IN  type
*  c  IN  objData, input object data
* RETURNS
*  EMSTATUS
* GLOBALS AFFECTED
*  none
*****************************************************************************/
EMSTATUS AddObject(EMSData*emsData, 
	kal_uint8  type, 
	EMSObjData *objData,
   kal_uint8  PredefNo)
{	

#ifdef __SLIM_EMS__
   return EMS_NOT_SUPPORT_OBJECT ;
#else
   EMSObject *newObj;
   EMSTATUS   ret, roll_ret;
   kal_bool   isExtObj = KAL_FALSE ;

   EMS_DATA_CHECK(emsData);
   
   if(emsData == NULL)
  return EMS_NULL_POINTER;

   /* if the object is not predefined, 
* objData can't be NULL 
*/
   if(IS_EMS_DATA_NEED(type)==KAL_TRUE && objData == NULL)
  return EMS_NULL_POINTER;

   SET_ACTIVE_EMS_ID(emsData->id);

   ASSERT(emsData->internal != NULL);


   if ((ret = EMSObjectCheck(type, objData, &isExtObj)) != EMS_OK)
  return ret;

   /* allocate memory for this object */
   newObj = EMSObjectAllocate(IS_EMS_DATA_NEED(type));   
   if(newObj == NULL)
  return EMS_NO_MEMORY;

   newObj->Type = type;
   newObj->OffsetToText = emsData->CurrentPosition.OffsetToText;   

#ifdef __EMS_REL5__
   if ((emsData->PortNum.isPortNumSet == KAL_TRUE) &&
   (isExtObj == KAL_FALSE ) &&
   (IS_EMS_DATA_NEED(type) == KAL_TRUE) )
   {
  if ((objData->common.pdu_length + 5 )> 128)
  {
 isExtObj = KAL_TRUE;
  }
   }

   if ( isExtObj == KAL_TRUE )
   {
  if (EMSFindRusedObjByObjData(emsData, newObj, type, objData) == KAL_TRUE)
  {
 ((EMSInternalData*)emsData->internal)->needReCompress  = KAL_TRUE; 
 goto InsertObj;
  }

  EXT_OBJ_INFO_P(newObj, isExtObj)   = isExtObj;
   }

   if (EMS_CNTX(isCompressSupp) == KAL_TRUE)
   {
  ((EMSInternalData*)emsData->internal)->needReCompress  = KAL_TRUE; 

  if((IS_TXT_FMT_OBJ (newObj) != KAL_TRUE)  &&
 (EXT_OBJ_INFO_P(newObj, isReusedObj) != KAL_TRUE) )
 EXT_OBJ_INFO_P(newObj, ExtObjRef)  = EMSGetExtObjRef(emsData);
   }
   else
   {
  if ( EXT_OBJ_INFO_P(newObj, isExtObj) == KAL_TRUE)
 EXT_OBJ_INFO_P(newObj, ExtObjRef)  = EMSGetExtObjRef(emsData);
   }
#endif

   switch(type)
   {
  /*================================
   * predefed sound : 0 ~ 9
   *================================*/
  case EMS_TYPE_PREDEF_SND:
   ret = AddPreDefSnd(PredefNo, newObj);
   break;		

  /*================================
   * predefed animation : 0 ~ 14		 
   *================================*/
  case EMS_TYPE_PREDEF_ANM:
   ret = AddPreDefAnim(PredefNo, newObj);	   
   break;			  

  /*================================
   * User defined sound : eg: iMelody
   *================================*/
  case EMS_TYPE_USERDEF_SND:

   ret = AddUsrDefSnd(newObj, objData->sound.attribute,
  objData->sound.pdu_length, objData->sound.pdu);	
   break;   

  /*================================
   * small animation :  8 x  8, 4 frames
   * large animation : 16 x 16, 4 frames
   *================================*/
  case EMS_TYPE_USERDEF_ANM:
   ret = AddUsrDefAnim(newObj, 
   objData->animation.attribute,
   objData->animation.cFrame,
   objData->animation.cRepeat,  
   objData->animation.Duration,
   objData->animation.vDim, 
   objData->animation.hDim,
   objData->animation.bitsPerPixel, 
   objData->animation.pdu_length,
   objData->animation.pdu   );

   break;			  
			  
  /*================================
   * Picture	 
   *================================*/			  
  case EMS_TYPE_PIC:
   ret = AddPicture(newObj,
objData->picture.attribute, 
objData->picture.vDim,
objData->picture.hDim,
objData->picture.bitsPerPixel, 
objData->picture.pdu_length,
objData->picture.pdu);
   break;

#ifdef __EMS_REL5__
  /*================================
   * VCard and VCalendar	 
   *================================*/			  
  case EMS_TYPE_VCARD:
  case EMS_TYPE_VCALENDAR:
   ret = AddVApp(newObj,
objData->vapp.attribute, 
objData->vapp.pdu_length,
objData->vapp.pdu);
   break;
#endif
  default : 
  	  ret = EMS_INVALID_OBJECT;
  	  break; 	  
   }/* switch */

   if(ret != EMS_OK)
   {
  EMS_MFREE(newObj);
  return ret;
   }

#ifdef __EMS_REL5__
InsertObj :
#endif
   AddObjectIntoList(emsData, newObj);
   emsData->CurrentPosition.Object = newObj; /* required */
   
   UpdateLenInfo(emsData);
   EMSSetUDHI(emsData);
   
   ret=EMSCalculateLength(emsData);
		
   /* rollback */
   if(ret != EMS_OK)
   {
  kal_uint16 n;

  n=BwCancelCurrentPosition(emsData, 1, KAL_TRUE, KAL_TRUE);
  ASSERT(n==1);
   }
#if 0
   else
   {
  EMSObject *obj_next; 
  EMSObject *obj = emsData->listHead;

  while(obj != NULL)
  { 
 if (obj->Type == EMS_TYPE_TEXT_FORMAT)
 {
/* reset alignment attribute */
obj->data->text_format.Alignment = EMS_ALIGN_LANG_DEP ;
if (KAL_TRUE == isDefaultTF( &(obj->data->text_format)))
{
   obj_next = obj->next;
   DeleteEMSObjectFromList(emsData, obj);

   if(emsData->CurrentTextFormatObj == obj)
  emsData->CurrentTextFormatObj = NULL;	
 
   obj = obj_next;
   continue ;
}
 }
 obj = obj->next;
  }
   }
#endif

  UpdateLenInfo(emsData);
  EMSSetUDHI(emsData);

   roll_ret = EMSCalculateLength(emsData);
   ASSERT(roll_ret == EMS_OK);

   return ret;
#endif
	
} /* end of AddObject */

/*****************************************************************************
* FUNCTION
*  AddString
* DESCRIPTION
*   This function adds a string.
*
* PARAMETERS
*  a  IN/OUT  emsData
*  b  IN  string
*  c  IN  num_char
*  d  IN  txt_fmt, NULL if no text format specified
* RETURNS
*  EMSTATUS
* GLOBALS AFFECTED
*  none
*****************************************************************************/
EMSTATUS AddString(EMSData   *emsData, 
   kal_uint8*string, 
   kal_uint16   num_char, 
   EMSTextFormat *txt_fmt)
{  
   kal_uint16	i;   
   EMSTATUS		ret;
#ifndef __SLIM_EMS__
   EMSTextFormat *txt_fmt_tmp = txt_fmt;
#endif
   
   /* one character need 2 bytes, 
* if DCS is GSM7-bit or 8-bit, 1st byte is for padding */
   kal_uint16 num_byte = num_char*2; 
   
   EMS_DATA_CHECK(emsData);

   if(emsData == NULL || string == NULL)
  return EMS_NULL_POINTER;

   if(num_char == 0)
  return EMS_UNSPECIFIED_ERROR;

   ASSERT(emsData->internal != NULL);

   if(emsData->Reminder.totalRemainingOctet < GetTxtLenInChar(emsData->dcs,num_byte))
   {
  return EMS_NO_SPACE;
   }
   
   SET_ACTIVE_EMS_ID(emsData->id);

   
#ifndef __SLIM_EMS__
   if(emsData->CurrentTextFormatObj != NULL && txt_fmt_tmp == NULL) 
  txt_fmt_tmp = &(emsData->CurrentTextFormatObj->data->text_format);

   if(txt_fmt_tmp != NULL)
   {
  /* input string associated with a TEXT FORMAT */
	   ret = AddTextFormat(emsData, txt_fmt_tmp, num_byte); 
	   if(ret != EMS_OK)
		   return ret;
   }
#endif
   
   if(emsData->textLength == emsData->CurrentPosition.OffsetToText)
   {
  /* append the input string */
	   ems_mem_cpy((emsData->textBuffer+emsData->textLength),
				   string,
				   num_byte);
   }
   else
   {
  /* insert the input string */
	   for(i = emsData->textLength; 
		   i >= emsData->CurrentPosition.OffsetToText + 1;
		   i--)
	   {
		   *(emsData->textBuffer + num_byte + i - 1) = *(emsData->textBuffer + i - 1);
	   }   
 	
	   ems_mem_cpy((emsData->textBuffer+emsData->CurrentPosition.OffsetToText),
				   string,
				   num_byte);
  

   }

   /*
* Update Objects' offset
*/
   UpdateEMSObjOffset(emsData,
  KAL_FALSE, /* not del => add */
  num_byte);
  
   emsData->CurrentPosition.OffsetToText += num_byte;
   emsData->textLength += num_byte; 

   /* increment length of the corresponding formatted text  */
   /* New string will be appended to old TF. */
#ifndef __SLIM_EMS__   
   if(txt_fmt == NULL && emsData->CurrentTextFormatObj != NULL && txt_fmt_tmp == NULL)   
   {
  EMS_P(emsData->CurrentTextFormatObj->data, text_format.textLength) += num_byte;
   }
#endif
 
   UpdateLenInfo(emsData);
   EMSSetUDHI(emsData);

   ret = EMSCalculateLength(emsData);

   /* rollback */
   if(ret != EMS_OK)
   {
	   EMSTATUS	roll_ret;
	   BwCancelCurrentPosition(emsData, num_char, KAL_TRUE, KAL_TRUE);

	   UpdateLenInfo(emsData);
	   EMSSetUDHI(emsData);

	   roll_ret=EMSCalculateLength(emsData);
	   if (roll_ret!=EMS_OK)
		   ASSERT(roll_ret==EMS_OK);
   }
   
   EMSAddNullTerminator(emsData->textBuffer + emsData->textLength);

   return ret;
} /* end of AddString */

/*****************************************************************************
* FUNCTION
*  AddChar
* DESCRIPTION
*   This function adds a character.
*
* PARAMETERS
*  a  IN/OUT  emsData
*  b  IN  string
*  d  IN  txt_fmt, NULL if no text format specified
* RETURNS
*  EMSTATUS
* GLOBALS AFFECTED
*  none
*****************************************************************************/
EMSTATUS AddChar(EMSData *emsData, kal_uint8 *string, EMSTextFormat *txt_fmt)
{
   return AddString(emsData, string, 1, txt_fmt);   
}

/*****************************************************************************
* FUNCTION
*  AddTextAlignment
* DESCRIPTION
*   This function adds a character.
*
* PARAMETERS
*  a  IN/OUT  emsData
*  b  IN  string
*  d  IN  txt_fmt, NULL if no text format specified
* RETURNS
*  EMSTATUS
* GLOBALS AFFECTED
*  none
*****************************************************************************/
EMSTATUS AddTextAlignment(EMSData *emsData, kal_uint16 offsetToText, EMSTextFormat *txt_fmt)
{

#ifdef __SLIM_EMS__
   return EMS_NOT_SUPPORT_OBJECT ;
#else

   EMSTATUS	ret, roll_ret;

   EMSObjQueue *txt_fmt_obj_list = NULL;
   EMSObjQueue *new_alignment_obj_list = NULL ;
   EMSObjQueue *q_node ;
   EMSObject   *obj, *newObj;
   EMSTextFormat txt_fmt_tmp ;
   kal_uint16	start_position, end_position ,remain_len;
   kal_bool  isResetTxtFmt;
   kal_bool  need_repack = KAL_FALSE;


   isResetTxtFmt = isDefaultTF(txt_fmt);

   start_position = offsetToText;
   end_position   = offsetToText + txt_fmt->textLength ;
   remain_len = txt_fmt->textLength ;
   ems_mem_cpy(&txt_fmt_tmp, txt_fmt,  sizeof(EMSTextFormat));

   obj = emsData->listHead;

   /* find out the overlapping txt fmt and add to queue. */
   while (obj != NULL)
   {
  /* 1. text fmt object.  */
  /* 2. start "or" end of the txt fmt is in the range of new alignment. */
  /*"or" the range of txt fmt is larger than new alignment. */
  if ((obj->Type == EMS_TYPE_TEXT_FORMAT) &&
  (((EMS_OBJ_OFFSET(obj) >= start_position) && (EMS_OBJ_OFFSET(obj) <  end_position)) || 
   ((TXT_FMT_OBJ_END(obj) > start_position) && (TXT_FMT_OBJ_END(obj)<= end_position))||
   ((EMS_OBJ_OFFSET(obj) <= start_position) && (TXT_FMT_OBJ_END(obj) >= end_position))))
  {
 if ((ret = AddToObjQueue(&txt_fmt_obj_list ,obj)) == EMS_NO_MEMORY )
   return EMS_NO_MEMORY;
  }

  obj = obj->next;
   }

   /* Cases shall be handled. */
   /* 1. The start of existent txt fmt is less than (in front of) start position.   */
   /*   |------------------------|  New text alignment. */
   /* case (1) |---------------|*/
   /* case (2) |---------------------------------|  */
   /* case (3) |----------------------------------------|   */
   /*case (1): end of existent txt fmt is less than (in front of) end of new txt alignment. */
   /*case (2): end of existent txt fmt is equal to end of new txt alignment.*/
   /*case (3): end of existent txt fmt is more than (behind) end of new txt alignment.  */
   /* 2. The start of existent txt fmt is equal to  start position.   */
   /*  |------------------------|  New text alignment. */
   /* case (4) |---------------|   */
   /* case (5) |------------------------|  */
   /* case (6) |--------------------------- ---|   */
   /*case (4): end of existent txt fmt is less than (in front of) end of new txt alignment. */
   /*case (5): end of existent txt fmt is equal to end of new txt alignment.*/
   /*case (6): end of existent txt fmt is more than (behind) end of new txt alignment.  */
   /* 3. The start of existent txt fmt is more than (behind) to  start position.   */
   /*|------------------------------|  New text alignment. */
   /* case (7) |---------------|   */
   /* case (8) |------------------------|  */
   /* case (9) |--------------------------- ---|   */
   /*case (7): end of existent txt fmt is less than (in front of) end of new txt alignment. */
   /*case (8): end of existent txt fmt is equal to end of new txt alignment.*/
   /*case (9): end of existent txt fmt is more than (behind) end of new txt alignment.  */

   if(txt_fmt_obj_list != NULL)
   {
  q_node = txt_fmt_obj_list;
  while(q_node != NULL)
  {
 obj = q_node->obj;

 if (obj->OffsetToText < start_position)
 {
if(TXT_FMT_OBJ_END(obj) <= start_position + remain_len )
{
   /* case (1) (2) */
   kal_uint16 new_fmt_len = TXT_FMT_OBJ_END(obj) - start_position;

   /* change the length of old one. */
   TXT_FMT_LEN(obj) = start_position - EMS_OBJ_OFFSET(obj);

   ems_mem_cpy(&txt_fmt_tmp,& (obj->data->text_format) ,  sizeof(EMSTextFormat));
   txt_fmt_tmp.textLength = new_fmt_len ;

   newObj = AddTextFmtObjByOffset(emsData, start_position, &txt_fmt_tmp);
   if(newObj == NULL)
   {
  ret = EMS_NO_MEMORY;
  goto Add_Fmt_No_mem;
   }
   /* The old one is already in existent queue. */
   /* replace it by new one. */
   q_node->obj = newObj ;

   /* set the q_node to type merge_prev_one */

   start_position += TXT_FMT_LEN(newObj) ;
   remain_len -= TXT_FMT_LEN(newObj) ;

}
else if(TXT_FMT_OBJ_END(obj) > start_position + remain_len )
{
   /* case (3) */
   kal_uint16 end_old_fmt = TXT_FMT_OBJ_END(obj) ;

   /* There shall be no any other txt fmt. */
   EMS_ASSERT (q_node->next == NULL);

   /* change the length of old one. */
   TXT_FMT_LEN(obj) = start_position - EMS_OBJ_OFFSET(obj);

   ems_mem_cpy(&txt_fmt_tmp,& (obj->data->text_format) ,  sizeof(EMSTextFormat));
   txt_fmt_tmp.textLength = remain_len ;

   newObj = AddTextFmtObjByOffset(emsData, start_position, &txt_fmt_tmp);
   if(newObj == NULL)
   {
  ret = EMS_NO_MEMORY;
  goto Add_Fmt_No_mem;
   }
   /* The old one is already in existent queue. */
   /* replace it by new one. */
   q_node->obj = newObj ;

   txt_fmt_tmp.textLength = end_old_fmt - (start_position+ remain_len)  ;
   newObj = AddTextFmtObjByOffset(emsData, (start_position + remain_len), &txt_fmt_tmp);
   if(newObj == NULL)
   {
  ret = EMS_NO_MEMORY;
  goto Add_Fmt_No_mem;
   }

   /* set the q_node to type merge_prev_one & next one */

   start_position += remain_len ;
   remain_len -= remain_len ;
}
else
{
   EMS_ASSERT(KAL_FALSE);
}


 }
 else if(obj->OffsetToText == start_position)
 {
if(TXT_FMT_OBJ_END(obj) <= start_position + remain_len )
{
   /* case (4),(5) */
   start_position += TXT_FMT_LEN(obj);
   remain_len -= TXT_FMT_LEN(obj);
}
else if(TXT_FMT_OBJ_END(obj) > start_position + remain_len )
{
   /* case (6) */
   kal_uint16 end_old_fmt = TXT_FMT_OBJ_END(obj) ;

   /* There shall be no any other txt fmt. */
   EMS_ASSERT (q_node->next == NULL);

   /* change the length of old one. */
   TXT_FMT_LEN(obj) = remain_len;

   ems_mem_cpy(&txt_fmt_tmp,& (obj->data->text_format) ,  sizeof(EMSTextFormat));
   txt_fmt_tmp.textLength = end_old_fmt - TXT_FMT_OBJ_END(obj);

   newObj = AddTextFmtObjByOffset(emsData, start_position + remain_len, &txt_fmt_tmp);
   if(newObj == NULL)
   {
  ret = EMS_NO_MEMORY;
  goto Add_Fmt_No_mem;
   }

   /* set the q_node to type merge_next_one */
   start_position += remain_len ;
   remain_len -= remain_len ;
}
else
{
   EMS_ASSERT(KAL_FALSE);
}
 }
 else /* obj->OffsetToText > start_position */
 {
/* case (7),(8),(9) */
ems_mem_cpy(&txt_fmt_tmp, txt_fmt, sizeof(EMSTextFormat));

txt_fmt_tmp.textLength = obj->OffsetToText - start_position ;
newObj = AddTextFmtObjByOffset(emsData, start_position, &txt_fmt_tmp);
if(newObj == NULL)
{
   ret = EMS_NO_MEMORY;
   goto Add_Fmt_No_mem;
}

if ((ret = AddToObjQueue(&new_alignment_obj_list ,newObj)) == EMS_NO_MEMORY )
   goto Add_Fmt_No_mem;

start_position += txt_fmt_tmp.textLength;
remain_len -= txt_fmt_tmp.textLength;

if(TXT_FMT_OBJ_END(obj) <= start_position + remain_len )
{
   /* case (7),(8) */
   start_position += TXT_FMT_LEN(obj);
   remain_len -= TXT_FMT_LEN(obj);
}
else if(TXT_FMT_OBJ_END(obj) > start_position + remain_len )
{
   /* case (9) */
   kal_uint16 end_old_fmt = TXT_FMT_OBJ_END(obj) ;

   /* There shall be no any other txt fmt. */
   EMS_ASSERT (q_node->next == NULL);

   /* change the length of old one. */
   TXT_FMT_LEN(obj) = remain_len;

   ems_mem_cpy(&txt_fmt_tmp,& (obj->data->text_format) ,  sizeof(EMSTextFormat));
   txt_fmt_tmp.textLength = end_old_fmt - TXT_FMT_OBJ_END(obj);

   newObj = AddTextFmtObjByOffset(emsData, start_position + remain_len, &txt_fmt_tmp);
   if(newObj == NULL)
   {
  ret = EMS_NO_MEMORY;
  goto Add_Fmt_No_mem;
   }

   /* set the q_node to type merge_next_one */

   start_position += remain_len ;
   remain_len -= remain_len ;
}
 }

 q_node = q_node->next;
  }

  if (offsetToText + txt_fmt->textLength > start_position)
  {

 EMS_ASSERT(remain_len > 0);

 ems_mem_cpy(&txt_fmt_tmp, txt_fmt ,  sizeof(EMSTextFormat));
 txt_fmt_tmp.textLength = offsetToText + txt_fmt->textLength - start_position ;

 newObj = AddTextFmtObjByOffset(emsData, start_position, &txt_fmt_tmp);
 if(newObj == NULL)
 {
ret = EMS_NO_MEMORY;
goto Add_Fmt_No_mem;
 }

 if ( (ret = AddToObjQueue(&new_alignment_obj_list ,newObj)) == EMS_NO_MEMORY )
goto Add_Fmt_No_mem;

 start_position += txt_fmt_tmp.textLength;
 remain_len -= txt_fmt_tmp.textLength;
  }

  EMS_ASSERT(remain_len == 0);
  EMS_ASSERT(start_position == (offsetToText + txt_fmt->textLength));
   }
   else
   {
  /* case (10) there is no existent txt fmt in the message. */

  newObj = AddTextFmtObjByOffset(emsData, offsetToText, txt_fmt);
  if(newObj == NULL)
  {
 ret = EMS_NO_MEMORY;
 goto Add_Fmt_No_mem;
  }


  if ((ret = AddToObjQueue(&new_alignment_obj_list ,newObj)) == EMS_NO_MEMORY )
 goto Add_Fmt_No_mem;
   }
  

   UpdateLenInfo(emsData);
   EMSSetUDHI(emsData);

   ret = EMSCalculateLength(emsData);

   if(ret == EMS_OK)
   {
  EMSObject   *tmp_obj;
  EMSObject   *last_obj = NULL;

  /* update the ovlapping existent txt fmt. */
  newObj = RemoveFromObjQueue(&txt_fmt_obj_list);
  while(newObj != NULL)
  {

 newObj->data->text_format.Alignment = txt_fmt->Alignment;
 if (KAL_TRUE == isDefaultTF(&(newObj->data->text_format )))
 {
if (newObj == emsData->CurrentPosition.Object)
   emsData->CurrentPosition.Object = emsData->CurrentPosition.Object->prev;

DeleteEMSObjectFromList(emsData, newObj);
need_repack = KAL_TRUE;
newObj = NULL;
 }
 else if (isResetTxtFmt == KAL_TRUE)
 {
tmp_obj= newObj->prev;
GET_PREV_TXT_FMT_OBJ(tmp_obj);   

if (KAL_TRUE == MergeTF(emsData, tmp_obj, newObj))
{
   need_repack = KAL_TRUE;
   newObj = NULL;
}
 }

 last_obj = newObj;
 newObj = RemoveFromObjQueue(&txt_fmt_obj_list);
  }

  if(last_obj != NULL && isResetTxtFmt == KAL_TRUE)
  {
 tmp_obj = obj->next;
 GET_NEXT_TXT_FMT_OBJ(tmp_obj);   

 if (KAL_TRUE == MergeTF(emsData, last_obj, tmp_obj))
   need_repack = KAL_TRUE;
  }
   }
   else
   {

Add_Fmt_No_mem:
  /* rollback */
  newObj = RemoveFromObjQueue(&new_alignment_obj_list);
  while(newObj != NULL)
  {
 if (newObj == emsData->CurrentPosition.Object)
emsData->CurrentPosition.Object = emsData->CurrentPosition.Object->prev;

 DeleteEMSObjectFromList(emsData, newObj);
 newObj = RemoveFromObjQueue(&new_alignment_obj_list);
  }

  obj = RemoveFromObjQueue(&txt_fmt_obj_list);
  while(obj != NULL)
  {
 EMSObject   *prev, *next;;

 prev = obj->prev;
 GET_PREV_TXT_FMT_OBJ(prev);   

 next = obj->next;
 GET_NEXT_TXT_FMT_OBJ(next);   

 if (KAL_TRUE == MergeTF(emsData, prev, obj))
obj= prev;

 MergeTF(emsData, obj, next);

 obj = RemoveFromObjQueue(&txt_fmt_obj_list);
  }

  need_repack = KAL_TRUE;
   }

   if (need_repack == KAL_TRUE)
   {
  UpdateLenInfo(emsData);
  EMSSetUDHI(emsData);

  roll_ret=EMSCalculateLength(emsData);
  if (roll_ret!=EMS_OK)
 ASSERT(roll_ret==EMS_OK);
   }
   
   emsData->CurrentTextFormatObj = NULL;
   SetCurrTxtFmtObj(emsData);

   return ret;
#endif

}

/*****************************************************************************
* FUNCTION
*  EMSPack
* DESCRIPTION
*   This function encodes user data header for EMS.
*
* PARAMETERS
*  a  IN  *emsData
*  b  IN/OUT  *numOfMsg, number of segments required
*  c  IN/OUT  *TPUD[]
*  d  IN/OUT		TPUDLen[]
* RETURNS
*  EMSTATUS
* GLOBALS AFFECTED
*  none
*****************************************************************************/
EMSTATUS EMSPack(EMSData   *emsData,
 kal_uint8 need_new_ref,
 kal_uint8 *numOfMsg,
 kal_uint8 *TPUD[],
 kal_uint8		 TPUDLen[])
{
   EMSTATUS ret;
   EMSInternalData *internal;

   EMS_DATA_CHECK(emsData);

   /* sanity check */
   if((emsData == NULL) || (TPUDLen == NULL)) 
  return EMS_NULL_POINTER;
   
   ASSERT(emsData->internal != NULL);

   SET_ACTIVE_EMS_ID(emsData->id);

   internal = (EMSInternalData*)emsData->internal;


#if defined(EMS_UT) 
   //EMSDataDump(emsData);
#endif

   ret = EMSInternalPack(emsData, TPUD, KAL_TRUE /* pack */);

   if(ret == EMS_OK)
   {
  kal_uint8 conc_iei;
  kal_uint8 conc_iei_len;
  kal_uint8 seg;
  kal_uint16 conc_mr; /* concatenated message reference */
  kal_uint8 udh_offset;

  updateEMSReminder(emsData);
  
  if(emsData->isConcat == KAL_TRUE)
  {
 if (need_new_ref == KAL_TRUE)
 {
/* get concat. message reference */
/* If callback function is not registered, use the sequential msg_ref. */
if(EMS_CNTX(concat_msg_ref_func) != NULL)
   conc_mr = (*(EMS_CNTX(concat_msg_ref_func)))();
else
   conc_mr = EMSGetConctMsgRef(); 

internal->CurrentConcatMsgRef= conc_mr; 
 }
 else
conc_mr = internal->CurrentConcatMsgRef; 
  }

#ifdef __EMS_REL5__
  {
 conc_iei = EMS_CONC16_MSG_IEI;
 conc_iei_len = 6 ; /* 2 + 4 */
  }
#else
  {
 conc_iei = EMS_CONC8_MSG_IEI;
 conc_iei_len = 5 ; /* 2 + 3 */
  }
#endif

  for( seg = 0 ; seg < emsData->Reminder.requiredSegment ; seg++)
  {
 udh_offset = 0;

 /* if UDH present, write UDHL which placed in 1st byte of TPUD */
 if(emsData->udhi == KAL_TRUE)
 {
*(TPUD[seg]) = internal->UDHL[seg]-1;
udh_offset ++;
 }


 /* if there is port, write the port IE */
 if(emsData->PortNum.isPortNumSet == KAL_TRUE)
 {
MakePortHdr(TPUD[seg]+udh_offset,
emsData->PortNum.src_port,
emsData->PortNum.dst_port);

/* 16-bits Port number IE length = 6 bytes. */
udh_offset += 6; 
 }

 /* if msg is concatenated, write the concatenated IE */
 if(emsData->isConcat == KAL_TRUE)
 {
MakeConcatHdr(TPUD[seg]+ udh_offset, 
  conc_iei,
  conc_mr, 
  emsData->Reminder.requiredSegment, 
  (kal_uint8)(seg+1));
 
/* Concat. IE length . */
udh_offset += conc_iei_len ;
 }

 TPUDLen[seg] = internal->TPUDLen[seg];
  } /* for */ 
   
  *numOfMsg = emsData->Reminder.requiredSegment;
   }

   return ret;
   
} /* end of EMSPack */



EMSTATUS EMSUnPack(EMSData *emsData,
   kal_uint8  udhi,
   kal_uint8  numOfMsg,
   kal_uint8  *TPUD[],
   kal_uint8  TPUDLen[])
{
   kal_uint8 seg;
   EMSTATUS ret = EMS_OK;/*reset*/

   EMSInternalData *i = (EMSInternalData*) emsData->internal;

   EMS_DATA_CHECK(emsData);

   emsData->udhi = udhi;

   /* sanity checks  */
   if((numOfMsg == 0) ||
  ((emsData->udhi == KAL_FALSE) && (numOfMsg > 1)))
   {
  return EMS_UNSPECIFIED_ERROR;
   }

   if((emsData == NULL) || (TPUDLen == NULL) || (TPUD == NULL))
   {
  return EMS_NULL_POINTER;
   }

   /* we cannot allow all segments missed!! */
   for(seg = 0 ; seg < numOfMsg ; seg++)
  if(TPUD[seg] != NULL) break;

   if(seg >= numOfMsg)
 return EMS_NULL_POINTER;

   SET_ACTIVE_EMS_ID(emsData->id);

   ASSERT(i != NULL);

   InitializeEMSInternalData(i, TPUD);

#ifdef __EMS_REL5__
   i->UnpackSegNum= numOfMsg;
#endif

   /* for(seg = 0 ; seg < numOfMsg && seg < EMS_MAX_SEG_NUM ; seg++) */
   for(seg = 0 ; seg < numOfMsg ; seg++)
   {
  EMS_PRINT("processing seg: %d ", seg);

  if(TPUD[seg] == NULL)
  {
 EMS_PRINT("[missed seg]\n",seg);
 /* increment text length for missed segment delimiter */
 i->baseTextOffset += EMSInsertMissSegStr(emsData->textBuffer + i->baseTextOffset);

 #ifdef __EMS_REL5__
 i->MissSegInCrossIE = KAL_TRUE;
 #endif
  }
  else
  {
 kal_uint16 txt_len_in_TPUD;
 kal_uint16 txt_len_in_buff;
 kal_bool   exceed_text_buff = KAL_FALSE ;

 
 /* in case of problematic network-side equipment that give us the  */
 /* "zero" User Data Header Length, we need to skip the UDHL*/
 if((emsData->udhi == KAL_TRUE) && (*(TPUD[seg]) > 0))
 {
if(*(TPUD[seg]) >= EMS_ONE_MSG_OCTET) /* UDHL is invalid */
{
   /* Invalid Segment, deal with as missed segment. */

   /* increment text length for missed segment delimiter */
   i->baseTextOffset += EMSInsertMissSegStr(emsData->textBuffer + i->baseTextOffset);

   #ifdef __EMS_REL5__
   i->MissSegInCrossIE = KAL_TRUE;
   #endif
   continue;
}

i->UDHL[seg] = *(TPUD[seg]) + 1;
 }
 else
 {
/* only one segment and only text */
i->UDHL[seg] = 0;
 }

 EMS_PRINT("[UDHL = 0x%x]\n", i->UDHL[seg]);

 if(TPUDLen[seg] < i->UDHL[seg])
break;

 txt_len_in_TPUD = TPUDLen[seg] - i->UDHL[seg];

 if ( emsData->dcs == SMSAL_UCS2_DCS && (txt_len_in_TPUD % 2) != 0 )	
txt_len_in_TPUD --;

 txt_len_in_buff = GetTxtLenInOctet(emsData->dcs, txt_len_in_TPUD );

 /* check if the text buffer is full. */
 if((i->baseTextOffset + txt_len_in_buff ) > EMS_MAX_TEXT_BUF_SIZE)
 {
exceed_text_buff  = KAL_TRUE ;
txt_len_in_buff = EMS_MAX_TEXT_BUF_SIZE - i->baseTextOffset ;

/* re-assign legal length of TPUD */
txt_len_in_TPUD = GetTxtLenInChar(emsData->dcs, txt_len_in_buff );
 }

 /* in case of problematic network-side equipment that give us the  */
 /* "zero" User Data Header Length, we need to skip to decode UDH   */
 if((emsData->udhi == KAL_TRUE) && (*(TPUD[seg]) > 0))
 {
/* process User Data Header */
if(EMSDecodeUDH(emsData, TPUD[seg], txt_len_in_TPUD ) != EMS_OK)
   break;
 }

 /* add text if presented */
 if (txt_len_in_buff > 0  )
 {
EMSTextCpy(emsData->textBuffer+ i->baseTextOffset, /* desc */
  TPUD[seg] + i->UDHL[seg],  /* src */
  txt_len_in_TPUD ,  /* length */
  emsData->dcs,
  KAL_FALSE  /* unpack */
  );
 }

 /* increment text length */
 i->baseTextOffset += txt_len_in_buff ;

 /* stop next seg. decoding if text buffer is exceeded. */
 if(exceed_text_buff == KAL_TRUE) 
break;

  }
   } /* for */


#ifndef __SLIM_EMS__
#ifdef __EMS_REL5__
   EMSCheckExtTextOffset(emsData);
   EMSFreePBuff(&(i->PBuff));
#endif
#endif

   emsData->textLength = i->baseTextOffset;

#ifndef __SLIM_EMS__
   {
  EMSObject *obj, *next;
  obj = emsData->listHead ;

  while (obj != NULL )
  {
 if (obj->Type == EMS_TYPE_TEXT_FORMAT)
 {
next = obj-> next ;

while(next != NULL &&
  next->Type != EMS_TYPE_TEXT_FORMAT &&
  next->OffsetToText <  obj->OffsetToText+TXT_FMT_LEN(obj) &&
  next->OffsetToText == obj->OffsetToText )
{
   EMSSwapObject(emsData, obj, next);
   next = obj->next;
}

next = obj->next ;

GET_NEXT_TXT_FMT_OBJ(next);   
if (KAL_FALSE == MergeTF(emsData, obj, next))
{
   obj = next;
}
 }
 else
 {
obj = obj->next;
 }
  }
   }
#endif

   if(numOfMsg > 1)		
  emsData->isConcat = KAL_TRUE;
   else
  emsData->isConcat = KAL_FALSE;
   
   /* 2004-11-15, MT msg may include non-support object.  */
   /* If all objects are not support, the UDHI flag shall be  */
   /* reset for forwarding action in MMI. */
   UpdateLenInfo(emsData);
   EMSSetUDHI(emsData);

   /* update reminder info. */
   EMSCalculateLength(emsData);

   ResetCurrentPosition(emsData);

   EMSAddNullTerminator(emsData->textBuffer + emsData->textLength);

#if defined(EMS_UT)
   //EMSDataDump(emsData);
#endif

   return ret;

} /* end of EMSUnPack */


EMSTATUS EMSMsgPreview(kal_uint8 numOfMsg,
			  kal_uint8 dcs,
   kal_uint8 udhi,
   kal_uint8 *TPUD[],
   kal_uint8 TPUDLen[],
   kal_uint16	BuffLen,/* octets */ 
   kal_uint8 *textBuff,
   kal_uint8 *is_obj_present, 
   kal_uint16*textLen)   /* octets */
{
   kal_uint8 seg;
   EMSTATUS ret = EMS_OK;/*reset*/
   kal_uint16	return_textLen=0;  /* octets */
   kal_uint8	UDHL[10 ];
   kal_bool		is_obj_exist = KAL_FALSE;

   if((numOfMsg == 0) ||
  ((udhi == KAL_FALSE) && (numOfMsg > 1)))
   {
	   return EMS_UNSPECIFIED_ERROR;
   }

   if((TPUDLen == NULL) || (TPUD == NULL))
   {
	   return EMS_NULL_POINTER;
   }

   if (numOfMsg > 10 )
	   numOfMsg =  10 ;

   if( BuffLen > 0 && (BuffLen % 2) != 0 )
	   BuffLen -- ;

   for(seg = 0 ; seg < numOfMsg ; seg++)
   {
	   if(TPUD[seg] == NULL)
	   {
		   if (return_textLen + EMSInsertMissSegStr(NULL) > BuffLen)
			   break;
		   
		   if(textBuff !=NULL)
			   return_textLen += EMSInsertMissSegStr(textBuff + return_textLen);
  }
  else
  {
	  kal_uint16 txt_len_in_TPUD;
	  kal_uint16 txt_len_in_buff;
	  kal_bool   exceed_text_buff = KAL_FALSE ;

	  if((udhi == KAL_TRUE) && (*(TPUD[seg]) > 0))
	  {
		  if(*(TPUD[seg]) >= EMS_ONE_MSG_OCTET) /* UDHL is invalid */
			  return EMS_UNSPECIFIED_ERROR;

		  UDHL[seg] = *(TPUD[seg]) + 1;
	  }
	  else
	  {
		  /* only one segment and only text */
		  UDHL[seg] = 0;
	  }

	  txt_len_in_TPUD = TPUDLen[seg] - UDHL[seg];

	  if ( dcs == SMSAL_UCS2_DCS && (txt_len_in_TPUD % 2) != 0 )	
		  txt_len_in_TPUD --;

	  txt_len_in_buff = GetTxtLenInOctet(dcs, txt_len_in_TPUD );

 /* check if the text buffer is full. */
	  if((return_textLen + txt_len_in_buff ) > BuffLen)
	  {
		  exceed_text_buff  = KAL_TRUE ;
		  txt_len_in_buff = BuffLen - return_textLen  ;

		  /* re-assign legal length of TPUD */
		  txt_len_in_TPUD = GetTxtLenInChar(dcs, txt_len_in_buff );
	  }

	  if((udhi == KAL_TRUE) && (*(TPUD[seg]) > 0))
	  {
		  /* process User Data Header */
		  if(is_obj_exist == KAL_FALSE)
			  is_obj_exist = EMSDetectObj(TPUD[seg]); 
	  }

 /* add text if presented */
	  if( (txt_len_in_buff > 0) && (textBuff !=NULL))
	  {

		  EMSTextCpy(textBuff + return_textLen,/* dest */
					 TPUD[seg] + UDHL[seg],/* src */
					 txt_len_in_TPUD , /* length in chars */ 
					 dcs,
					KAL_FALSE /* unpack */
					 );
	  }

 /* increment text length */
	  return_textLen += txt_len_in_buff ;

 /* stop next seg. decoding if text buffer is exceeded. */
	  if(exceed_text_buff == KAL_TRUE) 
		  break;

	}
   }

   if(is_obj_present != NULL)
	   *is_obj_present = is_obj_exist ;

   if(textLen != NULL)
	   *textLen = return_textLen;

   return ret;

}

EMSTATUS EMSSetMsgClass(EMSData *emsData, kal_uint8 msg_class)
{

   if (msg_class >= SMSAL_CLASS_UNSPECIFIED)
  return EMS_UNSPECIFIED_ERROR;

   emsData->ps_dcs &= 0x7c;   /* reset bit 0, 1, 4 */
   emsData->ps_dcs |= 0x10;   /* set bit 4, message class present */
   emsData->ps_dcs |= (msg_class & 0x03); /* set bit 0, 1 */

   return EMS_OK;
}

EMSTATUS EMSSetDCS(EMSData *emsData, kal_uint8 dcs)
{
   EMSTATUS ret;
   kal_uint8 ori_dcs = emsData->dcs;

   EMS_DATA_CHECK(emsData);

   if(emsData == NULL) /* sanity check */
  return EMS_NULL_POINTER;

   /* check data coding scheme */
   if((dcs != SMSAL_DEFAULT_DCS) &&
  (dcs != SMSAL_UCS2_DCS) &&
  (dcs != SMSAL_8BIT_DCS))
   {
  return EMS_INVALID_DCS;
   }


   if (ori_dcs == dcs)
  return EMS_OK;

   SET_ACTIVE_EMS_ID(emsData->id);

   ASSERT(emsData->internal != NULL);

   emsData->dcs = dcs;
   emsData->ps_dcs &= 0xf3; /* reset bit 2, 3 */
   emsData->ps_dcs |= (dcs & 0x0c); /* set bit 2, 3 */

   UpdateLenInfo(emsData);
   EMSSetUDHI(emsData);
   ret = EMSCalculateLength(emsData);

   /* rollback */
   if(ret != EMS_OK)
   {
  EMSTATUS r;

  emsData->dcs = ori_dcs;
  emsData->ps_dcs &= 0xf3; /* reset bit 2, 3 */
  emsData->ps_dcs |= (ori_dcs & 0x0c); /* set bit 2, 3 */

  UpdateLenInfo(emsData);
  EMSSetUDHI(emsData);
  r = EMSCalculateLength(emsData);
  
  ASSERT(r==EMS_OK);
   }

   return ret;

} /* end of EMSSetDCS */

EMSTATUS EMSResetPortNum(EMSData *emsData)
{

   EMSTATUS ret;

   EMS_DATA_CHECK(emsData);

   if(emsData == NULL) /* sanity check */
  return EMS_NULL_POINTER;

   if(emsData->PortNum.isPortNumSet != KAL_TRUE)
   {
  return EMS_UNSPECIFIED_ERROR;
   }
   else 
   {
  emsData->PortNum.isPortNumSet = KAL_FALSE ;

  UpdateLenInfo(emsData);
  EMSSetUDHI(emsData);

  ret=EMSCalculateLength(emsData);

  ASSERT(ret==EMS_OK);

  return ret ;
   }
}

EMSTATUS EMSSetPortNum(EMSData *emsData, kal_uint16 src_port, kal_uint16 dst_port)
{
   EMSTATUS ret;

   EMS_DATA_CHECK(emsData);

   if(emsData == NULL) /* sanity check */
  return EMS_NULL_POINTER;


   /* reserved Port number */
   if((src_port >= 17000)|| 
  (dst_port >= 17000))
   {
  return EMS_INVALID_PORT_NUM;
   }
#ifdef __EMS_REL5__
   {
  EMSObject *obj = emsData->listHead;

  while (obj != NULL)
  {
 if ((IS_TXT_FMT_OBJ (obj) != KAL_TRUE) &&
 (IS_EMS_DATA_NEED(obj->Type) == KAL_TRUE) &&
 (EXT_OBJ_INFO_P(obj, isExtObj)== KAL_FALSE))
 {
if ((obj->data->common.pdu_length + 5 ) > 128)
   return EMS_UNSPECIFIED_ERROR;
 }
 obj = obj->next;
  }
   }
#endif


   SET_ACTIVE_EMS_ID(emsData->id);

   ASSERT(emsData->internal != NULL);

   /* new setting , need to update len info. */
   if ( emsData->PortNum.isPortNumSet == KAL_FALSE)
  emsData->PortNum.isPortNumSet = KAL_TRUE;

   emsData->PortNum.src_port = src_port;
   emsData->PortNum.dst_port = dst_port;

   UpdateLenInfo(emsData);
   EMSSetUDHI(emsData);
   
   ret=EMSCalculateLength(emsData);
		
   /* rollback */
   if(ret != EMS_OK)
   {
  emsData->PortNum.isPortNumSet = KAL_FALSE ;

  /* reset len info to default, only concat. */
  UpdateLenInfo(emsData);
   }

   return ret;

} /* end of EMSSetPortNum */

kal_bool GetUDHIBit(kal_uint8 fo)
{
   if ((fo & 0x40) >> 6)
  return KAL_TRUE;
   else
  return KAL_FALSE;
}


EMSTATUS CopyEMSData(EMSData *dest, EMSData *src)
{   
#ifndef __SLIM_EMS__
   EMSObject *obj=NULL, *newObj=NULL, *curr_obj=NULL;
#endif
   EMSInternalData *i1, *i2;
   EMSTATUS ret;
   
   EMS_DATA_CHECK(src);
   EMS_DATA_CHECK(dest);


   if(dest == NULL || dest->internal == NULL || src == NULL || src->internal == NULL)
   {
  return EMS_NULL_POINTER;
   } 
   
   
   SET_ACTIVE_EMS_ID(dest->id);


   i1 = (EMSInternalData*)dest->internal;
   i2 = (EMSInternalData*)src->internal;

   /* ems_mem_set(dest->internal, 0, sizeof(EMSInternalData)); */
   i1->totalTxtFmtOctet= 0;
   i1->totalObjOctet   = 0;
   i1->numOfEMSObject  = 0;
#ifdef __EMS_REL5__
   i1->totalExtObjOctet= 0;
   i1->numOfExtObject  = 0;

   i1->needReCompress  = KAL_TRUE;
   i1->ExtObjRefNum = i2->ExtObjRefNum;
   i1->forceCompress= i2->forceCompress;
#endif

   i1->MaxLenPerSeg = i2->MaxLenPerSeg;
   i1->MaxUsableLen = i2->MaxUsableLen;
   i1->ConcatHdrLen = i2->ConcatHdrLen;

   /* copy the ems data element */
   dest->dcs= src->dcs;
   dest->ps_dcs = src->ps_dcs;
   dest->isConcat   = src->isConcat;
   dest->udhi   = src->udhi;
   dest->textLength = src->textLength;

   ems_mem_cpy((kal_uint8*)&dest->Reminder, (kal_uint8*)&src->Reminder, sizeof(EMSReminder));
   dest->CurrentPosition.OffsetToText = src->CurrentPosition.OffsetToText;
 
   ASSERT(dest->textBuffer != NULL);
   ASSERT(dest->textLength <= EMS_MAX_SEG_NUM *153*2);

   /* copy text buffer */   
   if(src->textLength > 0)
  ems_mem_cpy(dest->textBuffer, src->textBuffer, src->textLength);


   /* copy objects */
   dest->listHead = dest->listTail = NULL;   

#ifdef __SLIM_EMS__
   EMS_ASSERT (src->listHead == NULL);
#else
   for(obj = src->listHead ; obj ; obj = obj->next)
   {
#ifdef __EMS_REL5__
  if(IS_EMS_DATA_NEED(obj->Type) != KAL_FALSE)
  {
 if (obj->data->common.attribute == EMS_ATTB_NOT_FW)
 {
ret = EMS_IGNORE_NOT_FW_OBJ;
continue;
 }
  }

  if ( EXT_OBJ_INFO_P(obj, isReusedObj) == KAL_TRUE)
  {
 newObj = CopyResuedObject(dest, obj);

 if(newObj == NULL) 
continue;
  }
  else
#endif
  {
 newObj = CopyEMSObject(obj);

 if(newObj == NULL) 
return EMS_NO_MEMORY;  
  }

  AddObjectIntoList(dest, newObj);

  /* cause the next new object added in tail */
  dest->CurrentPosition.Object = newObj;  

  if((kal_uint32*)obj == (kal_uint32*)src->CurrentTextFormatObj)
 dest->CurrentTextFormatObj = newObj;   

  if((kal_uint32*)obj == (kal_uint32*)src->CurrentPosition.Object)
 curr_obj = newObj; /* save */
   }

   dest->CurrentPosition.Object = curr_obj;
#endif
  
   UpdateLenInfo(dest);
   EMSSetUDHI(dest);
   ret =EMSCalculateLength(dest);
  
   EMSAddNullTerminator(dest->textBuffer + dest->textLength);
  
   return ret;

} /* end of CopyEMSData */

void EMSConcatMsgRefFunc(EMS_CONCAT_MSG_REF_FUNCTION func)
{
   EMS_CNTX(concat_msg_ref_func) = func;   
}

/*****************************************************************************
* FUNCTION
*  EMSGetNextObject
* DESCRIPTION
*   This function must return the object at the given position and 
*   advance the given position to point to the next object or character.
*
* PARAMETERS
*  a  IN  data, the EMS data structure
*  b  IN/OUT  p, position in the EMS data structure 
* at which the object is to be retreived.
*  c  IN/OUT  c, if the object at current position is a character, 
* it is returned through this pointer.
*  d  IN/OUT  o, If the object at the current position 
* is an EMSObject, the pointer to the EMSObject 
* is returned through this pointer.
*
* RETURNS
*  0invalid position / data
*  1the object returned was a character
*  2the object returned was an EMSObject
*
* GLOBALS AFFECTED
*  none
*****************************************************************************/
kal_uint8 EMSGetNextObject(EMSData *data, 
   EMSPosition *p, 
   kal_uint16  *c, 
   EMSObject   **o)
{
   kal_uint8 return_type = 0; /* reset */
   kal_uint8 check_char = 0;  /* reset */

   if(data == NULL || p == NULL)
  return 0;
   
   if(p->Object != NULL)
   {
  if(p->Object->next != NULL && p->Object->next->OffsetToText == p->OffsetToText)
  {
 return_type = 2;
 *o = p->Object->next;
 p->Object = p->Object->next;
  }
  else
 check_char = 1;
  
}
else if(data->listHead != NULL)
{
   if(data->listHead->OffsetToText == p->OffsetToText)
   {
  return_type = 2;
  *o = data->listHead;
  p->Object = data->listHead;
   }
   else
  check_char = 1;
   }
   else
  check_char = 1;

   if(check_char == 1 && p->OffsetToText < data->textLength)
   {	
  return_type = 1;
#ifdef EMS_LITTLE_ENDIAN
  *c = (data->textBuffer[p->OffsetToText] | (data->textBuffer[p->OffsetToText+1]<<8));
#else
  *c = (data->textBuffer[p->OffsetToText+1] | (data->textBuffer[p->OffsetToText]<<8));
#endif
  p->OffsetToText += 2;
   }

   return return_type;

} /* end of EMSGetNextObject */

/*****************************************************************************
* FUNCTION
*  EMSGetPreviousObject
* DESCRIPTION
*   This function must return the object that occurs one position 
*   before the given position and move the given position to point 
*   the previous object or character.
*
* PARAMETERS
*  a  IN  data, the EMS data structure
*  b  IN/OUT  p, position in the EMS data structure 
* at which the object is to be retreived.
*  c  IN/OUT  c, if the object at current position is a character, 
* it is returned through this pointer.
*  d  IN/OUT  o, If the object at the current position 
* is an EMSObject, the pointer to the EMSObject 
* is returned through this pointer.
*
* RETURNS
*  0invalid position / data
*  1the object returned was a character
*  2the object returned was an EMSObject
*
* GLOBALS AFFECTED
*  none
*****************************************************************************/
kal_uint8 EMSGetPreviousObject(EMSData *data, 
   EMSPosition *p, 
   kal_uint16  *c, 
   EMSObject   **o)
{
   if(p->Object != NULL && p->Object->OffsetToText == p->OffsetToText)
   {		
  *o = p->Object;
  p->Object = p->Object->prev;		
  return 2; /* object */
   }
   else if(p->OffsetToText <= data->textLength && p->OffsetToText >= 2)
   {			
#ifdef EMS_LITTLE_ENDIAN
  *c=(data->textBuffer[p->OffsetToText-2]|(data->textBuffer[p->OffsetToText-1]<<8));
#else
  *c=(data->textBuffer[p->OffsetToText-1]|(data->textBuffer[p->OffsetToText-2]<<8));
#endif
  p->OffsetToText -= 2;
  return 1; /* character */
   }

   return 0; /* invalid pos */

} /* end of EMSGetPreviousObject */

kal_int32 EMSGetObjectSize ( kal_uint8 type, EMSObjData *objdata )
{
   if(IS_EMS_DATA_NEED(type) == KAL_TRUE)
  ASSERT( objdata != NULL );

   return getObjLength((EMSTYPE)type, objdata, 0, NULL);

}

kal_int32 EMSGetObjectSizeWithCheck ( kal_uint8 type, EMSObjData *objdata, kal_bool *isAllow )
{
   if(IS_EMS_DATA_NEED(type) == KAL_TRUE)
  ASSERT( objdata != NULL );
   
   return getObjLength((EMSTYPE)type, objdata, 0, isAllow);
}

EMSTATUS EMSSetCompressMode(kal_bool mode)
{
#ifdef __EMS_REL5__
   EMS_CNTX(isCompressSupp) = mode;
#endif
   return EMS_OK;
}

kal_bool EMSIsReusedObj (EMSData *emsData, EMSObject *obj_q)
{
#ifdef __EMS_REL5__
   EMSObject *obj = emsData->listHead;
   kal_bool is_obj_in_list = KAL_FALSE;

   while (obj != NULL)
   {
  if ( obj == obj_q )
  {
 is_obj_in_list = KAL_TRUE ;
 break;
  }
  obj = obj->next;
   }

   if (is_obj_in_list == KAL_TRUE)
   {
  if( EXT_OBJ_INFO_P(obj, isReusedObj)== KAL_TRUE) 
 return KAL_TRUE;
   }

   return KAL_FALSE;
#else
   return KAL_FALSE;
#endif
}



