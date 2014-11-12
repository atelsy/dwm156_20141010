/*****************************************************************************
*  Copyright Statement:
*  --------------------
*  This software is protected by Copyright and the information contained
*  herein is confidential. The software may not be copied and the information
*  contained herein may not be used or disclosed except with the written
*  permission of MediaTek Inc. (C) 2011
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

#ifndef _ME_OSAL_H
#define _ME_OSAL_H

#include "me_glbdef.h"

#ifdef WIN32
#include "StdAfx.h"
#include <process.h>
#include <stdio.h>
#include "..\publib\debug.h"
#else
#include "Win2Mac.h"
#endif
 
#define ME_Print				DebugOutA
#define BOOLEAN					bool
#define NTSTATUS				int
#define PUCHAR					char*
#define ULONG_PTR				unsigned long
#define ULONG					unsigned long
#define STATUS_SUCCESS				0x00000001L
#define STATUS_FAILED				0x00000000L
#define STATUS_INVALID_PARAMETER    100 //ER_UNKNOWN
#define me_sprintf              sprintf

typedef NTSTATUS (*ME_TX_NOTIFY)(NTSTATUS status, int len, void *pData);



typedef struct tagOSALLOCK
{
	CRITICAL_SECTION	Lock;
}OSALLOCK;

typedef struct tagOSALTIMER
{
	void	(*timer_cb)(void*);
	void	*pData;

	HANDLE hThread;
	HANDLE hSetEvt;
	HANDLE hKillEvt;
	HANDLE hExitEvt;
	HANDLE hKilledEvt;
	UINT   duetime;

}OSALTIMER;

void *me_memset(void *dst, int val, int count);
void *me_malloc(unsigned int size);
void me_free(void *pData);
char *me_strcpy(char *dst, const char *src);
void *me_memcpy( void *dst, const void *src, unsigned int count);
unsigned int me_strlen(const char *str);
int me_memcmp(const void *buf1, const void *buf2, unsigned int count);
int me_atoi(const char *str);
char * me_strstr (const char * str1, const char * str2);
void me_initlock(OSALLOCK *pLock);
void me_lock(OSALLOCK *pLock);
void me_unlock(OSALLOCK *pLock);
void me_inittimer(OSALTIMER *timer);
void me_settimer(OSALTIMER *timer, int duetime);
void me_killtimer(OSALTIMER *timer, int event);
void me_exittimer(OSALTIMER *timer);
void me_respond_ready(void *pData);
NTSTATUS me_get_rettype(void *pData);
NTSTATUS me_send_at(void* ioproxy, const char *str, ME_TX_NOTIFY notify, void *pData);
NTSTATUS me_send_pdu(void* ioproxy, const char *str, ME_TX_NOTIFY notify, void *pData);

#endif //_ME_OSAL_H