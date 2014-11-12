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
* ems_test.c
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



/* KAL - RTOS Abstraction Layer */
#include "kal_non_specific_general_types.h"
#include <stdio.h>
//#include <malloc.h> /* malloc, free */
#include <memory.h> /* memcpy, memset */
#include <assert.h> /* assert */

#include "smsal_defs.h"

#define ASSERT assert

#ifdef EMS_UT

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


/*================================================
 * EMS Debug Functions
 *================================================*/
#define OPTIMIZE_TXT_FMT
#define EMS_TEST_MAX_SEG MMI_SMS_MAX_MSG_SEG

#ifdef __EMS_REL5__
#define MAX_TEXT_LEN_SEPTET (EMS_TEST_MAX_SEG * 152)
#define MAX_TEXT_LEN_OCTET  (EMS_TEST_MAX_SEG * 133)
#define MAX_CHAR_UCS2   (EMS_TEST_MAX_SEG * 66)
#define EMS_TEST_CONC_HDR   (7)
#else
#define MAX_TEXT_LEN_SEPTET (EMS_TEST_MAX_SEG * 153)
#define MAX_TEXT_LEN_OCTET  (EMS_TEST_MAX_SEG * 134)
#define MAX_CHAR_UCS2   (EMS_TEST_MAX_SEG * 67)
#define EMS_TEST_CONC_HDR   (6)
#endif

#define NUM_EMS_BASIC_TEXT_ITEM (19)
#define EMS_TEST_DATA_PATH  "d:\\fs\\ems"

EMSData emsData; 
EMSData emsData1;

kal_uint8 *TPUD_p[EMS_TEST_MAX_SEG];   
kal_uint8 TPUDLen[EMS_TEST_MAX_SEG];
kal_uint8 *string;   


kal_uint8 var_pic1[8];
kal_uint8 small_anm[32];
kal_uint8 large_anm[128];
kal_uint8 small_pic1[32];

typedef struct 
{
   kal_uint8 type;
   EMSObjDataobjData;
   
} EMSTestObjData_t;

typedef enum
{
   EMS_EXT_TEST_OBJ_PIC_1B_128, /* 0 */ /* size between 128-512 */ 
   EMS_EXT_TEST_OBJ_PIC_1B_512,  
   EMS_EXT_TEST_OBJ_PIC_1B_1024, 
   EMS_EXT_TEST_OBJ_PIC_2B_64,   
   EMS_EXT_TEST_OBJ_PIC_2B_128,  
   EMS_EXT_TEST_OBJ_PIC_2B_512,  /* 5 */  
   EMS_EXT_TEST_OBJ_PIC_2B_1024, 
   EMS_EXT_TEST_OBJ_PIC_6B_128,  
   EMS_EXT_TEST_OBJ_PIC_6B_512,  
   EMS_EXT_TEST_OBJ_PIC_6B_1024, 


   EMS_EXT_TEST_OBJ_ANM_1B_128,  /* 10 */  
   EMS_EXT_TEST_OBJ_ANM_1B_512,  
   EMS_EXT_TEST_OBJ_ANM_1B_1024, 
   EMS_EXT_TEST_OBJ_ANM_2B_128,  
   EMS_EXT_TEST_OBJ_ANM_2B_512,  
   EMS_EXT_TEST_OBJ_ANM_2B_1024, /* 15 */  
   EMS_EXT_TEST_OBJ_ANM_6B_128,  
   EMS_EXT_TEST_OBJ_ANM_6B_512,  
   EMS_EXT_TEST_OBJ_ANM_6B_1024, 

   EMS_EXT_TEST_OBJ_SND_128, 
   EMS_EXT_TEST_OBJ_SND_512,  /* 20 */  
   EMS_EXT_TEST_OBJ_SND_1024,

   EMS_EXT_TEST_OBJ_VAPP_128, 
   EMS_EXT_TEST_OBJ_VAPP_512, 
   EMS_EXT_TEST_OBJ_VAPP_1024,

   NUM_EMS_EXT_TEST_OBJ_DATA /* 25 */  

}EMSTestObjData_enum ;

typedef enum
{
   EMS_OBJ_PIC_SMALL,
   EMS_OBJ_PIC_LARGE,  
   EMS_OBJ_ANM_SMALL, 
   EMS_OBJ_ANM_LARGE, 
   EMS_OBJ_SND,   
   NUM_EMS_TEST_OBJ_DATA 

}EMSTestObjData_enum ;

/* normal objData */
EMSTestObjData_t EMSTestObjData[NUM_EMS_TEST_OBJ_DATA];

/* extended object */
EMSTestObjData_t EMSTestExtObjData[NUM_EMS_EXT_TEST_OBJ_DATA];

void EMSFillObjData (void)
{

   kal_uint8 i;
   kal_uint8 *large_buf, *small_buf;
   EMSObjData *objData;


   large_buf = (kal_uint8 *) malloc(128) ;
   small_buf = (kal_uint8 *) malloc(32) ;

   for (i=0; i < 128; i++)
  large_buf[i] = i ;

   for (i=0; i < 32; i++)
  small_buf[i] = i ;

   EMSTestObjData[EMS_OBJ_PIC_SMALL].type = EMS_TYPE_PIC ;
   objData = &EMSTestObjData[EMS_OBJ_PIC_SMALL].objData;
   objData->picture.bitsPerPixel = 1; /* mono picture */
   objData->picture.vDim = 16; 
   objData->picture.hDim = 16;
   objData->picture.pdu_length = 32 ;
   objData->picture.pdu = small_buf;
   objData->picture.attribute = 0; 

   EMSTestObjData[EMS_OBJ_PIC_LARGE].type = EMS_TYPE_PIC ;
   objData = &EMSTestObjData[EMS_OBJ_PIC_LARGE].objData;
   objData->picture.bitsPerPixel = 1; /* mono picture */
   objData->picture.vDim = 32; 
   objData->picture.hDim = 32;
   objData->picture.pdu_length = 128 ;
   objData->picture.pdu = large_buf;
   objData->picture.attribute = 0; 

   EMSTestObjData[EMS_OBJ_ANM_SMALL].type = EMS_TYPE_USERDEF_ANM;
   objData = &EMSTestObjData[EMS_OBJ_ANM_SMALL].objData;
   objData->animation.bitsPerPixel = 1;
   objData->animation.vDim   = 8; 
   objData->animation.hDim   = 8;
   objData->animation.cFrame = 4;
   objData->animation.cRepeat= 0;
   objData->animation.Duration=0;
   objData->animation.pdu_length = 32 ;
   objData->animation.pdu = small_buf;
   objData->animation.attribute = 0; 

   EMSTestObjData[EMS_OBJ_ANM_LARGE].type = EMS_TYPE_USERDEF_ANM;
   objData = &EMSTestObjData[EMS_OBJ_ANM_LARGE].objData;
   objData->animation.bitsPerPixel = 1; 
   objData->animation.vDim   = 16; 
   objData->animation.hDim   = 16;
   objData->animation.cFrame = 4;
   objData->animation.cRepeat= 0;
   objData->animation.Duration=0;
   objData->animation.pdu_length = 128 ;
   objData->animation.pdu = large_buf;
   objData->animation.attribute = 0; 

   EMSTestObjData[EMS_OBJ_SND].type = EMS_TYPE_USERDEF_SND;
   objData = &EMSTestObjData[EMS_OBJ_SND].objData ;
   objData->sound.pdu_length = 128 ;
   objData->sound.pdu = large_buf;
   objData->sound.attribute = 0; 

}

void EMSDataDump(EMSData *emsData)
{
   EMSObject *obj = emsData->listHead;
   kal_uint8 buf[100];
   kal_uint8 i = 1;

   kal_print("\n================================================\n");
   
   sprintf(buf, "text length : %d\n", emsData->textLength);
   kal_print((kal_char*)buf);
   sprintf(buf, "current position: %d\n", emsData->CurrentPosition.OffsetToText );
   kal_print((kal_char*)buf);
   sprintf(buf, "current object  : 0x%x\n", emsData->CurrentPosition.Object );
   kal_print((kal_char*)buf);
   sprintf(buf, "current txt_fmt : 0x%x\n", emsData->CurrentTextFormatObj);
   kal_print((kal_char*)buf);
   sprintf(buf, "dcs=%d, udhi=%d, port=%d, seg=%d, remain_o=%d",  emsData->dcs, emsData->udhi,
 emsData->PortNum.isPortNumSet, emsData->Reminder.requiredSegment, emsData->Reminder.totalRemainingOctet);
   kal_print((kal_char*)buf);

   kal_print("\n\nEMS object:=====================\n");
   while(obj != NULL)
   {
  sprintf(buf, "[%2d][0x%x]: off=%3d  ", i,obj, obj->OffsetToText);
  kal_print((kal_char*)buf);

  if (obj == emsData->CurrentPosition.Object)
 kal_print("[*CO*]");
  else
 kal_print("  ");

  if (obj == emsData->CurrentTextFormatObj)
 kal_print("[*CT*]");
  else
 kal_print("  ");

#ifdef __EMS_REL5__
  {
 sprintf(buf,"[%2d]", EXT_OBJ_INFO_P(obj, ExtObjRef));
 kal_print((kal_char*)buf);
  }

  if (EXT_OBJ_INFO_P(obj, isExtObj) == KAL_TRUE) 
  {
 sprintf(buf, "[EXTEND:%d]", EXT_OBJ_INFO_P(obj, ExtObjRef));
 kal_print((kal_char*)buf);
  }

  if (EXT_OBJ_INFO_P(obj, isReusedObj) == KAL_TRUE)
  {
 sprintf(buf, "[REUSED:%d]", EXT_OBJ_INFO_P(obj, ExtObjRef));
 kal_print((kal_char*)buf);
  }

  ASSERT( !((EXT_OBJ_INFO_P(obj, isExtObj) == KAL_TRUE) &&
(EXT_OBJ_INFO_P(obj, isReusedObj) == KAL_TRUE)));
#endif
  switch(obj->Type)
  {
 case EMS_TYPE_TEXT_FORMAT:
sprintf(buf, "Text format : length = %d\n", EMS_P(obj->data,text_format.textLength));
kal_print((kal_char*)buf);
break;

 case EMS_TYPE_PIC:
sprintf(buf, "Picture: size=%d, (%d,%d,%d,%d)\n", EMS_P(obj->data,picture.pdu_length), 
   EMS_P(obj->data,picture.bitsPerPixel),
   EMS_P(obj->data,picture.hDim),
   EMS_P(obj->data,picture.vDim),
   EMS_P(obj->data,picture.attribute));
kal_print((kal_char*)buf);
break;

 case EMS_TYPE_PREDEF_ANM:
sprintf(buf, "Predefined animation: no=%d\n", obj->PredefNo);
kal_print((kal_char*)buf);
break;

 case EMS_TYPE_PREDEF_SND:
sprintf(buf, "Predefined sound: no=%d\n", obj->PredefNo);
kal_print((kal_char*)buf);
break;
 case EMS_TYPE_USERDEF_SND:
sprintf(buf, "User defined sound: size=%d (%d)\n", EMS_P(obj->data,sound.pdu_length),
   EMS_P(obj->data,sound.attribute));
kal_print((kal_char*)buf);
break;

 case EMS_TYPE_USERDEF_ANM:
sprintf(buf, "animation: size=%d, (%d,%d,%d,%d,%d)\n", EMS_P(obj->data,animation.pdu_length),
 EMS_P(obj->data,animation.bitsPerPixel),
 EMS_P(obj->data,animation.hDim),
 EMS_P(obj->data,animation.vDim),
 EMS_P(obj->data,animation.cFrame),
 EMS_P(obj->data,animation.attribute));
kal_print((kal_char*)buf);
break;
 case EMS_TYPE_VCARD:
sprintf(buf, "vCard: size=%d (%d)\n", EMS_P(obj->data,vapp.pdu_length),
  EMS_P(obj->data,vapp.attribute));
kal_print((kal_char*)buf);

break;
 case EMS_TYPE_VCALENDAR:
sprintf(buf, "vCard: size=%d (%d)\n", EMS_P(obj->data,vapp.pdu_length),
  EMS_P(obj->data,vapp.attribute));
kal_print((kal_char*)buf);

break;

 default:
kal_print("invalid EMS object\n");
break;
  }
  obj = obj->next;
  i++;
   }/* while */

   kal_print("\ntext part: ========================\n");
   if(emsData->textLength > 0)
   {
  kal_uint16 i;

#ifdef __WIN32__
  if(emsData->dcs == SMSAL_DEFAULT_DCS)
  {
 for(i=0;i<emsData->textLength/2;i++)
 {
#ifdef EMS_LITTLE_ENDIAN
printf("0x%x ", *(emsData->textBuffer+2*i));
#else
printf("0x%x ", *(emsData->textBuffer+2*i+1));
#endif
if(((i+1)%10)==0) printf("\n");
 }//for
  }
  else
  {
 for(i=0;i<emsData->textLength;i++)
 {
printf("0x%x ", *(emsData->textBuffer+i));
if(((i+1)%10)==0) printf("\n");
 }
  }//dcs
#endif//__WIN32__
   }
   kal_print("\n================================================\n");
   kal_print("\n\n");

} /* end of EMSDataDump */

void EMSDataDump_log(EMSData *emsData, FILE *fp_log)
{
   EMSObject *obj = emsData->listHead;
   kal_uint8 buf[100];
   kal_uint8 i = 1;

   sprintf(buf, "\n================================================\n");
   kal_print((kal_char*)buf);
   fwrite( buf, sizeof( char ), strlen(buf), fp_log );
   
   sprintf(buf, "text length : %d\n", emsData->textLength);
   kal_print((kal_char*)buf);
   fwrite( buf, sizeof( char ), strlen(buf), fp_log );

   sprintf(buf, "current position: %d\n", emsData->CurrentPosition.OffsetToText );
   kal_print((kal_char*)buf);
   fwrite( buf, sizeof( char ), strlen(buf), fp_log );

   sprintf(buf, "current object  : 0x%x\n", emsData->CurrentPosition.Object );
   kal_print((kal_char*)buf);
   fwrite( buf, sizeof( char ), strlen(buf), fp_log );

   sprintf(buf, "current txt_fmt : 0x%x\n", emsData->CurrentTextFormatObj);
   kal_print((kal_char*)buf);
   fwrite( buf, sizeof( char ), strlen(buf), fp_log );

   sprintf(buf, "dcs=%d, udhi=%d, port=%d, seg=%d, remain_o=%d",  emsData->dcs, emsData->udhi,
 emsData->PortNum.isPortNumSet, emsData->Reminder.requiredSegment, emsData->Reminder.totalRemainingOctet);
   kal_print((kal_char*)buf);
   fwrite( buf, sizeof( char ), strlen(buf), fp_log );

   sprintf(buf,"\n\nEMS object:=====================\n" );
   kal_print((kal_char*)buf);
   fwrite( buf, sizeof( char ), strlen(buf), fp_log );

   while(obj != NULL)
   {
  sprintf(buf, "[%2d][0x%x]: off=%3d  ", i,obj, obj->OffsetToText);
  kal_print((kal_char*)buf);
  fwrite( buf, sizeof( char ), strlen(buf), fp_log );

  if (obj == emsData->CurrentPosition.Object)
  {
 sprintf(buf, "[*CO*]");
 kal_print((kal_char*)buf);
 fwrite( buf, sizeof( char ), strlen(buf), fp_log );
  }
  else
  {
 sprintf(buf, "  ");
 kal_print((kal_char*)buf);
 fwrite( buf, sizeof( char ), strlen(buf), fp_log );
  }

  if (obj == emsData->CurrentTextFormatObj)
  {
 sprintf(buf, "[*CT*]");
 kal_print((kal_char*)buf);
 fwrite( buf, sizeof( char ), strlen(buf), fp_log );
  }
  else
  {
 sprintf(buf, "  ");
 kal_print((kal_char*)buf);
 fwrite( buf, sizeof( char ), strlen(buf), fp_log );
  }

#ifdef __EMS_REL5__
  {
 sprintf(buf,"[%2d]", EXT_OBJ_INFO_P(obj, ExtObjRef));
 kal_print((kal_char*)buf);
 fwrite( buf, sizeof( char ), strlen(buf), fp_log );
  }

  if (EXT_OBJ_INFO_P(obj, isExtObj) == KAL_TRUE) 
  {
 sprintf(buf, "[EXTEND:%d]", EXT_OBJ_INFO_P(obj, ExtObjRef));
 kal_print((kal_char*)buf);
 fwrite( buf, sizeof( char ), strlen(buf), fp_log );
  }

  if (EXT_OBJ_INFO_P(obj, isReusedObj) == KAL_TRUE)
  {
 sprintf(buf, "[REUSED:%d]", EXT_OBJ_INFO_P(obj, ExtObjRef));
 kal_print((kal_char*)buf);
 fwrite( buf, sizeof( char ), strlen(buf), fp_log );
  }

  ASSERT( !((EXT_OBJ_INFO_P(obj, isExtObj) == KAL_TRUE) &&
(EXT_OBJ_INFO_P(obj, isReusedObj) == KAL_TRUE)));
#endif
  switch(obj->Type)
  {
 case EMS_TYPE_TEXT_FORMAT:
sprintf(buf, "Text format : length = %d\n", EMS_P(obj->data,text_format.textLength));
kal_print((kal_char*)buf);
fwrite( buf, sizeof( char ), strlen(buf), fp_log );
break;

 case EMS_TYPE_PIC:
sprintf(buf, "Picture: size=%d, (%d,%d,%d,%d)\n", EMS_P(obj->data,picture.pdu_length), 
   EMS_P(obj->data,picture.bitsPerPixel),
   EMS_P(obj->data,picture.hDim),
   EMS_P(obj->data,picture.vDim),
   EMS_P(obj->data,picture.attribute));
kal_print((kal_char*)buf);
fwrite( buf, sizeof( char ), strlen(buf), fp_log );
break;

 case EMS_TYPE_PREDEF_ANM:
sprintf(buf, "Predefined animation: no=%d\n", obj->PredefNo);
kal_print((kal_char*)buf);
fwrite( buf, sizeof( char ), strlen(buf), fp_log );
break;

 case EMS_TYPE_PREDEF_SND:
sprintf(buf, "Predefined sound: no=%d\n", obj->PredefNo);
kal_print((kal_char*)buf);
fwrite( buf, sizeof( char ), strlen(buf), fp_log );
break;
 case EMS_TYPE_USERDEF_SND:
sprintf(buf, "User defined sound: size=%d (%d)\n", EMS_P(obj->data,sound.pdu_length),
   EMS_P(obj->data,sound.attribute));
kal_print((kal_char*)buf);
fwrite( buf, sizeof( char ), strlen(buf), fp_log );
break;

 case EMS_TYPE_USERDEF_ANM:
sprintf(buf, "animation: size=%d, (%d,%d,%d,%d,%d)\n", EMS_P(obj->data,animation.pdu_length),
 EMS_P(obj->data,animation.bitsPerPixel),
 EMS_P(obj->data,animation.hDim),
 EMS_P(obj->data,animation.vDim),
 EMS_P(obj->data,animation.cFrame),
 EMS_P(obj->data,animation.attribute));
kal_print((kal_char*)buf);
fwrite( buf, sizeof( char ), strlen(buf), fp_log );
break;
 case EMS_TYPE_VCARD:
sprintf(buf, "vCard: size=%d (%d)\n", EMS_P(obj->data,vapp.pdu_length),
  EMS_P(obj->data,vapp.attribute));
kal_print((kal_char*)buf);
fwrite( buf, sizeof( char ), strlen(buf), fp_log );

break;
 case EMS_TYPE_VCALENDAR:
sprintf(buf, "vCard: size=%d (%d)\n", EMS_P(obj->data,vapp.pdu_length),
  EMS_P(obj->data,vapp.attribute));
kal_print((kal_char*)buf);
fwrite( buf, sizeof( char ), strlen(buf), fp_log );

break;

 default:
kal_print("invalid EMS object\n");
break;
  }
  obj = obj->next;
  i++;
   }/* while */

   kal_print("\ntext part: ========================\n");
   if(emsData->textLength > 0)
   {
  kal_uint16 i;

#ifdef __WIN32__
  if(emsData->dcs == SMSAL_DEFAULT_DCS)
  {
 for(i=0;i<emsData->textLength/2;i++)
 {
#ifdef EMS_LITTLE_ENDIAN
printf("0x%x ", *(emsData->textBuffer+2*i));
#else
printf("0x%x ", *(emsData->textBuffer+2*i+1));
#endif
if(((i+1)%10)==0) printf("\n");
 }//for
  }
  else
  {
 for(i=0;i<emsData->textLength;i++)
 {
printf("0x%x ", *(emsData->textBuffer+i));
if(((i+1)%10)==0) printf("\n");
 }
  }//dcs
#endif//__WIN32__
   }
   sprintf(buf, "\n================================================\n");
   kal_print((kal_char*)buf);
   fwrite( buf, sizeof( char ), strlen(buf), fp_log );

   kal_print("\n\n");

} /* end of EMSDataDump */

#ifdef __WIN32__
void DisplayTPUD(kal_uint8 numOfMsg, kal_uint8 *TPUD_p[], kal_uint8 TPUDLen[])
{
   kal_uint8 i;
   kal_uint16 j;

   printf("\nTotal Segments: %d\n", numOfMsg);

   for(i = 0 ; i < numOfMsg ; i++)
   {  
  printf("TPUD length: %d\n", TPUDLen[i]);

  for(j=0;j<TPUDLen[i];j++)
  {
 printf("%2x ", *(TPUD_p[i]+j)); 
 if((j%10)==0) 
printf("\n");
  }

  printf("\n");
   }
}
#endif//__WIN32__

void DisplayReminderInfo(EMSReminder *Reminder)
{
   kal_uint8 buf[50];

   sprintf(buf, "seg: %d\n", Reminder->requiredSegment);
   kal_print((kal_char*)buf);

   sprintf(buf, "remaining char: %d\n", Reminder->segRemainingOctet);
   kal_print((kal_char*)buf);

   sprintf(buf, "total remaining char: %d\n", Reminder->totalRemainingOctet);
   kal_print((kal_char*)buf);
}

void EMSTestFillString(kal_uint8 string[], kal_uint16 len, char *src)
{
   kal_uint16 i;
   kal_uint16 str_len;

   if (src != NULL)
   {
  str_len = strlen((char*)src);

  for(i=0;i< str_len;i++)
  {
#ifdef EMS_LITTLE_ENDIAN
 string[2*i] = src[i];   
 string[2*i+1] = 0;
#else
 string[2*i] = 0;
 string[2*i+1] = src[i];
#endif
  }
   }
   else
   {
  for(i=0;i< len ;i++)
  {
#ifdef EMS_LITTLE_ENDIAN
 string[i*2+1]=0;
 string[i*2]= (i % '0') + '0'; /* 0~9 */
#else
 string[i*2]=0;
 string[i*2+1]= (i % '0') + '0'; /* 0~9 */
#endif
  }
   }

}

/* 
 * add string : 160 characters 
 */
EMSTATUS EMSTestAddString(kal_uint16 numOfChar, EMSTextFormat *tf)
{   
   kal_uint16 i;  
   kal_uint8 *string;
   EMSTATUS ret;
 
#ifdef __WIN32__
   string = (kal_uint8*)malloc(numOfChar*2);
#else
   string = get_ctrl_buffer(numOfChar*2);
#endif
   
   for(i=0;i<numOfChar;i++)
   {
#ifdef EMS_LITTLE_ENDIAN
  string[i*2+1]=0;
  string[i*2]= (i % '0') + '0'; /* 0~9 */
#else
  string[i*2]=0;
  string[i*2+1]= (i % '0') + '0'; /* 0~9 */
#endif
   }

   ret = AddString(&emsData, string, numOfChar, tf);
 
#ifdef __WIN32__
   free(string);
#else
   free_ctrl_buffer(string);
#endif
 
   return ret;
}

kal_bool EMSReminderVerdict(EMSReminder *r1, EMSReminder *r2)
{   
   if(r1->isSegChange!=r2->isSegChange)
  return 1;

   if(r1->requiredSegment!=r2->requiredSegment)
  return 1;

   if(r1->segRemainingOctet!=r2->segRemainingOctet)
  return 1;

   if(r1->segUsableOctet!=r2->segUsableOctet)
  return 1;

   if(r1->totalRemainingOctet!=r2->totalRemainingOctet)
  return 1;

   return 0;//the same
}
   

void EMSResourceVerdict(void)
{
  // modify by PhoneSuite
  //kal_uint8 i ;
  int i;
  for(i = 0 ; i < EMS_MAX_EMS_DATA ; i++)
   {
  if(EMS_CNTX(isEMSDataUsed[i]) != KAL_FALSE)
 ASSERT (0);
   }
  return;
}

/*============================
  BASIC 
 *============================*/
typedef struct
   {
   kal_uint8   dcs;
   kal_uint16  numOfChar;
   EMSTATUSexpect_ret;
   EMSReminder Reminder;
} EMSTestBasicTextData_t;

/* 
 * normal text (no text format)
 */
EMSTestBasicTextData_t EMSTestBasicTextData[NUM_EMS_BASIC_TEXT_ITEM] = 
{
/*   DCS numOfCharStatus isSeg  required RemainingOctet UsableOctet  RemainingOctet
 Change Segment  per segper seg  total */

/* gsm 7-bit */
/* 1  */ {0, 10,   EMS_OK,{0,1,   150,   160, (MAX_TEXT_LEN_SEPTET -10)}  }, /* 10 GSM 7-bit characters */
/* 2  */ {0, 153,  EMS_OK,{0,1,   7, 160, (MAX_TEXT_LEN_SEPTET -153)} }, /* 153 GSM 7-bit characters */
/* 3  */ {0, 160,  EMS_OK,{0,1,   0, 160, (MAX_TEXT_LEN_SEPTET -160)} }, /* 160 GSM 7-bit characters */
/* 4  */ {0, 306,  EMS_OK,{1,2,   0, 153, (MAX_TEXT_LEN_SEPTET -306)} }, /* Long text: 306 (=153*2) GSM 7-bit characters */
/* 5  */ {0, 459,  EMS_OK,{1,3,   0, 153, (MAX_TEXT_LEN_SEPTET -459)} }, /* Long text: 459 (=153*3) GSM 7-bit characters */
/* 6  */ {0, MAX_TEXT_LEN_SEPTET , EMS_OK,{1,EMS_TEST_MAX_SEG ,0,153, 0} }, /* Long text: 612 (=153*4) GSM 7-bit characters */
/* 7  */ {0, 161,  EMS_OK,{1,2,   145,   153, (MAX_TEXT_LEN_SEPTET -161)} }, /* Long text: 161 GSM 7-bit characters */
/* 8  */ {0, 307,  EMS_OK,{1,3,   152,   153, (MAX_TEXT_LEN_SEPTET -307)} }, /* Long text: 307 (=153*2+1) GSM 7-bit characters */
/* 9  */ {0, 460,  EMS_OK,{1,4,   152,   153, (MAX_TEXT_LEN_SEPTET -460)} }, /* Long text: 460 (=153*3+1) GSM 7-bit characters */
/* 10 */ {0, MAX_TEXT_LEN_SEPTET+1,EMS_NO_SPACE,  {0,0,   0, 0,   0} }, /* Long text: 613 (=153*4+1) GSM 7-bit characters shall FAIL */

/* ucs2 */
/* 11 */ {8, 10,   EMS_OK,{0,1,   120,   140, (MAX_TEXT_LEN_OCTET -20)} }, /* 10 UCS2 characters */   
/* 12 */ {8, 70,   EMS_OK,{0,1,   0, 140, (MAX_TEXT_LEN_OCTET -140)} }, /* 70 UCS2 characters */   
/* 13 */ {8, 134,  EMS_OK,{1,2,   0, 134, (MAX_TEXT_LEN_OCTET -268)} }, /* Long text: 134 (=67*2) UCS2 characters */
/* 14 */ {8, 201,  EMS_OK,{1,3,   0, 134, (MAX_TEXT_LEN_OCTET -402)} }, /* Long text: 201 (=67*3) UCS2 characters */
/* 15 */ {8, MAX_CHAR_UCS2,EMS_OK,{1,EMS_TEST_MAX_SEG ,0,134, 0} }, /* Long text: 268 (=67*4) UCS2 characters */
/* 16 */ {8, 71,   EMS_OK,{1,2,   126,   134, (MAX_TEXT_LEN_OCTET -142)} }, /* Long text: 71 UCS2 characters */
/* 17 */ {8, 135,  EMS_OK,{1,3,   132,   134, (MAX_TEXT_LEN_OCTET -270)} }, /* Long text: 135 (=67*2+1) UCS2 characters */
/* 18 */ {8, 202,  EMS_OK,{1,4,   132,   134, (MAX_TEXT_LEN_OCTET -404)} }, /* Long text: 202 (=67*3+1) UCS2 characters */
/* 19 */ {8, MAX_CHAR_UCS2 +1, EMS_NO_SPACE,  {0,0,   0, 0,   0} }, /* Long text: 269 (=67*4+1) UCS2 characters */

};

/* 
 * formatted text (bold)
 */
#define NUM_EMS_BASIC_FORMATTED_TEXT_ITEM (16)
EMSTestBasicTextData_t EMSTestBasicFormattedTextData[NUM_EMS_BASIC_FORMATTED_TEXT_ITEM] = 
{
/*   DCS numOfChar Status isSeg  required RemainingOctet UsableOctet  RemainingOctet
  Change Segment  per segper seg  total */

/* gsm 7-bit */
/* 1  */ {0, 153,  EMS_OK,{0,1,   0, 160, (MAX_TEXT_LEN_SEPTET -160)} }, 
/* 2  */ {0, 294,  EMS_OK,{1,2,   0, 153, (MAX_TEXT_LEN_SEPTET -153*2)} }, 
/* 3  */ {0, 441,  EMS_OK,{1,3,   0, 153, (MAX_TEXT_LEN_SEPTET -153*3)} }, 
/* 4  */ {0, 588,  EMS_OK,{1,4,   0, 153, 0} }, 
/* 5  */ {0, 154,  EMS_OK,{1,2,   140,   153, (MAX_TEXT_LEN_SEPTET -153-13)} }, 
/* 6  */ {0, 295,  EMS_OK,{1,3,   146,   153, (MAX_TEXT_LEN_SEPTET -153*2-7)} }, 
/* 7  */ {0, 442,  EMS_OK,{1,4,   146,   153, (MAX_TEXT_LEN_SEPTET -153*3-7)} }, 
/* 8  */ {0, 589,  EMS_NO_SPACE,  {0,0,   0, 0,   0} }, 

/* ucs2 */
/* 9  */ {8, 67,   EMS_OK,{0,1,   0, 140, (MAX_TEXT_LEN_OCTET -140)} },
/* 10 */ {8, 128,  EMS_OK,{1,2,   1,/*!!*/   134, (MAX_TEXT_LEN_OCTET -134*2+1)} }, /* 1 octet not used per seg */
/* 11 */ {8, 192,  EMS_OK,{1,3,   1, 134, (MAX_TEXT_LEN_OCTET -134*3+1)} }, 
/* 12 */ {8, 256,  EMS_OK,{1,4,   1, 134, (MAX_TEXT_LEN_OCTET -134*4+1)} }, 
/* 13 */ {8, 68,   EMS_OK,{1,2,   (64-4)*2+1,134, (MAX_TEXT_LEN_OCTET -134-13)}}, 
/* 14 */ {8, 129,  EMS_OK,{1,3,   (64-1)*2+1,134, (MAX_TEXT_LEN_OCTET -134*2-7)} }, 
/* 15 */ {8, 193,  EMS_OK,{1,4,   (64-1)*2+1,134, (MAX_TEXT_LEN_OCTET -134*3-7)} }, 
/* 16 */ {8, 257,  EMS_NO_SPACE,  {0,0,   0, 0,   0} },   

}; 
 
void EMSTestBasicText(EMSTestBasicTextData_t *test_data)
{   
   EMSTATUS ret;

   InitializeEMSData(&emsData, test_data->dcs);

   ret = EMSTestAddString(test_data->numOfChar, NULL);

   ASSERT(ret==test_data->expect_ret);
   
   /* check verdicts */
   if(ret==EMS_OK)
   {
  ASSERT(0==EMSReminderVerdict(&emsData.Reminder, &test_data->Reminder));   
   }
   
   ReleaseEMSData(&emsData);
   EMSResourceVerdict();
}

void EMSTestBasicFormattedText(EMSTestBasicTextData_t *test_data)
{   
   EMSTATUS ret;
   EMSTextFormat tf;
   kal_uint8 numOfMsg;

   InitializeEMSData(&emsData, test_data->dcs);
   ResetTextFormat(&tf);
   tf.isBold = KAL_TRUE;

   ret = EMSTestAddString(test_data->numOfChar, &tf);

   ASSERT(ret==test_data->expect_ret);
   
   /* check verdicts */
   if(ret==EMS_OK)
   {
  ASSERT(0==EMSReminderVerdict(&emsData.Reminder, &test_data->Reminder));   

  ret=EMSPack(&emsData, 1, &numOfMsg, TPUD_p, TPUDLen);
  ASSERT(ret == EMS_OK);
  EMSDataDump(&emsData);

  if(ret == EMS_OK)
  {
 InitializeEMSData(&emsData1, 0);
 /* set dcs & udhi */
 emsData1.dcs = emsData.dcs;

 ret=EMSUnPack(&emsData1,emsData.udhi, numOfMsg, TPUD_p, TPUDLen);
 EMSDataDump(&emsData1);

 if(ret == EMS_OK)
 {
kal_uint8 isSame = EMSDataCmp(&emsData, &emsData1, 0);

ASSERT(isSame == 0);
 }

 ReleaseEMSData(&emsData1);
  }
   }
   ReleaseEMSData(&emsData);
   EMSResourceVerdict();
}

   
   
   
/*============================
  MIX
 *============================*/
void EMSTestMix5(void)
{   
   kal_uint16 i;
   kal_uint16 numOfChar=161;
   
   kal_uint8 string[500];
   
   EMSObjData objData;
   
   for(i=0;i<numOfChar;i++)
   {
  string[i*2]=0;
  string[i*2+1]= (i % '0') + '0'; /* 0~9 */
   }

   AddString(&emsData, string, numOfChar, NULL);

   objData.picture.vDim = 8; 
   objData.picture.hDim = 8;
   objData.picture.pdu_length = 8;
   objData.picture.pdu = var_pic1;
   objData.picture.bitsPerPixel = 1; /* mono picture */
   AddObject(&emsData, EMS_TYPE_PIC, &objData, 0);

   objData.animation.vDim = 8;
   objData.animation.hDim = 8;
   objData.animation.bitsPerPixel = 1;
   objData.animation.cFrame = 4;
   objData.animation.pdu_length = 32;  
   objData.animation.pdu = small_anm;
   AddObject(&emsData, EMS_TYPE_USERDEF_ANM, &objData, 0);

   objData.animation.vDim = 16;
   objData.animation.hDim = 16;
   objData.animation.bitsPerPixel = 1;
   objData.animation.cFrame = 4;
   objData.animation.pdu_length = 128;  
   objData.animation.pdu = large_anm;   
   AddObject(&emsData, EMS_TYPE_USERDEF_ANM, &objData, 0);
}

/* test EMS */
void EMSTestEditSeq2(void)
{
   EMSTATUS ret;
   kal_uint16 step;

   kal_uint8 string[10];   
   kal_uint8 numOfMsg;
   kal_uint8 TPUD[160];
   kal_uint8 TPUDLen[4];
   kal_uint8 *TPUD_p[4];

   TPUD_p[0] = TPUD;
   TPUD_p[1] = NULL;
   TPUD_p[2] = NULL;
   TPUD_p[3] = NULL;

   InitializeEMSData(&emsData, 0);

   EMSTestFillString(string, 3, "tst");
   ret=AddString(&emsData, string, 3, NULL); /* add "tst" */
   ASSERT(ret==EMS_OK);

   step = BackwardCurrentPosition(&emsData, 2);
   ASSERT(step == 2);

   EMSTestFillString(string, 1,"e");

   ret=AddString(&emsData, string, 1, NULL); /* add "e" */
   ASSERT(ret==EMS_OK);

   step = ForwardCurrentPosition(&emsData, 2);
   ASSERT(step==2);
  
   EMSTestFillString(string, 3," EMS");

   ret=AddString(&emsData, string, 4, NULL); /* add " EMS" */
   ASSERT(ret==EMS_OK);
  
   ret=EMSPack(&emsData,1, &numOfMsg, TPUD_p, TPUDLen);
   ASSERT(ret==EMS_OK);
   
   ASSERT(numOfMsg==1);
   ASSERT(TPUDLen[0]==8);
   
   TPUD[TPUDLen[0]]='\0';//for strcmp
   ASSERT(strcmp((char*)TPUD, "test EMS")==0);
  
   ReleaseEMSData(&emsData);
   EMSResourceVerdict();
}

/* 
 * anm:[anm]pic:[pic]snd:[snd]
 */
void EMSTestEditSeq3(void)
{
   EMSTATUS ret;
   kal_uint8 string[10];   
   EMSObjData objData;
   kal_uint8 numOfMsg;
   kal_uint8 TPUD[160];
   kal_uint8 TPUDLen[4];
   kal_uint8 *TPUD_p[4];
   
   TPUD_p[0] = TPUD;
   TPUD_p[1] = NULL;
   TPUD_p[2] = NULL;
   TPUD_p[3] = NULL;

   InitializeEMSData(&emsData, 0);
  
   EMSTestFillString(string, 4,"pic:");
   ret=AddString(&emsData, string, 4, NULL); /* add "pic:" */
   ASSERT(ret==EMS_OK);

   objData.picture.vDim = 16; 
   objData.picture.hDim = 16;
   objData.picture.pdu_length = 32;
   objData.picture.pdu = small_pic1;
   objData.picture.bitsPerPixel = 1; /* mono picture */
   ret=AddObject(&emsData, EMS_TYPE_PIC, &objData, 0);
   ASSERT(ret==EMS_OK);
  
   EMSTestFillString(string, 4,"snd:");
   
   ret=AddString(&emsData, string, 4, NULL); /* add "snd:" */
   ASSERT(ret==EMS_OK);
  
   /* PredefNo = 0 */
   ret=AddObject(&emsData, EMS_TYPE_PREDEF_SND, &objData, 0);
   ASSERT(ret==EMS_OK);
  
   ////////////////////////////////////
   // go to the beginning position
   ////////////////////////////////////
   ResetCurrentPosition(&emsData);
   
   EMSTestFillString(string, 4,"anm:");
   ret=AddString(&emsData, string, 4, NULL); /* add "anm:" */
   ASSERT(ret==EMS_OK);

   /* PredefNo = 0 */
   ret=AddObject(&emsData, EMS_TYPE_PREDEF_ANM, &objData, 0);
   ASSERT(ret==EMS_OK);

   ret=EMSPack(&emsData, 1,&numOfMsg, TPUD_p, TPUDLen);
   ASSERT(ret==EMS_OK);

   ASSERT(numOfMsg==1);   
   ASSERT(TPUDLen[0]==strlen("anm:pic:snd:")+/*text len */
  1+/*udhl*/ 
  4+/*predef anm*/
  4+/*predef snd*/
  35/*small pic*/
);

   //////////////////////
   // check object part
   //////////////////////
   ASSERT(TPUD[0]==4+4+35);
   ASSERT(TPUD[1]==EMS_PREDEF_ANM_IEI);
   ASSERT(TPUD[2]==2);//IEDL
   ASSERT(TPUD[3]==4);//offset
   ASSERT(TPUD[4]==0);//predef no
   ASSERT(TPUD[5]==EMS_SMALL_PIC_IEI);
   ASSERT(TPUD[6]==33);
   ASSERT(TPUD[7]==8);//offset
   ASSERT(TPUD[40]==EMS_PREDEF_SND_IEI);
   ASSERT(TPUD[41]==2);//IEDL
   ASSERT(TPUD[42]==12);//offset
   ASSERT(TPUD[43]==0);//predef no
   
   ////////////////////
   // check text part
   ////////////////////
   TPUD[TPUDLen[0]]='\0';//for strcmp
   ASSERT(strcmp((char*)&TPUD[1+4+4+35], "anm:pic:snd:")==0);
   
   ReleaseEMSData(&emsData);
   EMSResourceVerdict();
}

void EMSTestEditSeq4(void)
{
   kal_uint8 string[10];   
   
   EMSObjData objData;
   extern EMSData emsData;
   extern kal_uint8 small_pic1[32];

   InitializeEMSData(&emsData, 0);

   string[0] = 0;
   string[1] = 'a';
   string[2] = 0;
   string[3] = 'n';
   string[4] = 0;
   string[5] = 'm';   
   string[6] = 0;
   string[7] = ':';
   AddString(&emsData, string, 4, NULL); /* add "anm:" */
   
   //objData.animation.PredefNo = 0;  
   AddObject(&emsData, EMS_TYPE_PREDEF_ANM, &objData, 0);
  
   BackwardCurrentPosition(&emsData, 2);
 
   GoToEndPosition(&emsData);
 
   string[0] = 0;
   string[1] = 'p';
   string[2] = 0;
   string[3] = 'i';
   string[4] = 0;
   string[5] = 'c';   
   string[6] = 0;
   string[7] = ':';
   AddString(&emsData, string, 4, NULL); /* add "pic:" */

   objData.picture.vDim = 16; 
   objData.picture.hDim = 16;
   objData.picture.pdu_length = 32;
   objData.picture.pdu = small_pic1;
   objData.picture.bitsPerPixel = 1; /* mono picture */
   AddObject(&emsData, EMS_TYPE_PIC, &objData, 0);
 
   ReleaseEMSData(&emsData);
   EMSResourceVerdict();
}

void EMSTestEditSeq5(void)
{
   kal_uint16 n;
   EMSTATUS ret;
   kal_uint8 string[20];  
   EMSTextFormat tf;
   
   ////////////////
   // bold
   ////////////////
   InitializeEMSData(&emsData, 0);
  
   EMSTestFillString(string, 4,"bold");
   
   ResetTextFormat(&tf);
   tf.isBold = KAL_TRUE;
   
   ret=AddString(&emsData, string, strlen("bold"), &tf); /* add "bold" */
   ASSERT(ret==EMS_OK);

   n=BackwardCurrentPosition(&emsData, 2);
   ASSERT(n==2);
   
   ////////////////
   // italic
   ////////////////

   EMSTestFillString(string, 6,"italic");
   
  
   ResetTextFormat(&tf);
   tf.isItalic = KAL_TRUE;
   
   ret=AddString(&emsData, string, strlen("italic"), &tf); /* add "italic" */
   ASSERT(ret==EMS_OK);

   /* "bo" : bold style */
   ASSERT(emsData.listHead->OffsetToText==0);
   ASSERT(EMS_P(emsData.listHead->data,text_format.isBold)==KAL_TRUE);
   ASSERT(EMS_P(emsData.listHead->data,text_format.textLength)==strlen("bo")*2);
   
   /* "italic" : bold style */
   ASSERT(emsData.listHead->next->OffsetToText==strlen("bo")*2);
   ASSERT(EMS_P(emsData.listHead->next->data,text_format.isItalic)==KAL_TRUE);
   ASSERT(EMS_P(emsData.listHead->next->data,text_format.textLength)==strlen("italic")*2);

   /* "ld" : bold style */
   ASSERT(emsData.listHead->next->next->OffsetToText==strlen("boitalic")*2);
   ASSERT(EMS_P(emsData.listHead->next->next->data,text_format.isBold)==KAL_TRUE);
   ASSERT(EMS_P(emsData.listHead->next->next->data,text_format.textLength)==strlen("ld")*2);

   ReleaseEMSData(&emsData);
   EMSResourceVerdict();
}

void EMSTestEditSeq6(void)
{
   kal_uint16 n;
   EMSTATUS ret;
   kal_uint8string[20];  
   EMSTextFormat tf;
   
   ////////////////
   // bold
   ////////////////
   InitializeEMSData(&emsData, 0);

   EMSTestFillString(string, 4,"bold");
   
   ResetTextFormat(&tf);
   tf.isBold = KAL_TRUE;
   ret=AddString(&emsData, string, strlen("bold"), &tf); /* add "bold" */
   ASSERT(ret==EMS_OK);
   
   n=BackwardCurrentPosition(&emsData, 2);
   ASSERT(n==2);

   ////////////////
   // normal
   ////////////////
   EMSTestFillString(string, 6,"normal");
   ResetTextFormat(&tf);   
   ret=AddString(&emsData, string, 6, &tf); /* add "normal" */   
   ASSERT(ret==EMS_OK);
  
   /* "bo" : bold style */
   ASSERT(emsData.listHead->OffsetToText==0);
   ASSERT(EMS_P(emsData.listHead->data,text_format.isBold)==KAL_TRUE);
   ASSERT(EMS_P(emsData.listHead->data,text_format.textLength)==4);
   
   /* "ld" : bold style */
   ASSERT(emsData.listHead->next->OffsetToText==strlen("bonormal")*2);
   ASSERT(EMS_P(emsData.listHead->next->data,text_format.isBold)==KAL_TRUE);
   ASSERT(EMS_P(emsData.listHead->next->data,text_format.textLength)==4);

   ReleaseEMSData(&emsData);
   EMSResourceVerdict();
}


void EMSTestEditSeq7(void)
{
   kal_uint16 n;
   EMSTATUS ret;
   kal_uint8 string[20];   
   EMSObjData objData;
   EMSTextFormat tf;
   
   ////////////////
   // normal1
   ////////////////
   InitializeEMSData(&emsData, 0);
  
   EMSTestFillString(string, 7,"normal1");
 
   ret=AddString(&emsData, string, strlen("normal1"), NULL); /* add "normal1" */
   ASSERT(ret==EMS_OK);

   /////////////////
   // bold
   /////////////////
   EMSTestFillString(string, 4,"bold");
   
   ResetTextFormat(&tf);
   tf.isBold = KAL_TRUE;
   
   ret=AddString(&emsData, string, strlen("bold"), &tf); /* add "bold" */
   ASSERT(ret==EMS_OK);
   ASSERT(emsData.CurrentPosition.Object != NULL && emsData.CurrentPosition.Object->Type==EMS_TYPE_TEXT_FORMAT);
   ASSERT(emsData.CurrentTextFormatObj != NULL && EMS_P(emsData.CurrentTextFormatObj->data,text_format.isBold)==KAL_TRUE);
   ASSERT(emsData.CurrentTextFormatObj->OffsetToText==strlen("normal1")*2);
   ASSERT(emsData.CurrentTextFormatObj != NULL && EMS_P(emsData.CurrentTextFormatObj->data,text_format.textLength)==strlen("bold")*2);
   
   //////////////
   // small pic
   //////////////
   objData.picture.vDim = 16; 
   objData.picture.hDim = 16;
   objData.picture.pdu_length = 32;
   objData.picture.pdu = small_pic1;
   objData.picture.bitsPerPixel = 1; /* mono picture */
   ret=AddObject(&emsData, EMS_TYPE_PIC, &objData, 0);
   ASSERT(ret==EMS_OK);
   ASSERT(emsData.CurrentPosition.Object != NULL && emsData.CurrentPosition.Object->Type==EMS_TYPE_PIC);
   ASSERT(emsData.CurrentPosition.Object->OffsetToText==strlen("normal1bold")*2);
   ASSERT(emsData.CurrentTextFormatObj != NULL && EMS_P(emsData.CurrentTextFormatObj->data,text_format.isBold)==KAL_TRUE);
   ASSERT(emsData.CurrentTextFormatObj->OffsetToText==strlen("normal1")*2);
   ASSERT(emsData.CurrentTextFormatObj != NULL && EMS_P(emsData.CurrentTextFormatObj->data,text_format.textLength)==strlen("bold")*2);
   
   ////////////////
   // normal2
   ////////////////
   EMSTestFillString(string, 7,"normal2");
   ResetTextFormat(&tf);  
   ret=AddString(&emsData, string, strlen("normal2"), &tf); /* add "normal2" */
   ASSERT(ret==EMS_OK);
   ASSERT(emsData.CurrentTextFormatObj == NULL);
   ASSERT(emsData.CurrentPosition.Object != NULL && emsData.CurrentPosition.Object->Type==EMS_TYPE_PIC);
   
   ////////////////
   // underline
   ////////////////
   EMSTestFillString(string, 9, "underline");  
   ResetTextFormat(&tf);
   tf.isUnderline = KAL_TRUE;   
   ret=AddString(&emsData, string, strlen("underline"), &tf); /* add "underline" */ 
   ASSERT(ret==EMS_OK);
   ASSERT(emsData.CurrentPosition.Object != NULL && emsData.CurrentPosition.Object->Type==EMS_TYPE_TEXT_FORMAT);
   ASSERT(emsData.CurrentTextFormatObj != NULL && EMS_P(emsData.CurrentTextFormatObj->data,text_format.isUnderline)==KAL_TRUE);
   ASSERT(emsData.CurrentTextFormatObj->OffsetToText==strlen("normal1boldnormal1")*2);
   ASSERT(emsData.CurrentTextFormatObj != NULL && EMS_P(emsData.CurrentTextFormatObj->data,text_format.textLength)==strlen("underline")*2);

   /* between "bo" "ld" */
   n=BackwardCurrentPosition(&emsData, strlen("ldnormal2underline")+1);
   ASSERT(n==strlen("ldnormal2underline")+1);
  
   ////////////////
   // italic
   ////////////////
   EMSTestFillString(string, 6,"italic");
   
   ResetTextFormat(&tf);
   tf.isItalic = KAL_TRUE;
   
   ret=AddString(&emsData, string, strlen("italic"), &tf); /* add "italic" */
   ASSERT(ret==EMS_OK);
   ASSERT(emsData.CurrentPosition.Object != NULL && emsData.CurrentPosition.Object->Type==EMS_TYPE_TEXT_FORMAT);
   ASSERT(emsData.CurrentTextFormatObj != NULL && EMS_P(emsData.CurrentTextFormatObj->data,text_format.isItalic)==KAL_TRUE);
   ASSERT((kal_uint32*)emsData.CurrentPosition.Object == (kal_uint32*)emsData.CurrentTextFormatObj);
   ASSERT(emsData.CurrentTextFormatObj->OffsetToText==strlen("normal1bo")*2);
   ASSERT(emsData.CurrentTextFormatObj != NULL && EMS_P(emsData.CurrentTextFormatObj->data,text_format.textLength)==strlen("italic")*2);
  
   //objData.animation.PredefNo = 0;  
   ret=AddObject(&emsData, EMS_TYPE_PREDEF_ANM, &objData, 0);
   ASSERT(ret==EMS_OK);
   ASSERT(emsData.CurrentPosition.Object != NULL && emsData.CurrentPosition.Object->Type==EMS_TYPE_PREDEF_ANM);
   ASSERT(emsData.CurrentTextFormatObj != NULL && EMS_P(emsData.CurrentTextFormatObj->data,text_format.isItalic)==KAL_TRUE);
   
   n=BackwardCurrentPosition(&emsData, 1);
   ASSERT(n==1);
   ASSERT(emsData.CurrentPosition.Object != NULL && emsData.CurrentPosition.Object->Type==EMS_TYPE_TEXT_FORMAT);
   ASSERT((kal_uint32*)emsData.CurrentPosition.Object == (kal_uint32*)emsData.CurrentTextFormatObj);
   
   ////////////////////
   // remove "italic"
   ////////////////////
   n=CancelCurrentPosition(&emsData, 6);
   ASSERT(n==6);
   ASSERT(emsData.CurrentPosition.Object != NULL && emsData.CurrentPosition.Object->Type==EMS_TYPE_TEXT_FORMAT);
   ASSERT(emsData.CurrentTextFormatObj != NULL && EMS_P(emsData.CurrentTextFormatObj->data,text_format.isBold)==KAL_TRUE);
   ASSERT(emsData.CurrentTextFormatObj->OffsetToText==strlen("normal1")*2);
#ifdef OPTIMIZE_TXT_FMT
   ASSERT(emsData.CurrentTextFormatObj != NULL && EMS_P(emsData.CurrentTextFormatObj->data,text_format.textLength)==strlen("bold")*2);
   ASSERT(emsData.CurrentPosition.Object->next->Type==EMS_TYPE_PREDEF_ANM);
   ASSERT(emsData.CurrentPosition.Object->next->OffsetToText==strlen("normal1bo")*2);
#else
   ASSERT(emsData.CurrentTextFormatObj != NULL && EMS_P(emsData.CurrentTextFormatObj->data,text_format.textLength)==strlen("bo")*2);   
   ASSERT(emsData.CurrentPosition.Object->next->Type==EMS_TYPE_PREDEF_ANM);
   ASSERT(emsData.CurrentPosition.Object->next->OffsetToText==strlen("normal1bo")*2);
   ASSERT(emsData.CurrentTextFormatObj->next->next != NULL && EMS_P(emsData.CurrentTextFormatObj->next->next->data,text_format.textLength)==strlen("ld")*2);
#endif
   
   ReleaseEMSData(&emsData);
   EMSResourceVerdict();
}

void EMSTestEditSeq8(void)
{
   EMSTATUS ret;
   kal_uint8string[20];  
   EMSTextFormat tf;
   
   InitializeEMSData(&emsData, 0);

   EMSTestFillString(string, 4,"bold");
   
   ResetTextFormat(&tf);
   tf.isBold = KAL_TRUE;
   ret=AddString(&emsData, string, strlen("bold"), &tf); /* add "bold" */
   ASSERT(ret==EMS_OK);
   ASSERT(emsData.CurrentPosition.Object != NULL && emsData.CurrentPosition.Object->Type==EMS_TYPE_TEXT_FORMAT);
   ASSERT(emsData.CurrentTextFormatObj != NULL && EMS_P(emsData.CurrentTextFormatObj->data,text_format.isBold)==KAL_TRUE);
   ASSERT(emsData.CurrentTextFormatObj->OffsetToText==0);
   ASSERT(emsData.CurrentTextFormatObj != NULL && EMS_P(emsData.CurrentTextFormatObj->data,text_format.textLength)==strlen("bold")*2);
   ResetCurrentPosition(&emsData);

   EMSTestFillString(string, 6,"normal");
  
   ret=AddString(&emsData, string, strlen("normal"), NULL); /* add "normal" */   
   ASSERT(ret==EMS_OK);
   ASSERT(emsData.CurrentPosition.Object == NULL);
   ASSERT(emsData.CurrentTextFormatObj == NULL);
  
   ReleaseEMSData(&emsData);
   EMSResourceVerdict();
}

void EMSTestEditSeq9(void)
{
   EMSTATUS ret;
   kal_uint8 string[20];   
  
   InitializeEMSData(&emsData, 0);
 
   EMSTestFillString(string, 6,"normal");
   
   ret=AddString(&emsData, string, strlen("normal"), NULL); /* add "normal" */
   ASSERT(ret==EMS_OK); 
   ASSERT(emsData.Reminder.segRemainingOctet==(160-strlen("normal")));

   ret=EMSSetDCS(&emsData, SMSAL_UCS2_DCS);
   ASSERT(ret==EMS_OK);
   ASSERT(emsData.Reminder.segRemainingOctet==(140-strlen("normal")*2));
   
   ReleaseEMSData(&emsData);
   EMSResourceVerdict();
}

void EMSTestEditSeq10(void)
{
   EMSTATUS ret;
   kal_uint8 string[20];   
  
   InitializeEMSData(&emsData, 0);

   ret=EMSSetDCS(&emsData, SMSAL_UCS2_DCS);
   ASSERT(ret==EMS_OK);
   
   EMSTestFillString(string, 6,"normal");
   
   ret=AddString(&emsData, string, strlen("normal"), NULL); /* add "normal" */
   ASSERT(ret==EMS_OK);
   ASSERT(emsData.Reminder.segRemainingOctet==(140-strlen("normal")*2));

   ret=EMSSetDCS(&emsData, SMSAL_DEFAULT_DCS);
   ASSERT(ret==EMS_OK);
   ASSERT(emsData.Reminder.segRemainingOctet==(160-strlen("normal")));
   
   ReleaseEMSData(&emsData);
   EMSResourceVerdict();
}

/* 
 * input a long text formatted string (across sms boundary)
 * and backward to middle of the string and insert a predef sound
 *
 * NOTE that :
 * the text formatted string shall be split into two text format objects !!
 * 
 * orignial: bold1...bold2 (the same text format), after backward and insert sound:
 * 
 * bold1 [snd] ...   ..... bold2....
 * <-- seg1  ---->   <--- seg2 ---->
 */

/* 
 * whole formatted text can be fit into one seg if
 * the Sound Object is not taken accounted !!
 */
void EMSTestEditSeq11(void)
{
   EMSTATUS ret;
   kal_uint8 i;   
   EMSTextFormat tf;
   EMSObjData objdata;
   kal_uint8 pdu[128];
   kal_uint8 numOfMsg;   

   InitializeEMSData(&emsData, 0);   
  
   /* large anm: 128+3 bytes */
   objdata.animation.pdu=pdu;
   objdata.animation.pdu_length=128;
   objdata.animation.bitsPerPixel=1;
   objdata.animation.cFrame=4;
   objdata.animation.cRepeat=0;
   objdata.animation.Duration=0;
   objdata.animation.hDim=16;
   objdata.animation.vDim=16;
   ret=AddObject(&emsData, EMS_TYPE_USERDEF_ANM, &objdata, 0);
   ASSERT(ret==EMS_OK);

   ResetTextFormat(&tf);
   tf.isBold=KAL_TRUE;   
   ret=AddString(&emsData, string, 100, &tf); 
   ASSERT(ret==EMS_OK);  

   ret=AddObject(&emsData, EMS_TYPE_PREDEF_SND, NULL, 5);
   ASSERT(ret==EMS_OK);   
  
   ret=AddString(&emsData, string, 47, NULL); 
   ASSERT(ret==EMS_OK);  

   ret=EMSPack(&emsData, 1,&numOfMsg, TPUD_p, TPUDLen);
   ASSERT(ret==EMS_OK); 
   ASSERT(numOfMsg==3);

   /* verdict seg2 */
   ASSERT(TPUD_p[1][1]==0);/* concat iei */
   ASSERT(TPUD_p[1][2]==3);/* concat iedl */
   ASSERT(TPUD_p[1][4]==3);/* total seg */
   ASSERT(TPUD_p[1][5]==2);/* seg */
   ASSERT(TPUD_p[1][6]==0x0a); /* txt fmt iei */
   ASSERT(TPUD_p[1][7]==3); /* txt fmt iedl */
   ASSERT(TPUD_p[1][8]==0); /* offset */

#ifdef OPTIMIZE_TXT_FMT
   /* tricky point !! */
   ASSERT(TPUD_p[1][9]==142); /* length */

   ASSERT(TPUD_p[1][11]==0x0b); /* predef snd iei */
   ASSERT(TPUD_p[1][12]==2); /* predef snd iedl */
   ASSERT(TPUD_p[1][13]==100); /* offset */
   ASSERT(TPUD_p[1][14]==5); /* predefno */
#else
   /* tricky point !! */
   ASSERT(TPUD_p[1][9]==100); /* length */

   ASSERT(TPUD_p[1][11]==0x0b); /* predef snd iei */
   ASSERT(TPUD_p[1][12]==2); /* predef snd iedl */
   ASSERT(TPUD_p[1][13]==100); /* offset */
   ASSERT(TPUD_p[1][14]==5); /* predefno */
   ASSERT(TPUD_p[1][15]==0x0a); /* txt fmt iei */
   ASSERT(TPUD_p[1][16]==3); /* txt fmt iedl */
   ASSERT(TPUD_p[1][17]==100); /* offset */
   ASSERT(TPUD_p[1][18]==37); /* length */

   /* verdict seg3 */
   ASSERT(TPUD_p[2][1]==0);/* concat iei */
   ASSERT(TPUD_p[2][2]==3);/* concat iedl */
   ASSERT(TPUD_p[2][4]==3);/* total seg */
   ASSERT(TPUD_p[2][5]==3);/* seg */
   ASSERT(TPUD_p[2][6]==0x0a); /* txt fmt iei */
   ASSERT(TPUD_p[2][7]==3); /* txt fmt iedl */
   ASSERT(TPUD_p[2][8]==0); /* offset */
   ASSERT(TPUD_p[2][9]==10); /* length */
#endif

   ReleaseEMSData(&emsData);
   EMSResourceVerdict();
}

void EMSTestEditSeq12(void)
{
   EMSTATUS ret;
   EMSTextFormat tf;
   kal_uint16 i,n;
   kal_uint8 numOfMsg;

   InitializeEMSData(&emsData, 0);   
  
   ResetTextFormat(&tf);
   tf.isBold=KAL_TRUE;   
   ret=AddString(&emsData, string, 200, &tf); 
   ASSERT(ret==EMS_OK);
   
   n=BackwardCurrentPosition(&emsData, 10);
   ASSERT(n==10);

   ret=AddObject(&emsData, EMS_TYPE_PREDEF_SND, NULL, 3);
   ASSERT(ret==EMS_OK);   
  
   ret=EMSPack(&emsData, 1,&numOfMsg, TPUD_p, TPUDLen);
   ASSERT(ret==EMS_OK); 
   ASSERT(numOfMsg==2);

#ifdef EMS_TEST_VERDICT 
#undef EMS_TEST_VERDICT
#endif   

#define EMS_TEST_VERDICT(seg, len) /* verdict */\
   ASSERT(TPUD_p[seg][1]==0);/* concat iei */\
   ASSERT(TPUD_p[seg][2]==3);/* concat iedl */\
   ASSERT(TPUD_p[seg][4]==2);/* total seg */\
   ASSERT(TPUD_p[seg][5]==seg+1);/* seg */\
   ASSERT(TPUD_p[seg][6]==0x0a); /* txt fmt iei */\
   ASSERT(TPUD_p[seg][7]==3); /* txt fmt iedl */\
   ASSERT(TPUD_p[seg][8]==0); /* offset */\
   /* tricky point !! */\
   ASSERT(TPUD_p[seg][9]==len); /* length */

   EMS_TEST_VERDICT(0,147);

#ifdef OPTIMIZE_TXT_FMT
   EMS_TEST_VERDICT(1,53);
   ASSERT(TPUD_p[1][11]==0x0b); /* predef snd iei */
   ASSERT(TPUD_p[1][12]==2); /* predef snd iedl */
   ASSERT(TPUD_p[1][13]==43); /* offset */
   ASSERT(TPUD_p[1][14]==3); /* predefno */
#else

   EMS_TEST_VERDICT(1,43);
   ASSERT(TPUD_p[1][11]==0x0b); /* predef snd iei */
   ASSERT(TPUD_p[1][12]==2); /* predef snd iedl */
   ASSERT(TPUD_p[1][13]==43); /* offset */
   ASSERT(TPUD_p[1][14]==3); /* predefno */
   ASSERT(TPUD_p[1][15]==0x0a); /* txt fmt iei */\
   ASSERT(TPUD_p[1][16]==3); /* txt fmt iedl */\
   ASSERT(TPUD_p[1][17]==43); /* offset */\
   ASSERT(TPUD_p[1][18]==10); /* length */
#endif 

   ReleaseEMSData(&emsData);
   EMSResourceVerdict();
}

/* split text format */
void EMSTestEditSeq13(void)
{
	EMSTextFormat tf;	
	kal_uint8 str[10];
   EMSTATUS ret;
   kal_uint16 n;

   /////////////////////////////////////////////
   // case 1 : 1 <snd> 45   (145: EMS_ALIGN_CENTER)
   //   ^23 (italic&EMS_ALIGN_CENTER)
   /////////////////////////////////////////////

   InitializeEMSData(&emsData, 0);
   
   // Text 1
	ResetTextFormat(&tf);  
	tf.Alignment = EMS_ALIGN_CENTER;
  	ret = AddString(&emsData, str, 1, &tf);
   ASSERT(ret==EMS_OK);
   
#ifdef EMS_TEST_VERDICT 
#undef EMS_TEST_VERDICT
#endif   

#define EMS_TEST_VERDICT(len,type_,value_)  ASSERT(emsData.CurrentTextFormatObj != NULL &&\
  emsData.CurrentTextFormatObj->data->text_format.type_==value_ &&\
  emsData.CurrentTextFormatObj->data->text_format.textLength==len)
	   
	ret=AddObject(&emsData, EMS_TYPE_PREDEF_SND, NULL, 2);
   ASSERT(ret==EMS_OK);
   ASSERT(emsData.CurrentPosition.Object != NULL && 
  emsData.CurrentPosition.Object->Type == EMS_TYPE_PREDEF_SND);
   EMS_TEST_VERDICT(2,Alignment,EMS_ALIGN_CENTER);

	// Text [45]
	ResetTextFormat(&tf);  
	tf.Alignment = EMS_ALIGN_CENTER;
  	ret = AddString(&emsData, str, 2, &tf);
   ASSERT(ret==EMS_OK);

#ifdef OPTIMIZE_TXT_FMT
   ASSERT(emsData.CurrentPosition.Object != NULL && 
  emsData.CurrentPosition.Object->Type == EMS_TYPE_PREDEF_SND);
   EMS_TEST_VERDICT(6,Alignment,EMS_ALIGN_CENTER);
#else
   EMS_TEST_VERDICT(4,Alignment,EMS_ALIGN_CENTER);  
#endif
	
	n=BackwardCurrentPosition(&emsData, 3);
   ASSERT(n==3);
   
	// Add [23]
	ResetTextFormat(&tf);
	tf.Alignment = EMS_ALIGN_CENTER;
	tf.isBold = KAL_TRUE;  
   ret = AddString(&emsData, str, 2, &tf);
   ASSERT(ret==EMS_OK);
   ASSERT((kal_uint32*)emsData.CurrentPosition.Object == (kal_uint32*)emsData.CurrentTextFormatObj);
   EMS_TEST_VERDICT(4,Alignment,EMS_ALIGN_CENTER);
   EMS_TEST_VERDICT(4,isBold,KAL_TRUE);
   ASSERT(emsData.CurrentPosition.Object->OffsetToText==2);
   ASSERT(emsData.CurrentPosition.Object->prev->Type==EMS_TYPE_TEXT_FORMAT);
   ASSERT(emsData.CurrentPosition.Object->prev->data->text_format.textLength==2);
   ASSERT(emsData.CurrentPosition.Object->prev->OffsetToText==0);
   ASSERT(emsData.CurrentPosition.Object->next->Type==EMS_TYPE_PREDEF_SND);
   ASSERT(emsData.CurrentPosition.Object->next->OffsetToText==6);
   ASSERT(emsData.CurrentPosition.Object->next->next->Type==EMS_TYPE_TEXT_FORMAT);
   ASSERT(emsData.CurrentPosition.Object->next->next->OffsetToText==6);
   ASSERT(emsData.CurrentPosition.Object->next->next->data->text_format.textLength==4);
	ReleaseEMSData(&emsData);

   /////////////////////////////////////////////
   // case 2 : 1 <snd> 45   (145: EMS_ALIGN_CENTER)
   // ^23 (italic&EMS_ALIGN_CENTER)
   /////////////////////////////////////////////

   InitializeEMSData(&emsData, 0);
   
   // Text 1
	ResetTextFormat(&tf);  
	tf.Alignment = EMS_ALIGN_CENTER;
  	ret = AddString(&emsData, str, 1, &tf);
   ASSERT(ret==EMS_OK);
   
#ifdef EMS_TEST_VERDICT 
#undef EMS_TEST_VERDICT
#endif   

#define EMS_TEST_VERDICT(len,type_,value_)  ASSERT(emsData.CurrentTextFormatObj != NULL &&\
  emsData.CurrentTextFormatObj->data->text_format.type_==value_ &&\
  emsData.CurrentTextFormatObj->data->text_format.textLength==len)
	   
	ret=AddObject(&emsData, EMS_TYPE_PREDEF_SND, NULL, 2);
   ASSERT(ret==EMS_OK);
   ASSERT(emsData.CurrentPosition.Object != NULL && 
  emsData.CurrentPosition.Object->Type == EMS_TYPE_PREDEF_SND);
   EMS_TEST_VERDICT(2,Alignment,EMS_ALIGN_CENTER);

	// Text [45]
	ResetTextFormat(&tf);  
	tf.Alignment = EMS_ALIGN_CENTER;
  	ret = AddString(&emsData, str, 2, &tf);
   ASSERT(ret==EMS_OK);

#ifdef OPTIMIZE_TXT_FMT
   ASSERT(emsData.CurrentPosition.Object != NULL && 
  emsData.CurrentPosition.Object->Type == EMS_TYPE_PREDEF_SND);
   EMS_TEST_VERDICT(6,Alignment,EMS_ALIGN_CENTER);
#else
   EMS_TEST_VERDICT(4,Alignment,EMS_ALIGN_CENTER);
#endif
	
	n=BackwardCurrentPosition(&emsData, 2);
   ASSERT(n==2);
   
	// Add [23]
	ResetTextFormat(&tf);
	tf.Alignment = EMS_ALIGN_CENTER;
	tf.isBold = KAL_TRUE;  
   ret = AddString(&emsData, str, 2, &tf);
   ASSERT(ret==EMS_OK);
   ASSERT((kal_uint32*)emsData.CurrentPosition.Object == (kal_uint32*)emsData.CurrentTextFormatObj);
   EMS_TEST_VERDICT(4,Alignment,EMS_ALIGN_CENTER);
   EMS_TEST_VERDICT(4,isBold,KAL_TRUE);
   ASSERT(emsData.CurrentPosition.Object->OffsetToText==2);
   ASSERT(emsData.CurrentPosition.Object->prev->prev->Type==EMS_TYPE_TEXT_FORMAT);
   ASSERT(emsData.CurrentPosition.Object->prev->prev->data->text_format.textLength==2);
   ASSERT(emsData.CurrentPosition.Object->prev->prev->OffsetToText==0);
   ASSERT(emsData.CurrentPosition.Object->prev->Type==EMS_TYPE_PREDEF_SND);
   ASSERT(emsData.CurrentPosition.Object->prev->OffsetToText==2);
   ASSERT(emsData.CurrentPosition.Object->next->Type==EMS_TYPE_TEXT_FORMAT);
   ASSERT(emsData.CurrentPosition.Object->next->OffsetToText==6);
   ASSERT(emsData.CurrentPosition.Object->next->data->text_format.textLength==4);

   ReleaseEMSData(&emsData);


   /////////////////////////////////////////////
   // case 3 : 1 <snd> 4 5   (145: EMS_ALIGN_CENTER)
   //   ^23 (italic&EMS_ALIGN_CENTER)
   /////////////////////////////////////////////

   InitializeEMSData(&emsData, 0);
   
   // Text 1
	ResetTextFormat(&tf);  
	tf.Alignment = EMS_ALIGN_CENTER;
  	ret = AddString(&emsData, str, 1, &tf);
   ASSERT(ret==EMS_OK);
   
#ifdef EMS_TEST_VERDICT 
#undef EMS_TEST_VERDICT
#endif   

#define EMS_TEST_VERDICT(len,type_,value_)  ASSERT(emsData.CurrentTextFormatObj != NULL &&\
  emsData.CurrentTextFormatObj->data->text_format.type_==value_ &&\
  emsData.CurrentTextFormatObj->data->text_format.textLength==len)
	   
	ret=AddObject(&emsData, EMS_TYPE_PREDEF_SND, NULL, 2);
   ASSERT(ret==EMS_OK);
   ASSERT(emsData.CurrentPosition.Object != NULL && 
  emsData.CurrentPosition.Object->Type == EMS_TYPE_PREDEF_SND);
   EMS_TEST_VERDICT(2,Alignment,EMS_ALIGN_CENTER);

	// Text [45]
	ResetTextFormat(&tf);  
	tf.Alignment = EMS_ALIGN_CENTER;
  	ret = AddString(&emsData, str, 2, &tf);
   ASSERT(ret==EMS_OK);

#ifdef OPTIMIZE_TXT_FMT
   ASSERT(emsData.CurrentPosition.Object != NULL && 
  emsData.CurrentPosition.Object->Type == EMS_TYPE_PREDEF_SND);
   EMS_TEST_VERDICT(6,Alignment,EMS_ALIGN_CENTER);
#else
   EMS_TEST_VERDICT(4,Alignment,EMS_ALIGN_CENTER);
#endif
	
	n=BackwardCurrentPosition(&emsData, 1);
   ASSERT(n==1);
   
	// Add [23]
	ResetTextFormat(&tf);
	tf.Alignment = EMS_ALIGN_CENTER;
	tf.isBold = KAL_TRUE;  
   ret = AddString(&emsData, str, 2, &tf);
   ASSERT(ret==EMS_OK);
   ASSERT((kal_uint32*)emsData.CurrentPosition.Object == (kal_uint32*)emsData.CurrentTextFormatObj);
   EMS_TEST_VERDICT(4,Alignment,EMS_ALIGN_CENTER);
   EMS_TEST_VERDICT(4,isBold,KAL_TRUE);
   ASSERT(emsData.CurrentPosition.Object->OffsetToText==4);

#ifdef OPTIMIZE_TXT_FMT
   ASSERT(emsData.CurrentPosition.Object->prev->prev->Type==EMS_TYPE_TEXT_FORMAT);
   ASSERT(emsData.CurrentPosition.Object->prev->prev->data->text_format.textLength==4);
   ASSERT(emsData.CurrentPosition.Object->prev->prev->OffsetToText==0);
   ASSERT(emsData.CurrentPosition.Object->prev->Type==EMS_TYPE_PREDEF_SND);
   ASSERT(emsData.CurrentPosition.Object->prev->OffsetToText==2);
   ASSERT(emsData.CurrentPosition.Object->next->Type==EMS_TYPE_TEXT_FORMAT);
   ASSERT(emsData.CurrentPosition.Object->next->OffsetToText==8);
   ASSERT(emsData.CurrentPosition.Object->next->data->text_format.textLength==2);
#else
   ASSERT(emsData.CurrentPosition.Object->prev->Type==EMS_TYPE_TEXT_FORMAT);
   ASSERT(emsData.CurrentPosition.Object->prev->data->text_format.textLength==2);
   ASSERT(emsData.CurrentPosition.Object->prev->OffsetToText==2);
   ASSERT(emsData.CurrentPosition.Object->prev->prev->Type==EMS_TYPE_PREDEF_SND);
   ASSERT(emsData.CurrentPosition.Object->prev->prev->OffsetToText==2);
   ASSERT(emsData.CurrentPosition.Object->prev->prev->prev->Type==EMS_TYPE_TEXT_FORMAT);
   ASSERT(emsData.CurrentPosition.Object->prev->prev->prev->data->text_format.textLength==2);
   ASSERT(emsData.CurrentPosition.Object->prev->prev->prev->OffsetToText==0);
   ASSERT(emsData.CurrentPosition.Object->next->Type==EMS_TYPE_TEXT_FORMAT);
   ASSERT(emsData.CurrentPosition.Object->next->OffsetToText==8);
   ASSERT(emsData.CurrentPosition.Object->next->data->text_format.textLength==2);
#endif

   ReleaseEMSData(&emsData);
   
   EMSResourceVerdict();
}


void EMSTestEditSeq14(void)
{
	EMSTextFormat tf;	
	kal_uint8 str[10];
   EMSTATUS ret;
   kal_uint16 n;
   
   InitializeEMSData(&emsData, 0);
   
   // Text 1
	ResetTextFormat(&tf);  
	tf.Alignment = EMS_ALIGN_CENTER;
  	ret = AddString(&emsData, str, 3, &tf);
   ASSERT(ret==EMS_OK);   
  
   n=BackwardCurrentPosition(&emsData, 1);
   ASSERT(n==1);

	ret=AddObject(&emsData, EMS_TYPE_PREDEF_SND, NULL, 2);
   
	n=ForwardCurrentPosition(&emsData, 1);
   ASSERT(n==1);

   ret=AddObject(&emsData, EMS_TYPE_PREDEF_ANM, NULL, 2);

   ResetCurrentPosition(&emsData);

#ifdef EMS_TEST_VERDICT 
#undef EMS_TEST_VERDICT
#endif   

#define EMS_TEST_VERDICT(off,_type)  ASSERT(emsData.CurrentTextFormatObj != NULL &&\
  emsData.CurrentPosition.OffsetToText==off&&\
  emsData.CurrentPosition.Object->Type==_type)

   n=ForwardCurrentPosition(&emsData, 1);
   ASSERT(n==1);
   EMS_TEST_VERDICT(2,EMS_TYPE_TEXT_FORMAT);

   n=ForwardCurrentPosition(&emsData, 1);
   ASSERT(n==1);
   EMS_TEST_VERDICT(4,EMS_TYPE_TEXT_FORMAT);

   n=ForwardCurrentPosition(&emsData, 1);
   ASSERT(n==1);
   EMS_TEST_VERDICT(4,EMS_TYPE_PREDEF_SND);

   n=ForwardCurrentPosition(&emsData, 1);
   ASSERT(n==1);
#ifdef OPTIMIZE_TXT_FMT
   EMS_TEST_VERDICT(6,EMS_TYPE_PREDEF_SND);
#else
   EMS_TEST_VERDICT(6,EMS_TYPE_TEXT_FORMAT);
#endif

   n=ForwardCurrentPosition(&emsData, 1);
   ASSERT(n==1);
   EMS_TEST_VERDICT(6,EMS_TYPE_PREDEF_ANM);

   ReleaseEMSData(&emsData);
   EMSResourceVerdict();
}

/* 
 * 123<snd1><snd2> ... where
 * a. "123..." is formatted text
 * b. snd1, snd2 OffsetToText are the same
 */
void EMSTestEditSeq15()
{
   EMSData emsData;
   EMSTATUS ret;
   EMSTextFormat tf;
   EMSObjData data1 ;
   kal_uint8 pdu[127];
   kal_uint8 numOfMsg;   
   kal_uint8 i;
   kal_uint16 n;


   InitializeEMSData(&emsData, 0);

   // Formatted Text
   ResetTextFormat(&tf);
   tf.isItalic = KAL_TRUE;   
   ret = AddString(&emsData, string, 200, &tf);
   ASSERT(ret==EMS_OK);

   n=BackwardCurrentPosition(&emsData, 100);
   ASSERT(n==100);

   // predef sound   
   ret = AddObject(&emsData, EMS_TYPE_PREDEF_SND, NULL, 6);
   ASSERT(ret==EMS_OK);
  
   // Melody
   data1.sound.pdu_length = 127;
   data1.sound.pdu = pdu;
   ret = AddObject(&emsData, EMS_TYPE_USERDEF_SND, &data1, 0);
   ASSERT(ret==EMS_OK);
	
   ret=EMSPack(&emsData, 1,&numOfMsg, TPUD_p, TPUDLen);
   ASSERT(ret==EMS_OK);
   ASSERT(numOfMsg==3);

   /* verdict seg1 , 100 formatted text + 1 predef snd */
   ASSERT(TPUD_p[0][0]==14);/* udhl */
   ASSERT(TPUD_p[0][1]==0);/* concat iei */
   ASSERT(TPUD_p[0][2]==3);/* concat iedl */
   ASSERT(TPUD_p[0][4]==3);/* total seg */
   ASSERT(TPUD_p[0][5]==1);/* seg */
   ASSERT(TPUD_p[0][6]==0x0a); /* txt fmt iei */
   ASSERT(TPUD_p[0][7]==3); /* txt fmt iedl */
   ASSERT(TPUD_p[0][8]==0); /* offset */
   ASSERT(TPUD_p[0][9]==100); /* txt fmt mode */
   ASSERT(TPUD_p[0][10]==0x23); /* txt fmt mode */
   ASSERT(TPUD_p[0][11]==0x0b); /* predef snd iei */
   ASSERT(TPUD_p[0][12]==2); /* predef snd iedl */
   ASSERT(TPUD_p[0][13]==100); /* offset */
   ASSERT(TPUD_p[0][14]==6); /* predef no */

   /* verdict seg2, user-defined snd */
   ASSERT(TPUD_p[1][0]==135);/* udhl */
   ASSERT(TPUD_p[1][1]==0);/* concat iei */
   ASSERT(TPUD_p[1][2]==3);/* concat iedl */
   ASSERT(TPUD_p[1][4]==3);/* total seg */
   ASSERT(TPUD_p[1][5]==2);/* seg */
   ASSERT(TPUD_p[1][6]==0x0c); /* usr-def snd iei */
   ASSERT(TPUD_p[1][7]==128); /* usr-def snd iedl */
   ASSERT(TPUD_p[1][8]==0); /* offset */
   
   /* verdict seg3 */
   ASSERT(TPUD_p[2][0]==10);/* udhl */
   ASSERT(TPUD_p[2][1]==0);/* concat iei */
   ASSERT(TPUD_p[2][2]==3);/* concat iedl */
   ASSERT(TPUD_p[2][4]==3);/* total seg */
   ASSERT(TPUD_p[2][5]==3);/* seg */
   ASSERT(TPUD_p[2][6]==0x0a); /* txt fmt iei */
   ASSERT(TPUD_p[2][7]==3); /* txt fmt iedl */
   ASSERT(TPUD_p[2][8]==0); /* offset */
   ASSERT(TPUD_p[2][9]==100); /* txt fmt mode */
   ASSERT(TPUD_p[2][10]==0x23); /* txt fmt mode */
  
   ReleaseEMSData(&emsData);
   EMSResourceVerdict();
}

/*
 * "bold..." "nor ^ mal..."
 * "bold^..." "nor ^ mal..."  
 */
void EMSTestEditSeq16()
{
   EMSData emsData;
   EMSTATUS ret;
   EMSTextFormat tf;   
   kal_uint8 str[50]; 
   kal_uint16 n, text_len=20*2;
   kal_uint8 round;
   
   //////////////////////////////
   // round = 0 : backward
   // round = 1 : cancel
   //////////////////////////////
   for(round = 0 ; round < 2 ; round++)
   {
  InitializeEMSData(&emsData, 0);

  // Formatted Text
  ResetTextFormat(&tf);
  tf.isItalic = KAL_TRUE;   
  ret = AddString(&emsData, str, 20, &tf);
  ASSERT(ret==EMS_OK);

  // normal text
  ResetTextFormat(&tf);
  ret = AddString(&emsData, str, 20, &tf);
  ASSERT(ret==EMS_OK);
  ASSERT(emsData.CurrentPosition.Object != NULL &&
 IS_TXT_FMT_OBJ(emsData.CurrentPosition.Object));
  ASSERT(emsData.CurrentTextFormatObj == NULL);

  if(round == 0)
 n = BackwardCurrentPosition(&emsData, 25);
  else 
  {
 n = CancelCurrentPosition(&emsData, 25);
 text_len -= 5*2;
  }

  ASSERT(n==25);

  ASSERT(emsData.CurrentPosition.Object != NULL &&
 IS_TXT_FMT_OBJ(emsData.CurrentPosition.Object));

  ASSERT(emsData.CurrentTextFormatObj != NULL);
  ASSERT(emsData.CurrentTextFormatObj->data->text_format.textLength==text_len);  

  ReleaseEMSData(&emsData);
   }   

   EMSResourceVerdict();
}

void EMSTestEditSeq17()
{
   EMSData emsData;
   EMSTATUS ret;
   EMSTextFormat tf;   
   kal_uint8 str[50]; 
   kal_uint16 n;
   EMSObject *obj;
  
   InitializeEMSData(&emsData, 0);

   // Formatted Text (italic)
   ResetTextFormat(&tf);
   tf.isItalic = KAL_TRUE;   
   ret = AddString(&emsData, str, 20, &tf);
   ASSERT(ret==EMS_OK);

   // Formatted Text (bold)
   ResetTextFormat(&tf);
   tf.isBold = KAL_TRUE;   
   ret = AddString(&emsData, str, 20, &tf);
   ASSERT(ret==EMS_OK);
   

#ifdef EMS_TEST_VERDICT 
#undef EMS_TEST_VERDICT
#endif   

#define EMS_TEST_VERDICT(obj,style_type)   ASSERT( obj != NULL && \
  obj->Type == EMS_TYPE_TEXT_FORMAT &&\
  obj->data->text_format.style_type == KAL_TRUE);

   obj = emsData.listHead;
   EMS_TEST_VERDICT(obj,isItalic);

   obj = obj->next;
   EMS_TEST_VERDICT(obj,isBold);

   obj = obj->next;
   ASSERT(obj == NULL);

   ReleaseEMSData(&emsData);
   EMSResourceVerdict();
}

void EMSTestEditSeq18()
{
   EMSData emsData;
   EMSTATUS ret;
   EMSTextFormat tf;   
   kal_uint16 n;
  
   kal_uint8 numOfMsg;   
   kal_uint8 i;
   

   InitializeEMSData(&emsData, 0);

   // Formatted Text (italic)
   ResetTextFormat(&tf);
   tf.isItalic = KAL_TRUE;   
   ret = AddString(&emsData, string, 291, &tf);
   ASSERT(ret==EMS_OK);

   ret=AddObject(&emsData, EMS_TYPE_PREDEF_SND, NULL, 2);
   ASSERT(ret==EMS_OK);

   // Formatted Text (italic)   
   ret = AddString(&emsData, string, 176, &tf);
   ASSERT(ret==EMS_OK);

   // Formatted Text (bold)
   ResetTextFormat(&tf);
   tf.isBold = KAL_TRUE;   
   ret = AddString(&emsData, string, 34, &tf);
   ASSERT(ret==EMS_OK);   

   ret=EMSPack(&emsData,1, &numOfMsg, TPUD_p, TPUDLen);
   ASSERT(ret==EMS_OK);
   ASSERT(numOfMsg==4);

   /* verdict seg1 , 147 formatted text(1) */
   ASSERT(TPUD_p[0][0]==10);/* udhl */
   ASSERT(TPUD_p[0][1]==0);/* concat iei */
   ASSERT(TPUD_p[0][2]==3);/* concat iedl */
   ASSERT(TPUD_p[0][4]==4);/* total seg */
   ASSERT(TPUD_p[0][5]==1);/* seg */
   ASSERT(TPUD_p[0][6]==0x0a); /* txt fmt iei */
   ASSERT(TPUD_p[0][7]==3); /* txt fmt iedl */
   ASSERT(TPUD_p[0][8]==0); /* offset */
   ASSERT(TPUD_p[0][9]==147); /* txt fmt len */
   ASSERT(TPUD_p[0][10]==0x23); /* txt fmt mode */
   
   /* verdict seg2 , 144 formatted text(1) */
   ASSERT(TPUD_p[1][0]==10);/* udhl */
   ASSERT(TPUD_p[1][1]==0);/* concat iei */
   ASSERT(TPUD_p[1][2]==3);/* concat iedl */
   ASSERT(TPUD_p[1][4]==4);/* total seg */
   ASSERT(TPUD_p[1][5]==2);/* seg */
   ASSERT(TPUD_p[1][6]==0x0a); /* txt fmt iei */
   ASSERT(TPUD_p[1][7]==3); /* txt fmt iedl */
   ASSERT(TPUD_p[1][8]==0); /* offset */
   ASSERT(TPUD_p[1][9]==144); /* txt fmt len */
   ASSERT(TPUD_p[1][10]==0x23); /* txt fmt mode */

   /* verdict seg3 , 142 formatted text(1) + 1 predef snd */
   ASSERT(TPUD_p[2][0]==14);/* udhl */
   ASSERT(TPUD_p[2][1]==0);/* concat iei */
   ASSERT(TPUD_p[2][2]==3);/* concat iedl */
   ASSERT(TPUD_p[2][4]==4);/* total seg */
   ASSERT(TPUD_p[2][5]==3);/* seg */
   ASSERT(TPUD_p[2][6]==0x0b); /* predef snd iei */
   ASSERT(TPUD_p[2][7]==2); /* predef snd iedl */
   ASSERT(TPUD_p[2][8]==0); /* offset */
   ASSERT(TPUD_p[2][9]==2); /* predef no */
   ASSERT(TPUD_p[2][10]==0x0a); /* txt fmt iei */
   ASSERT(TPUD_p[2][11]==3); /* txt fmt iedl */
   ASSERT(TPUD_p[2][12]==0); /* offset */
   ASSERT(TPUD_p[2][13]==142); /* txt fmt len */
   ASSERT(TPUD_p[2][14]==0x23); /* txt fmt mode */

   /* verdict seg4 , 34 formatted text(1), 34 formatted text(2) */
   ASSERT(TPUD_p[3][0]==15);/* udhl */
   ASSERT(TPUD_p[3][1]==0);/* concat iei */
   ASSERT(TPUD_p[3][2]==3);/* concat iedl */
   ASSERT(TPUD_p[3][4]==4);/* total seg */
   ASSERT(TPUD_p[3][5]==4);/* seg */
   ASSERT(TPUD_p[3][6]==0x0a); /* txt fmt iei */
   ASSERT(TPUD_p[3][7]==3); /* txt fmt iedl */
   ASSERT(TPUD_p[3][8]==0); /* offset */
   ASSERT(TPUD_p[3][9]==34); /* txt fmt len */
   ASSERT(TPUD_p[3][10]==0x23); /* txt fmt mode */
   ASSERT(TPUD_p[3][11]==0x0a); /* txt fmt iei */
   ASSERT(TPUD_p[3][12]==3); /* txt fmt iedl */
   ASSERT(TPUD_p[3][13]==34); /* offset */
   ASSERT(TPUD_p[3][14]==34); /* txt fmt len */
   ASSERT(TPUD_p[3][15]==0x13); /* txt fmt mode */

   ReleaseEMSData(&emsData);
   EMSResourceVerdict();
}

void EMSTestEditSeq19()
{
   EMSData emsData;
   EMSTATUS ret;
   EMSTextFormat tf;   
   kal_uint16 n;  
  
   
   InitializeEMSData(&emsData, 0);

   // Formatted Text (italic)
   ResetTextFormat(&tf);
   tf.isItalic = KAL_TRUE;   
   ret = AddString(&emsData, string, 291, &tf);
   ASSERT(ret==EMS_OK);

   // Formatted Text (bold)
   ResetTextFormat(&tf);
   tf.isBold = KAL_TRUE;   
   ret = AddString(&emsData, string, 58, &tf);
   ASSERT(ret==EMS_OK);   

   n=BackwardCurrentPosition(&emsData, 133);
   ASSERT(n==133);

   ret=AddObject(&emsData, EMS_TYPE_PREDEF_SND, NULL, 2);
   ASSERT(ret==EMS_OK);

   GoToEndPosition(&emsData);
   
   /* delete all text / objects */
   n=CancelCurrentPosition(&emsData, 514);
   ASSERT(n==350);

   ReleaseEMSData(&emsData);
   EMSResourceVerdict();
}

/* GoToEndPosition */
void EMSTestEditSeq20()
{
   EMSData emsData;
   EMSTATUS ret;
   EMSTextFormat tf;   
   kal_uint8 str[10]; 
 
   InitializeEMSData(&emsData, 0);

   // Formatted Text (italic)
   ResetTextFormat(&tf);
   tf.isItalic = KAL_TRUE;   
   ret = AddString(&emsData, str, 5, &tf);
   ASSERT(ret==EMS_OK);
   
   ret=AddObject(&emsData, EMS_TYPE_PREDEF_SND, NULL, 2);
   ASSERT(ret==EMS_OK);

   GoToEndPosition(&emsData);
   ASSERT(emsData.CurrentTextFormatObj != NULL);
  
   ReleaseEMSData(&emsData);
   EMSResourceVerdict();
}

/* 
 * |<-- text format -->|
 * "12345<snd><snd>12345" 
 *
 */
void EMSTestEditSeq21()
{
   EMSData emsData;
   EMSTATUS ret;
   EMSTextFormat tf;   
   kal_uint8 str[10]; 
   kal_uint16 n;

   InitializeEMSData(&emsData, 0);

   // Formatted Text (italic)
   ResetTextFormat(&tf);
   tf.isItalic = KAL_TRUE;   
   ret = AddString(&emsData, str, 5, &tf);
   ASSERT(ret==EMS_OK);
   
   ret=AddObject(&emsData, EMS_TYPE_PREDEF_SND, NULL, 2);
   ASSERT(ret==EMS_OK);

   ret=AddObject(&emsData, EMS_TYPE_PREDEF_SND, NULL, 2);
   ASSERT(ret==EMS_OK);

   // Formatted Text (italic)
   ResetTextFormat(&tf);
   tf.isItalic = KAL_TRUE;   
   ret = AddString(&emsData, str, 5, &tf);
   ASSERT(ret==EMS_OK);

   n=BackwardCurrentPosition(&emsData, 20);
   ASSERT(n==12);
   ASSERT(IS_CURR_POS_AT_HEAD((&emsData)) == KAL_TRUE);
   
   n=ForwardCurrentPosition(&emsData, 20);
   ASSERT(n==12);
   ASSERT(IS_CURR_POS_AT_END((&emsData)) == KAL_TRUE);
   ASSERT(emsData.CurrentTextFormatObj != NULL);

   n=CancelCurrentPosition(&emsData, 50);
   ASSERT(n==12);
  
   ReleaseEMSData(&emsData);
   EMSResourceVerdict();
}


/* single seg. normal text and add port */
void EMSTestPortNum01()
{
   EMSData emsData;
   EMSTATUS ret;
   kal_uint8 numOfMsg;   
   kal_uint8 i,j;
   kal_uint8 dcs;
   kal_uint8 chars;
   
   for (i=0; i<3; i++)
   {

  for(j=0;j<4;j++)
 ems_mem_set(TPUD_p[j], 0, 160);   

  switch(i)
  {
 case 0 :
dcs = SMSAL_DEFAULT_DCS ;
chars = 100 ;
break;
 case 1 :
dcs = SMSAL_8BIT_DCS ;
chars = 100 ;

   break;
 case 2 :
dcs = SMSAL_UCS2_DCS;
chars = 60 ;

   break;
 default:
   break;
  }

  InitializeEMSData(&emsData, 0);


  ret = AddString(&emsData, string, chars, NULL);
  ASSERT(ret==EMS_OK);

  ret = EMSSetDCS(&emsData, dcs);
  ASSERT(ret==EMS_OK);

  ret = EMSSetPortNum(&emsData,0x0304 , 0x0102);
  ASSERT(ret==EMS_OK);

  ret=EMSPack(&emsData,1, &numOfMsg, TPUD_p, TPUDLen);
  ASSERT(ret==EMS_OK);
  ASSERT(numOfMsg==1);

  EMSDataDump(&emsData);
  /* verdict seg1  */
  ASSERT(TPUD_p[0][0]==6);/* udhl=port(6) */
  ASSERT(TPUD_p[0][1]==5);/* port iei */
  ASSERT(TPUD_p[0][2]==4);/* port iedl */
  ASSERT(TPUD_p[0][3]==1);/* port num */
  ASSERT(TPUD_p[0][4]==2);/* port num */
  ASSERT(TPUD_p[0][5]==3);/* port num */
  ASSERT(TPUD_p[0][6]==4); /* port num */


  ReleaseEMSData(&emsData);
   }

   EMSResourceVerdict();
}

/* orignally, single seg. normal text 
 * after adding port num, seg 1 full */
void EMSTestPortNum02()
{
   EMSData emsData;
   EMSTATUS ret;
   kal_uint8 numOfMsg;   
   kal_uint8 i,j;
   kal_uint8 dcs;
   kal_uint8 chars;
   
   for (i=0; i<3; i++)
   {
  for(j=0;j<4;j++)
 ems_mem_set(TPUD_p[j], 0, 160);   

  switch (i)
  {
 case 0 :
dcs = SMSAL_DEFAULT_DCS ;
chars = 152 ;  //160 - ((1+6)*8)/7
break;
 case 1 :
dcs = SMSAL_8BIT_DCS ;
chars = 133 ;  //140 - (1+6)

   break;
 case 2 :
dcs = SMSAL_UCS2_DCS;
chars = 66  ;  //70 - ((1+6)+1)/2

   break;
 default:
   break;
  }

  InitializeEMSData(&emsData, 0);


  ret = AddString(&emsData, string, chars, NULL);
  ASSERT(ret==EMS_OK);

  ret = EMSSetDCS(&emsData, dcs);
  ASSERT(ret==EMS_OK);

  ret = EMSSetPortNum(&emsData,0x0304 , 0x0102);
  ASSERT(ret==EMS_OK);

  ret=EMSPack(&emsData,1, &numOfMsg, TPUD_p, TPUDLen);
  ASSERT(ret==EMS_OK);
  ASSERT(numOfMsg==1);

  /* verdict seg1  */
  ASSERT(TPUD_p[0][0]==6);/* udhl=port(6) */
  ASSERT(TPUD_p[0][1]==5);/* port iei */
  ASSERT(TPUD_p[0][2]==4);/* port iedl */
  ASSERT(TPUD_p[0][3]==1);/* port num */
  ASSERT(TPUD_p[0][4]==2);/* port num */
  ASSERT(TPUD_p[0][5]==3);/* port num */
  ASSERT(TPUD_p[0][6]==4); /* port num */


  ReleaseEMSData(&emsData);
   }

   EMSResourceVerdict();
}

/* orignally, single seg. normal text 
 * after adding port num, become 2 seg */
void EMSTestPortNum03()
{
   EMSData emsData;
   EMSTATUS ret;
   kal_uint8 numOfMsg;   
   kal_uint8 i,j;
   kal_uint8 dcs;
   kal_uint8 chars;
   
   for (i=0; i<3; i++)
   {
  for(j=0;j<4;j++)
 ems_mem_set(TPUD_p[j], 0, 160);   

  switch (i)
  {
 case 0 :
dcs = SMSAL_DEFAULT_DCS ;
chars = 155 ;  //full= 160 - ((1+6)*8)/7 =152
break;
 case 1 :
dcs = SMSAL_8BIT_DCS ;
chars = 137 ;  //full= 140 - (1+6) = 133

   break;
 case 2 :
dcs = SMSAL_UCS2_DCS;
chars = 68  ;  //full= 70 - ((1+6)+1)/2 =66

   break;
 default:
   break;
  }

  InitializeEMSData(&emsData, 0);


  ret = AddString(&emsData, string, chars, NULL);
  ASSERT(ret==EMS_OK);

  ret = EMSSetDCS(&emsData, dcs);
  ASSERT(ret==EMS_OK);

  ret = EMSSetPortNum(&emsData,0x0304 , 0x0102);
  ASSERT(ret==EMS_OK);

  ret=EMSPack(&emsData,1, &numOfMsg, TPUD_p, TPUDLen);
  ASSERT(ret==EMS_OK);
  ASSERT(numOfMsg==2);

  /* verdict seg1  */
  ASSERT(TPUD_p[0][0]==11);/* udhl = concat(5) + port (6) */

  ASSERT(TPUD_p[0][1]==5);/* port iei */
  ASSERT(TPUD_p[0][2]==4);/* port iedl */
  ASSERT(TPUD_p[0][3]==1);/* port num */
  ASSERT(TPUD_p[0][4]==2);/* port num */
  ASSERT(TPUD_p[0][5]==3);/* port num */
  ASSERT(TPUD_p[0][6]==4); /* port num */

  ASSERT(TPUD_p[0][7]==0);/* concat iei */
  ASSERT(TPUD_p[0][8]==3);/* concat iedl */
  ASSERT(TPUD_p[0][10]==2);/* total seg */
  ASSERT(TPUD_p[0][11]==1);/* seg */

  /* verdict seg2 */
  ASSERT(TPUD_p[1][0]==11);/* udhl = concat(5) + port (6) */

  ASSERT(TPUD_p[1][1]==5);/* port iei */
  ASSERT(TPUD_p[1][2]==4);/* port iedl */
  ASSERT(TPUD_p[1][3]==1);/* port num */
  ASSERT(TPUD_p[1][4]==2);/* port num */
  ASSERT(TPUD_p[1][5]==3);/* port num */
  ASSERT(TPUD_p[1][6]==4); /* port num */

  ASSERT(TPUD_p[1][7]==0);/* concat iei */
  ASSERT(TPUD_p[1][8]==3);/* concat iedl */
  ASSERT(TPUD_p[1][10]==2);/* total seg */
  ASSERT(TPUD_p[1][11]==2);/* seg */

  ReleaseEMSData(&emsData);
   }

   EMSResourceVerdict();

}


/* orignally, two seg. normal text 
 * after adding port num, 2 seg full */
void EMSTestPortNum04()
{
   EMSData emsData;
   EMSTATUS ret;
   kal_uint8 numOfMsg;   
   kal_uint8 i,j;
   kal_uint8 dcs;
   kal_uint16 chars;
   
   for (i=0; i<3; i++)
   {
  for(j=0;j<4;j++)
 ems_mem_set(TPUD_p[j], 0, 160);   

  switch (i)
  {
 case 0 :
dcs = SMSAL_DEFAULT_DCS ;
chars = 146*2 ;  //concat full= 160 - 14 =146
break;
 case 1 :
dcs = SMSAL_8BIT_DCS ;
chars = 128*2 ;  //concat full= 140 - 12 =128
break;
 case 2 :
dcs = SMSAL_UCS2_DCS;
chars = 64*2 ;  //concat full= 70 - 6 =64
break;
 default:
break;
  }

  InitializeEMSData(&emsData, 0);


  ret = AddString(&emsData, string, chars, NULL);
  ASSERT(ret==EMS_OK);

  ret = EMSSetDCS(&emsData, dcs);
  ASSERT(ret==EMS_OK);

  ret = EMSSetPortNum(&emsData,0x0304 , 0x0102);
  ASSERT(ret==EMS_OK);

  ret=EMSPack(&emsData,1, &numOfMsg, TPUD_p, TPUDLen);
  ASSERT(ret==EMS_OK);
  ASSERT(numOfMsg==2);

  /* verdict seg1  */
  ASSERT(TPUD_p[0][0]==11);/* udhl = concat(5) + port (6) */

  ASSERT(TPUD_p[0][1]==5);/* port iei */
  ASSERT(TPUD_p[0][2]==4);/* port iedl */
  ASSERT(TPUD_p[0][3]==1);/* port num */
  ASSERT(TPUD_p[0][4]==2);/* port num */
  ASSERT(TPUD_p[0][5]==3);/* port num */
  ASSERT(TPUD_p[0][6]==4); /* port num */

  ASSERT(TPUD_p[0][7]==0);/* concat iei */
  ASSERT(TPUD_p[0][8]==3);/* concat iedl */
  ASSERT(TPUD_p[0][10]==2);/* total seg */
  ASSERT(TPUD_p[0][11]==1);/* seg */

  /* verdict seg2 */
  ASSERT(TPUD_p[1][0]==11);/* udhl = concat(5) + port (6) */

  ASSERT(TPUD_p[1][1]==5);/* port iei */
  ASSERT(TPUD_p[1][2]==4);/* port iedl */
  ASSERT(TPUD_p[1][3]==1);/* port num */
  ASSERT(TPUD_p[1][4]==2);/* port num */
  ASSERT(TPUD_p[1][5]==3);/* port num */
  ASSERT(TPUD_p[1][6]==4); /* port num */

  ASSERT(TPUD_p[1][7]==0);/* concat iei */
  ASSERT(TPUD_p[1][8]==3);/* concat iedl */
  ASSERT(TPUD_p[1][10]==2);/* total seg */
  ASSERT(TPUD_p[1][11]==2);/* seg */

  ReleaseEMSData(&emsData);
   }

   EMSResourceVerdict();

}


/* orignally, two seg. normal text 
 * after adding port num, become three seg */
void EMSTestPortNum05()
{
   EMSData emsData;
   EMSTATUS ret;
   kal_uint8 numOfMsg;   
   kal_uint8 i,j;
   kal_uint8 dcs;
   kal_uint16 chars;
   
   for (i=0; i<3; i++)
   {
  for(j=0;j<4;j++)
 ems_mem_set(TPUD_p[j], 0, 160);   

  switch (i)
  {
 case 0 :
dcs = SMSAL_DEFAULT_DCS ;
chars = 146*2 +10 ;  //concat full= 160 - 14 =146
break;
 case 1 :
dcs = SMSAL_8BIT_DCS ;
chars = 128*2 +10;  //concat full= 140 - 12 =128
break;
 case 2 :
dcs = SMSAL_UCS2_DCS;
chars = 64*2 +3 ;  //concat full= 70 - 6 =64
break;
 default:
break;
  }

  InitializeEMSData(&emsData, 0);


  ret = AddString(&emsData, string, chars, NULL);
  ASSERT(ret==EMS_OK);

  ret = EMSSetDCS(&emsData, dcs);
  ASSERT(ret==EMS_OK);

  ret = EMSSetPortNum(&emsData,0x0304 , 0x0102);
  ASSERT(ret==EMS_OK);

  ret=EMSPack(&emsData,1, &numOfMsg, TPUD_p, TPUDLen);
  ASSERT(ret==EMS_OK);
  ASSERT(numOfMsg==3);

  /* verdict seg1  */
  ASSERT(TPUD_p[0][0]==11);/* udhl = concat(5) + port (6) */

  ASSERT(TPUD_p[0][1]==5);/* port iei */
  ASSERT(TPUD_p[0][2]==4);/* port iedl */
  ASSERT(TPUD_p[0][3]==1);/* port num */
  ASSERT(TPUD_p[0][4]==2);/* port num */
  ASSERT(TPUD_p[0][5]==3);/* port num */
  ASSERT(TPUD_p[0][6]==4); /* port num */

  ASSERT(TPUD_p[0][7]==0);/* concat iei */
  ASSERT(TPUD_p[0][8]==3);/* concat iedl */
  ASSERT(TPUD_p[0][10]==3);/* total seg */
  ASSERT(TPUD_p[0][11]==1);/* seg */

  /* verdict seg2 */
  ASSERT(TPUD_p[1][0]==11);/* udhl = concat(5) + port (6) */

  ASSERT(TPUD_p[1][1]==5);/* port iei */
  ASSERT(TPUD_p[1][2]==4);/* port iedl */
  ASSERT(TPUD_p[1][3]==1);/* port num */
  ASSERT(TPUD_p[1][4]==2);/* port num */
  ASSERT(TPUD_p[1][5]==3);/* port num */
  ASSERT(TPUD_p[1][6]==4); /* port num */

  ASSERT(TPUD_p[1][7]==0);/* concat iei */
  ASSERT(TPUD_p[1][8]==3);/* concat iedl */
  ASSERT(TPUD_p[1][10]==3);/* total seg */
  ASSERT(TPUD_p[1][11]==2);/* seg */

  /* verdict seg3 */
  ASSERT(TPUD_p[2][0]==11);/* udhl = concat(5) + port (6) */

  ASSERT(TPUD_p[2][1]==5);/* port iei */
  ASSERT(TPUD_p[2][2]==4);/* port iedl */
  ASSERT(TPUD_p[2][3]==1);/* port num */
  ASSERT(TPUD_p[2][4]==2);/* port num */
  ASSERT(TPUD_p[2][5]==3);/* port num */
  ASSERT(TPUD_p[2][6]==4); /* port num */

  ASSERT(TPUD_p[2][7]==0);/* concat iei */
  ASSERT(TPUD_p[2][8]==3);/* concat iedl */
  ASSERT(TPUD_p[2][10]==3);/* total seg */
  ASSERT(TPUD_p[2][11]==3);/* seg */

  ReleaseEMSData(&emsData);
   }

   EMSResourceVerdict();

}


/* orignally, four seg. normal text 
 * after adding port num, become full (4 seg) */
void EMSTestPortNum06()
{
   EMSData emsData;
   EMSTATUS ret;
   kal_uint8 numOfMsg;   
   kal_uint8 i,j;
   kal_uint8 dcs;
   kal_uint16 chars;
   
   for (i=0; i<3; i++)
   {
  for(j=0;j<4;j++)
 ems_mem_set(TPUD_p[j], 0, 160);   

  switch (i)
  {
 case 0 :
dcs = SMSAL_DEFAULT_DCS ;
chars = 146*4 ;  //concat full= 160 - 14 =146
break;
 case 1 :
dcs = SMSAL_8BIT_DCS ;
chars = 128*4 ;  //concat full= 140 - 12 =128
break;
 case 2 :
dcs = SMSAL_UCS2_DCS;
chars = 64*4 ;  //concat full= 70 - 6 =64
break;
 default:
break;
  }

  InitializeEMSData(&emsData, 0);

  ret = AddString(&emsData, string, chars, NULL);
  ASSERT(ret==EMS_OK);

  ret = EMSSetDCS(&emsData, dcs);
  ASSERT(ret==EMS_OK);

  ret = EMSSetPortNum(&emsData,0x0304 , 0x0102);
  ASSERT(ret==EMS_OK);

  ret=EMSPack(&emsData,1, &numOfMsg, TPUD_p, TPUDLen);
  ASSERT(ret==EMS_OK);
  ASSERT(numOfMsg==4);

  /* verdict seg1  */
  ASSERT(TPUD_p[0][0]==11);/* udhl = concat(5) + port (6) */

  ASSERT(TPUD_p[0][1]==5);/* port iei */
  ASSERT(TPUD_p[0][2]==4);/* port iedl */
  ASSERT(TPUD_p[0][3]==1);/* port num */
  ASSERT(TPUD_p[0][4]==2);/* port num */
  ASSERT(TPUD_p[0][5]==3);/* port num */
  ASSERT(TPUD_p[0][6]==4); /* port num */

  ASSERT(TPUD_p[0][7]==0);/* concat iei */
  ASSERT(TPUD_p[0][8]==3);/* concat iedl */
  ASSERT(TPUD_p[0][10]==4);/* total seg */
  ASSERT(TPUD_p[0][11]==1);/* seg */

  /* verdict seg2 */
  ASSERT(TPUD_p[1][0]==11);/* udhl = concat(5) + port (6) */

  ASSERT(TPUD_p[1][1]==5);/* port iei */
  ASSERT(TPUD_p[1][2]==4);/* port iedl */
  ASSERT(TPUD_p[1][3]==1);/* port num */
  ASSERT(TPUD_p[1][4]==2);/* port num */
  ASSERT(TPUD_p[1][5]==3);/* port num */
  ASSERT(TPUD_p[1][6]==4); /* port num */

  ASSERT(TPUD_p[1][7]==0);/* concat iei */
  ASSERT(TPUD_p[1][8]==3);/* concat iedl */
  ASSERT(TPUD_p[1][10]==4);/* total seg */
  ASSERT(TPUD_p[1][11]==2);/* seg */

  /* verdict seg3 */
  ASSERT(TPUD_p[2][0]==11);/* udhl = concat(5) + port (6) */

  ASSERT(TPUD_p[2][1]==5);/* port iei */
  ASSERT(TPUD_p[2][2]==4);/* port iedl */
  ASSERT(TPUD_p[2][3]==1);/* port num */
  ASSERT(TPUD_p[2][4]==2);/* port num */
  ASSERT(TPUD_p[2][5]==3);/* port num */
  ASSERT(TPUD_p[2][6]==4); /* port num */

  ASSERT(TPUD_p[2][7]==0);/* concat iei */
  ASSERT(TPUD_p[2][8]==3);/* concat iedl */
  ASSERT(TPUD_p[2][10]==4);/* total seg */
  ASSERT(TPUD_p[2][11]==3);/* seg */

  /* verdict seg4 */
  ASSERT(TPUD_p[3][0]==11);/* udhl = concat(5) + port (6) */

  ASSERT(TPUD_p[3][1]==5);/* port iei */
  ASSERT(TPUD_p[3][2]==4);/* port iedl */
  ASSERT(TPUD_p[3][3]==1);/* port num */
  ASSERT(TPUD_p[3][4]==2);/* port num */
  ASSERT(TPUD_p[3][5]==3);/* port num */
  ASSERT(TPUD_p[3][6]==4); /* port num */

  ASSERT(TPUD_p[3][7]==0);/* concat iei */
  ASSERT(TPUD_p[3][8]==3);/* concat iedl */
  ASSERT(TPUD_p[3][10]==4);/* total seg */
  ASSERT(TPUD_p[3][11]==4);/* seg */

  ReleaseEMSData(&emsData);
   }

   EMSResourceVerdict();
}


/* orignally, four seg. normal text 
 * after adding port num,  two long */
void EMSTestPortNum07()
{
   EMSData emsData;
   EMSTATUS ret;
   kal_uint8 numOfMsg;   
   kal_uint8 i,j;
   kal_uint8 dcs;
   kal_uint16 chars;
   
   for (i=0; i<3; i++)
   {
  for(j=0;j<EMS_TEST_MAX_SEG;j++)
 ems_mem_set(TPUD_p[j], 0, 160);   

  switch (i)
  {
 case 0 :
dcs = SMSAL_DEFAULT_DCS ;
chars = 146*EMS_TEST_MAX_SEG + 10 ;  //concat full= 160 - 14 =146
break;
 case 1 :
dcs = SMSAL_8BIT_DCS ;
chars = 128*EMS_TEST_MAX_SEG  + 10 ;  //concat full= 140 - 12 =128
break;
 case 2 :
dcs = SMSAL_UCS2_DCS;
chars = 64*EMS_TEST_MAX_SEG   + 5 ;  //concat full= 70 - 6 =64
break;
 default:
break;
  }

  InitializeEMSData(&emsData, 0);

  ret = AddString(&emsData, string, chars, NULL);
  ASSERT(ret==EMS_OK);

  ret = EMSSetDCS(&emsData, dcs);
  ASSERT(ret==EMS_OK);

  ret = EMSSetPortNum(&emsData,0x0304 , 0x0102);
  ASSERT(ret!=EMS_OK);

  ret=EMSPack(&emsData,1, &numOfMsg, TPUD_p, TPUDLen);
  ASSERT(ret==EMS_OK);
  ASSERT(numOfMsg==EMS_TEST_MAX_SEG);

  /* verdict seg1  */
  ASSERT(TPUD_p[0][0]==5);/* udhl = concat(5) + port (6) */
  ASSERT(TPUD_p[0][1]==0);/* concat iei */
  ASSERT(TPUD_p[0][2]==3);/* concat iedl */
  ASSERT(TPUD_p[0][4]==EMS_TEST_MAX_SEG);/* total seg */
  ASSERT(TPUD_p[0][5]==1);/* seg */

  /* verdict seg2 */
  ASSERT(TPUD_p[1][0]==5);/* udhl = concat(5) + port (6) */
  ASSERT(TPUD_p[1][1]==0);/* concat iei */
  ASSERT(TPUD_p[1][2]==3);/* concat iedl */
  ASSERT(TPUD_p[1][4]==EMS_TEST_MAX_SEG);/* total seg */
  ASSERT(TPUD_p[1][5]==2);/* seg */

  /* verdict seg3 */
  ASSERT(TPUD_p[2][0]==5);/* udhl = concat(5) + port (6) */
  ASSERT(TPUD_p[2][1]==0);/* concat iei */
  ASSERT(TPUD_p[2][2]==3);/* concat iedl */
  ASSERT(TPUD_p[2][4]==EMS_TEST_MAX_SEG);/* total seg */
  ASSERT(TPUD_p[2][5]==3);/* seg */

  /* verdict seg4 */
  ASSERT(TPUD_p[3][0]==5);/* udhl = concat(5) + port (6) */
  ASSERT(TPUD_p[3][1]==0);/* concat iei */
  ASSERT(TPUD_p[3][2]==3);/* concat iedl */
  ASSERT(TPUD_p[3][4]==EMS_TEST_MAX_SEG);/* total seg */
  ASSERT(TPUD_p[3][5]==4);/* seg */


  ReleaseEMSData(&emsData);
   }

   EMSResourceVerdict();
}

//from vCalendar: length=134, dcs=8bit
void EMSTestPortNum08()
{
   EMSData emsData;
   EMSTATUS ret;
   kal_uint8 numOfMsg;   
   kal_uint8 i,j;
   kal_uint8 dcs;
   kal_uint16 chars;
   
   for(j=0;j<100;j++)
  *(string +2*j)=j;

   for(j=0;j<4;j++)
   {
  ems_mem_set(TPUD_p[j], 0, 160);   
   }


   dcs = SMSAL_8BIT_DCS ;
   chars = 134;  //concat full= 140 - 12 =128

   InitializeEMSData(&emsData, 0);

   ret = EMSSetPortNum(&emsData,0x0304 , 0x0102);
   ASSERT(ret==EMS_OK);

   ret = EMSSetDCS(&emsData, dcs);
   ASSERT(ret==EMS_OK);


   for(j=0;j<chars-2;j++)
   {
  ret = AddString(&emsData, string, 1, NULL);
  ASSERT(ret==EMS_OK);
   }

   ret = AddString(&emsData, string, 1, NULL);
   ret = AddString(&emsData, string, 1, NULL);


   ret=EMSPack(&emsData,1, &numOfMsg, TPUD_p, TPUDLen);
   ASSERT(ret==EMS_OK);
   ASSERT(numOfMsg==2);

   /* verdict seg1  */
   ASSERT(TPUD_p[0][0]==11);/* udhl = concat(5) + port (6) */

   ASSERT(TPUD_p[0][1]==5);/* port iei */
   ASSERT(TPUD_p[0][2]==4);/* port iedl */
   ASSERT(TPUD_p[0][3]==1);/* port num */
   ASSERT(TPUD_p[0][4]==2);/* port num */
   ASSERT(TPUD_p[0][5]==3);/* port num */
   ASSERT(TPUD_p[0][6]==4); /* port num */

   ASSERT(TPUD_p[0][7]==0);/* concat iei */
   ASSERT(TPUD_p[0][8]==3);/* concat iedl */
   ASSERT(TPUD_p[0][10]==2);/* total seg */
   ASSERT(TPUD_p[0][11]==1);/* seg */

   /* verdict seg2 */
   ASSERT(TPUD_p[1][0]==11);/* udhl = concat(5) + port (6) */

   ASSERT(TPUD_p[1][1]==5);/* port iei */
   ASSERT(TPUD_p[1][2]==4);/* port iedl */
   ASSERT(TPUD_p[1][3]==1);/* port num */
   ASSERT(TPUD_p[1][4]==2);/* port num */
   ASSERT(TPUD_p[1][5]==3);/* port num */
   ASSERT(TPUD_p[1][6]==4); /* port num */
 
   ASSERT(TPUD_p[1][7]==0);/* concat iei */
   ASSERT(TPUD_p[1][8]==3);/* concat iedl */
   ASSERT(TPUD_p[1][10]==2);/* total seg */
   ASSERT(TPUD_p[1][11]==2);/* seg */

   ReleaseEMSData(&emsData);

   EMSResourceVerdict();
}

/* text only */
void EMSTestMsgPreview01()
{
   EMSData emsData;
   EMSTATUS ret;
   kal_uint8 *textBuff; 
   kal_uint8 numOfMsg;   
   kal_uint16 i,j;
   kal_uint8 dcs;
   kal_uint16 chars;
   kal_uint8  is_obj_present; 
   kal_uint16 textLen, input_len;
   
   textBuff = (kal_uint8*) get_ctrl_buffer(612*2);


   for(j=0;j<612;j++)
  *(string +2*j)= (j & 0xff);

   for(j=0;j<4;j++)
   {
  ems_mem_set(TPUD_p[j], 0, 160);   
   }

   for(j=1;j< 10;j++)
   {
  chars = (j * 160 + rand())% 612 ;

  InitializeEMSData(&emsData, SMSAL_DEFAULT_DCS);

  ret = AddString(&emsData, string, chars , NULL);
  ASSERT(ret==EMS_OK);

  ret=EMSPack(&emsData,1, &numOfMsg, TPUD_p, TPUDLen);
  ASSERT(ret==EMS_OK);

  /* ======================================== */
  ems_mem_set(textBuff , 0, 612*2);   
  input_len = chars * 2 ;

  ret = EMSMsgPreview(numOfMsg,
			  SMSAL_DEFAULT_DCS,
   KAL_TRUE,
   TPUD_p,
   TPUDLen,
   input_len ,/* octets */ 
   textBuff ,
   &is_obj_present, 
   &textLen);   /* octets */

  ASSERT(ret==EMS_OK);
  ASSERT(is_obj_present ==KAL_FALSE);
  ASSERT(textLen == input_len);
   
  /* ======================================== */
  ReleaseEMSData(&emsData);
   }


   EMSResourceVerdict();
}


/* Text formatted text: 
 * 10 bold characters,
 * 10 italic characters,
 * 10 underline characters,
 * 10 strikethrough characters,
 * 10 large font characters,
 * 10 right alignment characters
 */
void EMSTestAllTextFormat(kal_uint8 dcs)
{
   EMSTATUS ret;
   EMSTextFormat tf;
   kal_uint8 numOfMsg;
   kal_uint8 TPUD[160];
   kal_uint8 TPUDLen[4];
   kal_uint8 *TPUD_p[4];
   const kal_uint8 text_num=6;
   kal_uint8 chars=5;
   kal_uint8 text=10;
   kal_uint8 i,offset;
   kal_uint8 mode[]=
   {  //0x03 : default alignment
  0x13,//bold
  0x23,//italic
  0x43,//underline
  0x83,//strikethrough
  0x07,//large font
  0x02 //right alignment
   };

   if(dcs==SMSAL_UCS2_DCS)
  text = chars*2;
   else
  text = chars;

   TPUD_p[0] = TPUD;
   TPUD_p[1] = NULL;
   TPUD_p[2] = NULL;
   TPUD_p[3] = NULL;

   InitializeEMSData(&emsData, dcs);

   /* 10 bold characters */
   ResetTextFormat(&tf);
   tf.isBold = KAL_TRUE;
   ret = EMSTestAddString(chars, &tf);
   ASSERT(ret==EMS_OK);

   /* 10 italic characters */
   ResetTextFormat(&tf);
   tf.isItalic = KAL_TRUE;
   ret = EMSTestAddString(chars, &tf);
   ASSERT(ret==EMS_OK);

   /* 10 underline characters */
   ResetTextFormat(&tf);
   tf.isUnderline = KAL_TRUE;
   ret = EMSTestAddString(chars, &tf);
   ASSERT(ret==EMS_OK);

   /* 10 strikethrough characters */
   ResetTextFormat(&tf);
   tf.isStrikethrough = KAL_TRUE;
   ret = EMSTestAddString(chars, &tf);
   ASSERT(ret==EMS_OK);

   /* 10 large font characters */
   ResetTextFormat(&tf);
   tf.FontSize = EMS_FONTSIZE_LARGE;
   ret = EMSTestAddString(chars, &tf);
   ASSERT(ret==EMS_OK);

   /* 10 right alignment characters */
   ResetTextFormat(&tf);
   tf.Alignment = EMS_ALIGN_RIGHT;
   ret = EMSTestAddString(chars, &tf);
   ASSERT(ret==EMS_OK);
   
   ret = EMSPack(&emsData,1, &numOfMsg, TPUD_p, TPUDLen);
   ASSERT(ret==EMS_OK);

   /* check verdicts */
   ASSERT(numOfMsg == 1);
   ASSERT(TPUDLen[0] == (1+/*udhl*/
 text_num*5+/*text fmt hdr*/
 text*text_num/*text*/));

   ASSERT(TPUD[0]==5*text_num);//udhl=30

   offset=1;
   for(i=0;i<text_num;i++)
   {
  ASSERT(TPUD[offset++]==EMS_TEXT_FORMAT_IEI);
  ASSERT(TPUD[offset++]==EMS_TXTFMT_IEDL);
  ASSERT(TPUD[offset++]==i*chars);/* start position*/
  ASSERT(TPUD[offset++]==chars);  /* text length (in char) */
  ASSERT(TPUD[offset++]==mode[i]);/* text format mode */
#ifdef __EMS_TXTFMT_SUPP_COLOR__//check color
#endif
   } 

   ReleaseEMSData(&emsData);
   EMSResourceVerdict();
}

void EMSTestPredefAnm(void)
{
   EMSTATUS ret;
   kal_uint8 i;
   kal_uint8 numOfMsg;
   kal_uint8 TPUD[160];
   kal_uint8 TPUDLen[4];
   kal_uint8 *TPUD_p[4];
   kal_uint8 offset;
   kal_uint8 PredefNo;

   TPUD_p[0] = TPUD;
   TPUD_p[1] = NULL;
   TPUD_p[2] = NULL;
   TPUD_p[3] = NULL;

   InitializeEMSData(&emsData, 0);

   /* predefined animation 0-14 */
   for(i=0;i<15;i++)
   {
  PredefNo = i; 
  ret=AddObject(&emsData, EMS_TYPE_PREDEF_ANM, NULL, PredefNo);

  ASSERT(ret==EMS_OK);
   }
   
   ret = EMSPack(&emsData, 1,&numOfMsg, TPUD_p, TPUDLen);
   ASSERT(ret==EMS_OK);

   /* check verdicts */
   ASSERT(numOfMsg == 1);
   ASSERT(TPUDLen[0] == (1+/*udhl*/
 4*15/* ie */));

   ASSERT(TPUD[0]==4*15);//udhl

   offset=1;
   for(i=0;i<15;i++)
   {
  ASSERT(TPUD[offset++]==EMS_PREDEF_ANM_IEI);
  ASSERT(TPUD[offset++]==EMS_PREDEF_IEDL);
  ASSERT(TPUD[offset++]==0);//start position
  ASSERT(TPUD[offset++]==i);//animation number
   }

   ReleaseEMSData(&emsData);
   EMSResourceVerdict();
}

void EMSTestLargefAnm(void)
{ 
   EMSObjData objData;   
   EMSTATUS ret;
   
   kal_uint8 numOfMsg;
   kal_uint8 TPUD[160];
   kal_uint8 TPUDLen[4];
   kal_uint8 *TPUD_p[4];
   kal_uint8 offset;

   TPUD_p[0] = TPUD;
   TPUD_p[1] = NULL;
   TPUD_p[2] = NULL;
   TPUD_p[3] = NULL;

   InitializeEMSData(&emsData, 0);
  
   /* initialize ems object to animation */
   objData.animation.vDim = 16;
   objData.animation.hDim = 16;
   objData.animation.bitsPerPixel = 1;
   objData.animation.cFrame = 4;

   objData.animation.pdu_length=128;
   objData.animation.pdu = get_ctrl_buffer(128);
   kal_mem_cpy(objData.animation.pdu, large_anm, 128);
   ret=AddObject(&emsData, EMS_TYPE_USERDEF_ANM, &objData,0);
   ASSERT(ret==EMS_OK);
  
   ret = EMSPack(&emsData,1, &numOfMsg, TPUD_p, TPUDLen);
   ASSERT(ret==EMS_OK);

   /* check verdicts */
   ASSERT(numOfMsg == 1);
   ASSERT(TPUDLen[0] == (1+/*udhl*/
 3+128/* ie */));

   ASSERT(TPUD[0]==3+128);//udhl

   offset=1;
   ASSERT(TPUD[offset++]==EMS_LARGE_ANM_IEI);
   ASSERT(TPUD[offset++]==1+128);
   ASSERT(TPUD[offset++]==0);//start position
   //check raw data
   //for(i=0;i<128;i++)
   //   ASSERT(TPUD[offset++]==large_anm[i]);

   free_ctrl_buffer(objData.animation.pdu);

   ReleaseEMSData(&emsData);
   EMSResourceVerdict();
}

void EMSTestSmallfAnm(void)
{ 
   EMSObjData objData;   
   EMSTATUS ret;
   
   kal_uint8 numOfMsg;
   kal_uint8 TPUD[160];
   kal_uint8 TPUDLen[4];
   kal_uint8 *TPUD_p[4];
   kal_uint8 offset;

   TPUD_p[0] = TPUD;
   TPUD_p[1] = NULL;
   TPUD_p[2] = NULL;
   TPUD_p[3] = NULL;

   InitializeEMSData(&emsData, 0);
  
   /* initialize ems object to animation */
   objData.animation.vDim = 8;
   objData.animation.hDim = 8;
   objData.animation.bitsPerPixel = 1;
   objData.animation.cFrame = 4;

   objData.animation.pdu_length=32;
   objData.animation.pdu = get_ctrl_buffer(32);
   kal_mem_cpy(objData.animation.pdu, small_anm, 32);
   ret=AddObject(&emsData, EMS_TYPE_USERDEF_ANM, &objData,0);
   ASSERT(ret==EMS_OK);
  
   ret = EMSPack(&emsData, 1,&numOfMsg, TPUD_p, TPUDLen);
   ASSERT(ret==EMS_OK);

   /* check verdicts */
   ASSERT(numOfMsg == 1);
   ASSERT(TPUDLen[0] == (1+/*udhl*/
 3+32/* ie */));

   ASSERT(TPUD[0]==3+32);//udhl

   offset=1;
   ASSERT(TPUD[offset++]==EMS_SMALL_ANM_IEI);
   ASSERT(TPUD[offset++]==1+32);
   ASSERT(TPUD[offset++]==0);//start position
   //check raw data
   //for(i=0;i<128;i++)
   //   ASSERT(TPUD[offset++]==small_anm[i]);

   free_ctrl_buffer(objData.animation.pdu);
   ReleaseEMSData(&emsData);
   EMSResourceVerdict();
}

void EMSTestPredefSnd(void)
{
   EMSTATUS ret;
   kal_uint8 i;
   kal_uint8 numOfMsg;
   kal_uint8 TPUD[160];
   kal_uint8 TPUDLen[4];
   kal_uint8 *TPUD_p[4];
   kal_uint8 offset;
   kal_uint8 PredefNo;

   TPUD_p[0] = TPUD;
   TPUD_p[1] = NULL;
   TPUD_p[2] = NULL;
   TPUD_p[3] = NULL;

   InitializeEMSData(&emsData, 0);

   /* predefined sound 0-9 */
   for(i=0;i<10;i++)
   {
  PredefNo = i;  
  ret=AddObject(&emsData, EMS_TYPE_PREDEF_SND, NULL, PredefNo);
  ASSERT(ret==EMS_OK);
   }
   
   ret = EMSPack(&emsData, 1,&numOfMsg, TPUD_p, TPUDLen);
   ASSERT(ret==EMS_OK);

   /* check verdicts */
   ASSERT(numOfMsg == 1);
   ASSERT(TPUDLen[0] == (1+/*udhl*/
 4*10/* ie */));

   ASSERT(TPUD[0]==4*10);//udhl

   offset=1;
   for(i=0;i<10;i++)
   {
  ASSERT(TPUD[offset++]==EMS_PREDEF_SND_IEI);
  ASSERT(TPUD[offset++]==EMS_PREDEF_IEDL);
  ASSERT(TPUD[offset++]==0);//start position
  ASSERT(TPUD[offset++]==i);//animation number
   }

   ReleaseEMSData(&emsData);
   EMSResourceVerdict();
}

/* check two objects are the same or not */
void EMSObjectVerdict(EMSObject *o1, EMSObject *o2)
{
   if(o1==NULL && o2==NULL)
  return;

   ASSERT(o1!=NULL && o2!=NULL);

   ASSERT(o1->Type==o2->Type); 

   ASSERT(o1->OffsetToText==o2->OffsetToText);

#ifdef __EMS_REL5__
#if 0
   ASSERT(EXT_OBJ_INFO_P(o1, isExtObj)== EXT_OBJ_INFO_P(o2, isExtObj) ) ;
   ASSERT(EXT_OBJ_INFO_P(o1, isReusedObj) == EXT_OBJ_INFO_P(o2, isReusedObj) ) ;
   ASSERT(EXT_OBJ_INFO_P(o1, ExtObjRef)   == EXT_OBJ_INFO_P(o2, ExtObjRef) ) ;
#endif
#endif

   switch(o1->Type)
   {
  case EMS_TYPE_TEXT_FORMAT:
   ASSERT(EMS_P(o1->data,text_format.FontSize)== EMS_P(o2->data,text_format.FontSize));
   ASSERT(EMS_P(o1->data,text_format.Alignment)   == EMS_P(o2->data,text_format.Alignment));
   ASSERT(EMS_P(o1->data,text_format.isBold)  == EMS_P(o2->data,text_format.isBold));
   ASSERT(EMS_P(o1->data,text_format.isItalic)== EMS_P(o2->data,text_format.isItalic));
   ASSERT(EMS_P(o1->data,text_format.isUnderline) == EMS_P(o2->data,text_format.isUnderline));
   ASSERT(EMS_P(o1->data,text_format.isStrikethrough) == EMS_P(o2->data,text_format.isStrikethrough));
   ASSERT(EMS_P(o1->data,text_format.textLength)  == EMS_P(o2->data,text_format.textLength));
   
   ASSERT(EMS_P(o1->data,text_format.bgColor) == EMS_P(o2->data,text_format.bgColor));
   ASSERT(EMS_P(o1->data,text_format.fgColor) == EMS_P(o2->data,text_format.fgColor));   
   break;

  case EMS_TYPE_USERDEF_ANM:		

   ASSERT(EMS_P(o1->data,animation.pdu_length)  == EMS_P(o2->data,animation.pdu_length));  
   ASSERT(EMS_P(o1->data,animation.hDim)== EMS_P(o2->data,animation.hDim)) ;
   ASSERT(EMS_P(o1->data,animation.vDim)== EMS_P(o2->data,animation.vDim)) ;
   ASSERT(EMS_P(o1->data,animation.bitsPerPixel)== EMS_P(o2->data,animation.bitsPerPixel)) ;
   ASSERT(EMS_P(o1->data,animation.cFrame)  == EMS_P(o2->data,animation.cFrame)) ;
   ASSERT(EMS_P(o1->data,animation.cRepeat) == EMS_P(o2->data,animation.cRepeat)) ;
   ASSERT(EMS_P(o1->data,animation.Duration)== EMS_P(o2->data,animation.Duration)) ;
   /* ASSERT(EMS_P(o1->data,animation.attribute)   == EMS_P(o2->data,animation.attribute)) ; */

			  break;

  case EMS_TYPE_PIC:	   
 {
   int i, len=EMS_P(o1->data,picture.pdu_length);
   ASSERT(EMS_P(o1->data,picture.pdu_length)   == EMS_P(o2->data,picture.pdu_length));
   ASSERT(EMS_P(o1->data,picture.hDim) == EMS_P(o2->data,picture.hDim));
   ASSERT(EMS_P(o1->data,picture.vDim) == EMS_P(o2->data,picture.vDim));
   ASSERT(EMS_P(o1->data,picture.bitsPerPixel) == EMS_P(o2->data,picture.bitsPerPixel));
   /* ASSERT(EMS_P(o1->data,picture.attribute)== EMS_P(o2->data,picture.attribute)) ; */

   for(i=0;i<len;i++)
  if(EMS_P(o1->data,picture.pdu[i]) != EMS_P(o2->data,picture.pdu[i]))
 ASSERT(0);
 }
   break;
  
  case EMS_TYPE_USERDEF_SND:
 {
   int i, len=EMS_P(o1->data,sound.pdu_length);
   ASSERT(EMS_P(o1->data,sound.pdu_length) == EMS_P(o2->data,sound.pdu_length));
   /* ASSERT(EMS_P(o1->data,sound.attribute)  == EMS_P(o2->data,sound.attribute)) ; */

   for(i=0;i<len;i++)
  if(EMS_P(o1->data,sound.pdu[i]) != EMS_P(o2->data,sound.pdu[i]))
 ASSERT(0);
 }  
   break; 
   }
  
}

/* 0: the same */
kal_bool EMSObjectCmp(EMSObject *o1, EMSObject *o2)
{
   if(o1==NULL && o2==NULL)
  return 1;

   if( !(o1!=NULL && o2!=NULL)) return 1;

   if(o1->Type!=o2->Type) return 1;

   if(o1->OffsetToText!=o2->OffsetToText) return 1;

#ifdef __EMS_REL5__
   if(EXT_OBJ_INFO_P(o1, isExtObj) != EXT_OBJ_INFO_P(o2, isExtObj) ) return 1;
   if(EXT_OBJ_INFO_P(o1, isReusedObj) != EXT_OBJ_INFO_P(o2, isReusedObj) ) return 1;
   if(EXT_OBJ_INFO_P(o1, ExtObjRef) != EXT_OBJ_INFO_P(o2, ExtObjRef) ) return 1;
#endif

   switch(o1->Type)
   {
  case EMS_TYPE_TEXT_FORMAT:
   if(EMS_P(o1->data,text_format.FontSize)!= EMS_P(o2->data,text_format.FontSize)) return 1;
   if(EMS_P(o1->data,text_format.Alignment)   != EMS_P(o2->data,text_format.Alignment)) return 1;
   if(EMS_P(o1->data,text_format.isBold)  != EMS_P(o2->data,text_format.isBold)) return 1;
   if(EMS_P(o1->data,text_format.isItalic)!= EMS_P(o2->data,text_format.isItalic)) return 1;
   if(EMS_P(o1->data,text_format.isUnderline) != EMS_P(o2->data,text_format.isUnderline)) return 1;
   if(EMS_P(o1->data,text_format.isStrikethrough) != EMS_P(o2->data,text_format.isStrikethrough)) return 1;
   if(EMS_P(o1->data,text_format.textLength)  != EMS_P(o2->data,text_format.textLength)) return 1;
   
   if(EMS_P(o1->data,text_format.bgColor) != EMS_P(o2->data,text_format.bgColor)) return 1;
   if(EMS_P(o1->data,text_format.fgColor) != EMS_P(o2->data,text_format.fgColor)) return 1;
   break;

  case EMS_TYPE_USERDEF_ANM:		
 {
   int i, len=EMS_P(o1->data,animation.pdu_length);
   if(EMS_P(o1->data,animation.pdu_length)  != EMS_P(o2->data,animation.pdu_length)) return 1;  
   if(EMS_P(o1->data,animation.hDim)!= EMS_P(o2->data,animation.hDim)) return 1;
   if(EMS_P(o1->data,animation.vDim)!= EMS_P(o2->data,animation.vDim)) return 1;
   if(EMS_P(o1->data,animation.bitsPerPixel)!= EMS_P(o2->data,animation.bitsPerPixel)) return 1;
   if(EMS_P(o1->data,animation.cFrame)  != EMS_P(o2->data,animation.cFrame)) return 1;
   if(EMS_P(o1->data,animation.cRepeat) != EMS_P(o2->data,animation.cRepeat)) return 1;
   if(EMS_P(o1->data,animation.Duration)!= EMS_P(o2->data,animation.Duration)) return 1;
   if(EMS_P(o1->data,animation.attribute)   != EMS_P(o2->data,animation.attribute)) return 1;

   for(i=0;i<len;i++)
  if(EMS_P(o1->data,animation.pdu[i]) != EMS_P(o2->data,animation.pdu[i]))
 return 1;
 }
			  break;

  case EMS_TYPE_PIC:	   
 {
   int i, len=EMS_P(o1->data,picture.pdu_length);
   if(EMS_P(o1->data,picture.pdu_length)   != EMS_P(o2->data,picture.pdu_length)) return 1;
   if(EMS_P(o1->data,picture.hDim) != EMS_P(o2->data,picture.hDim)) return 1;
   if(EMS_P(o1->data,picture.vDim) != EMS_P(o2->data,picture.vDim)) return 1;
   if(EMS_P(o1->data,picture.bitsPerPixel) != EMS_P(o2->data,picture.bitsPerPixel)) return 1;
   if(EMS_P(o1->data,picture.attribute)!= EMS_P(o2->data,picture.attribute)) return 1;

   for(i=0;i<len;i++)
  if(EMS_P(o1->data,picture.pdu[i]) != EMS_P(o2->data,picture.pdu[i]))
 return 1;
 }
   break;
  
  case EMS_TYPE_USERDEF_SND:
 {
   int i, len=EMS_P(o1->data,sound.pdu_length);
   if(EMS_P(o1->data,sound.pdu_length) != EMS_P(o2->data,sound.pdu_length)) return 1;
   if(EMS_P(o1->data,sound.attribute)  != EMS_P(o2->data,sound.attribute)) return 1;

   for(i=0;i<len;i++)
  if(EMS_P(o1->data,sound.pdu[i]) != EMS_P(o2->data,sound.pdu[i]))
 return 1;
 }  
   break; 
   }
  
   return 0;
}

/* 0: the same */
kal_bool EMSDataCmp(EMSData *d1, EMSData *d2, kal_uint8 cmpCurrentPosition)
{
   kal_uint16 i;
   EMSObject *obj1, *obj2;
   EMSInternalData *i1, *i2;

   if(d1->dcs!= d2->dcs) return 1;
   if(d1->isConcat   != d2->isConcat   ) return 1;
   if(d1->udhi   != d2->udhi   ) return 1;
   if(d1->textLength != d2->textLength ) return 1;

   if(cmpCurrentPosition)
   {
  if(d1->CurrentPosition.OffsetToText != d2->CurrentPosition.OffsetToText) return 1;

  if(EMSObjectCmp(d1->CurrentPosition.Object, d2->CurrentPosition.Object) != 0) return 1;

  if(EMSObjectCmp(d1->CurrentTextFormatObj, d2->CurrentTextFormatObj) != 0) return 1;
   }

   i1 = (EMSInternalData*)d1->internal;
   i2 = (EMSInternalData*)d2->internal;

   if(i1->totalObjOctet != i2->totalObjOctet) return 1;
   if(i1->numOfEMSObject != i2->numOfEMSObject) return 1;

   for(i = 0 ; i < d1->textLength ; i++)   
  if(d1->textBuffer[i] != d2->textBuffer[i]) 
 return 1;

   for(obj1=d1->listHead,obj2=d2->listHead ; obj1!=NULL||obj2!=NULL ; obj1=obj1->next,obj2=obj2->next)
   {
  if(!(obj1!=NULL && obj2!=NULL)) return 1;
  
  if(EMSObjectCmp(obj1,obj2)!=0)
 return 1;
   }

   if (EMSReminderVerdict(&(d1->Reminder) ,&(d2->Reminder))!=0)
  return 1;

   return 0;//the same
}

void EMSDataVerdict(EMSData *d1, EMSData *d2)
{
   kal_uint16 i;
   EMSObject *obj1, *obj2;
   EMSInternalData *i1, *i2;

   ASSERT(d1->dcs== d2->dcs);
   ASSERT(d1->isConcat   == d2->isConcat   );
   ASSERT(d1->udhi   == d2->udhi   );
   ASSERT(d1->textLength == d2->textLength );

   ASSERT(d1->CurrentPosition.OffsetToText == d2->CurrentPosition.OffsetToText);

   EMSObjectVerdict(d1->CurrentPosition.Object, d2->CurrentPosition.Object);
   EMSObjectVerdict(d1->CurrentTextFormatObj, d2->CurrentTextFormatObj);

   i1 = (EMSInternalData*)d1->internal;
   i2 = (EMSInternalData*)d2->internal;

   ASSERT(i1->totalObjOctet == i2->totalObjOctet);
   ASSERT(i1->numOfEMSObject == i2->numOfEMSObject);

   for(i = 0 ; i < d1->textLength ; i++)   
  ASSERT(d1->textBuffer[i] == d2->textBuffer[i]);

   for(obj1=d1->listHead,obj2=d2->listHead ; obj1!=NULL||obj2!=NULL ; obj1=obj1->next,obj2=obj2->next)
   {
  ASSERT(obj1!=NULL && obj2!=NULL);
  EMSObjectVerdict(obj1,obj2);
   }
}

void EMSTestLargePic(void)
{
   EMSObject pic;
   EMSObjData objData;   
   EMSTATUS ret;  
   kal_uint8 numOfMsg;
   kal_uint8 offset;
   
   InitializeEMSData(&emsData, 0);   

   objData.picture.vDim = 16; 
   objData.picture.hDim = 16;
   objData.picture.pdu_length = 32;
   objData.picture.pdu = small_pic1;
   objData.picture.bitsPerPixel = 1; /* mono picture */
   
   pic.Type=EMS_TYPE_PIC;
   pic.OffsetToText=0;

   pic.data=&objData;

   ret = AddObject(&emsData, EMS_TYPE_PIC, &objData,0);
   ASSERT(ret==EMS_OK);
  
   ret = EMSPack(&emsData,1, &numOfMsg, TPUD_p, TPUDLen);
   ASSERT(ret==EMS_OK);

   /* check verdicts */
   ASSERT(numOfMsg == 1);
   ASSERT(TPUDLen[0] == (1+/*udhl*/
 3+32/* ie */));

   ASSERT(TPUD_p[0][0]==35);//udhl

   offset=1;
   ASSERT(TPUD_p[0][offset++]==EMS_SMALL_PIC_IEI);
   ASSERT(TPUD_p[0][offset++]==33);
   ASSERT(TPUD_p[0][offset++]==0);//start position

   ReleaseEMSData(&emsData);

   /* unpack */
   InitializeEMSData(&emsData, 0);
   emsData.udhi=1;
   ret = EMSUnPack(&emsData,emsData.udhi, numOfMsg, TPUD_p, TPUDLen);
   ASSERT(ret==EMS_OK);
   ASSERT(emsData.listHead != NULL);
   ASSERT(emsData.listHead == emsData.listTail);

   EMSObjectVerdict(emsData.listHead, &pic);

   ReleaseEMSData(&emsData);
   EMSResourceVerdict();
   
}

void EMSTestVarPic(void)
{
   EMSData   emsData1;
   EMSObject pic;
   EMSObjData objData;   
   EMSTATUS ret;  
   kal_uint8 numOfMsg;
   kal_uint8 TPUD[160];
   kal_uint8 TPUDLen[4];
   kal_uint8 *TPUD_p[4];
   kal_uint8 offset;
   
   TPUD_p[0] = TPUD;
   TPUD_p[1] = NULL;
   TPUD_p[2] = NULL;
   TPUD_p[3] = NULL;

   InitializeEMSData(&emsData, 0);   

   objData.picture.vDim = 25; 
   objData.picture.hDim = 40;
   objData.picture.pdu_length = 125;
   objData.picture.pdu = small_pic1;
   objData.picture.bitsPerPixel = 1; /* mono picture */
   
   pic.Type=EMS_TYPE_PIC;
   pic.OffsetToText=0;

   pic.data=&objData;

   ret = AddObject(&emsData, EMS_TYPE_PIC, &objData,0);
   ASSERT(ret==EMS_OK);
  
   InitializeEMSData(&emsData1, 0);  

   CopyEMSData(&emsData1, &emsData);

	ReleaseEMSData(&emsData1);	
	ReleaseEMSData(&emsData);

   EMSResourceVerdict();

#if 0
   ret = EMSPack(&emsData,1, &numOfMsg, TPUD_p, TPUDLen);
   ASSERT(ret==EMS_OK);

   /* check verdicts */
   ASSERT(numOfMsg == 1);
  
   ASSERT(TPUDLen[0] == (1+/*udhl*/
 3+32/* ie */));

   ASSERT(TPUD[0]==35);//udhl

   offset=1;
   ASSERT(TPUD[offset++]==EMS_SMALL_PIC_IEI);
   ASSERT(TPUD[offset++]==33);
   ASSERT(TPUD[offset++]==0);//start position

   ReleaseEMSData(&emsData);

   /* unpack */
   InitializeEMSData(&emsData, 0);
   emsData.udhi=1;
   ret = EMSUnPack(&emsData,emsData.udhi, numOfMsg, TPUD_p, TPUDLen);
   ASSERT(ret==EMS_OK);
   ASSERT(emsData.listHead != NULL);
   ASSERT(emsData.listHead == emsData.listTail);

   EMSObjectVerdict(emsData.listHead, &pic);

   ReleaseEMSData(&emsData);
#endif
}

void EMSTestBasicObject(void)
{
   /* predefined animation 0-14 */
   EMSTestPredefAnm();

   /* large animation */
   EMSTestLargefAnm();

   /* small animation */
   EMSTestSmallfAnm();

   /* predefined sound 0-9 */
   EMSTestPredefSnd();

   /* user defined sound */

   /* large picture */
   EMSTestLargePic();

   /* small picture */
   

   /* variable picture */
   EMSTestVarPic();

   
}


void ems_test_1(void)
{
   EMSData emsData;
   EMSTATUS ret;
   EMSTextFormat tf;
   EMSObjData data1 ;

   kal_uint8 str[10];
   kal_uint8 pdu[127];

   InitializeEMSData(&emsData, 0);

   // Melody1
   data1.sound.pdu_length = 127;
   data1.sound.pdu = pdu;
   ret = AddObject(&emsData, EMS_TYPE_USERDEF_SND, &data1, 0);
   ASSERT(ret==EMS_OK);
   ASSERT(emsData.CurrentPosition.Object != NULL&&
  emsData.CurrentPosition.Object->Type == EMS_TYPE_USERDEF_SND);
   ASSERT(emsData.CurrentTextFormatObj == NULL);

   // Melody2
   data1.sound.pdu_length = 127;
   data1.sound.pdu = pdu;
   ret = AddObject(&emsData, EMS_TYPE_USERDEF_SND, &data1, 0);
   ASSERT(ret==EMS_OK);
   ASSERT(emsData.CurrentPosition.Object != NULL&&
  emsData.CurrentPosition.Object->Type == EMS_TYPE_USERDEF_SND);
   ASSERT(emsData.CurrentTextFormatObj == NULL);

   // Text
   str[0]=0;
   str[1]='1';
   str[2]=0;
   str[3]='2';
   ret = AddString(&emsData, str, 2, NULL);
   ASSERT(ret==EMS_OK);
   ASSERT(emsData.CurrentPosition.Object != NULL&&
  emsData.CurrentPosition.Object->Type == EMS_TYPE_USERDEF_SND);
   ASSERT(emsData.CurrentTextFormatObj == NULL);

   BackwardCurrentPosition(&emsData, 1);
   CancelCurrentPosition(&emsData, 1);

   // Text
   ResetTextFormat(&tf);
   tf.isItalic = KAL_TRUE;
   ret = AddString(&emsData, str, 1, &tf);
   ASSERT(ret==EMS_OK);
   ASSERT(emsData.CurrentPosition.Object != NULL&&
  emsData.CurrentPosition.Object->Type == EMS_TYPE_TEXT_FORMAT);
   ASSERT(emsData.CurrentTextFormatObj != NULL &&
  EMS_P(emsData.CurrentTextFormatObj->data,text_format.isItalic)==KAL_TRUE &&
  EMS_P(emsData.CurrentTextFormatObj->data,text_format.textLength)==2);

   ReleaseEMSData(&emsData);
   EMSResourceVerdict();
}

void ems_test_2(void)
{

   EMSData emsData;
   EMSTATUS ret;
   EMSTextFormat tf;
   EMSObjData data1 ;
   kal_uint16 n;
   kal_uint8 str[10];
   kal_uint8 pdu[127];

   InitializeEMSData(&emsData, 0);

   // Melody1
	
   data1.sound.pdu_length = 127;
   data1.sound.pdu = pdu;
   ret = AddObject(&emsData, EMS_TYPE_USERDEF_SND, &data1, 0);
   ASSERT(ret==EMS_OK);

   // Text
   str[0]=0;
   str[1]='1';
   str[2]=0;
   str[3]='2';
   ret = AddString(&emsData, str, 2, NULL);

   n=BackwardCurrentPosition(&emsData, 1);
   ASSERT(n==1);

   ///////////////////
   // remove '1'
   ///////////////////
   n=CancelCurrentPosition(&emsData, 1);

   ////////////////////////////////
   // add '1' with Italic style
   ////////////////////////////////
   ResetTextFormat(&tf);
   tf.isItalic = KAL_TRUE;
   ret = AddString(&emsData, str, 1, &tf);
   ASSERT(ret==EMS_OK);
   ASSERT(emsData.CurrentTextFormatObj != NULL &&
  EMS_P(emsData.CurrentTextFormatObj->data,text_format.isItalic)==KAL_TRUE &&
  EMS_P(emsData.CurrentTextFormatObj->data,text_format.textLength)==2);

   n=ForwardCurrentPosition(&emsData, 1);
   ASSERT(n==1);
   ASSERT(emsData.CurrentTextFormatObj == NULL);

   ///////////////////
   // remove '1'
   ///////////////////
   n=CancelCurrentPosition(&emsData, 1);
   ASSERT(n==1);
   ASSERT(emsData.CurrentTextFormatObj != NULL &&
  EMS_P(emsData.CurrentTextFormatObj->data,text_format.isItalic)==KAL_TRUE &&
  EMS_P(emsData.CurrentTextFormatObj->data,text_format.textLength)==2);

   ////////////////////////////////
   // add '2' with Italic style
   ////////////////////////////////
   str[0]=0;
   str[1]='2';
   ret = AddString(&emsData, str, 1, &tf);
   ASSERT(ret==EMS_OK);

   ASSERT(emsData.listHead->Type==EMS_TYPE_USERDEF_SND);
   ASSERT(emsData.listHead->next->Type==EMS_TYPE_TEXT_FORMAT);
   ASSERT(EMS_P(emsData.listHead->next->data,text_format.textLength)==4);
   ReleaseEMSData(&emsData);
   EMSResourceVerdict();
}

void ems_test_3(void)
{
   EMSData emsData;
   EMSTATUS ret;
   EMSTextFormat tf;
   EMSObjData data1 ;
  
   kal_uint8 str[10];
   kal_uint8 pdu[127];

   InitializeEMSData(&emsData, 0);

   // Melody1

   data1.sound.pdu_length = 127;
   data1.sound.pdu = pdu;
   ret = AddObject(&emsData, EMS_TYPE_USERDEF_SND, &data1, 0);
   ASSERT(ret==EMS_OK);

   // Text
   ResetTextFormat(&tf);
   tf.isItalic = KAL_TRUE;
   str[0]=0;
   str[1]='1';
   ret = AddString(&emsData, str, 1, &tf);
   ASSERT(ret==EMS_OK);
   ASSERT(emsData.CurrentPosition.Object != NULL&&
  emsData.CurrentPosition.Object->Type == EMS_TYPE_TEXT_FORMAT);
   ASSERT(emsData.CurrentTextFormatObj != NULL &&
  EMS_P(emsData.CurrentTextFormatObj->data,text_format.isItalic)==KAL_TRUE &&
  EMS_P(emsData.CurrentTextFormatObj->data,text_format.textLength)==2);
	
   data1.sound.pdu_length = 127;
   data1.sound.pdu = pdu;
   ret = AddObject(&emsData, EMS_TYPE_USERDEF_SND, &data1, 0);
   ASSERT(ret==EMS_OK);
   ASSERT(emsData.CurrentPosition.Object != NULL&&
  emsData.CurrentPosition.Object->Type == EMS_TYPE_USERDEF_SND);
   ASSERT(emsData.CurrentTextFormatObj != NULL &&
  EMS_P(emsData.CurrentTextFormatObj->data,text_format.isItalic)==KAL_TRUE &&
  EMS_P(emsData.CurrentTextFormatObj->data,text_format.textLength)==2);

   ReleaseEMSData(&emsData);
	
   EMSResourceVerdict();
}

/* 
 * text format boundary test
 *
 * <snd> 1 <snd> 2 ("12": formatted text)
 * seg1  seg2  seg3  seg4
 */
void ems_test_4()
{
   EMSData emsData;
   EMSTATUS ret;
   EMSTextFormat tf;
   EMSObjData data1 ;
   kal_uint8 str[10];
   kal_uint8 pdu[127];
   kal_uint8 numOfMsg;   
   kal_uint8 i;

   InitializeEMSData(&emsData, 0);

   // Melody1
	
   data1.sound.pdu_length = 127;
   data1.sound.pdu = pdu;
   ret = AddObject(&emsData, EMS_TYPE_USERDEF_SND, &data1, 0);
   ASSERT(ret==EMS_OK);

   // Text1
   ResetTextFormat(&tf);
   tf.isItalic = KAL_TRUE;
   str[0]=0;
   str[1]='1';
   ret = AddString(&emsData, str, 1, &tf);
   ASSERT(ret==EMS_OK);

   // Melody2

   data1.sound.pdu_length = 127;
   data1.sound.pdu = pdu;
   ret = AddObject(&emsData, EMS_TYPE_USERDEF_SND, &data1, 0);
   ASSERT(ret==EMS_OK);

	
   // Text2
   str[0]=0;
   str[1]='1';
   ret = AddString(&emsData, str, 1, &tf);

#ifdef OPTIMIZE_TXT_FMT
   ASSERT(emsData.CurrentPosition.Object != NULL&&
  emsData.CurrentPosition.Object->Type == EMS_TYPE_USERDEF_SND);
   ASSERT(emsData.CurrentTextFormatObj != NULL &&
  EMS_P(emsData.CurrentTextFormatObj->data,text_format.isItalic)==KAL_TRUE &&
  EMS_P(emsData.CurrentTextFormatObj->data,text_format.textLength)==4);
#else
   ASSERT(emsData.CurrentTextFormatObj != NULL &&
  EMS_P(emsData.CurrentTextFormatObj->data,text_format.isItalic)==KAL_TRUE &&
  EMS_P(emsData.CurrentTextFormatObj->data,text_format.textLength)==2);
#endif
  
   ret=EMSPack(&emsData,1, &numOfMsg, TPUD_p, TPUDLen);
   ASSERT(ret==EMS_OK);
   ASSERT(numOfMsg==4);

   /* verdict seg1 */
   ASSERT(TPUD_p[0][0]==135);/* udhl */
   ASSERT(TPUD_p[0][1]==0);/* concat iei */
   ASSERT(TPUD_p[0][2]==3);/* concat iedl */
   ASSERT(TPUD_p[0][4]==4);/* total seg */
   ASSERT(TPUD_p[0][5]==1);/* seg */
   ASSERT(TPUD_p[0][6]==0x0c); /* txt fmt iei */
   ASSERT(TPUD_p[0][7]==128); /* txt fmt iedl */
   ASSERT(TPUD_p[0][8]==0); /* offset */

   /* verdict seg2 */
   ASSERT(TPUD_p[1][0]==10);/* udhl */
   ASSERT(TPUD_p[1][1]==0);/* concat iei */
   ASSERT(TPUD_p[1][2]==3);/* concat iedl */
   ASSERT(TPUD_p[1][4]==4);/* total seg */
   ASSERT(TPUD_p[1][5]==2);/* seg */
   ASSERT(TPUD_p[1][6]==0x0a); /* txt fmt iei */
   ASSERT(TPUD_p[1][7]==3); /* txt fmt iedl */
   ASSERT(TPUD_p[1][8]==0); /* offset */
   ASSERT(TPUD_p[1][9]==1); /* txt fmt len */
   ASSERT(TPUD_p[1][10]==0x23); /* txt fmt mode */

   /* verdict seg3 */
   ASSERT(TPUD_p[2][0]==135);/* udhl */
   ASSERT(TPUD_p[2][1]==0);/* concat iei */
   ASSERT(TPUD_p[2][2]==3);/* concat iedl */
   ASSERT(TPUD_p[2][4]==4);/* total seg */
   ASSERT(TPUD_p[2][5]==3);/* seg */
   ASSERT(TPUD_p[2][6]==0x0c); /* txt fmt iei */
   ASSERT(TPUD_p[2][7]==128); /* txt fmt iedl */
   ASSERT(TPUD_p[2][8]==0); /* offset */

   /* verdict seg4 */
   ASSERT(TPUD_p[3][0]==10);/* udhl */
   ASSERT(TPUD_p[3][1]==0);/* concat iei */
   ASSERT(TPUD_p[3][2]==3);/* concat iedl */
   ASSERT(TPUD_p[3][4]==4);/* total seg */
   ASSERT(TPUD_p[3][5]==4);/* seg */
   ASSERT(TPUD_p[3][6]==0x0a); /* txt fmt iei */
   ASSERT(TPUD_p[3][7]==3); /* txt fmt iedl */
   ASSERT(TPUD_p[3][8]==0); /* offset */
   ASSERT(TPUD_p[3][9]==1); /* txt fmt len */
   ASSERT(TPUD_p[3][10]==0x23); /* txt fmt mode */

   //EMSDataDump(&emsData);

   ReleaseEMSData(&emsData);
   EMSResourceVerdict();
}

void ems_test_5()
{
   EMSData emsData;
   EMSTATUS ret;
   EMSTextFormat tf;
   EMSObjData data1 ;
   kal_uint8 str[10];
   kal_uint8 pdu[127];

   InitializeEMSData(&emsData, 0);

   // Melody1	
   data1.sound.pdu_length = 127;
   data1.sound.pdu = pdu;
   ret = AddObject(&emsData, EMS_TYPE_USERDEF_SND, &data1, 0);
   ASSERT(ret==EMS_OK);

   BackwardCurrentPosition(&emsData, 1);

   // Text1
   ResetTextFormat(&tf);
   tf.isItalic = KAL_TRUE;
   str[0]=0;
   str[1]='1';
   ret = AddString(&emsData, str, 1, &tf);
   ASSERT(ret==EMS_OK);
   ASSERT(emsData.CurrentPosition.Object != NULL&&
  emsData.CurrentPosition.Object->Type == EMS_TYPE_TEXT_FORMAT);
   ASSERT(emsData.CurrentTextFormatObj != NULL &&
  EMS_P(emsData.CurrentTextFormatObj->data,text_format.isItalic)==KAL_TRUE &&
  EMS_P(emsData.CurrentTextFormatObj->data,text_format.textLength)==2);

   EMSDataDump(&emsData);

   ReleaseEMSData(&emsData);
   EMSResourceVerdict();
}

void ems_test_6()
{
   EMSData emsData;
   EMSTATUS ret;   
   EMSObjData data1 ;
   kal_uint8 str[10];
   kal_uint8 pdu[127];

   InitializeEMSData(&emsData, 0);

   // Text1
#ifdef EMS_LITTLE_ENDIAN
   str[0]='1';
   str[1]=0;
#else
   str[0]=0;
   str[1]='1';
#endif
   ret = AddString(&emsData, str, 1, NULL);
   ASSERT(ret==EMS_OK);

   // Melody1
	
   data1.sound.pdu_length = 127;
   data1.sound.pdu = pdu;
   ret = AddObject(&emsData, EMS_TYPE_USERDEF_SND, &data1, 0);
   ASSERT(ret==EMS_OK);

   BackwardCurrentPosition(&emsData, 1);

	// Text2
#ifdef EMS_LITTLE_ENDIAN	
   str[0]='2';
   str[1]=0;
#else
   str[0]=0;
   str[1]='2';
#endif
   ret = AddString(&emsData, str, 1, NULL);
	ASSERT(ret==EMS_OK);

   /* check text part */
   ASSERT(emsData.textLength==4);

#ifdef EMS_LITTLE_ENDIAN
   ASSERT(*emsData.textBuffer=='1' &&
  *(emsData.textBuffer+1)==0 &&
  *(emsData.textBuffer+2)=='2' &&
  *(emsData.textBuffer+3)==0
  );
#else
   ASSERT(*emsData.textBuffer==0 &&
  *(emsData.textBuffer+1)=='1' &&
  *(emsData.textBuffer+2)==0 &&
  *(emsData.textBuffer+3)=='2'
  );
#endif

   /* check object part */
   ASSERT(emsData.listHead->OffsetToText==4);

   EMSDataDump(&emsData);

   ReleaseEMSData(&emsData);
   EMSResourceVerdict();

}

void ems_test_7()
{
   EMSData emsData;
   EMSTATUS ret;
   EMSTextFormat tf;
   EMSObjData data1 ;
   kal_uint8 str[10];
   kal_uint8 pdu[127];

   InitializeEMSData(&emsData, 0);

   // Text1
#ifdef EMS_LITTLE_ENDIAN
   str[0]='1';
   str[1]=0;
#else
   str[0]=0;
   str[1]='1';
#endif
   ret = AddString(&emsData, str, 1, NULL);
   ASSERT(ret==EMS_OK);

   // Melody1	
   data1.sound.pdu_length = 127;
   data1.sound.pdu = pdu;
   ret = AddObject(&emsData, EMS_TYPE_USERDEF_SND, &data1, 0);
   ASSERT(ret==EMS_OK);

   BackwardCurrentPosition(&emsData, 1);

   // Text2
   ResetTextFormat(&tf);
   tf.isItalic = KAL_TRUE;
#ifdef EMS_LITTLE_ENDIAN
   str[0]='2';
   str[1]=0;
#else
   str[0]=0;
   str[1]='2';
#endif
   ret = AddString(&emsData, str, 1, &tf);
   ASSERT(ret==EMS_OK);

   /* check text part */
   ASSERT(emsData.textLength==4);
#ifdef EMS_LITTLE_ENDIAN
   ASSERT(*emsData.textBuffer=='1' &&
  *(emsData.textBuffer+1)==0 &&
  *(emsData.textBuffer+2)=='2' &&
  *(emsData.textBuffer+3)==0
  );
#else
   ASSERT(*emsData.textBuffer==0 &&
  *(emsData.textBuffer+1)=='1' &&
  *(emsData.textBuffer+2)==0 &&
  *(emsData.textBuffer+3)=='2'
  );
#endif
   /* check object part */
   ASSERT((kal_uint32*)emsData.listHead == (kal_uint32*)emsData.CurrentTextFormatObj);
   ASSERT(emsData.listHead->Type==EMS_TYPE_TEXT_FORMAT);
   ASSERT(emsData.listHead->OffsetToText==2);
   ASSERT(EMS_P(emsData.listHead->data,text_format.textLength)==2);
   ASSERT(emsData.listHead->next->Type==EMS_TYPE_USERDEF_SND);   
   ASSERT(emsData.listHead->next->OffsetToText==4);

   EMSDataDump(&emsData);

   ReleaseEMSData(&emsData);
   EMSResourceVerdict();

}

void ems_test_8()
{
   EMSData emsData;
   EMSTATUS ret;
   EMSTextFormat tf;   
   kal_uint8 str[10];
	
   InitializeEMSData(&emsData, 0);

   // Text1   
#ifdef EMS_LITTLE_ENDIAN
   str[0]='1';
   str[1]=0;
#else
   str[0]=0;
   str[1]='1';
#endif
   ret = AddString(&emsData, str, 1, NULL);
   ASSERT(ret==EMS_OK);

   // Text2	
   ResetTextFormat(&tf);
   tf.isBold = KAL_TRUE;
#ifdef EMS_LITTLE_ENDIAN
   str[0]='3';
   str[1]=0;
#else
   str[0]=0;
   str[1]='3';
#endif
   ret = AddString(&emsData, str, 1, &tf);
   ASSERT(ret==EMS_OK);

   BackwardCurrentPosition(&emsData, 1);

   // Text3
   ResetTextFormat(&tf);
   tf.isItalic = KAL_TRUE;
#ifdef EMS_LITTLE_ENDIAN
   str[0]='2';
   str[1]=0;
#else
   str[0]=0;
   str[1]='2';
#endif
   ret = AddString(&emsData, str, 1, &tf);
   ASSERT(ret==EMS_OK);

   /* check text part */
   ASSERT(emsData.textLength==6);
#ifdef EMS_LITTLE_ENDIAN
   ASSERT(*emsData.textBuffer=='1' &&
  *(emsData.textBuffer+1)==0 &&
  *(emsData.textBuffer+2)=='2' &&
  *(emsData.textBuffer+3)==0 &&
  *(emsData.textBuffer+4)=='3' &&
  *(emsData.textBuffer+5)==0
  );
#else
   ASSERT(*emsData.textBuffer==0 &&
  *(emsData.textBuffer+1)=='1' &&
  *(emsData.textBuffer+2)==0 &&
  *(emsData.textBuffer+3)=='2' &&
  *(emsData.textBuffer+4)==0 &&
  *(emsData.textBuffer+5)=='3'
  );
#endif

   /* check object part */
   ASSERT((kal_uint32*)emsData.listHead == (kal_uint32*)emsData.CurrentTextFormatObj);

   /* '2' */
   ASSERT(emsData.listHead->Type==EMS_TYPE_TEXT_FORMAT);   
   ASSERT(emsData.listHead->OffsetToText==2);
   ASSERT(EMS_P(emsData.listHead->data,text_format.textLength) == 2);
   ASSERT(EMS_P(emsData.listHead->data,text_format.isItalic)   == 1);

   /* '3' */
   ASSERT(emsData.listTail->Type==EMS_TYPE_TEXT_FORMAT);
   ASSERT(emsData.listTail->OffsetToText==4);
   ASSERT(EMS_P(emsData.listTail->data,text_format.textLength) == 2);
   ASSERT(EMS_P(emsData.listTail->data,text_format.isBold) == 1);   

   EMSDataDump(&emsData);

   ReleaseEMSData(&emsData);

   EMSResourceVerdict();
}

void ems_test_9()
{
   EMSData data;
   EMSTATUS ret;  
   EMSObjData data1;  	
   kal_uint8 pdu[127] = {0};

   InitializeEMSData(&emsData, 0);

   // Melody1	
   data1.sound.pdu_length = 127;
   data1.sound.pdu = pdu;
   ret = AddObject(&emsData, EMS_TYPE_USERDEF_SND, &data1, 0);
   ASSERT(ret==EMS_OK);
	
   InitializeEMSData(&data, 0);
   ret = CopyEMSData(&data, &emsData);
   
   EMSDataVerdict(&data, &emsData);

   ReleaseEMSData(&data);
   ReleaseEMSData(&emsData);
   EMSResourceVerdict();
}

void ems_test_10()
{
   EMSTATUS ret;
   EMSTextFormat tf;
   EMSObjData data1; 
   kal_uint8 str[10];
   kal_uint8 pdu[127] = {0};

	InitializeEMSData(&emsData, 0);

   // Melody1	
   data1.sound.pdu_length = 127;
   data1.sound.pdu = pdu;
   ret = AddObject(&emsData, EMS_TYPE_USERDEF_SND, &data1, 0);
   ASSERT(ret==EMS_OK);

   ResetTextFormat(&tf);
	tf.Alignment = EMS_ALIGN_CENTER;
	// Add "123"	
  	str[0]=0;
	str[1]='1';
  	str[2]=0;
	str[3]='2';
  	str[4]=0;
	str[5]='3';
	ret = AddString(&emsData, str, 3, &tf);

   ResetTextFormat(&tf);	
	// Add "123"	
  	str[0]=0;
	str[1]='4';
  	str[2]=0;
	str[3]='5';
  
	ret = AddString(&emsData, str, 2, &tf);

	// Melody2	
   data1.sound.pdu_length = 127;
   data1.sound.pdu = pdu;
   ret = AddObject(&emsData, EMS_TYPE_USERDEF_SND, &data1, 0);
   ASSERT(ret==EMS_OK);

   ResetCurrentPosition(&emsData);//begin
   ForwardCurrentPosition(&emsData, 1);//after melody1
   ForwardCurrentPosition(&emsData, 1);//after '1'
   ForwardCurrentPosition(&emsData, 1);//after '2'
   ForwardCurrentPosition(&emsData, 1);//after '3'
   ForwardCurrentPosition(&emsData, 1);//after '4'
   ForwardCurrentPosition(&emsData, 1);//after '5'
   ForwardCurrentPosition(&emsData, 1);//after melody2
   ReleaseEMSData(&emsData);
   EMSResourceVerdict();
}

void ems_test_11()
{
   EMSTATUS ret;
	EMSTextFormat tf;
  
   	kal_uint8 str[10];

	InitializeEMSData(&emsData, 0);

	// Add "123"
	ResetTextFormat(&tf);
  	str[0]=0;
	str[1]='1';
  	str[2]=0;
	str[3]='2';
  	str[4]=0;
	str[5]='3';
	ret = AddString(&emsData, str, 3, &tf);

	// Delet "1" & Add "1" (CENTER)
	BackwardCurrentPosition(&emsData, 2);
	CancelCurrentPosition(&emsData, 1);
	ResetTextFormat(&tf);
	tf.Alignment = EMS_ALIGN_CENTER;
	ret = AddString(&emsData, str, 1, &tf);

	// Delete "2" & Add "2" (CENTER)
	ForwardCurrentPosition(&emsData, 1);
   //BackwardCurrentPosition(&emsData, 1);
	CancelCurrentPosition(&emsData, 1);
	str[0]=0;
   str[1]='2'; 
	ret = AddString(&emsData, str, 1, &tf);

	// Delete "3" & Add "3" (CENTER)
	ForwardCurrentPosition(&emsData, 1);
	CancelCurrentPosition(&emsData, 1);
	str[0]=0;
	str[1]='3'; 
	ret = AddString(&emsData, str, 1, &tf);

	// Add "45" (CENTER)
 	str[0]=0;
	str[1]='4';
  	str[2]=0;
	str[3]='5';
	ret = AddString(&emsData, str, 2, &tf);

	CancelCurrentPosition(&emsData, 1);

   ReleaseEMSData(&emsData);
   EMSResourceVerdict();
}

void ems_test_12()
{
   EMSTATUS ret;
   EMSTextFormat tf;
   EMSObjData data1; 
   kal_uint8 str[10];
   kal_uint8 pdu[127] = {0};

	InitializeEMSData(&emsData, 0);

   

   ResetTextFormat(&tf);
	tf.Alignment = EMS_ALIGN_CENTER;
	// Add "123"	
  	str[0]=0;
	str[1]='1';
  	str[2]=0;
	str[3]='2';
  	str[4]=0;
	str[5]='3';
	ret = AddString(&emsData, str, 3, &tf);

   
   // Melody1	
   data1.sound.pdu_length = 127;
   data1.sound.pdu = pdu;
   ret = AddObject(&emsData, EMS_TYPE_USERDEF_SND, &data1, 0);
   ASSERT(ret==EMS_OK);

   BackwardCurrentPosition(&emsData, 1);

	
   ReleaseEMSData(&emsData);
   EMSResourceVerdict();
}

void ems_test_13()
{
   kal_uint16 n;
	EMSTATUS ret;
   EMSTextFormat tf;
  
   kal_uint8 str[10];
	InitializeEMSData(&emsData, 0);

	// Add "12"
	ResetTextFormat(&tf);
   EMSTestFillString(str, 2,"12");  	
  	ret = AddString(&emsData, str, 2, &tf);
   ASSERT(ret==EMS_OK);
   ASSERT(emsData.CurrentPosition.Object==NULL);
   ASSERT(emsData.CurrentTextFormatObj==NULL);

	// Add "34"
	ResetTextFormat(&tf);
	tf.isBold = KAL_TRUE;
  	EMSTestFillString(str, 2,"34");
  	ret = AddString(&emsData, str, 2, &tf);
   ASSERT(ret==EMS_OK);
   ASSERT(emsData.CurrentPosition.Object!=NULL);
   ASSERT(emsData.CurrentTextFormatObj!=NULL);
   ASSERT((kal_uint32*)emsData.CurrentTextFormatObj==(kal_uint32*)emsData.CurrentPosition.Object);
   ASSERT(emsData.CurrentTextFormatObj->OffsetToText==4);
   ASSERT(EMS_P(emsData.CurrentTextFormatObj->data,text_format.textLength)==4);
   ASSERT(EMS_P(emsData.CurrentTextFormatObj->data,text_format.isBold)==1);

	// Delet "1" & Add "1" (CENTER)
	n=BackwardCurrentPosition(&emsData, 3);
   ASSERT(n==3);
   ASSERT(emsData.CurrentPosition.Object==NULL);
   ASSERT(emsData.CurrentTextFormatObj==NULL);

	n=CancelCurrentPosition(&emsData, 1);
   ASSERT(n==1);
   ASSERT(emsData.CurrentPosition.Object==NULL);
   ASSERT(emsData.CurrentTextFormatObj==NULL);
   
	ResetTextFormat(&tf);
	tf.Alignment = EMS_ALIGN_CENTER;
	EMSTestFillString(str, 1,"1");
	ret = AddString(&emsData, str, 1, &tf);
   ASSERT(ret==EMS_OK);
   ASSERT(emsData.CurrentTextFormatObj->OffsetToText==0);
   ASSERT(EMS_P(emsData.CurrentTextFormatObj->data,text_format.textLength)==2);
   ASSERT(EMS_P(emsData.CurrentTextFormatObj->data,text_format.Alignment)==EMS_ALIGN_CENTER);

	// Delete "2" & Add "2" (CENTER)
	n=ForwardCurrentPosition(&emsData, 1);
   ASSERT(n==1);
   ASSERT(emsData.CurrentPosition.Object->Type==EMS_TYPE_TEXT_FORMAT);
   ASSERT(emsData.CurrentTextFormatObj==NULL);

	n=CancelCurrentPosition(&emsData, 1);
   ASSERT(n==1);
   ASSERT(EMS_P(emsData.CurrentTextFormatObj->data,text_format.Alignment)==EMS_ALIGN_CENTER);

	EMSTestFillString(str, 1,"2");
	ret = AddString(&emsData, str, 1, &tf);
   ASSERT(EMS_P(emsData.CurrentTextFormatObj->data,text_format.textLength)==4);
   ASSERT(EMS_P(emsData.CurrentTextFormatObj->data,text_format.Alignment)==EMS_ALIGN_CENTER);

   ASSERT(emsData.CurrentPosition.Object->next->OffsetToText==4);/*"34"*/

   // Delete "3" & Add "3" (CENTER)
	tf.isBold = KAL_TRUE;
	n=ForwardCurrentPosition(&emsData, 1);
   ASSERT(n==1);
   ASSERT(EMS_P(emsData.CurrentTextFormatObj->data,text_format.isBold)==KAL_TRUE);
   ASSERT(EMS_P(emsData.CurrentTextFormatObj->data,text_format.textLength)==4);
   ASSERT((kal_uint32*)emsData.CurrentPosition.Object==(kal_uint32*)emsData.CurrentTextFormatObj);

	n=CancelCurrentPosition(&emsData, 1);
   ASSERT(n==1);
   ASSERT(EMS_P(emsData.CurrentTextFormatObj->data,text_format.Alignment)==EMS_ALIGN_CENTER);
   ASSERT((kal_uint32*)emsData.CurrentPosition.Object==(kal_uint32*)emsData.CurrentTextFormatObj);
   ASSERT(EMS_P(emsData.CurrentTextFormatObj->next->data,text_format.isBold)==KAL_TRUE);
   ASSERT(EMS_P(emsData.CurrentTextFormatObj->next->data,text_format.textLength)==2);

	EMSTestFillString(str, 1,"3"); 
	ret = AddString(&emsData, str, 1, &tf);
   ASSERT(ret==EMS_OK);

   /* '3' bold/center */
   ASSERT(emsData.CurrentTextFormatObj->OffsetToText==4);
   ASSERT(EMS_P(emsData.CurrentTextFormatObj->data,text_format.isBold)==KAL_TRUE);
   ASSERT(EMS_P(emsData.CurrentTextFormatObj->data,text_format.Alignment)==EMS_ALIGN_CENTER);
   ASSERT(EMS_P(emsData.CurrentTextFormatObj->data,text_format.textLength)==2);
   
   EMSDataDump(&emsData);

   ReleaseEMSData(&emsData);
   EMSResourceVerdict();
}

void ems_test_14(void)
{
   kal_uint16 n;
   EMSTATUS ret;
	kal_uint8 pdu[125];
	kal_uint8 str[10];
	EMSObjData data;
   EMSTextFormat tf;

	InitializeEMSData(&emsData, 0);

	// Add Object
	data.animation.pdu = (unsigned char*)pdu;
	data.animation.pdu_length = 128;	
	data.animation.bitsPerPixel = 1;
	data.animation.hDim = 16;
	data.animation.vDim = 16;
	data.animation.cFrame = 4;
	data.animation.cRepeat = 5;
	data.animation.Duration = 2;
	ret=AddObject(&emsData, EMS_TYPE_USERDEF_ANM, &data, 0);
	ASSERT(ret==EMS_OK);
   ASSERT(emsData.CurrentPosition.Object->Type==EMS_TYPE_USERDEF_ANM);
   ASSERT(emsData.CurrentPosition.Object->next==NULL);

	// Add "1"	
	ResetTextFormat(&tf);
	tf.Alignment = EMS_ALIGN_LEFT;
  	EMSTestFillString(str, 1,"1");
	ret = AddString(&emsData, str, 1, &tf);
   ASSERT(ret==EMS_OK);
   ASSERT(emsData.CurrentTextFormatObj->OffsetToText==0);
   ASSERT(EMS_P(emsData.CurrentTextFormatObj->data, text_format.Alignment)==EMS_ALIGN_LEFT);
   ASSERT(EMS_P(emsData.CurrentTextFormatObj->data, text_format.textLength)==2);
   ASSERT((kal_uint32*)emsData.CurrentPosition.Object==(kal_uint32*)emsData.CurrentTextFormatObj);

	// Add "23"
	ResetTextFormat(&tf);
	tf.isBold = KAL_TRUE;
  	EMSTestFillString(str, 2,"23");
	ret = AddString(&emsData, str, 2, &tf);
   ASSERT(ret==EMS_OK);
   ASSERT(emsData.CurrentTextFormatObj->OffsetToText==2);
   ASSERT(EMS_P(emsData.CurrentTextFormatObj->data, text_format.isBold)==KAL_TRUE);
   ASSERT(EMS_P(emsData.CurrentTextFormatObj->data, text_format.textLength)==4);
   ASSERT((kal_uint32*)emsData.CurrentPosition.Object==(kal_uint32*)emsData.CurrentTextFormatObj);

   /* ^ : current pos */

   /* ^ anm 1 2 3 */
	ResetCurrentPosition(&emsData);
   ASSERT(emsData.CurrentTextFormatObj==NULL);
   ASSERT(emsData.CurrentPosition.Object==NULL);

   /* anm 1 2 ^ 3 */
	n=ForwardCurrentPosition(&emsData, 3);
   ASSERT(n==3);
   ASSERT(EMS_P(emsData.CurrentTextFormatObj->data, text_format.isBold)==KAL_TRUE);
   ASSERT((kal_uint32*)emsData.CurrentPosition.Object==(kal_uint32*)emsData.CurrentTextFormatObj);

   /* anm 1 ^ 2 3 */
	n=BackwardCurrentPosition(&emsData, 1);
   ASSERT(n==1);
   ASSERT(EMS_P(emsData.CurrentTextFormatObj->data, text_format.Alignment)==EMS_ALIGN_LEFT);
   ASSERT((kal_uint32*)emsData.CurrentPosition.Object==(kal_uint32*)emsData.CurrentTextFormatObj);

   /* anm ^ 1 2 3 */
	n=BackwardCurrentPosition(&emsData, 1);
   ASSERT(n==1);
   ASSERT(emsData.CurrentTextFormatObj==NULL);
   ASSERT(emsData.CurrentPosition.Object!=NULL && emsData.CurrentPosition.Object->Type==EMS_TYPE_USERDEF_ANM);

   ReleaseEMSData(&emsData);
   EMSResourceVerdict();
}

void ems_test_15(void)
{
	EMSTATUS ret;
   EMSTextFormat tf;
   kal_uint16 n;
   kal_uint8 str[10];
	
	InitializeEMSData(&emsData, 0);

	// Text [1]
	ResetTextFormat(&tf);
  	EMSTestFillString(str, 1,"1");
	ret = AddString(&emsData, str, 1, &tf);
   ASSERT(ret==EMS_OK);

	// Text [2]
	ResetTextFormat(&tf);
   tf.isItalic = KAL_TRUE;
  	EMSTestFillString(str, 1,"2");
	ret = AddString(&emsData, str, 1, &tf);
   ASSERT(ret==EMS_OK);

	// Text [3]
	ResetTextFormat(&tf);
  	EMSTestFillString(str, 1,"3");
	ret = AddString(&emsData, str, 1, &tf);
   ASSERT(ret==EMS_OK);

	n=BackwardCurrentPosition(&emsData, 2);
   ASSERT(n==2);
   ASSERT(emsData.CurrentPosition.Object==NULL);
   ASSERT(emsData.CurrentTextFormatObj==NULL);

	n=CancelCurrentPosition(&emsData, 1);		
   ASSERT(n==1);

   ReleaseEMSData(&emsData);
   EMSResourceVerdict();
}

void ems_test_16(void)
{
   EMSTATUS ret;
   kal_uint8 pdu[125];
	kal_uint8 str[10];
	EMSObjData data;
   EMSTextFormat tf;
   kal_uint16 n;

	InitializeEMSData(&emsData, 0);

	// Add Object
	data.animation.pdu = (unsigned char*)pdu;
	data.animation.pdu_length = 128;	
	data.animation.bitsPerPixel = 1;
	data.animation.hDim = 16;
	data.animation.vDim = 16;
	data.animation.cFrame = 4;
	data.animation.cRepeat = 5;
	data.animation.Duration = 2;
	ret=AddObject(&emsData, EMS_TYPE_USERDEF_ANM, &data, 0);
   ASSERT(ret==EMS_OK);
   ASSERT(emsData.CurrentPosition.Object != NULL &&
  emsData.CurrentPosition.Object->Type == EMS_TYPE_USERDEF_ANM);
   ASSERT(emsData.CurrentTextFormatObj == NULL);

	// Add "1234"	
	ResetTextFormat(&tf);
  	EMSTestFillString(str, 4,"1234");
	ret = AddString(&emsData, str, 4, &tf);
   ASSERT(ret==EMS_OK);
   
	// Add "1234"
	ResetTextFormat(&tf);
	tf.isBold = KAL_TRUE;
	ret = AddString(&emsData, str, 4, &tf);
   ASSERT(ret==EMS_OK);
   ASSERT(emsData.CurrentPosition.Object != NULL &&
  emsData.CurrentPosition.Object->Type == EMS_TYPE_TEXT_FORMAT);
   ASSERT(emsData.CurrentTextFormatObj != NULL &&
  emsData.CurrentTextFormatObj->data->text_format.isBold == KAL_TRUE);

	// Delete "1"
	n=BackwardCurrentPosition(&emsData, 7);
   ASSERT(n==7);
   ASSERT(emsData.CurrentPosition.OffsetToText == 2);
   ASSERT(emsData.CurrentPosition.Object != NULL &&
  emsData.CurrentPosition.Object->Type == EMS_TYPE_USERDEF_ANM);
   ASSERT(emsData.CurrentTextFormatObj == NULL);

	n=CancelCurrentPosition(&emsData, 1);
   ASSERT(n==1);
   ASSERT(emsData.CurrentPosition.OffsetToText == 0);

	// Add "1 - Center"	
	ResetTextFormat(&tf);
	tf.Alignment = EMS_ALIGN_CENTER;
	ret = AddString(&emsData, str, 1, &tf);
   ASSERT(ret==EMS_OK);

#ifdef EMS_TEST_VERDICT 
#undef EMS_TEST_VERDICT
#endif   

#define EMS_TEST_VERDICT(offset_)  do { ASSERT(emsData.CurrentPosition.OffsetToText == offset_);\
   ASSERT(emsData.CurrentPosition.Object != NULL && \
  emsData.CurrentPosition.Object->Type == EMS_TYPE_TEXT_FORMAT);\
   ASSERT(emsData.CurrentTextFormatObj != NULL &&\
  emsData.CurrentTextFormatObj->data->text_format.Alignment == EMS_ALIGN_CENTER);  \
   ASSERT((kal_uint32*)emsData.CurrentTextFormatObj == (kal_uint32*)emsData.CurrentPosition.Object); } while(0)

   EMS_TEST_VERDICT(2);

	n=ForwardCurrentPosition(&emsData, 1);
   ASSERT(n==1);
   ASSERT(emsData.CurrentPosition.OffsetToText == 4);
   ASSERT(emsData.CurrentPosition.Object != NULL &&
  emsData.CurrentPosition.Object->Type == EMS_TYPE_TEXT_FORMAT &&
  emsData.CurrentPosition.Object->data->text_format.Alignment == EMS_ALIGN_CENTER);
   ASSERT(emsData.CurrentTextFormatObj == NULL);

	n=CancelCurrentPosition(&emsData, 1);		
   ASSERT(n==1);
   EMS_TEST_VERDICT(2);

   ReleaseEMSData(&emsData);
   EMSResourceVerdict();
}

void ems_test_17(void)
{
	EMSTATUS ret;
   kal_uint8 pdu[125];
	kal_uint8 str[10];
   EMSObjData data;
   EMSTextFormat tf;
   kal_uint16 n;

	InitializeEMSData(&emsData, 0);

	// Add "1"
	ResetTextFormat(&tf);  
	ret = AddString(&emsData, str, 1, &tf);
   ASSERT(ret==EMS_OK);

	// Add Object	
	data.animation.pdu = (unsigned char*)pdu;
	data.animation.pdu_length = 128;	
	data.animation.bitsPerPixel = 1;
	data.animation.hDim = 16;
	data.animation.vDim = 16;
	data.animation.cFrame = 4;
	data.animation.cRepeat = 5;
	data.animation.Duration = 2;
	ret=AddObject(&emsData, EMS_TYPE_USERDEF_ANM, &data, 0);
	ASSERT(ret==EMS_OK);

	// Add "23"
	ResetTextFormat(&tf);
	tf.isBold = KAL_TRUE;  	
	ret = AddString(&emsData, str, 2, &tf);
   ASSERT(ret==EMS_OK);

#ifdef EMS_TEST_VERDICT 
#undef EMS_TEST_VERDICT
#endif
   
#define EMS_TEST_VERDICT(offset_, type_, value_)  do { ASSERT(emsData.CurrentPosition.OffsetToText == offset_);\
   ASSERT(emsData.CurrentPosition.Object != NULL && \
  emsData.CurrentPosition.Object->Type == EMS_TYPE_TEXT_FORMAT);\
   ASSERT(emsData.CurrentTextFormatObj != NULL &&\
  emsData.CurrentTextFormatObj->data->text_format.type_ == value_);  \
   ASSERT((kal_uint32*)emsData.CurrentTextFormatObj == (kal_uint32*)emsData.CurrentPosition.Object); } while(0)

   EMS_TEST_VERDICT(6, isBold, KAL_TRUE);

   // Add "ab"
	ResetTextFormat(&tf);  	
	ret = AddString(&emsData, str, 2, &tf);
   ASSERT(ret==EMS_OK);
   ASSERT(emsData.CurrentTextFormatObj == NULL);

	// Add "45"
	ResetTextFormat(&tf);
	tf.Alignment = EMS_ALIGN_CENTER;  	
	ret = AddString(&emsData, str, 2, &tf);
   ASSERT(ret==EMS_OK);
   EMS_TEST_VERDICT(14, Alignment, EMS_ALIGN_CENTER);

	n=CancelCurrentPosition(&emsData, 4);
   ASSERT(n==4);
   ASSERT(emsData.CurrentPosition.Object != NULL &&
  emsData.CurrentPosition.Object->Type == EMS_TYPE_TEXT_FORMAT &&
  TXT_FMT_LEN(emsData.CurrentPosition.Object) == 4);

   ReleaseEMSData(&emsData);
   EMSResourceVerdict();
}

void ems_test_18(void)
{
	EMSTATUS ret;
   EMSTextFormat tf;  
   kal_uint8 str[10];
   kal_uint16 n;

	InitializeEMSData(&emsData, 0);

	// Text [1]
	ResetTextFormat(&tf);
  	ret = AddString(&emsData, str, 1, &tf);
   ASSERT(ret==EMS_OK);

	// Melody1
	ret = AddObject(&emsData, EMS_TYPE_PREDEF_SND, NULL, 0);
   ASSERT(ret==EMS_OK);

	// Text [2]
	ResetTextFormat(&tf);  
   ret = AddString(&emsData, str, 1, &tf);
   ASSERT(ret==EMS_OK);

	// Delete [1]
	n=BackwardCurrentPosition(&emsData, 2);
   ASSERT(n==2);
	n=CancelCurrentPosition(&emsData, 1);
   ASSERT(n==1);

	// Text [1] - CENTER
	ResetTextFormat(&tf);
	tf.Alignment = EMS_ALIGN_CENTER;
  	ret = AddString(&emsData, str, 1, &tf);
   ASSERT(ret==EMS_OK);

#ifdef EMS_TEST_VERDICT 
#undef EMS_TEST_VERDICT
#endif   

#define EMS_TEST_VERDICT(offset_)  do { ASSERT(emsData.CurrentTextFormatObj != NULL && emsData.CurrentTextFormatObj->data->text_format.textLength==offset_); }while(0)

   EMS_TEST_VERDICT(2);

   // Delete [2]
	n=ForwardCurrentPosition(&emsData, 2);
   ASSERT(n==2);
   ASSERT(emsData.CurrentTextFormatObj == NULL);

	n=CancelCurrentPosition(&emsData, 1);
   ASSERT(n==1);
	EMS_TEST_VERDICT(2);

   // Text [2] - CENTER
	ResetTextFormat(&tf);
	tf.Alignment = EMS_ALIGN_CENTER;
  	ret = AddString(&emsData, str, 1, &tf);
   ASSERT(ret==EMS_OK);
#ifdef OPTIMIZE_TXT_FMT
   EMS_TEST_VERDICT(4);
#else
   EMS_TEST_VERDICT(2);
#endif
   ReleaseEMSData(&emsData);
   EMSResourceVerdict();
}


void ems_test_19(void)
{
	EMSTATUS ret;
	kal_uint8 str[10];
	EMSTextFormat tf;
   kal_uint16 n;

	InitializeEMSData(&emsData, 0);

	// Add Animation
	ret=AddObject(&emsData, EMS_TYPE_PREDEF_ANM, NULL, 3);
   ASSERT(ret==EMS_OK);
	
	// Add "12"	
	ResetTextFormat(&tf);  	
	ret = AddString(&emsData, str, 4, &tf);
   ASSERT(ret==EMS_OK);
   ASSERT(emsData.CurrentTextFormatObj == NULL);

	// Add "3"  	
  	ret = AddString(&emsData, str, 1, &tf);
   ASSERT(ret==EMS_OK);
   
	// Add Sound
	ret = AddObject(&emsData, EMS_TYPE_PREDEF_SND, NULL, 0);
   ASSERT(ret==EMS_OK);

	// Add "4"  	
  	ret = AddString(&emsData, str, 1, &tf);
   ASSERT(ret==EMS_OK);

	// Delete "1" & Add "1" - RIGHT
	n=BackwardCurrentPosition(&emsData, 6);
   ASSERT(n==6);
   ASSERT(emsData.CurrentTextFormatObj == NULL);

	n=CancelCurrentPosition(&emsData, 1);
   ASSERT(n==1);

	ResetTextFormat(&tf);
	tf.Alignment = EMS_ALIGN_RIGHT;  	
	ret = AddString(&emsData, str, 1, &tf);
   ASSERT(ret==EMS_OK);

#ifdef EMS_TEST_VERDICT 
#undef EMS_TEST_VERDICT
#endif   

#define EMS_TEST_VERDICT(len)  ASSERT(emsData.CurrentTextFormatObj != NULL &&\
  emsData.CurrentTextFormatObj->data->text_format.Alignment==EMS_ALIGN_RIGHT&&\
  emsData.CurrentTextFormatObj->data->text_format.textLength==len);

   EMS_TEST_VERDICT(2);

	// Delete "2" & Add "2" - RIGHT
	n=ForwardCurrentPosition(&emsData, 1);
   ASSERT(n==1);
   ASSERT(emsData.CurrentTextFormatObj == NULL);

	n=CancelCurrentPosition(&emsData, 1);
   ASSERT(n==1);
	EMS_TEST_VERDICT(2);

	ret = AddString(&emsData, str, 1, &tf);
   ASSERT(ret==EMS_OK);
   EMS_TEST_VERDICT(4);

	// Delete "0x0d 0x0a" & Add "0x0d 0x0a" - RIGHT
	n=ForwardCurrentPosition(&emsData, 2);
   ASSERT(n==2);
   ASSERT(emsData.CurrentTextFormatObj == NULL);

	n=CancelCurrentPosition(&emsData, 2);
   ASSERT(n==2);	
   EMS_TEST_VERDICT(4);
	ret = AddString(&emsData, str, 2, &tf);
   ASSERT(ret==EMS_OK); 
	EMS_TEST_VERDICT(8);

   n=ForwardCurrentPosition(&emsData, 3);
   ASSERT(n==3);
   ASSERT(emsData.CurrentTextFormatObj == NULL);

	// Delete "3" & Add "3" - Center
	n=BackwardCurrentPosition(&emsData, 2);
   ASSERT(n==2);
   ASSERT(emsData.CurrentTextFormatObj == NULL);

	n=CancelCurrentPosition(&emsData, 1);		
   ASSERT(n==1);
   EMS_TEST_VERDICT(8);

	tf.Alignment = EMS_ALIGN_CENTER;	
	ret = AddString(&emsData, str, 1, &tf);
   ASSERT(ret==EMS_OK);

	// Delete "4" & Add "4" - Center
	n=ForwardCurrentPosition(&emsData, 2);
   ASSERT(n=2);

	n=CancelCurrentPosition(&emsData, 1);	
   ASSERT(n==1);
	
	ret = AddString(&emsData, str, 1, &tf);
   ASSERT(ret==EMS_OK);

	n=BackwardCurrentPosition(&emsData, 8);	
   ASSERT(n==8);

   ReleaseEMSData(&emsData);
   EMSResourceVerdict();
}

void ems_test_20(void)
{
	EMSTATUS ret;
	kal_uint8 str[10];
   EMSTextFormat tf;
   kal_uint16 n;

	InitializeEMSData(&emsData, 0);

	// Add Animation
	ret=AddObject(&emsData, EMS_TYPE_PREDEF_ANM, NULL, 3);
	ASSERT(ret==EMS_OK);

	// Add "12"	
	ResetTextFormat(&tf);  	
	ret = AddString(&emsData, str, 2, &tf);
   ASSERT(ret==EMS_OK);

	// Delete "1" & Add "1"-CENTER
	BackwardCurrentPosition(&emsData, 1);
	CancelCurrentPosition(&emsData, 1);
	ResetTextFormat(&tf);
	tf.Alignment = EMS_ALIGN_CENTER;  	
	ret = AddString(&emsData, str, 1, &tf);
   ASSERT(ret==EMS_OK);

#ifdef EMS_TEST_VERDICT 
#undef EMS_TEST_VERDICT
#endif  
#define EMS_TEST_VERDICT(len,alig)  ASSERT(emsData.CurrentTextFormatObj != NULL &&\
  emsData.CurrentTextFormatObj->data->text_format.Alignment==alig&&\
  emsData.CurrentTextFormatObj->data->text_format.textLength==len);

   EMS_TEST_VERDICT(2,EMS_ALIGN_CENTER);

	// Delete "2" & Add "2"-CENTER
	ForwardCurrentPosition(&emsData, 1);
	CancelCurrentPosition(&emsData, 1);  	
	ret = AddString(&emsData, str, 1, &tf);
   ASSERT(ret==EMS_OK);
   EMS_TEST_VERDICT(4,EMS_ALIGN_CENTER);

	// Add "\r\n"-CENTER  	
  	ret = AddString(&emsData, str, 2, &tf);
   ASSERT(ret==EMS_OK);
   EMS_TEST_VERDICT(8,EMS_ALIGN_CENTER);

	// Add "34"	  	
	ret = AddString(&emsData, str, 2, &tf);
   ASSERT(ret==EMS_OK);
   EMS_TEST_VERDICT(12,EMS_ALIGN_CENTER);

	// Add Sound
	ret = AddObject(&emsData, EMS_TYPE_PREDEF_SND, NULL, 0);
   ASSERT(ret==EMS_OK);
   EMS_TEST_VERDICT(12,EMS_ALIGN_CENTER);
   ASSERT(emsData.CurrentPosition.Object != NULL &&
  emsData.CurrentPosition.Object->Type == EMS_TYPE_PREDEF_SND);

	// Add "56"	  	
	ret = AddString(&emsData, str, 2, &tf);
   ASSERT(ret==EMS_OK);
#ifdef OPTIMIZE_TXT_FMT
   EMS_TEST_VERDICT(16,EMS_ALIGN_CENTER);
#else
   EMS_TEST_VERDICT(4,EMS_ALIGN_CENTER);
#endif

	// Delete "3" & Add "3"-RIGHT
	n=BackwardCurrentPosition(&emsData, 4);
   ASSERT(n==4);
	n=CancelCurrentPosition(&emsData, 1);   
   ASSERT(n==1);
#ifdef OPTIMIZE_TXT_FMT
   EMS_TEST_VERDICT(14,EMS_ALIGN_CENTER);
#else
   EMS_TEST_VERDICT(10,EMS_ALIGN_CENTER);
#endif

	ResetTextFormat(&tf);
	tf.Alignment = EMS_ALIGN_RIGHT;  	
	ret = AddString(&emsData, str, 1, &tf);
   ASSERT(ret==EMS_OK);

   /*split*/
   /* EMS_TEST_VERDICT(2,EMS_ALIGN_RIGHT); */
   ASSERT(emsData.CurrentPosition.Object->prev != NULL && 
  emsData.CurrentPosition.Object->prev->Type==EMS_TYPE_TEXT_FORMAT &&
  emsData.CurrentPosition.Object->prev->OffsetToText==0 &&
  emsData.CurrentPosition.Object->prev->data->text_format.textLength==8 &&
  emsData.CurrentPosition.Object->prev->data->text_format.Alignment==EMS_ALIGN_CENTER);
#ifdef OPTIMIZE_TXT_FMT
   ASSERT(emsData.CurrentPosition.Object->next != NULL && 
  emsData.CurrentPosition.Object->next->Type==EMS_TYPE_TEXT_FORMAT &&
  emsData.CurrentPosition.Object->next->OffsetToText == 10 &&
  emsData.CurrentPosition.Object->next->data->text_format.textLength==6 &&
  emsData.CurrentPosition.Object->next->data->text_format.Alignment==EMS_ALIGN_CENTER);
   ASSERT(emsData.CurrentPosition.Object->next->next != NULL &&
  emsData.CurrentPosition.Object->next->next->Type==EMS_TYPE_PREDEF_SND);
#else
   ASSERT(emsData.CurrentPosition.Object->next != NULL && 
  emsData.CurrentPosition.Object->next->Type==EMS_TYPE_TEXT_FORMAT &&
  emsData.CurrentPosition.Object->next->OffsetToText == 10 &&
  emsData.CurrentPosition.Object->next->data->text_format.textLength==2 &&
  emsData.CurrentPosition.Object->next->data->text_format.Alignment==EMS_ALIGN_CENTER);
   ASSERT(emsData.CurrentPosition.Object->next->next != NULL &&
  emsData.CurrentPosition.Object->next->next->Type==EMS_TYPE_PREDEF_SND);
   ASSERT(emsData.CurrentPosition.Object->next->next->next != NULL && 
  emsData.CurrentPosition.Object->next->next->next->Type==EMS_TYPE_TEXT_FORMAT &&
  emsData.CurrentPosition.Object->next->next->next->OffsetToText == 12 &&
  emsData.CurrentPosition.Object->next->next->next->data->text_format.textLength==4 &&
  emsData.CurrentPosition.Object->next->next->next->data->text_format.Alignment==EMS_ALIGN_CENTER);
   
#endif
	
   // Delete "4" & Add "4"-RIGHT
	n=ForwardCurrentPosition(&emsData, 1);
   ASSERT(n==1);
#ifdef OPTIMIZE_TXT_FMT
   EMS_TEST_VERDICT(6,EMS_ALIGN_CENTER);
#else
   EMS_TEST_VERDICT(2,EMS_ALIGN_CENTER);
#endif

	n=CancelCurrentPosition(&emsData, 1);  
   ASSERT(n==1);
   EMS_TEST_VERDICT(2,EMS_ALIGN_RIGHT);
	ret = AddString(&emsData, str, 1, &tf);
   ASSERT(ret==EMS_OK);
   EMS_TEST_VERDICT(4,EMS_ALIGN_RIGHT);

	// Delete "5" & Add "5"-RIGHT
	n=ForwardCurrentPosition(&emsData, 2);
   ASSERT(n==2);
   EMS_TEST_VERDICT(4,EMS_ALIGN_CENTER);

	n=CancelCurrentPosition(&emsData, 1);  
   ASSERT(n==1);
   EMS_TEST_VERDICT(4,EMS_ALIGN_RIGHT);
   ASSERT(emsData.CurrentTextFormatObj->next != NULL &&
  emsData.CurrentTextFormatObj->next->next != NULL &&
  emsData.CurrentTextFormatObj->next->next->Type == EMS_TYPE_TEXT_FORMAT &&
  emsData.CurrentTextFormatObj->next->next->data->text_format.textLength==2);

	ret = AddString(&emsData, str, 1, &tf);
   ASSERT(ret==EMS_OK);
#ifdef OPTIMIZE_TXT_FMT
   EMS_TEST_VERDICT(6,EMS_ALIGN_RIGHT);
#else
   EMS_TEST_VERDICT(2,EMS_ALIGN_RIGHT);
#endif

	// Delete "6" & Add "6"-RIGHT
	n=ForwardCurrentPosition(&emsData, 1);
   ASSERT(n==1);
   EMS_TEST_VERDICT(2,EMS_ALIGN_CENTER);

	n=CancelCurrentPosition(&emsData, 1);
  	ASSERT(n==1);

#ifdef OPTIMIZE_TXT_FMT
   EMS_TEST_VERDICT(6,EMS_ALIGN_RIGHT);
#else
   EMS_TEST_VERDICT(2,EMS_ALIGN_RIGHT);
#endif

	ret = AddString(&emsData, str, 1, &tf);
   ASSERT(ret==EMS_OK);
#ifdef OPTIMIZE_TXT_FMT
   EMS_TEST_VERDICT(8,EMS_ALIGN_RIGHT);
#else
   EMS_TEST_VERDICT(4,EMS_ALIGN_RIGHT);
#endif

	n=BackwardCurrentPosition(&emsData, 3);
   ASSERT(n==3);
	n=BackwardCurrentPosition(&emsData, 1);	//	<-- Assert
   ASSERT(n==1);

   ReleaseEMSData(&emsData);
   EMSResourceVerdict();
}


void ems_test_21(void)
{
	EMSTATUS ret;
	kal_uint8 pdu[125];
	kal_uint8 str[10];
		EMSTextFormat tf;
  kal_uint16 n;

	InitializeEMSData(&emsData, 0);

	// Add "123"
	
	ResetTextFormat(&tf);
  	str[0]=0;
	str[1]='1';
	str[2]=0;
	str[3]='2';
	str[4]=0;
	str[5]='3';	
	ret = AddString(&emsData, str, 3, &tf);
   ASSERT(ret==EMS_OK);

	// Add Animation
	ret=AddObject(&emsData, EMS_TYPE_PREDEF_ANM, NULL, 3);
   ASSERT(ret==EMS_OK);

	// Delete "1" & Add "1"-CENTER
	n=BackwardCurrentPosition(&emsData, 3);
   ASSERT(n==3);
	n=CancelCurrentPosition(&emsData, 1);
   ASSERT(n==1);
	ResetTextFormat(&tf);
	tf.Alignment = EMS_ALIGN_CENTER;
  	str[0]=0;
   str[1]='1';
	ret = AddString(&emsData, str, 1, &tf);
	ASSERT(ret==EMS_OK);

	// Delete "2" & Add "2"-CENTER & BOLD
	n=ForwardCurrentPosition(&emsData, 1);
   ASSERT(n==1);
	n=CancelCurrentPosition(&emsData, 1);
   ASSERT(n==1);
	ResetTextFormat(&tf);
	tf.Alignment = EMS_ALIGN_CENTER;	
	tf.isBold = KAL_TRUE;
  	str[0]=0;
   str[1]='2';
	ret = AddString(&emsData, str, 1, &tf);
   ASSERT(ret==EMS_OK);

	// Delete "3" & Add "3"-CENTER
	n=ForwardCurrentPosition(&emsData, 1);
   ASSERT(n==1);
	n=CancelCurrentPosition(&emsData, 1);
   ASSERT(n==1);
	ResetTextFormat(&tf);
	tf.Alignment = EMS_ALIGN_CENTER;	
  	str[0]=0;
	str[1]='3';
	ret = AddString(&emsData, str, 1, &tf);
   ASSERT(ret==EMS_OK);

	n=BackwardCurrentPosition(&emsData, 3);
   ASSERT(n==3);

	// Delete "1" & Add "1"
	n=ForwardCurrentPosition(&emsData, 1);
   ASSERT(n==1);
	n=CancelCurrentPosition(&emsData, 1);
   ASSERT(n==1);
	ResetTextFormat(&tf);
  	str[0]=0;
	str[1]='1';
	ret = AddString(&emsData, str, 1, &tf);
	ASSERT(ret==EMS_OK);

	// Delete "2" & Add "2"-LEFT
	n=ForwardCurrentPosition(&emsData, 1);
   ASSERT(n==1);
	n=CancelCurrentPosition(&emsData, 1);
   ASSERT(n==1);
	ResetTextFormat(&tf);
	tf.Alignment = EMS_ALIGN_LEFT;
	tf.isBold = KAL_TRUE;
  	str[0]=0;
	str[1]='2';
	ret = AddString(&emsData, str, 1, &tf);
	ASSERT(ret==EMS_OK);
	
   // Delete "3" & Add "3"
	n=ForwardCurrentPosition(&emsData, 1);
	n=CancelCurrentPosition(&emsData, 1);
	ResetTextFormat(&tf);
  	str[0]=0;
	str[1]='3';
	ret = AddString(&emsData, str, 1, &tf);
	ASSERT(ret==EMS_OK);

	n=BackwardCurrentPosition(&emsData, 3);	//	<-- Access Violation
   ASSERT(n==3);
   ASSERT(emsData.listHead != NULL && 
  IS_TXT_FMT_OBJ(emsData.listHead) == KAL_TRUE &&
  emsData.listHead->data->text_format.textLength==2 &&
  emsData.listHead->data->text_format.Alignment ==EMS_ALIGN_LEFT &&
	   emsData.listHead->data->text_format.isBold == KAL_TRUE);
   ASSERT(emsData.listHead->next != NULL &&
  emsData.listHead->next->Type == EMS_TYPE_PREDEF_ANM &&
  emsData.listHead->next->PredefNo ==3);

   ReleaseEMSData(&emsData);
   EMSResourceVerdict();
}

void ems_test_22(void)
{
	EMSTATUS ret;
   kal_uint8 pdu[125];
	kal_uint8 str[10];
	kal_uint16 n;
   EMSTextFormat tf;

	InitializeEMSData(&emsData, 0);

	// Add "1"	
	ResetTextFormat(&tf);  	
	ret = AddString(&emsData, str, 1, &tf);
   ASSERT(ret==EMS_OK);

	// Add "2"
	ResetTextFormat(&tf);
	tf.isBold = KAL_TRUE;  	
	ret = AddString(&emsData, str, 1, &tf);
   ASSERT(ret==EMS_OK);

	// Add Animation
	ret = AddObject(&emsData, EMS_TYPE_PREDEF_ANM, NULL, 3);
   ASSERT(ret==EMS_OK);

	// Add "345"
	ResetTextFormat(&tf);
	tf.isBold = KAL_TRUE;  	
	ret = AddString(&emsData, str, 3, &tf);
   ASSERT(ret==EMS_OK);

	// Delete "34" & Add "34"
	n=BackwardCurrentPosition(&emsData, 1);
   ASSERT(n==1);
	n=CancelCurrentPosition(&emsData, 2);
   ASSERT(n==2);

	ResetTextFormat(&tf);  	
	ret = AddString(&emsData, str, 2, &tf);
   ASSERT(ret==EMS_OK);
   
   /* current obj shall be "2" */
   ASSERT(emsData.CurrentPosition.Object != NULL &&
  emsData.CurrentPosition.Object->prev != NULL &&
  emsData.CurrentPosition.Object->prev->OffsetToText==2);
   
	// Delete "34" & Add "34"	
	n=CancelCurrentPosition(&emsData, 2);
   ASSERT(n==2);
	ResetTextFormat(&tf);	
	tf.isUnderline = KAL_TRUE;  	
	ret = AddString(&emsData, str, 2, &tf);
   ASSERT(ret==EMS_OK);
   
	n=ForwardCurrentPosition(&emsData, 1);	//	<-- Assert
   ASSERT(n==1);
   ASSERT(emsData.listHead != NULL &&
  IS_TXT_FMT_OBJ(emsData.listHead) == KAL_TRUE &&
  emsData.listHead->data->text_format.isBold == KAL_TRUE);
   ASSERT(emsData.listHead->next != NULL &&
  emsData.listHead->next->Type == EMS_TYPE_PREDEF_ANM &&
  emsData.listHead->next->PredefNo == 3);
   ASSERT(emsData.listHead->next->next != NULL &&
  IS_TXT_FMT_OBJ(emsData.listHead->next->next) == KAL_TRUE &&
  emsData.listHead->next->next->data->text_format.isUnderline == KAL_TRUE);

   ReleaseEMSData(&emsData);
   EMSResourceVerdict();
}

void ems_test_23(void)
{
	EMSTATUS ret;
   kal_uint8 i,pdu[125];
	kal_uint8 str[10];
	kal_uint16 n;
   EMSTextFormat tf;
   EMSObjData data1;

	InitializeEMSData(&emsData, 0);

	// Add "1"	
	ResetTextFormat(&tf);  	
	ret = AddString(&emsData, str, 1, &tf);
   ASSERT(ret==EMS_OK);

	// Add "2"
	ResetTextFormat(&tf);
	tf.isBold = KAL_TRUE;  	
	ret = AddString(&emsData, str, 1, &tf);
   ASSERT(ret==EMS_OK);

	// Add Animation
   // Melody
   data1.sound.pdu_length = 127;
   data1.sound.pdu = pdu;
   ret = AddObject(&emsData, EMS_TYPE_USERDEF_SND, &data1, 0);   
   ASSERT(ret==EMS_OK);
   data1.sound.pdu_length = 127;
   data1.sound.pdu = pdu;
   //ret = AddObject(&emsData, EMS_TYPE_USERDEF_SND, &data1, 0);   
   //ASSERT(ret==EMS_OK);
   //ret = AddObject(&emsData, EMS_TYPE_PREDEF_ANM, NULL, 3);
   //ASSERT(ret==EMS_OK);
   for(i=0;i<40;i++)
   { 
	   ret = AddObject(&emsData, EMS_TYPE_PREDEF_ANM, NULL, 3);
  /* ASSERT(ret==EMS_OK); */
  if(ret != EMS_OK)
 break;
   }

	ret = AddString(&emsData, str, 3, NULL);

#ifdef OPTIMIZE_TXT_FMT
   ASSERT(ret==EMS_OK);
#else
   ASSERT(ret==EMS_OK);
#endif
   ReleaseEMSData(&emsData);
   EMSResourceVerdict();
}

void ems_test_24()
{
   EMSTATUS ret; 
   kal_uint16 i,n;
   EMSObject *obj;
   kal_uint8 *TPUD_p[4];

   /* TF -> ANM */
   kal_uint8 TPUD[160]=
   {  
  0x9,
  0x0a, 0x03, 0x00,0x03,0x01,
  0x0d, 0x02, 0x00,0x04,
  'a','b','c'
   };
   kal_uint8 TPUDLen[4]={13,0,0,0};

   TPUD_p[0]=TPUD;


   ///////////////////////////////////
   // part1
   ///////////////////////////////////
   InitializeEMSData(&emsData, 0);   

   emsData.udhi=1;
   emsData.dcs=0;

   ret=EMSUnPack(&emsData,emsData.udhi, 1, TPUD_p, TPUDLen);
   ASSERT(ret==EMS_OK);
   
   GoToEndPosition(&emsData);

   n=1;i=0;
   while(n==1)
   {  
  n = BackwardCurrentPosition(&emsData, 1);
  if(n==1)i++;
   }

   ASSERT(i==4);

   n = BackwardCurrentPosition(&emsData, 1);
   ASSERT(n==0);

   n=1;   i=0;
   while(n==1)
   {
  n = ForwardCurrentPosition(&emsData, 1);
  if(n==1)i++;
   }

   ASSERT(i==4);
   
   n = ForwardCurrentPosition(&emsData, 1);
   ASSERT(n==0);

   ReleaseEMSData(&emsData);

   ///////////////////////////////////
   // part2
   ///////////////////////////////////
   InitializeEMSData(&emsData, 0);   

   emsData.udhi=1;
   emsData.dcs=0;

   ret=EMSUnPack(&emsData,emsData.udhi, 1, TPUD_p, TPUDLen);
   ASSERT(ret==EMS_OK);
   
   GoToEndPosition(&emsData);

   n=1;i=0;
   while(n==1)
   {  
  n = BackwardCurrentPosition(&emsData, 1);
  if(n==0)break;
  n = BackwardCurrentPosition(&emsData, 1);
  if(n==0)break;
  n = ForwardCurrentPosition(&emsData, 1);
  if(n==1)i++;
   }

   n = BackwardCurrentPosition(&emsData, 1);
   ASSERT(n==0);

   n=1;   i=0;
   while(n==1)
   {
  n = ForwardCurrentPosition(&emsData, 1);
  if(n==1)i++;
   }

   ASSERT(i==4);
   
   n = ForwardCurrentPosition(&emsData, 1);
   ASSERT(n==0);

   ReleaseEMSData(&emsData);

   ///////////////////////////////////
   // part3
   ///////////////////////////////////
   InitializeEMSData(&emsData, 0);   

   emsData.udhi=1;
   emsData.dcs=0;

   ret=EMSUnPack(&emsData,emsData.udhi, 1, TPUD_p, TPUDLen);
   ASSERT(ret==EMS_OK);
   
   GoToEndPosition(&emsData);

   n=1;i=0;
   while(n==1)
   {  
  n = BackwardCurrentPosition(&emsData, 1);
  n = ForwardCurrentPosition(&emsData, 1);
  n = CancelCurrentPosition(&emsData, 1);
  
  if(n==1)i++;
   }

   ASSERT(i==4);
   
   ReleaseEMSData(&emsData);
   EMSResourceVerdict();
}
void ems_test_25(void)
{
   EMSData emsData;
   EMSObjData data;
   EMSTATUS ret;
   kal_uint8 numOfMsg=0;
   kal_uint8 pdu[128];

   ret=InitializeEMSData(&emsData, 0);
   ASSERT(ret==EMS_OK);
   data.picture.pdu_length=128;
   data.picture.bitsPerPixel=1;
   data.picture.vDim=16;
   data.picture.hDim=8*8;
   data.picture.pdu=pdu;
   ret=AddObject(&emsData, EMS_TYPE_PIC, &data, 0);
   ASSERT(ret==EMS_OK);

   ret=AddObject(&emsData, EMS_TYPE_PREDEF_ANM, NULL, 0);
   ASSERT(ret==EMS_OK);
   ret=AddObject(&emsData, EMS_TYPE_PREDEF_ANM, NULL, 0);
   ASSERT(ret==EMS_OK);

   ret=EMSPack(&emsData,1, &numOfMsg, TPUD_p, TPUDLen);
   ASSERT(ret==EMS_OK);
   ASSERT(numOfMsg==1);

   ret=ReleaseEMSData(&emsData);
   EMSResourceVerdict();
   
}

/* abonormal pre-define object */
void ems_test_26()
{
   EMSTATUS ret; 
   kal_uint16 i,n;
   EMSObject *obj;
   kal_uint8 *TPUD_p[4];
   
   kal_uint8 TPUD[160]=
   {  
  4,
  0xd, 2, 0, 0
   };
   kal_uint8 TPUDLen[4]={4,0,0,0};

   TPUD_p[0]=TPUD;
   
   InitializeEMSData(&emsData, 0); 

   emsData.udhi=1;

   ret=EMSUnPack(&emsData,emsData.udhi, 1, TPUD_p, TPUDLen);
   ASSERT(ret==EMS_OK);
  
   /* ASSERT ( ((EMSInternalData*)emsData.internal)->numOfEMSObject == 0 ); */
   /* ASSERT ( emsData.textLength == 6 );   */

   ReleaseEMSData(&emsData);
   EMSResourceVerdict();
}

void ems_test_27(void)
{
   EMSTATUS ret;
   EMSTextFormat tf;
   kal_uint16 i,n;
   kal_uint8 numOfMsg;

   InitializeEMSData(&emsData, 0);   
  
   ResetTextFormat(&tf);
   tf.isBold=KAL_TRUE;   
   ret=AddString(&emsData, string, 300, &tf); 
   ASSERT(ret==EMS_OK);
   
  
   ret=EMSPack(&emsData,1, &numOfMsg, TPUD_p, TPUDLen);
   ASSERT(ret==EMS_OK); 
   ASSERT(numOfMsg==3);


   ReleaseEMSData(&emsData);
   EMSResourceVerdict();
}

void ems_test_28(void)
{
   EMSTATUS ret;
   EMSTextFormat tf;
   kal_uint16 i,n;
   kal_uint8 numOfMsg;

   for (i=0; i < EMS_TEST_MAX_SEG; i++)
   {
  TPUDLen[i]= 0;
   }

   InitializeEMSData(&emsData, 0);   
  
   ret = EMSSetDCS(&emsData, SMSAL_8BIT_DCS);
   ASSERT(ret==EMS_OK);

   ResetTextFormat(&tf);
   tf.isBold=KAL_TRUE;   
   ret=AddString(&emsData, string, 513, &tf); 
   ASSERT(ret==EMS_OK);
   
   n = ForwardCurrentPosition(&emsData, 0xc1);
   ASSERT(n==0);

   n = BackwardCurrentPosition(&emsData, 0x179);
   ASSERT(n==0x179);

   ResetTextFormat(&tf);
   tf.isItalic=KAL_TRUE;   
   ret=AddString(&emsData, string, 1, &tf); 

   if(EMS_TEST_MAX_SEG > 4)
   {
  ASSERT(ret==EMS_OK);
   }
   else
   {
  ASSERT(ret!=EMS_OK);
   }

   ret=EMSPack(&emsData,1, &numOfMsg, TPUD_p, TPUDLen);
   ASSERT(ret==EMS_OK); 

   if(EMS_TEST_MAX_SEG > 4)
   {
  ASSERT(numOfMsg==5);
   }

   ReleaseEMSData(&emsData);
   EMSResourceVerdict();
}

/* abonormal UDH */
void ems_test_29()
{
   EMSTATUS ret; 
   kal_uint16 i,n;
   EMSObject *obj;
   kal_uint8 *TPUD_p[4];
   
   kal_uint8 TPUD[160]=
   {  
  0x05, 0xc0, 0xe0, 0x41, 0x18, 0xcc, 0x33,
  0x33, 0x33, 0x33, 0x33, 0x33, 0x33, 0x33, 0x33, 0x33, 0x33, 0x33, 0x33, 0x33, 0x33, 0x33, 0x33,
  0x33, 0x33, 0x33, 0x33, 0x33, 0x33, 0x33, 0x33, 0x33, 0x33, 0x33, 0x33, 0x33, 0x33, 0x33, 0x33,
  0x33, 0x33, 0x33, 0x33, 0x33, 0x33, 0x33, 0x33, 0x33, 0x33, 0x33, 0x33, 0x33, 0x33, 0x33, 0x33,
  0x33, 0x33, 0x33, 0x33, 0x33, 0x33, 0x33, 0x33, 0x33, 0x33, 0x33, 0x33, 0x33, 0x33, 0x33, 0x33,
  0x33, 0x33, 0x33, 0x33, 0x33, 0x33, 0x33, 0x33, 0x33, 0x33, 0x33, 0x33, 0x33, 0x33, 0x33, 0x33,
  0x33, 0x33, 0x33, 0x33, 0x33, 0x33, 0x33, 0x33, 0x33, 0x33, 0x33, 0x33, 0x33, 0x33, 0x33, 0x33,
  0x33, 0x33, 0x33, 0x33, 0x33, 0x33, 0x33, 0x33, 0x33, 0x33, 0x33, 0x33, 0x33, 0x33, 0x33, 0x33,
  0x33, 0x33, 0x33, 0x33, 0x33, 0x33, 0x33, 0x33, 0x33, 0x33, 0x33, 0x33, 0x33, 0x33, 0x33, 0x33,
  0x33, 0x33, 0x33, 0x33, 0x33, 0x33, 0x33, 0x33, 0x33, 0x33, 0x33, 0x33, 0x33, 0x33, 0x33, 0x33,
  0x33, 0x33, 0x33, 0x33, 0x33, 0x33, 0x33
   };
   kal_uint8 TPUDLen[4]={0x9e,0,0,0};

   TPUD_p[0]=TPUD;
   
   InitializeEMSData(&emsData, 0); 

   emsData.udhi=1;

   ret=EMSUnPack(&emsData,emsData.udhi, 1, TPUD_p, TPUDLen);
   ASSERT(ret==EMS_OK);
  
   ASSERT ( emsData.textLength == 304 );
   ASSERT ( emsData.Reminder.segRemainingOctet == 8 );

   ReleaseEMSData(&emsData);
   EMSResourceVerdict();
}


void ems_test_30()
{
   EMSTATUS ret;
   EMSObjData data;
   kal_uint8 pdu[128];
   EMSTextFormat tf;
   kal_uint16 i,n;
   kal_uint8 numOfMsg;

   InitializeEMSData(&emsData, 0);   
  
   ResetTextFormat(&tf);
   tf.isBold=KAL_TRUE;   
   ret=AddString(&emsData, string, 3, &tf); 
   ASSERT(ret==EMS_OK);

   ret=AddObject(&emsData, EMS_TYPE_PREDEF_ANM, NULL, 0);
   ASSERT(ret==EMS_OK);

   ret=AddObject(&emsData, EMS_TYPE_PREDEF_SND, NULL, 0);
   ASSERT(ret==EMS_OK);

   data.picture.pdu_length=128;
   data.picture.bitsPerPixel=1;
   data.picture.vDim=16;
   data.picture.hDim=8*8;
   data.picture.pdu=pdu;
   ret=AddObject(&emsData, EMS_TYPE_PIC, &data, 0);
   ASSERT(ret==EMS_OK);

   ret=AddString(&emsData, string, 1, &tf); 
   ASSERT(ret==EMS_OK);

   ret=AddString(&emsData, string, 1, &tf); 
   ASSERT(ret==EMS_OK);
   
   ret=AddString(&emsData, string, 1, &tf); 
   ASSERT(ret==EMS_OK);

   ret=AddString(&emsData, string, 1, &tf); 
   ASSERT(ret==EMS_OK);

   ret=AddString(&emsData, string, 1, &tf); 
   ASSERT(ret==EMS_OK);

   ret=AddString(&emsData, string, 1, &tf); 
   ASSERT(ret==EMS_OK);


	ResetCurrentPosition(&emsData);

   {

  kal_uint16 c,OffsetToText;
  kal_uint8*	textBuffer=emsData.textBuffer;
  kal_uint16  textBufferLength=emsData.textLength;
  EMSObject* object;
  EMSData *data = &emsData ;

  while((OffsetToText=data->CurrentPosition.OffsetToText)<textBufferLength)
  {
		kal_uint8 type=EMSGetNextObject(data,&data->CurrentPosition,&c,&object);
		switch(type)
		{
			case 0:
				continue;
				break;
			case 1:
				BackwardCurrentPosition(data,1);
				break;
			case 2:
				if((object->Type==EMS_TYPE_TEXT_FORMAT)
					&&(EMSGetNextObject(data,&data->CurrentPosition,&c,&object)==1))
					BackwardCurrentPosition(data,1);
				else
					continue;
				break;
		}
		
#ifdef EMS_LITTLE_ENDIAN
		c=(kal_uint16)(textBuffer[OffsetToText]|(textBuffer[OffsetToText+1]<<8));
#else
		c=(kal_uint16)(textBuffer[OffsetToText+1]|(textBuffer[OffsetToText]<<8));
#endif
		if((c&0xff00)!=0)
		{
#ifdef EMS_LITTLE_ENDIAN
			textBuffer[OffsetToText]=(kal_uint8)(c&0xff);
			textBuffer[OffsetToText+1]=(kal_uint8)(c>>8);
#else
			textBuffer[OffsetToText+1]=(kal_uint8)(c&0xff);
			textBuffer[OffsetToText+0]=(kal_uint8)(c>>8);
#endif
		}
		else if(c==27)   /* EMS_ESCAPE_CHARACTER */
		{

 extern const kal_uint8 DefaultToExtendedAsciiArray[128];
#ifdef EMS_LITTLE_ENDIAN
			textBuffer[OffsetToText]=(kal_uint8)(c&0xff);
			textBuffer[OffsetToText+1]=(kal_uint8)(c>>8);

			if(ForwardCurrentPosition(data,1)!=1) break;
			OffsetToText=data->CurrentPosition.OffsetToText;

			c=(kal_uint16)(textBuffer[OffsetToText]|(textBuffer[OffsetToText+1]<<8));
			c=DefaultToExtendedAsciiArray[c];

			textBuffer[OffsetToText]=(kal_uint8)(c&0xff);
			textBuffer[OffsetToText+1]=(kal_uint8)(c>>8);
#else
			textBuffer[OffsetToText+1]=(kal_uint8)(c&0xff);
			textBuffer[OffsetToText+0]=(kal_uint8)(c>>8);

			if(ForwardCurrentPosition(data,1)!=1) break;
			OffsetToText=data->CurrentPosition.OffsetToText;

			c=(kal_uint16)(textBuffer[OffsetToText+1]|(textBuffer[OffsetToText]<<8));
			c=DefaultToExtendedAsciiArray[c];

			textBuffer[OffsetToText+1]=(kal_uint8)(c&0xff);
			textBuffer[OffsetToText+0]=(kal_uint8)(c>>8);

#endif

		}
		else
		{	

 extern const kal_uint8 DefaultToAsciiArray[128];
 c=DefaultToAsciiArray[c];
#ifdef EMS_LITTLE_ENDIAN
			textBuffer[OffsetToText]=(kal_uint8)(c&0xff);
			textBuffer[OffsetToText+1]=(kal_uint8)(c>>8);
#else
			textBuffer[OffsetToText+1]=(kal_uint8)(c&0xff);
			textBuffer[OffsetToText+0]=(kal_uint8)(c>>8);
#endif
		}
		if(ForwardCurrentPosition(data,1)!=1) break;
	}
   }

   ret=EMSPack(&emsData,1, &numOfMsg, TPUD_p, TPUDLen);
   ASSERT(ret==EMS_OK); 

   ReleaseEMSData(&emsData);
   EMSResourceVerdict();
}

void ems_test_31()
{
   EMSTATUS ret;
   EMSObjData data;
   kal_uint8 pdu[128];
   EMSTextFormat tf;
   kal_uint16 i,n;
   kal_uint8 numOfMsg;

   InitializeEMSData(&emsData, SMSAL_UCS2_DCS);   
  
   ret=AddObject(&emsData, EMS_TYPE_PREDEF_ANM, NULL, 0);
   ASSERT(ret==EMS_OK);

   ResetTextFormat(&tf);
   ret=AddString(&emsData, string, 2, &tf); 
   ASSERT(ret==EMS_OK);

   ret=AddObject(&emsData, EMS_TYPE_PREDEF_ANM, NULL, 0);
   ASSERT(ret==EMS_OK);

   ret=AddString(&emsData, string, 2, &tf); 
   ASSERT(ret==EMS_OK);

   ret=AddObject(&emsData, EMS_TYPE_PREDEF_ANM, NULL, 0);
   ASSERT(ret==EMS_OK);

   ret=AddString(&emsData, string, 84, &tf); 
   ASSERT(ret==EMS_OK);

   tf.FontSize = EMS_FONTSIZE_LARGE  ;   
   ret=AddString(&emsData, string, 20, &tf); 
   ASSERT(ret==EMS_OK);

   tf.Alignment = EMS_ALIGN_CENTER  ;   
   ret=AddString(&emsData, string, 49, &tf); 
   ASSERT(ret==EMS_OK);

   tf.Alignment = EMS_ALIGN_RIGHT  ;   
   ret=AddString(&emsData, string, 27, &tf); 
   ASSERT(ret==EMS_OK);

   BackwardCurrentPosition (&emsData, 2);

   tf.isBold = KAL_TRUE  ;   
   tf.isUnderline = KAL_TRUE  ;   
   ret=AddString(&emsData, string, 1, &tf); 
   ASSERT(ret==EMS_OK);

   ret=EMSPack(&emsData,1, &numOfMsg, TPUD_p, TPUDLen);
   ASSERT(ret==EMS_OK); 

   ReleaseEMSData(&emsData);
   EMSResourceVerdict();
}



////////////////////////////////////////////
// one iMelody with 8-bit dcs setting
////////////////////////////////////////////
void EMSTest8bitDCS1()
{   
   EMSData  data;
   EMSTATUS ret;  
   EMSObjData data1;  	
   kal_uint8 pdu[127] = {0};
   
   kal_uint8 numOfMsg;
   
   InitializeEMSData(&emsData, 0);

   // Melody1	
   data1.sound.pdu_length = 127;
   data1.sound.pdu = pdu;
   ret = AddObject(&emsData, EMS_TYPE_USERDEF_SND, &data1, 0);
   ASSERT(ret==EMS_OK);

   //change dcs to 8-bit
   ret = EMSSetDCS(&emsData, SMSAL_8BIT_DCS);
   ASSERT(ret==EMS_OK);
   
   ret = EMSPack(&emsData,1, &numOfMsg, TPUD_p, TPUDLen);
   ASSERT(ret==EMS_OK);
   

   ////////////////
   // unpack 
   ////////////////
   InitializeEMSData(&data, SMSAL_8BIT_DCS);

   //set udhi
   data.udhi = 1;

	ret = EMSUnPack(&data,emsData.udhi, numOfMsg, TPUD_p, TPUDLen);
   ASSERT(ret==EMS_OK);

   ResetCurrentPosition(&emsData);

   EMSDataVerdict(&data, &emsData);

   ReleaseEMSData(&emsData);
   ReleaseEMSData(&data);
   EMSResourceVerdict();
}

//////////////////////////////////////////////////////////
// one iMelody and some text with 8-bit dcs setting
//////////////////////////////////////////////////////////
void EMSTest8bitDCS2()
{   
   EMSData  data;
   EMSTATUS ret;  
   EMSObjData data1;  	
   kal_uint8 pdu[127] = {0};
   kal_uint8 str[10];
   kal_uint8 numOfMsg;
   EMSTextFormat tf;

   InitializeEMSData(&emsData, 0);

#ifdef EMS_LITTLE_ENDIAN
   // Text1
   str[0]='1';
   str[1]=0;
   str[2]='2';
   str[3]=0;   
#else
   // Text1
   str[0]=0;
   str[1]='1';
   str[2]=0;
   str[3]='2';   
#endif

   ret=AddString(&emsData, str, 2, NULL);
   ASSERT(ret==EMS_OK);

   // Melody1	
   data1.sound.pdu_length = 127;
   data1.sound.pdu = pdu;
   ret = AddObject(&emsData, EMS_TYPE_USERDEF_SND, &data1, 0);
   ASSERT(ret==EMS_OK);

   //change dcs to 8-bit
   ret = EMSSetDCS(&emsData, SMSAL_8BIT_DCS);
   ASSERT(ret==EMS_OK);
   
   // Text2
   ResetTextFormat(&tf);
   tf.isBold = KAL_TRUE;
#ifdef EMS_LITTLE_ENDIAN
   str[0]='3';
   str[1]=0;
   str[2]='4';
   str[3]=0;   
#else
   str[0]=0;
   str[1]='3';
   str[2]=0;
   str[3]='4';   
#endif
   ret=AddString(&emsData, str, 2, &tf);
   ASSERT(ret==EMS_OK);

   ret = EMSPack(&emsData,1, &numOfMsg, TPUD_p, TPUDLen);
   ASSERT(ret==EMS_OK);
   

   ////////////////
   // unpack 
   ////////////////
   InitializeEMSData(&data, SMSAL_8BIT_DCS);

   //set udhi
   data.udhi = 1;

   ret = EMSUnPack(&data,emsData.udhi, numOfMsg, TPUD_p, TPUDLen);
   ASSERT(ret==EMS_OK);

   ResetCurrentPosition(&emsData);

   EMSDataVerdict(&data, &emsData);

   ReleaseEMSData(&emsData);
   ReleaseEMSData(&data);
   EMSResourceVerdict();
}

//////////////////////////////////////////////////////////
// some text with UCS2 dcs setting
//////////////////////////////////////////////////////////
void EMSTestUCS2DCS()
{   
   EMSData  data;
   EMSTATUS ret;  	
   kal_uint8 numOfMsg;
   
   int length=(134/2)*EMS_MAX_SEG_NUM ;
   int i;

   InitializeEMSData(&emsData, 0);

#ifdef EMS_LITTLE_ENDIAN
   // Text1
   for(i=0;i<length;i++)
   {
  string[2*i]   = '1';
  string[2*i+1] = 0;
   }
#else
   // Text1
   for(i=0;i<length;i++)
   {
  string[2*i]   = 0;
  string[2*i+1] = '1';
   }
#endif

   ret=AddString(&emsData, string, (kal_uint16)length, NULL);
   ASSERT(ret==EMS_OK);
	
   //change dcs to UCS2
   ret = EMSSetDCS(&emsData, SMSAL_UCS2_DCS);
   ASSERT(ret==EMS_OK);
   
   ////////////////////////////////////////////////////////////////
   // whote spaces are used, remaining characters shall be zero
   ////////////////////////////////////////////////////////////////
   ASSERT(emsData.Reminder.isSegChange==1);
   ASSERT(emsData.Reminder.requiredSegment==EMS_MAX_SEG_NUM );
   ASSERT(emsData.Reminder.totalRemainingOctet==0);

   // Text2   
#ifdef EMS_LITTLE_ENDIAN
   string[0]='1';
   string[1]=0;
   string[2]='1';
   string[3]=0;   
#else
   string[0]=0;
   string[1]='1';
   string[2]=0;
   string[3]='1';   
#endif
   ret=AddString(&emsData, string, 2, NULL);
   ASSERT(ret==EMS_NO_SPACE);

   //change dcs to GSM 7-bit
   ret = EMSSetDCS(&emsData, 0);
   ASSERT(ret==EMS_OK);

   ret=AddString(&emsData, string, 2, NULL);
   ASSERT(ret==EMS_OK);

   ret = EMSPack(&emsData,1, &numOfMsg, TPUD_p, TPUDLen);
   ASSERT(ret==EMS_OK);

   ////////////////
   // unpack 
   ////////////////
   InitializeEMSData(&data, 0);

   //set udhi
   data.udhi = 1;

   ret = EMSUnPack(&data,emsData.udhi, numOfMsg, TPUD_p, TPUDLen);
   ASSERT(ret==EMS_OK);

   ResetCurrentPosition(&emsData);

   EMSDataVerdict(&data, &emsData);

   ReleaseEMSData(&data);
   ReleaseEMSData(&emsData);
   
   EMSResourceVerdict();

}

/* GSM 7-bit */
void EMSTestEndian_7BIT(void)
{
   EMSData  data;
   EMSTATUS ret;  
   EMSObjData data1;  	
	kal_uint8 pdu[127] = {0};
   kal_uint8 str[10];
   kal_uint8 numOfMsg;
   EMSTextFormat tf;
   int i;

   InitializeEMSData(&emsData, 0);

#ifndef EMS_LITTLE_ENDIAN

   //////////////////
   // big-endian
   //////////////////

   // Text1
   str[0]=0;
   str[1]='1';
   str[2]=0;
   str[3]='2';   
   ret=AddString(&emsData, str, 2, NULL);
   ASSERT(ret==EMS_OK);

   // Melody1	
   data1.sound.pdu_length = 127;
   data1.sound.pdu = pdu;
   ret = AddObject(&emsData, EMS_TYPE_USERDEF_SND, &data1, 0);
   ASSERT(ret==EMS_OK);
  
   // Text2
   ResetTextFormat(&tf);
   tf.isBold = KAL_TRUE;
   str[0]=0;
   str[1]='3';
   str[2]=0;
   str[3]='4';   
   ret=AddString(&emsData, str, 2, &tf);
   ASSERT(ret==EMS_OK);

   ret = EMSPack(&emsData,1, &numOfMsg, TPUD_p, TPUDLen);
   ASSERT(ret==EMS_OK);
  

#else

   ////////////////
   // little-endian
   ////////////////

   // Text1   
   str[0]='1';
   str[1]=0;   
   str[2]='2';   
   str[3]=0;
   ret=AddString(&emsData, str, 2, NULL);
   ASSERT(ret==EMS_OK);

   // Melody1	
   data1.sound.pdu_length = 127;
   data1.sound.pdu = pdu;
   ret = AddObject(&emsData, EMS_TYPE_USERDEF_SND, &data1, 0);
   ASSERT(ret==EMS_OK);
  
   // Text2
   ResetTextFormat(&tf);
   tf.isBold = KAL_TRUE;
   
   str[0]='3';
   str[1]=0;   
   str[2]='4';   
   str[3]=0;
   ret=AddString(&emsData, str, 2, &tf);
   ASSERT(ret==EMS_OK);

   ret = EMSPack(&emsData,1, &numOfMsg, TPUD_p, TPUDLen);
   ASSERT(ret==EMS_OK);


#endif

   ////////////////
   // unpack 
   ////////////////
   InitializeEMSData(&data, 0);

   //set udhi
   data.udhi = 1;

   ret = EMSUnPack(&data,emsData.udhi, numOfMsg, TPUD_p, TPUDLen);
   ASSERT(ret==EMS_OK);

   ResetCurrentPosition(&emsData);
   EMSDataVerdict(&data, &emsData);

#ifdef EMS_LITTLE_ENDIAN
   for(i=0;i<data.textLength/2;i++) //odd byte shall be zero
  ASSERT(*(data.textBuffer+2*i+1)==0);   
#else
   for(i=0;i<data.textLength/2;i++) //even byte shall be zero  
  ASSERT(*(data.textBuffer+2*i)==0);   
#endif

   ReleaseEMSData(&emsData);
   ReleaseEMSData(&data);
   EMSResourceVerdict();
}

/* UCS2 */
void EMSTestEndian_UCS2(void)
{
   EMSData  data;
   EMSTATUS ret; 
   kal_uint8 str[10];
   kal_uint8 numOfMsg;
   int i;

   InitializeEMSData(&emsData, SMSAL_UCS2_DCS);

#ifndef EMS_LITTLE_ENDIAN

   //////////////////
   // big-endian
   //////////////////

   // Text1
   str[0]=0;
   str[1]='1';
   str[2]=0;
   str[3]='2';   
   ret=AddString(&emsData, str, 2, NULL);
   ASSERT(ret==EMS_OK);
	  
   // Text2
  
   str[0]=0;
   str[1]='3';
   str[2]=0;
   str[3]='4';   
   ret=AddString(&emsData, str, 2, NULL);
   ASSERT(ret==EMS_OK);

   ret = EMSPack(&emsData,1, &numOfMsg, TPUD_p, TPUDLen);
   ASSERT(ret==EMS_OK);
  

#else

   ////////////////
   // little-endian
   ////////////////

   // Text1   
   str[0]='1';
   str[1]=0;   
   str[2]='2';   
   str[3]=0;
   ret=AddString(&emsData, str, 2, NULL);
   ASSERT(ret==EMS_OK);
	  
   // Text2
  
   str[0]='3';
   str[1]=0;   
   str[2]='4';   
   str[3]=0;
   ret=AddString(&emsData, str, 2, NULL);
   ASSERT(ret==EMS_OK);

   ret = EMSPack(&emsData,1, &numOfMsg, TPUD_p, TPUDLen);
   ASSERT(ret==EMS_OK);  

#endif

   /////////////////////////////////////
   // AIR PDU shall be big-endian
   /////////////////////////////////////
   for(i = 0 ; i < TPUDLen[0]/2 ; i++)
  ASSERT(TPUD_p[0][2*i]==0);   

   ////////////////
   // unpack 
   ////////////////
   InitializeEMSData(&data, SMSAL_UCS2_DCS);

   //set udhi
   data.udhi = 0;

	ret = EMSUnPack(&data,emsData.udhi, numOfMsg, TPUD_p, TPUDLen);
   ASSERT(ret==EMS_OK);

   ResetCurrentPosition(&emsData);
   EMSDataVerdict(&data, &emsData);

#ifdef EMS_LITTLE_ENDIAN
   for(i=0;i<data.textLength/2;i++) //odd byte shall be zero
  ASSERT(*(data.textBuffer+2*i+1)==0);   
#else
   for(i=0;i<data.textLength/2;i++) //even byte shall be zero  
  ASSERT(*(data.textBuffer+2*i)==0);   
#endif

   ReleaseEMSData(&emsData);
   ReleaseEMSData(&data);
   EMSResourceVerdict();
}

void EMSTestCopyEMSData(void)
{
   EMSData dest, src;
   EMSTATUS ret;
   kal_uint8 str[20];


   ////////////////////
   // empty ems data
   ////////////////////
   InitializeEMSData(&src, 0);
   InitializeEMSData(&dest, 0);

   ret=CopyEMSData(&dest, &src);
   ASSERT(ret==EMS_OK);

   EMSDataVerdict(&dest, &src);

   ReleaseEMSData(&dest);
   ReleaseEMSData(&src);

   //////////////////////
   // only text
   //////////////////////
   InitializeEMSData(&src, 0);
   InitializeEMSData(&dest, 0);

   ret=AddString(&src, str, 10, NULL);
   ASSERT(ret==EMS_OK);

   ret=CopyEMSData(&dest, &src);
   ASSERT(ret==EMS_OK);

   EMSDataVerdict(&dest, &src);

   ReleaseEMSData(&dest);
   ReleaseEMSData(&src);

   
   //////////////////////
   // only object
   //////////////////////
   InitializeEMSData(&src, 0);
   InitializeEMSData(&dest, 0);

   ret=AddObject(&src, EMS_TYPE_PREDEF_SND, NULL, 0);
   ASSERT(ret==EMS_OK);

   ret=CopyEMSData(&dest, &src);
   ASSERT(ret==EMS_OK);

   EMSDataVerdict(&dest, &src);

   ReleaseEMSData(&dest);
   ReleaseEMSData(&src);
   

   //////////////////////
   // text/object mix
   //////////////////////
   InitializeEMSData(&src, 0);
   InitializeEMSData(&dest, 0);

   ret=AddString(&src, str, 10, NULL);
   ASSERT(ret==EMS_OK);

   ret=AddObject(&src, EMS_TYPE_PREDEF_SND, NULL, 0);
   ASSERT(ret==EMS_OK);

   ret=AddString(&src, str, 10, NULL);
   ASSERT(ret==EMS_OK);

   ret=AddObject(&src, EMS_TYPE_PREDEF_SND, NULL, 0);
   ASSERT(ret==EMS_OK);

   ret=CopyEMSData(&dest, &src);
   ASSERT(ret==EMS_OK);

   EMSDataVerdict(&dest, &src);

   ReleaseEMSData(&dest);
   ReleaseEMSData(&src);  
   EMSResourceVerdict();

}

void EMSTestNormalSMS(kal_uint16 len, kal_uint8 dcs)
{
   EMSData data;
   EMSTATUS ret;
   kal_uint8 *str;
   kal_uint8 numOfMsg;
   kal_uint8 TPUD[160];
   kal_uint8 TPUDLen[4];
   kal_uint8 *TPUD_p[4];   
   int i;

   TPUD_p[0] = TPUD;
   TPUD_p[1] = NULL;
   TPUD_p[2] = NULL;
   TPUD_p[3] = NULL;

   str = (kal_uint8*)get_ctrl_buffer((kal_uint16)(len*2));

   for(i=0;i<len;i++)
   {
#ifdef EMS_LITTLE_ENDIAN
  str[2*i]='1';
  str[2*i+1]=0;
#else
  str[2*i]=0;
  str[2*i+1]='1';
#endif
   }


   InitializeEMSData(&emsData, dcs);

   ret = AddString(&emsData, str, len, NULL);
   ASSERT(ret==EMS_OK);
   ASSERT(emsData.Reminder.segRemainingOctet==0);
   
   ////////////////////////
   // pack
   ////////////////////////

   ret = EMSPack(&emsData, 1,&numOfMsg, TPUD_p, TPUDLen);
   ASSERT(ret==EMS_OK);
   ASSERT(numOfMsg==1);

   if(dcs==SMSAL_UCS2_DCS)
   {
  for(i=0;i<len;i++)
  {
 ASSERT(TPUD[2*i]==0);
 ASSERT(TPUD[2*i+1]=='1');
  }
   }
   else
   {
  for(i=0;i<len;i++)   
 ASSERT(TPUD[i]=='1');  
   }

   ////////////////////////
   // unpack
   ////////////////////////
   InitializeEMSData(&data, dcs);

   ret = EMSUnPack(&data,emsData.udhi, numOfMsg, TPUD_p, TPUDLen);
   ASSERT(ret==EMS_OK);

   ResetCurrentPosition(&emsData);
   EMSDataVerdict(&emsData, &data);

   ReleaseEMSData(&emsData);
   ReleaseEMSData(&data);

   free_ctrl_buffer(str);
   EMSResourceVerdict();
}

#define EMS_TEST_MISS_SEG_MAP(x1,x2,x3,x4) { \
int k;  \
kal_uint8 x[4]; \
kal_uint8 *miss_TPUD_p[4]; \
for(k=0;k<4;k++) miss_TPUD_p[k]=TPUD_p[k]; \
x[0]=x1;x[1]=x2;x[2]=x3;x[3]=x4;  \
if(x1==0) miss_TPUD_p[0]=NULL;   \
if(x2==0) miss_TPUD_p[1]=NULL;   \
if(x3==0) miss_TPUD_p[2]=NULL;   \
if(x4==0) miss_TPUD_p[3]=NULL;   \
EMSTestMissSeg(x, miss_TPUD_p, TPUDLen); }
  



void EMSTestMissSeg(kal_uint8 x[],
kal_uint8 *miss_TPUD_p[],  
kal_uint8 TPUDLen[])
{   
   EMSTATUS ret;  
 
   int i,j;
   kal_uint16 off=0, miss_str_len;
   kal_uint8 tmp[64];

   miss_str_len = EMSInsertMissSegStr(tmp);
 
   InitializeEMSData(&emsData, 0);
 
   emsData.udhi=1;
   ret = EMSUnPack(&emsData,emsData.udhi, 4, miss_TPUD_p, TPUDLen);
#if 1
   if(x[0]==0 && x[1]==0 && x[2]==0 && x[3]==0)//all segments are missed
   {
  ASSERT(ret==EMS_NULL_POINTER);
  ReleaseEMSData(&emsData);
  return;
   }
   else
#endif
  ASSERT(ret==EMS_OK);

   ////////////
   // check
   ////////////
   for(i=0;i<4;i++)
   {
  if(x[i]==0)//miss
  {
 for(j=0;j<miss_str_len ;j++)
ASSERT( *(emsData.textBuffer+off+j)==tmp[j] );
 off += miss_str_len ;
  }
  else
  {
 for(j=0;j<153;j++)
#ifdef EMS_LITTLE_ENDIAN 
ASSERT( *(emsData.textBuffer+off+2*j)=='0'+1 );
#else
ASSERT( *(emsData.textBuffer+off+2*j+1)=='0'+1 );
#endif
 off += (153*2);
  }
  
   }

   ReleaseEMSData(&emsData);
   EMSResourceVerdict();

   
}

void EMSTestMissSegment(void)
{
   int i,j;   

   for(i=0;i<4;i++)
   {
  for(j=0;j<160;j++)
 *(TPUD_p[i]+j)='0'+1;

  *(TPUD_p[i])   = 5;//udhl
  *(TPUD_p[i]+1) = 0;//concat hdr ie
  *(TPUD_p[i]+2) = 3;//concat hdr iedl
  *(TPUD_p[i]+3) = 0;//concat ref
  *(TPUD_p[i]+4) = 4;//total seg
  *(TPUD_p[i]+5) = i+1;//seg
  TPUDLen[i]=159; /* concat. hdr (6) + 153 char */
   }


   ////////////////////////
   // 0 : miss
   // 1 : exist
   ////////////////////////
   

   /* miss one segment */
   EMS_TEST_MISS_SEG_MAP(0,1,1,1);
   EMS_TEST_MISS_SEG_MAP(1,0,1,1);
   EMS_TEST_MISS_SEG_MAP(1,1,0,1);
   EMS_TEST_MISS_SEG_MAP(1,1,1,0);

   /* miss two segments */
   EMS_TEST_MISS_SEG_MAP(0,0,1,1);
   EMS_TEST_MISS_SEG_MAP(1,0,0,1);
   EMS_TEST_MISS_SEG_MAP(1,1,0,0);

   /* miss three segments */
   EMS_TEST_MISS_SEG_MAP(0,0,0,1);
   EMS_TEST_MISS_SEG_MAP(0,0,1,0);
   EMS_TEST_MISS_SEG_MAP(0,1,0,0);
   EMS_TEST_MISS_SEG_MAP(1,0,0,0);

   /* miss four segments */
   EMS_TEST_MISS_SEG_MAP(0,0,0,0);

   EMSResourceVerdict();

}


/* predef object */
void EMSTooManyObjects1(void)
{
   EMSTATUS ret; 
   kal_uint16 i;
 
   InitializeEMSData(&emsData, 0);   
   
   for(i=0;i<EMS_MAX_OBJECT;i++)
   {
  ret=AddObject(&emsData, EMS_TYPE_PREDEF_SND, NULL, 3);
  ASSERT(ret==EMS_OK);  
   }

   ret=AddObject(&emsData, EMS_TYPE_PREDEF_SND, NULL, 3);
   ASSERT(ret==EMS_NO_MEMORY);   

   ReleaseEMSData(&emsData);
   EMSResourceVerdict();
}

/* text format object */
void EMSTooManyObjects2(void)
{
   EMSTATUS ret; 
   kal_uint16 i;
   kal_uint8 str[2];
   EMSTextFormat tf;
 
   InitializeEMSData(&emsData, 0);   
   
   for(i=0;i<EMS_MAX_OBJECT;i++)
   {
  ResetTextFormat(&tf);
  if((i%2)==0)
 tf.isBold=1;
  else
 tf.isItalic=1;

  ret=AddString(&emsData, str, 1, &tf);
  ASSERT(ret==EMS_OK);  
   }

   ResetTextFormat(&tf);
   tf.isStrikethrough=1;
   ret=AddString(&emsData, str, 1, &tf);
   ASSERT(ret==EMS_NO_MEMORY);   

   ReleaseEMSData(&emsData);
   EMSResourceVerdict();
}

/* text format object , split case */
void EMSTooManyObjects3(void)
{
   EMSTATUS ret; 
   kal_uint16 i;
   kal_uint8 str[10];
   EMSTextFormat tf;

   ////////////////////////////////////////////////
   // case 1 : when addstring, no object allow
   ////////////////////////////////////////////////
   InitializeEMSData(&emsData, 0);   
   
   for(i=0;i<EMS_MAX_OBJECT;i++)
   {
  ResetTextFormat(&tf);
  if((i%2)==0)
 tf.isBold=1;
  else
 tf.isItalic=1;

  ret=AddString(&emsData, str, 10, &tf);
  ASSERT(ret==EMS_OK);  
   }

   BackwardCurrentPosition(&emsData, 5);

   ResetTextFormat(&tf);
   tf.isStrikethrough=1;
   ret=AddString(&emsData, str, 1, &tf);
   ASSERT(ret==EMS_NO_MEMORY);   

   ReleaseEMSData(&emsData);

   ////////////////////////////////////////////////
   // case 2 : when addstring, only one object allow
   ////////////////////////////////////////////////
   InitializeEMSData(&emsData, 0);   
   
   for(i=0;i<EMS_MAX_OBJECT-1;i++)
   {
  ResetTextFormat(&tf);
  if((i%2)==0)
 tf.isBold=1;
  else
 tf.isItalic=1;

  ret=AddString(&emsData, str, 10, &tf);
  ASSERT(ret==EMS_OK);  
   }

   BackwardCurrentPosition(&emsData, 5);

   ResetTextFormat(&tf);
   tf.isStrikethrough=1;
   ret=AddString(&emsData, str, 1, &tf);
   ASSERT(ret==EMS_NO_MEMORY);   

   ReleaseEMSData(&emsData);
   EMSResourceVerdict();
}

/* unpack ems with too many objects */
void EMSTooManyObjects4(void)
{
   EMSTATUS ret; 
   kal_uint16 i;
   EMSObject *obj;
   kal_uint8 *TPUD_p[4];

   /* 34 predefined sound objects */
   kal_uint8 TPUD[160]=
   {
  4*34,//udhl
  0x0b, 0x02, 0x00,  0x00, 
  0x0b, 0x02, 0x00,  0x01,
  0x0b, 0x02, 0x00,  0x02,
  0x0b, 0x02, 0x00,  0x03,
  0x0b, 0x02, 0x00,  0x04,
  0x0b, 0x02, 0x00,  0x05,
  0x0b, 0x02, 0x00,  0x06,
  0x0b, 0x02, 0x00,  0x07,
  0x0b, 0x02, 0x00,  0x08,
  0x0b, 0x02, 0x00,  0x09,
  
  0x0b, 0x02, 0x00,  0x00, 
  0x0b, 0x02, 0x00,  0x01,
  0x0b, 0x02, 0x00,  0x02,
  0x0b, 0x02, 0x00,  0x03,
  0x0b, 0x02, 0x00,  0x04,
  0x0b, 0x02, 0x00,  0x05,
  0x0b, 0x02, 0x00,  0x06,
  0x0b, 0x02, 0x00,  0x07,
  0x0b, 0x02, 0x00,  0x08,
  0x0b, 0x02, 0x00,  0x09,

  0x0b, 0x02, 0x00,  0x00, 
  0x0b, 0x02, 0x00,  0x01,
  0x0b, 0x02, 0x00,  0x02,
  0x0b, 0x02, 0x00,  0x03,
  0x0b, 0x02, 0x00,  0x04,
  0x0b, 0x02, 0x00,  0x05,
  0x0b, 0x02, 0x00,  0x06,
  0x0b, 0x02, 0x00,  0x07,
  0x0b, 0x02, 0x00,  0x08,
  0x0b, 0x02, 0x00,  0x09,

  0x0b, 0x02, 0x00,  0x00, 
  0x0b, 0x02, 0x00,  0x01,
  0x0b, 0x02, 0x00,  0x02,
  0x0b, 0x02, 0x00,  0x03,
  0x0b, 0x02, 0x00,  0x04,  
   };
   kal_uint8 TPUDLen[4]={4*34+1, 0,0,0};

   TPUD_p[0]=TPUD;
   TPUD_p[1]=NULL;
   TPUD_p[2]=NULL;
   TPUD_p[3]=NULL;
   
   InitializeEMSData(&emsData, 0);   

   emsData.udhi=1;

   ret=EMSUnPack(&emsData,emsData.udhi, 1, TPUD_p, TPUDLen);
   ASSERT(ret==EMS_OK);
   ASSERT(emsData.listHead != NULL);
   obj = emsData.listHead;

   for(i=0;i<EMS_MAX_OBJECT;i++)
   {
  ASSERT(obj->Type == EMS_TYPE_PREDEF_SND);
  ASSERT(obj->PredefNo == (i%10));
  obj = obj->next;
   }

   /* verdict */
   ASSERT(obj == NULL);

   ReleaseEMSData(&emsData);   
   EMSResourceVerdict();
}

void ems_get_next_prev_obj_test(void)
{
   EMSTATUS ret; 
   kal_uint16 i;
   kal_uint8 str[10];
   EMSTextFormat tf;
   EMSPosition p;
   kal_uint16 c;
   EMSObject *obj;
   kal_uint8 n;

   ////////////////////////////////////////////////
   // case 1 : empty ems data
   ////////////////////////////////////////////////
   InitializeEMSData(&emsData, 0); 

   p.OffsetToText = 0; /* reset position */
   p.Object = NULL;

   n=EMSGetNextObject(&emsData, &p, &c, &obj);
   ASSERT(n==0); /* invalid pos */

   n=EMSGetPreviousObject(&emsData, &p, &c, &obj);
   ASSERT(n==0); /* invalid pos */

   ReleaseEMSData(&emsData);

   ////////////////////////////////////////////////
   // case 2 : only text
   ////////////////////////////////////////////////
   InitializeEMSData(&emsData, 0); 

   p.OffsetToText = 0; /* reset position */
   p.Object = NULL;

   EMSTestFillString(str, 5,"12345");
   ret= AddString(&emsData, str, 5, NULL);
   ASSERT(ret==EMS_OK);

   ResetCurrentPosition(&emsData);

   for(i=1;i<=5;i++)  
   {
  n=EMSGetNextObject(&emsData, &p, &c, &obj);  
  ASSERT(n==1);  
  ASSERT((c-'0') == i);
   }

   n=EMSGetNextObject(&emsData, &p, &c, &obj);  
   ASSERT(n==0); 

   for(i=5;i>=1;i--)  
   {
  n=EMSGetPreviousObject(&emsData, &p, &c, &obj);  
  ASSERT(n==1);  
  ASSERT((c-'0') == i);
   }

   n=EMSGetPreviousObject(&emsData, &p, &c, &obj);  
   ASSERT(n==0); 

   ReleaseEMSData(&emsData);

   ////////////////////////////////////////////////
   // case 3 : only object
   ////////////////////////////////////////////////
   InitializeEMSData(&emsData, 0); 

   p.OffsetToText = 0; /* reset position */
   p.Object = NULL;

   ret=AddObject(&emsData, EMS_TYPE_PREDEF_SND, NULL, 0);
   ASSERT(ret==EMS_OK);

   ret=AddObject(&emsData, EMS_TYPE_PREDEF_ANM, NULL, 0);
   ASSERT(ret==EMS_OK);

   n=EMSGetNextObject(&emsData, &p, &c, &obj);  
   ASSERT(n==2);
   ASSERT(obj->Type == EMS_TYPE_PREDEF_SND);

   n=EMSGetNextObject(&emsData, &p, &c, &obj);  
   ASSERT(n==2);
   ASSERT(obj->Type == EMS_TYPE_PREDEF_ANM);

   n=EMSGetNextObject(&emsData, &p, &c, &obj);  
   ASSERT(n==0);

   n=EMSGetPreviousObject(&emsData, &p, &c, &obj);  
   ASSERT(n==2);
   ASSERT(obj->Type == EMS_TYPE_PREDEF_ANM);

   n=EMSGetPreviousObject(&emsData, &p, &c, &obj);  
   ASSERT(n==2);
   ASSERT(obj->Type == EMS_TYPE_PREDEF_SND);

   n=EMSGetPreviousObject(&emsData, &p, &c, &obj);  
   ASSERT(n==0);

   ReleaseEMSData(&emsData);

   ////////////////////////////////////////////////
   // case 4 : mix 
   ////////////////////////////////////////////////
   InitializeEMSData(&emsData, 0); 

   p.OffsetToText = 0; /* reset position */
   p.Object = NULL;

   /* [snd]12345[anm]12345 */

   ret=AddObject(&emsData, EMS_TYPE_PREDEF_SND, NULL, 0);
   ASSERT(ret==EMS_OK);

   EMSTestFillString(str, 5,"12345");
   ret= AddString(&emsData, str, 5, NULL);
   ASSERT(ret==EMS_OK);

   ret=AddObject(&emsData, EMS_TYPE_PREDEF_ANM, NULL, 0);
   ASSERT(ret==EMS_OK);

   EMSTestFillString(str, 5,"12345");
   ret= AddString(&emsData, str, 5, NULL);
   ASSERT(ret==EMS_OK);

   n=EMSGetNextObject(&emsData, &p, &c, &obj);  
   ASSERT(n==2);
   ASSERT(obj->Type == EMS_TYPE_PREDEF_SND);

   for(i=1;i<=5;i++)  
   {
  n=EMSGetNextObject(&emsData, &p, &c, &obj);  
  ASSERT(n==1);  
  ASSERT((c-'0') == i);
   }

   n=EMSGetNextObject(&emsData, &p, &c, &obj);  
   ASSERT(n==2);
   ASSERT(obj->Type == EMS_TYPE_PREDEF_ANM);

   for(i=1;i<=5;i++)  
   {
  n=EMSGetNextObject(&emsData, &p, &c, &obj);  
  ASSERT(n==1);  
  ASSERT((c-'0') == i);
   }

   n=EMSGetNextObject(&emsData, &p, &c, &obj);  
   ASSERT(n==0);

   for(i=5;i>=1;i--)  
   {
  n=EMSGetPreviousObject(&emsData, &p, &c, &obj);  
  ASSERT(n==1);  
  ASSERT((c-'0') == i);
   }

   n=EMSGetPreviousObject(&emsData, &p, &c, &obj);  
   ASSERT(n==2);
   ASSERT(obj->Type == EMS_TYPE_PREDEF_ANM);

   for(i=5;i>=1;i--)  
   {
  n=EMSGetPreviousObject(&emsData, &p, &c, &obj);  
  ASSERT(n==1);  
  ASSERT((c-'0') == i);
   }

   n=EMSGetPreviousObject(&emsData, &p, &c, &obj);  
   ASSERT(n==2);
   ASSERT(obj->Type == EMS_TYPE_PREDEF_SND);

   n=EMSGetPreviousObject(&emsData, &p, &c, &obj);  
   ASSERT(n==0);

   ReleaseEMSData(&emsData);
   EMSResourceVerdict();

}

void EMS_unpack_test1(void)
{
   EMSTATUS ret; 
   kal_uint16 i,n;
   EMSObject *obj;
   kal_uint8 *TPUD_p[4];

   kal_uint8 TPUD[160]=
   {
  17,//udhl
  0x0a, 0x03, 0x00,  0x0a, 0x14 /* bold & large font */ ,
  0x0b, 0x02, 0x01,  0x01,
  0x0b, 0x02, 0x02,  0x02,
  0x0b, 0x02, 0x03,  0x03, 
   };
   kal_uint8 TPUDLen[4]={17+1+10, 0,0,0};

   TPUD_p[0]=TPUD;
   TPUD_p[1]=NULL;
   TPUD_p[2]=NULL;
   TPUD_p[3]=NULL;
   
   InitializeEMSData(&emsData, 0);   

   emsData.udhi = 1;

   ret=EMSUnPack(&emsData,emsData.udhi, 1, TPUD_p, TPUDLen);
   ASSERT(ret==EMS_OK);

   ASSERT(emsData.listHead != NULL &&
  emsData.listHead->Type == EMS_TYPE_TEXT_FORMAT &&
  emsData.listHead->data->text_format.isBold == KAL_TRUE && 
  emsData.listHead->data->text_format.FontSize == EMS_FONTSIZE_LARGE &&
  TXT_FMT_LEN(emsData.listHead) == 20);
   
   obj = emsData.listHead;

   for(i = 1 ; i<=3 ; i++)
   {
  obj = obj->next;  
  ASSERT(obj != NULL &&
 obj->Type == EMS_TYPE_PREDEF_SND &&
 obj->OffsetToText == (2*i));
   }
 
   n=ForwardCurrentPosition(&emsData, 1);
   ASSERT(n==1);

   n=CancelCurrentPosition(&emsData, 1);
   ASSERT(n==1);

   ReleaseEMSData(&emsData);   
   EMSResourceVerdict();

}



void EMSTestInvalidTextFormat1()
{
   EMSTATUS ret; 
   kal_uint16 i,n;
   EMSObject *obj;
   kal_uint8 *TPUD_p[4];

   /* with text format header but no text */
   kal_uint8 TPUD[160]=
   {  
  0x9,
  0x0a, 0x03, 0x00,0x03,0x01,
  0x0d, 0x02, 0x00,0x04
   };
   kal_uint8 TPUDLen[4]={10,0,0,0};

   TPUD_p[0]=TPUD;
   
   InitializeEMSData(&emsData, 0);   

   emsData.udhi=1;
   emsData.dcs=0;

   ret=EMSUnPack(&emsData,emsData.udhi, 1, TPUD_p, TPUDLen);
   ASSERT(ret==EMS_OK);
  
   /* text format ie shall be discarded !! */
   ASSERT ( ((EMSInternalData*)emsData.internal)->numOfEMSObject == 1 );

   ReleaseEMSData(&emsData);
   EMSResourceVerdict();
}


void EMSTestInvalidTextFormat2()
{
   EMSTATUS ret; 
   kal_uint16 i,n;
   EMSObject *obj;
   kal_uint8 *TPUD_p[4];

   /* length of text format is greater than the remaining text length  */
   kal_uint8 TPUD[160]=
   {  
  14,
  0x0a, 0x03, 0x00,0x03,0x01,/*abc*/
  0x0d, 0x02, 0x00,0x04,
  0x0a, 0x03, 0x00,0x03,0x01,/*this tf shall be discarded */
  'a','b','c'
   };
   kal_uint8 TPUDLen[4]={18,0,0,0};

   TPUD_p[0]=TPUD;
   
   InitializeEMSData(&emsData, 0);   

   emsData.udhi=1;
   emsData.dcs=0;

   ret=EMSUnPack(&emsData,emsData.udhi, 1, TPUD_p, TPUDLen);
   ASSERT(ret==EMS_OK);
  
   /* the 2nd text format ie shall be discarded !! */
   ASSERT ( ((EMSInternalData*)emsData.internal)->numOfEMSObject == 2 );
   ASSERT ( emsData.listHead != NULL && !IS_TXT_FMT_OBJ(emsData.listHead) );
   ASSERT ( emsData.listHead->next != NULL && IS_TXT_FMT_OBJ(emsData.listHead->next) );

   ReleaseEMSData(&emsData);
   EMSResourceVerdict();
}

/* UCS2 */
void EMSTestInvalidTextFormat3()
{
   EMSTATUS ret; 
   kal_uint16 i,n;
   EMSObject *obj;
   kal_uint8 *TPUD_p[4];

   
   kal_uint8 TPUD[160]=
   {  
  14,
  0x0a, 0x03, 0x00,0x03,0x01,/*abc*/
  0x0d, 0x02, 0x00,0x04,
  0x0a, 0x03, 0x00,0x03,0x01,/* duplicated text format object */
  0,'a',0,'b',0,'c',
  0,'d',0,'e',0,'f'
   };
   kal_uint8 TPUDLen[4]={27,0,0,0};

   TPUD_p[0]=TPUD;
   
   InitializeEMSData(&emsData, 8); /*UCS2*/

   emsData.udhi=1;

   ret=EMSUnPack(&emsData,emsData.udhi, 1, TPUD_p, TPUDLen);
   ASSERT(ret==EMS_OK);
  
   /* the duplicated text format ie shall be discarded !! */
   ASSERT ( ((EMSInternalData*)emsData.internal)->numOfEMSObject == 2 );
   ASSERT ( emsData.listHead != NULL && !IS_TXT_FMT_OBJ(emsData.listHead) );
   ASSERT ( emsData.listHead->next != NULL && IS_TXT_FMT_OBJ(emsData.listHead->next) );

   ReleaseEMSData(&emsData);
   EMSResourceVerdict();
}

void EMSTestInvalidObjOffset1()
{
   EMSTATUS ret; 
   kal_uint16 i,n;
   EMSObject *obj;
   kal_uint8 *TPUD_p[4];

   
   kal_uint8 TPUD[160]=
   {  
  17,
  0x0a, 0x03, 0x00,0x03,0x01,/*abc*/
  0x0d, 0x02, 0x03,0x04,
  0x0d, 0x02, 0x00,0x04,/*invalid offset, discard this object*/
  0x0b, 0x02, 0x03,0x04,
  'a','b','c',
  'd','e','f'
   };
   kal_uint8 TPUDLen[4]={24,0,0,0};

   TPUD_p[0]=TPUD;
   
   InitializeEMSData(&emsData, 0); 

   emsData.udhi=1;

   ret=EMSUnPack(&emsData,emsData.udhi, 1, TPUD_p, TPUDLen);
   ASSERT(ret==EMS_OK);
  
   /* the duplicated text format ie shall be discarded !! */
   ASSERT ( ((EMSInternalData*)emsData.internal)->numOfEMSObject == 3 );
   ASSERT ( emsData.listHead != NULL && IS_TXT_FMT_OBJ(emsData.listHead) );
   ASSERT ( emsData.listHead->next != NULL && !IS_TXT_FMT_OBJ(emsData.listHead->next) );
   ASSERT ( emsData.listTail != NULL && emsData.listTail->Type == EMS_TYPE_PREDEF_SND );

   ReleaseEMSData(&emsData);
   EMSResourceVerdict();
}

//ucs2
void EMSTestInvalidObjOffset2()
{
   EMSTATUS ret; 
   kal_uint16 i,n;
   EMSObject *obj;
   kal_uint8 *TPUD_p[4];

   
   kal_uint8 TPUD[160]=
   {  
  17,
  0x0a, 0x03, 0x00,0x03,0x01,/*abc*/
  0x0d, 0x02, 0x03,0x04,
  0x0d, 0x02, 0x00,0x04,/*invalid offset, discard this object*/
  0x0b, 0x02, 0x03,0x04,
  0,'a',0,'b',0,'c',
  0,'d',0,'e',0,'f'
   };
   kal_uint8 TPUDLen[4]={30,0,0,0};

   TPUD_p[0]=TPUD;
   
   InitializeEMSData(&emsData, 8); /*UCS2*/

   emsData.udhi=1;

   ret=EMSUnPack(&emsData,emsData.udhi, 1, TPUD_p, TPUDLen);
   ASSERT(ret==EMS_OK);
  
   /* the duplicated text format ie shall be discarded !! */
   ASSERT ( ((EMSInternalData*)emsData.internal)->numOfEMSObject == 3 );
   ASSERT ( emsData.listHead != NULL && IS_TXT_FMT_OBJ(emsData.listHead) );
   ASSERT ( emsData.listHead->next != NULL && !IS_TXT_FMT_OBJ(emsData.listHead->next) );
   ASSERT ( emsData.listTail != NULL && emsData.listTail->Type == EMS_TYPE_PREDEF_SND );

   ReleaseEMSData(&emsData);
   EMSResourceVerdict();
}

void EMSTestInvalidUDHL1()
{
   EMSTATUS ret; 
   kal_uint16 i,n;
   EMSObject *obj;
   kal_uint8 *TPUD_p[4];
   
   kal_uint8 TPUD[160]=
   {  
  0,/*invalid udhl : UDHL == 0 */  
  0x0d, 0x02, 0x03,0x04,
  0x0d, 0x02, 0x00,0x04,
  0x0b, 0x02, 0x03,0x04,
  'a','b','c'
   };
   kal_uint8 TPUDLen[4]={16,0,0,0};

   TPUD_p[0]=TPUD;
   
   InitializeEMSData(&emsData, 0); 

   emsData.udhi=1;

   ret=EMSUnPack(&emsData,emsData.udhi, 1, TPUD_p, TPUDLen);
   ASSERT(ret==EMS_OK);
  
   ASSERT ( ((EMSInternalData*)emsData.internal)->numOfEMSObject == 0 );
   ASSERT ( emsData.textLength == 32 );

   ReleaseEMSData(&emsData);
   EMSResourceVerdict();
}

void EMSTestInvalidUDHL2()
{
   EMSTATUS ret; 
   kal_uint16 i,n;
   EMSObject *obj;
   kal_uint8 *TPUD_p[4];
   
   kal_uint8 TPUD[160]=
   {  
  3,/*invalid udhl: UDHL is too small */  
  0x0d, 0x02, 0x03,0x04,
  0x0d, 0x02, 0x00,0x04,
  0x0b, 0x02, 0x03,0x04,
  'a','b','c'
   };
   kal_uint8 TPUDLen[4]={16,0,0,0};

   TPUD_p[0]=TPUD;
   
   InitializeEMSData(&emsData, 0); 

   emsData.udhi=1;

   ret=EMSUnPack(&emsData,emsData.udhi, 1, TPUD_p, TPUDLen);
   ASSERT(ret==EMS_OK);
  
   ASSERT ( ((EMSInternalData*)emsData.internal)->numOfEMSObject == 0 );

   ReleaseEMSData(&emsData);
   EMSResourceVerdict();
}

/* no udh / single sms */
void EMSTestInvalidUCS2len1()
{
   EMSTATUS ret; 
   kal_uint16 i,n;
   EMSObject *obj;
   kal_uint8 *TPUD_p[4];
   
   kal_uint8 TPUD[160]=
   {  
  0,'a',0,'b',0,'c','d'
   };
   kal_uint8 TPUDLen[4]={7,0,0,0};

   TPUD_p[0]=TPUD;
   
   InitializeEMSData(&emsData, 8); 

   ret=EMSUnPack(&emsData,0, 1, TPUD_p, TPUDLen);
   ASSERT(ret==EMS_OK);
  
   ASSERT ( ((EMSInternalData*)emsData.internal)->numOfEMSObject == 0 );
   ASSERT ( emsData.textLength == 6 );

   ReleaseEMSData(&emsData);
   EMSResourceVerdict();
}

/* concat. sms */
void EMSTestInvalidUCS2len2()
{
   EMSTATUS ret; 
   kal_uint16 i,n;
   EMSObject *obj;
   kal_uint8 *TPUD_p[4];
   
   kal_uint8 TPUD[160]=
   {  
  5,
  0x0, 3, 0, 3, 1,
  0,'a',0,'b',0,'c','d'
   };
   kal_uint8 TPUDLen[4]={13,0,0,0};

   TPUD_p[0]=TPUD;
   
   InitializeEMSData(&emsData, 8); 

   emsData.udhi=1;

   ret=EMSUnPack(&emsData,emsData.udhi, 1, TPUD_p, TPUDLen);
   ASSERT(ret==EMS_OK);
  
   ASSERT ( ((EMSInternalData*)emsData.internal)->numOfEMSObject == 0 );
   ASSERT ( emsData.textLength == 6 );

   ReleaseEMSData(&emsData);
   EMSResourceVerdict();
}

void EMSTestWMA()
{
   EMSTATUS ret; 
   kal_uint16 i,n;
   EMSObject *obj;
   kal_uint8 *TPUD_p[4];
   
   kal_uint8 TPUD[160]=
   {  
  5,
  0x0, 3, 0, 3, 1,
  0,'a',0,'b',0,'c','d'
   };
   kal_uint8 TPUDLen[4]={13,0,0,0};

   TPUD_p[0]=TPUD;
   
   InitializeEMSData(&emsData, 8); 

   emsData.udhi=1;

   ret=EMSUnPack(&emsData,emsData.udhi, 1, TPUD_p, TPUDLen);
   ASSERT(ret==EMS_OK);
  
   ASSERT ( ((EMSInternalData*)emsData.internal)->numOfEMSObject == 0 );
   ASSERT ( emsData.textLength == 6 );

   ReleaseEMSData(&emsData);
   EMSResourceVerdict();
}

void EMSTestNewTxtAlignment_01()
{
   EMSTATUS ret;
   EMSTextFormat tf;
   kal_uint8 numOfMsg;
   kal_uint8 i,j;
   kal_uint16 chars, total_chars ;
   kal_uint8 dcs;
   kal_uint8 buf[64];

   for (i=0; i < EMS_TEST_MAX_SEG; i++)
  TPUDLen[i]= 0;

   /* j = 0, 1 for succeeded case */
   /* j = 2, 3 for fail case  */
   for (j=0; j< 4 ;j++)
   {
  if (j==0 || j==2)
  {
 dcs  = SMSAL_DEFAULT_DCS ;
 if (j == 0)
chars = 200 ;
 else
chars = MAX_TEXT_LEN_SEPTET - 30 ;
  }
  else
  {
 dcs  = SMSAL_UCS2_DCS ;
 if (j == 1)
chars = 200 ;
 else
chars = MAX_CHAR_UCS2 - 10 ;
  }

  InitializeEMSData(&emsData, dcs);
  total_chars = 0;

  ResetTextFormat(&tf);
  tf.isItalic = KAL_TRUE;
  ret = EMSTestAddString(30 , &tf);
  ASSERT(ret==EMS_OK);
  total_chars += 30;

  ret=AddObject(&emsData, EMS_TYPE_PREDEF_ANM, NULL, 0);
  ASSERT(ret==EMS_OK);

  /* normal characters */
  ResetTextFormat(&tf);
  ret = EMSTestAddString(chars-30 , &tf);
  ASSERT(ret==EMS_OK);
  total_chars += (chars-30);

  ResetTextFormat(&tf);
  tf.Alignment = EMS_ALIGN_CENTER ;
  tf.textLength = total_chars * 2 - 20;
  ret = AddTextAlignment(&emsData, 20, &tf);

  if (j==3 || j==2)
  {
 ASSERT(ret != EMS_OK);
  }
  else
  {
 ASSERT(ret == EMS_OK);
  }

  ret=EMSPack(&emsData, 1,&numOfMsg, TPUD_p, TPUDLen);
  ASSERT(ret == EMS_OK);
  EMSDataDump(&emsData);


  if(ret == EMS_OK)
  {
 InitializeEMSData(&emsData1, 0);
 /* set dcs & udhi */
 emsData1.dcs = emsData.dcs;
 emsData1.udhi = emsData.udhi;

 ret=EMSUnPack(&emsData1,emsData.udhi, numOfMsg, TPUD_p, TPUDLen);
 sprintf(buf, "UnPack: ret=%d\n", ret);
 kal_print((kal_char*)buf);

 EMSDataDump(&emsData1);

 if(ret == EMS_OK)
 {
kal_uint8 isSame = EMSDataCmp(&emsData, &emsData1, 0);
sprintf(buf, "EMSDataCmp: ret=%d\n", isSame);
kal_print((kal_char*)buf);
ASSERT(isSame == 0);
 }
 ReleaseEMSData(&emsData1);
  }

  ReleaseEMSData(&emsData);
   }

   EMSResourceVerdict();
}

void EMSTestNewTxtAlignment_02()
{
   EMSTATUS ret;
   EMSTextFormat tf;
   kal_uint8 numOfMsg;
   kal_uint8 i,j;
   kal_uint16 chars, total_chars ;
   kal_uint8 dcs;
   kal_uint8 buf[64];

   for (i=0; i < EMS_TEST_MAX_SEG; i++)
  TPUDLen[i]= 0;

   /* j = 0, 1 for succeeded case */
   /* j = 2, 3 for fail case  */
   for (j=0; j< 4 ;j++)
   {
  if (j==0 || j==2)
  {
 dcs  = SMSAL_DEFAULT_DCS ;
 if (j == 0)
chars = 200 ;
 else
chars = MAX_TEXT_LEN_SEPTET - 69 ;
  }
  else
  {
 dcs  = SMSAL_UCS2_DCS ;
 if (j == 1)
chars = 200 ;
 else
chars = MAX_CHAR_UCS2 - EMS_TEST_MAX_SEG * (5 + 1)/2 -2;
  }

  InitializeEMSData(&emsData, dcs);
  total_chars = 0;

  ResetTextFormat(&tf);
  tf.isItalic = KAL_TRUE;
  ret = EMSTestAddString(30 , &tf);
  ASSERT(ret==EMS_OK);
  total_chars += 30;

  ret=AddObject(&emsData, EMS_TYPE_PREDEF_ANM, NULL, 0);
  ASSERT(ret==EMS_OK);

  ret = EMSTestAddString(chars-30 , &tf);
  ASSERT(ret==EMS_OK);
  total_chars += (chars-30);

  ResetTextFormat(&tf);
  tf.Alignment = EMS_ALIGN_CENTER ;
  tf.textLength = total_chars * 2 - 20;
  ret = AddTextAlignment(&emsData, 20, &tf);

  if (j==3 || j==2)
  {
 ASSERT(ret != EMS_OK);
  }
  else
  {
 ASSERT(ret == EMS_OK);
  }

  ret=EMSPack(&emsData, 1,&numOfMsg, TPUD_p, TPUDLen);
  ASSERT(ret == EMS_OK);
  EMSDataDump(&emsData);


  if(ret == EMS_OK)
  {
 InitializeEMSData(&emsData1, 0);
 /* set dcs & udhi */
 emsData1.dcs = emsData.dcs;
 emsData1.udhi = emsData.udhi;

 ret=EMSUnPack(&emsData1,emsData.udhi, numOfMsg, TPUD_p, TPUDLen);
 sprintf(buf, "UnPack: ret=%d\n", ret);
 kal_print((kal_char*)buf);

 EMSDataDump(&emsData1);

 if(ret == EMS_OK)
 {
kal_uint8 isSame = EMSDataCmp(&emsData, &emsData1, 0);
sprintf(buf, "EMSDataCmp: ret=%d\n", isSame);
kal_print((kal_char*)buf);
ASSERT(isSame == 0);
 }
 ReleaseEMSData(&emsData1);
  }

  ReleaseEMSData(&emsData);
   }

   EMSResourceVerdict();
}

void EMSTestNewTxtAlignment_03()
{
   EMSTATUS ret;
   EMSTextFormat tf;
   kal_uint8 numOfMsg;
   kal_uint8 i,j;
   kal_uint16 chars, total_chars ;
   kal_uint8 dcs;
   kal_uint8 buf[64];

   for (i=0; i < EMS_TEST_MAX_SEG; i++)
  TPUDLen[i]= 0;

   /* j = 0, 1 for succeeded case */
   /* j = 2, 3 for fail case  */
   for (j=0; j< 4 ;j++)
   {
  if (j==0 || j==2)
  {
 dcs  = SMSAL_DEFAULT_DCS ;
 if (j == 0)
chars = 200 ;
 else
chars = MAX_TEXT_LEN_SEPTET - 69 ;
  }
  else
  {
 dcs  = SMSAL_UCS2_DCS ;
 if (j == 1)
chars = 200 ;
 else
chars = MAX_CHAR_UCS2 - EMS_TEST_MAX_SEG * (5 + 1)/2 -2;
  }

  InitializeEMSData(&emsData, dcs);
  total_chars = 0;

  ResetTextFormat(&tf);
  tf.isItalic = KAL_TRUE;
  ret = EMSTestAddString(30 , &tf);
  ASSERT(ret==EMS_OK);
  total_chars += 30;

  ret=AddObject(&emsData, EMS_TYPE_PREDEF_ANM, NULL, 0);
  ASSERT(ret==EMS_OK);

  ret = EMSTestAddString(chars-30 , &tf);
  ASSERT(ret==EMS_OK);
  total_chars += (chars-30);

  BackwardCurrentPosition(&emsData, 10);

  ResetTextFormat(&tf);
  tf.Alignment = EMS_ALIGN_CENTER ;
  tf.textLength = (total_chars -10) * 2 - 20;
  ret = AddTextAlignment(&emsData, 20, &tf);

  if (j==3 || j==2)
  {
 ASSERT(ret != EMS_OK);
  }
  else
  {
 ASSERT(ret == EMS_OK);
  }

  ret=EMSPack(&emsData, 1,&numOfMsg, TPUD_p, TPUDLen);
  ASSERT(ret == EMS_OK);
  EMSDataDump(&emsData);


  if(ret == EMS_OK)
  {
 InitializeEMSData(&emsData1, 0);
 /* set dcs & udhi */
 emsData1.dcs = emsData.dcs;
 emsData1.udhi = emsData.udhi;

 ret=EMSUnPack(&emsData1,emsData.udhi, numOfMsg, TPUD_p, TPUDLen);
 sprintf(buf, "UnPack: ret=%d\n", ret);
 kal_print((kal_char*)buf);

 EMSDataDump(&emsData1);

 if(ret == EMS_OK)
 {
kal_uint8 isSame = EMSDataCmp(&emsData, &emsData1, 0);
sprintf(buf, "EMSDataCmp: ret=%d\n", isSame);
kal_print((kal_char*)buf);
ASSERT(isSame == 0);
 }
 ReleaseEMSData(&emsData1);
  }

  ReleaseEMSData(&emsData);
   }

   EMSResourceVerdict();
}

void EMSTestNewTxtAlignment_04()
{
   EMSTATUS ret;
   EMSTextFormat tf;
   kal_uint8 numOfMsg;
   kal_uint8 i,j;
   kal_uint16 chars, total_chars ;
   kal_uint8 dcs;
   kal_uint8 buf[64];

   for (i=0; i < EMS_TEST_MAX_SEG; i++)
  TPUDLen[i]= 0;

   /* j = 0, 1 for succeeded case */
   /* j = 2, 3 for fail case  */
   for (j=0; j< 4 ;j++)
   {
  if (j==0 || j==2)
  {
 dcs  = SMSAL_DEFAULT_DCS ;
 if (j == 0)
chars = 200 ;
 else
chars = MAX_TEXT_LEN_SEPTET - 30 ;
  }
  else
  {
 dcs  = SMSAL_UCS2_DCS ;
 if (j == 1)
chars = 200 ;
 else
chars = MAX_CHAR_UCS2 - 10 ;
  }

  InitializeEMSData(&emsData, dcs);
  total_chars = 0;

  ResetTextFormat(&tf);
  tf.isItalic = KAL_TRUE;
  ret = EMSTestAddString(30 , &tf);
  ASSERT(ret==EMS_OK);
  total_chars += 30;

  ret=AddObject(&emsData, EMS_TYPE_PREDEF_ANM, NULL, 0);
  ASSERT(ret==EMS_OK);

  /* normal characters */
  ResetTextFormat(&tf);
  ret = EMSTestAddString(chars-30 , &tf);
  ASSERT(ret==EMS_OK);
  total_chars += (chars-30);

  ResetTextFormat(&tf);
  tf.Alignment = EMS_ALIGN_CENTER ;
  tf.textLength = total_chars * 2 ;
  ret = AddTextAlignment(&emsData, 0, &tf);

  if (j==3 || j==2)
  {
 ASSERT(ret != EMS_OK);
  }
  else
  {
 ASSERT(ret == EMS_OK);
  }

  ret=EMSPack(&emsData, 1,&numOfMsg, TPUD_p, TPUDLen);
  ASSERT(ret == EMS_OK);
  EMSDataDump(&emsData);


  if(ret == EMS_OK)
  {
 InitializeEMSData(&emsData1, 0);
 /* set dcs & udhi */
 emsData1.dcs = emsData.dcs;
 emsData1.udhi = emsData.udhi;

 ret=EMSUnPack(&emsData1,emsData.udhi, numOfMsg, TPUD_p, TPUDLen);
 sprintf(buf, "UnPack: ret=%d\n", ret);
 kal_print((kal_char*)buf);

 EMSDataDump(&emsData1);

 if(ret == EMS_OK)
 {
kal_uint8 isSame = EMSDataCmp(&emsData, &emsData1, 0);
sprintf(buf, "EMSDataCmp: ret=%d\n", isSame);
kal_print((kal_char*)buf);
ASSERT(isSame == 0);
 }
 ReleaseEMSData(&emsData1);
  }

  ReleaseEMSData(&emsData);
   }

   EMSResourceVerdict();
}

void EMSTestNewTxtAlignment_05()
{
   EMSTATUS ret;
   EMSTextFormat tf;
   kal_uint8 numOfMsg;
   kal_uint8 i,j;
   kal_uint16 chars, total_chars ;
   kal_uint8 dcs;
   kal_uint8 buf[64];

   for (i=0; i < EMS_TEST_MAX_SEG; i++)
  TPUDLen[i]= 0;

   /* j = 0, 1 for succeeded case */
   /* j = 2, 3 for fail case  */
   for (j=0; j< 4 ;j++)
   {
  if (j==0 || j==2)
  {
 dcs  = SMSAL_DEFAULT_DCS ;
 if (j == 0)
chars = 200 ;
 else
chars = MAX_TEXT_LEN_SEPTET - 60 ;
  }
  else
  {
 dcs  = SMSAL_UCS2_DCS ;
 if (j == 1)
chars = 200 ;
 else
chars = MAX_CHAR_UCS2 - EMS_TEST_MAX_SEG * (5 + 1)/2 -2;
/* chars = MAX_CHAR_UCS2 - 20 ; */
  }

  InitializeEMSData(&emsData, dcs);
  total_chars = 0;

  ResetTextFormat(&tf);
  tf.isItalic = KAL_TRUE;
  ret = EMSTestAddString(chars , &tf);
  ASSERT(ret==EMS_OK);
  total_chars += chars;

  ResetTextFormat(&tf);
  tf.Alignment = EMS_ALIGN_CENTER ;
  tf.textLength = total_chars * 2 ;
  ret = AddTextAlignment(&emsData, 0, &tf);

  if (j==3 || j==2)
  {
   //  ASSERT(ret != EMS_OK);
  }
  else
  {
 ASSERT(ret == EMS_OK);
  }

  ret=EMSPack(&emsData, 1,&numOfMsg, TPUD_p, TPUDLen);
  ASSERT(ret == EMS_OK);
  EMSDataDump(&emsData);


  if(ret == EMS_OK)
  {
 InitializeEMSData(&emsData1, 0);
 /* set dcs & udhi */
 emsData1.dcs = emsData.dcs;
 emsData1.udhi = emsData.udhi;

 ret=EMSUnPack(&emsData1,emsData.udhi, numOfMsg, TPUD_p, TPUDLen);
 sprintf(buf, "UnPack: ret=%d\n", ret);
 kal_print((kal_char*)buf);

 EMSDataDump(&emsData1);

 if(ret == EMS_OK)
 {
kal_uint8 isSame = EMSDataCmp(&emsData, &emsData1, 0);
sprintf(buf, "EMSDataCmp: ret=%d\n", isSame);
kal_print((kal_char*)buf);
ASSERT(isSame == 0);
 }
 ReleaseEMSData(&emsData1);
  }

  ReleaseEMSData(&emsData);
   }

   EMSResourceVerdict();
}

void EMSTestNewTxtAlignment_06()
{
   EMSTATUS ret;
   EMSTextFormat tf;
   kal_uint8 numOfMsg;
   kal_uint8 i,j;
   kal_uint16 chars, total_chars ;
   kal_uint8 dcs;
   kal_uint8 buf[64];

   for (i=0; i < EMS_TEST_MAX_SEG; i++)
  TPUDLen[i]= 0;

   /* j = 0, 1 for succeeded case */
   /* j = 2, 3 for fail case  */
   for (j=0; j< 4 ;j++)
   {
  if (j==0 || j==2)
  {
 dcs  = SMSAL_DEFAULT_DCS ;
 if (j == 0)
chars = 200 ;
 else
chars = MAX_TEXT_LEN_SEPTET - 65 ;
  }
  else
  {
 dcs  = SMSAL_UCS2_DCS ;
 if (j == 1)
chars = 200 ;
 else
chars = MAX_CHAR_UCS2 - EMS_TEST_MAX_SEG * (5 + 1)/2 -2;
  }

  InitializeEMSData(&emsData, dcs);
  total_chars = 0;

  ResetTextFormat(&tf);
  tf.isItalic = KAL_TRUE;
  ret = EMSTestAddString(chars , &tf);
  ASSERT(ret==EMS_OK);
  total_chars += chars;

  BackwardCurrentPosition(&emsData, 10);
  
  ResetTextFormat(&tf);
  tf.Alignment = EMS_ALIGN_CENTER ;
  tf.textLength = (total_chars -10) * 2 ;
  ret = AddTextAlignment(&emsData, 0, &tf);

  if (j==3 || j==2)
  {
// ASSERT(ret != EMS_OK);
  }
  else
  {
 ASSERT(ret == EMS_OK);
  }

  ret=EMSPack(&emsData, 1,&numOfMsg, TPUD_p, TPUDLen);
  ASSERT(ret == EMS_OK);
  EMSDataDump(&emsData);


  if(ret == EMS_OK)
  {
 InitializeEMSData(&emsData1, 0);
 /* set dcs & udhi */
 emsData1.dcs = emsData.dcs;
 emsData1.udhi = emsData.udhi;

 ret=EMSUnPack(&emsData1,emsData.udhi, numOfMsg, TPUD_p, TPUDLen);
 sprintf(buf, "UnPack: ret=%d\n", ret);
 kal_print((kal_char*)buf);

 EMSDataDump(&emsData1);

 if(ret == EMS_OK)
 {
kal_uint8 isSame = EMSDataCmp(&emsData, &emsData1, 0);
sprintf(buf, "EMSDataCmp: ret=%d\n", isSame);
kal_print((kal_char*)buf);
ASSERT(isSame == 0);
 }
 ReleaseEMSData(&emsData1);
  }

  ReleaseEMSData(&emsData);
   }

   EMSResourceVerdict();
}

void EMSTestNewTxtAlignment_07()
{
   EMSTATUS ret;
   EMSTextFormat tf;
   kal_uint8 numOfMsg;
   kal_uint8 i,j;
   kal_uint16 chars, total_chars ;
   kal_uint8 dcs;
   kal_uint8 buf[64];

   for (i=0; i < EMS_TEST_MAX_SEG; i++)
  TPUDLen[i]= 0;

   /* j = 0, 1 for succeeded case */
   /* j = 2, 3 for fail case  */
   for (j=0; j< 4 ;j++)
   {
  if (j==0 || j==2)
  {
 dcs  = SMSAL_DEFAULT_DCS ;
 if (j == 0)
chars = 200 ;
 else
chars = MAX_TEXT_LEN_SEPTET - 69 ;
  }
  else
  {
 dcs  = SMSAL_UCS2_DCS ;
 if (j == 1)
chars = 200 ;
 else
chars = MAX_CHAR_UCS2 - EMS_TEST_MAX_SEG * (5 + 1)/2 -2;
  }

  InitializeEMSData(&emsData, dcs);
  total_chars = 0;

  /* normal characters */
  ResetTextFormat(&tf);
  ret = EMSTestAddString(30 , &tf);
  ASSERT(ret==EMS_OK);
  total_chars += 30;

  ret=AddObject(&emsData, EMS_TYPE_PREDEF_ANM, NULL, 0);
  ASSERT(ret==EMS_OK);

  ResetTextFormat(&tf);
  tf.isItalic = KAL_TRUE;
  ret = EMSTestAddString(chars-60 , &tf);
  ASSERT(ret==EMS_OK);
  total_chars += (chars-60);

  /* normal characters */
  ResetTextFormat(&tf);
  ret = EMSTestAddString(30 , &tf);
  ASSERT(ret==EMS_OK);
  total_chars += 30;

  ResetTextFormat(&tf);
  tf.Alignment = EMS_ALIGN_CENTER ;
  tf.textLength = total_chars * 2 ;
  ret = AddTextAlignment(&emsData, 0, &tf);

  if (j==3 || j==2)
  {
 ASSERT(ret != EMS_OK);
  }
  else
  {
 ASSERT(ret == EMS_OK);
  }

  ret=EMSPack(&emsData, 1,&numOfMsg, TPUD_p, TPUDLen);
  ASSERT(ret == EMS_OK);
  EMSDataDump(&emsData);


  if(ret == EMS_OK)
  {
 InitializeEMSData(&emsData1, 0);
 /* set dcs & udhi */
 emsData1.dcs = emsData.dcs;
 emsData1.udhi = emsData.udhi;

 ret=EMSUnPack(&emsData1,emsData.udhi, numOfMsg, TPUD_p, TPUDLen);
 sprintf(buf, "UnPack: ret=%d\n", ret);
 kal_print((kal_char*)buf);

 EMSDataDump(&emsData1);

 if(ret == EMS_OK)
 {
kal_uint8 isSame = EMSDataCmp(&emsData, &emsData1, 0);
sprintf(buf, "EMSDataCmp: ret=%d\n", isSame);
kal_print((kal_char*)buf);
ASSERT(isSame == 0);
 }
 ReleaseEMSData(&emsData1);
  }

  ReleaseEMSData(&emsData);
   }

   EMSResourceVerdict();
}

void EMSTestNewTxtAlignment_08()
{
   EMSTATUS ret;
   EMSTextFormat tf;
   kal_uint8 numOfMsg;
   kal_uint8 i,j;
   kal_uint16 chars, total_chars ;
   kal_uint8 dcs;
   kal_uint8 buf[64];

   for (i=0; i < EMS_TEST_MAX_SEG; i++)
  TPUDLen[i]= 0;

   /* j = 0, 1 for succeeded case */
   /* j = 2, 3 for fail case  */
   for (j=0; j< 4 ;j++)
   {
  if (j==0 || j==2)
  {
 dcs  = SMSAL_DEFAULT_DCS ;
 if (j == 0)
chars = 200 ;
 else
chars = MAX_TEXT_LEN_SEPTET - 69 ;
  }
  else
  {
 dcs  = SMSAL_UCS2_DCS ;
 if (j == 1)
chars = 200 ;
 else
chars = MAX_CHAR_UCS2 - EMS_TEST_MAX_SEG * (5 + 1)/2 -2;
  }

  InitializeEMSData(&emsData, dcs);
  total_chars = 0;

  /* normal characters */
  ResetTextFormat(&tf);
  ret = EMSTestAddString(30 , &tf);
  ASSERT(ret==EMS_OK);
  total_chars += 30;

  ret=AddObject(&emsData, EMS_TYPE_PREDEF_ANM, NULL, 0);
  ASSERT(ret==EMS_OK);

  ResetTextFormat(&tf);
  tf.isItalic = KAL_TRUE;
  ret = EMSTestAddString(chars-30 , &tf);
  ASSERT(ret==EMS_OK);
  total_chars += (chars-30);

  ResetTextFormat(&tf);
  tf.Alignment = EMS_ALIGN_CENTER ;
  tf.textLength = total_chars * 2 ;
  ret = AddTextAlignment(&emsData, 0, &tf);

  if (j==3 || j==2)
  {
 ASSERT(ret != EMS_OK);
  }
  else
  {
 ASSERT(ret == EMS_OK);
  }

  ret=EMSPack(&emsData, 1,&numOfMsg, TPUD_p, TPUDLen);
  ASSERT(ret == EMS_OK);
  EMSDataDump(&emsData);


  if(ret == EMS_OK)
  {
 InitializeEMSData(&emsData1, 0);
 /* set dcs & udhi */
 emsData1.dcs = emsData.dcs;
 emsData1.udhi = emsData.udhi;

 ret=EMSUnPack(&emsData1,emsData.udhi, numOfMsg, TPUD_p, TPUDLen);
 sprintf(buf, "UnPack: ret=%d\n", ret);
 kal_print((kal_char*)buf);

 EMSDataDump(&emsData1);

 if(ret == EMS_OK)
 {
kal_uint8 isSame = EMSDataCmp(&emsData, &emsData1, 0);
sprintf(buf, "EMSDataCmp: ret=%d\n", isSame);
kal_print((kal_char*)buf);
ASSERT(isSame == 0);
 }
 ReleaseEMSData(&emsData1);
  }

  ReleaseEMSData(&emsData);
   }

   EMSResourceVerdict();
}

void EMSTestNewTxtAlignment_09()
{
   EMSTATUS ret;
   EMSTextFormat tf;
   kal_uint8 numOfMsg;
   kal_uint8 i,j;
   kal_uint16 chars, total_chars ;
   kal_uint8 dcs;
   kal_uint8 buf[64];

   for (i=0; i < EMS_TEST_MAX_SEG; i++)
  TPUDLen[i]= 0;

   /* j = 0, 1 for succeeded case */
   /* j = 2, 3 for fail case  */
   for (j=0; j< 4 ;j++)
   {
  if (j==0 || j==2)
  {
 dcs  = SMSAL_DEFAULT_DCS ;
 if (j == 0)
chars = 200 ;
 else
chars = MAX_TEXT_LEN_SEPTET - 69 ;
  }
  else
  {
 dcs  = SMSAL_UCS2_DCS ;
 if (j == 1)
chars = 200 ;
 else
chars = MAX_CHAR_UCS2 - EMS_TEST_MAX_SEG * (5 + 1)/2 -2;
  }

  InitializeEMSData(&emsData, dcs);
  total_chars = 0;

  /* normal characters */
  ResetTextFormat(&tf);
  ret = EMSTestAddString(30 , &tf);
  ASSERT(ret==EMS_OK);
  total_chars += 30;

  ret=AddObject(&emsData, EMS_TYPE_PREDEF_ANM, NULL, 0);
  ASSERT(ret==EMS_OK);

  ResetTextFormat(&tf);
  tf.isItalic = KAL_TRUE;
  ret = EMSTestAddString(chars-30 , &tf);
  ASSERT(ret==EMS_OK);
  total_chars += (chars-30);

  BackwardCurrentPosition(&emsData, 10);

  ResetTextFormat(&tf);
  tf.Alignment = EMS_ALIGN_CENTER ;
  tf.textLength = (total_chars -10) * 2 ;
  ret = AddTextAlignment(&emsData, 0, &tf);

  if (j==3 || j==2)
  {
 ASSERT(ret != EMS_OK);
  }
  else
  {
 ASSERT(ret == EMS_OK);
  }

  ret=EMSPack(&emsData, 1,&numOfMsg, TPUD_p, TPUDLen);
  ASSERT(ret == EMS_OK);
  EMSDataDump(&emsData);


  if(ret == EMS_OK)
  {
 InitializeEMSData(&emsData1, 0);
 /* set dcs & udhi */
 emsData1.dcs = emsData.dcs;
 emsData1.udhi = emsData.udhi;

 ret=EMSUnPack(&emsData1,emsData.udhi, numOfMsg, TPUD_p, TPUDLen);
 sprintf(buf, "UnPack: ret=%d\n", ret);
 kal_print((kal_char*)buf);

 EMSDataDump(&emsData1);

 if(ret == EMS_OK)
 {
kal_uint8 isSame = EMSDataCmp(&emsData, &emsData1, 0);
sprintf(buf, "EMSDataCmp: ret=%d\n", isSame);
kal_print((kal_char*)buf);
ASSERT(isSame == 0);
 }
 ReleaseEMSData(&emsData1);
  }

  ReleaseEMSData(&emsData);
   }

   EMSResourceVerdict();
}

void EMSTestNewTxtAlignment_10()
{
   EMSTATUS ret;
   EMSTextFormat tf;
   kal_uint8 numOfMsg;
   kal_uint8 i,j;
   kal_uint16 chars, total_chars ;
   kal_uint8 dcs;
   kal_uint8 buf[64];

   for (i=0; i < EMS_TEST_MAX_SEG; i++)
  TPUDLen[i]= 0;

   /* j = 0, 1 for succeeded case */
   /* j = 2, 3 for fail case  */
   for (j=0; j< 4 ;j++)
   {
  if (j==0 || j==2)
  {
 dcs  = SMSAL_DEFAULT_DCS ;
 if (j == 0)
chars = 200 ;
 else
chars = MAX_TEXT_LEN_SEPTET - 80 ;
  }
  else
  {
 dcs  = SMSAL_UCS2_DCS ;
 if (j == 1)
chars = 200 ;
 else
/* chars = MAX_CHAR_UCS2 - 27 ; */
chars = MAX_CHAR_UCS2 - EMS_TEST_MAX_SEG * (5 + 1)/2 -7;
  }

  InitializeEMSData(&emsData, dcs);
  total_chars = 0;

  ResetTextFormat(&tf);
  tf.isItalic = KAL_TRUE;
  ret = EMSTestAddString(30 , &tf);
  ASSERT(ret==EMS_OK);
  total_chars += 30;

  ret=AddObject(&emsData, EMS_TYPE_PREDEF_ANM, NULL, 0);
  ASSERT(ret==EMS_OK);

  /* normal characters */
  ResetTextFormat(&tf);
  ret = EMSTestAddString(30 , &tf);
  ASSERT(ret==EMS_OK);
  total_chars += 30;

  ResetTextFormat(&tf);
  tf.isBold = KAL_TRUE;
  ret = EMSTestAddString(30 , &tf);
  ASSERT(ret==EMS_OK);
  total_chars += 30;

  /* normal characters */
  ResetTextFormat(&tf);
  ret = EMSTestAddString(30 , &tf);
  ASSERT(ret==EMS_OK);
  total_chars += 30;

  ResetTextFormat(&tf);
  tf.isStrikethrough = KAL_TRUE;
  ret = EMSTestAddString(chars-120 , &tf);
  ASSERT(ret==EMS_OK);
  total_chars += (chars - 120);

  ResetTextFormat(&tf);
  tf.Alignment = EMS_ALIGN_CENTER ;
  tf.textLength = total_chars * 2 - 20;
  ret = AddTextAlignment(&emsData, 20, &tf);

  if (j==3 || j==2)
  {
 ASSERT(ret != EMS_OK);
  }
  else
  {
 ASSERT(ret == EMS_OK);
  }

  ret=EMSPack(&emsData, 1,&numOfMsg, TPUD_p, TPUDLen);
  ASSERT(ret == EMS_OK);
  EMSDataDump(&emsData);


  if(ret == EMS_OK)
  {
 InitializeEMSData(&emsData1, 0);
 /* set dcs & udhi */
 emsData1.dcs = emsData.dcs;
 emsData1.udhi = emsData.udhi;

 ret=EMSUnPack(&emsData1,emsData.udhi, numOfMsg, TPUD_p, TPUDLen);
 sprintf(buf, "UnPack: ret=%d\n", ret);
 kal_print((kal_char*)buf);

 EMSDataDump(&emsData1);

 if(ret == EMS_OK)
 {
kal_uint8 isSame = EMSDataCmp(&emsData, &emsData1, 0);
sprintf(buf, "EMSDataCmp: ret=%d\n", isSame);
kal_print((kal_char*)buf);
ASSERT(isSame == 0);
 }
 ReleaseEMSData(&emsData1);
  }

  ReleaseEMSData(&emsData);
   }

   EMSResourceVerdict();
}

void EMSTestNewTxtAlignment_11()
{
   EMSTATUS ret;
   EMSTextFormat tf;
   kal_uint8 numOfMsg;
   kal_uint8 i,j;
   kal_uint16 chars, total_chars ;
   kal_uint8 dcs;
   kal_uint8 buf[64];

   for (i=0; i < EMS_TEST_MAX_SEG; i++)
  TPUDLen[i]= 0;

   /* j = 0, 1 for succeeded case */
   /* j = 2, 3 for fail case  */
   for (j=0; j< 4 ;j++)
   {
  if (j==0 || j==2)
  {
 dcs  = SMSAL_DEFAULT_DCS ;
 if (j == 0)
chars = 200 ;
 else
chars = MAX_TEXT_LEN_SEPTET - 80 ;
  }
  else
  {
 dcs  = SMSAL_UCS2_DCS ;
 if (j == 1)
chars = 200 ;
 else
/* chars = MAX_CHAR_UCS2 - 27 ; */
chars = MAX_CHAR_UCS2 - EMS_TEST_MAX_SEG * (5 + 1)/2 -7;
  }

  InitializeEMSData(&emsData, dcs);
  total_chars = 0;

  ResetTextFormat(&tf);
  tf.isItalic = KAL_TRUE;
  ret = EMSTestAddString(30 , &tf);
  ASSERT(ret==EMS_OK);
  total_chars += 30;

  ret=AddObject(&emsData, EMS_TYPE_PREDEF_ANM, NULL, 0);
  ASSERT(ret==EMS_OK);

  /* normal characters */
  ResetTextFormat(&tf);
  ret = EMSTestAddString(30 , &tf);
  ASSERT(ret==EMS_OK);
  total_chars += 30;

  ResetTextFormat(&tf);
  tf.isBold = KAL_TRUE;
  ret = EMSTestAddString(30 , &tf);
  ASSERT(ret==EMS_OK);
  total_chars += 30;

  /* normal characters */
  ResetTextFormat(&tf);
  ret = EMSTestAddString(30 , &tf);
  ASSERT(ret==EMS_OK);
  total_chars += 30;

  ResetTextFormat(&tf);
  tf.isStrikethrough = KAL_TRUE;
  ret = EMSTestAddString(chars-120 , &tf);
  ASSERT(ret==EMS_OK);
  total_chars += (chars - 120);

  BackwardCurrentPosition(&emsData, 10);

  ResetTextFormat(&tf);
  tf.Alignment = EMS_ALIGN_CENTER ;
  tf.textLength = (total_chars-10) * 2 - 20;
  ret = AddTextAlignment(&emsData, 20, &tf);

  if (j==3 || j==2)
  {
 ASSERT(ret != EMS_OK);
  }
  else
  {
 ASSERT(ret == EMS_OK);
  }

  ret=EMSPack(&emsData, 1,&numOfMsg, TPUD_p, TPUDLen);
  ASSERT(ret == EMS_OK);
  EMSDataDump(&emsData);


  if(ret == EMS_OK)
  {
 InitializeEMSData(&emsData1, 0);
 /* set dcs & udhi */
 emsData1.dcs = emsData.dcs;
 emsData1.udhi = emsData.udhi;

 ret=EMSUnPack(&emsData1,emsData.udhi, numOfMsg, TPUD_p, TPUDLen);
 sprintf(buf, "UnPack: ret=%d\n", ret);
 kal_print((kal_char*)buf);

 EMSDataDump(&emsData1);

 if(ret == EMS_OK)
 {
kal_uint8 isSame = EMSDataCmp(&emsData, &emsData1, 0);
sprintf(buf, "EMSDataCmp: ret=%d\n", isSame);
kal_print((kal_char*)buf);
ASSERT(isSame == 0);
 }
 ReleaseEMSData(&emsData1);
  }

  ReleaseEMSData(&emsData);
   }

   EMSResourceVerdict();
}

void EMSTestNewTxtAlignment_12()
{
   EMSTATUS ret;
   EMSTextFormat tf;
   kal_uint8 numOfMsg;
   kal_uint8 i,j;
   kal_uint16 chars, total_chars ;
   kal_uint8 dcs;
   kal_uint8 buf[64];

   for (i=0; i < EMS_TEST_MAX_SEG; i++)
  TPUDLen[i]= 0;

   /* j = 0, 1 for succeeded case */
   /* j = 2, 3 for fail case  */
   for (j=0; j< 4 ;j++)
   {
  if (j==0 || j==2)
  {
 dcs  = SMSAL_DEFAULT_DCS ;
 if (j == 0)
chars = 200 ;
 else
chars = MAX_TEXT_LEN_SEPTET - 80 ;
  }
  else
  {
 dcs  = SMSAL_UCS2_DCS ;
 if (j == 1)
chars = 200 ;
 else
/* chars = MAX_CHAR_UCS2 - 27 ; */
chars = MAX_CHAR_UCS2 - EMS_TEST_MAX_SEG * (5 + 1)/2 -7;
  }

  InitializeEMSData(&emsData, dcs);
  total_chars = 0;

  ResetTextFormat(&tf);
  tf.isItalic = KAL_TRUE;
  ret = EMSTestAddString(30 , &tf);
  ASSERT(ret==EMS_OK);
  total_chars += 30;

  ret=AddObject(&emsData, EMS_TYPE_PREDEF_ANM, NULL, 0);
  ASSERT(ret==EMS_OK);

  /* normal characters */
  ResetTextFormat(&tf);
  ret = EMSTestAddString(30 , &tf);
  ASSERT(ret==EMS_OK);
  total_chars += 30;

  ResetTextFormat(&tf);
  tf.isBold = KAL_TRUE;
  ret = EMSTestAddString(30 , &tf);
  ASSERT(ret==EMS_OK);
  total_chars += 30;

  /* normal characters */
  ResetTextFormat(&tf);
  ret = EMSTestAddString(30 , &tf);
  ASSERT(ret==EMS_OK);
  total_chars += 30;

  ResetTextFormat(&tf);
  tf.isStrikethrough = KAL_TRUE;
  ret = EMSTestAddString(chars-120 , &tf);
  ASSERT(ret==EMS_OK);
  total_chars += (chars - 120);

  ResetTextFormat(&tf);
  tf.Alignment = EMS_ALIGN_CENTER ;
  tf.textLength = total_chars * 2 ;
  ret = AddTextAlignment(&emsData, 0, &tf);

  if (j==3 || j==2)
  {
 ASSERT(ret != EMS_OK);
  }
  else
  {
 ASSERT(ret == EMS_OK);
  }

  ret=EMSPack(&emsData, 1,&numOfMsg, TPUD_p, TPUDLen);
  ASSERT(ret == EMS_OK);
  EMSDataDump(&emsData);


  if(ret == EMS_OK)
  {
 InitializeEMSData(&emsData1, 0);
 /* set dcs & udhi */
 emsData1.dcs = emsData.dcs;
 emsData1.udhi = emsData.udhi;

 ret=EMSUnPack(&emsData1,emsData.udhi, numOfMsg, TPUD_p, TPUDLen);
 sprintf(buf, "UnPack: ret=%d\n", ret);
 kal_print((kal_char*)buf);

 EMSDataDump(&emsData1);

 if(ret == EMS_OK)
 {
kal_uint8 isSame = EMSDataCmp(&emsData, &emsData1, 0);
sprintf(buf, "EMSDataCmp: ret=%d\n", isSame);
kal_print((kal_char*)buf);
ASSERT(isSame == 0);
 }
 ReleaseEMSData(&emsData1);
  }

  ReleaseEMSData(&emsData);
   }

   EMSResourceVerdict();
}
void EMSTestNewTxtAlignment_13()
{
   EMSTATUS ret;
   EMSTextFormat tf;
   kal_uint8 numOfMsg;
   kal_uint8 i,j;
   kal_uint16 chars, total_chars ;
   kal_uint8 dcs;
   kal_uint8 buf[64];

   for (i=0; i < EMS_TEST_MAX_SEG; i++)
  TPUDLen[i]= 0;

   /* j = 0, 1 for succeeded case */
   /* j = 2, 3 for fail case  */
   for (j=0; j< 4 ;j++)
   {
  if (j==0 || j==2)
  {
 dcs  = SMSAL_DEFAULT_DCS ;
 if (j == 0)
chars = 200 ;
 else
chars = MAX_TEXT_LEN_SEPTET - 80 ;
  }
  else
  {
 dcs  = SMSAL_UCS2_DCS ;
 if (j == 1)
chars = 200 ;
 else
/* chars = MAX_CHAR_UCS2 - 27 ; */
chars = MAX_CHAR_UCS2 - EMS_TEST_MAX_SEG * (5 + 1)/2 -7;
  }

  InitializeEMSData(&emsData, dcs);
  total_chars = 0;

  ResetTextFormat(&tf);
  tf.isItalic = KAL_TRUE;
  ret = EMSTestAddString(30 , &tf);
  ASSERT(ret==EMS_OK);
  total_chars += 30;

  ret=AddObject(&emsData, EMS_TYPE_PREDEF_ANM, NULL, 0);
  ASSERT(ret==EMS_OK);

  /* normal characters */
  ResetTextFormat(&tf);
  ret = EMSTestAddString(30 , &tf);
  ASSERT(ret==EMS_OK);
  total_chars += 30;

  ResetTextFormat(&tf);
  tf.isBold = KAL_TRUE;
  ret = EMSTestAddString(30 , &tf);
  ASSERT(ret==EMS_OK);
  total_chars += 30;

  /* normal characters */
  ResetTextFormat(&tf);
  ret = EMSTestAddString(30 , &tf);
  ASSERT(ret==EMS_OK);
  total_chars += 30;

  ResetTextFormat(&tf);
  tf.isStrikethrough = KAL_TRUE;
  ret = EMSTestAddString(chars-120 , &tf);
  ASSERT(ret==EMS_OK);
  total_chars += (chars - 120);

  BackwardCurrentPosition(&emsData, 10);

  ResetTextFormat(&tf);
  tf.Alignment = EMS_ALIGN_CENTER ;
  tf.textLength = (total_chars-10) * 2 ;
  ret = AddTextAlignment(&emsData, 0, &tf);

  if (j==3 || j==2)
  {
 ASSERT(ret != EMS_OK);
  }
  else
  {
 ASSERT(ret == EMS_OK);
  }

  ret=EMSPack(&emsData, 1,&numOfMsg, TPUD_p, TPUDLen);
  ASSERT(ret == EMS_OK);
  EMSDataDump(&emsData);


  if(ret == EMS_OK)
  {
 InitializeEMSData(&emsData1, 0);
 /* set dcs & udhi */
 emsData1.dcs = emsData.dcs;
 emsData1.udhi = emsData.udhi;

 ret=EMSUnPack(&emsData1,emsData.udhi, numOfMsg, TPUD_p, TPUDLen);
 sprintf(buf, "UnPack: ret=%d\n", ret);
 kal_print((kal_char*)buf);

 EMSDataDump(&emsData1);

 if(ret == EMS_OK)
 {
kal_uint8 isSame = EMSDataCmp(&emsData, &emsData1, 0);
sprintf(buf, "EMSDataCmp: ret=%d\n", isSame);
kal_print((kal_char*)buf);
ASSERT(isSame == 0);
 }
 ReleaseEMSData(&emsData1);
  }

  ReleaseEMSData(&emsData);
   }

   EMSResourceVerdict();
}

void EMSTestNewTxtAlignment_14()
{
   EMSTATUS ret;
   EMSTextFormat tf;
   kal_uint8 numOfMsg;
   kal_uint8 i,j;
   kal_uint16 chars, total_chars ;
   kal_uint8 dcs;
   kal_uint8 buf[64];

   for (i=0; i < EMS_TEST_MAX_SEG; i++)
  TPUDLen[i]= 0;

   /* j = 0, 1 for succeeded case */
   for (j=0; j< 2 ;j++)
   {
  if (j==0 || j==2)
  {
 dcs  = SMSAL_DEFAULT_DCS ;
 if (j == 0)
chars = 200 ;
 else
chars = MAX_TEXT_LEN_SEPTET - 80 ;
  }
  else
  {
 dcs  = SMSAL_UCS2_DCS ;
 if (j == 1)
chars = 200 ;
 else
/* chars = MAX_CHAR_UCS2 - 27 ; */
chars = MAX_CHAR_UCS2 - EMS_TEST_MAX_SEG * (5 + 1)/2 -7;
  }

  InitializeEMSData(&emsData, dcs);
  total_chars = 0;

  ResetTextFormat(&tf);
  tf.isItalic = KAL_TRUE;
  ret = EMSTestAddString(30 , &tf);
  ASSERT(ret==EMS_OK);
  total_chars += 30;

  ret=AddObject(&emsData, EMS_TYPE_PREDEF_ANM, NULL, 0);
  ASSERT(ret==EMS_OK);

  /* normal characters */
  ResetTextFormat(&tf);
  ret = EMSTestAddString(30 , &tf);
  ASSERT(ret==EMS_OK);
  total_chars += 30;

  ResetTextFormat(&tf);
  tf.isBold = KAL_TRUE;
  ret = EMSTestAddString(30 , &tf);
  ASSERT(ret==EMS_OK);
  total_chars += 30;

  /* normal characters */
  ResetTextFormat(&tf);
  ret = EMSTestAddString(30 , &tf);
  ASSERT(ret==EMS_OK);
  total_chars += 30;

  ResetTextFormat(&tf);
  tf.isStrikethrough = KAL_TRUE;
  ret = EMSTestAddString(chars-120 , &tf);
  ASSERT(ret==EMS_OK);
  total_chars += (chars - 120);

  BackwardCurrentPosition(&emsData, 10);

  EMSDataDump(&emsData);

  ResetTextFormat(&tf);
  tf.Alignment = EMS_ALIGN_CENTER ;
  tf.textLength = (total_chars-10) * 2 - 20;
  ret = AddTextAlignment(&emsData, 20, &tf);

  if (j==3 || j==2)
  {
 ASSERT(ret != EMS_OK);
  }
  else
  {
 ASSERT(ret == EMS_OK);
  }

  EMSDataDump(&emsData);

  ResetTextFormat(&tf);
  tf.textLength = (total_chars-10) * 2 - 20;
  ret = AddTextAlignment(&emsData, 20, &tf);
	  ASSERT(ret == EMS_OK);
  EMSDataDump(&emsData);

  ret=EMSPack(&emsData, 1,&numOfMsg, TPUD_p, TPUDLen);
  ASSERT(ret == EMS_OK);

  if(ret == EMS_OK)
  {
 InitializeEMSData(&emsData1, 0);
 /* set dcs & udhi */
 emsData1.dcs = emsData.dcs;
 emsData1.udhi = emsData.udhi;

 ret=EMSUnPack(&emsData1,emsData.udhi, numOfMsg, TPUD_p, TPUDLen);
 sprintf(buf, "UnPack: ret=%d\n", ret);
 kal_print((kal_char*)buf);

 EMSDataDump(&emsData1);

 if(ret == EMS_OK)
 {
kal_uint8 isSame = EMSDataCmp(&emsData, &emsData1, 0);
sprintf(buf, "EMSDataCmp: ret=%d\n", isSame);
kal_print((kal_char*)buf);
ASSERT(isSame == 0);
 }
 ReleaseEMSData(&emsData1);
  }

  ReleaseEMSData(&emsData);
   }

   EMSResourceVerdict();
}

void EMSTestNewTxtAlignment_15()
{
   EMSTATUS ret;
   EMSTextFormat tf;
   kal_uint8 numOfMsg;
   kal_uint8 i,j;
   kal_uint16 chars, total_chars ;
   kal_uint8 dcs;
   kal_uint8 buf[64];

   for (i=0; i < EMS_TEST_MAX_SEG; i++)
  TPUDLen[i]= 0;

   /* j = 0, 1 for succeeded case */
   for (j=0; j< 2 ;j++)
   {
  if (j==0 || j==2)
  {
 dcs  = SMSAL_DEFAULT_DCS ;
 if (j == 0)
chars = 200 ;
 else
chars = MAX_TEXT_LEN_SEPTET - 80 ;
  }
  else
  {
 dcs  = SMSAL_UCS2_DCS ;
 if (j == 1)
chars = 200 ;
 else
/* chars = MAX_CHAR_UCS2 - 27 ; */
chars = MAX_CHAR_UCS2 - EMS_TEST_MAX_SEG * (5 + 1)/2 -7;
  }

  InitializeEMSData(&emsData, dcs);
  total_chars = 0;

  /* normal characters */
  ResetTextFormat(&tf);
  ret = EMSTestAddString(30 , &tf);
  ASSERT(ret==EMS_OK);
  total_chars += 30;

  ret=AddObject(&emsData, EMS_TYPE_PREDEF_ANM, NULL, 0);
  ASSERT(ret==EMS_OK);

  /* normal characters */
  ResetTextFormat(&tf);
  ret = EMSTestAddString(chars-30 , &tf);
  ASSERT(ret==EMS_OK);
  total_chars += (chars - 30);


  ResetTextFormat(&tf);
  tf.Alignment = EMS_ALIGN_CENTER ;
  tf.textLength = (total_chars) * 2 ;
  ret = AddTextAlignment(&emsData, 0, &tf);

  BackwardCurrentPosition(&emsData, 170);

  /* bold characters */
  ResetTextFormat(&tf);
  tf.isBold = KAL_TRUE;
  ret = EMSTestAddString(30 , &tf);
  ASSERT(ret==EMS_OK);

  ForwardCurrentPosition(&emsData, 30);

  /* characters */
  ResetTextFormat(&tf);
  tf.isItalic = KAL_TRUE;
  ret = EMSTestAddString(30 , &tf);
  ASSERT(ret==EMS_OK);

  ForwardCurrentPosition(&emsData, 30);

  /* characters */
  ResetTextFormat(&tf);
  tf.isStrikethrough = KAL_TRUE;
  ret = EMSTestAddString(30 , &tf);
  ASSERT(ret==EMS_OK);

  ret=EMSPack(&emsData, 1,&numOfMsg, TPUD_p, TPUDLen);
  ASSERT(ret == EMS_OK);
  EMSDataDump(&emsData);

  if(ret == EMS_OK)
  {
 InitializeEMSData(&emsData1, 0);
 /* set dcs & udhi */
 emsData1.dcs = emsData.dcs;
 emsData1.udhi = emsData.udhi;

 ret=EMSUnPack(&emsData1,emsData.udhi, numOfMsg, TPUD_p, TPUDLen);
 sprintf(buf, "UnPack: ret=%d\n", ret);
 kal_print((kal_char*)buf);

 EMSDataDump(&emsData1);

 if(ret == EMS_OK)
 {
kal_uint8 isSame = EMSDataCmp(&emsData, &emsData1, 0);
sprintf(buf, "EMSDataCmp: ret=%d\n", isSame);
kal_print((kal_char*)buf);
ASSERT(isSame == 0);
 }
 ReleaseEMSData(&emsData1);
  }

  ReleaseEMSData(&emsData);
   }

   EMSResourceVerdict();
}

void EMSTestNewTxtAlignmentSolution()
{

   /* for case 1 - 9, included fail case */
   EMSTestNewTxtAlignment_01();
   EMSTestNewTxtAlignment_02();
   EMSTestNewTxtAlignment_03();
   EMSTestNewTxtAlignment_04();
   EMSTestNewTxtAlignment_05();
   EMSTestNewTxtAlignment_06();
   EMSTestNewTxtAlignment_07();
   EMSTestNewTxtAlignment_08();
   EMSTestNewTxtAlignment_09();

   /* combine 1,7,8 */
   EMSTestNewTxtAlignment_10();
   /* combine 1,7,9 */
   EMSTestNewTxtAlignment_11();
   /* combine 4,7,8 */
   EMSTestNewTxtAlignment_12();
   /* combine 4,7,9 */
   EMSTestNewTxtAlignment_13();

   /* reset(remove) the txt fmt */
   EMSTestNewTxtAlignment_14();

   /* there is txt alignment, then insert new txt fmt. */
   EMSTestNewTxtAlignment_15();

}

void EMSTestCodeTrace(void)
{
   kal_uint16 n;
   EMSTATUS ret;
   EMSTextFormat tf;
   kal_uint16 i,j;
   kal_uint8 numOfMsg;   

   for(j=0;j<4;j++)
   {
  ems_mem_set(TPUD_p[j], 0, 160);   
   }
   ////////////////
   // bold
   ////////////////
   InitializeEMSData(&emsData, 0);
  
   for(j=0;j<500;j++)
  string[j]='a';

   ret=AddString(&emsData, string, 147 , NULL); /* add "bold" */

   ems_mem_set(string, 0, 600);   
   EMSTestFillString(string, 8,"italicii");
   ResetTextFormat(&tf);
   tf.isItalic = KAL_TRUE;
   
   ret=AddString(&emsData, string, 8, &tf); /* add "italic" */
   ASSERT(ret==EMS_OK);

   ret=EMSPack(&emsData,1, &numOfMsg, TPUD_p, TPUDLen);
   ASSERT(ret==EMS_OK);

   EMSDataDump(&emsData);

   ReleaseEMSData(&emsData);
   EMSResourceVerdict();
}

void EMSReadRel5ObjData(void)
{
   kal_uint8 buf[64];
   FILE *fp;
   kal_uint32i,j;
   kal_int16 ret=0, pdu_len;
   kal_int8  type;
   EMSObjData*objData;
   kal_int8  c;
   kal_int16 vDim, hDim, bitsPerPixel, cFrame;

   for(i=0; i< NUM_EMS_EXT_TEST_OBJ_DATA ; i++)
   {

  if (i>= 10)
 sprintf(buf, "%s\\MO\\ext_obj_data_%d.txt",EMS_TEST_DATA_PATH ,  i);
  else
 sprintf(buf, "%s\\MO\\ext_obj_data_0%d.txt",EMS_TEST_DATA_PATH , i);

  fp = fopen(buf, "r");
  ASSERT(fp != NULL);


  ret = fscanf (fp, "%X\n", &type);
  ASSERT (ret != EOF );

  objData = &(EMSTestExtObjData[i].objData);

  switch (type)
  {
 case EMS_TYPE_PIC:
EMSTestExtObjData[i].type = EMS_TYPE_PIC ;
fscanf (fp, "%hX ",  &bitsPerPixel);
fscanf (fp, "%hX ",  &hDim);
fscanf (fp, "%hX\n", &vDim);

objData->picture.bitsPerPixel = (kal_uint8) bitsPerPixel;
objData->picture.hDim = hDim;
objData->picture.vDim = vDim;

fscanf (fp, "%hX\n", &(objData->picture.pdu_length));

pdu_len = ((objData->picture.bitsPerPixel * 
objData->picture.hDim * 
objData->picture.vDim + 7) >>3 ) ;

ASSERT(pdu_len == objData->picture.pdu_length);
break;
 case EMS_TYPE_USERDEF_ANM:

EMSTestExtObjData[i].type = EMS_TYPE_USERDEF_ANM;
fscanf (fp, "%hX ",  &bitsPerPixel);
fscanf (fp, "%hX ",  &hDim);
fscanf (fp, "%hX ",  &vDim);
fscanf (fp, "%hX\n",  &cFrame);
fscanf (fp, "%hX\n", &(objData->animation.pdu_length));

objData->animation.bitsPerPixel = bitsPerPixel;
objData->animation.hDim = hDim ;
objData->animation.vDim = vDim ;
objData->animation.cFrame   = cFrame ;

pdu_len = ((objData->animation.bitsPerPixel * 
objData->animation.hDim * 
objData->animation.vDim + 7) >>3 ) * 
objData->animation.cFrame; 

ASSERT(pdu_len == objData->animation.pdu_length);

break;
 case EMS_TYPE_USERDEF_SND:
 case EMS_TYPE_VCARD:
 case EMS_TYPE_VCALENDAR:

EMSTestExtObjData[i].type = type;
fscanf (fp, "%hX\n", &(objData->animation.pdu_length));

break;

break;
 default:
ASSERT(0);
  }

  objData->common.pdu = (kal_uint8 *) malloc (objData->common.pdu_length);
  for (j = 0 ; j< objData->common.pdu_length; j++)
  {
 ret = fscanf (fp, "%X ", &c);
 ASSERT (ret != EOF );

 *(objData->common.pdu + j) = c ;
  }

  fclose(fp);
   }

   return ;

}
kal_bool EMSReadRel5RawData (kal_uint8 *filename, kal_uint8 *total_seg, 
 kal_uint8 *TPUD_p[], kal_uint8 *TPUDLen )
{

   FILE *fp;
   kal_uint32 i,j;
   kal_int16 ret=0;
   kal_int8  seg_num,c;

   fp = fopen(filename, "r");
   ASSERT(fp != NULL);

   ret = fscanf (fp, "%X\n", total_seg);
   ASSERT (ret != EOF );

   for(i=0; i< *total_seg; i++)
   {
  ret = fscanf (fp, "%X %X\n", &seg_num, TPUDLen + i);
  ASSERT (ret != EOF );
  ASSERT (seg_num ==(i+1));

  for (j = 0 ; j< *(TPUDLen + i); j++)
  {
 ret = fscanf (fp, "%X ", &c);
 ASSERT (ret != EOF );

 *(TPUD_p[i]+j) = c ;
  }
   }

   fclose(fp);
   return KAL_TRUE;

}

/* insert a valid picture */
void EMSExt_test_01(void)
{

   kal_uint8 numOfMsg;   
   kal_uint8 i, j;
   kal_int16 ret=0;
   kal_uint8 buf[64];

   EMSObjData  objData;
   for (i=0; i < EMS_TEST_MAX_SEG; i++)
   {
  TPUDLen[i]= 0;
   }


   for(j=0; j < 3; j++)
   {
  for(i=0; i< NUM_EMS_EXT_TEST_OBJ_DATA ; i++)
  {

 InitializeEMSData(&emsData, 0);

 if (j=2)
EMSSetCompressMode(KAL_TRUE);

 objData = EMSTestExtObjData[i].objData ;

 if (j==1)
objData.common.attribute = EMS_ATTB_NOT_FW;
 else
objData.common.attribute = EMS_ATTB_NORMAL;

 ret=AddObject(&emsData, EMSTestExtObjData[i].type, &objData, 0);
 ASSERT(ret == EMS_OK);

 ret=EMSPack(&emsData,1, &numOfMsg, TPUD_p, TPUDLen);
 EMSDataDump(&emsData);

 if (j==1) /* no compression, not forward */
 {
if (i != 3)
{
   ASSERT(TPUD_p[0][12]==EMS_ATTB_NOT_FW);
}
else
{
   ASSERT(TPUD_p[0][6]==EMS_ATTB_NOT_FW); 
}
 }
 else if (j==0) /* no compression, normal */
 {
if (i != 3)
{
   ASSERT(TPUD_p[0][12]==EMS_ATTB_NORMAL); 
}
else
{
   ASSERT(TPUD_p[0][6]==EMS_ATTB_NORMAL); 
}
 }

 if(ret == EMS_OK)
 {
InitializeEMSData(&emsData1, 0);
/* set dcs & udhi */
emsData1.dcs = emsData.dcs;
emsData1.udhi = emsData.udhi;

ret=EMSUnPack(&emsData1,emsData.udhi, numOfMsg, TPUD_p, TPUDLen);
sprintf(buf, "UnPack: ret=%d\n", ret);
kal_print((kal_char*)buf);

EMSDataDump(&emsData1);

if(ret == EMS_OK)
{
   kal_uint8 isSame = EMSDataCmp(&emsData, &emsData1, 0);
   sprintf(buf, "EMSDataCmp: ret=%d\n", isSame);
   kal_print((kal_char*)buf);

   ASSERT(isSame == 0);
}

ReleaseEMSData(&emsData1);
 }
 else
 {
ASSERT(KAL_FALSE);
 }

 ReleaseEMSData(&emsData);
  }
   }

   EMSResourceVerdict();
   EMSSetCompressMode(KAL_FALSE);
}

/* insert a valid picture */
void EMSExt_test_02(void)
{

   kal_uint8 i;
   kal_uint8 type;
   EMSObjData objData;
   kal_int16 ret=0;

   for (i=0; i < EMS_TEST_MAX_SEG; i++)
   {
  TPUDLen[i]= 0;
   }

   for(i=0; i< EMS_EXT_TEST_OBJ_SND_128; i++)
   {
  InitializeEMSData(&emsData, 0);

  type= EMSTestExtObjData[i].type ;
  objData = EMSTestExtObjData[i].objData ;

  switch (i)
  {
 /* wrong bitsPerPixel */
 case EMS_EXT_TEST_OBJ_PIC_1B_128:
 case EMS_EXT_TEST_OBJ_ANM_1B_128:
objData.picture.bitsPerPixel = 3 ;

break;
 case EMS_EXT_TEST_OBJ_PIC_2B_128:
 case EMS_EXT_TEST_OBJ_ANM_2B_128:
objData.picture.bitsPerPixel = 5 ;

break;
 case EMS_EXT_TEST_OBJ_PIC_6B_128:  
 case EMS_EXT_TEST_OBJ_ANM_6B_128:
objData.picture.bitsPerPixel = 8 ;

break;

/* wrong length */
 case EMS_EXT_TEST_OBJ_PIC_2B_64:   
 case EMS_EXT_TEST_OBJ_PIC_1B_512:  
 case EMS_EXT_TEST_OBJ_PIC_2B_512:
 case EMS_EXT_TEST_OBJ_PIC_6B_512:   
 case EMS_EXT_TEST_OBJ_ANM_1B_512 : 
 case EMS_EXT_TEST_OBJ_ANM_2B_512 : 
 case EMS_EXT_TEST_OBJ_ANM_6B_512 : 
objData.picture.vDim = objData.picture.vDim/2;

break;
 case EMS_EXT_TEST_OBJ_PIC_1B_1024 : 
 case EMS_EXT_TEST_OBJ_PIC_2B_1024 : 
 case EMS_EXT_TEST_OBJ_PIC_6B_1024 : 
objData.picture.hDim = objData.picture.hDim/2;

break;
 case EMS_EXT_TEST_OBJ_ANM_1B_1024 : 
 case EMS_EXT_TEST_OBJ_ANM_2B_1024 : 
 case EMS_EXT_TEST_OBJ_ANM_6B_1024 : 
objData.animation.cFrame = objData.animation.cFrame / 2 ;

break;

 default:
continue;
  }


  ret=AddObject(&emsData, type, &objData, 0);
  ASSERT(ret != EMS_OK);

  ReleaseEMSData(&emsData);
  EMSResourceVerdict();
   }

}

void EMSExt_test_03(void)
{   
   kal_uint16 i;
   kal_uint16 numOfChar=161;

   kal_uint8 numOfMsg;   
   kal_int16 ret=0;
   kal_uint8 buf[64];

   EMSObjData objData;

   for (i=0; i < EMS_TEST_MAX_SEG; i++)
   {
  TPUDLen[i]= 0;
   }

   
   /* Predefined Sound */
   InitializeEMSData(&emsData, 0);
   AddObject(&emsData, EMS_TYPE_PREDEF_SND, NULL, 0);
   i = EMS_EXT_TEST_OBJ_PIC_1B_128 ;
   ret=AddObject(&emsData, EMSTestExtObjData[i].type, &(EMSTestExtObjData[i].objData), 0);
   ASSERT(ret == EMS_OK);
   
   ret=EMSPack(&emsData,1, &numOfMsg, TPUD_p, TPUDLen);
   ASSERT(ret == EMS_OK);
   EMSDataDump(&emsData);

   /* UDHL= 1, concat_info= 6, */
   ASSERT(TPUD_p[0][7]==0x0b); /*predefined snd */
   ASSERT(TPUD_p[0][8]==0x2); 
   ReleaseEMSData(&emsData);


   /* Predefined Animation */
   InitializeEMSData(&emsData, 0);
   AddObject(&emsData, EMS_TYPE_PREDEF_ANM, NULL, 0);
   i = EMS_EXT_TEST_OBJ_ANM_1B_128 ;
   ret=AddObject(&emsData, EMSTestExtObjData[i].type, &(EMSTestExtObjData[i].objData), 0);
   ASSERT(ret == EMS_OK);
   
   ret=EMSPack(&emsData,1, &numOfMsg, TPUD_p, TPUDLen);
   ASSERT(ret == EMS_OK);
   EMSDataDump(&emsData);

   /* UDHL= 1, concat_info= 6, */
   ASSERT(TPUD_p[0][7] ==0x0d); /* predefined anm */
   ASSERT(TPUD_p[0][8] ==0x2); 
   ASSERT(TPUD_p[0][11]==0x14); /* extended object */
   ReleaseEMSData(&emsData);


   /* Picture */
   InitializeEMSData(&emsData, 0);

   objData.picture.vDim = 16; 
   objData.picture.hDim = 16;
   objData.picture.pdu_length = 32;
   objData.picture.pdu = small_pic1;
   objData.picture.bitsPerPixel = 1; /* mono picture */
   AddObject(&emsData, EMS_TYPE_PIC, &objData, 0);

   i = EMS_EXT_TEST_OBJ_PIC_1B_128 ;
   ret=AddObject(&emsData, EMSTestExtObjData[i].type, &(EMSTestExtObjData[i].objData), 0);
   ASSERT(ret == EMS_OK);
   
   ret=EMSPack(&emsData,1, &numOfMsg, TPUD_p, TPUDLen);
   ASSERT(ret == EMS_OK);
   EMSDataDump(&emsData);

   /* UDHL= 1, concat_info= 6, */
   ASSERT(TPUD_p[0][7] ==0x11); /* small pic */
   ASSERT(TPUD_p[0][8] ==33); 
   ASSERT(TPUD_p[0][42]==0x14); /* extended object */
   ReleaseEMSData(&emsData);


   /* Small Animation */
   InitializeEMSData(&emsData, 0);

   objData.animation.vDim = 8;
   objData.animation.hDim = 8;
   objData.animation.bitsPerPixel = 1;
   objData.animation.cFrame = 4;
   objData.animation.pdu_length = 32;  
   objData.animation.pdu = small_anm;
   AddObject(&emsData, EMS_TYPE_USERDEF_ANM, &objData, 0);

   i = EMS_EXT_TEST_OBJ_ANM_1B_128 ;
   ret=AddObject(&emsData, EMSTestExtObjData[i].type, &(EMSTestExtObjData[i].objData), 0);
   ASSERT(ret == EMS_OK);
   
   ret=EMSPack(&emsData, 1,&numOfMsg, TPUD_p, TPUDLen);
   ASSERT(ret == EMS_OK);
   EMSDataDump(&emsData);

   /* UDHL= 1, concat_info= 6, */
   ASSERT(TPUD_p[0][7] ==0x0F); /* small pic */
   ASSERT(TPUD_p[0][8] ==33); 
   ASSERT(TPUD_p[0][42]==0x14); /* extended object */
   ReleaseEMSData(&emsData);




   /*  user def. snd */
   InitializeEMSData(&emsData, 0);

   objData.sound.pdu_length = 32;  
   objData.sound.pdu = small_anm;   
   AddObject(&emsData, EMS_TYPE_USERDEF_SND, &objData, 0);

   i = EMS_EXT_TEST_OBJ_SND_128;
   ret=AddObject(&emsData, EMSTestExtObjData[i].type, &(EMSTestExtObjData[i].objData), 0);
   ASSERT(ret == EMS_OK);
   
   ret=EMSPack(&emsData, 1,&numOfMsg, TPUD_p, TPUDLen);
   ASSERT(ret == EMS_OK);
   EMSDataDump(&emsData);

   /* UDHL= 1, concat_info= 6, */
   ASSERT(TPUD_p[0][7] ==0x0c); /* small pic */
   ASSERT(TPUD_p[0][8] ==33); 
   ASSERT(TPUD_p[0][42]==0x14); /* extended object */
   ReleaseEMSData(&emsData);

   EMSResourceVerdict();

}

void EMSExt_test_04(void)
{   
   kal_uint16 i,j,idx;
   kal_uint8 numOfMsg;   
   kal_int16 ret=0;
   EMSObject *obj;
   kal_uint8 buf[64];

   for (i=0; i < EMS_TEST_MAX_SEG; i++)
   {
  TPUDLen[i]= 0;
   }

   for(j=0; j < 2; j++)
   {
  if(j==1)
 EMSSetCompressMode(KAL_TRUE);

  for(i=0; i< EMS_EXT_TEST_OBJ_SND_128; i++)
  {
 /* Predefined Sound */
 InitializeEMSData(&emsData, 0);

 ret=AddObject(&emsData, EMSTestExtObjData[i].type, &(EMSTestExtObjData[i].objData), 0);
 ASSERT(ret == EMS_OK);
 idx = (i+1)%EMS_EXT_TEST_OBJ_SND_128;

 ret=AddObject(&emsData, EMSTestExtObjData[idx].type, &(EMSTestExtObjData[idx].objData), 0);

 /* for compression, it may add fail because buffer more than 2048 */
 if (j!=1)
 {
ASSERT(ret == EMS_OK);

obj = emsData.listHead;

while(obj!=NULL)
{
   kal_uint8 prev_ref;

   ASSERT (EXT_OBJ_INFO_P(obj, isExtObj) == KAL_TRUE );

   if (obj->prev != emsData.listHead)
  ASSERT (prev_ref != EXT_OBJ_INFO_P(obj, ExtObjRef) );

   prev_ref = EXT_OBJ_INFO_P(obj, ExtObjRef) ;

   obj = obj->next ;
}
 }

 ret=EMSPack(&emsData, 1,&numOfMsg, TPUD_p, TPUDLen);
 ASSERT(ret == EMS_OK);
 EMSDataDump(&emsData);

 if(ret == EMS_OK)
 {
InitializeEMSData(&emsData1, 0);
/* set dcs & udhi */
emsData1.dcs = emsData.dcs;
emsData1.udhi = emsData.udhi;

ret=EMSUnPack(&emsData1,emsData.udhi, numOfMsg, TPUD_p, TPUDLen);
sprintf(buf, "UnPack: ret=%d\n", ret);
kal_print((kal_char*)buf);

EMSDataDump(&emsData1);

if(ret == EMS_OK)
{
   kal_uint8 isSame = EMSDataCmp(&emsData, &emsData1, 0);
   sprintf(buf, "EMSDataCmp: ret=%d\n", isSame);
   kal_print((kal_char*)buf);

   ASSERT(isSame == 0);
}

ReleaseEMSData(&emsData1);
 }

 ReleaseEMSData(&emsData);
  }
   }

   EMSSetCompressMode(KAL_FALSE);
   EMSResourceVerdict();

}

void EMSExt_test_05(void)
{   
   kal_uint16 i,j;
   kal_uint8 numOfMsg, dcs ;   
   kal_int16 ret=0;
   kal_uint8 buf[64];

   for (j=0; j< 4 ;j++)
   {

  if (j==0 || j==2)
 dcs = SMSAL_DEFAULT_DCS ;
  else
 dcs = SMSAL_UCS2_DCS ;

  if (j > 1 )
 EMSSetCompressMode(KAL_TRUE);

  InitializeEMSData(&emsData, dcs);
  /* Predefined Sound */
  AddObject(&emsData, EMS_TYPE_PREDEF_SND, NULL, 0);

  i = EMS_EXT_TEST_OBJ_PIC_1B_128 ;
  ret=AddObject(&emsData, EMSTestExtObjData[i].type, &(EMSTestExtObjData[i].objData), 0);
  ASSERT(ret == EMS_OK);

  /* Predefined ANN */
  AddObject(&emsData, EMS_TYPE_PREDEF_ANM, NULL, 0);

  i = EMS_EXT_TEST_OBJ_ANM_6B_512 ;
  ret=AddObject(&emsData, EMSTestExtObjData[i].type, &(EMSTestExtObjData[i].objData), 0);
  ASSERT(ret == EMS_OK);

  /* user sound*/
  i = EMS_OBJ_SND ;
  ret=AddObject(&emsData, EMSTestObjData[i].type, &(EMSTestObjData[i].objData), 0);
  ASSERT(ret == EMS_OK);

  ret=EMSPack(&emsData, 1,&numOfMsg, TPUD_p, TPUDLen);
  ASSERT(ret == EMS_OK);
  EMSDataDump(&emsData);

  if (j<2)
  {
 ASSERT (numOfMsg == 11 );

 /* UDHL= 1, concat_info= 6, */
 ASSERT(TPUD_p[0][7]  == 0x0b); /*predefined snd */
 ASSERT(TPUD_p[0][8]  == 0x2); 
 ASSERT(TPUD_p[0][11] == 0x14); /* extended IE */
 ASSERT(TPUD_p[0][12] == 0x7f); /* extended IE len */

 ASSERT(TPUD_p[1][7]  == 0x14); /* extended IE */
 ASSERT(TPUD_p[1][8]  == 0x83); /* extended IE len */

 ASSERT(TPUD_p[2][7]  == 0x14); /* extended IE */
 ASSERT(TPUD_p[2][8]  == 0x7 ); /* extended IE len */
 ASSERT(TPUD_p[2][16] == 0x0d); /*predefined anm */
 ASSERT(TPUD_p[2][17] == 0x2); 
 ASSERT(TPUD_p[2][20] == 0x14); /* extended IE */
 ASSERT(TPUD_p[2][21] == 0x76); /* extended IE len */

 for (i= 3; i<=8; i++)
 {
ASSERT(TPUD_p[i][7]  == 0x14); /* extended IE */
ASSERT(TPUD_p[i][8]  == 0x83); /* extended IE len */
 }

 ASSERT(TPUD_p[9][0]  == 75);   /* UDHL*/
 ASSERT(TPUD_p[9][7]  == 0x14); /* extended IE */
 ASSERT(TPUD_p[9][8]  == 0x43); /* extended IE len */

 ASSERT(TPUD_p[10][7] == 0xc); /* sound */
 ASSERT(TPUD_p[10][8] == 0x81); 
  }

  if(ret == EMS_OK)
  {
 InitializeEMSData(&emsData1, 0);
 /* set dcs & udhi */
 emsData1.dcs = emsData.dcs;
 emsData1.udhi = emsData.udhi;

 ret=EMSUnPack(&emsData1,emsData.udhi, numOfMsg, TPUD_p, TPUDLen);
 sprintf(buf, "UnPack: ret=%d\n", ret);
 kal_print((kal_char*)buf);

 EMSDataDump(&emsData1);

 if(ret == EMS_OK)
 {
kal_uint8 isSame = EMSDataCmp(&emsData, &emsData1, 0);
sprintf(buf, "EMSDataCmp: ret=%d\n", isSame);
kal_print((kal_char*)buf);

/* If objects have the same offset, and compression is support, */
/* it may be out of sequence after Unpack.  */
if (j<2)
   ASSERT(isSame == 0);
 }

 ReleaseEMSData(&emsData1);
  }

  ReleaseEMSData(&emsData);
   }

   EMSResourceVerdict();
   EMSSetCompressMode(KAL_FALSE);

}

void EMSExt_test_06(void)
{   
   kal_uint16 i,j;
   kal_uint8 numOfMsg, dcs;   
   kal_int16 ret=0;
   kal_uint8 buf[64];
   
   EMSTestFillString(string, 170, NULL);

   for (j=0; j< 4 ;j++)
   {
  if (j==0 || j==2)
 dcs = SMSAL_DEFAULT_DCS ;
  else
 dcs = SMSAL_UCS2_DCS ;

  if (j > 1 )
 EMSSetCompressMode(KAL_TRUE);

  /* init at 7-bits dcs */
  InitializeEMSData(&emsData, 0);

  ret=AddString(&emsData, string, 10, NULL); 
  ASSERT(ret == EMS_OK);

  i = EMS_EXT_TEST_OBJ_PIC_1B_512 ;
  ret=AddObject(&emsData, EMSTestExtObjData[i].type, &(EMSTestExtObjData[i].objData), 0);
  ASSERT(ret == EMS_OK);

  ret=EMSSetDCS(&emsData, dcs);
  ASSERT(ret == EMS_OK);

  ret=AddString(&emsData, string, 160, NULL); 
  ASSERT(ret == EMS_OK);

  i = EMS_EXT_TEST_OBJ_ANM_6B_128 ;
  ret=AddObject(&emsData, EMSTestExtObjData[i].type, &(EMSTestExtObjData[i].objData), 0);
  ASSERT(ret == EMS_OK);

  /* Predefined ANN */
  AddObject(&emsData, EMS_TYPE_PREDEF_ANM, NULL, 0);

  ret=EMSPack(&emsData, 1,&numOfMsg, TPUD_p, TPUDLen);
  ASSERT(ret == EMS_OK);
  EMSDataDump(&emsData);

  if (j<2)
  {
 if (dcs == SMSAL_DEFAULT_DCS )
 {
ASSERT (numOfMsg == 11 );

/* UDHL= 1, concat_info= 6, */
ASSERT(TPUD_p[0][7]  == 0x14); /* extended IE */
ASSERT(TPUD_p[0][8]  == 0x7a); /* extended IE len */
ASSERT(TPUD_p[0][15] == 10); /*  position byte, offset = 10 chars */

for (i= 1; i<=5; i++)
{
   ASSERT(TPUD_p[i][7]  == 0x14); /* extended IE */
   ASSERT(TPUD_p[i][8]  == 0x83); /* extended IE len */
}

/* seg 6 is all text */

/* seg 7 has header of 2nd object */
ASSERT(TPUD_p[7][7]  == 0x14); /* extended IE */
ASSERT(TPUD_p[7][8]  == 0x7c); /* extended IE len */
ASSERT(TPUD_p[7][15] == 170); /*  position byte, offset = 170 chars */

ASSERT(TPUD_p[8][7]  == 0x14); /* extended IE */
ASSERT(TPUD_p[8][8]  == 0x83); /* extended IE len */
ASSERT(TPUD_p[9][7]  == 0x14); /* extended IE */
ASSERT(TPUD_p[9][8]  == 0x83); /* extended IE len */

ASSERT(TPUD_p[10][7] == 0x14); /* extended IE */
ASSERT(TPUD_p[10][8] == 0x9) ; /* extended IE len */
ASSERT(TPUD_p[10][18]== 0x0d); /* snd */
ASSERT(TPUD_p[10][20]== 0x00); /* position shall be 0. */
 }
 else
 {
ASSERT (numOfMsg == 12 );

/* UDHL= 1, concat_info= 6, */
ASSERT(TPUD_p[0][7]  == 0x14); /* extended IE */
ASSERT(TPUD_p[0][8]  == 0x6F); /* extended IE len */
ASSERT(TPUD_p[0][15] == 10); /*  position byte, offset = 10 chars */

for (i= 1; i<=5; i++)
{
   ASSERT(TPUD_p[i][7]  == 0x14); /* extended IE */
   ASSERT(TPUD_p[i][8]  == 0x83); /* extended IE len */
}

ASSERT(TPUD_p[6][7]  == 0x14); /* extended IE */
ASSERT(TPUD_p[6][8]  == 0x0b); /* extended IE len */

/* seg 7 is all text */


/* seg 8 has header of 2nd object */
ASSERT(TPUD_p[8][7]  == 0x14); /* extended IE */
ASSERT(TPUD_p[8][8]  == 0x3F); /* extended IE len */
ASSERT(TPUD_p[8][15] == 170); /*  position byte, offset = 170 chars */

ASSERT(TPUD_p[9][7]  == 0x14); /* extended IE */
ASSERT(TPUD_p[9][8]  == 0x83); /* extended IE len */
ASSERT(TPUD_p[10][7] == 0x14); /* extended IE */
ASSERT(TPUD_p[10][8] == 0x83) ; /* extended IE len */

ASSERT(TPUD_p[11][7] == 0x14); /* extended IE */
ASSERT(TPUD_p[11][8] == 0x46) ; /* extended IE len */
ASSERT(TPUD_p[11][79]== 0x0d); /* snd */
ASSERT(TPUD_p[11][81]== 0x00); /* position shall be 0. */
 }
  }

  if(ret == EMS_OK)
  {
 InitializeEMSData(&emsData1, 0);
 /* set dcs & udhi */
 emsData1.dcs = emsData.dcs;
 emsData1.udhi = emsData.udhi;

 ret=EMSUnPack(&emsData1,emsData.udhi, numOfMsg, TPUD_p, TPUDLen);
 sprintf(buf, "UnPack: ret=%d\n", ret);
 kal_print((kal_char*)buf);

 EMSDataDump(&emsData1);

 if(ret == EMS_OK)
 {
kal_uint8 isSame = EMSDataCmp(&emsData, &emsData1, 0);
sprintf(buf, "EMSDataCmp: ret=%d\n", isSame);
kal_print((kal_char*)buf);

/* If objects have the same offset, and compression is support, */
/* it may be out of sequence after Unpack.  */
if (j<2)
   ASSERT(isSame == 0);
 }

 ReleaseEMSData(&emsData1);
  }

  ReleaseEMSData(&emsData);
   }

   EMSResourceVerdict();
   EMSSetCompressMode(KAL_FALSE);
}

void EMSExt_test_07(void)
{   
   kal_uint16 i,j;
   kal_uint8 numOfMsg, dcs, dcs2;   
   kal_int16 ret=0;
   kal_uint8 buf[64];
   
   EMSTestFillString(string, 290, NULL);

   for (j=0; j< 4 ;j++)
   {

  if (j==0 || j==2)
  {
 dcs  = SMSAL_DEFAULT_DCS ;
 dcs2 = SMSAL_UCS2_DCS ;
  }
  else
  {
 dcs  = SMSAL_UCS2_DCS ;
 dcs2 = SMSAL_DEFAULT_DCS ;
  }

  if (j > 1 )
 EMSSetCompressMode(KAL_TRUE);

  InitializeEMSData(&emsData, dcs);

  ret=AddString(&emsData, string, 20, NULL); 
  ASSERT(ret == EMS_OK);

  i = EMS_OBJ_PIC_SMALL ;
  ret=AddObject(&emsData, EMSTestObjData[i].type, &(EMSTestObjData[i].objData), 0);
  ASSERT(ret == EMS_OK);

  ret=EMSSetDCS(&emsData, dcs2);
  ASSERT(ret == EMS_OK);

  ret=AddString(&emsData, string, 50, NULL); 
  ASSERT(ret == EMS_OK);

  i = EMS_EXT_TEST_OBJ_PIC_2B_128;
  ret=AddObject(&emsData, EMSTestExtObjData[i].type, &(EMSTestExtObjData[i].objData), 0);
  ASSERT(ret == EMS_OK);

  /* Predefined ANN */
  AddObject(&emsData, EMS_TYPE_PREDEF_ANM, NULL, 0);

  ret=EMSSetDCS(&emsData, dcs);
  ASSERT(ret == EMS_OK);

  ret=AddString(&emsData, string, 190, NULL); 
  ASSERT(ret == EMS_OK);

  i = EMS_EXT_TEST_OBJ_PIC_6B_128;
  ret=AddObject(&emsData, EMSTestExtObjData[i].type, &(EMSTestExtObjData[i].objData), 0);
  ASSERT(ret == EMS_OK);

  ret=EMSSetDCS(&emsData, dcs2);
  ASSERT(ret == EMS_OK);

  ret=AddString(&emsData, string, 30, NULL); 
  ASSERT(ret == EMS_OK);

  AddObject(&emsData, EMS_TYPE_PREDEF_SND, NULL, 0);

  BackwardCurrentPosition(&emsData, 31);

  i = EMS_OBJ_ANM_LARGE ;
  ret=AddObject(&emsData, EMSTestObjData[i].type, &(EMSTestObjData[i].objData), 0);
  ASSERT(ret == EMS_OK);
  ForwardCurrentPosition(&emsData, 20);

  ret=EMSSetDCS(&emsData, dcs);
  ASSERT(ret == EMS_OK);

  i = EMS_EXT_TEST_OBJ_ANM_2B_512;
  ret=AddObject(&emsData, EMSTestExtObjData[i].type, &(EMSTestExtObjData[i].objData), 0);
  ASSERT(ret == EMS_OK);

  ret=EMSPack(&emsData, 1,&numOfMsg, TPUD_p, TPUDLen);
  ASSERT(ret == EMS_OK);
  EMSDataDump(&emsData);

  if (j <2)
  {
 if (dcs == SMSAL_DEFAULT_DCS )
 {
ASSERT (numOfMsg == 15 );

/* UDHL= 1, concat_info= 6, */
ASSERT(TPUD_p[0][7]  == 0x11); 
ASSERT(TPUD_p[0][8]  == 0x21);
ASSERT(TPUD_p[0][9]  == 20); /*  position byte, offset = 20 chars */
ASSERT(TPUD_p[0][42] == 0x14); /* extended IE */
ASSERT(TPUD_p[0][43] == 0x22); /* extended IE len */

for (i= 1; i<=2; i++)
{
   ASSERT(TPUD_p[i][7]  == 0x14); /* extended IE */
   ASSERT(TPUD_p[i][8]  == 0x83); /* extended IE len */
}

ASSERT(TPUD_p[3][7]  == 0x14); /* extended IE */
ASSERT(TPUD_p[3][8]  == 0x61); /* extended IE len */
ASSERT(TPUD_p[3][106]== 0x0d); 
ASSERT(TPUD_p[3][107]== 0x02); 
ASSERT(TPUD_p[3][108]== 0x00); 

ASSERT(TPUD_p[5][7]  == 0x14); /* extended IE */
ASSERT(TPUD_p[5][8]  == 0x7f); /* extended IE len */

ASSERT(TPUD_p[6][7]  == 0x14); /* extended IE */
ASSERT(TPUD_p[6][8]  == 0x4a); /* extended IE len */

ASSERT(TPUD_p[7][7]  == 0x0e);
ASSERT(TPUD_p[7][8]  == 0x81);
ASSERT(TPUD_p[7][9]  == 0); /*  position byte, offset = 170 chars */

ASSERT(TPUD_p[8][7]  == 0x14); /* extended IE */
ASSERT(TPUD_p[8][8]  == 0x73); /* extended IE len */

for (i= 9; i<=13; i++)
{
   ASSERT(TPUD_p[i][7]  == 0x14); /* extended IE */
   ASSERT(TPUD_p[i][8]  == 0x83); /* extended IE len */
}

ASSERT(TPUD_p[14][7] == 0x14); /* extended IE */
ASSERT(TPUD_p[14][8] == 0x9) ; /* extended IE len */
ASSERT(TPUD_p[14][18]== 0x0b); /* snm */
ASSERT(TPUD_p[14][20]== 0x0a); /* position shall be a. */
 }
 else
 {
ASSERT (numOfMsg == 17 );

/* UDHL= 1, concat_info= 6, */
ASSERT(TPUD_p[0][7]  == 0x11); 
ASSERT(TPUD_p[0][8]  == 0x21);
ASSERT(TPUD_p[0][9]  == 20); /*  position byte, offset = 20 chars */

ASSERT(TPUD_p[1][7]  == 0x14); /* extended IE */
ASSERT(TPUD_p[1][8]  == 0x59); /* extended IE len */


for (i= 2; i<=3; i++)
{
   ASSERT(TPUD_p[i][7]  == 0x14); /* extended IE */
   ASSERT(TPUD_p[i][8]  == 0x83); /* extended IE len */
}

ASSERT(TPUD_p[4][7]  == 0x14); /* extended IE */
ASSERT(TPUD_p[4][8]  == 0x2A); /* extended IE len */
ASSERT(TPUD_p[4][51] == 0x0d); 
ASSERT(TPUD_p[4][52] == 0x02); 
ASSERT(TPUD_p[4][53] == 0x00); 

ASSERT(TPUD_p[7][7]  == 0x14); /* extended IE */
ASSERT(TPUD_p[7][8]  == 0x63); /* extended IE len */

ASSERT(TPUD_p[8][7]  == 0x14); /* extended IE */
ASSERT(TPUD_p[8][8]  == 0x66); /* extended IE len */

ASSERT(TPUD_p[9][7]  == 0x0e);
ASSERT(TPUD_p[9][8]  == 0x81);
ASSERT(TPUD_p[9][9]  == 0); /*  position byte, offset = 170 chars */

ASSERT(TPUD_p[10][7]  == 0x14); /* extended IE */
ASSERT(TPUD_p[10][8]  == 0x5D); /* extended IE len */

for (i= 11; i<=15; i++)
{
   ASSERT(TPUD_p[i][7]  == 0x14); /* extended IE */
   ASSERT(TPUD_p[i][8]  == 0x83); /* extended IE len */
}

ASSERT(TPUD_p[16][7] == 0x14); /* extended IE */
ASSERT(TPUD_p[16][8] == 0x1F) ; /* extended IE len */
ASSERT(TPUD_p[16][40]== 0x0b); /* snm */
ASSERT(TPUD_p[16][41]== 0x02); /* position shall be a. */
 }
  }

  if(ret == EMS_OK)
  {
 InitializeEMSData(&emsData1, 0);
 /* set dcs & udhi */
 emsData1.dcs = emsData.dcs;
 emsData1.udhi = emsData.udhi;

 ret=EMSUnPack(&emsData1,emsData.udhi, numOfMsg, TPUD_p, TPUDLen);
 sprintf(buf, "UnPack: ret=%d\n", ret);
 kal_print((kal_char*)buf);

 EMSDataDump(&emsData1);

 if(ret == EMS_OK)
 {
kal_uint8 isSame = EMSDataCmp(&emsData, &emsData1, 0);
sprintf(buf, "EMSDataCmp: ret=%d\n", isSame);
kal_print((kal_char*)buf);

/* If objects have the same offset, and compression is support, */
/* it may be out of sequence after Unpack.  */
if (j<2)
   ASSERT(isSame == 0);
 }

 ReleaseEMSData(&emsData1);
  }

  ReleaseEMSData(&emsData);
   }

   EMSResourceVerdict();
   EMSSetCompressMode(KAL_FALSE);

}

void EMSExt_test_08(void)
{   
   kal_uint16 i,j;
   kal_uint8 numOfMsg, dcs, dcs2;   
   kal_int16 ret=0;
   kal_uint8 buf[64];
   EMSTextFormat tf ;
   
   EMSTestFillString(string, 250, NULL);

   for (j=0; j< 4 ;j++)
   {

  if (j==0 || j==2)
  {
 dcs  = SMSAL_DEFAULT_DCS ;
 dcs2 = SMSAL_UCS2_DCS ;
  }
  else
  {
 dcs  = SMSAL_UCS2_DCS ;
 dcs2 = SMSAL_DEFAULT_DCS ;
  }

  if (j > 1 )
 EMSSetCompressMode(KAL_TRUE);

  InitializeEMSData(&emsData, dcs);

  ret=AddString(&emsData, string, 10, NULL); 
  ASSERT(ret == EMS_OK);

  i = EMS_EXT_TEST_OBJ_PIC_6B_128;
  ret=AddObject(&emsData, EMSTestExtObjData[i].type, &(EMSTestExtObjData[i].objData), 0);
  ASSERT(ret == EMS_OK);

  ret=EMSSetDCS(&emsData, dcs2);
  ASSERT(ret == EMS_OK);

  ResetTextFormat(&tf);
  tf.isItalic = KAL_TRUE;
  ret=AddString(&emsData, string, 160, &tf); 
  ASSERT(ret == EMS_OK);

  BackwardCurrentPosition(&emsData, 30);

  /* Predefined SND */
  AddObject(&emsData, EMS_TYPE_PREDEF_SND, NULL, 0);

  i = EMS_EXT_TEST_OBJ_ANM_6B_128;
  ret=AddObject(&emsData, EMSTestExtObjData[i].type, &(EMSTestExtObjData[i].objData), 0);
  ASSERT(ret == EMS_OK);

  ForwardCurrentPosition(&emsData, 30);

  i = EMS_OBJ_ANM_SMALL ;
  ret=AddObject(&emsData, EMSTestObjData[i].type, &(EMSTestObjData[i].objData), 0);
  ASSERT(ret == EMS_OK);

  ret=EMSSetDCS(&emsData, dcs);
  ASSERT(ret == EMS_OK);

  ResetTextFormat(&tf);
  tf.isBold = KAL_TRUE;
  ret=AddString(&emsData, string, 180, &tf); 
  ASSERT(ret == EMS_OK);

  ret=EMSPack(&emsData, 1,&numOfMsg, TPUD_p, TPUDLen);
  ASSERT(ret == EMS_OK);
  EMSDataDump(&emsData);

  if (j<2)
  {
 if (dcs == SMSAL_DEFAULT_DCS )
 {
ASSERT (numOfMsg == 8 );

/* UDHL= 1, concat_info= 6, */
ASSERT(TPUD_p[0][7]  == 0x14); /* extended IE */
ASSERT(TPUD_p[0][8]  == 0x7a); /* extended IE len */

ASSERT(TPUD_p[1][7]  == 0x14); /* extended IE */
ASSERT(TPUD_p[1][8]  == 0x4f); /* extended IE len */
ASSERT(TPUD_p[1][88] == 0x0a); /* txt fmt */
ASSERT(TPUD_p[1][90] == 0x00); /* offset */
ASSERT(TPUD_p[1][91] == 0x35); /* len */


ASSERT(TPUD_p[2][7]  == 0x0a); /* txt fmt */
ASSERT(TPUD_p[2][9]  == 0x00); /* offset */
ASSERT(TPUD_p[2][10] == 0x4d); /* len */

ASSERT(TPUD_p[2][16] == 0x14); /* extended IE */
ASSERT(TPUD_p[2][17] == 0x36); 

for (i= 3; i<=4; i++)
{
   ASSERT(TPUD_p[i][7]  == 0x14); /* extended IE */
   ASSERT(TPUD_p[i][8]  == 0x83); /* extended IE len */
}

ASSERT(TPUD_p[5][7]  == 0x14); /* extended IE */
ASSERT(TPUD_p[5][8]  == 0x4f); /* extended IE len */
ASSERT(TPUD_p[5][88] == 0x0a); /* txt fmt */
ASSERT(TPUD_p[5][90] == 0x00); /* offset */
ASSERT(TPUD_p[5][91] == 0x1e); /* len */

ASSERT(TPUD_p[6][7]  == 0x0f);
ASSERT(TPUD_p[6][8]  == 0x21);
ASSERT(TPUD_p[6][42] == 0x0a); /* txt fmt */
ASSERT(TPUD_p[6][44] == 0x00); /* offset */

 }
 else
 {
ASSERT (numOfMsg == 11 );

/* UDHL= 1, concat_info= 6, */
ASSERT(TPUD_p[0][7]  == 0x14); /* extended IE */
ASSERT(TPUD_p[0][8]  == 0x6f); /* extended IE len */

ASSERT(TPUD_p[1][7]  == 0x14); /* extended IE */
ASSERT(TPUD_p[1][8]  == 0x5a); /* extended IE len */
ASSERT(TPUD_p[1][99] == 0x0a); /* txt fmt */
ASSERT(TPUD_p[1][101]== 0x00); /* offset */
ASSERT(TPUD_p[1][102]== 0x12); /* len */


ASSERT(TPUD_p[2][7]  == 0x0a); /* txt fmt */
ASSERT(TPUD_p[2][9]  == 0x00); /* offset */
ASSERT(TPUD_p[2][10] == 0x40); /* len */

ASSERT(TPUD_p[3][7]  == 0x0a); /* txt fmt */
ASSERT(TPUD_p[3][9]  == 0x00); /* offset */
ASSERT(TPUD_p[3][10] == 0x30); /* len */
ASSERT(TPUD_p[3][12] == 0x0b); 

ASSERT(TPUD_p[3][16]  == 0x14); /* extended IE */
ASSERT(TPUD_p[3][17]  == 0x1a); /* extended IE len */

for (i= 4; i<=5; i++)
{
   ASSERT(TPUD_p[i][7]  == 0x14); /* extended IE */
   ASSERT(TPUD_p[i][8]  == 0x83); /* extended IE len */
}

ASSERT(TPUD_p[6][7]  == 0x14); /* extended IE */
ASSERT(TPUD_p[6][8]  == 0x6b); /* extended IE len */
ASSERT(TPUD_p[6][116]== 0x0a); /* txt fmt */
ASSERT(TPUD_p[6][118]== 0x00); /* offset */
ASSERT(TPUD_p[6][119]== 0x09); /* len */

ASSERT(TPUD_p[7][7]  == 0x0a); /* txt fmt */
ASSERT(TPUD_p[7][9]  == 0x00); /* offset */
ASSERT(TPUD_p[7][10] == 0x15); /* len */
ASSERT(TPUD_p[7][12] == 0x0f);
ASSERT(TPUD_p[7][13] == 0x21);
ASSERT(TPUD_p[7][47] == 0x0a); /* txt fmt */
ASSERT(TPUD_p[7][49] == 0x15); /* offset */

 }
  }

  if(ret == EMS_OK)
  {
 InitializeEMSData(&emsData1, 0);
 /* set dcs & udhi */
 emsData1.dcs = emsData.dcs;
 emsData1.udhi = emsData.udhi;

 ret=EMSUnPack(&emsData1,emsData.udhi, numOfMsg, TPUD_p, TPUDLen);
 sprintf(buf, "UnPack: ret=%d\n", ret);
 kal_print((kal_char*)buf);

 EMSDataDump(&emsData1);

 if(ret == EMS_OK)
 {
kal_uint8 isSame = EMSDataCmp(&emsData, &emsData1, 0);
sprintf(buf, "EMSDataCmp: ret=%d\n", isSame);
kal_print((kal_char*)buf);

/* If objects have the same offset, and compression is support, */
/* it may be out of sequence after Unpack.  */
if (j<2)
   ASSERT(isSame == 0);
 }

 ReleaseEMSData(&emsData1);
  }

  ReleaseEMSData(&emsData);
   }

   EMSResourceVerdict();
   EMSSetCompressMode(KAL_FALSE);

}

void EMSExt_test_09(void)
{   
   kal_uint16 i;
   kal_uint8 numOfMsg;   
   kal_int16 ret=0;
   kal_uint8 buf[64];
   
   EMSTestFillString(string, 250, NULL);

   InitializeEMSData(&emsData, 0);

   ret=AddString(&emsData, string, 10, NULL); 
   ASSERT(ret == EMS_OK);

   i = EMS_EXT_TEST_OBJ_PIC_6B_128;
   ret=AddObject(&emsData, EMSTestExtObjData[i].type, &(EMSTestExtObjData[i].objData), 0);
   ASSERT(ret == EMS_OK);

   ret=AddString(&emsData, string, 10, NULL); 
   ASSERT(ret == EMS_OK);

   i = EMS_EXT_TEST_OBJ_PIC_6B_128;
   ret=AddObject(&emsData, EMSTestExtObjData[i].type, &(EMSTestExtObjData[i].objData), 0);
   ASSERT(ret == EMS_OK);

   ASSERT(EXT_OBJ_INFO_P(emsData.CurrentPosition.Object, isReusedObj)== KAL_TRUE);
   ASSERT(EXT_OBJ_INFO_P(emsData.CurrentPosition.Object, isExtObj)== KAL_FALSE);
   ASSERT(EXT_OBJ_INFO_P(emsData.CurrentPosition.Object->prev, isExtObj)== KAL_TRUE);

   BackwardCurrentPosition(&emsData, 6);

   i = EMS_EXT_TEST_OBJ_PIC_6B_128;
   ret=AddObject(&emsData, EMSTestExtObjData[i].type, &(EMSTestExtObjData[i].objData), 0);
   ASSERT(ret == EMS_OK);
   ASSERT(EXT_OBJ_INFO_P(emsData.CurrentPosition.Object, isReusedObj)== KAL_TRUE);
   ASSERT(EXT_OBJ_INFO_P(emsData.CurrentPosition.Object, isExtObj)== KAL_FALSE);
   ASSERT(EXT_OBJ_INFO_P(emsData.CurrentPosition.Object->prev, isExtObj)== KAL_TRUE);
   ASSERT(EXT_OBJ_INFO_P(emsData.CurrentPosition.Object->next, isReusedObj)== KAL_TRUE);

   BackwardCurrentPosition(&emsData, 12);
   
   i = EMS_EXT_TEST_OBJ_PIC_6B_128;
   ret=AddObject(&emsData, EMSTestExtObjData[i].type, &(EMSTestExtObjData[i].objData), 0);
   ASSERT(ret == EMS_OK);

   ASSERT(EXT_OBJ_INFO_P(emsData.CurrentPosition.Object, isExtObj)== KAL_TRUE);
   ASSERT(EXT_OBJ_INFO_P(emsData.CurrentPosition.Object, isReusedObj)== KAL_FALSE);
   ASSERT(EXT_OBJ_INFO_P(emsData.CurrentPosition.Object->next, isReusedObj)== KAL_TRUE);
   ASSERT(EXT_OBJ_INFO_P(emsData.CurrentPosition.Object->next->next, isReusedObj)== KAL_TRUE);
   ASSERT(EXT_OBJ_INFO_P(emsData.CurrentPosition.Object->next->next->next, isReusedObj)== KAL_TRUE);

   ret=EMSPack(&emsData,1, &numOfMsg, TPUD_p, TPUDLen);
   ASSERT(ret == EMS_OK);
   EMSDataDump(&emsData);

   if(ret == EMS_OK)
   {
  InitializeEMSData(&emsData1, 0);
  /* set dcs & udhi */
  emsData1.dcs = emsData.dcs;
  emsData1.udhi = emsData.udhi;

  ret=EMSUnPack(&emsData1,emsData.udhi, numOfMsg, TPUD_p, TPUDLen);
  sprintf(buf, "UnPack: ret=%d\n", ret);
  kal_print((kal_char*)buf);

  EMSDataDump(&emsData1);

  if(ret == EMS_OK)
  {
 kal_uint8 isSame = EMSDataCmp(&emsData, &emsData1, 0);
 sprintf(buf, "EMSDataCmp: ret=%d\n", isSame);
 kal_print((kal_char*)buf);

 ASSERT(isSame == 0);
  }

  ReleaseEMSData(&emsData1);
   }

   ReleaseEMSData(&emsData);

   EMSResourceVerdict();

}

void EMSExt_test_10(void)
{
   kal_uint16 i;
   kal_uint8 numOfMsg;   
   kal_int16 ret=0;
   kal_uint8 buf[64];
   EMSObjDataobjData;

   InitializeEMSData(&emsData, 0);

   /* not forward object */
   i = EMS_OBJ_PIC_SMALL;
   objData = EMSTestObjData[i].objData ;
   objData.common.attribute = EMS_ATTB_NOT_FW;
   ret=AddObject(&emsData, EMSTestObjData[i].type, &objData, 0);

   /* not forward object */
   i = EMS_OBJ_PIC_LARGE;
   objData = EMSTestObjData[i].objData ;
   objData.common.attribute = EMS_ATTB_NOT_FW;
   ret=AddObject(&emsData, EMSTestObjData[i].type, &objData, 0);

   /* normal object */
   i = EMS_OBJ_ANM_SMALL;
   objData = EMSTestObjData[i].objData ;
   ret=AddObject(&emsData, EMSTestObjData[i].type, &objData, 0);


   /* not forward object */
   i = EMS_OBJ_SND;
   objData = EMSTestObjData[i].objData ;
   objData.common.attribute = EMS_ATTB_NOT_FW;
   ret=AddObject(&emsData, EMSTestObjData[i].type, &objData, 0);

   ret=EMSPack(&emsData, 1,&numOfMsg, TPUD_p, TPUDLen);
   ASSERT(ret == EMS_OK);
   EMSDataDump(&emsData);

   if(ret == EMS_OK)
   {
  InitializeEMSData(&emsData1, 0);
  /* set dcs & udhi */
  emsData1.dcs = emsData.dcs;
  emsData1.udhi = emsData.udhi;

  ret=EMSUnPack(&emsData1,emsData.udhi, numOfMsg, TPUD_p, TPUDLen);
  sprintf(buf, "UnPack: ret=%d\n", ret);
  kal_print((kal_char*)buf);

  EMSDataDump(&emsData1);

  if(ret == EMS_OK)
  {
 kal_uint8 isSame = EMSDataCmp(&emsData, &emsData1, 0);
 sprintf(buf, "EMSDataCmp: ret=%d\n", isSame);
 kal_print((kal_char*)buf);

 ASSERT(isSame == 0);
  }

  ReleaseEMSData(&emsData1);
   }

   ReleaseEMSData(&emsData);

   EMSResourceVerdict();

}

void EMSExt_test_11(void)
{
   kal_uint16 i, j;
   kal_uint8 numOfMsg;   
   kal_int16 ret=0;
   kal_uint8 buf[64];
   EMSObjDataobjData;
   EMSTextFormat tf ;

   EMSTestFillString(string, 250, NULL);

   for (j=0; j< 4 ;j++)
   {

  InitializeEMSData(&emsData, 0);

  ResetTextFormat(&tf);
  tf.isBold = KAL_TRUE;
  ret=AddString(&emsData, string, 160, &tf); 
  ASSERT(ret == EMS_OK);

  if (j==0 || j==2)
 BackwardCurrentPosition(&emsData, 18);
  else
 BackwardCurrentPosition(&emsData, 15);

  /* not forward object */
  i = EMS_OBJ_PIC_SMALL;
  objData = EMSTestObjData[i].objData ;
  objData.common.attribute = EMS_ATTB_NOT_FW;
  ret=AddObject(&emsData, EMSTestObjData[i].type, &objData, 0);

  if (j > 1)
 ForwardCurrentPosition(&emsData, 2);

  /* not forward object */
  i = EMS_OBJ_PIC_LARGE;
  objData = EMSTestObjData[i].objData ;
  objData.common.attribute = EMS_ATTB_NOT_FW;
  ret=AddObject(&emsData, EMSTestObjData[i].type, &objData, 0);

  /* normal object */
  i = EMS_OBJ_ANM_SMALL;
  objData = EMSTestObjData[i].objData ;
  ret=AddObject(&emsData, EMSTestObjData[i].type, &objData, 0);


  /* not forward object */
  i = EMS_OBJ_SND;
  objData = EMSTestObjData[i].objData ;
  objData.common.attribute = EMS_ATTB_NOT_FW;
  ret=AddObject(&emsData, EMSTestObjData[i].type, &objData, 0);

  ret=EMSPack(&emsData,1, &numOfMsg, TPUD_p, TPUDLen);
  ASSERT(ret == EMS_OK);
  EMSDataDump(&emsData);

  if(ret == EMS_OK)
  {
 InitializeEMSData(&emsData1, 0);
 /* set dcs & udhi */
 emsData1.dcs = emsData.dcs;
 emsData1.udhi = emsData.udhi;

 ret=EMSUnPack(&emsData1,emsData.udhi, numOfMsg, TPUD_p, TPUDLen);
 sprintf(buf, "UnPack: ret=%d\n", ret);
 kal_print((kal_char*)buf);

 EMSDataDump(&emsData1);

 if(ret == EMS_OK)
 {
kal_uint8 isSame = EMSDataCmp(&emsData, &emsData1, 0);
sprintf(buf, "EMSDataCmp: ret=%d\n", isSame);
kal_print((kal_char*)buf);

ASSERT(isSame == 0);
 }

 ReleaseEMSData(&emsData1);
  }

  ReleaseEMSData(&emsData);
   }

   EMSResourceVerdict();

}


void EMSTestExtObjMO(void)
{

   /* case 1, 2, 3, 17 */
   /* case 5-8 */
   EMSExt_test_01();
   EMSExt_test_02();

   /* case 4 */
   EMSExt_test_03();

   /* case 9 */
   EMSExt_test_04();

   /* case 10 */
   EMSExt_test_05();

   /* case 11, 12, 13 */
   EMSExt_test_06();
   EMSExt_test_07();

   /* case 14 */
   EMSExt_test_08();

   /* case 15, 16 */
   EMSExt_test_09();

   /* case 17 */
   EMSExt_test_10();
   EMSExt_test_11();
}

void EMSTestExtObjMT(void)
{
   kal_uint8 numOfMsg, i, j;
   kal_uint8 buf[64];
   kal_bool  ret;
   EMSData   emsData;

   for (i=1; i <= 30; i++) 
   {
  if (i>= 10)
 sprintf(buf, "%s\\MT\\ext_%d.txt", EMS_TEST_DATA_PATH, i);
  else
 sprintf(buf, "%s\\MT\\ext_0%d.txt", EMS_TEST_DATA_PATH, i);

  ret = EMSReadRel5RawData (buf, &numOfMsg, TPUD_p, TPUDLen );
  ASSERT (ret == KAL_TRUE);

  InitializeEMSData(&emsData, 0);
  emsData.udhi=1;
  ret = EMSUnPack(&emsData,emsData.udhi, numOfMsg, TPUD_p, TPUDLen);

  sprintf(buf, "\n\n=========Extended object test : %d", i);
  kal_print((kal_char*)buf);
  EMSDataDump(&emsData);
  ReleaseEMSData(&emsData);

   }

}

void EMSTestExtObjMissSeg(void)
{
   kal_uint8  *miss_TPUD_p[20];   
   kal_uint8  numOfMsg; 
   kal_uint8  buf[64];
   kal_uint32 bit_map, count, i, j, k;
   kal_bool  ret;
   EMSData   emsData;
   kal_uint32 bit_map_a[5] = {0xdeffe, 0xbdfeb, 0xff7df, 0xfbf9f, 0xfffc0};

   sprintf(buf, "%s\\MT\\ext_miss_seg.txt", EMS_TEST_DATA_PATH);

   ret = EMSReadRel5RawData (buf, &numOfMsg, TPUD_p, TPUDLen );
   ASSERT (ret == KAL_TRUE);

   count = (1 << numOfMsg) ;
   /* count = 5; */

   for (i=0; i < count ; i++) 
   {

  /* init */
  for(j=0;j<20;j++) 
 miss_TPUD_p[j]=TPUD_p[j]; 

  bit_map = i ;
  /* bit_map = bit_map_a[i]; */

  for(j=0;(i>0 && j<numOfMsg);j++)
  /* for(j=0;(j<numOfMsg);j++)  */
  {
 if( (bit_map&0x01) == 0x00)
miss_TPUD_p[j]= NULL;

 bit_map >>= 1;
  }

  sprintf(buf, "\nExtended obj miss seg:[0x%x]\n", i);
  kal_print((kal_char*)buf);

  InitializeEMSData(&emsData, 0);
  emsData.udhi=1;
  ret = EMSUnPack(&emsData,emsData.udhi, numOfMsg, miss_TPUD_p, TPUDLen);

  EMSDataDump(&emsData);
  ReleaseEMSData(&emsData);
   }

}

#ifdef __EMS_REL5__
void EMSTestCompression(void)
{

#ifdef __WIN32__
   FILE *fp_raw, *fp_comp;
   kal_uint32 i,j;
   kal_uint8 buf[256];
   kal_uint8 raw_buf[6000];
   kal_uint8 comp_buf[6000];
   kal_uint32 raw_len, comp_len, out_len = 0, ret ;
   kal_uint8 *out_buff;


   for (j=0; j<8 ;j++)
   {
  kal_print("\n================================================\n");
  sprintf(buf, "%s\\compression\\r_00%d.bin",EMS_TEST_DATA_PATH,  j+1);
  kal_print((kal_char*)buf);
  kal_print("\n");
  fp_raw  = fopen(buf, "rb");

  ret = fread(&raw_len, sizeof(kal_uint32), 1, fp_raw);
  ret = fread(&raw_len, sizeof(kal_uint32), 1, fp_raw);
  ret = fread( (void *)raw_buf, 1, raw_len, fp_raw);

  fclose(fp_raw);

  sprintf(buf, "%s\\compression\\c_00%d.bin",EMS_TEST_DATA_PATH,  j+1);
  kal_print((kal_char*)buf);
  kal_print("\n");
  fp_comp = fopen(buf, "rb");

  ret = fread(&comp_len, sizeof(kal_uint32), 1, fp_comp);
  ret = fread(&comp_len, sizeof(kal_uint32), 1, fp_comp);
  ret = fread((void *)comp_buf, 1, comp_len, fp_comp);

  fclose(fp_comp);

  
  sprintf(buf, "\nraw_len= %d\n", raw_len);
  kal_print((kal_char*)buf);
  for (i=0; i<raw_len ;i++)
  {
 if ((i & 0xf) == 0xf)
sprintf(buf, "0x%x\n", raw_buf[i]);
 else
sprintf(buf, "0x%x ", raw_buf[i]);

 kal_print((kal_char*)buf);
  }



  sprintf(buf, "comp_len= %d\n", comp_len);
  kal_print((kal_char*)buf);
  for (i=0; i<comp_len ;i++)
  {
 if ((i & 0xf) == 0xf)
sprintf(buf, "0x%x\n", comp_buf[i]);
 else
sprintf(buf, "0x%x ", comp_buf[i]);

 kal_print((kal_char*)buf);
  }


  out_len = 0 ;
  if (KAL_FALSE ==  EMSDeCompression(KAL_FALSE, comp_len,  comp_buf, 
   &out_len, NULL))
  {
 ASSERT(KAL_FALSE);
 /* query the output length */
  }

  sprintf(buf, "\nde-compression out_len= %d\n", out_len);
  kal_print((kal_char*)buf);

  out_buff  = (kal_uint8*) malloc(out_len);

  if (KAL_TRUE ==  EMSDeCompression(KAL_TRUE, comp_len,  comp_buf, 
   &out_len, out_buff))
  {
 for (i=0; i<out_len ;i++)
 {
if ((i & 0xf) == 0xf)
   sprintf(buf, "0x%x\n", out_buff[i]);
else
   sprintf(buf, "0x%x ", out_buff[i]);

kal_print((kal_char*)buf);
 }
  }
  else
 ASSERT(KAL_FALSE);

  ASSERT (out_len == raw_len );
  ASSERT (0 == ems_mem_cmp(out_buff, raw_buf, out_len));

  if (out_buff != NULL)
 free(out_buff);


  out_len = 0 ;
  if (KAL_FALSE ==  EMSCompression(KAL_FALSE, raw_len,  raw_buf, 
   &out_len, NULL, NULL))
  {
 ASSERT(KAL_FALSE);
 /* query the output length */
  }

  sprintf(buf, "\nCompression out_len= %d\n", out_len);
  kal_print((kal_char*)buf);

  out_buff  = (kal_uint8*) malloc(out_len);

  if (KAL_TRUE ==  EMSCompression(KAL_TRUE, raw_len,  raw_buf, 
   &out_len, out_buff, comp_buf))
  {
 for (i=0; i<out_len ;i++)
 {
if ((i & 0xf) == 0xf)
   sprintf(buf, "0x%x\n", out_buff[i]);
else
   sprintf(buf, "0x%x ", out_buff[i]);

kal_print((kal_char*)buf);
 }
  }
  else
 ASSERT(KAL_FALSE);

  ASSERT (out_len == comp_len );
  ASSERT (0 == ems_mem_cmp(out_buff, comp_buf, out_len));

  if (out_buff != NULL)
 free(out_buff);

   }

   for (j=0; j< 1000 ;j++)
   {
  srand( (unsigned)time( NULL ) );

  raw_len = (kal_uint32 )(rand() + time(NULL)) & 0x00000fff ;
  for( i = 0;   i < raw_len ;i++ )
 raw_buf[i] =(kal_uint8) (rand() * time(NULL))& 0xff ;

  /* setp 1: Compression */
  comp_len = 0 ;
  if (KAL_FALSE ==  EMSCompression(KAL_FALSE, raw_len,  raw_buf, 
   &comp_len , NULL, NULL))
  {
 kal_print("\nassert !!");
 while(1);
 ASSERT(KAL_FALSE);
 /* query the output length */
  }

  sprintf(buf, "\nCompression comp_len= %d\n", comp_len);
  kal_print((kal_char*)buf);

  ASSERT (comp_len < 5000);

  if (KAL_TRUE ==  EMSCompression(KAL_TRUE, raw_len,  raw_buf, 
   &comp_len, comp_buf, NULL))
  {
 for (i=0; i< comp_len ;i++)
 {
if ((i & 0xf) == 0xf)
   sprintf(buf, "0x%x\n", comp_buf[i]);
else
   sprintf(buf, "0x%x ", comp_buf[i]);

kal_print((kal_char*)buf);
 }
  }
  else
 ASSERT(KAL_FALSE);

  /* setp 2: De-Compression */
  out_len = 0 ;
  if (KAL_FALSE ==  EMSDeCompression(KAL_FALSE, comp_len,  comp_buf, 
   &out_len, NULL))
  {
 kal_print("\nassert !!");
 while(1);
 ASSERT(KAL_FALSE);
 /* query the output length */
  }

  sprintf(buf, "\nde-compression out_len= %d\n", out_len);
  kal_print((kal_char*)buf);

  out_buff  = (kal_uint8*) malloc(out_len);

  if (KAL_TRUE ==  EMSDeCompression(KAL_TRUE, comp_len,  comp_buf, 
   &out_len, out_buff))
  {
 for (i=0; i<out_len ;i++)
 {
if ((i & 0xf) == 0xf)
   sprintf(buf, "0x%x\n", out_buff[i]);
else
   sprintf(buf, "0x%x ", out_buff[i]);

kal_print((kal_char*)buf);
 }
  }
  else
 ASSERT(KAL_FALSE);

  ASSERT (out_len == raw_len );
  ASSERT (0 == ems_mem_cmp(out_buff, raw_buf, out_len));

  if (out_buff != NULL)
 free(out_buff);
   }

#endif
}
#endif

#ifdef __WIN32__

/* count: testing times
 * force_ucs2: focus on ucs2
 * use_pluto_mmi: forward/backward/cancel/addstring one character 
 */
#define EMS_TEXT_MAX_ACT_CNT  200

/* mode : 0: use error seq to verify 
  1: testing */
void ems_robustness_test (kal_uint8  mode,
  kal_uint8 *filename,
  kal_uint16 count,
  kal_bool force_ucs2,
  kal_bool use_pluto_mmi,
  kal_bool need_compression
)
{
   kal_uint16 i,j,k;
   
   EMSTextFormat txt_fmt;
   EMSTATUS ret;
   kal_uint8 buf[100];
   kal_uint8 txt_mode;
   EMSObjData objdata;   
   kal_uint8 type;   
   kal_uint16 random[EMS_TEXT_MAX_ACT_CNT*3 ];
   kal_uint16 cnt=0;

   kal_uint16 obj_type;
   kal_uint16 PredefNo;
   kal_uint16 dcs;
   kal_uint16 action;
   kal_uint16 num_char, step, n;
   EMSData emsData1;
   kal_uint8 numOfMsg;
   kal_uint8 default_dcs = 0;
   kal_bool  force_ucs2_run, need_compression_run, use_pluto_mmi_run;

#ifdef __WIN32__
   FILE *fp, *fp_log;

   srand( (unsigned)time( NULL ) );   

   if(!mode)
   {
  for(i=0;i<EMS_TEXT_MAX_ACT_CNT*3 ;i++)
 random[i]=0xffff;

  fp = fopen(filename, "rb+");
  if (fp == NULL)
 return;

  for(i=0;i<EMS_TEXT_MAX_ACT_CNT*3 ;i++)
  {
 if (1 != fread(&random[i], sizeof(kal_uint16), 1, fp))
break;
  }
  fclose(fp);
   }

#endif
   force_ucs2_run = force_ucs2;
   need_compression_run = need_compression;
   use_pluto_mmi_run = use_pluto_mmi;


   if(force_ucs2_run )
  default_dcs = 8;/*ucs2*/

   for(i=0;i<count;i++)
   {
  if(mode==0 && i==1)
 break;

  fp_log = fopen("c:\\ems_edit_seq_log.txt", "w+");

  sprintf(buf, "round: %d\n", i);
  kal_print((kal_char*)buf);
  fwrite( buf, sizeof( char ), strlen(buf), fp_log );

  cnt=0;

  if(mode)
  {
 fp = fopen("c:\\ems_edit_seq.txt", "wb+");


 random[cnt++]=(kal_uint16)force_ucs2;
 fwrite(&random[cnt-1], sizeof(kal_uint16), 1, fp);
 fflush(fp);

 random[cnt++]=(kal_uint16)use_pluto_mmi;
 fwrite(&random[cnt-1], sizeof(kal_uint16), 1, fp);
 fflush(fp);

 random[cnt++]=(kal_uint16)need_compression;
 fwrite(&random[cnt-1], sizeof(kal_uint16), 1, fp);
 fflush(fp);
  }
  else
  {
 force_ucs2_run   = (kal_bool)random[cnt++];
 use_pluto_mmi_run= (kal_bool)random[cnt++];
 need_compression_run = (kal_bool)random[cnt++];

 if(force_ucs2_run )
default_dcs = 8;/*ucs2*/
  }

#ifdef __EMS_REL5__
  EMSSetCompressMode(need_compression_run );
#endif

  InitializeEMSData(&emsData, default_dcs);
  InitializeEMSData(&emsData1, default_dcs);  
  
  for(j=0;j<EMS_TEXT_MAX_ACT_CNT  ;j++)
  {
 //choose action
 if(mode)
 {
//srand( (unsigned)time( NULL ) );
/* action = rand() % EMS_TEXT_MAX_ACTION; */
#ifdef __EMS_REL5__
action = (rand())%29;
#else
action = (rand())%22;
#endif
//srand( (unsigned)(time( NULL ) + rand()) ); 
random[cnt++]=action;
fwrite(&random[cnt-1], sizeof(kal_uint16), 1, fp);
fflush(fp);
 }
 else
 action = random[cnt++];


 sprintf(buf, "\n====action count=%d\n", j);
 kal_print((kal_char*)buf);
 fwrite( buf, sizeof( char ), strlen(buf), fp_log );
 switch(action)
 {
case 0: //addstring
case 10: 
case 11: 
case 12: 
case 13: 
   {
  if(use_pluto_mmi_run )
 num_char = 1;
  else
  {
 if(mode)
 {
num_char = rand() % MAX_TEXT_LEN_SEPTET ;
if(num_char==0) num_char=1;
random[cnt++]=num_char;
fwrite(&random[cnt-1], sizeof(kal_uint16), 1, fp);
fflush(fp);
//printf("[%d]\n", random[cnt-1]);
 }
 else
num_char = random[cnt++];
  }

  ResetTextFormat(&txt_fmt);

  if(mode)
  {
 txt_mode = rand()%4;
 random[cnt++]=txt_mode;
 fwrite(&random[cnt-1], sizeof(kal_uint16), 1, fp);
 fflush(fp);
 //printf("[%d]\n", random[cnt-1]);
  }
  else
 txt_mode = random[cnt++];

  switch(txt_mode)
  {
 case 0: txt_fmt.isBold=1;  break;
 case 1: txt_fmt.isItalic=1;break;
 case 2: txt_fmt.isStrikethrough=1; break;
 default:break;
  }/*switch*/

  sprintf(buf, "AddString: num_char=%d txt_fmt=%d ", num_char, txt_mode);
  kal_print((kal_char*)buf);
  fwrite( buf, sizeof( char ), strlen(buf), fp_log );
  
  EMSTestFillString(string, num_char, NULL);

  ret=AddString(&emsData, string, num_char, &txt_fmt);
  
  sprintf(buf, "ret=%d\n", ret);
  kal_print((kal_char*)buf);
  fwrite( buf, sizeof( char ), strlen(buf), fp_log );
   
   }
   break;

case 1: //addobject
case 14: 
case 15: 
   {
  if(mode)
  {
 obj_type = rand()%8;
 random[cnt++]=obj_type;
 fwrite(&random[cnt-1], sizeof(kal_uint16), 1, fp);
 fflush(fp);
 //printf("[%d]\n", random[cnt-1]);
  }
  else
 obj_type = random[cnt++];

  switch(obj_type)
  {
 case 0: //predef anm
{
   if(mode)
   {
  PredefNo = rand() % 15;
  random[cnt++]=PredefNo;
  fwrite(&random[cnt-1], sizeof(kal_uint16), 1, fp);
  fflush(fp);
  //printf("[%d]\n", random[cnt-1]);
   }
   else
  PredefNo = random[cnt++];

   sprintf(buf, "AddObject: predef anm=%d ", PredefNo);
   kal_print((kal_char*)buf);
   fwrite( buf, sizeof( char ), strlen(buf), fp_log );
   ret=AddObject(&emsData, EMS_TYPE_PREDEF_ANM, NULL, PredefNo);
   sprintf(buf, "ret=%d\n", ret);
   kal_print((kal_char*)buf);
   fwrite( buf, sizeof( char ), strlen(buf), fp_log );
}
break;

 case 1: //predef snd
{
   if(mode)
   {
  PredefNo = rand() % 10;
  random[cnt++]=PredefNo;
  fwrite(&random[cnt-1], sizeof(kal_uint16), 1, fp);
  fflush(fp);
  //printf("[%d]\n", random[cnt-1]);
   }
   else
  PredefNo = random[cnt++];

   sprintf(buf, "AddObject: predef snd=%d ", PredefNo);
   kal_print((kal_char*)buf);
   fwrite( buf, sizeof( char ), strlen(buf), fp_log );

   ret=AddObject(&emsData, EMS_TYPE_PREDEF_SND, NULL, PredefNo);
   sprintf(buf, " ret=%d\n", ret);
   kal_print((kal_char*)buf);
   fwrite( buf, sizeof( char ), strlen(buf), fp_log );
}
break;

 case 2: //usr snd
{

   objdata = EMSTestObjData[EMS_OBJ_SND].objData ;
   type= EMSTestObjData[EMS_OBJ_SND].type;
   kal_print("AddObject: usrdef sound ");   
   ret=AddObject(&emsData, type , &objdata, 0);
   sprintf(buf, "ret=%d\n", ret);
   kal_print((kal_char*)buf);
   fwrite( buf, sizeof( char ), strlen(buf), fp_log );
}
break;

 case 3: //small anm
{
   objdata = EMSTestObjData[EMS_OBJ_ANM_SMALL].objData ;
   type= EMSTestObjData[EMS_OBJ_ANM_SMALL].type;
   kal_print("AddObject: small anm ");
   ret=AddObject(&emsData, type , &objdata, 0);
   sprintf(buf, " ret=%d\n", ret);
   kal_print((kal_char*)buf);
   fwrite( buf, sizeof( char ), strlen(buf), fp_log );
}
break;

 case 4: //large anm
{
   objdata = EMSTestObjData[EMS_OBJ_ANM_LARGE].objData ;
   type= EMSTestObjData[EMS_OBJ_ANM_LARGE].type;
   kal_print("AddObject: large anm ");

   ret=AddObject(&emsData, type , &objdata, 0);
   sprintf(buf, "AddObject: ret=%d\n", ret);
   kal_print((kal_char*)buf);
   fwrite( buf, sizeof( char ), strlen(buf), fp_log );
}
break;

 case 5: //small pic
{
   objdata = EMSTestObjData[EMS_OBJ_PIC_SMALL].objData ;
   type= EMSTestObjData[EMS_OBJ_PIC_SMALL].type;
   kal_print("AddObject: small pic ");

   ret=AddObject(&emsData, type , &objdata, 0);
   sprintf(buf, "AddObject: ret=%d\n", ret);
   kal_print((kal_char*)buf);
   fwrite( buf, sizeof( char ), strlen(buf), fp_log );
}
break;
 case 6: //large pic
 case 7: //var pic
{
   objdata = EMSTestObjData[EMS_OBJ_PIC_LARGE].objData ;
   type= EMSTestObjData[EMS_OBJ_PIC_LARGE].type;
   kal_print("AddObject: large pic ");
   ret=AddObject(&emsData, type , &objdata, 0);
   sprintf(buf, "AddObject: ret=%d\n", ret);
   kal_print((kal_char*)buf);
   fwrite( buf, sizeof( char ), strlen(buf), fp_log );
}
break;
 default: break;
  }
   }
   break;

case 2: //set dcs
   {
  if(force_ucs2_run ) break;/*skip*/

  if(mode)
  {
 dcs=rand()%3;
 random[cnt++]=dcs;
 fwrite(&random[cnt-1], sizeof(kal_uint16), 1, fp);
 fflush(fp);
 //rintf("[%d]\n", random[cnt-1]);
  }
  else
 dcs = random[cnt++];

  switch(dcs)
  {
 case 0: dcs=0; break;
 case 1: dcs=SMSAL_UCS2_DCS; break;
 case 2: dcs=SMSAL_8BIT_DCS; break;
  }
  sprintf(buf, "EMSSetDCS: dcs=%d ", dcs);
  kal_print((kal_char*)buf);
  fwrite( buf, sizeof( char ), strlen(buf), fp_log );

  ret=EMSSetDCS(&emsData, dcs);
  sprintf(buf, "ret=%d\n", ret);
  kal_print((kal_char*)buf);
  fwrite( buf, sizeof( char ), strlen(buf), fp_log );
   }
   break;

case 3: //backward
case 16: 
case 17: 
   if(use_pluto_mmi_run )
  step = 1;
   else
   {
  if(mode)
  {
 step = rand() % MAX_TEXT_LEN_SEPTET ;
 random[cnt++]=step;
 fwrite(&random[cnt-1], sizeof(kal_uint16), 1, fp);
 fflush(fp);
 //printf("[%d]\n", random[cnt-1]);
  }
  else 
 step = random[cnt++];
   }
   sprintf(buf, "BackwardCurrentPosition: step=%d ", step);
   kal_print((kal_char*)buf);
   fwrite( buf, sizeof( char ), strlen(buf), fp_log );

   n = BackwardCurrentPosition(&emsData, step);
   sprintf(buf, "ret=%d\n", n);
   kal_print((kal_char*)buf);
   fwrite( buf, sizeof( char ), strlen(buf), fp_log );
   break;

case 4: //cancel
case 18: 
case 19: 
   if(use_pluto_mmi_run )
  step = 1;
   else
   {
  if(mode)
  {
 step = rand() % MAX_TEXT_LEN_SEPTET ;
 random[cnt++]=step;
 fwrite(&random[cnt-1], sizeof(kal_uint16), 1, fp);
 fflush(fp);
 //printf("[%d]\n", random[cnt-1]);
  }
  else
 step = random[cnt++];
   }
   sprintf(buf, "CancelCurrentPosition: step=%d ", step);
   kal_print((kal_char*)buf);
   fwrite( buf, sizeof( char ), strlen(buf), fp_log );

   n = CancelCurrentPosition(&emsData, step);
   sprintf(buf, "ret=%d\n", n);
   kal_print((kal_char*)buf);
   fwrite( buf, sizeof( char ), strlen(buf), fp_log );
   break;

case 5: //forward
case 20: 
case 21: 
   if(use_pluto_mmi_run )
  step = 1;
   else
   {
  if(mode)
  {
 step = rand() % MAX_TEXT_LEN_SEPTET ;
 random[cnt++]=step;
 fwrite(&random[cnt-1], sizeof(kal_uint16), 1, fp);
 fflush(fp);
 //printf("[%d]\n", random[cnt-1]);
  }
  else
 step = random[cnt++];
   }

   sprintf(buf, "ForwardCurrentPosition: step=%d ", step);
   kal_print((kal_char*)buf);
   fwrite( buf, sizeof( char ), strlen(buf), fp_log );

   n = ForwardCurrentPosition(&emsData, step);
   sprintf(buf, "ret=%d\n", n);
   kal_print((kal_char*)buf);
   fwrite( buf, sizeof( char ), strlen(buf), fp_log );
   break;

case 6: //resetcurrent
   ResetCurrentPosition(&emsData);
   kal_print("ResetCurrentPosition\n");
   break;

case 7: //goto end
   GoToEndPosition(&emsData);
   kal_print("GoToEndPosition\n");
   break;

case 8: // set port
   ret=EMSSetPortNum(&emsData,1234,5678);
   sprintf(buf, "SetPortNum: ret=%d ", ret);
   kal_print((kal_char*)buf);
   fwrite( buf, sizeof( char ), strlen(buf), fp_log );
   break;

case 9: // reset port
   ret = EMSResetPortNum(&emsData);
   sprintf(buf, "ReSetPortNum: ret=%d ", ret);
   kal_print((kal_char*)buf);
   fwrite( buf, sizeof( char ), strlen(buf), fp_log );
   break;
#ifdef __EMS_REL5__
case 22: //add extended object
case 23: 
case 24: 
case 25: 
case 26: 
   {
  if(mode)
  {
 obj_type = rand()%NUM_EMS_EXT_TEST_OBJ_DATA;
 random[cnt++]=obj_type;
 fwrite(&random[cnt-1], sizeof(kal_uint16), 1, fp);
 fflush(fp);
  }
  else
 obj_type = random[cnt++];

  {
 objdata = EMSTestExtObjData[obj_type].objData ;
 type= EMSTestExtObjData[obj_type].type;
 sprintf(buf, "AddObject: extended object, id=[%d]",obj_type);   
 kal_print((kal_char*)buf);
 fwrite( buf, sizeof( char ), strlen(buf), fp_log );

 ret=AddObject(&emsData, type , &objdata, 0);
 sprintf(buf, "ret=%d\n", ret);
 kal_print((kal_char*)buf);
 fwrite( buf, sizeof( char ), strlen(buf), fp_log );
  }
   }
   break;
case 27: //add normal object with attribute
case 28: 
   {
  if(mode)
  {
 obj_type = rand()%NUM_EMS_TEST_OBJ_DATA;
 random[cnt++]=obj_type;
 fwrite(&random[cnt-1], sizeof(kal_uint16), 1, fp);
 fflush(fp);
  }
  else
 obj_type = random[cnt++];

  {
 objdata = EMSTestObjData[obj_type].objData ;
 type= EMSTestObjData[obj_type].type;
 objdata.common.attribute = EMS_ATTB_NOT_FW;

 sprintf(buf, "AddObject: normal object with attribute, id=[%d]",obj_type);   
 kal_print((kal_char*)buf);
 fwrite( buf, sizeof( char ), strlen(buf), fp_log );

 ret=AddObject(&emsData, type , &objdata, 0);
 sprintf(buf, "ret=%d\n", ret);
 kal_print((kal_char*)buf);
 fwrite( buf, sizeof( char ), strlen(buf), fp_log );
  }
   }
   break;
#endif
default: break;
 }/*switch*/

 EMSDataDump_log(&emsData, fp_log);

 if (action == 0xffff)
break;

 if ((cnt + 2) > (EMS_TEXT_MAX_ACT_CNT*3)) 
break;
  
 fflush(fp_log);
  }/*for j */   
  
  fclose(fp);

  //EMSDataDump(&emsData);

  ret=EMSPack(&emsData,1, &numOfMsg, TPUD_p, TPUDLen);
  sprintf(buf, "Pack: ret=%d numOfMsg=%d\n", ret, numOfMsg);
  kal_print((kal_char*)buf);
  fwrite( buf, sizeof( char ), strlen(buf), fp_log );

  if(ret==EMS_OK)
  {
 /* set dcs & udhi */
 emsData1.dcs = emsData.dcs;
 emsData1.udhi = emsData.udhi;

 ret=EMSUnPack(&emsData1,emsData.udhi, numOfMsg, TPUD_p, TPUDLen);
 sprintf(buf, "UnPack: ret=%d\n", ret);
 kal_print((kal_char*)buf);
 fwrite( buf, sizeof( char ), strlen(buf), fp_log );

 EMSDataDump_log(&emsData1, fp_log);

 if(ret == EMS_OK)
 {
kal_uint8 isSame = EMSDataCmp(&emsData, &emsData1, 0);
sprintf(buf, "EMSDataCmp: ret=%d\n", isSame);
kal_print((kal_char*)buf);
fwrite( buf, sizeof( char ), strlen(buf), fp_log );
#if 1
if(need_compression_run == KAL_FALSE)
{
   ASSERT(isSame == 0);
}
#endif
 }
  }

  ReleaseEMSData(&emsData);
  ReleaseEMSData(&emsData1);

  fclose(fp_log);

   }/*for i*/

} /* end of ems_robustness_test */

#endif//__WIN32__

#define NUM_EMS_TEST_SEQ_NUM 18
void ems_run_robust_seq(void)
{
   kal_uint8 buf[64];
   kal_uint16 i;

   for(i=1; i< NUM_EMS_TEST_SEQ_NUM ; i++)
   {

  if (i>= 100)
 sprintf(buf, "%s\\seq\\ems_edit_seq_%d.txt",EMS_TEST_DATA_PATH ,  i);
  else if (i>= 10)
 sprintf(buf, "%s\\seq\\ems_edit_seq_0%d.txt",EMS_TEST_DATA_PATH , i);
  else
 sprintf(buf, "%s\\seq\\ems_edit_seq_00%d.txt",EMS_TEST_DATA_PATH , i);


  ems_robustness_test(0, buf, 1, 0, 1, 0);
   }

   return ;

}
 
void EMSUT(void)
{
   kal_uint16 i;

   for (i=0; i < EMS_TEST_MAX_SEG; i++)
   {
  TPUD_p[i] = (kal_uint8*)malloc(160);
  TPUDLen[i]= 0;
   }

   string= (kal_uint8*)malloc(MAX_TEXT_LEN_SEPTET);

#ifdef __WIN32__
   printf("sizeof(EMSObject):\t%d\n", sizeof(EMSObject));
   printf("sizeof(EMSObjData):\t%d\n", sizeof(EMSObjData));   
   printf("sizeof(EMSTYPE):\t%d\n", sizeof(EMSTYPE));   
   printf("sizeof(EMSInternalData):\t%d\n", sizeof(EMSInternalData));
   printf("sizeof(EMSContext):\t%d\n", sizeof(EMSContext));
#endif

   EMSFillObjData();

#if 0

   /* 
* normal sms pack/unpack   
*/
   EMS_PRINT_STR("==========================================\n");
   EMS_PRINT_STR("normal sms pack/unpack  \n");
   EMS_PRINT_STR("==========================================\n");
   EMSTestNormalSMS(160, SMSAL_DEFAULT_DCS);
   EMSTestNormalSMS(70,  SMSAL_UCS2_DCS);
   EMSTestNormalSMS(140, SMSAL_8BIT_DCS);

   /* 
* basic text testing 
*/
   EMS_PRINT_STR("==========================================\n");
   EMS_PRINT_STR("basic text test:\n");
   EMS_PRINT_STR("==========================================\n");

   for(i=0;i<NUM_EMS_BASIC_TEXT_ITEM;i++)
   {  
  EMS_PRINT("basic text test no. %d\n", i);
  EMSTestBasicText(&EMSTestBasicTextData[i]);
   }

   /* 
* basic formatted text testing 
*/
   EMS_PRINT_STR("==========================================\n");
   EMS_PRINT_STR("basic formatted text testing\n");
   EMS_PRINT_STR("==========================================\n");
   EMSTestAllTextFormat(SMSAL_DEFAULT_DCS);
   EMSTestAllTextFormat(SMSAL_UCS2_DCS);

#if 0
   for(i=0;i<NUM_EMS_BASIC_FORMATTED_TEXT_ITEM;i++)
   {  
  EMS_PRINT("basic formatted text test no. %d\n", i);
  EMSTestBasicFormattedText(&EMSTestBasicFormattedTextData[i]);
   }
#endif
   
   /* 
* basic EMS objects, eg, picture.
*/
   EMS_PRINT_STR("==========================================\n");
   EMS_PRINT_STR("basic EMS objects, eg, picture\n");
   EMS_PRINT_STR("==========================================\n");
   EMSTestBasicObject();


   /*
* change dcs, eg, 8-bit dcs
*/
   EMS_PRINT_STR("==========================================\n");
   EMS_PRINT_STR("change dcs\n");
   EMS_PRINT_STR("==========================================\n");
   EMSTest8bitDCS1();
   EMSTest8bitDCS2();
   EMSTestUCS2DCS();  

   /*
* copy ems data
*/
   EMSTestCopyEMSData();

   /*
* little-endian or big-endian testing   
*/
   EMS_PRINT_STR("==========================================\n");
   EMS_PRINT_STR("little-endian or big-endian testing\n");
   EMS_PRINT_STR("==========================================\n");
   EMSTestEndian_7BIT();
   EMSTestEndian_UCS2();

   /* 
* unpack ems which missed some segments
*/
   EMSTestMissSegment();

   /* 
* some edit sequences
*/
   EMSTestEditSeq2();
   EMSTestEditSeq3();
   EMSTestEditSeq5();
   EMSTestEditSeq6();
   EMSTestEditSeq7();
   EMSTestEditSeq8();
   EMSTestEditSeq9();
   EMSTestEditSeq10();
   EMSTestEditSeq11();
   EMSTestEditSeq12();
   EMSTestEditSeq13();
   EMSTestEditSeq14();
   EMSTestEditSeq15();
   EMSTestEditSeq16();
   EMSTestEditSeq17();
   EMSTestEditSeq18();
   EMSTestEditSeq19();
   EMSTestEditSeq20();
   EMSTestEditSeq21();

   EMSTestMsgPreview01();


   EMSTestPortNum01();
   EMSTestPortNum02();
   EMSTestPortNum03();
   EMSTestPortNum04();
   EMSTestPortNum05();
   EMSTestPortNum06();
   EMSTestPortNum07();
   EMSTestPortNum08();

#if 0
   /*
* too many objects tests
*/
   EMSTooManyObjects1();
   EMSTooManyObjects2();
   EMSTooManyObjects3();
   EMSTooManyObjects4();
#endif

   /* 
* unpack tests
*/
   EMS_unpack_test1();

   /* 
* misc.
*/
   EMS_PRINT_STR("==========================================\n");
   EMS_PRINT_STR("misc.\n");
   EMS_PRINT_STR("==========================================\n");
   ems_test_1();
   ems_test_2();
   ems_test_3();
   ems_test_4();
   ems_test_5();
   ems_test_6();
   ems_test_7();
   ems_test_8();
   ems_test_9();
   ems_test_10();
   ems_test_11();
   ems_test_12();   
   ems_test_13();
   ems_test_14();
   ems_test_15();
   ems_test_16();
   ems_test_17();
   ems_test_18();
   ems_test_19();
   /* ems_test_20(); not OK after new text alignment */
   ems_test_21();   
   ems_test_22();   
   ems_test_23();
   ems_test_24();
   ems_test_25();
   ems_test_26();
   ems_test_27();
   ems_test_28();
   ems_test_29();
   ems_test_30();

   ems_test_31();

   /* EMSGetNextObject / EMSGetPrevisouObject test */
   ems_get_next_prev_obj_test();
  
   /*
* invalid data, need more !!!!!!!!!!!!!!!!!
*/
   EMSTestInvalidTextFormat1();
   EMSTestInvalidTextFormat2();
   EMSTestInvalidTextFormat3();//ucs2
#if 0
   EMSTestInvalidObjOffset1();
   EMSTestInvalidObjOffset2();//ucs2
#endif
   EMSTestInvalidUDHL1();
   EMSTestInvalidUDHL2();
   EMSTestInvalidUCS2len1();
   EMSTestInvalidUCS2len2();

#endif

   EMSTestNewTxtAlignmentSolution();

#ifdef __EMS_REL5__
   //EMSTestCompression();

   EMSReadRel5ObjData();
#if 0
   EMSTestExtObjMT();
   EMSTestExtObjMO();
#endif
   //EMSTestExtObjMissSeg();
#endif

#if 0
   ems_run_robust_seq();
#endif

#if 1
   /* robust test debug */
   ems_robustness_test(0, "c:\\ems_edit_seq.txt", 5, 0, 1, 0);
#endif


   /* robustness test */
   kal_print("ems_robustness_test\n");

   ems_robustness_test(1, NULL, 1000000, 0, 1, 0);/*one step, sim MMI */
   ems_robustness_test(1, NULL, 1000000, 1, 1, 0);
   ems_robustness_test(1, NULL, 1000000, 0, 0, 0);
   ems_robustness_test(1, NULL, 1000000, 1, 0, 0);/*force to use ucs2 */

#ifdef __EMS_REL5__
   ems_robustness_test(1, NULL, 1000000, 0, 1, 1);/*one step, sim MMI */
   ems_robustness_test(1, NULL, 1000000, 1, 1, 1);
   ems_robustness_test(1, NULL, 1000000, 0, 0, 1);
   ems_robustness_test(1, NULL, 1000000, 1, 0, 1);/*force to use ucs2 */
#endif
   EMS_PRINT_STR("all ems test cases passed\n");

   for (i=0; i < EMS_TEST_MAX_SEG; i++)
  free(TPUD_p[i]);


   free(string);
   while(1);

}


#endif /* EMS_UT */

 

