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
* ems_init.c
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


#define EMS_INIT_C

#include "kal_non_specific_general_types.h"
#include <stdio.h>
#include <memory.h> /* memcpy, memset */
#include <assert.h> /* assert */
#include <time.h>

#define ASSERT assert

#include <stdio.h>
#include <stdlib.h>
#include "smsal_l4c_enum.h"
#include "ems.h"
#include "ems_defs.h"
#include "ems_enums.h"
#include "ems_context.h"
#include "ems_init.h"
#include "ems_utils.h"
#include "ems_obj_proc.h"
#include "customer_ps_inc.h"
#include "custom_ems_context.h"

/*========================
 * EMS context 
 *========================*/
EMSContext ems_cntx_g;

const static kal_uint32 ems_subpool_size[5]={16,32,64,0xffffffff,0};
/*
 * EMS internal functions
 */

#ifndef __SLIM_EMS__
void EMSPduMemPoolInitialize(void)
{
#ifdef __EMS_REL5__
   EMSPduMemPool *pool;
   // modify by PhoneSuite
   //kal_uint16 i;
   int i;

   for(i = 0 ; i < EMS_MAX_EMS_DATA ; i++)
   {   

  pool = &EMS_CNTX(pdu_mem_pool[i]);
   
  /* get pool buffer and size from custom library. */
  pool->pool_ptr  = (kal_uint8 *) ems_get_pdu_mem_pool_info (i, &(pool->pool_size));

  pool->head = pool->tail = NULL;
  pool->end_ptr   = pool->pool_ptr + pool->pool_size; 

  pool->free_ptr  = pool->pool_ptr;

  /* check 4-byte alignment. */
  ASSERT(((kal_uint32)pool->free_ptr & 0x03) == 0);

   } /* for */   
#endif
}

void *EMS_get_mem_from_pdu_pool(kal_uint16 size, EMSObject *obj)
{   

#ifndef __EMS_REL5__

   return EMS_malloc(size);

#else
   // modify by PhoneSuite
   //kal_uint8  ems_id = EMS_CNTX(activeEMSDataID);  
   int  ems_id = EMS_CNTX(activeEMSDataID);  
   EMSPduMemPool *pool;
   EMSPduBuffMB  *new_mb;
   EMSPduBuffFooter  *footer_ptr;

   ASSERT(ems_id < EMS_MAX_EMS_DATA);
   
   pool = &EMS_CNTX(pdu_mem_pool[ems_id]);

   /* make 4-byte alignment. */
   size = ((((size) + 3) >> 2) << 2);

   size += sizeof(EMSPduBuffMB);

   if(EMS_CNTX(pdu_mem_footer)==KAL_TRUE)
  size += sizeof(EMSPduBuffFooter);

   if((kal_uint32)pool->free_ptr + size > (kal_uint32)pool->end_ptr)
  return NULL ;


   new_mb = (EMSPduBuffMB *)pool->free_ptr;

   new_mb->next  = NULL;
   new_mb->obj   = obj;
   new_mb->buff_size = size ;  /*include EMSPduBuffMB structure and footer */
   new_mb->ref_count = 1 ; 

   pool->free_ptr   += size;

   if(EMS_CNTX(pdu_mem_footer)==KAL_TRUE)
   {
  footer_ptr = (EMSPduBuffFooter *)(pool->free_ptr - sizeof(EMSPduBuffFooter));
  footer_ptr->footer = EMS_BUFF_FOOTPRINT;
   }

   /* insert to list */
   if(pool->head == NULL)
   {
  pool->head = new_mb;
  pool->tail = new_mb;
   }
   else
   {
  pool->tail->next = new_mb ;
  pool->tail = new_mb ;
   }

   return (void *)((kal_uint8 *)new_mb + sizeof (EMSPduBuffMB));
#endif

} /* end of EMS_get_mem_from_pdu_pool */

kal_bool EMS_free_mem_to_pdu_pool(kal_uint8 *pdu_ptr, EMSObject *obj)
{
#ifndef __EMS_REL5__

   EMS_mfree(pdu_ptr);

#else
   // modify by PhoneSuite
   //kal_uint8 ems_id = EMS_CNTX(activeEMSDataID);  
   int ems_id = EMS_CNTX(activeEMSDataID);  
   kal_uint16free_size, src_size;  
   EMSPduMemPool *pool;
   EMSPduBuffMB  *freed_mb, *src_mb, *dst_mb;
   EMSPduBuffFooter  *footer_ptr;

   ASSERT(ems_id < EMS_MAX_EMS_DATA);
   pool = &EMS_CNTX(pdu_mem_pool[ems_id]);

   freed_mb = (EMSPduBuffMB *)(pdu_ptr - sizeof(EMSPduBuffMB));

   EMS_EXT_ASSERT( freed_mb->ref_count != 0, freed_mb->ref_count, 0, 0);
  
   freed_mb->ref_count -- ;

   if (freed_mb->ref_count != 0)
  return KAL_FALSE;

   /* ref. count = 0; free the buffer */

   /* for EMS 5.1, it shall not be a reused extended obj */
   EMS_ASSERT(obj == freed_mb->obj);

   EMS_ASSERT(EXT_OBJ_INFO_P(obj, isReusedObj) == KAL_FALSE) ;

   if(EMS_CNTX(pdu_mem_footer)==KAL_TRUE)
   {
  footer_ptr = (EMSPduBuffFooter *)(pdu_ptr + freed_mb->buff_size - sizeof(kal_uint32));
  if (footer_ptr->footer != EMS_BUFF_FOOTPRINT)
  {
 EMS_ASSERT(0);
  }
   }

   /* 1. clear ptr in block buffer pool */
   free_size = freed_mb->buff_size ; 
   freed_mb->obj->data->common.pdu = NULL;

   /* 2. move existence block buffer */
   if (pool->head == freed_mb && pool->tail == freed_mb)
   {
  /* there is only one block */
  pool->head = NULL;
  pool->tail = NULL;
  pool->free_ptr = pool->pool_ptr;
  return KAL_TRUE;
   }
   else if(pool->tail == freed_mb)
   {
  /* delete the last block */
  kal_bool found = KAL_FALSE;

  src_mb = pool->head ;
  while(src_mb != NULL)
  {
 if(src_mb->next == freed_mb)
 {
found = KAL_TRUE;
break;
 }
 src_mb = src_mb->next ;
  }

  if(found == KAL_TRUE)
  {
 src_mb->next = NULL;
 pool->tail = src_mb;
 pool->free_ptr = (kal_uint8*)freed_mb ;

 return KAL_TRUE;
  }
  else
  {
 ASSERT(0);
  }
   }

   /* delete the first and intermediate block */
   /* head of list doesn't need to change. */

   /* move from src to dst */
   /* set src to the next block of freed block */
   /* set dst to the freed block */
   src_mb = freed_mb->next;
   dst_mb = freed_mb;

   while (src_mb != NULL)
   {
  /* update block ptr to pdu pointer of object */
  EMSUpdatePduBuffPtr(src_mb->obj, (kal_uint8 *)dst_mb + sizeof(EMSPduBuffMB));

  src_size = src_mb->buff_size ;
  /* move EMSPduBuffMB, raw data, and footer */
  ems_mem_cpy((kal_uint8 *)dst_mb,(kal_uint8 *)src_mb, src_size);

  if (pool->tail == src_mb)
 pool->tail = dst_mb;

  src_mb = dst_mb->next;

  if (src_mb != NULL)
  {
 dst_mb->next = (EMSPduBuffMB *)((kal_uint8 *)dst_mb + dst_mb->buff_size);
 dst_mb = dst_mb->next;
  }
   }

   ASSERT(((kal_uint32)pool->tail+ pool->tail->buff_size + free_size) == (kal_uint32)pool->free_ptr);

   pool->free_ptr -= free_size;
   ASSERT(((kal_uint32)pool->free_ptr & 0x03) == 0);

#endif
   return KAL_TRUE;
} /* end of EMS_free_mem_to_pdu_pool */

#endif

void *EMS_malloc(kal_uint16 size)
{
#ifdef __SLIM_EMS__
   return NULL;
#else
   void *ptr;

   ASSERT(size > 0);
   
   if(size <= 64)
   {
  ptr = ems_get_buffer(size);
   
  /* 4-byte alignment checking */
  ASSERT(((kal_uint32)ptr & 0x3)== 0);
   }
   else if (size <= 2048)
  ptr = ems_get_buffer(size);
   else
	  ptr = NULL;

   if (ptr != NULL)
  ems_mem_set(ptr, 0, size);

   return ptr;
#endif

} /* end of EMS_malloc */

void EMS_mfree(void *ptr)
{
free(ptr); /* not belong to EMS mem mgmt */
} /* end of EMS_mfree */

kal_bool EMSGetEMSDataResource(EMSData *emsData)
{
   int i;

   for(i = 0 ; i < EMS_MAX_EMS_DATA ; i++)
   {
  if(EMS_CNTX(isEMSDataUsed[i]) == KAL_FALSE)
  {
 EMS_CNTX(isEMSDataUsed[i]) = KAL_TRUE; /* mark as used */
 emsData->id= i; /* keep this id */
 emsData->internal  = (void*) &EMS_CNTX(internalData[i]);
 emsData->textBufferSize= EMS_MAX_SEG_NUM *153*2;
 emsData->textBuffer= EMS_CNTX(textBuffer[i]);
 return KAL_TRUE;
  }
   }
#ifndef _TEST
   if (i == EMS_MAX_EMS_DATA)
   {
		time_t ltime;
		unsigned long nId;
		time(&ltime);
		nId = ltime%EMS_MAX_EMS_DATA;
		if (nId < 5000)
			nId = EMS_MAX_EMS_DATA - nId;

		EMS_CNTX(isEMSDataUsed[nId]) = KAL_TRUE; /* mark as used */
		emsData->id= (int)nId; /* keep this id */
		emsData->internal  = (void*) &EMS_CNTX(internalData[nId]);
		emsData->textBufferSize= EMS_MAX_SEG_NUM *153*2;
		emsData->textBuffer= EMS_CNTX(textBuffer[nId]);
		return KAL_TRUE;
  }
#endif
   return KAL_FALSE; /* all ems data are used */
} /* end of EMSGetEMSDataResource */


void *EMS_asm_malloc(kal_uint16 size)
{
#ifdef __SLIM_EMS__
   return NULL;
#else
   void *ptr;

   ASSERT(size > 0);
#ifdef APPMEM_SUPPORT
   
   ptr = kal_app_mem_alloc(size, KAL_APPMEM_ASYNC, MOD_MMI); 

#else
   if (size <= 2048)
  ptr = ems_get_buffer(size);
   else
	  ptr = NULL;
#endif

   if (ptr != NULL)
  ems_mem_set(ptr, 0, size);

   return ptr;
#endif

} /* end of EMS_asm_malloc */

void EMS_asm_mfree(void *ptr)
{
#ifndef __SLIM_EMS__
#ifdef APPMEM_SUPPORT
   kal_app_mem_free(ptr);
#else
   ems_free_buffer(ptr); /* not belong to EMS mem mgmt */
#endif
#endif
} /* end of EMS_asm_mfree */

void EMSFreeEMSDataResource(EMSData *emsData)
{
   ASSERT(emsData->id < EMS_MAX_EMS_DATA);

   EMS_CNTX(isEMSDataUsed[emsData->id]) = KAL_FALSE; /* mark as free */
   
} /* end of EMSFreeEMSDataResource */


/*****************************************************************************
* FUNCTION
*  InitializeEMSInternalData
* DESCRIPTION
*   This function initializes the internal EMS data->
*
* PARAMETERS
*  a  IN/OUT  internal
*  b  IN  TPUD
* RETURNS
*  none
* GLOBALS AFFECTED
*  none
*****************************************************************************/
void InitializeEMSInternalData(EMSInternalData *internal, kal_uint8 *TPUD[])
{
   kal_uint8 i;

   internal->notFinishedObjectProcessed = KAL_FALSE;
   internal->NumFinishedObject = 0;
   internal->notFinishedObject = NULL;

   internal->numSegment= 0;
   internal->textOffset= 0; 
   internal->baseTextOffset= 0; 	
   internal->TPUD  = TPUD;   
#ifdef __EMS_REL5__
   internal->ObjDistIndCount   = 0 ;
#endif
   
   for(i=0;i<EMS_MAX_SEG_NUM ;i++)
   {
  internal->UDHL[i] = 0;	/* reset TP-UDH length */
  internal->TPUDLen[i] = 0; /* reset TPUD length */
   }

} /* end of InitializeEMSInternalData */


void UpdateLenInfo(EMSData *emsData)
{

   kal_uint8 concat_hdr;
   kal_uint8 port_info_hdr = 0;
   EMSInternalData *internal = (EMSInternalData*)emsData->internal;
   EMSReminder *reminder = (EMSReminder *)&(emsData->Reminder);

#ifdef __EMS_REL5__
   concat_hdr = EMS_16BIT_CONC_HDR_LEN_OCTET ;
#else
   concat_hdr = EMS_8BIT_CONC_HDR_LEN_OCTET ;
#endif


   if (emsData->PortNum.isPortNumSet == KAL_TRUE)
  port_info_hdr = EMS_PORT_HDR_LEN_OCTET ;


   internal->ConcatHdrLen = (kal_uint8) GetNumOfChar(emsData->dcs, 
(EMS_UDHL_HDR_LEN_OCTET + concat_hdr + port_info_hdr) );

   if(emsData->dcs == SMSAL_DEFAULT_DCS)
   {		
  internal->MaxLenPerSeg = EMS_ONE_MSG_LEN;
  internal->MaxUsableLen = EMS_ONE_MSG_LEN - internal->ConcatHdrLen ;

  reminder->segRemainingOctet = EMS_ONE_MSG_LEN;
  reminder->segUsableOctet= EMS_ONE_MSG_LEN;
  reminder->maxUsableLenPerSeg  = internal->MaxUsableLen ;
  reminder->totalRemainingOctet = EMS_MAX_SEG_NUM *internal->MaxUsableLen;
   }
   else
   {  
  internal->MaxLenPerSeg = EMS_ONE_MSG_OCTET;
  internal->MaxUsableLen = EMS_ONE_MSG_OCTET - internal->ConcatHdrLen; 

  reminder->segRemainingOctet = EMS_ONE_MSG_OCTET;
  reminder->segUsableOctet= EMS_ONE_MSG_OCTET;
  reminder->maxUsableLenPerSeg  = internal->MaxUsableLen ;
  reminder->totalRemainingOctet = EMS_MAX_SEG_NUM *internal->MaxUsableLen;
   }

} /* end of UpdateLenInfo */






