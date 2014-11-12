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

#include "me_osal.h"
#include "me_comm.h"
#include "ATResult.h"

#ifndef WIN32
#include "SerialPort.h"
#else
#include "..\comm\comm.h"
#endif

void *me_memset(void *dst, int val, int count)
{
	void *start = dst;

	while (count--) {
		*(char *)dst = (char)val;
		dst = (char *)dst + 1;
	}
	return(start);
}

void *me_memcpy(void *dst, const void *src, unsigned int count)
{
	void * ret = dst;
	
	while (count--) 
	{
		*(char *)dst = *(char *)src;
		dst = (char *)dst + 1;
		src = (char *)src + 1;
	}

	return ret;
}

unsigned int me_strlen(const char *str)
{
	const char *eos = str;
	
	while( *eos++ );
	
	return( (int)(eos - str - 1) );
}

int me_memcmp(const void *buf1, const void *buf2, unsigned int count)
{
	if (!count)
		return(0);
	
	while ( --count && *(char *)buf1 == *(char *)buf2 ) 
	{
		buf1 = (char *)buf1 + 1;
		buf2 = (char *)buf2 + 1;
	}
	
	return( *((unsigned char *)buf1) - *((unsigned char *)buf2) );

}

char *me_strcpy(char *dst, const char *src )
{
	char * cp = dst;
	
	while((*cp++ = *src++) != 0);

	
	return dst;
}

char * me_strstr (const char * str1, const char * str2)
{
	char *cp = (char *) str1;
	char *s1, *s2;
	
	if ( !*str2 )
		return((char *)str1);
	
	while (*cp)
	{
		s1 = cp;
		s2 = (char *) str2;
		
		while ( *s1 && *s2 && !(*s1-*s2) )
			s1++, s2++;
		
		if (!*s2)
			return(cp);
		
		cp++;
	}
	
	return NULL;
}

int me_atoi(const char *str)
{
	int c;
	long total;
	int sign;
	
	while (*str == ' ')
		++str;
	
	c = (int)(unsigned char)*str++;
	sign = c;
	if (c == '-' || c == '+')
		c = (int)(unsigned char)*str++;
	
	total = 0;
	
	while((c >='0')&&(c <= '9'))
	{
		total = 10 * total + (c - '0'); 
		c = (int)(unsigned char)*str++;
	}
	
	if (sign == '-')
		return (int)-total;
	else
		return (int)total;
}

void *me_malloc(unsigned int size)
{
	return malloc(size);
}

void me_free(void *pData)
{
	free(pData);
}

void me_initlock(OSALLOCK *pLock)
{
	InitializeCriticalSection(&pLock->Lock);
}

void me_lock(OSALLOCK *pLock)
{
	EnterCriticalSection(&pLock->Lock);
}

void me_unlock(OSALLOCK *pLock)
{
	LeaveCriticalSection(&pLock->Lock);
}

unsigned int __stdcall TimerThreadFunc(LPVOID param)
{
	UINT uTimeOut = INFINITE;
	OSALTIMER *pTimer = (OSALTIMER *)(param);
	
	const HANDLE hEvents[] = 
	{
		pTimer->hSetEvt,
		pTimer->hKillEvt,
		pTimer->hExitEvt
	};
	
	ResetEvent(hEvents[0]);
	ResetEvent(hEvents[1]);
	ResetEvent(hEvents[2]);

	while(TRUE)
	{
		switch(WaitForMultipleObjects(3, hEvents, FALSE, uTimeOut))
		{
			case WAIT_OBJECT_0:	//set event
				uTimeOut = pTimer->duetime;
				break; 
			case WAIT_OBJECT_0+1:  //kill event	
				uTimeOut = INFINITE;
				SetEvent(pTimer->hKilledEvt);
				break;
			case WAIT_OBJECT_0+2:  //exit event
				return 0;
			case WAIT_TIMEOUT:
				uTimeOut = INFINITE;
				SetEvent(pTimer->hKilledEvt);
				pTimer->timer_cb(pTimer->pData);
				ResetEvent(pTimer->hKilledEvt);
				break;
			default:
				break;	
		}
	}

	return 0;
}

UINT StartTimer(OSALTIMER *timer)
{
	if (NULL != timer->hThread)
		return 0;
	if (NULL == timer->hSetEvt)
		timer->hSetEvt = CreateEvent(NULL, FALSE, FALSE, NULL);
	if (NULL == timer->hKillEvt)
		timer->hKillEvt = CreateEvent(NULL, FALSE, FALSE, NULL);
	if (NULL == timer->hExitEvt)
		timer->hExitEvt = CreateEvent(NULL, FALSE, FALSE, NULL);
	if (NULL == timer->hKilledEvt)
		timer->hKilledEvt = CreateEvent(NULL, FALSE, FALSE, NULL);


	timer->hThread = (HANDLE)_beginthreadex(
	NULL, 0, TimerThreadFunc, (LPVOID)timer, 0, NULL);
	
	Sleep(1000);
	return (NULL != timer->hThread);
}


void me_inittimer(OSALTIMER *timer)
{
	timer->hThread = NULL;
	timer->hSetEvt = NULL;
	timer->hKillEvt = NULL;
	timer->hExitEvt = NULL;
	timer->hKilledEvt = NULL;
	timer->duetime =  INFINITE;
	StartTimer(timer);
}

void me_settimer(OSALTIMER *timer, int duetime)
{
	timer->duetime = duetime;
	SetEvent(timer->hSetEvt);
}

void me_killtimer(OSALTIMER *timer, int event)
{
	if(event == AT_TIMEOUT)
	{
		DebugOutA("[medrv]: Kill timer, event is AT_TIMEOUT\n");
		return;
	}
	
	SetEvent(timer->hKillEvt);
	WaitForSingleObject(timer->hKilledEvt, INFINITE);
}

void me_exittimer(OSALTIMER *timer)
{
	SetEvent(timer->hExitEvt);
	WaitForSingleObject(timer->hThread, INFINITE);
	CloseHandle(timer->hThread);
	timer->hThread = NULL;
	DebugOutA("[medrv]: Exit timer thread\n");

}

NTSTATUS me_send_at(void* ioproxy, const char *str, ME_TX_NOTIFY notify, void *pData)
{
//	ME_Print("\r\n[txd] %s\r\n", str);

	NTSTATUS ret = STATUS_FAILED;
	
	if(ioproxy != NULL)
	{
		CommModule *pCom = (CommModule*)ioproxy;
		if(pCom->WriteData((const char*)str, (const int)me_strlen(str), 0))
		{
			notify(STATUS_SUCCESS, (const int)me_strlen(str), pData);
			ret = STATUS_SUCCESS;
		}
		else 
		{
			notify(STATUS_FAILED, (const int)me_strlen(str), pData);
		}

	}
	PMEFSM pfsm = (PMEFSM)pData;
	
	WaitForSingleObject(pfsm->req.hBlockEvt, INFINITE);
	ResetEvent(pfsm->req.hBlockEvt);
	
	return (ret == STATUS_SUCCESS);
}

NTSTATUS me_send_pdu(void* ioproxy, const char *str, ME_TX_NOTIFY notify, void *pData)
{

	NTSTATUS ret = STATUS_FAILED;
	
	if(ioproxy != NULL)
	{
		CommModule *pCom = (CommModule*)ioproxy;
		if(pCom->WriteData((const char*)str, (const int)me_strlen(str), 0))
		{
			notify(STATUS_SUCCESS, (const int)me_strlen(str), pData);
			ret = STATUS_SUCCESS;
		}
		else 
		{
			notify(STATUS_FAILED, (const int)me_strlen(str), pData);
		}
		
	}

	return ret;
}

void me_respond_ready(void *pData)
{
	PMEFSM pfsm = (PMEFSM)pData;
	SetEvent(pfsm->req.hBlockEvt);
}

NTSTATUS me_get_rettype(void *pData)
{
	ATResult *pRet = (ATResult*)((PMEFSM)pData)->req.context;
	return pRet->retType;
}
