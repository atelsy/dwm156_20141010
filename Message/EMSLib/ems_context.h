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
*   ems_context.h
*
* Project:
* --------
*   DCT
*
* Description:
* ------------
*   This file contains function prototypes of .
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


#ifndef _EMS_CONTEXT_H
#define _EMS_CONTEXT_H


typedef kal_uint16 (*EMS_CONCAT_MSG_REF_FUNCTION)(void);

typedef kal_bool (*const EMS_OBJ_PACK_FUNCTION)( EMSData *,
 EMSObject *,
					  kal_bool /* needPack */);

typedef struct
{
   kal_uint8  isExtObj; 
   kal_uint8  isReusedObj; 
   kal_uint8  ExtObjRef; /* reference number of extended object or reused object */ 
   kal_uint8  Padding;   /* to make sure 4-byte align */
}EMSExtObjInfo;

/* Following are used for the IE which may cross segment,  */
/* i.e. Compression or Extended Object.  */

/* For MT case: */
/* If it occurs missed segment condition, all pending variables  */
/* and buffer will be freed. The IE will be ignored.*/

/* For MO case: */
/* The buffer is used as temp buffer of extended object before compression or packed. */

typedef struct
{
   kal_bool	BufType;   /* Is there pending IE existed.  */
   kal_uint16  BufLen;/* MT: IE(buffer) length. Get from IE information. */
  /* MO: total extend IE length in buffer */
   kal_uint16  BufOffset; /* MT: existed data length in buffer */
  /* MO: data length already packed */

   kal_uint8   *BufPtr;   /* Buffer for IE data */
}EMSPendingBuff;

typedef struct 
{
   kal_uint32  footer;
}EMSPduBuffFooter; 


typedef struct EMSPduBuffMB_struct
{
   struct EMSPduBuffMB_struct  *next;
   EMSObject   *obj;
   kal_uint16  buff_size;
   kal_uint8   ref_count;
   kal_uint8   padding;
}EMSPduBuffMB; 

typedef struct
{
   kal_uint16	pool_size;   /* pool size */
   kal_uint8	*pool_ptr;/* start of pool */

   kal_uint8	*end_ptr; /* end ptr of block buffer */
   kal_uint8	*free_ptr;/* start ptr of free buffer */

   struct EMSPduBuffMB_struct  *head;/* head of used buffer */
   struct EMSPduBuffMB_struct  *tail;/* tail of used buffer */

}EMSPduMemPool; 

typedef struct _EMSObjQueue
{
   EMSObject   *obj;   	/* object pointer */
   struct _EMSObjQueue *prev;/* pointer to previous element */
   struct _EMSObjQueue *next;/* pointer to next element */
} EMSObjQueue;


typedef struct
{
   /* used when packing EMS data into TP-UD(s)  */
   kal_uint16  totalTxtFmtOctet;  /* total text fmt object octet number */
   kal_uint16  totalObjOctet; /* total object octet number, exclude extended object */
   kal_uint16  totalExtObjOctet;  /* total Extended Object octet number */
   kal_uint8   numOfEMSObject;
   kal_uint8   numOfExtObject;

   kal_uint8   MaxLenPerSeg;/* 160 if dcs is GSM7-bit; 
						   * 140 otherwise */   
   kal_uint8   MaxUsableLen;/* exclude concatenated ie */
   kal_uint8   ConcatHdrLen;

   kal_uint8   CurrentConcatMsgRef; 
	
   kal_uint16  baseTextOffset;
   kal_uint16  textOffset;   	
   kal_uint8   numSegment;

   kal_uint8   NumFinishedObject;
   kal_bool		notFinishedObjectProcessed;
   void	*notFinishedObject;

   kal_uint8   **TPUD;
   kal_uint8   *TPUDLen; 
   kal_uint8   *UDHL;


#ifdef __EMS_REL5__
   kal_uint16  CompressIELen;   /* The Compress IE length, include 3 byte header info. */
   kal_boolneedReCompress;  /* Is there pending IE existed.  */
   kal_boolforceCompress;   /* force to compress.  */

   /* used when unpacking EMS data from TP_UD */
   kal_uint8   UnpackSegNum;/* Buffer for IE data */
   kal_boolMissSegInCrossIE;/* does missing segment occur in cross segment IE.  */

   kal_uint8   ObjDistIndCount;
   kal_uint8   ExtObjRefNum;

   kal_boolisObjSwitched; 
   kal_uint8   RefNumOfSwitchObj;

   EMSPendingBuff PBuff;
#endif

   kal_uint8   error; /* ref to EMSTATUS */
   
} EMSInternalData;

typedef struct
{
   kal_bool isInit; /* initialize or not */   
   kal_bool isCompressSupp; /* Support Compress or not */  
   
   // modify by PhoneSuite
   //kal_uint8 activeEMSDataID; /* store active EMS Data id */
   int activeEMSDataID; /* store active EMS Data id */
   kal_bool  isEMSDataUsed[EMS_MAX_EMS_DATA]; /* record which EMS Data is used */

   EMSInternalData   internalData[EMS_MAX_EMS_DATA]; /* internal Data */
   kal_uint8 *textBuffer[EMS_MAX_EMS_DATA]; /* text buffer */

   void  *mem_pool_buff[EMS_MAX_EMS_DATA]; 
//#ifndef EMS_ON_WIN32
  // KAL_ADM_IDEMSPoolID[EMS_MAX_EMS_DATA];
//#endif

   EMSPduMemPool pdu_mem_pool[EMS_MAX_EMS_DATA];
   kal_bool  pdu_mem_footer;   

   EMS_CONCAT_MSG_REF_FUNCTION concat_msg_ref_func;
   kal_uint16  concat_msg_ref;
   kal_uint8   max_seg_num;
   kal_uint16  max_obj_num;

} EMSContext;


extern EMSContext ems_cntx_g;

#endif /* _EMS_CONTEXT_H */


