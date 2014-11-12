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
 * ems_utils.c
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


#define EMS_UTILS_C

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
#include "customer_ps_inc.h"
#include "custom_ems_context.h"


kal_uint16 GetNumOfChar(kal_uint8 dcs, kal_uint16 numOfOctet)
{   
   if(dcs == SMSAL_DEFAULT_DCS)
   {
  /* GSM 7-bit encoding */
  kal_uint16 NumOfChar = (numOfOctet * 8) / 7;
  if(((numOfOctet * 8) % 7) > 0)
 NumOfChar++;
  return NumOfChar;
   }
   else
   {
  return numOfOctet;
   }
}

kal_uint16 GetOctetNumFromCharNum(kal_uint8 dcs, kal_uint16 numOfChar)
{   
   if(dcs == SMSAL_DEFAULT_DCS)
   {
  /* GSM 7-bit encoding */
	  return  ((numOfChar * 7) >> 3 );
   }
   else
   {
  return numOfChar;
   }
}


#ifdef __EMS_REL5__
kal_uint8 EMSGetExtObjRef(EMSData *emsData) 
{
   EMSInternalData *i = (EMSInternalData*) emsData->internal;
   EMSObject   *obj;
   kal_uint16  j;
   kal_boolis_ref_num_used;

   if( i->ExtObjRefNum == 255 )
   {
  if (emsData->listHead == NULL)
  {
 /* reset the number */
 i->ExtObjRefNum = 1 ;
 return i->ExtObjRefNum ;
  }

  /* over flow */
  /* re-find free reference number from object list */
  for(j=1; j<255; j++)
  {
 is_ref_num_used = KAL_FALSE;
 obj = emsData->listHead;
 while (obj != NULL)
 {
if( IS_TXT_FMT_OBJ (obj) != KAL_TRUE  &&
EXT_OBJ_INFO_P(obj, isReusedObj) != KAL_TRUE) 
{
   if((EMS_CNTX(isCompressSupp) == KAL_FALSE && EXT_OBJ_INFO_P(obj, isExtObj) == KAL_TRUE ) ||
  (EMS_CNTX(isCompressSupp) == KAL_TRUE) )
   {
  if(j== EXT_OBJ_INFO_P(obj, ExtObjRef))
  {
 is_ref_num_used = KAL_TRUE;
 break;
  }
   }
}

obj = obj->next;
 }

 if (is_ref_num_used == KAL_FALSE)
return (kal_uint8) j;
  }

  if(j==255)
 ASSERT(KAL_FALSE);
   }   
   else
   {
  i->ExtObjRefNum ++  ;
   }

   return i->ExtObjRefNum ;
}
#endif

kal_uint8 EMSGetConctMsgRef( void )
{   
/* This function returns the concatenated message reference, 
 * which goes as a part of TPDU data for MT concatenated messages.
 * The value of message reference ranges from 0..255, after 255 
 * concatenated message reference is rested to 0. */

if( EMS_CNTX(concat_msg_ref) == 255 )
{
   EMS_CNTX(concat_msg_ref) = 0;
}   
else
{
   EMS_CNTX(concat_msg_ref) ++;
}


return EMS_CNTX(concat_msg_ref) ;


} /* end of function EMSGetConctMsgRef */


/* calculate offset to unpack message, eg: if UDH contains only 
 * concatenated 8-bit reference, then udhl = 6, thus need 
 * 7 septets to contain 6 octets */
kal_uint16 EMS_compute_udh_offset(kal_uint8 udhl)
{
   kal_uint16 offset = (udhl * 8) / 7;
   if(((udhl * 8) % 7) > 0)
  offset = offset + 1;
   return (offset * 7); /* in terms of bits */
}

kal_uint16 EMSInsertMissSegStr(kal_uint8 *textBuffer)
{

   kal_uint8  *miss_str, i;
   kal_uint16  miss_str_len;

   miss_str = ems_get_miss_seg_str (&miss_str_len);

   if(textBuffer != NULL)
   {
  for(i = 0 ; i < miss_str_len ; i++)  
  {
#ifdef EMS_LITTLE_ENDIAN
 *(textBuffer+ 2*i)= *(miss_str + i);
 *(textBuffer+ 2*i +1) = 0;
#else
 *(textBuffer+ 2*i)= 0;
 *(textBuffer+ 2*i +1) = *(miss_str + i);
#endif
  }
   }

   return (2 * miss_str_len);
}

void EMSAddNullTerminator(kal_uint8 *textBuffer)
{
   if(textBuffer != NULL)
   {
  *(textBuffer)   = '\0';
  *(textBuffer+1) = '\0';
   }

   return;
}

#ifdef __EMS_REL5__
#define  EMS_MAX_SLICE_LEN   63
#define  EMS_MAX_SLICE_OFFSET   511
#define  EMS_MIN_MATCH_LEN3
#define  EMS_MAX_LITERAL_BLOCK  127

kal_bool EMSCompression(kal_bool need_pack,
kal_uint16  in_len,  kal_uint8 *in_buff, 
kal_uint16 *out_len, kal_uint8 *out_buff,
kal_uint8 *comp_buf)
{

   kal_uint8  c;
   kal_int16  i;
   kal_uint16 j, len, offset, read_byte, index;
   kal_uint16 lab_index; /* start of look ahead buffer */
   kal_uint16 lb_index, lb_len; /* start index and length of literal data block */

   if(in_buff == NULL)
  return KAL_FALSE;

   if(need_pack == KAL_TRUE )
   {
  if(out_buff == NULL)
 return KAL_FALSE ;

  for (i = 0; i < (*out_len) ; i++) 
 out_buff[i] =  '\0';
   }

   index = 0;
   lab_index = 0;
   read_byte = 0;

   lb_index  = 0;
   lb_len= 0;

   while( read_byte < in_len)
   {
  c = *(in_buff + read_byte); 

  offset = 0 ;
  len= 1 ;

  for (i = read_byte - 1 ;(read_byte > 2) && (i >= lab_index ) ; i--)
  {
 if (*(in_buff + i) == c) 
 {
for (j = 1; (i+j) < read_byte && (read_byte +j) < in_len ; j++)
{
   if (*(in_buff + i +j) != *(in_buff + read_byte + j) ) 
  break;
   if ( j == EMS_MAX_SLICE_LEN) 
  break;
}

if (j > len || j == EMS_MAX_SLICE_LEN) 
{
   offset = read_byte - i;  
   len= j;

   if ( j == EMS_MAX_SLICE_LEN) 
  break;
}
 }
  }

  if (len < EMS_MIN_MATCH_LEN) 
  {
 /* update length of literal data block */
 len = 1;
 lb_len += len;

 if(lb_len >= EMS_MAX_LITERAL_BLOCK || (read_byte + len) == in_len)
 {
kal_uint8 literal_len;
literal_len = (lb_len > EMS_MAX_LITERAL_BLOCK )? 
  EMS_MAX_LITERAL_BLOCK : lb_len ;

if(need_pack == KAL_TRUE )
   *(out_buff + index) = (literal_len | 0x80) ;

index ++ ;

if(need_pack == KAL_TRUE )
   ems_mem_cpy (out_buff +index, in_buff + lb_index, literal_len);  

index+= literal_len ;
lb_index += literal_len ;
lb_len   -= literal_len ;
 }
  }
  else
  {
 if(lb_len > 0 ) 
 {
if(need_pack == KAL_TRUE )
   *(out_buff + index) = (lb_len | 0x80) ;
index ++ ;

if(need_pack == KAL_TRUE )
   ems_mem_cpy (out_buff +index, in_buff + lb_index, lb_len);  

index+= lb_len ;
lb_index += lb_len ;
lb_len   = 0;
 }

 lb_index += len ;

 if(need_pack == KAL_TRUE )
*(out_buff + index) = (kal_uint8 )(0x7f & (((len & 0x003f) << 1) | ((offset>>8)& 0x01) ));
 index ++ ;

 if(need_pack == KAL_TRUE )
*(out_buff + index) = (kal_uint8 )(offset & 0xff) ;
 index ++ ;
  }

  read_byte += len;  
  if (read_byte > EMS_MAX_SLICE_OFFSET )
 lab_index = read_byte - EMS_MAX_SLICE_OFFSET;


  if(index > 0 && comp_buf != NULL)
  {
		
 if(0 != ems_mem_cmp(out_buff, comp_buf, index))
 {
   while(1);
 }
  }

   }

   if(need_pack == KAL_TRUE )
   {
   if(index != (*out_len))
   return KAL_FALSE;
   }

   if( out_len != NULL)
  *out_len = index;

   return KAL_TRUE;
}

/*****************************************************************************
* FUNCTION
*  EMSDeCompression
* DESCRIPTION
*   This function is to decompress compression control IE. 
*
* PARAMETERS
*  a  IN  in_len  , exclude 3 bytes header of compression IE
*  b  IN  in_buff
*  c  OUT out_len
*  d  OUT out_buff
* RETURNS
* EMSTATUS 
* GLOBALS AFFECTED
*  none
*****************************************************************************/
kal_bool EMSDeCompression(kal_bool need_unpack,
  kal_uint16  in_len,  kal_uint8 *in_buff, 
  kal_uint16 *out_len, kal_uint8 *out_buff)
{

   kal_uint8  c;
   kal_uint16 i, len, offset, index, r_index, read_byte;

   if(in_buff == NULL)
  return KAL_FALSE;

   if(need_unpack== KAL_TRUE )
   {
  if(out_buff == NULL)
 return KAL_FALSE ;

  for (i = 0; i < (*out_len) ; i++) 
 out_buff[i] =  '\0';
   }

   index = 0;
   read_byte = 0;

   while( read_byte < in_len)
   {
  c = *(in_buff + read_byte); 
  read_byte ++;
  if ((c & 0x80) == 0x80) 
  {
 len = (c & 0x7f) ;

 if(len > 0) 
 {
if(need_unpack == KAL_TRUE )
   ems_mem_cpy (out_buff +index, in_buff + read_byte, len);  

index += len;
read_byte += len;
 }
  } 
  else 
  {
 len = (c >> 1);
 offset = ((c & 0x01) << 8) +  *(in_buff + read_byte) ;
 read_byte ++;

 if (index < offset)
 {
 return KAL_FALSE;
 }

 r_index = index - offset;

 if(len > 0) 
 {
if(need_unpack == KAL_TRUE )
   ems_mem_cpy (out_buff +index, out_buff + r_index, len);  

index += len;
r_index += len;
 };
  }
   }

   if(need_unpack == KAL_TRUE )
   {
   if(index != (*out_len))
   return KAL_FALSE;
   }

   if( out_len != NULL)
  *out_len = index;

   return KAL_TRUE;
}

void EMSFreePBuff(EMSPendingBuff *pbuff)
{
   if (pbuff->BufType != EMS_PBUFF_NONE)
   {
  if (pbuff->BufPtr != NULL)
 EMS_ASM_MFREE(pbuff->BufPtr);
 
  pbuff->BufType   = EMS_PBUFF_NONE;
  pbuff->BufPtr= NULL;
  pbuff->BufLen= 0;
  pbuff->BufOffset = 0;
   }
   else
   {
  ASSERT(pbuff->BufPtr == NULL);
   }
}


EMSTATUS EMSGetPBuff(EMSPendingBuff *pbuff, kal_uint16 size, 
 EMSPBuffType type, kal_bool need_buff)
{
   if (pbuff->BufType != EMS_PBUFF_NONE)
   {
  EMS_EXT_ASSERT(KAL_FALSE, pbuff->BufType,0,0);

  if (pbuff->BufPtr != NULL)
 EMS_ASM_MFREE(pbuff->BufPtr);
   }

   if (need_buff == KAL_TRUE)
   {
  pbuff->BufPtr = (kal_uint8*) EMS_ASM_MALLOC(size);

  if(pbuff->BufPtr == NULL)
  {
 pbuff->BufType = EMS_PBUFF_NONE ;
 return EMS_NO_MEMORY;
  }
   }

   pbuff->BufType   = type;
   pbuff->BufLen= size;
   pbuff->BufOffset = 0;

   return EMS_OK;
}


#endif




