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
 * ems_txt_fmt_proc.c
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


#define EMS_TXT_FMT_PROC_C

#include "kal_non_specific_general_types.h"
#include <stdio.h>
#include <memory.h> /* memcpy, memset */
#include <assert.h> /* assert */

#define ASSERT assert

#include "smsal_l4c_enum.h"
#include "ems.h"
#include "ems_defs.h"
#include "ems_enums.h"
#include "ems_context.h"
#include "ems_utils.h"
#include "ems_obj_proc.h"
#include "ems_txt_proc.h"
#include "ems_segment_proc.h"


/*****************************************************************************
* FUNCTION
*  EMSDeleteText
* DESCRIPTION
*   This function deletes text from current position.
*
* PARAMETERS
*  a  IN/OUT  emsData
*  b  IN  bytes, number of bytes which wanted to be deleted
* RETURNS
*  none
* GLOBALS AFFECTED
*  none
*****************************************************************************/
void EMSDeleteText(EMSData *emsData, kal_uint16 bytes)
{   
   kal_uint16 offset = emsData->CurrentPosition.OffsetToText;

   if(offset != emsData->textLength)
   {
  /* current position is at the middle of text buffer */		
  ems_mem_cpy((emsData->textBuffer+offset-bytes),
  emsData->textBuffer+offset,
  emsData->textLength-offset);
   }	
   emsData->textLength -= bytes;
} /* end of EMSDeleteText */

void SetCurrTxtFmtObj(EMSData *emsData)
{
   EMSObject *obj = emsData->CurrentPosition.Object;

   GET_PREV_TXT_FMT_OBJ(obj);   
   
   if(obj != NULL && 
  ( BETWEEN_EMS_TXT_FMT(CURR_TXT_OFFSET(emsData), obj) == KAL_TRUE ||
IS_FOLLOW_EMS_TXT_FMT(CURR_TXT_OFFSET(emsData), obj) == KAL_TRUE
  ))
   {
  emsData->CurrentTextFormatObj = obj;
   }
   
} /* end of SetCurrTxtFmtObj */   

#ifndef __SLIM_EMS__
/*****************************************************************************
* FUNCTION
*  MergeTF
* DESCRIPTION
*   This function merges two Text Format Objects whose text attributes 
*   (eg, alignment, font size, etc) are the same.
*
* PARAMETERS
*  a  IN/OUT  emsData
*  b  IN/OUT  prev, the preceded text format object
*  c  IN/OUT  next, the succeeded text format object, will be deleted
* RETURNS
*  kal_bool   KAL_TRUE  : merge two object
* KAL_FALSE : not merge two object
* GLOBALS AFFECTED
*  none
*****************************************************************************/
kal_bool MergeTF(EMSData *emsData, EMSObject *prev, EMSObject *next)
{
   kal_bool ret=KAL_FALSE;
   /* sanity check */
   if((prev == NULL) || (next == NULL))
  return ret;

   if((IS_TXT_FMT_OBJ(prev) == KAL_FALSE)||
  (IS_TXT_FMT_OBJ(next) == KAL_FALSE))
  return ret;
#if 0
#ifndef OPTIMIZE_TXT_FMT
   /* 
* if there is any object in between those two text format objects
* we don't merge them
*/
   if((kal_uint32*)prev->next != (kal_uint32*)next)
  return ret;
#endif
#endif

   if(TextFormatCmp(&EMS_P(prev->data,text_format), 
&EMS_P(next->data,text_format)) == KAL_FALSE)
   {
  /* two text format attributes are the same, 
   * check whether two texts are linked together
   */
  if((prev->OffsetToText+EMS_P(prev->data, text_format.textLength)) == next->OffsetToText)
  {
 EMS_P(prev->data,text_format.textLength) += EMS_P(next->data,text_format.textLength);
 DeleteEMSObjectFromList(emsData, next);
 ret=KAL_TRUE;
  }
   }
   return ret;

} /* end of MergeTF */

/*****************************************************************************
* FUNCTION
*  BwCancelCurrentTF
* DESCRIPTION
*   This function backwards (or delete) the current Text Format Object.
*
* PARAMETERS
*  a  IN/OUT  emsData
*  b  IN  needCancel
*  c  IN  steps
* RETURNS
*  none
* GLOBALS AFFECTED
*  none
*****************************************************************************/
void BwCancelCurrentTF(EMSData *emsData, kal_bool needCancel)
{   
   EMSObject *prev, *next;   
   
   ASSERT(emsData->CurrentTextFormatObj != NULL);
	
   prev = emsData->CurrentTextFormatObj->prev;   

   /* 12/07/2003, Kevin
* fix issue that text format precedes other objects (with the same offset)
* without this fix: some objects will be skipped !!
*/
   if(((kal_uint32*)emsData->CurrentPosition.Object) == 
  ((kal_uint32*)emsData->CurrentTextFormatObj))
  emsData->CurrentPosition.Object = prev;

   GET_PREV_TXT_FMT_OBJ(prev);   
   
   next = emsData->CurrentTextFormatObj->next;
   GET_NEXT_TXT_FMT_OBJ(next);   
   
   if(needCancel == KAL_TRUE && 
  EMS_P(emsData->CurrentTextFormatObj->data,text_format.textLength) == 0)
   {  
  DeleteEMSObjectFromList(emsData, emsData->CurrentTextFormatObj);
  MergeTF(emsData, prev, next);  
   }

   emsData->CurrentTextFormatObj = prev;

   if(emsData->CurrentTextFormatObj != NULL)
   {
  if(CURR_TXT_OFFSET(emsData) > TXT_FMT_OBJ_END(emsData->CurrentTextFormatObj))  
  {
 emsData->CurrentTextFormatObj = NULL;
  }
   }

} /* end of BwCancelCurrentTF */

/*****************************************************************************
* FUNCTION
*  TextAlignmentCmp
* DESCRIPTION
*   Check if only alignment of text format is different between two text
*   format. 
*
* PARAMETERS
*  a  IN  tf1
*  b  IN  tf2
* RETURNS
*  KAL_TRUE: only type of alignment is different
*(i.e. except alignment, others are the same)
* GLOBALS AFFECTED
*  none
*****************************************************************************/
kal_bool TextAlignmentCmp(EMSTextFormat *tf1, EMSTextFormat *tf2)
{
   if(tf1->FontSize != tf2->FontSize)
  return KAL_FALSE;

   if(tf1->isBold != tf2->isBold)
  return KAL_FALSE;
   
   if(tf1->isItalic != tf2->isItalic)
  return KAL_FALSE;

   if(tf1->isUnderline != tf2->isUnderline)
  return KAL_FALSE;

   if(tf1->isStrikethrough != tf2->isStrikethrough)
  return KAL_FALSE;

   if(tf1->bgColor != tf2->bgColor)
  return KAL_FALSE;

   if(tf1->fgColor != tf2->fgColor)
  return KAL_FALSE;
	
   if(tf1->Alignment != tf2->Alignment)
   	return KAL_TRUE;
   
   return KAL_FALSE;
   
} /* end of TextAlignmentCmp */


/*****************************************************************************
* FUNCTION
*  TextFormatCmp
* DESCRIPTION
*   This function compares two Text Format Objects to check whether they 
*   are the same or not.
*
* PARAMETERS
*  a  IN  tf1
*  b  IN  tf2
* RETURNS
*  KAL_FALSE: the same
* GLOBALS AFFECTED
*  none
*****************************************************************************/
kal_bool TextFormatCmp(EMSTextFormat *tf1, EMSTextFormat *tf2)
{
   if(tf1->FontSize != tf2->FontSize)
  return KAL_TRUE;

   if(tf1->Alignment != tf2->Alignment)
   	return KAL_TRUE;
   
   if(tf1->isBold != tf2->isBold)
   	return KAL_TRUE;
   
   if(tf1->isItalic != tf2->isItalic)
   	return KAL_TRUE;

   if(tf1->isUnderline != tf2->isUnderline)
   	return KAL_TRUE;

   if(tf1->isStrikethrough != tf2->isStrikethrough)
   	return KAL_TRUE;

   if(tf1->bgColor != tf2->bgColor)
  return KAL_TRUE;

   if(tf1->fgColor != tf2->fgColor)
  return KAL_TRUE;
	
   return KAL_FALSE;
   
} /* end of TextFormatCmp */

EMSObject *AddTextFmtObjByOffset(EMSData *emsData, kal_uint16 offsetToText, EMSTextFormat *txt_fmt)
{
   EMSObject *newobj;

   newobj = EMSObjectAllocate(KAL_TRUE);
   if(newobj == NULL)
  return NULL;
   
   newobj->next=NULL;
   newobj->prev=NULL;
   newobj->Type = EMS_TYPE_TEXT_FORMAT;
   newobj->OffsetToText = offsetToText;
   
   ASSERT(EMS_R(newobj->data) != NULL);
   ems_mem_cpy(&EMS_P(newobj->data,text_format),
   txt_fmt,
   sizeof(EMSTextFormat));
   		
   AddObjectIntoListByOffset(emsData, newobj);

   if (offsetToText < emsData->CurrentPosition.OffsetToText)
   {
  if (emsData->CurrentPosition.Object != NULL)
  {
 if( offsetToText >= emsData->CurrentPosition.Object->OffsetToText) 
emsData->CurrentPosition.Object = newobj ;
  }
  else 
 emsData->CurrentPosition.Object = newobj ;
   }

   return newobj;
}


/*****************************************************************************
* FUNCTION
*  AddTextFormatObject
* DESCRIPTION
*   This function adds a Text Format Object into object list.
*
* PARAMETERS
*  a  IN/OUT  emsData
*  b  IN  txt_fmt, input text format
* RETURNS
*  EMSObject, pointer of the new created object
* GLOBALS AFFECTED
*  none
*****************************************************************************/
EMSObject *AddTextFormatObject(EMSData *emsData, EMSTextFormat *txt_fmt)
{
   EMSObject *newobj;

#if 0
   if(((EMSInternalData*)emsData->internal)->numOfEMSObject >= EMS_MAX_OBJECT)   
  return NULL;   
#endif

   newobj = EMSObjectAllocate(KAL_TRUE);
   if(newobj == NULL)
  return NULL;
   
   newobj->next=NULL;
   newobj->prev=NULL;
   newobj->Type = EMS_TYPE_TEXT_FORMAT;
   
   ASSERT(EMS_R(newobj->data) != NULL);
   ems_mem_cpy(&EMS_P(newobj->data,text_format),
   txt_fmt,
   sizeof(EMSTextFormat));
   		
   AddObjectIntoList(emsData, newobj);

   emsData->CurrentPosition.Object = newobj;
   emsData->CurrentTextFormatObj = newobj;

   return newobj;
} /* end of AddTextFormatObject */

/*****************************************************************************
* FUNCTION
*  needSplitTextFormat
* DESCRIPTION
*   This function checks whether a Text Format Object is needed to split
*   as another Text Format Object inserting.
*
* PARAMETERS
*  a  IN/OUT  emsData
*  b  IN  second variable
* RETURNS
*  KAL_TRUE: need split
* GLOBALS AFFECTED
*  none
*****************************************************************************/
kal_bool needSplitTextFormat(EMSData *emsData)
{

   /* NOTE that: "TF(bold rest of bold)" ... */
   /*^ -- current position   */
   /* if current position is in a TF, and attempt to insert  */
   /* a different TF, old TF needs to be spilt.  */
   
   kal_uint16 begin, end;
   EMSObject *obj;

   if(emsData->CurrentTextFormatObj == NULL)
  return KAL_FALSE;
   
   obj = emsData->CurrentTextFormatObj;
   begin = obj->OffsetToText;
   end   = obj->OffsetToText + EMS_P(obj->data,text_format.textLength);

   if((emsData->CurrentPosition.OffsetToText > begin) &&
  (emsData->CurrentPosition.OffsetToText < end ))
  return KAL_TRUE;
   
   return KAL_FALSE;

} /* end of needSplitTextFormat */

/*****************************************************************************
* FUNCTION
*  isTFValid
* DESCRIPTION
*   This function checks whether a Text Format Object is valid
*
* PARAMETERS
*  a  IN  txt_fmt
* RETURNS
*  KAL_TRUE: valid
* GLOBALS AFFECTED
*  none
*****************************************************************************/
kal_bool isTFValid(EMSTextFormat *txt_fmt)
{
   if(txt_fmt->Alignment > EMS_ALIGN_LANG_DEP)
  return KAL_FALSE;
   
   if((txt_fmt->FontSize != EMS_FONTSIZE_NORMAL) &&
 (txt_fmt->FontSize != EMS_FONTSIZE_LARGE) &&
 (txt_fmt->FontSize != EMS_FONTSIZE_SMALL))
   {
  return KAL_FALSE;
   }
   
   if((txt_fmt->isBold != KAL_TRUE && txt_fmt->isBold != KAL_FALSE) ||
 (txt_fmt->isItalic != KAL_TRUE && txt_fmt->isItalic != KAL_FALSE) ||   	
 (txt_fmt->isUnderline != KAL_TRUE && txt_fmt->isUnderline != KAL_FALSE) ||
 (txt_fmt->isStrikethrough != KAL_TRUE && txt_fmt->isStrikethrough != KAL_FALSE))
   {
  return KAL_FALSE;
   }
   
   return KAL_TRUE;
} /* end of isTFValid */

/*****************************************************************************
* FUNCTION
*  isDefaultTF
* DESCRIPTION
*   This function checks whether is a default Text Format, ie:
*   language-dependent alignment, normal font size, 
*   no bold, no italic, no underline no strikethrough.
*
* PARAMETERS
*  a  IN  txt_fmt
* RETURNS
*  KAL_TRUE: is default
* GLOBALS AFFECTED
*  none
*****************************************************************************/
kal_bool isDefaultTF(EMSTextFormat *txt_fmt)
{
   if(txt_fmt->Alignment != EMS_ALIGN_LANG_DEP)
  return KAL_FALSE;
   
   if(txt_fmt->FontSize != EMS_FONTSIZE_NORMAL)
   {
  return KAL_FALSE;
   }
   
   if((txt_fmt->isBold == KAL_TRUE) ||
 (txt_fmt->isItalic == KAL_TRUE) ||   	
 (txt_fmt->isUnderline == KAL_TRUE) ||
 (txt_fmt->isStrikethrough == KAL_TRUE))
   {
  return KAL_FALSE;
   }

   if (txt_fmt->fgColor != EMS_BLACK || txt_fmt->bgColor != EMS_WHITE )
   {
  return KAL_FALSE;
   }
   
   return KAL_TRUE;
} /* end of isDefaultTF */

/*****************************************************************************
* FUNCTION
*  AddTextFormat
* DESCRIPTION
*   This function adds a text format.
*
* PARAMETERS
*  a  IN  emsData
*  b  IN  txt_fmt, input text format
*  b  IN/OUT  length (text length)
* RETURNS
*  EMSTATUS
* GLOBALS AFFECTED
*  none
*****************************************************************************/
EMSTATUS AddTextFormat(EMSData *emsData, EMSTextFormat *txt_fmt, kal_uint16 length)
{ 
   EMSObject *oldobj1=NULL, *oldobj2=NULL, *newobj=NULL;
   EMSObject *ori_curr_obj, *prev_fmt;
   kal_bool isDefault;
   kal_bool normalAdd=KAL_TRUE; /* reset */
   kal_bool needSplit=KAL_FALSE;/*reset*/
   
   /* check whether this text format is valid or not */
   if(isTFValid(txt_fmt) == KAL_FALSE)
  return EMS_INVALID_OBJECT;


   isDefault = isDefaultTF(txt_fmt);
  	
   /* deal with the special case when non-optmized text fmt occurs. */
   /* format : "normal<snd1>^<snd2>^<snd3>^bold" ... */
   /* object list: A(snd1) -- B(bold) -- C(snd2> -- D(snd3) */
   /* The curr_fmt of highlight position shall be NULL. */
   /* But it may be insert another txt fmt, the obj B shall be swapped. */
   /* Then the new txt fmt can be insert.   */
   /* If it can't be swap, return false */

   if(emsData->CurrentTextFormatObj == NULL && 
  emsData->CurrentPosition.Object != NULL)
   {
  prev_fmt = emsData->CurrentPosition.Object ;
  GET_PREV_TXT_FMT_OBJ(prev_fmt);   

  if (prev_fmt!= NULL &&
  prev_fmt->OffsetToText == emsData->CurrentPosition.Object->OffsetToText )
  {
 EMSObject *txt_obj_next  = prev_fmt->next ;
 while(txt_obj_next != NULL &&
   txt_obj_next->Type != EMS_TYPE_TEXT_FORMAT &&
   txt_obj_next->OffsetToText <  prev_fmt->OffsetToText+TXT_FMT_LEN(prev_fmt) &&
   txt_obj_next->OffsetToText == prev_fmt->OffsetToText )
 {
kal_uint8 ret;
EMSSwapObject(emsData, prev_fmt,txt_obj_next);

/* if <snd3> has long raw data (128 bytes), then it should  */
/* be move to next segment. */
/* However, 128 + 3(snd IE) + 5(tfm IE) + 6(concat IE) will exceed one segment. */
/* If here is last segment, it would return NO_SPACE. */
/* If "cancel" action from MMI, it does not make sense. */
/* So....keep non-optimizated text format.*/
EMSSetUDHI(emsData);
ret=EMSCalculateLength(emsData);
if(ret!=EMS_OK)
{
   /* rollback and break; */
   EMSSwapObject(emsData, txt_obj_next ,prev_fmt);
   return EMS_NO_SPACE;
}

if (emsData->CurrentPosition.Object == txt_obj_next &&
emsData->CurrentPosition.Object->OffsetToText < emsData->CurrentPosition.OffsetToText)
   emsData->CurrentPosition.Object = prev_fmt;

txt_obj_next = prev_fmt->next;
 }
  }
   }

   ori_curr_obj = emsData->CurrentPosition.Object ;

   /* Is the position (to be inserted) associated  with Text Format ? */
   if(emsData->CurrentTextFormatObj != NULL)
   {
  if( (TextFormatCmp(&EMS_P(emsData->CurrentTextFormatObj->data,text_format), txt_fmt) == KAL_FALSE)  )
  {
 /* the same text format */
#if 0
#ifndef OPTIMIZE_TXT_FMT
 if((kal_uint32*)emsData->CurrentTextFormatObj == 
(kal_uint32*)emsData->CurrentPosition.Object)
#endif
#endif
 {
EMS_P(emsData->CurrentTextFormatObj->data,text_format.textLength) += length;
return EMS_OK;
 }
 /* else case: */
 /* If there is TF at current position and it is "not equal" to object at currrnt position, */
 /* it need add a new object (with old TF) to append new text. */
  }
  else if((needSplit=needSplitTextFormat(emsData)) == KAL_TRUE)
  {
 EMSObject *NextObj, *tmp_obj;
 kal_bool needMoveToNextObj = KAL_FALSE; /* reset */

 /* different text format, split current format text object
  * into two objects :
  *
  * C is the new text format
  * A (CurrentTxtFormat) -- B ==> A1 -- C -- A2 -- B
  */
 normalAdd = KAL_FALSE; /* need split */
   		
 /* old obj (A1) */
 oldobj1 = emsData->CurrentTextFormatObj;

 /* Robert note:   */
 /*the following condition would not occur TF is not optmized. */
 /*   format : "bold^<snd> rest of bold" ...   */
 /*   object list: A (bold rest of bold) -- B (sound) --==>*/

 /*
  * NOTE that: "bo^ld^<snd1>^re^st^<snd2>^<snd3>^of^ bold" ...
  * 
  * A (bold rest of bold) -- B (snd1) -- C (snd2) -- D (snd3)
  * four cases from left to right(position "^"), 
  *
  * (1)(2).   CO=A, CT=A 
  * (3)(4)(5).CO=B, CT=A 
  * (6).  CO=C, CT=A 
  * (7)(8).   CO=D, CT=A 
  * After insert a new TF, object list become
  * (1). A1(bo) -- E(new text) -- A2 (ld rest of bold) -- B(snd1) -- C (snd2) -- D (snd3)
  * (2). A1(bold) -- E(new text) -- B(snd1) -- A2 (rest of bold) -- C (snd2) -- D (snd3)
  * (3). A1(bold) -- B(snd1) -- E(new text) -- A2 (rest of bold) -- C (snd2) -- D (snd3)
  * (4). A1(bold re) -- B(snd1) -- E(new text) -- A2 (st of bold) -- C (snd2) -- D (snd3)
  * (5). A1(bold rest) -- B(snd1) -- E(new text) -- C (snd2) -- D (snd3) -- A2 (of bold) 
  * (6). A1(bold rest) -- B(snd1) -- C (snd2) -- E(new text) -- D (snd3) -- A2 (of bold) 
  * (7). A1(bold rest) -- B(snd1) -- C (snd2) -- D (snd3) -- E(new text) -- A2 (of bold) 
  * (8). A1(bold rest of) -- B(snd1) -- C (snd2) -- D (snd3) -- E(new text) -- A2 (bold) 
  */

 tmp_obj = emsData->CurrentPosition.Object->next ;
 while(tmp_obj != NULL &&
   tmp_obj->Type != EMS_TYPE_TEXT_FORMAT &&
   tmp_obj->OffsetToText < oldobj1->OffsetToText+TXT_FMT_LEN(oldobj1))
 {
/* only case (2),(5),(6) need special action */
/* keep B(snd1) or D(snd3), after insert E, move CO to B/D, then insert A2. */
if(emsData->CurrentPosition.OffsetToText == tmp_obj->OffsetToText )
{
   needMoveToNextObj =KAL_TRUE;
   NextObj= tmp_obj;
}
else if(emsData->CurrentPosition.OffsetToText < tmp_obj->OffsetToText )
   break;

tmp_obj = tmp_obj->next;
/* other 3 cases follow normal precedure. */
/* insert C first then A2 */
 }

 if(isDefault==KAL_FALSE)	
 {
#if 0
/* temp solution for text alignment issue */
if(txt_fmt->Alignment != EMS_ALIGN_LANG_DEP)
{
   if(emsData->Reminder.totalRemainingOctet < (EMS_MAX_SEG_NUM * 6 + 2 ))
  return EMS_NO_SPACE;

   if (isObjExist == KAL_TRUE)
  return EMS_NO_ALIGNMENT_FMT;
}
#endif

/* Adding a new txt fmt in existent alignment txt fmt. */
/* The new txt fmt shall be added with alignment type  */
/* the same with existent txt fmt  */
if ( oldobj1->data->text_format.Alignment != EMS_ALIGN_LANG_DEP)
   txt_fmt->Alignment = oldobj1->data->text_format.Alignment ;

newobj = AddTextFormatObject(emsData, txt_fmt);/* new text format(C)*/

if(newobj == NULL)
   goto no_memory;   
 }

 /* change CO to B(snd)/D(snd) to insert A2(rest of bold) for case 2,5,6 */
 if(needMoveToNextObj ==KAL_TRUE)
 {
/* deal with the case 2 with more than one <snd> object */
/*   format : "bold^<snd><snd>....<snd>rest of bold" ... */
/*   object list: A (bold rest of bold) -- B1(snd) --B2(snd) -- */
/* while(NextObj->next != NULL &&*/
/*   NextObj->next->Type != EMS_TYPE_TEXT_FORMAT )   */
/* { */
/*if( NextObj->next->OffsetToText == NextObj->OffsetToText ) */
/*   NextObj = NextObj->next ;   */
/*else   */
/*   break;  */
/* } */
emsData->CurrentPosition.Object = NextObj;
 }

 /* old obj (A2) */
 oldobj2 = AddTextFormatObject(emsData, 
   &EMS_P(oldobj1->data,text_format));

 if(oldobj2 == NULL)
goto no_memory;

 /* update the text offset of the old obj (A2) */   		   		
 oldobj2->OffsetToText = emsData->CurrentPosition.OffsetToText;
 
 /* update the text length of the old obj (A1) */
 EMS_P(oldobj1->data,text_format.textLength) =	  
emsData->CurrentPosition.OffsetToText - oldobj1->OffsetToText;

 /* update the text length of the old obj (A2) */
 EMS_P(oldobj2->data,text_format.textLength) -= EMS_P(oldobj1->data,text_format.textLength);   			

  }  /* if */
  		
   } /* if(emsData->CurrentTextFormatObj != NULL) */
   

   /* format : "normal^[bold]" ... */
   /* object list: A (bold)  and CO=CT=NULL */

   /* format : "[italic]^[bold]" ... */
   /* object list: B (italic) -- A (bold)  and CO=CT=B */

   /* Add new string <new format> with bold.   */
   /* Because it is the same with next text format, it should be added to old tfm. */
   if((emsData->CurrentTextFormatObj == NULL) ||
  (((emsData->CurrentTextFormatObj != NULL) &&
   (TextFormatCmp(&EMS_P(emsData->CurrentTextFormatObj->data,text_format), txt_fmt) == KAL_TRUE)) ||
   ((emsData->CurrentPosition.OffsetToText == 
emsData->CurrentTextFormatObj->OffsetToText+TXT_FMT_LEN(emsData->CurrentTextFormatObj)) &&
   ( needSplit == KAL_FALSE ))))
   {
  EMSObject *next_tf_obj;

  if( emsData->CurrentPosition.Object !=NULL) 
  {
 next_tf_obj = emsData->CurrentPosition.Object ;
 if(emsData->CurrentTextFormatObj == next_tf_obj) 
next_tf_obj = emsData->CurrentPosition.Object->next ;
  }
  else 
 next_tf_obj = emsData->listHead;

  GET_NEXT_TXT_FMT_OBJ(next_tf_obj);   
  if (next_tf_obj != NULL &&
  next_tf_obj->Type == EMS_TYPE_TEXT_FORMAT &&
  (TextFormatCmp(&(next_tf_obj->data->text_format), txt_fmt) == KAL_FALSE) && /* the same text format */
  next_tf_obj->OffsetToText == emsData->CurrentPosition.OffsetToText )
  {
 EMSObject *prev_of_fmt_obj;

 /* format : "normal^<snd1><snd2>[bold]" ... */
 /* object list: X(snd1) -- Y(snd2) -- A (bold) , all have the same offset. 
  * and CO=CT=NULL */
 /* change to new object list : A (new text bold)-- X(snd1) -- Y(snd2)   */
 prev_of_fmt_obj = next_tf_obj->prev;
 while (prev_of_fmt_obj  != NULL &&
prev_of_fmt_obj->Type != EMS_TYPE_TEXT_FORMAT &&
prev_of_fmt_obj != emsData->CurrentPosition.Object &&
prev_of_fmt_obj->OffsetToText == emsData->CurrentPosition.OffsetToText )
 {
EMSSwapObject(emsData, prev_of_fmt_obj, next_tf_obj);
prev_of_fmt_obj = next_tf_obj->prev;
 }

 emsData->CurrentTextFormatObj = next_tf_obj;
 emsData->CurrentPosition.Object  = emsData->CurrentTextFormatObj ;

 EMS_P(emsData->CurrentTextFormatObj->data,text_format.textLength) += length;
 return EMS_OK;
  }
   }

   if(isDefault == KAL_FALSE)
   {
  if(normalAdd == KAL_TRUE)
  {
 /* temp solution for text alignment issue */
#if 0
 if(txt_fmt->Alignment != EMS_ALIGN_LANG_DEP)
 {
if(emsData->Reminder.totalRemainingOctet < (EMS_MAX_SEG_NUM * 6 + 2 ))
   return EMS_NO_SPACE;

if (isObjExist == KAL_TRUE)
   return EMS_NO_ALIGNMENT_FMT;
 }
#endif
 newobj = AddTextFormatObject(emsData, txt_fmt);	
  }

  if(newobj==NULL)
 goto no_memory;   

  newobj->OffsetToText = emsData->CurrentPosition.OffsetToText;
  EMS_P(newobj->data,text_format.textLength) = length;

  /* set current text format object to this new text format object */
  emsData->CurrentPosition.Object = newobj;
  emsData->CurrentTextFormatObj = newobj;
   }
   else
   {
  emsData->CurrentTextFormatObj = NULL;

  /* because AddTextFormatObject() will set current object to (A2) AND 
   * the insert text is normal text, thus current object shall be previous one */
  if( needSplit == KAL_TRUE )
  {
 /* restore CurrentPosition.Object. */
 ASSERT( emsData->CurrentPosition.Object != NULL );

 /* for case : "bold^<snd><snd>....<snd>rest of bold" ... */
 /* the previous one may not the right one (the orginal CO) */
 /* emsData->CurrentPosition.Object = emsData->CurrentPosition.Object->prev; */
 emsData->CurrentPosition.Object = ori_curr_obj ;
  }
   }
   
   return EMS_OK;	

no_memory:

   /* call DeleteEMSObjectFromList rather ReleaseEMSObject
* because this object already added into linked list 
*/
   if(newobj) DeleteEMSObjectFromList(emsData, newobj);
   if(oldobj2) DeleteEMSObjectFromList(emsData, oldobj2);

   return EMS_NO_MEMORY;

} /* end of AddTextFormat */

/*****************************************************************************
* FUNCTION
*  GetTxtFmtRemainingSpace
* DESCRIPTION
*   This function is to packing a text format.
*
* PARAMETERS
*  a  IN  emsData
*  b  IN  i   
*  c  IN  ori_udh_len  , exist UDH length in current segment. 
*  d  IN  udh_len  , UDH IE length of this text fmt.
*  e  IN  txt_len  , text length before this text fmt.
*  f  IN  txt_fmt  , input text format (with length info.)
*  g  OUT udhLength, total UDH length 
*  h  OUT numOfChar, char num. can be put in this segment
*  i  OUT numOfOctet   , octet num. can be put in this segment
* RETURNS
*  EMSTATUS
* GLOBALS AFFECTED
*  none
*****************************************************************************/
kal_bool GetTxtFmtRemainingSpace(EMSData *emsData, 
 EMSInternalData *i, 
 kal_uint16  ori_udh_len,
 kal_uint16 udh_len,
 kal_uint16 txt_len,
 EMSTextFormat *txt_fmt,
 kal_uint16  *udLength,
 kal_uint16  *numOfChar, 
 kal_uint16  *numOfOctet)
{
   kal_uint16  length;   

   *udLength = GetNumOfChar(emsData->dcs, (kal_uint16)(ori_udh_len+udh_len)); 

   /* not enough, shall not happen, because it already checked in flush function */
   if( i->MaxLenPerSeg  <= *udLength ||
   i->MaxLenPerSeg  <= (*udLength + txt_len) ) 
  return KAL_FALSE; 

   /* remaining length that the text length in this txt_fmt can put */
   length = i->MaxLenPerSeg - *udLength - txt_len; 

   /* if UCS2 is adopted, 
* we shall ensure a UCS2 (2-byte) character can not carray together */   
   if((emsData->dcs == SMSAL_UCS2_DCS) && ((length % 2) != 0))
  length--;

   *numOfChar = GetTxtLenInChar(emsData->dcs, txt_fmt->textLength);

   if(length >= *numOfChar)
   {
  length = *numOfChar;   
   }

   *numOfOctet = GetTxtLenInOctet(emsData->dcs, length); /* actual octets no. */

   return KAL_TRUE;
}

/*****************************************************************************
* FUNCTION
*  PackTextFormat
* DESCRIPTION
*   This function packs a character.
*
* PARAMETERS
*  a  IN/OUT  emsData
*  b  IN  object
*  d  IN  needPack
* RETURNS
*  TRUE: this object was processed
* GLOBALS AFFECTED
*  none
*****************************************************************************/
kal_bool PackTextFormat(EMSData*emsData,
EMSObject  *object,
kal_bool   needPack)
{  
   kal_uint8   fmt_mode, fmt_color, fmt_color_len =0;
   kal_uint8   *data, *offset;
   kal_uint16   udLength;
   kal_uint16  numOfChar, numOfOctet;   
   kal_bool		ret;
   EMSObject   *next_obj;

   /* with initial value */
   EMSInternalData  *i = (EMSInternalData*)emsData->internal;
   EMSTextFormat*txt_fmt = &EMS_P(object->data,text_format);
   kal_uint16   objLen = 0; 
   kal_bool needNotFinObj = KAL_FALSE; 
   kal_uint16   txt_char_len = GetTxtLenInChar(emsData->dcs, 
  (object->OffsetToText - i->baseTextOffset));
   kal_uint16   text_OffsetToText ;
#ifdef __EMS_REL5__
   EMSObject   *obj_dist_ind = NULL;
   kal_uint8objDistCnt;

   /* keep the value to restore. */
   objDistCnt = i->ObjDistIndCount ;
#endif
 
   if(txt_fmt->textLength == 0)
  return KAL_TRUE; 

   text_OffsetToText  = object->OffsetToText;

   /* get UDH length. */
   offset = &i->TPUDLen[i->numSegment];

   if (txt_fmt->fgColor != EMS_BLACK || txt_fmt->bgColor != EMS_WHITE )
  fmt_color_len = 1;
   
   if((needPack == KAL_TRUE) && (i->TPUD[i->numSegment] != NULL))
   /* if(needPack == KAL_TRUE) */
   {
  data = i->TPUD[i->numSegment];
		
  ASSERT(data != NULL);

  fmt_mode = 0; /* reset */
  fmt_mode |= txt_fmt->Alignment; 
  fmt_mode |= txt_fmt->FontSize;
   
  if(txt_fmt->isBold == KAL_TRUE)
 fmt_mode |=  EMS_STYLE_BOLD;
  
  if(txt_fmt->isItalic == KAL_TRUE)
 fmt_mode |=  EMS_STYLE_ITALIC;
  
  if(txt_fmt->isUnderline == KAL_TRUE)
 fmt_mode |=  EMS_STYLE_UNDERLINE;
  
  if(txt_fmt->isStrikethrough == KAL_TRUE)
 fmt_mode |=  EMS_STYLE_STRIKE;  

  if (txt_fmt->fgColor != EMS_BLACK || txt_fmt->bgColor != EMS_WHITE )
 fmt_color = (txt_fmt->fgColor | (txt_fmt->bgColor<<4));
   }
   
   ret = GetTxtFmtRemainingSpace(emsData, i, 
 *offset,  /* UDH length. */
 EMS_TXTFMT_IEDL+ fmt_color_len +2,
 txt_char_len, /* total text length, before this obj */
 txt_fmt,
 &udLength,/* out: new UDH length */
 &numOfChar,   /* out: chars in this txt_fmt */
 &numOfOctet); /* out: actual octet can be put at current segment */

   ASSERT(ret == KAL_TRUE);
   
   next_obj = object->next;

   /* why this checking?? */
   /*   for format : "bold1 [snd] rest of bold string"  */
   /*   object list: A (bold rest of bold) -- B (sound) --==>*/
   while((next_obj != NULL && next_obj->Type != EMS_TYPE_TEXT_FORMAT) )
   {
  /* check if the object fall in the current text format. */
  if((object->OffsetToText + numOfOctet) > next_obj->OffsetToText)
  {
 kal_uint16   udLength_tmp;
 kal_uint16  numOfChar_tmp, numOfOctet_tmp;   

 {
#ifdef __EMS_REL5__
if(EMS_CNTX(isCompressSupp) == KAL_TRUE &&
   i->totalExtObjOctet > 0 ) 
{
   next_obj = next_obj->next;
   continue;
}
else
{
   if( EXT_OBJ_INFO_P(next_obj, isExtObj) == KAL_TRUE )
   {
  /* The total content shall be included, or */
  /* the text after the extended object will be accommodated to current seg. */
  kal_uint16 obj_len;
  EMSGetExtObjLen(next_obj, &obj_len, NULL);

  objLen += (2 + obj_len) ; 
   }
   else if ( EXT_OBJ_INFO_P(next_obj, isReusedObj) == KAL_TRUE)
   {
  objLen += 5;/* IE +IEL (2), ref. no (1), position (2) */
   }
   else
#endif
   {
#ifdef __EMS_REL5__
  EMSNeedObjDistInd(emsData, &next_obj);

  if (next_obj->Type == EMS_TYPE_OBJ_DIST_IND)
 obj_dist_ind  = next_obj;

  if (KAL_TRUE == EMSCheckObjDistInd(next_obj))
 i->ObjDistIndCount --;
#endif
  objLen += getObjLength((EMSTYPE)next_obj->Type, next_obj->data, 0/*dummy*/, NULL); 
   }
#ifdef __EMS_REL5__
}
#endif
 }

 /* plus this object's UDH length to calculate this remaining space 
  * for text format string */  
 if(GetTxtFmtRemainingSpace(emsData, i, 
  *offset,
  EMS_TXTFMT_IEDL+ fmt_color_len +2+objLen,
  txt_char_len,
  txt_fmt,
  &udLength_tmp,
  &numOfChar_tmp, 
  &numOfOctet_tmp) == KAL_TRUE)
 {
if((object->OffsetToText + numOfOctet_tmp) < next_obj->OffsetToText)
{
   /* The next object can "NOT" be accommodated in the same segment */

   /* Although the legth is enough for raw data of next object, the  */
   /* next_obj->OffsetToText is out of the text in this segment. */

   /* Move next obj and text from next_obj->OffsetToText to next segment. */
   numOfOctet = next_obj->OffsetToText - object->OffsetToText;
   needNotFinObj = KAL_TRUE;
   break;
}
else
{
   /* the next object can be accommodated in the same segment */
   /* try the next one if it exist. */
   udLength  = udLength_tmp;
   numOfChar = numOfChar_tmp;
   numOfOctet = numOfOctet_tmp;
}
 }
 else
 {
/* the next object can NOT be accomodated in the same segment */
/* "bold""[snd] rest of bold string" */
/* |- seg1 -|   | -- seg2 , ...  --|  */
numOfOctet = next_obj->OffsetToText - object->OffsetToText;
needNotFinObj = KAL_TRUE;
break;
 } 

 next_obj = next_obj->next;

#ifdef __EMS_REL5__
 if (obj_dist_ind != NULL)
 {
ReleaseEMSObject(obj_dist_ind ); 
obj_dist_ind  = NULL;
 }
#endif
  }
  else
 break;
   }/* while */

#ifdef __EMS_REL5__
   if (obj_dist_ind != NULL)
  ReleaseEMSObject(obj_dist_ind ); 

   i->ObjDistIndCount = objDistCnt;
#endif
  
   if(needNotFinObj == KAL_TRUE || 
  is_TPUD_avail(i, udLength, numOfChar + txt_char_len, 0) == KAL_FALSE)
   {
  EMSObject *new_tf_obj;

  /* this Text Format object can not be filled within one segment,
   * thus rest of this text format will be filled in other segments */

  /* 
   * if the following object exists and is not a text formatted object
   * we need to ensure the sequence of objects, eg, 
   * "bold1 [snd] rest of bold string"
   *
   * "bold [snd]"  "rest of bold string"
   * |- seg1 --|   | -- seg2 , ...  --|
   */

  new_tf_obj = EMSObjectAllocate(KAL_TRUE);
  if(new_tf_obj == NULL)
  {
 i->error = EMS_NO_MEMORY;
 return KAL_TRUE;
  }

  new_tf_obj->Type = object->Type;
  new_tf_obj->next = object->next;
  ASSERT(EMS_R(new_tf_obj->data) != NULL);
  
  /* update the OffsetToText and textLength of new_tf_obj . */
  EMS_P(new_tf_obj->data,text_format) = EMS_P(object->data,text_format);  
  EMS_P(new_tf_obj->data,text_format.textLength) -= numOfOctet;
  new_tf_obj->OffsetToText = object->OffsetToText + numOfOctet;

  /* before assign new NOT Finish new_tf_obj , release the old one */
  EMS_RELEASE_NOT_FINISH_OBJ(i);

  i->notFinishedObject = new_tf_obj ;
  i->notFinishedObjectProcessed = KAL_FALSE;
  /* i->NumFinishedObject++; |+ for new_tf_obj +|  */

   } /* if (is_TPUD_avail) */

   /* Don't add text fmt IE if the length is 0. */
   if (numOfOctet == 0 )
   return KAL_TRUE;

   if((needPack == KAL_TRUE) && (i->TPUD[i->numSegment] != NULL))
   /* if(needPack == KAL_TRUE) */
   {
  *(data+*offset)   = EMS_TEXT_FORMAT_IEI;  /* IEI */
  *(data+*offset+1) = EMS_TXTFMT_IEDL + fmt_color_len ;/* IEDL */
  *(data+*offset+2) = (kal_uint8) ((text_OffsetToText - i->baseTextOffset)/2);
  *(data+*offset+3) = (kal_uint8) (numOfOctet/2); /* in terms of character */
  *(data+*offset+4) = fmt_mode;

  if ( fmt_color_len > 0)
 *(data+*offset+5) = fmt_color;
   }
   
   *offset += (EMS_TXTFMT_IEDL + fmt_color_len +2); /* IEI + IEDL */
   i->UDHL[i->numSegment] += (EMS_TXTFMT_IEDL + fmt_color_len +2);
   
   return KAL_TRUE;

} /* end of PackTextFormat */

#endif

/* 
 * NOTE: from Protocol Stack is Big-Endian for UCS2  
 *   eg, for 'a' (UCS2) ==> 0x00 0x61
 * 
 * however, depend on the implementation of the entity (MMI or PC tool, etc)
 * who use the EMS library, we define a EMS_LITTLE_ENDIAN to let the entity 
 * choose to use little-endian. Once EMS_LITTLE_ENDIAN is defined, 
 * the whote text buffer is treated as little-endian in EMS library.
 *
 * OTHERWISE, EMS library adopts big-endian as default.
 */
void EMSTextCpy(kal_uint8  *dest, 
kal_uint8  *src, 
kal_uint16 len, 
kal_uint8  dcs, 
kal_bool   isPack)
{
   kal_uint16 i;
	
   if(dcs == SMSAL_UCS2_DCS) 	
   {
#ifdef EMS_LITTLE_ENDIAN
  for(i = 0 ; i < len/2 ; i++)
  {
 *(dest+2*i)   = *(src+2*i+1);
 *(dest+2*i+1) = *(src+2*i);
  }
#else
  ems_mem_cpy(dest, src, len);
#endif
   }
   else
   {
  /* gsm 7-bit or 8bit data : 
   * skip the 1st byte of each character (contains two bytes) */
  if(isPack == KAL_TRUE)
  {
 for(i = 0 ; i < len ; i++)  
#ifdef EMS_LITTLE_ENDIAN
*(dest+i)=*(src+2*i);
#else
*(dest+i)=*(src+2*i+1);
#endif
  }
  else
  {
 for(i = 0 ; i < len ; i++)  
 {
#ifdef EMS_LITTLE_ENDIAN
*(dest+2*i)   = *(src+i);
*(dest+2*i+1) = 0;
#else
*(dest+2*i) = 0;
*(dest+2*i+1) = *(src+i);
#endif
 }
  }
   }
} /* end of EMSTextCpy */



kal_bool DecodeTFAttribute(kal_uint8 ie_len, 
   kal_uint8 *data, 
   EMSTextFormat *tf)
{
   tf->Alignment = (EMSALIGNMENT) (*data & 0x03);
   tf->FontSize  = (EMSFONTSIZE ) (*data & 0x0c);
   tf->isBold  = ((*data & 0x10) == 0x10) ? KAL_TRUE : KAL_FALSE;
   tf->isItalic= ((*data & 0x20) == 0x20) ? KAL_TRUE : KAL_FALSE;
   tf->isUnderline = ((*data & 0x40) == 0x40) ? KAL_TRUE : KAL_FALSE;
   tf->isStrikethrough = ((*data & 0x80) == 0x80) ? KAL_TRUE : KAL_FALSE;
  
   tf->fgColor = EMS_BLACK;
   tf->bgColor = EMS_WHITE;

   if(ie_len > 3)
   {
  tf->fgColor = *(data+1) & 0x0f;
  tf->bgColor = (*(data+1) & 0xf0) >> 4;
   }

   /* check alignment */
   if(tf->Alignment != EMS_ALIGN_LEFT   && 
  tf->Alignment != EMS_ALIGN_CENTER &&
  tf->Alignment != EMS_ALIGN_RIGHT  &&
  tf->Alignment != EMS_ALIGN_LANG_DEP)
   {
  return KAL_FALSE;
   }

   /* check font size */
   if(tf->FontSize != EMS_FONTSIZE_NORMAL &&
  tf->FontSize != EMS_FONTSIZE_LARGE  &&
  tf->FontSize != EMS_FONTSIZE_SMALL)
   {
  return KAL_FALSE;
   }

   return KAL_TRUE;

} /* end of DecodeTFAttribute */



