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
* ems_segment_proc.c
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


#define EMS_SEGMENT_PROC_C

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

/*****************************************************************************
* FUNCTION
*  EMSSetUDHI
* DESCRIPTION
*   This function check whether concatenated message is needed or
*   whether message is EMS.
*
* PARAMETERS
*  a  IN/OUT  emsData
*
* RETURNS
*  none
* GLOBALS AFFECTED
*  none
*****************************************************************************/
void EMSSetUDHI(EMSData *emsData)
{	
   kal_uint16 totalOctet = 0; /* reset */
   kal_uint16 totalChar = 0; /* reset */
   EMSInternalData *internal = (EMSInternalData*)emsData->internal;

   totalOctet = internal->totalTxtFmtOctet;
   totalOctet += internal->totalObjOctet;

#ifdef __EMS_REL5__
   if(EMS_CNTX(isCompressSupp) == KAL_TRUE)
   {
  if (internal->totalExtObjOctet > 0  || internal->forceCompress == KAL_TRUE)
  {
 /* All non-extended objects are compressed. */
 /* Subtract the octets of them. */
 totalOctet -= internal->totalObjOctet;

 totalOctet += (internal->CompressIELen + 2);
  }
   }
   else
  totalOctet += internal->totalExtObjOctet ;
#endif

   /* add port number UDH length */
   if ( emsData->PortNum.isPortNumSet == KAL_TRUE)
   {
  totalOctet += EMS_PORT_HDR_LEN_OCTET;
   }
   
   if(totalOctet > 0) /* all UDH length */
   {
  totalChar = GetNumOfChar(emsData->dcs,
(kal_uint16)(totalOctet+1) /* include UDHL */
   ); 
   }   
   
   if(emsData->dcs != SMSAL_UCS2_DCS)
  totalChar += (emsData->textLength/2);
   else
  totalChar += emsData->textLength;

   if(totalChar > internal->MaxLenPerSeg)
   {
  emsData->isConcat = KAL_TRUE;   	
   }  
   else
   {
  emsData->isConcat = KAL_FALSE;  
   }

   if((emsData->isConcat == KAL_TRUE) || 
	   (emsData->PortNum.isPortNumSet == KAL_TRUE) ||
  (internal->numOfEMSObject > 0)
#ifdef __EMS_REL5__
  ||(internal->numOfExtObject> 0)
#endif
   )
   {
  emsData->udhi = KAL_TRUE;   	
   }
   else
   {
  emsData->udhi = KAL_FALSE;
   }

} /* end of EMSSetUDHI */

kal_bool EMSChangeSegment(EMSData *ems, EMSInternalData *i)
{
   if((i->numSegment+1)>=EMS_MAX_SEG_NUM )
   {
  i->error = EMS_NO_SPACE;/* run out of space */
  return KAL_FALSE;
   }

   i->numSegment++; 	
   i->baseTextOffset = i->textOffset;

   if(ems->udhi == KAL_TRUE)
   {
  i->UDHL[i->numSegment]+= EMS_UDHL_HDR_LEN_OCTET;
  i->TPUDLen[i->numSegment] += EMS_UDHL_HDR_LEN_OCTET;  
   }

#ifdef __EMS_REL5__
   {
  i->UDHL[i->numSegment]+= EMS_16BIT_CONC_HDR_LEN_OCTET;
  i->TPUDLen[i->numSegment] += EMS_16BIT_CONC_HDR_LEN_OCTET;  
   }
#else
   {
  i->UDHL[i->numSegment]+= EMS_8BIT_CONC_HDR_LEN_OCTET;
  i->TPUDLen[i->numSegment] += EMS_8BIT_CONC_HDR_LEN_OCTET;  
   }
#endif
  
   /* i->UDHL[i->numSegment] = EMS_CONC_HDR_LEN;  */
   /* i->TPUDLen[i->numSegment] = EMS_CONC_HDR_LEN;   */

   /* 16-bit App Port address IEI , 6 bytes */
   if(ems->PortNum.isPortNumSet == KAL_TRUE)
   {
  i->UDHL[i->numSegment]+= EMS_PORT_HDR_LEN_OCTET;
  i->TPUDLen[i->numSegment] += EMS_PORT_HDR_LEN_OCTET;  
   }

   return KAL_TRUE;
}

void EMSFlushText(EMSData *ems, 
  EMSInternalData *i,
  kal_bool	needPack,
  kal_uint16 *udhLength, 
  kal_uint16 *txt_len,
  kal_uint16 *txt_len_in_char)
{

   /* *udhLength:  input : UDH lengeh, (in unit CHAR)*/
   /*  output: UDH length + text length  */
   /* *txt_len,input : text lengeh need to be flush ,*/
   /*  output: remain text length (not flushed)  */

   kal_uint8  *udBuffer;
   kal_uint16 txt_to_write;
  
   /* flush TEXT into tail of TP-UD , may use several segments
* if the TEXT is long */
   txt_to_write = *txt_len; /* in Octets */

   EMS_DBG_(
			if(ems->dcs == SMSAL_UCS2_DCS) 
			ASSERT((txt_to_write % 2) == 0);
   )

   if(*txt_len_in_char > (i->MaxLenPerSeg - *udhLength))
   {
  *txt_len_in_char = i->MaxLenPerSeg - *udhLength; 

  /* if UCS2 is adopted, 
   * we shall ensure a UCS2 (2-byte) character can not carray together */   
  if((ems->dcs == SMSAL_UCS2_DCS) && ((*txt_len_in_char % 2) != 0))
 (*txt_len_in_char)--;

  txt_to_write = GetTxtLenInOctet(ems->dcs, *txt_len_in_char);
   }

   if((needPack == KAL_TRUE) && (i->TPUD[i->numSegment] != NULL))
   {
  udBuffer = i->TPUD[i->numSegment];  

  ASSERT ( udBuffer != NULL ) ;

  EMSTextCpy(udBuffer+i->TPUDLen[i->numSegment], /* dest */
 (kal_uint8*)(ems->textBuffer+i->textOffset),/* src */
 *txt_len_in_char,
 ems->dcs,
 KAL_TRUE); 
   }
   
   i->TPUDLen[i->numSegment] += *txt_len_in_char; 
   i->textOffset += txt_to_write; /* in Octets */
   *udhLength += *txt_len_in_char;
   *txt_len -= txt_to_write;
 
} /* end of EMSFlushText */

void EMSFlushAll(EMSData	   *ems,
 kal_uint16	newLength,
 kal_uint16	textEndOffset,					 
 kal_bool  needPack)
{  
   EMSInternalData *i = (EMSInternalData*)ems->internal;
   kal_uint16 udhLength; /* length of UDH */   
   
   kal_uint16 txt_len;
   kal_uint16 txt_len_in_char;   
  
   udhLength = GetNumOfChar(ems->dcs, i->TPUDLen[i->numSegment]);
   
   ASSERT(textEndOffset >= i->textOffset);

   txt_len = textEndOffset - i->textOffset;
   txt_len_in_char = GetTxtLenInChar(ems->dcs, txt_len);

   
   EMS_DBG_(
   if(ems->dcs == SMSAL_UCS2_DCS) 
  ASSERT((txt_len % 2) == 0);
   )  

   while(txt_len > 0)
   {
  EMSFlushText(ems, i, needPack, &udhLength, &txt_len, &txt_len_in_char);  
  
  //if(is_TPUD_avail(i, udhLength, txt_len_in_char, newLength) == KAL_FALSE)

  /* now, txt_len   stores "remaining octets" wait to write */
  /* now, udhLength stores "total len" in this segment */
  /* suppose udhLength + txt_len would exceed segment length. */
  if(is_TPUD_avail(i, udhLength, (kal_uint16)GetTxtLenInChar(ems->dcs, txt_len), newLength) == KAL_FALSE)
  {
 if(EMSChangeSegment(ems, i)==KAL_FALSE)
return;
  }  

  udhLength = GetNumOfChar(ems->dcs, i->TPUDLen[i->numSegment]);
  txt_len = textEndOffset - i->textOffset;
  txt_len_in_char = GetTxtLenInChar(ems->dcs, txt_len);
  
   } /* while */ 
   
} /* end of EMSFlushAll */ 

#ifndef __SLIM_EMS__
void EMSFlush(EMSData	   *ems,
  EMSObject*object, 
  kal_bool needPack)
{  
   EMSInternalData *i = (EMSInternalData*)ems->internal;
   kal_uint16  udhLength; /* length of UDH */
   kal_uint16  new_udhLength;   
   kal_uint16  txt_len_in_char;
   kal_uint16  txt_len;  
   kal_uint16  newLength;
   kal_uint16  dummy_len, textEndOffset ;


   if(object->Type == EMS_TYPE_TEXT_FORMAT)
   {
   /* text format shall be associated with one character at least */
   if(ems->dcs == SMSAL_UCS2_DCS)
   dummy_len=2; /* in octets */
   else
   dummy_len=1;
   }
   else
  dummy_len=0;

   /* if the current segment cannot contain the object,*/
   /* flush text into current segment, insert this object to next segment  */
#ifdef __EMS_REL5__
   if( EXT_OBJ_INFO_P(object, isExtObj) == KAL_TRUE )
   {
  newLength = 2 + 7 ; /* IE +IEL (2), ext. header(7) */

  /* compose Ext. object data to internal->PBuff.BufPtr */
  /* only update IE length to internal->PBuff.BufLen if no pack */
  if (EMS_OK != EMSEncodeExtObjIE(ems, object, needPack))
 return; /* ignor the object */
   }
   else if ( EXT_OBJ_INFO_P(object, isReusedObj) == KAL_TRUE)
  newLength = 5; /* IE +IEL (2), ref. no (1), position (2) */
   else
#endif
  newLength = (kal_uint16)getObjLength((EMSTYPE)object->Type, EMS_R(object->data), dummy_len,NULL); 

   textEndOffset = object->OffsetToText ;
   
   /* before flush all text, i->TPUDLen[seg] == i->UDHL[seg]. */
   udhLength = (kal_uint8)GetNumOfChar(ems->dcs, i->TPUDLen[i->numSegment]);

   ASSERT(textEndOffset >= i->textOffset);

   /* get text length before this object in current segment. */
   txt_len = textEndOffset - i->textOffset;
   txt_len_in_char = GetTxtLenInChar(ems->dcs, txt_len);

   
   EMS_DBG_(
   if(ems->dcs == SMSAL_UCS2_DCS) 
  ASSERT((txt_len % 2) == 0);
   )  
  
   /* get new UDHLength = old UDHLength + new object length. */
   new_udhLength = GetNumOfChar(ems->dcs, (kal_uint16)(i->TPUDLen[i->numSegment] + newLength));
   
   /* check if (new_udhLength + text_len before this obj) exceed segment. */

   /* if len exceeds seg length, Flush the text before this obj and  */
   /* check in next segment. */
   while(is_TPUD_avail(i, new_udhLength, txt_len_in_char, 0) == KAL_FALSE)
   {  
  if(txt_len > 0)
  {
 EMSFlushText(ems, i, needPack, 
  &udhLength, 
  &txt_len, 
  &txt_len_in_char);
  } 

  /* Change to "new segment" */
  if(EMSChangeSegment(ems, i) == KAL_FALSE)
 return;

  /* get new UDHL and new text length before this obj in "new segment". */
  txt_len = textEndOffset - i->textOffset;
  txt_len_in_char = GetTxtLenInChar(ems->dcs, txt_len);
  udhLength = GetNumOfChar(ems->dcs, i->TPUDLen[i->numSegment]);
  new_udhLength = GetNumOfChar(ems->dcs, (kal_uint16)(i->TPUDLen[i->numSegment] + newLength)) ;   

  /* then check segment length again. */
   } /* while */

   /* pack the EMS object */
   if(i->error == EMS_OK)
   {
#ifdef __EMS_REL5__
  if( EXT_OBJ_INFO_P(object, isExtObj) == KAL_TRUE )
  {
 EMSPackExtObj(ems, object, needPack); 
  }
  else if ( EXT_OBJ_INFO_P(object, isReusedObj) == KAL_TRUE)
  {
 EMSPackReusedObj(ems, object, needPack); 
  }
  else if (object->Type == EMS_TYPE_OBJ_DIST_IND )
  {
 EMSPackObjDistInd (ems, object, needPack); 
  }
  else
#endif
  {
 EMS_ASSERT(object->Type < NUM_OF_EMS_SUPPORTED_OBJ );
 EMS_OBJ_PACK[object->Type] (ems, object, needPack); 
  }
   }
 
} /* end of EMSFlush */ 
#endif

#ifdef __EMS_REL5__
void EMSFlushCompressData(EMSData	   *ems,
  kal_bool needPack)
{
   EMSInternalData  *i = (EMSInternalData*)ems->internal;
   kal_uint8 *data;
   kal_uint8 *offset;
   kal_uint16txt_len_in_char, txt_len; 
   kal_uint16packed_obj_len, remain_udh_len;  
   kal_uint16udhLength; /* length of UDH */
   kal_uint16new_udhLength;   
   kal_uint16textEndOffset ;
   kal_uint16newLength ;

   if (i->PBuff.BufLen == 0)
  return;

   /* flush text into current segment, insert this object to next segment  */
   newLength = 2 + 3 ; /* IE +IEL (2), compressed. header(7) */

   textEndOffset = ems->textLength;
   
   /* before flush all text, i->TPUDLen[seg] == i->UDHL[seg]. */
   udhLength = (kal_uint8)GetNumOfChar(ems->dcs, i->TPUDLen[i->numSegment]);

   ASSERT(textEndOffset >= i->textOffset);

   /* get text length before this object in current segment. */
   txt_len = textEndOffset - i->textOffset;
   txt_len_in_char = GetTxtLenInChar(ems->dcs, txt_len);

   
   /* get new UDHLength = old UDHLength + new object length. */
   new_udhLength = GetNumOfChar(ems->dcs, (kal_uint16)(i->TPUDLen[i->numSegment] + newLength));
   
   /* check if (new_udhLength + text_len before this obj) exceed segment. */
   /* if len exceeds seg length, Flush the text before this obj and  */
   /* check in next segment. */
   while(is_TPUD_avail(i, new_udhLength, txt_len_in_char, 0) == KAL_FALSE)
   {  
  if(txt_len > 0)
  {
 EMSFlushText(ems, i, needPack, 
  &udhLength, 
  &txt_len, 
  &txt_len_in_char);
  } 

  /* Change to "new segment" */
  if(EMSChangeSegment(ems, i) == KAL_FALSE)
 return;

  /* get new UDHL and new text length before this obj in "new segment". */
  txt_len = textEndOffset - i->textOffset;
  txt_len_in_char = GetTxtLenInChar(ems->dcs, txt_len);
  udhLength = GetNumOfChar(ems->dcs, i->TPUDLen[i->numSegment]);
  new_udhLength = GetNumOfChar(ems->dcs, (kal_uint16)(i->TPUDLen[i->numSegment] + newLength)) ;   

  /* then check segment length again. */
   } /* while */


   offset = &i->TPUDLen[i->numSegment];
   
   do 
   {
  remain_udh_len = EMSGetExtObjRemainingSpace(ems, i, 
  *offset , /* original UDHL */
  txt_len_in_char);

  EMS_ASSERT (i->PBuff.BufType == EMS_PBUFF_COMP_OUT);

  if ((remain_udh_len - 2) < (i->PBuff.BufLen - i->PBuff.BufOffset))
 packed_obj_len = remain_udh_len - 2 ;
  else
 packed_obj_len = i->PBuff.BufLen - i->PBuff.BufOffset;

  if((needPack == KAL_TRUE) && (i->TPUD[i->numSegment] != NULL))
  {
 data = i->TPUD[i->numSegment];

 ASSERT(data != NULL);
 ASSERT (i->PBuff.BufPtr != NULL);

 *(data+*offset)   = EMS_COMPRESSED_IEI ;
 *(data+*offset+1) = packed_obj_len; /* IEDL */
 ems_mem_cpy(data+*offset + 2, i->PBuff.BufPtr + i->PBuff.BufOffset, packed_obj_len);
  }

  *offset += (packed_obj_len + 2); 
  i->UDHL[i->numSegment] += (packed_obj_len + 2);

  i->PBuff.BufOffset += packed_obj_len;

  udhLength = (kal_uint8)GetNumOfChar(ems->dcs, i->TPUDLen[i->numSegment]);

  if (i->PBuff.BufOffset == i->PBuff.BufLen)
  {
 /* all extened object data is packed */
 EMSFreePBuff(&(i->PBuff));
 break;
  }

  if (txt_len > 0 )
  {
 EMSFlushText(ems, i, needPack, 
  &udhLength, 
  &txt_len, 
  &txt_len_in_char);

 /* suppose all text shall be writen. */
 EMS_ASSERT(txt_len == 0);
  }

  /* Change to next segment */
  if(EMSChangeSegment(ems, i) == KAL_FALSE)
  {
 EMSFreePBuff(&(i->PBuff));
 return ;
  }

  /* get new UDHL and new text length before this obj in "new segment". */
  /* text length shall be 0 */
  txt_len = textEndOffset - i->textOffset;
  txt_len_in_char = GetTxtLenInChar(ems->dcs, txt_len);
  offset = &i->TPUDLen[i->numSegment];

   } while (1) ;
   

   ASSERT (i->PBuff.BufPtr == NULL);
   return ;

}
#endif

/*****************************************************************************
* FUNCTION
*  EMSInternalPack
* DESCRIPTION
*   This function pack (or virtual pack for calculating) EMS.
*
* PARAMETERS
*  a  IN/OUT  emsData
*  b  IN  TPUD
*  c  IN  needPack
* RETURNS
*  EMSTATUS
* GLOBALS AFFECTED
*  none
*****************************************************************************/
EMSTATUS EMSInternalPack(EMSData*emsData, 
 kal_uint8  *TPUD[], 
 kal_bool	needPack)
{   
   EMSInternalData *internal;
   EMSObject *object; 
#ifndef __SLIM_EMS__
   EMSObject *obj_next;
#endif
   
   if(emsData == NULL) /* sanity check */ 
  return EMS_NULL_POINTER;
	   
   internal = (EMSInternalData*) emsData->internal;

   ASSERT(internal != NULL);

   InitializeEMSInternalData(internal, TPUD);


#ifndef __SLIM_EMS__
#ifdef __EMS_REL5__
   if(EMS_CNTX(isCompressSupp) == KAL_TRUE)
   {
  if(internal->needReCompress == KAL_TRUE || needPack == KAL_TRUE)
  {
 /* reset compressed IE length  */
 internal->CompressIELen = 0 ;

 /* compress or update data length of compressed IE. */
 if (internal->totalExtObjOctet > 0  ||
 internal->forceCompress == KAL_TRUE)
 {
EMSTATUS ret;
ret = EMSCompressedObjIE(emsData, needPack);
if (ret != EMS_OK)
   return ret;
 }
 else
EMSFreePBuff(&(internal->PBuff));

 EMSSetUDHI(emsData);
 internal->needReCompress = KAL_FALSE ;
  }
  else
  {
 /* if not re-compress, it needs to fill the length info to PBuff */
 if (internal->CompressIELen  > 0)
EMSGetPBuff(&(internal->PBuff), internal->CompressIELen , EMS_PBUFF_COMP_OUT, KAL_FALSE);
  }
   }   
#endif 
#endif 


   /* 1 byte UDHL indicator */
   if(emsData->udhi == KAL_TRUE)
   {
  internal->UDHL[0] += EMS_UDHL_HDR_LEN_OCTET ;
  internal->TPUDLen[0] += EMS_UDHL_HDR_LEN_OCTET ;

  /* concatenated IEI length */
  if(emsData->isConcat == KAL_TRUE)
  {
#ifdef __EMS_REL5__
 {
internal->UDHL[0]+= EMS_16BIT_CONC_HDR_LEN_OCTET;
internal->TPUDLen[0] += EMS_16BIT_CONC_HDR_LEN_OCTET;  
 }
#else
 {
internal->UDHL[0]+= EMS_8BIT_CONC_HDR_LEN_OCTET;
internal->TPUDLen[0] += EMS_8BIT_CONC_HDR_LEN_OCTET;  
 }
#endif
  }

  /* 16-bit App Port address IEI , 6 bytes */
  if(emsData->PortNum.isPortNumSet == KAL_TRUE)
  {
 internal->UDHL[0]+= EMS_PORT_HDR_LEN_OCTET;
 internal->TPUDLen[0] += EMS_PORT_HDR_LEN_OCTET;  
  }
   }

   object = emsData->listHead; /* point to 1st object */
   
   internal->error = EMS_OK; /* reset */

   /* 
* Leave this while-loop, if all EMS objects and text are processed or error occurred
*/
   while((internal->error == EMS_OK)&&
 ((object != NULL) || (internal->textOffset < emsData->textLength)
#ifndef __SLIM_EMS__
#ifdef __EMS_REL5__
  || (internal->PBuff.BufType == EMS_PBUFF_COMP_OUT)
#endif
#endif
  ))
   {  

  if(object == NULL)
  {
#ifndef __SLIM_EMS__
#ifdef __EMS_REL5__
 if((EMS_CNTX(isCompressSupp) == KAL_TRUE) &&
(internal->PBuff.BufType == EMS_PBUFF_COMP_OUT))
 {
	 EMSFlushCompressData(emsData, needPack);
	 continue; /* go to beginning of while-loop */
 }
 else
#endif
#endif
 {
/* all objects are processed, write all remaining texts */
	 EMSFlushAll(emsData, 0,  emsData->textLength, needPack);
	 continue; /* go to beginning of while-loop */
 }
  }
		   	
#ifdef __SLIM_EMS__
  EMS_ASSERT(KAL_FALSE);
#else
  /* because if this is a NOT FINISHED object, this object will be free */
  /* in corresponding pack function, we need to keep the pointer of next object */
  /* in advance */

#ifdef __EMS_REL5__
  /* If compression is supported, ignore all object, except txt fmt. */
  /* All objects will be packed as ext. objects, and compressed. */
  if(EMS_CNTX(isCompressSupp) == KAL_TRUE) 
  {
 if ((internal->totalExtObjOctet > 0 || internal->forceCompress == KAL_TRUE ) &&
 (IS_TXT_FMT_OBJ (object) != KAL_TRUE) )
 {
object = object->next;
if (internal->notFinishedObject !=NULL)
{
   obj_next = object;
   goto ProcessNotFinishObj;
}
else
   continue;
 }
  }

  EMSNeedObjDistInd(emsData, &object);
#endif

  obj_next = object->next;

  /*======================
   * process EMS object 
   *======================*/

  EMSFlush(emsData, object,needPack);
  
#ifdef __EMS_REL5__
   if (object->Type == EMS_TYPE_OBJ_DIST_IND )
  ReleaseEMSObject(object); 

#endif
  object = obj_next;

ProcessNotFinishObj:
  if(internal->notFinishedObjectProcessed == KAL_TRUE)
  {
 /* more than one NotFinish objects */
 /* EMSObject *obj = ((EMSObject*)internal->notFinishedObject)->next; */
 ReleaseEMSObject(internal->notFinishedObject);
 internal->notFinishedObject = NULL;
 internal->notFinishedObjectProcessed = KAL_FALSE;
 /* internal->notFinishedObject = obj;  */
  }
  else if (internal->notFinishedObject !=NULL)
  {
 if( IS_TXT_FMT_OBJ(((EMSObject *)internal->notFinishedObject)) != KAL_TRUE)
ASSERT(KAL_FALSE);

 if(object !=NULL)
 {
if(object->OffsetToText > ((EMSObject*)internal->notFinishedObject)->OffsetToText )
{
   object = (EMSObject*)internal->notFinishedObject ;
   object->next = obj_next ;
   internal->notFinishedObjectProcessed = KAL_TRUE;
}
 }
 else
 {
object = (EMSObject*)internal->notFinishedObject ;
object->next = obj_next ;
internal->notFinishedObjectProcessed = KAL_TRUE;
 }

  }
#endif
  
   } /* while */
  
#ifndef __SLIM_EMS__
   EMS_RELEASE_NOT_FINISH_OBJ(internal);   
#ifdef __EMS_REL5__
   /* PBuff may not be free or reset when flush error occurs. */
   EMSFreePBuff(&(internal->PBuff));
#endif
#endif
  
   return (EMSTATUS)internal->error;
   
} /* end of EMSInternalPack */
   
/*****************************************************************************
* FUNCTION
*  MakeConcatHdr
* DESCRIPTION
*   This function makes concatenated IE.
*
* PARAMETERS
*  a  IN/OUT  data
*  b  IN  msg_ref, concatenated message reference
*  c  IN  totalSeg, total segment
*  d  IN  seg, segment no
* RETURNS
*  EMSTATUS
* GLOBALS AFFECTED
*  none
*****************************************************************************/
void MakeConcatHdr(kal_uint8 *data, kal_uint8 conc_iei, kal_uint16 msg_ref, 
   kal_uint8 totalSeg, kal_uint8 seg)
{
   *data = conc_iei;
   if (conc_iei == EMS_CONC8_MSG_IEI )
   {
  *(data+1) = 3;
  *(data+2) = (kal_uint8)msg_ref;
  *(data+3) = totalSeg;
  *(data+4) = seg;
   }
   else
   {
  *(data+1) = 4;
  *(data+2) = (kal_uint8)(msg_ref >> 8);
  *(data+3) = (kal_uint8)(msg_ref & 0x00ff);
  *(data+4) = totalSeg;
  *(data+5) = seg;
   }
}

void MakePortHdr(kal_uint8 *data, kal_uint16 src_port, kal_uint16 dst_port)
{
   *data = EMS_APP_PORT16_IEI;
   *(data+1) = 4; 
   *(data+2) = (kal_uint8)((dst_port & 0xff00) >> 8);
   *(data+3) = (kal_uint8)(dst_port & 0x00ff) ;
   *(data+4) = (kal_uint8)((src_port & 0xff00) >> 8);
   *(data+5) = (kal_uint8)(src_port & 0x00ff) ;
}


/*****************************************************************************
* FUNCTION
*  updateEMSReminder
* DESCRIPTION
*   This function updates the EMS Reminder Info.
*
* PARAMETERS
*  a  IN/OUT  emsData
*
* RETURNS
*  none
* GLOBALS AFFECTED
*  none
*****************************************************************************/
void updateEMSReminder(EMSData *emsData)
{
   EMSInternalData *internal = (EMSInternalData*)emsData->internal;   
   kal_uint8 numSegment = internal->numSegment;
   kal_uint8 numOfChar;

   /* if DCS is GSM 7-bit, we shall convert length of TP-UDH from
* octet to septet */
   if(emsData->dcs == SMSAL_DEFAULT_DCS)
   {  
  numOfChar = internal->TPUDLen[numSegment]-internal->UDHL[numSegment];/* text part */
  numOfChar += EMS_compute_udh_offset(internal->UDHL[numSegment])/7;/* UDH part */
   }
   else
   {
  numOfChar = internal->TPUDLen[numSegment];
   }

   /* handle some special cases */   
   if(numSegment > 0)
   {
  /* 2, 3, ... segment */
  if(numOfChar == internal->ConcatHdrLen)
  {
 /* actually, the no. of required segment is less by one segment */
 numSegment--; 
 
 if(numSegment == 0)
numOfChar = internal->TPUDLen[0]; 
 else
numOfChar = internal->MaxLenPerSeg;
  }
   }

   if((emsData->Reminder.requiredSegment-1) != numSegment)
  emsData->Reminder.isSegChange = KAL_TRUE;
	
   emsData->Reminder.requiredSegment = numSegment + 1;	

   emsData->Reminder.segUsableOctet = (emsData->isConcat ? internal->MaxUsableLen : 
   internal->MaxLenPerSeg);

   /* 16-bit App Port address IEI , 6 bytes */
   /* only decrease port len in fisrt seg*/
   /* for later segs, the port IEI is included in internal->MaxUsableLen */
   /* +1 for UDHL (user data header length */
   if((emsData->isConcat == KAL_FALSE ) && (emsData->PortNum.isPortNumSet == KAL_TRUE))
   {
  if(emsData->dcs == SMSAL_DEFAULT_DCS)
 emsData->Reminder.segUsableOctet -= (EMS_compute_udh_offset(EMS_PORT_HDR_LEN_OCTET+1)/7);
  else
 emsData->Reminder.segUsableOctet -= (EMS_PORT_HDR_LEN_OCTET+ 1) ;
   }

   emsData->Reminder.segRemainingOctet = internal->MaxLenPerSeg - numOfChar;
  
   emsData->Reminder.totalRemainingOctet = 
  internal->MaxUsableLen * (EMS_MAX_SEG_NUM - emsData->Reminder.requiredSegment) + 
  emsData->Reminder.segRemainingOctet;

   if(emsData->Reminder.requiredSegment == 1)
  emsData->Reminder.totalRemainingOctet -= internal->ConcatHdrLen;

} /* end of updateEMSReminder */

EMSTATUS EMSCalculateLength(EMSData *emsData)
{  
   EMSTATUS ret;

   /*==============================
* Virtual Pack 
*==============================*/
   ret = EMSInternalPack(emsData,   
   NULL,
   KAL_FALSE /* no pack */
  );
	
   /*==============================
* update the EMS Reminder 
*==============================*/
   if(ret == EMS_OK)   
  updateEMSReminder(emsData);
   
   return ret;
   
} /* end of EMSCalculateLength */
	
EMSTATUS EMSDecodeUDH(EMSData *emsData, kal_uint8 *data, kal_uint16 text_len)
{   

   EMSTATUS   ret = EMS_OK; /* reset */
   kal_uint8  udhl=0;   /* user data header length */
   kal_uint8  read_byte=0;  /* already read bytes */
   kal_uint8  iei;  /* IE identifier */
   kal_uint8  ie_len;   /* IE length */
   EMSObject  *object;
   kal_uint16 offset;
#ifndef __SLIM_EMS__
   kal_uint16 remaining_len = text_len;
#endif
   kal_uint16 prev_offset = 0;
   EMSInternalData*i = (EMSInternalData*) emsData->internal;

   /* 12/08/2003, Kevin
* keep a text format object to verify the text format is valid or not
* eg, duplicated text format shall be discarded 
*/
   EMSObjData tf_data;
   EMSObject  tf_obj;

   tf_obj.OffsetToText = 65530;/*reset*/
   tf_obj.data = &tf_data;

   udhl = *data;
   
   if(udhl > EMS_ONE_MSG_OCTET)
  return ret;

   read_byte += 1;

   while(ret == EMS_OK && read_byte <= udhl)
   { 
  object = NULL; /* reset */

  /* Get IEI */
  iei = *(data + read_byte);  

  /* Get IE Length */
  ie_len = *(data + read_byte + 1);

  read_byte += 2;

  if( (read_byte + ie_len - 1) > udhl)  
 break; /* remaining udh length can't contain an IE data */   

#if 1
  /* check whether the "offset" field is valid. if not, skip the object. */
  if(iei != EMS_CONC8_MSG_IEI && iei != EMS_CONC16_MSG_IEI &&
 iei != EMS_APP_PORT8_IEI && iei != EMS_APP_PORT16_IEI &&
 iei != EMS_EXT_OBJ_IEI   && iei != EMS_REUSED_EXT_OBJ_IEI  &&
 iei != EMS_COMPRESSED_IEI&& iei != EMS_OBJ_DIST_IND_IEI)
  {
 offset = (emsData->dcs == SMSAL_UCS2_DCS) ? (*(data+read_byte)*2) : *(data+read_byte);

 if(offset > text_len || (*(data+read_byte)*2 + i->baseTextOffset) < prev_offset)
 {
read_byte += ie_len;
continue;
 }
  }
#endif

  switch(iei)
  {
  /*----------------------------------------------
   * Concatenated short message , 8-bit and 16-bit reference 
   *----------------------------------------------*/ 
 case EMS_CONC8_MSG_IEI:   
 case EMS_CONC16_MSG_IEI:   
  
  break;

 /*----------------------------------------------- 
  * Application Port Address , 8-bit Address 
  *-----------------------------------------------*/
 case EMS_APP_PORT8_IEI:
 
  //if len is wrong, ignore this ie.
  if(ie_len!=2)
 break;

  emsData->PortNum.isPortNumSet = KAL_TRUE;
  emsData->PortNum.dst_port = *(data + read_byte );
  emsData->PortNum.src_port = *(data + read_byte + 1);

  break;

 /*----------------------------------------------- 
  * Application Port Address , 16-bit Address 
  *-----------------------------------------------*/
 case EMS_APP_PORT16_IEI:

  //if len is wrong, ignore this ie.
  if(ie_len!=4)
 break;

  emsData->PortNum.isPortNumSet = KAL_TRUE;
  emsData->PortNum.dst_port = *(data + read_byte) * 256 + *(data + read_byte + 1);
  emsData->PortNum.src_port = *(data + read_byte+2) * 256 + *(data + read_byte + 3);

  break;

#ifndef __SLIM_EMS__
#ifdef __EMS_REL5__
 case EMS_OBJ_DIST_IND_IEI:  /* Object Distribution Indicator IEI */ 

  if(ie_len != 2) 
 break;

  if( (*(data + read_byte + 1 ) & 0x01) == 0x01 )
  {
 if(*(data + read_byte) == 0) /* apply to all following objects */
i->ObjDistIndCount  = 0xff;
 else
i->ObjDistIndCount  = *(data + read_byte);
  }
  break;

#endif
 case EMS_TEXT_FORMAT_IEI :  /* Text Formating */  
  
  EMS_PRINT_STR("recv text format ie\n");

  if( ( *(data+read_byte+1)*2 ) == 0 )
  {
 /* TODO in REL5 EMS */
 /* Text formatting length is 0.  */
 /* The text format shall beused as a default text format for all SM. */
 /* (for all text in a concatenated SM.)  */
  }
  else
  { 
 if(ie_len >= 3) 
 {
EMSTextFormat tf;
kal_uint16 tf_len = (emsData->dcs==SMSAL_UCS2_DCS) ? (*(data+read_byte+1)*2) : *(data+read_byte+1);

tf.textLength = *(data + read_byte + 1)*2;

if((DecodeTFAttribute(ie_len, data + read_byte + 2, &tf) == KAL_TRUE) &&
   (remaining_len >= tf_len))
{

   if( (offset + tf_len ) > text_len)
  break;

   /* check : if offset is the same, check whether tf is duplcated */
   if((offset == tf_obj.OffsetToText) && 
   TextFormatCmp(&tf_obj.data->text_format, &tf) == KAL_FALSE) 
  break;   

   /* save the tf object */
   tf_obj.OffsetToText = offset;
   ems_mem_cpy(&tf_obj.data->text_format, &tf,
   sizeof(EMSTextFormat));

   object = EMSObjectAllocate(KAL_TRUE);
   if(object == NULL) 
  goto no_mem;

   object->Type = EMS_TYPE_TEXT_FORMAT;
   object->OffsetToText = *(data+read_byte)*2+i->baseTextOffset;

   ASSERT(EMS_R(object->data) != NULL);

   ems_mem_cpy(&EMS_P(object->data, text_format), &tf,
   sizeof(EMSTextFormat));

   remaining_len -= tf_len;  

} /* tf attribute ok && txt len allow */
 } /* if(ie_len>=3) */
  } /* text_len == 0 */

  break;   


 case EMS_PREDEF_SND_IEI :   /* Predefined Sound */
 case EMS_USER_DEF_SND_IEI : /* User Defined Sound */
 case EMS_PREDEF_ANM_IEI :   /* Predefined Animation */
 case EMS_LARGE_ANM_IEI :/* Large Animation */
 case EMS_SMALL_ANM_IEI :/* Small Animation */
 case EMS_LARGE_PIC_IEI :/* Large Picture */
 case EMS_SMALL_PIC_IEI :/* Small Picture */
 case EMS_VAR_PIC_IEI :  /* Variable Picture */ 
#ifdef __EMS_REL5__
 case EMS_EXT_OBJ_IEI :  /* Extended Object IEI */ 
 case EMS_COMPRESSED_IEI:/* Compression Control IEI */ 
 case EMS_REUSED_EXT_OBJ_IEI :/* Reused Extended Object IEI */ 
#endif
  ret = EMSDecodeUDHObj(&object, emsData, iei, ie_len, (data + read_byte)) ;
  if(ret == EMS_NO_MEMORY) 
 goto no_mem;

  break;
#endif
 default:
/* unrecognized ie, ignore this ie */  
break;
  } /* switch */

#ifndef __SLIM_EMS__
  /* get a new EMS object, and it into object list */
  if(object != NULL)
  { 
 object->next = NULL;
 object->prev = NULL;

#ifdef __EMS_REL5__
 if (EMS_CNTX(isCompressSupp) == KAL_TRUE &&
 IS_TXT_FMT_OBJ (object) != KAL_TRUE)  
 {
i->ExtObjRefNum = 255 ;
EXT_OBJ_INFO_P(object, ExtObjRef)  = EMSGetExtObjRef(emsData);
 }
#endif

 /* for MT case :using insert by offset */
 AddObjectIntoListByOffset(emsData, object);

 emsData->CurrentPosition.Object = object;
  }
#endif

   read_byte += ie_len;

   } /* while */

   return ret;

#ifndef __SLIM_EMS__
no_mem:

   if(object != NULL)
  ReleaseEMSObject(object);

   return EMS_NO_MEMORY;
#endif

} /* end of EMSDecodeUDH */


kal_bool EMSDetectObj(kal_uint8 *data)
{   

   kal_uint8  udhl=0;   /* user data header length */
   kal_uint8  read_byte=0;  /* already read bytes */
   kal_uint8  iei;  /* IE identifier */
   kal_uint8  ie_len;   /* IE length */

   udhl = *data;
   
   read_byte += 1;

   while(read_byte <= udhl)
   { 
  /* Get IEI */
  iei = *(data + read_byte);  

  /* Get IE Length */
  ie_len = *(data + read_byte + 1);

  read_byte += 2;

  if( (read_byte + ie_len - 1) > udhl)  
 break; /* remaining udh length can't contain an IE data */   


  switch(iei)
  {
 case EMS_CONC8_MSG_IEI:   
 case EMS_CONC16_MSG_IEI:   
 case EMS_APP_PORT8_IEI:
 case EMS_APP_PORT16_IEI:
  
  break;


 case EMS_TEXT_FORMAT_IEI :  /* Text Formating */  
 case EMS_PREDEF_SND_IEI :   /* Predefined Sound */
 case EMS_USER_DEF_SND_IEI : /* User Defined Sound */
 case EMS_PREDEF_ANM_IEI :   /* Predefined Animation */
 case EMS_LARGE_ANM_IEI :/* Large Animation */
 case EMS_SMALL_ANM_IEI :/* Small Animation */
 case EMS_LARGE_PIC_IEI :/* Large Picture */
 case EMS_SMALL_PIC_IEI :/* Small Picture */
 case EMS_VAR_PIC_IEI :  /* Variable Picture */ 
#ifdef __EMS_REL5__
 case EMS_EXT_OBJ_IEI :  /* Extended Object IEI */ 
 case EMS_COMPRESSED_IEI:/* Compression Control IEI */ 
 case EMS_REUSED_EXT_OBJ_IEI:/* Reused Extended Object IEI */ 
 case EMS_OBJ_DIST_IND_IEI:  /* Object Distribution Indicator IEI */ 
#endif
 default:
/* unrecognized ie, ignore this ie */  
  return KAL_TRUE;
break;
  } /* switch */

   read_byte += ie_len;

   } /* while */

   return KAL_FALSE;

} /* end of EMSDetectObj*/


