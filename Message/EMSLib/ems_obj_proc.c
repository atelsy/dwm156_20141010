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
* ems_obj_proc.c
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


#define EMS_OBJ_PROC_C

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
#include "ems_init.h"
#include "ems_utils.h"
#include "ems_obj_proc.h"
#include "ems_txt_proc.h"
#include "ems_segment_proc.h"



#ifndef __SLIM_EMS__
EMS_OBJ_PACK_FUNCTION EMS_OBJ_PACK[NUM_OF_EMS_SUPPORTED_OBJ]=
{  
   PackTextFormat,  /* EMS_TYPE_TEXT_FORMAT */
   PackPredefSound, /* EMS_TYPE_PREDEF_SND  */
   PackUserDefSound,/* EMS_TYPE_USERDEF_SND */ 
   PackPredefAnimation, /* EMS_TYPE_PREDEF_ANM  */
   PackAnimation,   /* EMS_TYPE_USERDEF_ANM */
   PackPicture  /* EMS_TYPE_PIC */
};


EMSObject *EMSObjectAllocate(kal_bool is_data_present)
{
   EMSObject *newObj = NULL;
   kal_uint8 size = sizeof(EMSObject);

#ifdef __EMS_REL5__
   size += sizeof(EMSExtObjInfo);
#endif

   if(is_data_present == KAL_TRUE) 
  size += sizeof(EMSObjData);

   newObj = (EMSObject*) EMS_MALLOC(size);
   if(newObj != NULL)
   {
  ems_mem_set(newObj, 0, size);

  if(is_data_present == KAL_TRUE)
  {
#ifdef __EMS_REL5__
 newObj->data = (EMSObjData*) ((kal_uint8*)newObj+sizeof(EMSObject)+sizeof(EMSExtObjInfo));
#else
 newObj->data = (EMSObjData*) ((kal_uint8*)newObj+sizeof(EMSObject));
#endif
  }
   }

   return newObj;

} /* end of EMSObjectAllocate */


kal_uint8 *EMSAllocObjPduBuff(kal_uint16 size, EMSObject *obj)
{
   kal_uint8 *pdu_ptr;

   pdu_ptr = (kal_uint8 *) EMS_get_mem_from_pdu_pool(size, obj);

   if(pdu_ptr == NULL)
  return NULL ;

   ems_mem_set(pdu_ptr, 0, size);

   return (pdu_ptr);

} /* end of EMSAllocObjPduBuff */


void EMSFreeObjPduBuff(kal_uint8 *pdu_ptr, EMSObject *obj)
{
   if(pdu_ptr == NULL)
  return ;

   EMS_free_mem_to_pdu_pool(pdu_ptr, obj);
   return ;

} /* end of EMSFreeObjPduBuff */


#ifdef __EMS_REL5__
void EMSHoldObjPduBuff(kal_uint8 *pdu_ptr )
{
   EMSPduBuffMB  *pdu_mb;

   if(pdu_ptr == NULL)
  return ;

   pdu_mb = (EMSPduBuffMB *)(pdu_ptr - sizeof(EMSPduBuffMB));

   EMS_EXT_ASSERT( pdu_mb->ref_count != 0, pdu_mb->ref_count, 0, 0);

   pdu_mb->ref_count ++ ;

   return ;
} /* end of EMSHoldObjPduBuff */

void EMSUpdateObjPtr(EMSObject *obj )
{
   EMSPduBuffMB  *pdu_mb;

   EMS_ASSERT (IS_EMS_DATA_NEED(obj->Type) == KAL_TRUE  );
   pdu_mb = (EMSPduBuffMB *)(obj->data->common.pdu - sizeof(EMSPduBuffMB));

   EMS_ASSERT(pdu_mb->obj != obj);

   pdu_mb->obj = obj;
   return ;
} /* end of EMSUpdateObjPtr */

void EMSUpdatePduBuffPtr(EMSObject *obj , kal_uint8 *new_pdu_ptr)
{
   kal_uint8   *old_pdu_ptr;

   EMS_ASSERT(IS_EMS_DATA_NEED(obj->Type) == KAL_TRUE  );

   old_pdu_ptr = obj->data->common.pdu ;
   obj->data->common.pdu = new_pdu_ptr;

   {
  EMSObject   *obj_tmp;
  EMS_ASSERT(EXT_OBJ_INFO_P(obj, isReusedObj) == KAL_FALSE) ;
  obj_tmp = obj->next;

  while (obj_tmp != NULL)
  {
 if ((EXT_OBJ_INFO_P(obj_tmp, isExtObj) == KAL_FALSE)  &&
 (EXT_OBJ_INFO_P(obj_tmp, isReusedObj) == KAL_TRUE) &&
 (EXT_OBJ_INFO_P(obj_tmp, ExtObjRef) == EXT_OBJ_INFO_P(obj, ExtObjRef)) &&
 (obj_tmp->Type == obj->Type))
 {
EMS_ASSERT(obj_tmp->data->common.pdu == old_pdu_ptr ); 
obj_tmp->data->common.pdu = new_pdu_ptr;
 }
 obj_tmp = obj_tmp->next ;
  }
   }
   return ;
} /* end of EMSUpdatePduBuffPtr */
#endif



/*****************************************************************************
* FUNCTION
*  ReleaseEMSObject
* DESCRIPTION
*   This function releases the memory held by EMS object.
*
* PARAMETERS
*  a  IN  object
* RETURNS
*  EMSTATUS
* GLOBALS AFFECTED
*  none
*****************************************************************************/
EMSTATUS ReleaseEMSObject (EMSObject *object)
{
   ASSERT( object != NULL );
   
   switch(object->Type)
   {
  case EMS_TYPE_USERDEF_ANM:		
   if(EMS_P(object->data, animation.pdu) != NULL)
  EMSFreeObjPduBuff(EMS_P(object->data, animation.pdu), object);
   break;
			  
  case EMS_TYPE_PIC:	   
   if(EMS_P(object->data, picture.pdu) != NULL)
  EMSFreeObjPduBuff(EMS_P(object->data, picture.pdu), object);
   break;
   
  case EMS_TYPE_USERDEF_SND:
  	  if(EMS_P(object->data, sound.pdu) != NULL)
  EMSFreeObjPduBuff(EMS_P(object->data, sound.pdu), object);
   break; 

  case EMS_TYPE_VCARD:
  case EMS_TYPE_VCALENDAR:
  	  if(EMS_P(object->data, vapp.pdu) != NULL)
  EMSFreeObjPduBuff(EMS_P(object->data, vapp.pdu), object);
   break; 
   }
   EMS_MFREE(object);
   return EMS_OK;
} /* end of ReleaseEMSObject */

/*****************************************************************************
* FUNCTION
*  DeleteEMSObjectFromList
* DESCRIPTION
*   This function deletes an EMS object from object list, 
*   mainly for link list pointer manipulation.
*
* PARAMETERS
*  a  IN/OUT  emsData
*  b  IN  object
* RETURNS
*  none
* GLOBALS AFFECTED
*  none
*****************************************************************************/
void DeleteEMSObjectFromList(EMSData *emsData, EMSObject *Object)
{
   EMSInternalData *internal = (EMSInternalData*)emsData->internal;

#ifdef __EMS_REL5__
   EMSObject *obj ;
   EMSTATUS ret;
   kal_uint16 obj_len;
   kal_bool need_switch_obj = KAL_FALSE;

   if( EXT_OBJ_INFO_P(Object, isExtObj) == KAL_TRUE ) 
   {
  ret = EMSGetExtObjLen(Object, &obj_len, NULL);
  ASSERT(ret == EMS_OK);

  obj = Object->next;

  while (obj != NULL)
  {
 if ((EXT_OBJ_INFO_P(obj, isExtObj) == KAL_FALSE)  &&
 (EXT_OBJ_INFO_P(obj, isReusedObj) == KAL_TRUE) &&
 (EXT_OBJ_INFO_P(obj, ExtObjRef) == EXT_OBJ_INFO_P(Object, ExtObjRef)) &&
 (obj->Type == Object->Type))
 {
/* switch both objects */
EXT_OBJ_INFO_P(obj, isExtObj)= KAL_TRUE ;
EXT_OBJ_INFO_P(obj, isReusedObj) = KAL_FALSE ;

EMSUpdateObjPtr(obj);

/* change old object for freeing pdu buffer */
EXT_OBJ_INFO_P(Object, isExtObj)= KAL_FALSE ;
EXT_OBJ_INFO_P(Object, isReusedObj) = KAL_TRUE ;

need_switch_obj = KAL_TRUE;
internal->RefNumOfSwitchObj = EXT_OBJ_INFO_P(Object, ExtObjRef);
internal->isObjSwitched = KAL_TRUE ;
break;
 }

 obj= obj->next ;
  }

  if (need_switch_obj == KAL_TRUE)
 internal->totalExtObjOctet -= 5 ; /* remove a reused object. */
  else
 internal->totalExtObjOctet -= (obj_len + 2) ;

  internal->numOfExtObject --;
   }
   else if (EXT_OBJ_INFO_P(Object, isReusedObj) == KAL_TRUE )
   {
  internal->totalExtObjOctet -= 5 ;
  internal->numOfExtObject -- ;
   }
   else
#endif
   {
  if (IS_TXT_FMT_OBJ(Object) == KAL_TRUE)
 internal->totalTxtFmtOctet -= getObjLength((EMSTYPE)Object->Type, EMS_R(Object->data), 0, NULL);
  else
 internal->totalObjOctet -= getObjLength((EMSTYPE)Object->Type, EMS_R(Object->data), 0, NULL);

  internal->numOfEMSObject--;
   }


#ifdef __EMS_REL5__
   if(EMS_CNTX(isCompressSupp) == KAL_TRUE)
  internal->needReCompress = KAL_TRUE;
#endif

   /* Update object list. */
   if((kal_uint32*)emsData->listHead == (kal_uint32*)Object && 
  (kal_uint32*)emsData->listTail == (kal_uint32*)Object)
   {
  /* there is only one object */
  emsData->listHead = NULL;
  emsData->listTail = NULL;
   }
   else if((kal_uint32*)emsData->listHead == (kal_uint32*)Object)
   {
  /* delete the first object */
  emsData->listHead = Object->next;		
  (Object->next)->prev = NULL;	
   }
   else if((kal_uint32*)emsData->listTail == (kal_uint32*)Object)
   {
  /* delete the last object */
  emsData->listTail = Object->prev;		
  (Object->prev)->next = NULL;
   }
   else
   {
  /* delete the intermediate object */ 
  (Object->prev)->next = Object->next;
  (Object->next)->prev = Object->prev;
   }

   ReleaseEMSObject(Object); /* release memory */
} /* end of DeleteEMSObjectFromList */ 


/* 
 * original : ancestor <--> obj1 <--> obj2 <--> descendant
 * new:   ancestor <--> obj2 <--> obj1 <--> descendant
 */
void EMSSwapObject(EMSData *emsData, EMSObject *obj1, EMSObject *obj2)
{
   EMSObject *ancestor, *descendant;

   if(obj1 == NULL || obj2 == NULL) return;

   ancestor = obj1->prev;
   descendant = obj2->next;

   if(ancestor != NULL)
  ancestor->next = obj2;

   if(descendant != NULL)
  descendant->prev = obj1;

   obj2->prev = ancestor;
   obj2->next = obj1;
   obj1->prev = obj2;
   obj1->next = descendant;

   if((kal_uint32*)obj1 == (kal_uint32*)emsData->listHead)
  emsData->listHead = obj2;

   if((kal_uint32*)obj2 == (kal_uint32*)emsData->listTail)
  emsData->listTail = obj1;
} /* end of EMSSwapObject */



/*****************************************************************************
* FUNCTION
*  AddPicture
* DESCRIPTION
*   This function adds a picture.
*
* PARAMETERS
*  a  IN  picture, input picture
*  b  IN/OUT  object
* RETURNS
*  EMSTATUS
* GLOBALS AFFECTED
*  none
*****************************************************************************/
EMSTATUS AddPicture(EMSObject *object,
kal_uint8 attribute, 
kal_uint8 vDim,
kal_uint8 hDim,
kal_uint8 bitsPerPixel, 
kal_uint16 pdu_length,
kal_uint8 *pdu)
{
   kal_uint16 length;
   EMSPicture *newPic;
   
   ASSERT(EMS_R(object->data) != NULL);
   newPic = &EMS_P(object->data,picture);

   length = (bitsPerPixel * vDim * hDim  + 7) >>3; 
   
   if(pdu_length != length)
   {
  return EMS_INVALID_OBJECT;
   }
  	  
   newPic->attribute= attribute;
   newPic->vDim = vDim;
   newPic->hDim = hDim;
   newPic->pdu_length   = length; 
   newPic->bitsPerPixel = bitsPerPixel;
   newPic->pdu  = (kal_uint8*)EMSAllocObjPduBuff(length, object);	   	  

   if(newPic->pdu == NULL)
  return EMS_NO_MEMORY;   

   ems_mem_cpy(newPic->pdu, pdu, length);	
  
   return EMS_OK;
	
} /* end of AddPicture */

/*****************************************************************************
* FUNCTION
*  AddUsrDefAnim
* DESCRIPTION
*   This function adds an user defined animation.
*
* PARAMETERS
*  a  IN  animation, input animation
*  b  IN/OUT  object
* RETURNS
*  EMSTATUS
* GLOBALS AFFECTED
*  none
*****************************************************************************/
EMSTATUS AddUsrDefAnim(EMSObject *object, 
   kal_uint8 attribute,
   kal_uint8 cFrame,
   kal_uint8 cRepeat,  
   kal_uint8 Duration,
   kal_uint8 vDim, 
   kal_uint8 hDim,
   kal_uint8 bitsPerPixel, 
   kal_uint16 pdu_length,
   kal_uint8 *pdu   )

{
   kal_uint16 length;
   EMSAnim *newAnim;

   ASSERT(EMS_R(object->data) != NULL);   

   newAnim = &EMS_P(object->data,animation);
   
   length = ((bitsPerPixel * hDim * vDim + 7) >>3 ) * cFrame; 

   if(pdu_length != length)
   {
  return EMS_INVALID_OBJECT;
   }

   newAnim->attribute= attribute;
   newAnim->vDim = vDim;
   newAnim->hDim = hDim;
   newAnim->pdu_length   = length;  
   newAnim->bitsPerPixel = bitsPerPixel;
   newAnim->cFrame   = cFrame;
   newAnim->cRepeat  = cRepeat;   
   newAnim->Duration = Duration;
   newAnim->pdu  = (kal_uint8*) EMSAllocObjPduBuff(length, object);	   	  

   if(newAnim->pdu == NULL)
  return EMS_NO_MEMORY;

   ems_mem_cpy(newAnim->pdu, pdu, length);	   	

   return EMS_OK;	
} /* end of AddUsrDefAnim */

/*****************************************************************************
* FUNCTION
*  AddPreDefAnim
* DESCRIPTION
*   This function adds an pre-defined animation.
*
* PARAMETERS
*  a  IN  animation, input animation
*  b  IN/OUT  object
* RETURNS
*  EMSTATUS
* GLOBALS AFFECTED
*  none
*****************************************************************************/
EMSTATUS AddPreDefAnim(kal_uint8 PredefNo, EMSObject *object)
{ 
   /* check whether this animation is valid or not */
   if(PredefNo > EMS_MAX_ANIM_PREDEF_NO)
  return EMS_INVALID_OBJECT;
 
   object->PredefNo = PredefNo; 
   return EMS_OK;	
} /* end of AddPreDefAnim */


/*****************************************************************************
* FUNCTION
*  AddUsrDefSnd
* DESCRIPTION
*   This function adds an user defined sound.
*
* PARAMETERS
*  a  IN  snd, input sound
*  b  IN/OUT  object
* RETURNS
*  EMSTATUS
* GLOBALS AFFECTED
*  none
*****************************************************************************/
EMSTATUS AddUsrDefSnd(EMSObject   *object,
  kal_uint8   attribute, 
  kal_uint16  pdu_length, 
  kal_uint8   *pdu)
{

   EMS_P(object->data,sound.attribute)  = attribute;  
   EMS_P(object->data,sound.pdu_length) = pdu_length;  
   EMS_P(object->data,sound.pdu)= (kal_uint8*) EMSAllocObjPduBuff(pdu_length, object);	
	   	  
   if(EMS_P(object->data,sound.pdu) == NULL)
  return EMS_NO_MEMORY;
	   	  
   ems_mem_cpy(EMS_P(object->data,sound.pdu), pdu, pdu_length);	   	 	  	  

   return EMS_OK;
	
} /* end of AddUsrDefSnd */

/*****************************************************************************
* FUNCTION
*  AddPreDefSnd
* DESCRIPTION
*   This function adds an pre-defined sound.
*
* PARAMETERS
*  a  IN  snd, input sound
*  b  IN/OUT  object
* RETURNS
*  EMSTATUS
* GLOBALS AFFECTED
*  none
*****************************************************************************/
EMSTATUS AddPreDefSnd(kal_uint8 PredefNo, EMSObject *object)
{ 
   /* check whether this sound is valid or not */
   if(PredefNo > EMS_MAX_SND_PREDEF_NO)
  return EMS_INVALID_OBJECT;
 
   object->PredefNo=PredefNo; 
   return EMS_OK;	
} /* end of AddPreDefSnd */


/*****************************************************************************
* FUNCTION
*  AddObjectIntoListByOffset
* DESCRIPTION
*   This function adds an object into object list. 
*
* PARAMETERS
*  a  IN/OUT  emsData
*  b  IN/OUT  newObj
* RETURNS
*  none
* GLOBALS AFFECTED
*  none
*****************************************************************************/
void AddObjectIntoListByOffset(EMSData *emsData, EMSObject *newObj)
{
   EMSObject *obj;
   EMSObject *old_curr_obj = emsData->CurrentPosition.Object;   

   if ((obj = emsData->listHead) != NULL)
   {
  while (obj != NULL )
  {
 if (newObj->OffsetToText < obj->OffsetToText)
break;
 else
obj = obj->next ;
  }

  if (obj != NULL)
 emsData->CurrentPosition.Object = obj->prev;
  else
 emsData->CurrentPosition.Object = emsData->listTail;

   }

   AddObjectIntoList(emsData, newObj);

   emsData->CurrentPosition.Object = old_curr_obj ; 
	
} /* end of AddObjectIntoListByOffset */


/*****************************************************************************
* FUNCTION
*  AddObjectIntoList
* DESCRIPTION
*   This function adds an object into object list. 
*
* PARAMETERS
*  a  IN/OUT  emsData
*  b  IN/OUT  newObj
* RETURNS
*  none
* GLOBALS AFFECTED
*  none
*****************************************************************************/
void AddObjectIntoList(EMSData *emsData, EMSObject *newObj)
{
   EMSInternalData *internal=(EMSInternalData*)emsData->internal;
   EMSObject *tmpObj;
   EMSObject *obj = emsData->CurrentPosition.Object;   

   ASSERT(internal != NULL);


#ifdef __EMS_REL5__

   if( EXT_OBJ_INFO_P(newObj, isExtObj) == KAL_TRUE ) 
   {
  EMSTATUS ret;
  kal_uint16 obj_len;
  ret = EMSGetExtObjLen(newObj, &obj_len, NULL);
  ASSERT(ret == EMS_OK);

  internal->totalExtObjOctet += (obj_len + 2) ;
  internal->numOfExtObject++;
   }
   else if (EXT_OBJ_INFO_P(newObj, isReusedObj) == KAL_TRUE )
   {
  internal->totalExtObjOctet += 5 ;
  internal->numOfExtObject++;
   }
   else
#endif
   {
  if (IS_TXT_FMT_OBJ(newObj) == KAL_TRUE)
 internal->totalTxtFmtOctet += getObjLength((EMSTYPE)newObj->Type, EMS_R(newObj->data), 0, NULL);
  else
 internal->totalObjOctet += getObjLength((EMSTYPE)newObj->Type, EMS_R(newObj->data), 0, NULL);

  internal->numOfEMSObject++;
   }

   /* Manage EMS object list  */
   if(emsData->listHead == NULL)
   {
  /* this is the first object */
  emsData->listHead = newObj;
  emsData->listTail = newObj;
   }	
   else if((kal_uint32*)obj == (kal_uint32*)emsData->listTail)
   {
  /* current object is the last one */
  /* add newObj to tail. */
  emsData->listTail->next = newObj;
  newObj->prev = emsData->listTail;
  emsData->listTail = newObj;
   }
   else if(obj == NULL)
   {
  /* the new object will be inserted to the head of object list */
  /* add newObj to head. */
  emsData->listHead->prev = newObj;
  newObj->next = emsData->listHead;
  emsData->listHead = newObj;		
   }
   else
   {
  /* insert this new object */
  tmpObj = obj->next;
  obj->next = newObj;
  newObj->next = tmpObj;
  tmpObj->prev = newObj;
  newObj->prev = obj;
   }
	
} /* end of AddObjectIntoList */

EMSTATUS AddToObjQueue(EMSObjQueue **objQueue, EMSObject *newObj)
{

   EMSObjQueue *newNode = NULL;
   EMSObjQueue *tmpNode ;

   newNode = (EMSObjQueue*) EMS_MALLOC(sizeof(EMSObjQueue));
   if(newNode == NULL)
  return EMS_NO_MEMORY ;
   else
  ems_mem_set(newNode, 0, sizeof(EMSObjQueue));

   newNode->obj = newObj ;

   /* Manage EMS object list  */
   if(*objQueue == NULL)
   {
  /* this is the first object */
  *objQueue = newNode;
   }	
   else 
   {
  /* Add to tail. */
  tmpNode = *objQueue;

  while (tmpNode->next != NULL)
  {
 tmpNode = tmpNode->next ;
  }

  tmpNode->next = newNode ;
   }
	
   return EMS_OK ;

} /* end of AddToObjQueue */

EMSObject* RemoveFromObjQueue(EMSObjQueue **objQueue)
{

   EMSObjQueue *objNode ;
   EMSObject   *obj ;

   if (*objQueue == NULL)
  return NULL ;

   objNode = *objQueue;
   *objQueue = objNode->next;

   obj = objNode->obj;

   EMS_MFREE(objNode);

   return obj;
	
} /* end of RemoveFromObjQueue */

/*****************************************************************************
* FUNCTION
*  PackPredefSound
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
kal_bool PackPredefSound(EMSData   *emsData,
 EMSObject *object, 							
							kal_bool  needPack)
{
   EMSInternalData*i = (EMSInternalData*)emsData->internal;
   kal_uint8 *data;
   kal_uint8 *offset;
   
   offset = &i->TPUDLen[i->numSegment];
   
   if((needPack == KAL_TRUE) && (i->TPUD[i->numSegment] != NULL))
   {
  data = i->TPUD[i->numSegment];

  ASSERT(data != NULL);

  *(data+*offset)   = EMS_PREDEF_SND_IEI;
  *(data+*offset+1) = EMS_PREDEF_IEDL; 	
  *(data+*offset+2) = (kal_uint8) ((object->OffsetToText - i->baseTextOffset)/2);
  *(data+*offset+3) = object->PredefNo;   
   }
   *offset += (EMS_PREDEF_IEDL+2); 
   i->UDHL[i->numSegment] += (EMS_PREDEF_IEDL+2);

   return KAL_TRUE;

} /* end of PackPredefSound */

/*****************************************************************************
* FUNCTION
*  PackPredefAnimation
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
kal_bool PackPredefAnimation(EMSData  *emsData,
 EMSObject*object, 
							kal_bool needPack)
{
   EMSInternalData*i = (EMSInternalData*)emsData->internal;
   kal_uint8 *data;
   kal_uint8 *offset;
   
   offset = &i->TPUDLen[i->numSegment];
   
   if((needPack == KAL_TRUE) && (i->TPUD[i->numSegment] != NULL))
   {
  data = i->TPUD[i->numSegment];

  ASSERT(data != NULL);

  *(data+*offset)   = EMS_PREDEF_ANM_IEI;
  *(data+*offset+1) = EMS_PREDEF_IEDL; 
  *(data+*offset+2) = (kal_uint8) ((object->OffsetToText - i->baseTextOffset)/2);
  *(data+*offset+3) = object->PredefNo;   
   }
   *offset += (EMS_PREDEF_IEDL+2); 
   i->UDHL[i->numSegment] += (EMS_PREDEF_IEDL+2);
   
   return KAL_TRUE;

} /* end of PackPredefAnimation */

/*****************************************************************************
* FUNCTION
*  PackUserDefSound
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
kal_bool PackUserDefSound(EMSData  *emsData,
  EMSObject*object, 
							 kal_bool needPack)
{
   EMSInternalData*i = (EMSInternalData*)emsData->internal;
   kal_uint8 *data;
   kal_uint8 *offset;
   kal_uint8 att_obj_len = 0;

   
   offset = &i->TPUDLen[i->numSegment];
   
   EMS_EXT_ASSERT(object->data->sound.pdu_length <= EMS_MAX_IMELODY_SIZE,
  object->data->sound.pdu_length, 0, 0);

#ifdef __EMS_REL5__
   if(object->data->sound.attribute == EMS_ATTB_NOT_FW)
   {
  /* att_obj_len = 4; */

  EMS_ASSERT(i->ObjDistIndCount > 0) ;
  i->ObjDistIndCount --;
   }
#endif

   if((needPack == KAL_TRUE) && (i->TPUD[i->numSegment] != NULL))
   {
  data = i->TPUD[i->numSegment];
  
  ASSERT(data != NULL);

#if 0
   /* If attribute is not forwarded, object distribution indicator shall be added. */
  if(object->data->sound.attribute == EMS_ATTB_NOT_FW)
  {
 *(data + *offset )= EMS_OBJ_DIST_IND_IEI;
 *(data + *offset + 1) = 2; /* IEDL */
 *(data + *offset + 2) = 1; /* number of IE */
 *(data + *offset + 3) = EMS_ATTB_NOT_FW;  /* Attributes */
  }
#endif

  *(data + *offset + att_obj_len )= EMS_USER_DEF_SND_IEI;
  *(data + *offset + att_obj_len + 1) = EMS_P(object->data,sound.pdu_length) + 1; /* IEDL */
  *(data + *offset + att_obj_len + 2) = (kal_uint8) ((object->OffsetToText - i->baseTextOffset)/2);
  ems_mem_cpy((data+*offset+ att_obj_len +3),
  	  EMS_P(object->data,sound.pdu),
  EMS_P(object->data,sound.pdu_length)
  ); 
   }
   *offset += (EMS_P(object->data,sound.pdu_length)+3 + att_obj_len);
   i->UDHL[i->numSegment] += (EMS_P(object->data,sound.pdu_length)+3 + att_obj_len);
   
   return KAL_TRUE;

} /* end of PackUserDefSound */

/*****************************************************************************
* FUNCTION
*  PackAnimation
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
kal_bool PackAnimation(EMSData  *emsData,
   EMSObject*object, 					
					kal_bool needPack)
{  
   EMSInternalData  *i = (EMSInternalData*)emsData->internal;
   EMSAnim *anim = &EMS_P(object->data,animation);
   kal_uint8 *data;
   kal_uint8 *offset;
   kal_uint8 att_obj_len = 0;
					  
   offset = &i->TPUDLen[i->numSegment];
   

#ifdef __EMS_REL5__
   if(anim->attribute == EMS_ATTB_NOT_FW)
   {
  /* att_obj_len = 4; */

  EMS_ASSERT(i->ObjDistIndCount > 0) ;
  i->ObjDistIndCount --;
   }
#endif

   if((needPack == KAL_TRUE) && (i->TPUD[i->numSegment] != NULL))
   {
  data = i->TPUD[i->numSegment];
  
  ASSERT(data != NULL);

#if 0
   /* If attribute is not forwarded, object distribution indicator shall be added. */
  if(anim->attribute == EMS_ATTB_NOT_FW)
  {
 *(data + *offset )= EMS_OBJ_DIST_IND_IEI;
 *(data + *offset + 1) = 2; /* IEDL */
 *(data + *offset + 2) = 1; /* number of IE */
 *(data + *offset + 3) = EMS_ATTB_NOT_FW;  /* Attributes */
  }
#endif
  /* small/large animation */
  if(anim->hDim == 8 && anim->vDim == 8 && anim->pdu_length == 32 ) /* small animation */
 *(data+*offset + att_obj_len ) = EMS_SMALL_ANM_IEI;
  else if(anim->hDim == 16 && anim->vDim == 16 && anim->pdu_length == 128 ) /* large animation */
 *(data+*offset + att_obj_len) = EMS_LARGE_ANM_IEI;
  else
  {
 EMS_EXT_ASSERT(KAL_FALSE, anim->hDim ,anim->vDim ,anim->pdu_length ) ;
  }
	  
  *(data+*offset + att_obj_len+1) = anim->pdu_length + 1;
  *(data+*offset + att_obj_len+2) = (kal_uint8) ((object->OffsetToText - i->baseTextOffset)/2);
   
  ems_mem_cpy((data+*offset + att_obj_len+3), anim->pdu, anim->pdu_length);
   }

   *offset += (anim->pdu_length + att_obj_len +3);
   i->UDHL[i->numSegment] += (anim->pdu_length + att_obj_len +3);
  
   return KAL_TRUE;

} /* end of PackAnimation */

/*****************************************************************************
* FUNCTION
*  PackPicture
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
kal_bool PackPicture(EMSData*emsData,
 EMSObject  *object, 					  
 kal_bool   needPack)
{
   EMSInternalData  *i = (EMSInternalData*)emsData->internal;
   EMSPicture   *pic_ptr = &EMS_P(object->data,picture);
   kal_uint8 pduOffset = 0; /* reset */
   kal_uint8 iei;
   kal_uint8 *data;
   kal_uint8 *offset;
   kal_uint8 att_obj_len = 0;
					  
   EMS_EXT_ASSERT(pic_ptr->bitsPerPixel==1, 
  pic_ptr->bitsPerPixel, pic_ptr->pdu_length,0 );

   if(pic_ptr->hDim == 16 && pic_ptr->vDim == 16 && pic_ptr->pdu_length == 32 ) /* small animation */
  iei = EMS_SMALL_PIC_IEI;
   else if(pic_ptr->hDim == 32 && pic_ptr->vDim == 32 && pic_ptr->pdu_length == 128 ) /* large animation */
  iei = EMS_LARGE_PIC_IEI;
   else
   {
  EMS_EXT_ASSERT((pic_ptr->hDim & 0x07) == 0, pic_ptr->hDim,
  pic_ptr->bitsPerPixel, pic_ptr->pdu_length);

  iei = EMS_VAR_PIC_IEI; 		   	
  pduOffset = 2;
   }
   
   offset = &i->TPUDLen[i->numSegment];
   
#ifdef __EMS_REL5__
   if(pic_ptr->attribute == EMS_ATTB_NOT_FW)
   {
  /* att_obj_len = 4; */

  EMS_ASSERT(i->ObjDistIndCount > 0) ;
  i->ObjDistIndCount --;
   }
#endif


   if((needPack == KAL_TRUE) && (i->TPUD[i->numSegment] != NULL))
   {
  data = i->TPUD[i->numSegment];

  ASSERT(data != NULL);

#if 0
   /* If attribute is not forwarded, object distribution indicator shall be added. */
  if(pic_ptr->attribute == EMS_ATTB_NOT_FW)
  {
 *(data + *offset )= EMS_OBJ_DIST_IND_IEI;
 *(data + *offset + 1) = 2; /* IEDL */
 *(data + *offset + 2) = 1; /* number of IE */
 *(data + *offset + 3) = EMS_ATTB_NOT_FW;  /* Attributes */
  }
#endif

  *(data+*offset + att_obj_len)   = iei;
  *(data+*offset + att_obj_len+1) = pic_ptr->pdu_length + pduOffset+ 1; /*IEDL*/
  *(data+*offset + att_obj_len+2) = (kal_uint8) ((object->OffsetToText - i->baseTextOffset)/2);

  if(pduOffset == 2)
  {  
 /* variable picture */
 *(data+*offset + att_obj_len+3) = pic_ptr->hDim/8; /* horizontal dimension (octets) */
 *(data+*offset + att_obj_len+4) = pic_ptr->vDim;   /* vertical dimension (pixels) */
  }
  ems_mem_cpy((data+*offset + att_obj_len+3+pduOffset),
  pic_ptr->pdu,
  pic_ptr->pdu_length
 );
   }
	
   *offset += (pic_ptr->pdu_length + att_obj_len+3+pduOffset); 
   i->UDHL[i->numSegment] += (pic_ptr->pdu_length+ att_obj_len + 3+pduOffset);
   
   return KAL_TRUE;

} /* end of PackPicture */

#ifdef __EMS_REL5__

kal_bool EMSCheckObjDistInd( EMSObject *object) 					  
{
   if(EXT_OBJ_INFO_P(object, isExtObj) != KAL_TRUE &&
  EXT_OBJ_INFO_P(object, isReusedObj) != KAL_TRUE &&
  IS_TXT_FMT_OBJ (object) != KAL_TRUE &&
  object->Type != EMS_TYPE_OBJ_DIST_IND &&
  IS_EMS_DATA_NEED(object->Type) == KAL_TRUE  )
   {
  /* need object distrbute indicator IE. */
  if (object->data->common.attribute == EMS_ATTB_NOT_FW)
 return KAL_TRUE;
   }

   return KAL_FALSE;
}

EMSTATUS EMSNeedObjDistInd (EMSData*emsData,
EMSObject  **object)
{

   EMSObject*obj, *newObj;
   EMSInternalData  *i = (EMSInternalData*)emsData->internal;

   if (KAL_FALSE == EMSCheckObjDistInd(*object))
  return EMS_OK;

   if (i->ObjDistIndCount > 0)
  return EMS_OK;

   i->ObjDistIndCount = 1;

   obj = *object;

   while(obj != NULL)
   {
  if (obj->next != NULL)
  {
 if(KAL_TRUE == EMSCheckObjDistInd(obj->next))
i->ObjDistIndCount ++;
 else
break ;
  }
  obj = obj->next ;
   }

   /* object distributed indicator shall be added. */
   newObj = EMSObjectAllocate(KAL_FALSE);   
   if(newObj == NULL)
  return EMS_NO_MEMORY;

   newObj->Type = EMS_TYPE_OBJ_DIST_IND ;
   newObj->OffsetToText = (*object)->OffsetToText;   
   newObj->PredefNo = i->ObjDistIndCount;   

   newObj->next = *object;
   *object = newObj ;

   return EMS_OK;
}

kal_bool EMSPackObjDistInd (EMSData*emsData,
EMSObject  *object, 					  
kal_bool   needPack)
{
   EMSInternalData  *i = (EMSInternalData*)emsData->internal;
   kal_uint8 *data;
   kal_uint8 *offset;
					  
   offset = &i->TPUDLen[i->numSegment];
   
   if((needPack == KAL_TRUE) && (i->TPUD[i->numSegment] != NULL))
   {
  data = i->TPUD[i->numSegment];

  ASSERT(data != NULL);

  *(data + *offset) = EMS_OBJ_DIST_IND_IEI;
  *(data + *offset + 1) = 2; /* IEDL */
  *(data + *offset + 2) = object->PredefNo ; /* number of IE */
  *(data + *offset + 3) = EMS_ATTB_NOT_FW;  /* Attributes */
   }
	
   *offset += 4; 
   i->UDHL[i->numSegment] += 4;
   
   return KAL_TRUE;

} /* end of EMSPackObjDistInd */


/*****************************************************************************
* FUNCTION
*  EMSPackReusedObj
* DESCRIPTION
*   This function packs reused extended object.
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
kal_bool EMSPackReusedObj(EMSData*emsData,
  EMSObject  *object, 					  
  kal_bool   needPack)
{
   EMSInternalData  *i = (EMSInternalData*)emsData->internal;
   kal_uint8 *data;
   kal_uint8 *offset;
					  
   offset = &i->TPUDLen[i->numSegment];
   
   if((needPack == KAL_TRUE) && (i->TPUD[i->numSegment] != NULL))
   {
  data = i->TPUD[i->numSegment];

  ASSERT(data != NULL);

  *(data+*offset)   = EMS_REUSED_EXT_OBJ_IEI;
  *(data+*offset+1) = 3; /*IEDL*/
  *(data+*offset+2) = EXT_OBJ_INFO_P(object, ExtObjRef);
  *(data+*offset+3) = (kal_uint8)((object->OffsetToText/2) >>8) ;
  *(data+*offset+4) = (kal_uint8)((object->OffsetToText/2) & 0x00ff) ;

   }
	
   *offset += 5; 
   i->UDHL[i->numSegment] += 5;
   
   return KAL_TRUE;

} /* end of EMSPackReusedObj */

/*****************************************************************************
* FUNCTION
*  EMSPackExtObj
* DESCRIPTION
*   This function packs reused extended object.
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
kal_bool EMSPackExtObj(EMSData*emsData,
   EMSObject  *object, 					  
   kal_bool   needPack)
{
   EMSInternalData  *i = (EMSInternalData*)emsData->internal;
   kal_uint8 *data;
   kal_uint8 *offset;
   kal_uint16txt_len_in_char, txt_len; 
   kal_uint16packed_obj_len, remain_udh_len;  
   kal_uint16udhLength; /* length of UDH */
					  
   offset = &i->TPUDLen[i->numSegment];
   
   txt_len = object->OffsetToText  - i->textOffset;
   txt_len_in_char = GetTxtLenInChar(emsData->dcs, txt_len);

   do 
   {

  remain_udh_len = EMSGetExtObjRemainingSpace(emsData, i, 
  *offset , /* original UDHL */
  txt_len_in_char);

  EMS_ASSERT (i->PBuff.BufType == EMS_PBUFF_MO_EXT_OBJ);

  if ((remain_udh_len - 2) < (i->PBuff.BufLen - i->PBuff.BufOffset))
 packed_obj_len = remain_udh_len - 2 ;
  else
 packed_obj_len = i->PBuff.BufLen - i->PBuff.BufOffset;

  if((needPack == KAL_TRUE) && (i->TPUD[i->numSegment] != NULL))
  {
 data = i->TPUD[i->numSegment];

 ASSERT(data != NULL);
 ASSERT (i->PBuff.BufPtr != NULL);

 *(data+*offset)   = EMS_EXT_OBJ_IEI ;
 *(data+*offset+1) = packed_obj_len; /* IEDL */
 ems_mem_cpy(data+*offset + 2, i->PBuff.BufPtr + i->PBuff.BufOffset, packed_obj_len);
  }

  *offset += (packed_obj_len + 2); 
  i->UDHL[i->numSegment] += (packed_obj_len + 2);

  i->PBuff.BufOffset += packed_obj_len;

  udhLength = (kal_uint8)GetNumOfChar(emsData->dcs, i->TPUDLen[i->numSegment]);

  if(i->PBuff.BufOffset == i->PBuff.BufLen)
  {
 /* all extened object data is packed */
 EMSFreePBuff(&(i->PBuff));
 break;
  }

  if (txt_len > 0 )
  {
 EMSFlushText(emsData, i, needPack, 
  &udhLength, 
  &txt_len, 
  &txt_len_in_char);

 /* suppose all text shall be writen. */
 EMS_ASSERT(txt_len == 0);
  }

  /* Change to next segment */
  if(EMSChangeSegment(emsData, i) == KAL_FALSE)
  {
 EMSFreePBuff(&(i->PBuff));
 return KAL_FALSE;
  }

  /* get new UDHL and new text length before this obj in "new segment". */
  /* text length shall be 0 */
  txt_len = object->OffsetToText  - i->textOffset;
  txt_len_in_char = GetTxtLenInChar(emsData->dcs, txt_len);
  offset = &i->TPUDLen[i->numSegment];

   } while (1);
   

   ASSERT (i->PBuff.BufPtr == NULL);
   return KAL_TRUE;

} /* end of EMSPackExtObj */


kal_uint16 EMSGetExtObjRemainingSpace(EMSData *emsData, 
  EMSInternalData *i, 
  kal_uint16 ori_udh_len,
  kal_uint16 txt_len)
{
   kal_uint16  remain_char, udhLength;  /* both in char number */ 

   remain_char = i->MaxLenPerSeg - txt_len; 
   udhLength = GetOctetNumFromCharNum(emsData->dcs, remain_char);

   return (udhLength - ori_udh_len); 
}
#endif


#ifdef __EMS_REL5__
EMSObject *CopyResuedObject(EMSData *emsData, EMSObject *obj)
{
   EMSObject   *newObj;
   EMSObject   *obj_tmp;
   
   /* allocate memory for this object */
   newObj = EMSObjectAllocate(IS_EMS_DATA_NEED(obj->Type));
   if(newObj == NULL)
  return NULL;

   newObj->Type = obj->Type;
   newObj->next = NULL;
   newObj->prev = NULL;
   newObj->OffsetToText = obj->OffsetToText;
   
   /* copy EMSObjData */
   ASSERT( EMS_R(newObj->data) != NULL );
   ems_mem_cpy(EMS_R(newObj->data), EMS_R(obj->data), sizeof(EMSObjData));

   /* copy EMSExtObjInfo */
   ems_mem_cpy(((kal_uint8 *)newObj + sizeof(EMSObject)), ((kal_uint8 *)obj + sizeof(EMSObject)), 
sizeof(EMSExtObjInfo));

   obj_tmp = emsData->listHead;

   while (obj_tmp != NULL)
   {
  if ((EXT_OBJ_INFO_P(obj_tmp, isExtObj) == KAL_TRUE)  &&
  (EXT_OBJ_INFO_P(obj_tmp, isReusedObj) == KAL_FALSE) &&
  (EXT_OBJ_INFO_P(obj_tmp, ExtObjRef) == EXT_OBJ_INFO_P(obj, ExtObjRef)) &&
  (obj_tmp->Type == obj->Type))
  {
/* copy the obj. Pdu ptr */
newObj->data->common.pdu = obj_tmp->data->common.pdu ; 

/* hold pdu buffer */
EMSHoldObjPduBuff(obj_tmp->data->common.pdu);
EXT_OBJ_INFO_P(newObj, ExtObjRef)   = EXT_OBJ_INFO_P(obj_tmp, ExtObjRef);

return newObj;
  }

  obj_tmp = obj_tmp->next;
   }

   /* it can't find ext. object for reused. */
   ReleaseEMSObject(newObj);

   return NULL;
}
#endif

EMSObject *CopyEMSObject(EMSObject *obj)
{
   EMSObject *newObj;
   
   /* allocate memory for this object */
   newObj = EMSObjectAllocate(IS_EMS_DATA_NEED(obj->Type));
   if(newObj == NULL)
  return NULL;

   newObj->Type = obj->Type;
   newObj->next = NULL;
   newObj->prev = NULL;
   newObj->OffsetToText = obj->OffsetToText;
   
   /* if object is predefined, EMSObjData is no need */
   if(IS_EMS_DATA_NEED(obj->Type) == KAL_FALSE)
   {
  newObj->PredefNo = obj->PredefNo;
  return newObj;
   }
   
   /* copy EMSObjData */
   ASSERT( EMS_R(newObj->data) != NULL );
   ems_mem_cpy(EMS_R(newObj->data), EMS_R(obj->data), sizeof(EMSObjData));

#ifdef __EMS_REL5__
   /* copy EMSExtObjInfo */
   ems_mem_cpy(((kal_uint8 *)newObj + sizeof(EMSObject)), ((kal_uint8 *)obj + sizeof(EMSObject)), 
sizeof(EMSExtObjInfo));
#endif

   switch(obj->Type)
   {  
  case EMS_TYPE_USERDEF_ANM:
   EMS_P(newObj->data,animation.pdu) = EMSAllocObjPduBuff(EMS_P(obj->data,animation.pdu_length), newObj);

   if(EMS_P(newObj->data,animation.pdu) == NULL)
  goto no_mem;

   ems_mem_cpy(EMS_P(newObj->data,animation.pdu),
   EMS_P(obj->data,animation.pdu),
   EMS_P(obj->data,animation.pdu_length));   
   break;		  
		  			  
  case EMS_TYPE_PIC:
   EMS_P(newObj->data,picture.pdu) = EMSAllocObjPduBuff(EMS_P(obj->data,picture.pdu_length), newObj);

   if(EMS_P(newObj->data,picture.pdu) == NULL)
  goto no_mem;

   ems_mem_cpy(EMS_P(newObj->data,picture.pdu),
   EMS_P(obj->data,picture.pdu),
   EMS_P(obj->data,picture.pdu_length));
   break;  			 
  
  case EMS_TYPE_USERDEF_SND:
   EMS_P(newObj->data,sound.pdu) = EMSAllocObjPduBuff(EMS_P(obj->data,sound.pdu_length), newObj);

   if(EMS_P(newObj->data,sound.pdu) == NULL)
  goto no_mem;

   ems_mem_cpy(EMS_P(newObj->data,sound.pdu),
   EMS_P(obj->data,sound.pdu),
   EMS_P(obj->data,sound.pdu_length)); 
   break;   
 	  
  default : 
  	  
  	  break; 	  
   }/* switch */   
 
   return newObj;
   
no_mem:

   ReleaseEMSObject(newObj);

   return NULL;

} /* end of CopyEMSObject */


/*****************************************************************************
* FUNCTION
*  EMSObjectCompare
* DESCRIPTION
*   This function is to check header of extended object IE.
*
* PARAMETERS
*  a  IN  emsData
*  b  IN  ie_len
*  c  IN  data  , raw data of UDH IE
*  d  OUT obj_data_length,  length of extended object data.
* RETURNS
* EMSTATUS 
* GLOBALS AFFECTED
*  none
*****************************************************************************/
kal_bool EMSObjectCompare(kal_uint8 type, EMSObjData *objData1, EMSObjData *objData2)
{
   
   kal_bool	ret = KAL_FALSE;

   switch(type)
   {		
  case EMS_TYPE_PIC:
		
		   if ( objData1->picture.bitsPerPixel == objData2->picture.bitsPerPixel &&
			   objData1->picture.vDim == objData2->picture.vDim &&
			   objData1->picture.hDim == objData2->picture.hDim &&
			   objData1->picture.hDim == objData2->picture.hDim &&
			   objData1->picture.pdu_length   == objData2->picture.pdu_length )
		   {

			   if (0 == ems_mem_cmp (objData1->picture.pdu, objData2->picture.pdu,
						   objData1->picture.pdu_length))
			   {
				   ret =KAL_TRUE;
		}
		   }
		   break;
			 
  case EMS_TYPE_USERDEF_ANM:	
	
		   if ( objData1->animation.bitsPerPixel == objData2->animation.bitsPerPixel &&
			   objData1->animation.vDim == objData2->animation.vDim &&
			   objData1->animation.hDim == objData2->animation.hDim &&
			   objData1->animation.hDim == objData2->animation.hDim &&
			   objData1->animation.cFrame   == objData2->animation.cFrame   &&
			   objData1->animation.cRepeat  == objData2->animation.cRepeat  &&
			   objData1->animation.Duration == objData2->animation.Duration &&
			   objData1->animation.pdu_length   == objData2->animation.pdu_length )
		   {

			   if (0 == ems_mem_cmp (objData1->animation.pdu, objData2->animation.pdu,
									 objData1->animation.pdu_length))
			   {
				   ret =KAL_TRUE;
			   }
		   }
		   break;	
  				
  case EMS_TYPE_USERDEF_SND:
  case EMS_TYPE_VCARD:
  case EMS_TYPE_VCALENDAR:

 /* EMSAnim and EMSVApp have the same structure. */
		   if (objData1->sound.pdu_length   == objData2->sound.pdu_length )
		   {

			   if (0 == ems_mem_cmp (objData1->sound.pdu, objData2->sound.pdu,
									 objData1->sound.pdu_length))
			   {
				   ret =KAL_TRUE;
			   }
		   }
		   break;	
  default : 
		   break;
  	  
   } /* switch */

   return ret;
}

/*****************************************************************************
* FUNCTION
*  EMSObjHeaderCheck
* DESCRIPTION
*   This function is to check header of extended object IE.
*
* PARAMETERS
*  a  IN  emsData
*  b  IN  ie_len
*  c  IN  data  , raw data of UDH IE
*  d  OUT obj_data_length,  length of extended object data.
* RETURNS
* EMSTATUS 
* GLOBALS AFFECTED
*  none
*****************************************************************************/
EMSTATUS EMSObjectCheck(kal_uint8 type, EMSObjData *objData, kal_bool *isExtObj)
{
   EMSTATUS  ret =EMS_OK;

   if(isExtObj != NULL)
  *isExtObj  = KAL_FALSE ;

   /* pre-defined ANM and SND  */
   if (IS_EMS_DATA_NEED(type) == KAL_FALSE)
  return ret ;

   switch(type)
   {
  case EMS_TYPE_USERDEF_SND:
 {
/* check whether this sound is valid or not */
if(objData->sound.pdu_length > EMS_MAX_IMELODY_SIZE)
{
#ifndef __EMS_REL5__
   ret = EMS_INVALID_OBJECT;
#else
   if(isExtObj != NULL)
  *isExtObj  = KAL_TRUE ;
#endif
}
 }
 break;   

  /*================================
   * small animation :  8 x  8, 4 frames
   * large animation : 16 x 16, 4 frames
   *================================*/
  case EMS_TYPE_USERDEF_ANM:
 {
kal_uint16 pdu_len;
/* check whether this animation is valid or not */

/* colorful picture is only supported, in EMS 5.1 */
if ((objData->animation.bitsPerPixel != 1) || 
(objData->animation.cFrame != 4 ))
{
#ifndef __EMS_REL5__
   return EMS_NOT_SUPPORT_COLOR_PIC;		   
#else
   if (objData->animation.bitsPerPixel != 1 &&
   objData->animation.bitsPerPixel != 2 &&  
   objData->animation.bitsPerPixel != 6 ) 
  return EMS_INVALID_OBJECT;

   if(isExtObj != NULL)
  *isExtObj  = KAL_TRUE ;
#endif
}

pdu_len = ((objData->animation.bitsPerPixel * 
objData->animation.hDim * 
objData->animation.vDim + 7) >>3 ) * 
objData->animation.cFrame; 

 if((objData->animation.hDim != 8 || 
 objData->animation.vDim != 8 || pdu_len != 32) &&  /* small animation */
(objData->animation.hDim != 16 || 
 objData->animation.vDim != 16 || pdu_len != 128) ) /* large animation */
{
#ifndef __EMS_REL5__
   return EMS_INVALID_OBJECT;		   
#else
   if(isExtObj != NULL)
  *isExtObj  = KAL_TRUE ;
#endif
}

if (pdu_len != objData->animation.pdu_length)
   return EMS_INVALID_OBJECT;
 }
 break;			  
			  
  /*================================
   * Picture	 
   *================================*/			  
  case EMS_TYPE_PIC:
 {
kal_uint16 pdu_len;
/* check whether this picture is valid or not */

/* colorful picture is only supported, in EMS 5.1 */
if (objData->picture.bitsPerPixel != 1) 
{
#ifndef __EMS_REL5__
   return EMS_NOT_SUPPORT_COLOR_PIC;		   
#else
   if (objData->picture.bitsPerPixel != 2 &&  
   objData->picture.bitsPerPixel != 6 ) 
  return EMS_INVALID_OBJECT;

   if(isExtObj != NULL)
  *isExtObj  = KAL_TRUE ;
#endif
}

pdu_len = ((objData->picture.bitsPerPixel * 
objData->picture.hDim * 
objData->picture.vDim + 7) >>3 ) ;

if (pdu_len != objData->picture.pdu_length)
   return EMS_INVALID_OBJECT;

if(pdu_len > EMS_MAX_PIC_SIZE )
{
#ifndef __EMS_REL5__
   return EMS_INVALID_OBJECT;		   
#else
   if(isExtObj != NULL)
  *isExtObj  = KAL_TRUE ;
#endif
}
else
{
   if((objData->picture.hDim != 16 || 
   objData->picture.vDim != 16 || pdu_len!= 32 ) && /* small animation */
  (objData->picture.hDim != 32 || 
   objData->picture.vDim != 32 || pdu_len!= 128 ) && /* large animation */
  ((objData->picture.hDim & 0x07)!= 0)  /* for variable size, hDim shall be multiple of 8 */
 )
   {
#ifndef __EMS_REL5__
  return EMS_INVALID_OBJECT;		   
#else
  if(isExtObj != NULL)
 *isExtObj  = KAL_TRUE ;
#endif
   }
}
 }
 break;
#ifdef __EMS_REL5__
  /*================================
   * VCard and VCalendar	 
   *================================*/			  
  case EMS_TYPE_VCARD:
  case EMS_TYPE_VCALENDAR:
 {
if(isExtObj != NULL)
   *isExtObj  = KAL_TRUE ;
 }
 break;
#endif
  default : 
  	  ret = EMS_INVALID_OBJECT;
  	  break; 	  
   }/* switch */


   return ret;
}

#ifdef __EMS_REL5__

/*****************************************************************************
* FUNCTION
*  AddVApp
* DESCRIPTION
*   This function adds vCard or vCalendar.
*
* PARAMETERS
*  a  IN  picture, input picture
*  b  IN/OUT  object
* RETURNS
*  EMSTATUS
* GLOBALS AFFECTED
*  none
*****************************************************************************/
EMSTATUS AddVApp(EMSObject *object,
 kal_uint8 attribute, 
 kal_uint16 pdu_length,
 kal_uint8 *pdu)
{

   EMS_P(object->data,vapp.attribute)  = attribute;  
   EMS_P(object->data,vapp.pdu_length) = pdu_length;  
   EMS_P(object->data,vapp.pdu)= (kal_uint8*) EMSAllocObjPduBuff(pdu_length, object);	
	   	  
   if(EMS_P(object->data,vapp.pdu) == NULL)
  return EMS_NO_MEMORY;
	   	  
   ems_mem_cpy(EMS_P(object->data,vapp.pdu), pdu, pdu_length);	   	 	  	  

   return EMS_OK;
	
}


/*****************************************************************************
* FUNCTION
*  EMSFindRusedObjByObjData
* DESCRIPTION
*   This function is to check header of extended object IE.
*
* PARAMETERS
*  a  IN  emsData
*  b  IN  ie_len
*  c  IN  data  , raw data of UDH IE
*  d  OUT obj_data_length,  length of extended object data.
* RETURNS
* EMSTATUS 
* GLOBALS AFFECTED
*  none
*****************************************************************************/
kal_bool EMSFindRusedObjByObjData(EMSData *emsData, EMSObject *newObj, 
  kal_uint8 type, EMSObjData *objData) 
{
   EMSObject   *obj, *obj_tmp;
   kal_boolret = KAL_FALSE;
   kal_uint16  obj_len;
   EMSTATUSret_status;
   EMSInternalData *internal = (EMSInternalData*)emsData->internal;

   obj = emsData->listHead;

   while (obj != NULL)
   {
  if ((EXT_OBJ_INFO_P(obj, isExtObj) == KAL_TRUE)  &&
  (EXT_OBJ_INFO_P(obj, isReusedObj) == KAL_FALSE) &&
  (obj->Type == type))
  {
 if(KAL_TRUE == EMSObjectCompare(type, obj->data, objData))
 {
kal_bool need_switch_obj = KAL_FALSE;
EMS_ASSERT (newObj->data != NULL);

/* copy the obj. Data */
*(newObj->data) = *(obj->data);

/* hold pdu buffer */
EMSHoldObjPduBuff(obj->data->sound.pdu);
EXT_OBJ_INFO_P(newObj, ExtObjRef)   = EXT_OBJ_INFO_P(obj, ExtObjRef);

if (newObj->OffsetToText == obj->OffsetToText )
{

   if (emsData->CurrentPosition.Object != NULL)
  obj_tmp = emsData->CurrentPosition.Object->next ;
   else
  obj_tmp = emsData->listHead;

   while (obj_tmp != NULL)
   {
  if ( obj_tmp == obj)
  {
 need_switch_obj = KAL_TRUE;
 break;
  }
  obj_tmp = obj_tmp->next;
   }
}

if (newObj->OffsetToText < obj->OffsetToText || need_switch_obj == KAL_TRUE) 
{
   /* reused object is before extended object */
   /* switch both objects */
   EXT_OBJ_INFO_P(newObj, isExtObj) = KAL_TRUE ;
   EMSUpdateObjPtr(newObj);

   EXT_OBJ_INFO_P(obj, isExtObj)= KAL_FALSE ;
   EXT_OBJ_INFO_P(obj, isReusedObj) = KAL_TRUE ;

   ret_status = EMSGetExtObjLen(obj, &obj_len, NULL);
   ASSERT(ret_status == EMS_OK);

   /* decrease object length and add reused object length. */
   internal->totalExtObjOctet -= (obj_len + 2) ;
   internal->totalExtObjOctet += 5 ;
}
else
{
   /* extended object is before reused object */
   EXT_OBJ_INFO_P(newObj, isReusedObj) = KAL_TRUE ;
}

ret = KAL_TRUE;
break;
 }
  }

  obj= obj->next ;
   }

   return ret;
}

/*****************************************************************************
* FUNCTION
*  EMSFindRusedObjByRef
* DESCRIPTION
*   This function is to check header of extended object IE.
*
* PARAMETERS
*  a  IN  emsData
*  b  IN  ie_len
*  c  IN  data  , raw data of UDH IE
*  d  OUT obj_data_length,  length of extended object data.
* RETURNS
* EMSTATUS 
* GLOBALS AFFECTED
*  none
*****************************************************************************/
EMSObject *EMSFindRusedObjByRef(EMSData *emsData, kal_uint8 msg_ref)
{
   EMSObject   *obj;

   obj = emsData->listHead;

   while (obj != NULL)
   {
  if ((EXT_OBJ_INFO_P(obj, isExtObj) == KAL_TRUE)  && 
  (EXT_OBJ_INFO_P(obj, ExtObjRef) == msg_ref))
  {
 return obj;
  }
  obj= obj->next ;
   }

   return NULL;
}

/*****************************************************************************
* FUNCTION
*  EMSObjHeaderCheck
* DESCRIPTION
*   This function is to check header of extended object IE.
*
* PARAMETERS
*  a  IN  emsData
*  b  IN  ie_len
*  c  IN  data  , raw data of UDH IE
*  d  OUT obj_data_length,  length of extended object data.
* RETURNS
* EMSTATUS 
* GLOBALS AFFECTED
*  none
*****************************************************************************/
EMSTATUS EMSObjHeaderCheck(EMSData *emsData, kal_uint8 iei, kal_uint8 ie_len, 
   kal_uint8 *data, kal_uint16 *obj_data_length)
{
   kal_uint16 data_length, max_legal_len;
   kal_uint8  header_len;
   kal_uint8  obj_type;
   EMSInternalData*i = (EMSInternalData*) emsData->internal;

   if (iei == EMS_EXT_OBJ_IEI )  /* Extended Object IEI */ 
  header_len = 7 ;
   else
  header_len = 3 ;

   if(ie_len < header_len )
  return EMS_INVALID_EMS50_IE;

   /* octet 2 and 3 in both Extended Obj and Compression Obj  */
   data_length = (*(data+1) << 8) + (*(data+2)) ;

   if (data_length == 0 || (data_length + header_len) < ie_len)
  return EMS_INVALID_EMS50_IE;

   if (i->UnpackSegNum > 1)
  max_legal_len = i->UnpackSegNum * (EMS_ONE_MSG_OCTET- i->ConcatHdrLen);
   else
  max_legal_len =  EMS_ONE_MSG_OCTET ;

   if ( (data_length + header_len) > max_legal_len  )
  return EMS_INVALID_EMS50_IE;

   if (iei == EMS_EXT_OBJ_IEI )  /* Extended Object IEI */ 
   {
  obj_type= *(data+4);
  /* reserved value */
  if( obj_type >= 0x0d && obj_type <= 0xfe )
 return EMS_INVALID_EMS50_IE;
   }

   if(obj_data_length != NULL )
  *obj_data_length = data_length + header_len;

   return EMS_OK;
}

EMSTATUS EMSFillExtObjIE (EMSObject *obj, kal_uint8 iei, kal_uint8 *obj_buff, kal_uint16 obj_len)
{
   EMSObjData  *objData = obj->data; 

   /* fill header information */
   *(obj_buff) = EXT_OBJ_INFO_P(obj, ExtObjRef);
   *(obj_buff + 1) = (kal_uint8)(obj_len >> 8 ) ;
   *(obj_buff + 2) = (kal_uint8)(obj_len & 0x00ff );

   if (IS_EMS_DATA_NEED_EXT_TYPE(iei) == KAL_TRUE)
  *(obj_buff + 3) = objData->common.attribute ;
   else
  *(obj_buff + 3) = EMS_ATTB_NORMAL;

   *(obj_buff + 4) = iei;
   *(obj_buff + 5) = (kal_uint8)((obj->OffsetToText/2)  >> 8 ) ;
   *(obj_buff + 6) = (kal_uint8)((obj->OffsetToText/2)  & 0x00ff );

   switch(iei)
   {
  case EMS_EXT_PREDEF_SND :   /* User Defined Sound */
  case EMS_EXT_PREDEF_ANM :   /* User Defined Sound */
 *(obj_buff + 7) = obj->PredefNo;
 break;

  case EMS_EXT_USER_DEF_SND : /* User Defined Sound */
  case EMS_EXT_VCARD: /* vCard */
  case EMS_EXT_VCALENDAR :/* vCalendar */
 ems_mem_cpy(obj_buff +7 , objData->common.pdu, objData->common.pdu_length);
 break;

  case EMS_EXT_BW_ANM :  /* B&W Animation */
  case EMS_EXT_2BIT_ANM: /* 2-bit greyscale Animation */
  case EMS_EXT_6BIT_ANM: /* 6-bit greyscale Animation */
 { 
*(obj_buff + 7)  = objData->animation.hDim ; 
*(obj_buff + 8)  = objData->animation.vDim ; 
*(obj_buff + 9)  = objData->animation.cFrame ; 
*(obj_buff + 10) = ((objData->animation.Duration << 4) | 
  (objData->animation.cRepeat & 0x0f) ) ; 

ems_mem_cpy(obj_buff + 11 , objData->animation.pdu, objData->animation.pdu_length);
 }
 break;

  case EMS_EXT_BW_PIC: /* B&W Picture */
  case EMS_EXT_2BIT_PIC:   /* 2-bit greyscale Picture */
  case EMS_EXT_6BIT_PIC:  /* 6-bit greyscale Picture */
 { 
*(obj_buff + 7)  = objData->picture.hDim ; 
*(obj_buff + 8)  = objData->picture.vDim ; 

ems_mem_cpy(obj_buff + 9 , objData->picture.pdu, objData->picture.pdu_length);
 }
 break;
  default:
 /* unrecognized ie, ignore this ie */  
 EMS_ASSERT(KAL_FALSE);
 return EMS_UNSPECIFIED_ERROR;
 break;
   }

   return EMS_OK;
}

/*****************************************************************************
* FUNCTION
*  EMSEncodeExtObjIE
* DESCRIPTION
*   This function is to encode extended object IE.
*
* PARAMETERS
*  a  OUT object,  new object from UDH Ie
*  b  IN  emsData
*  b  IN  len
*  c  IN  data  , raw data of UDH IE
* RETURNS
* EMSTATUS 
* GLOBALS AFFECTED
*  none
*****************************************************************************/
EMSTATUS EMSEncodeExtObjIE(EMSData *emsData, EMSObject *obj, kal_bool needPack)
{

   EMSInternalData*i = (EMSInternalData*) emsData->internal;
   EMSTATUSret ;
   kal_uint8   iei;
   kal_uint16  obj_len ; /* extended object header */

   ret = EMSGetExtObjLen(obj, &obj_len, &iei);
   if(ret != EMS_OK )
  return ret;

   ret = EMSGetPBuff(&(i->PBuff), obj_len, EMS_PBUFF_MO_EXT_OBJ, needPack);

   if(needPack == KAL_TRUE)
   {
  if(ret == EMS_OK && i->PBuff.BufPtr != NULL)
  {
 ret =EMSFillExtObjIE (obj, iei, i->PBuff.BufPtr, obj_len - 7);
 if(ret != EMS_OK)
EMSFreePBuff(&(i->PBuff));
  }
   }

   return ret;
}


/*****************************************************************************
* FUNCTION
*  EMSDecodeExtObjIE
* DESCRIPTION
*   This function is to check header of extended object IE.
*
* PARAMETERS
*  a  OUT object,  new object from UDH Ie
*  b  IN  emsData
*  b  IN  len
*  c  IN  data  , raw data of UDH IE
* RETURNS
* EMSTATUS 
* GLOBALS AFFECTED
*  none
*****************************************************************************/
EMSTATUS EMSDecodeExtObjIE(EMSData *emsData, kal_uint16 len, kal_uint8 *data)
{

   EMSTATUS   ret = EMS_UNSPECIFIED_ERROR;
   EMSObject  *new_object = NULL;
   kal_uint16 data_len, pdu_len, offset;
   kal_uint8  data_type ,bitsPerPixel = 1;
   EMSATTRIBUTE obj_attr = EMS_ATTB_NORMAL;
   kal_bool   isExtObj;

   data_len   = (*(data+1) << 8) + (*(data+2)) ;
   data_type  = *(data + 4) ;
   offset = (*(data+5) << 8) + (*(data+6)) ;

   if ( (*(data+3) & 0x01 ) == 0x01)
  obj_attr = EMS_ATTB_NOT_FW;  

   new_object = EMSObjectAllocate(IS_EMS_DATA_NEED_EXT_TYPE(data_type));   
   if(new_object == NULL)
  return EMS_NO_MEMORY;

   EXT_OBJ_INFO_P(new_object, ExtObjRef) = *data; 

   new_object->OffsetToText = offset * 2 ;

   EMS_EXT_ASSERT (len == (data_len + 7), len, data_len,0);

   switch(data_type)
   {
  case EMS_EXT_PREDEF_SND:   /* Predefined Sound */

 EMS_PRINT_STR("recv ext. predef sound ie\n");

 if( *(data+7) == 0 ||
 *(data+7) <= EMS_MAX_SND_PREDEF_NO) 
 {
new_object->Type = EMS_TYPE_PREDEF_SND;
new_object->PredefNo = *(data + 7);

ASSERT(new_object->data == NULL);
ret = EMS_OK ;
 }

 break;

  case EMS_EXT_PREDEF_ANM:   /* Predefined Animation */

 EMS_PRINT_STR("recv ext. predef animation ie\n");

 if( *(data+7) == 0 ||
 *(data+7) <= EMS_MAX_ANIM_PREDEF_NO) 
 {
new_object->Type = EMS_TYPE_PREDEF_ANM;
new_object->PredefNo = *(data + 7);

ASSERT(new_object->data == NULL); 
ret = EMS_OK ;
 }

 break;

/* #define EMS_EXT_POLY_SND   (0x0c)   |+ Polyphonic melody +| */
  case EMS_EXT_USER_DEF_SND : /* User Defined Sound */

 EMS_PRINT_STR("recv user ext. defined sound ie\n");

 if(data_len > 1)
 {
ASSERT(EMS_R(new_object->data) != NULL);   

new_object->Type = EMS_TYPE_USERDEF_SND;

ret = AddUsrDefSnd(new_object, obj_attr, data_len, (data + 7));
 }
 break;


  case EMS_EXT_BW_ANM :  /* B&W Animation */
  case EMS_EXT_2BIT_ANM: /* 2-bit greyscale Animation */
  case EMS_EXT_6BIT_ANM: /* 6-bit greyscale Animation */
 { 

kal_uint8 cFrame;/* count of frames */
kal_uint8 cRepeat;   /* number of repetitions*/
kal_uint8 Duration;  /* in tenths of a second, eg, 0: 0.1s15: 1.6s */
kal_uint8 vDim;  /* verical number of pixtels */
kal_uint8 hDim;  /* horizontal number of pixtels */

EMS_PRINT_STR("recv variable animation ie\n");
ASSERT(EMS_R(new_object->data) != NULL);

if ( data_type == EMS_EXT_2BIT_ANM)
   bitsPerPixel = 2 ;
else if ( data_type == EMS_EXT_6BIT_ANM)
   bitsPerPixel = 6 ;

new_object->Type = EMS_TYPE_USERDEF_ANM;

hDim   = *(data+7) ;
vDim   = *(data+8) ;
cFrame = *(data+9) ;

Duration = (*(data+10)) >> 4 ;
cRepeat  = (*(data+10) & 0x0f );

pdu_len = ((bitsPerPixel * hDim * vDim + 7) >>3 ) * cFrame; 
  
if(data_len == (pdu_len + 4))
{
   ret = AddUsrDefAnim(new_object, obj_attr, cFrame, cRepeat, 
   Duration, vDim, hDim, bitsPerPixel,
   pdu_len, (data + 11)) ;
}

 }
 break;

  case EMS_EXT_BW_PIC: /* B&W Picture */
  case EMS_EXT_2BIT_PIC:   /* 2-bit greyscale Picture */
  case EMS_EXT_6BIT_PIC:  /* 6-bit greyscale Picture */
 { 
EMS_PRINT_STR("recv variable picture ie\n");
ASSERT(EMS_R(new_object->data) != NULL);

if ( data_type == EMS_EXT_2BIT_PIC )
   bitsPerPixel = 2 ;
else if ( data_type == EMS_EXT_6BIT_PIC )
   bitsPerPixel = 6 ;

new_object->Type  = EMS_TYPE_PIC;

pdu_len = (bitsPerPixel * (*(data+7)) * (*(data+8))  + 7) >>3; 
  
if(data_len == (pdu_len + 2))
{
   ret = AddPicture(new_object, obj_attr, *(data+8),
 (*(data+7) ), bitsPerPixel, pdu_len, (data +9));
}
break;
 }
  case EMS_EXT_VCARD:  /* vCard */
  case EMS_EXT_VCALENDAR : /* vCalendar */

 if(data_len > 0)
 {
ASSERT(EMS_R(new_object->data) != NULL);   

if(data_type == EMS_EXT_VCARD )
   new_object->Type = EMS_TYPE_VCARD;
else
   new_object->Type = EMS_TYPE_VCALENDAR;

ret = AddVApp(new_object, obj_attr, data_len, (data + 7));
 }

 break;

  default:
 /* unrecognized ie, ignore this ie */  
 break;
   }

   if (ret != EMS_OK)
   {
  if(new_object != NULL)
 ReleaseEMSObject(new_object);
   }
   else
   {
  if(new_object != NULL)
  {
 new_object->next = NULL;
 new_object->prev = NULL;

 if (EMS_OK == EMSObjectCheck(new_object->Type, new_object->data, &isExtObj))
 {
EXT_OBJ_INFO_P(new_object, isExtObj)  = isExtObj ;

AddObjectIntoListByOffset(emsData, new_object);
emsData->CurrentPosition.Object = new_object;

if (isExtObj == KAL_TRUE)
   ((EMSInternalData*)emsData->internal)->needReCompress  = KAL_TRUE; 
 }
 else
ReleaseEMSObject(new_object);
  }
   }

   return ret;
}

/*****************************************************************************
* FUNCTION
*  EMSDecodeReusedExtObjIE 
* DESCRIPTION
*   This function is to decode reused extended object.
*
* PARAMETERS
*  a  IN  emsData
*  b  IN  len
*  c  IN  data
* RETURNS
* EMSTATUS 
* GLOBALS AFFECTED
*  none
*****************************************************************************/
EMSTATUS EMSDecodeReusedExtObjIE (EMSData *emsData, kal_uint8 len, kal_uint8 *data)
{
   EMSTATUS   ret = EMS_OK; /* reset */
   EMSObject  *obj,*new_object = NULL;
   kal_uint16 offset;
   kal_uint8  msg_ref ;

   if (len != 3 )
  return EMS_UNSPECIFIED_ERROR;

   msg_ref = *data ;
   offset  = (*(data+1) << 8) + (*(data+2)) ;

   obj = EMSFindRusedObjByRef(emsData, msg_ref);
   if (obj == NULL || (offset * 2) < obj->OffsetToText )
   {
  EMS_PRINT_STR("==== Reused ext obj IE: Ext. msg_ref can't be found ===\n");
  return EMS_UNSPECIFIED_ERROR; /* no extended obj is found */
   }

   new_object = EMSObjectAllocate(IS_EMS_DATA_NEED(obj->Type));
   if(new_object == NULL)
  return EMS_NO_MEMORY;

   new_object ->Type = obj->Type;
   new_object ->next = NULL;
   new_object ->prev = NULL;
   new_object->OffsetToText = offset * 2 ;

   /* if object is predefined, EMSObjData is no need */
   if(IS_EMS_DATA_NEED(obj->Type) == KAL_FALSE)
   {
  new_object->PredefNo = obj->PredefNo;
   }
   else
   {
  ASSERT( obj->data != NULL );
  ems_mem_cpy(EMS_R(new_object->data), EMS_R(obj->data), sizeof(EMSObjData));

  EMSHoldObjPduBuff(obj->data->common.pdu);
  new_object->data->common.pdu  = obj->data->common.pdu ;

  EXT_OBJ_INFO_P(new_object, isReusedObj) = KAL_TRUE;
  EXT_OBJ_INFO_P(new_object, ExtObjRef)   = msg_ref; 

  ((EMSInternalData*)emsData->internal)->needReCompress  = KAL_TRUE; 
   }

   if(new_object != NULL)
   {
  AddObjectIntoListByOffset(emsData, new_object);
  emsData->CurrentPosition.Object = new_object;
   }

   return ret;
}

/*****************************************************************************
* FUNCTION
*  EMSGetDataForComp
* DESCRIPTION
*   This function is to check header of extended object IE.
*
* PARAMETERS
*  a  IN  emsData
* RETURNS
* EMSTATUS 
* GLOBALS AFFECTED
*  none
*****************************************************************************/
EMSTATUS EMSGetDataForComp(EMSData *emsData, kal_uint8 *buff, kal_uint16 *buff_len) 
{

   EMSTATUS   ret = EMS_OK;
   EMSObject  *obj;
   kal_uint8  iei;
   kal_uint16 obj_len, offset = 0;

   obj = emsData->listHead;

   while (obj != NULL)
   { 
  if( IS_TXT_FMT_OBJ (obj) != KAL_TRUE) 
  {
 if (EXT_OBJ_INFO_P(obj, isReusedObj) == KAL_TRUE) 
 {
if (buff !=NULL)
{
   *(buff + offset  ) = EMS_REUSED_EXT_OBJ_IEI ;
   *(buff + offset+1) = EXT_OBJ_INFO_P(obj, ExtObjRef);
   *(buff + offset+2) = (kal_uint8)((obj->OffsetToText/2) >>8) ;
   *(buff + offset+3) = (kal_uint8)((obj->OffsetToText/2) & 0x00ff) ;
}

offset += 4 ;
 }
 else
 {
ret = EMSGetExtObjLen(obj, &obj_len, &iei);
if (ret == EMS_OK )
{
   if (buff !=NULL)
  ret = EMSFillExtObjIE (obj, iei, (buff + offset + 1), obj_len - 7);

   if (ret == EMS_OK )
   {
  if (buff !=NULL)
 *(buff + offset ) = EMS_EXT_OBJ_IEI ;

  offset += (obj_len + 1) ;
   }
}
 }
  }

  obj= obj->next ;
   }

   if (buff_len != NULL)
  *buff_len = offset ;

   return ret;
}

EMSTATUS EMSGetExtObjLen(EMSObject *obj, kal_uint16 *length, kal_uint8 *iei_info)
{

   EMSTATUSret = EMS_OK;
   EMSObjData  *objData = obj->data; 
   kal_uint8   iei;
   kal_uint16  obj_len = 7; /* extended object header */

   switch(obj->Type)
   {
  case EMS_TYPE_PREDEF_SND: /* predefined sound */
 {
iei = EMS_EXT_PREDEF_SND ;
obj_len += 1;
 }
 break;

  case EMS_TYPE_PREDEF_ANM: /* predefined animation */
 {
iei = EMS_EXT_PREDEF_ANM ;
obj_len += 1;
 }
 break;

  case EMS_TYPE_USERDEF_SND:
 {
/* check whether this sound is valid or not */
iei = EMS_EXT_USER_DEF_SND ;
obj_len +=  objData->sound.pdu_length ;
 }
 break;   

  case EMS_TYPE_USERDEF_ANM:
 {
switch( objData->animation.bitsPerPixel ) 
{
   case 1:  /* B&W Animation */
  iei = EMS_EXT_BW_ANM ;
  break;
   case 2:  /* 2-bit greyscale Animation */
  iei = EMS_EXT_2BIT_ANM ;
  break;
   case 6:  /* 6-bit greyscale Animation */
  iei = EMS_EXT_6BIT_ANM ;
  break;

   default :
  EMS_EXT_ASSERT(KAL_FALSE, objData->animation.bitsPerPixel, 0, 0);
  ret = EMS_INVALID_OBJECT;
}

obj_len += ( 4 + objData->animation.pdu_length ) ;
 }
 break;			  
			  
  /*================================
   * Picture	 
   *================================*/			  
  case EMS_TYPE_PIC:
 {
switch( objData->picture.bitsPerPixel ) 
{
   case 1:  /* B&W Picture */
  iei = EMS_EXT_BW_PIC ;
  break;
   case 2:  /* 2-bit greyscale Picture */
  iei = EMS_EXT_2BIT_PIC ;
  break;
   case 6:  /* 6-bit greyscale Picture */
  iei = EMS_EXT_6BIT_PIC ;
  break;

   default :
  EMS_EXT_ASSERT(KAL_FALSE, objData->picture.bitsPerPixel, 0, 0);
  ret = EMS_INVALID_OBJECT;
}

obj_len += ( 2 + objData->picture.pdu_length ) ;

 }
 break;
  /*================================
   * VCard and VCalendar	 
   *================================*/			  
  case EMS_TYPE_VCARD:
 iei = EMS_EXT_VCARD ;
 obj_len +=  objData->vapp.pdu_length ;
 break;

  case EMS_TYPE_VCALENDAR:
 iei = EMS_EXT_VCALENDAR ;
 obj_len +=  objData->vapp.pdu_length ;
 break;
  default : 
 EMS_EXT_ASSERT(KAL_FALSE, obj->Type,0,0);
 ret = EMS_INVALID_OBJECT;
 break; 	  
   }/* switch */


   if (length != NULL)
  *length = obj_len ;

   if (iei_info != NULL)
  *iei_info = iei ;

   return ret;

}
/*****************************************************************************
* FUNCTION
*  EMSCompressedObjIE
* DESCRIPTION
*   This function is to check header of extended object IE.
*
* PARAMETERS
*  a  IN  emsData
* RETURNS
* EMSTATUS 
* GLOBALS AFFECTED
*  none
*****************************************************************************/
EMSTATUS EMSCompressedObjIE(EMSData *emsData, kal_bool needPack) 
{
   EMSTATUSret ;
   EMSInternalData*i = (EMSInternalData*)emsData->internal;
   kal_uint16  in_len, out_len ;
   kal_uint8   *in_buff ;

   /* detect data len */
   ret = EMSGetDataForComp(emsData, NULL, &in_len);
   if(ret != EMS_OK)
   {
  return ret;
   }

   if (in_len == 0)
   {
  i->forceCompress = KAL_FALSE;
  return ret;
   }

   in_buff  = (kal_uint8*) EMS_ASM_MALLOC(in_len);

   if (in_buff == NULL)
  return EMS_NO_MEMORY;

   ret = EMSGetDataForComp(emsData, in_buff, &in_len);
   if(ret != EMS_OK)
   {
  EMS_ASM_MFREE(in_buff);
  return ret;
   }

   if (KAL_FALSE ==  EMSCompression(KAL_FALSE, in_len,  in_buff , 
 &out_len, NULL, NULL))
   {
  /* query the output length */
  EMS_ASM_MFREE(in_buff);
  return EMS_UNSPECIFIED_ERROR ;
   }

   ret = EMSGetPBuff(&(i->PBuff), (out_len + 3) , EMS_PBUFF_COMP_OUT, needPack);

   if(needPack == KAL_TRUE)
   {
  /* need pack the data */
  if(ret == EMS_OK && i->PBuff.BufPtr != NULL)
  {
 if (KAL_TRUE ==  EMSCompression(KAL_TRUE, in_len,  in_buff , 
   &out_len, i->PBuff.BufPtr + 3, NULL))
 {
/* fill compression header */
*(i->PBuff.BufPtr) = 0;
*(i->PBuff.BufPtr + 1) = (kal_uint8)(out_len >> 8);
*(i->PBuff.BufPtr + 2) = (kal_uint8)(out_len & 0x00ff);
 }
 else
EMSFreePBuff(&(i->PBuff));
  }
   }

   EMS_ASM_MFREE(in_buff);
   i->CompressIELen = out_len + 3;

   return ret;
}

/*****************************************************************************
* FUNCTION
*  EMSDeCompressedObjIE
* DESCRIPTION
*   This function is to check header of extended object IE.
*
* PARAMETERS
*  a  IN  emsData
* RETURNS
* EMSTATUS 
* GLOBALS AFFECTED
*  none
*****************************************************************************/
EMSTATUS EMSDeCompressedObjIE(EMSData *emsData) 
{
   EMSInternalData*i = (EMSInternalData*) emsData->internal;
   EMSTATUSret = EMS_UNSPECIFIED_ERROR ;
   kal_uint16  out_len, in_len , read_byte , obj_len ;
   kal_uint8   *in_buff, *out_buff, iei;

   in_buff = i->PBuff.BufPtr;
   ASSERT ( in_buff != NULL);

   if( (*in_buff & 0x0f) != 0)
   {
  EMS_PRINT_STR("[ERROR] non-support compression method\n");
  return EMS_UNSPECIFIED_ERROR ;
   }

   in_len = (*(in_buff+1) << 8) + (*(in_buff+2)) ;

   if (i->PBuff.BufOffset != (in_len + 3))
   {
  EMS_PRINT_STR("[ERROR] Compression IE length error\n");
  return EMS_UNSPECIFIED_ERROR ;
   }

   if (KAL_FALSE ==  EMSDeCompression(KAL_FALSE, in_len,  (in_buff +3), 
 &out_len, NULL))
   {
  /* query the output length */
  return EMS_UNSPECIFIED_ERROR ;
   }

   out_buff  = (kal_uint8*) EMS_ASM_MALLOC(out_len);

   if (out_buff == NULL)
  return EMS_NO_MEMORY;

   if (KAL_TRUE ==  EMSDeCompression(KAL_TRUE, in_len,  (in_buff +3), 
 &out_len, out_buff))
   {
  read_byte = 0 ;

  while(read_byte < out_len)
  { 
 ret = EMS_OK ;

 /* Get IEI */
 iei = *(out_buff + read_byte);  
 read_byte ++;


 switch(iei)
 {
case EMS_EXT_OBJ_IEI :  /* Extended Object IEI */ 
   /* Get IE Length */
   obj_len = (*(out_buff + read_byte +1) << 8) + 
 (*(out_buff + read_byte +2))  + 7 /*header*/;

   if (read_byte + obj_len <= out_len)
  ret = EMSDecodeExtObjIE(emsData, obj_len, (out_buff + read_byte));

   break;

case EMS_REUSED_EXT_OBJ_IEI :/* Reused Extended Object IEI */ 

   obj_len = 3 ;
   if (read_byte + obj_len <= out_len)
  ret = EMSDecodeReusedExtObjIE (emsData, obj_len, (out_buff + read_byte));

   break;
default:
   /* unrecognized ie, ignore this ie */  
   break;
 } /* switch */

 if (ret == EMS_NO_MEMORY)
break;

 read_byte += obj_len;

  } /* while */
   }

  EMS_ASM_MFREE (out_buff); 

   return ret;
}

void EMSCheckExtTextOffset(EMSData *emsData)
{
   EMSObject   *obj;
   EMSInternalData*i = (EMSInternalData*)emsData->internal;

   obj = emsData->listHead;

   while (obj != NULL)
   {
  if ((EXT_OBJ_INFO_P(obj, isExtObj) == KAL_TRUE)  || 
  (EXT_OBJ_INFO_P(obj, isReusedObj) == KAL_TRUE))
  {
 if(obj->OffsetToText > i->baseTextOffset)
 {
obj->OffsetToText = i->baseTextOffset;
 }
  }
  obj= obj->next ;
   }

}

#endif

/*****************************************************************************
* FUNCTION
*  EMSDecodeUDHObj
* DESCRIPTION
*   This function is to decode EMS objects in UDH.
*
* PARAMETERS
*  a  OUT object,  new object from UDH Ie
*  b  IN  emsData
*  c  IN  iei
*  d  IN  ie_len
*  e  IN  data  , raw data of UDH IE
* RETURNS
* EMSTATUS 
* GLOBALS AFFECTED
*  none
*****************************************************************************/

EMSTATUS EMSDecodeUDHObj(EMSObject **object, EMSData *emsData, kal_uint8 iei, 
 kal_uint8 ie_len, kal_uint8 *data)
{
   EMSTATUS   ret = EMS_UNSPECIFIED_ERROR;
   EMSObject  *new_object = NULL;
   kal_uint8  obj_attr = EMS_ATTB_NORMAL ;
   EMSInternalData*i = (EMSInternalData*) emsData->internal;

   /* compression IE doesn't need object buffer  */
   /* Extended obj. allocate buffer in EMSDecodeExtObjIE() */
   if ( iei != EMS_COMPRESSED_IEI && iei != EMS_EXT_OBJ_IEI  &&
iei != EMS_REUSED_EXT_OBJ_IEI )
   {
  new_object = EMSObjectAllocate(IS_EMS_DATA_NEED_IEI(iei));   
  if(new_object == NULL)
 return EMS_NO_MEMORY;

#ifdef __EMS_REL5__
  if(i->ObjDistIndCount > 0  && new_object->data != NULL )
  {
 i->ObjDistIndCount --;
 obj_attr = EMS_ATTB_NOT_FW ;
  }
#endif
   }

   switch(iei)
   {
  case EMS_PREDEF_SND_IEI :   /* Predefined Sound */

 EMS_PRINT_STR("recv predef sound ie\n");

 if( *(data+1) == 0 ||
 *(data+1) <= EMS_MAX_SND_PREDEF_NO) 
 {
new_object->Type = EMS_TYPE_PREDEF_SND;
new_object->OffsetToText = *(data)*2 + i->baseTextOffset;
new_object->PredefNo = *(data + 1);

ASSERT(new_object->data == NULL);
ret = EMS_OK ;
 }

 break;

  case EMS_PREDEF_ANM_IEI :   /* Predefined Animation */

 EMS_PRINT_STR("recv predef animation ie\n");

 if( *(data+1) == 0 ||
 *(data+1) <= EMS_MAX_ANIM_PREDEF_NO) 
 {
new_object->Type = EMS_TYPE_PREDEF_ANM;
new_object->OffsetToText = *(data)*2 + i->baseTextOffset;
new_object->PredefNo = *(data + 1);

ASSERT(new_object->data == NULL); 
ret = EMS_OK ;
 }

 break;

  case EMS_USER_DEF_SND_IEI : /* User Defined Sound */

 EMS_PRINT_STR("recv user defined sound ie\n");

 if((ie_len > 1) && ((ie_len-1) <= EMS_MAX_IMELODY_SIZE))
 {
ASSERT(EMS_R(new_object->data) != NULL);   

new_object->Type = EMS_TYPE_USERDEF_SND;
new_object->OffsetToText = *(data)*2 + i->baseTextOffset;

ret = AddUsrDefSnd(new_object, obj_attr, (kal_uint16)(ie_len - 1 ), (data +1));
 }

 break;


  case EMS_LARGE_ANM_IEI :/* Large Animation */

 EMS_PRINT_STR("recv large animation ie\n");

 if(ie_len == 128+1)
 {
ASSERT(EMS_R(new_object->data) != NULL);

new_object->Type = EMS_TYPE_USERDEF_ANM;
new_object->OffsetToText = *(data)*2 + i->baseTextOffset;

ret = AddUsrDefAnim(new_object, obj_attr, 4, 0, 0, 16, 16, 1,
128, (data +1)) ;
 }

 break;

  case EMS_SMALL_ANM_IEI :/* Small Animation */

 EMS_PRINT_STR("recv small animation ie\n");

 if(ie_len == 32+1)
 {
ASSERT(EMS_R(new_object->data) != NULL);

new_object->Type = EMS_TYPE_USERDEF_ANM;
new_object->OffsetToText = *(data)*2 + i->baseTextOffset;

ret = AddUsrDefAnim(new_object, obj_attr, 4, 0, 0, 8, 8, 1,
 32, (data +1)) ;
 }

 break;

  case EMS_LARGE_PIC_IEI :/* Large Picture */

 EMS_PRINT_STR("recv large picture ie\n");

 if(ie_len == 128+1)
 {
ASSERT(EMS_R(new_object->data) != NULL);

new_object->Type = EMS_TYPE_PIC;
new_object->OffsetToText = *(data)*2 + i->baseTextOffset;

ret = AddPicture(new_object, obj_attr, 32, 32, 1, 128, (data +1));
 }

 break;

  case EMS_SMALL_PIC_IEI :/* Small Picture */

 EMS_PRINT_STR("recv small picture ie\n");

 if(ie_len == 32+1)
 {
ASSERT(EMS_R(new_object->data) != NULL);

new_object->Type = EMS_TYPE_PIC;
new_object->OffsetToText = *(data)*2 + i->baseTextOffset;

ret = AddPicture(new_object, obj_attr, 16, 16, 1, 32, (data +1));
 }

 break;

  case EMS_VAR_PIC_IEI :  /* Variable Picture */ 
 { 
kal_uint16 pdu_len;

EMS_PRINT_STR("recv variable picture ie\n");

ASSERT(EMS_R(new_object->data) != NULL);

new_object->Type = EMS_TYPE_PIC;
new_object->OffsetToText = *(data)*2 + i->baseTextOffset;

/* start pos, h dim, v dim, at least one octet pdu */
if(ie_len < 4) 
   break;
  
pdu_len = ((*(data+1)<<3) * /* horizontal no. of 8 pixels */
*(data+2) + 7) >> 3; /* vertical no. of pixels */
  
if(ie_len == pdu_len + 3)
{
   ret = AddPicture(new_object, obj_attr, *(data+2),
 (*(data+1)*8 ), 1, pdu_len, (data +3));
}

break;
 }
#ifdef __EMS_REL5__
  case EMS_EXT_OBJ_IEI :  /* Extended Object IEI */ 
  case EMS_COMPRESSED_IEI:  /* Compression Control IEI */ 
 { 
kal_bool  is_legal_IE = KAL_FALSE;
EMS_PRINT_STR("recv ext. or compression ie\n");

ret = EMS_OK;

if(i->PBuff.BufType != EMS_PBUFF_NONE)
{
   if((i->MissSegInCrossIE == KAL_FALSE) &&
  (i->PBuff.BufOffset + ie_len <= i->PBuff.BufLen))
   {
  /* save the data and check if the IE is finished. */
  EMS_ASSERT( i->PBuff.BufPtr != NULL );
  is_legal_IE = KAL_TRUE ;
   }
   else
   {
  /* (1) miss segment or (2).illegal length   */
  /* It shall ignor existed data in buffer, and re-detect */
  /* current IE.  */
  EMSFreePBuff(&(i->PBuff));
   }
}

if (is_legal_IE == KAL_FALSE)
{
   /* (1). no pending data in buffer */
   /* (2). miss segment or   */
   /* (3). illegal length*/

   /* it need to check if header is illegal. */

   /* total Extended Object or Compression Control data length, */
   /* included header (7 bytes and 3 bytes).*/
   kal_uint16  obj_length;

   EMS_ASSERT( i->PBuff.BufPtr == NULL );

   /* If there is missed seg. before extended object IE,  */
   /* it may not impact ext. IE.   */
   /* However, it may also lose the extended object header (7 bytes). */

   /* Suppose there is no impact first.  */
   /* If length doesn't match, ignor the IE finally. */
   i->MissSegInCrossIE = KAL_FALSE;

   ret = EMSObjHeaderCheck(emsData, iei, ie_len, data, &obj_length);
   if (ret == EMS_OK)
   {
#if 0
  /* Size control buffer pool is limited (2048 bytes). */
  /* If the supported segment number is very large, please consider the case  */
  /* that IE data size is larger than 2048 byte.   */
  /* When it is larger 2048 byte, the buffer shall use the specific static buffer. */
  EMS_ASSERT( obj_length <= 2048 );
#endif
  kal_uint8 type;
  if(iei == EMS_EXT_OBJ_IEI )
 type = EMS_PBUFF_MT_EXT_OBJ ;
  else
 type = EMS_PBUFF_DECOMP_IN;

  /* new extended object, allocate buffer */
  ret = EMSGetPBuff(&(i->PBuff), obj_length, type, KAL_TRUE);

  is_legal_IE = KAL_TRUE ;
   }
   else
   {
  is_legal_IE = KAL_FALSE ;
  EMS_PRINT_STR("[ERROR] ignor ext. or compression ie\n");
   }
}


if (is_legal_IE == KAL_TRUE &&
(i->PBuff.BufOffset + ie_len) <= i->PBuff.BufLen )
{
   /* copy data and check if a completed IE. */
   ems_mem_cpy(i->PBuff.BufPtr + i->PBuff.BufOffset , data, ie_len);	
   i->PBuff.BufOffset  += ie_len;

   if (i->PBuff.BufOffset == i->PBuff.BufLen)
   {
  if (iei == EMS_EXT_OBJ_IEI )
 ret = EMSDecodeExtObjIE(emsData, i->PBuff.BufOffset , i->PBuff.BufPtr);
  else
 ret = EMSDeCompressedObjIE(emsData); 

  EMSFreePBuff(&(i->PBuff));

  i->MissSegInCrossIE = KAL_FALSE;
   }
}
else
{
   if(i->PBuff.BufType != EMS_PBUFF_NONE)
   {
  /* abnormal length  or */
  /* there is extended object data in missed segment. */
  /* ignor this IE. Free buffer and reset variable. */
  EMSFreePBuff(&(i->PBuff));
  i->MissSegInCrossIE = KAL_FALSE;

  ret = EMS_INVALID_EMS50_IE;
   }
}
break;
 }
  case EMS_REUSED_EXT_OBJ_IEI: /* Reused Extended Object IEI */ 
 {
EMS_PRINT_STR("recv reused ext. ie\n");
ret = EMSDecodeReusedExtObjIE (emsData, ie_len, data);
break;
 }
#endif
  default:
 /* unrecognized ie, ignore this ie */  
 break;
   }

   if ( ret != EMS_OK)
   {
  if(new_object != NULL)
 ReleaseEMSObject(new_object);
   }
   else
  *object = new_object ;

   return ret;
}
#endif


/*****************************************************************************
* FUNCTION
*  UpdateEMSObjOffset
* DESCRIPTION
*   This function updates the Object's Text Offset.
*
* PARAMETERS
*  a  IN/OUT  emsData
*  b  IN  isDel
*  c  IN  numOfOctet
* RETURNS
*  none
* GLOBALS AFFECTED
*  none
*****************************************************************************/
void UpdateEMSObjOffset(EMSData  *emsData,
kal_bool isDel, 
kal_uint16  numOfOctet)
{
   EMSObject *obj = emsData->CurrentPosition.Object;
   
   if(obj == NULL) 
  obj = emsData->listHead;
   else
  obj = obj->next;

   while(obj != NULL)
   { 
  /* if obj is current text format we don't update the offset of that obj */
  if((obj->OffsetToText >= emsData->CurrentPosition.OffsetToText) &&
 ((kal_uint32*)obj != (kal_uint32*)emsData->CurrentTextFormatObj))
  {
 if(isDel == KAL_TRUE)
obj->OffsetToText -= numOfOctet;
 else
obj->OffsetToText += numOfOctet; 

#ifdef __EMS_REL5__
 ((EMSInternalData*)emsData->internal)->needReCompress  = KAL_TRUE; 
#endif
  }
  obj = obj->next;
   }
} /* end of UpdateEMSObjOffset */

/*****************************************************************************
* FUNCTION
*  getObjLength
* DESCRIPTION
*   This function gets the required space for a object.
*
* PARAMETERS
*  a  IN  obj
*  b  IN  text_len
*  d  IN/OUT  isAllow, check wether this object exceeds the max size.
* RETURNS
*  length
* GLOBALS AFFECTED
*  none
*****************************************************************************/
kal_uint16 getObjLength(EMSTYPE type, EMSObjData *objData, kal_uint16 text_len, kal_bool *isAllow)
{
   kal_uint16 len=0; /* reset */
   kal_uint16 raw_data_len = 0; /*reset*/

   switch(type)
   {		
  case EMS_TYPE_PREDEF_SND: /*the same for BOTH */
  case EMS_TYPE_PREDEF_ANM:			  

 len = (EMS_PREDEF_IEDL+2);
 break;		  
				  
  case EMS_TYPE_PIC:
		
   ASSERT(objData != NULL);  

   raw_data_len = objData->picture.pdu_length;
   len = objData->picture.pdu_length+3; 	   	   

   if(( objData->picture.pdu_length != 32) &&
  ( objData->picture.pdu_length != 128) &&
  ( objData->picture.bitsPerPixel==1))
   {				 
  len+=2;/* variable */		   		  
   }   	  
  	  break;
			 
  case EMS_TYPE_USERDEF_ANM:	
   ASSERT( objData != NULL);
   len = (3+ objData->animation.pdu_length);
   raw_data_len =  objData->animation.pdu_length;
   break;	
  				
  case EMS_TYPE_USERDEF_SND:
   ASSERT( objData!= NULL);
   len = ( objData->sound.pdu_length+3);
   raw_data_len =  objData->sound.pdu_length;
   break;   
  	  
  case EMS_TYPE_TEXT_FORMAT:
   {
  kal_uint8 fmt_color_len = 0;

  if ( objData->text_format.fgColor != EMS_BLACK ||
   objData->text_format.bgColor != EMS_WHITE)
 fmt_color_len = 1;

  len = EMS_TXTFMT_IEDL+ fmt_color_len + 2 + text_len;
   break;   
   }
  	  
  case EMS_TYPE_OBJ_DIST_IND :
   len = (EMS_PREDEF_IEDL+2);
   break;
  default : 
   break;
  	  
   } /* switch */

#if 0
   /* If attribute is not forwarded, object distribution indicator shall be added. */
   if(type == EMS_TYPE_PIC || type == EMS_TYPE_USERDEF_SND || type == EMS_TYPE_USERDEF_ANM) 
   {
  if(objData->common.attribute == EMS_ATTB_NOT_FW)
 len += 4;
   }
#endif

   if(isAllow) 
   {
#ifndef __EMS_REL5__
  if(raw_data_len > EMS_MAX_IMELODY_SIZE)
 *isAllow = KAL_FALSE;
  else
#endif
 *isAllow = KAL_TRUE;
   }

   return len;
} /* end of getObjLength */


   
