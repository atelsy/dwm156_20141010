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
* ems_position_proc.c
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


#define EMS_POSITION_PROC_C

#include "kal_non_specific_general_types.h"
#include <stdio.h>
//#include <malloc.h> /* malloc, free */
#include <memory.h> /* memcpy, memset */
#include <assert.h> /* assert */

#define ASSERT assert

#include "ems.h"
#include "ems_defs.h"
#include "ems_enums.h"
#include "ems_context.h"
#include "ems_obj_proc.h"
#include "ems_txt_proc.h"
#include "ems_position_proc.h"
#include "ems_segment_proc.h"



/*****************************************************************************
* FUNCTION
*  BwCancelCurrentPosition
* DESCRIPTION
*   This function backwards and cancels (if required) text or object.
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
kal_uint16 BwCancelCurrentPosition(EMSData *emsData, kal_uint16 steps, 
   kal_bool needCancel, kal_bool is_roll_back)
{
#ifndef __SLIM_EMS__
   EMSObject *obj;  
#endif
   kal_bool isPrevTFexist;
   kal_uint16 maxStep, maxStepInOctet;
   kal_uint16 lastObjOffset;
   kal_uint16 remainingStep = steps;
   kal_bool isCurrTxtFmtChange; /* reset*/
   
   /* sanity check */
   if((emsData == NULL) || (steps == 0)) return 0;
	
   if(IS_EMS_INIT == KAL_FALSE || 
  IS_EMS_DATA_VALID(emsData) == KAL_FALSE) return 0;   
   
   SET_ACTIVE_EMS_ID(emsData->id);

   ASSERT(emsData->internal != NULL);

   while(remainingStep > 0)
   {
  isPrevTFexist  = KAL_FALSE; /* reset */
  isCurrTxtFmtChange = KAL_FALSE; /* reset*/
  
  if(IS_CURR_POS_AT_HEAD(emsData) == KAL_TRUE)
  {
 /* already reached the HEAD */
 break;
  }
		
#ifdef __SLIM_EMS__
  EMS_ASSERT(emsData->CurrentPosition.Object == NULL);
#else
  if(emsData->CurrentPosition.Object != NULL)			
  {
 lastObjOffset = emsData->CurrentPosition.Object->OffsetToText;	

 /* case (3),(4),(5),(11) */
 if(IS_TXT_FMT_OBJ(emsData->CurrentPosition.Object) == KAL_TRUE)
 {
isPrevTFexist = KAL_TRUE; 

/* handle a special case that current object is a text format object and
 * current position points to normal text */

/* case (3) */
if(emsData->CurrentTextFormatObj == NULL)   
   lastObjOffset = TXT_FMT_OBJ_END(emsData->CurrentPosition.Object);
 }
  }
  else
#endif
  {
 /* no obj in obj list. */
 lastObjOffset = 0;
  }
		
#ifdef EMS_UT
  if(!(emsData->CurrentPosition.OffsetToText >= lastObjOffset))
  {
 kal_print("assert!!");/* set breakpoint here!! */
  }
#endif

  /* in case that MMI doesn't call EMS library properly, 
   * we turn off this assertion in production version.
   */
  EMS_ASSERT(emsData->CurrentPosition.OffsetToText >= lastObjOffset);
  if(emsData->CurrentPosition.OffsetToText < lastObjOffset)
  {
 return (steps - remainingStep);	
  }

  maxStepInOctet = emsData->CurrentPosition.OffsetToText - lastObjOffset;
  /* it doesn't need to check DCS here. */
  /* for UCS2, 1 step (1 char in screen) cost two octets, too. */
  maxStep = maxStepInOctet/2;


  /* for "cancel" case, we need to ensure that the steps for only normal text. */
  /* <bo ld>normal^ , backward 8 steps*/
  /*^ */
  /* first BW 6 steps to end of text_fmt. */
  /* then BW 2 steps in next loop.*/
#ifndef __SLIM_EMS__
  if(needCancel == KAL_TRUE &&
 emsData->CurrentPosition.Object != NULL && 
 emsData->CurrentTextFormatObj == NULL)
  { 
 /* case (13),(7),(6),(3),(2),(1) */

 /* for case (3), CO=TF but CT=NULL */
 /* EMSObject *tf_obj = emsData->CurrentPosition.Object->prev; */
 EMSObject *tf_obj = emsData->CurrentPosition.Object;

 GET_PREV_TXT_FMT_OBJ(tf_obj);
  
 /* only for case (3),(7), 
  * others would get NULL tf_obj or lastObjOffset > tf_obj_end */
 if(tf_obj != NULL && 
(emsData->CurrentPosition.OffsetToText-maxStepInOctet <= TXT_FMT_OBJ_END(tf_obj)) &&
(emsData->CurrentPosition.OffsetToText-maxStepInOctet > EMS_OBJ_OFFSET(tf_obj)) )   
 {								
maxStepInOctet = emsData->CurrentPosition.OffsetToText - TXT_FMT_OBJ_END(tf_obj);
maxStep = maxStepInOctet/2;
 }   
  }
#endif

  if(maxStep > remainingStep)
  {  
 maxStep = remainingStep;		
 maxStepInOctet = maxStep*2;
  }
 
   /* in the case : current_obj present & not text fmt object AND  */
   /*   current_position is at offset of obj.  */
   /* [hint]: There is a non-TF object at current position. */
   /* It costs one step.*/
#ifndef __SLIM_EMS__
  if(emsData->CurrentPosition.Object != NULL &&
 IS_TXT_FMT_OBJ(emsData->CurrentPosition.Object) == KAL_FALSE &&
 emsData->CurrentPosition.OffsetToText == EMS_OBJ_OFFSET(emsData->CurrentPosition.Object))
  {
 /* object except text format object */
 EMSObject *tf_obj = emsData->CurrentPosition.Object;

 EMS_ASSERT(maxStep ==0);
 if( maxStep != 0)
 {
return (steps - remainingStep);	
 }
 
 remainingStep--; /* backward by one object */
 obj = emsData->CurrentPosition.Object;

 /* following is to delete the non-TF object, and  */
 /* update new CO and CTF object.  */

 /* 12/07/2003, Kevin
  * fix issue that text format precedes other objects (with the same offset)
  */
 if((obj->prev != NULL) && 
(IS_TXT_FMT_OBJ(obj->prev) == KAL_TRUE) && 
(EMS_OBJ_OFFSET(obj->prev) == EMS_OBJ_OFFSET(obj)))
 {
/* obj->prev is a text format but this object shall be skiped */
emsData->CurrentPosition.Object = obj->prev->prev;
tf_obj = obj->prev->prev;

/* If opt. txt fmt. is used, there should not be format : "<snd>bold" ... */
/* with object list: A (bold) -- B (sound) ==> the same offset */

/* But there are some case which can't be changed to optimized text format. */
/* This condition may be occur. */
 }
 else
emsData->CurrentPosition.Object = obj->prev;

 /* case (7),  it may need to update CTF*/
 /* when each OBJ is Bw/Cancel, except update CO, it also need to check/update CTF. */
 GET_PREV_TXT_FMT_OBJ(tf_obj); 

 if(tf_obj != NULL)
 {
/* note that it should used "<=" (include "=") */
if(emsData->CurrentPosition.OffsetToText - maxStepInOctet <= TXT_FMT_OBJ_END(tf_obj))   
{
   kal_bool need_update_fmt = KAL_TRUE;

   if((tf_obj->next != NULL) &&
  (IS_TXT_FMT_OBJ(tf_obj->next) == KAL_FALSE) && 
  (EMS_OBJ_OFFSET(tf_obj->next) == EMS_OBJ_OFFSET(tf_obj)))
   {
  need_update_fmt = KAL_FALSE;
   }

   if (need_update_fmt == KAL_TRUE)
   {
  if(emsData->CurrentTextFormatObj == NULL)
 isCurrTxtFmtChange = KAL_TRUE;

  emsData->CurrentTextFormatObj = tf_obj;
   }
}

 }
 else
 {
emsData->CurrentTextFormatObj = NULL;
 }

 if(needCancel == KAL_TRUE)
 {
/* remove object */
DeleteEMSObjectFromList(emsData, obj);
 }

  }
  else
#endif
  {
 /* to delete/backward the text content, included text with TF. */

 /* in case that MMI doesn't call EMS library properly, 
  * we turn off this assertion in production version.
  */
 EMS_ASSERT( maxStep != 0 );
 if( maxStep == 0)
 {
return (steps - remainingStep);	
 }

 if(needCancel == KAL_TRUE)
 {			
/* remove text, and update textLength */
EMSDeleteText(emsData, maxStepInOctet); 

/* Update Objects' offset after CO */
UpdateEMSObjOffset(emsData,
   KAL_TRUE, /* del */
   maxStepInOctet);

 } /* if(needCancel) */

 /* backward current position */
 remainingStep -= maxStep; 
 emsData->CurrentPosition.OffsetToText -= maxStepInOctet;


 /* case (4),(5),(8),(9),(10),(11) */
 /* to delete and update CTF information. */
#ifndef __SLIM_EMS__
 if(emsData->CurrentTextFormatObj != NULL)
 {
if(needCancel == KAL_TRUE)
{
   /* determine and update the length of formatted text  */
   kal_uint16 StepOfTxtFmt = maxStepInOctet;/*reset*/

   if(isCurrTxtFmtChange == KAL_TRUE)
   {
  if(BETWEEN_EMS_TXT_FMT(CURR_TXT_OFFSET(emsData), emsData->CurrentTextFormatObj) == KAL_TRUE)
 StepOfTxtFmt = TXT_FMT_OBJ_END(emsData->CurrentTextFormatObj)-CURR_TXT_OFFSET(emsData);
  else
 StepOfTxtFmt = 0;
   }

   TXT_FMT_LEN(emsData->CurrentTextFormatObj) -= StepOfTxtFmt;//maxStepInOctet;
}

if(emsData->CurrentTextFormatObj->OffsetToText == 
   emsData->CurrentPosition.OffsetToText)
{
   EMSObject *txt_obj  ;
   EMSObject *txt_obj_next ;   
   kal_uint16 txt_fmt_len  ;

   txt_obj  = emsData->CurrentTextFormatObj;
   txt_obj_next = emsData->CurrentTextFormatObj->next;   
   txt_fmt_len  = TXT_FMT_LEN(txt_obj);

   /* get the previous TEXT FORMAT object */				
   BwCancelCurrentTF(emsData, needCancel);			   				

   /* 
* 1^<snd> 2 3 where "123" is the formatted text,
* and user cancel "1", then we need to change the object list
* from "23"--<snd> to <snd>--"23"
*/
   if(needCancel == KAL_TRUE && txt_fmt_len > 0 ) /* if the object is not deleted */
   {
  /* 
   * 1^<snd1><snd2><snd3> 2 3 where "123" is the formatted text,
   * and user cancel "1", then we need to change the object list
   * from "23"--<snd1><snd2><snd3> to <snd1><snd2><snd3>--"23"
   */
  while(txt_obj_next != NULL &&
txt_obj_next->Type != EMS_TYPE_TEXT_FORMAT &&
txt_obj_next->OffsetToText <  txt_obj->OffsetToText+TXT_FMT_LEN(txt_obj) &&
txt_obj_next->OffsetToText == txt_obj->OffsetToText )
  {
 kal_uint8 ret;
 EMSSwapObject(emsData, txt_obj,txt_obj_next);

 /* if <snd3> has long raw data (128 bytes), then it should  */
 /* be move to next segment. */
 /* However, 128 + 3(snd IE) + 5(tfm IE) + 6(concat IE) will exceed one segment. */
 /* If here is last segment, it would return NO_SPACE. */
 /* If "cancel" action from MMI, it does not make sense. */
 /* So....keep non-optimizated text format.*/
 if (is_roll_back == KAL_FALSE)
 {
EMSSetUDHI(emsData);
ret=EMSCalculateLength(emsData);
if(ret!=EMS_OK)
{
   /* rollback and break; */
   EMSSwapObject(emsData, txt_obj_next ,txt_obj);
   break;
}
 }

 txt_obj_next = txt_obj->next;
  }
   }

}
 } /*if(emsData->CurrentTextFormatObj != NULL)*/

 /*
  * eg, ...bold normal ...
  *   ^
  * current_obj --> bold
  * current_txt_fmt --> null
  * 
  * in this case, isPrevTFexist==1 (CO == text fmt)
  * [hint] : CO = TF(bold), CTF = NULL
  *  only for of case (3)
  */
 else 
 {
kal_bool update_curr_obj = KAL_FALSE;

if(isPrevTFexist == KAL_TRUE) 
{
   /* update current object */
   while(CURR_OBJ_OFFSET(emsData) >= CURR_TXT_OFFSET(emsData)) 
  emsData->CurrentPosition.Object = emsData->CurrentPosition.Object->prev;

   /* update current text format object */
   if(IS_CURRENT_OBJ_PRESENT(emsData) == KAL_TRUE && 
  IS_TXT_FMT_OBJ(emsData->CurrentPosition.Object) == KAL_TRUE &&
  (
   BETWEEN_EMS_TXT_FMT(CURR_TXT_OFFSET(emsData), emsData->CurrentPosition.Object) == KAL_TRUE ||
   IS_FOLLOW_EMS_TXT_FMT(CURR_TXT_OFFSET(emsData), emsData->CurrentPosition.Object) == KAL_TRUE
  )
 )
   {
  emsData->CurrentTextFormatObj = emsData->CurrentPosition.Object;
  update_curr_obj = KAL_TRUE;
   }
}
else
{
   /* case (1),(6),(9),(12),(13) */
   /* and "cancel" of case (3),(7)(cancel normal text first) */
   /* and "bw" of case (7),(bw to prev obj) */

   /* 
* 1) bold <sound> 1 2 3 ^ , backward 3 steps
*^
* current_obj  --> sound
* current_text_fmt --> bold
*
* 2) bold <sound> 1 2 3 ^, backward 1 step
*^
* current_obj  --> sound
* current_text_fmt --> NULL
*/
   if(emsData->CurrentPosition.Object != NULL && emsData->CurrentTextFormatObj == NULL)
  SetCurrTxtFmtObj(emsData);
}


/* check does CurrentTextFormatObj need to merge with prev or next txt_fmt. */
if(needCancel == KAL_TRUE && 
   emsData->CurrentTextFormatObj != NULL)
{

   EMSObject *prev_obj = emsData->CurrentTextFormatObj->prev;
   EMSObject *next_obj = emsData->CurrentTextFormatObj->next;
   EMSObject *prev_curr_obj = emsData->CurrentPosition.Object->prev;

   GET_NEXT_TXT_FMT_OBJ(next_obj);
   GET_PREV_TXT_FMT_OBJ(prev_obj);

   MergeTF(emsData, emsData->CurrentTextFormatObj, next_obj);  
   if(KAL_TRUE == MergeTF(emsData, prev_obj, emsData->CurrentTextFormatObj))
   {
  /* merge previous one, it needs to update current information. */
  emsData->CurrentTextFormatObj = prev_obj; 
  if(update_curr_obj == KAL_TRUE ) 
 emsData->CurrentPosition.Object = prev_curr_obj ;

  SetCurrTxtFmtObj(emsData);
   }

}

 } /*if(emsData->CurrentTextFormatObj != NULL)*/
#endif
  }

   } /* while */

   if(emsData->CurrentTextFormatObj != NULL && IS_CURRENT_OBJ_PRESENT(emsData) == KAL_TRUE)
   {
  if((IS_TXT_FMT_OBJ(emsData->CurrentPosition.Object) == KAL_FALSE) &&
 (emsData->CurrentPosition.OffsetToText == EMS_OBJ_OFFSET(emsData->CurrentTextFormatObj)) &&
 (emsData->CurrentPosition.OffsetToText == EMS_OBJ_OFFSET(emsData->CurrentPosition.Object)))
  {
 emsData->CurrentTextFormatObj = NULL;
  }
   }
   
   return (steps - remainingStep); /* return the actual steps moved */	
	
} /* end of BwCancelCurrentPosition */




