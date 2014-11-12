/*
 *  Win2Mac.M
 *  Demo
 *
 *  Created by MBJ on 11-9-21.
 *  Copyright 2011 MediaTek Inc. All rights reserved.
 *
 */

#import <AppKit/NSApplication.h>
#import <Foundation/NSTimer.h>
#import "DCTAppDelegate.h"

BOOL PostMessage(HWND hWnd, UINT msg, WPARAM wparam, LPARAM lparam)
{
	[(DCTAppDelegate*)TheDelegate HandleWndMsg:hWnd
											Message:msg
											 Param1:wparam
											 Param2:lparam
									  WaitUntilDone:FALSE];
	return TRUE;
}

BOOL SendMessage(HWND hWnd, UINT msg, WPARAM wparam, LPARAM lparam)
{
	[TheDelegate HandleWndMsg:hWnd
						   Message:msg
							Param1:wparam
							Param2:lparam
					 WaitUntilDone:TRUE];
	return TRUE;
}

LPVOID SetTimer(HWND hWnd, UINT nIDEvent, UINT nElapse, LPVOID lpTimerFunc)
{
	//	only support default timer function now.
	NSTimer* timer = [TheDelegate HandleWndTimer:hWnd
											   Elapse:nElapse
											  TimerID:nIDEvent
											TimerFunc:lpTimerFunc];
	return (LPVOID)timer;
}

BEGIN_MESSAGE_MAP(CWnd,CWnd)
END_MESSAGE_MAP()

CWnd::CWnd()
{
	m_MagicNumber = CPPCLASS_MAGIC;
	m_hWnd = NULL; 
	
	InitializeCriticalSection(&m_timerLock);
}

void CWnd::HandleMsg(CWnd* pWnd, UINT msg, WPARAM wp, LPARAM lp)
{
	MSGMAP* pMap = pWnd->GetMsgMap();
	
	while (pMap->msg && pMap->func)
	{
		if (pMap->msg != msg)
		{
			pMap++;
			continue;
		}
		if (msg == WM_TIMER) 
			(pWnd->*(void (CWnd::*)(UINT))(pMap->func))((UINT)wp);
		else if (msg == WM_CLOSE)
			(pWnd->*(void (CWnd::*)(void))(pMap->func))();
		else
			(pWnd->*(LRESULT (CWnd::*)(WPARAM,LPARAM))(pMap->func))(wp,lp);
		break;
	}
}

CWnd::~CWnd()
{
	std::map<UINT,LPVOID>::iterator iter = m_TimerMap.begin();

	while (iter != m_TimerMap.end())
	{
		NSTimer* timer = (NSTimer*)(*iter).second;
		
		[TheDelegate HandleWndMsg:m_hWnd
							   Message:WM_KILLTIMER
								Param1:(long)timer
								Param2:0
						 WaitUntilDone:TRUE];
		m_TimerMap.erase(iter);
		iter ++;
	}
	
	DeleteCriticalSection(&m_timerLock);
}

UINT CWnd::SetTimer(UINT nIDEvent, UINT nElapse, LPVOID lpTimerFunc)
{
	EnterCriticalSection(&m_timerLock);

	std::map<UINT,LPVOID>::iterator iter = m_TimerMap.find(nIDEvent);
	
	if (iter == m_TimerMap.end())
	{
		LPVOID timer = ::SetTimer(m_hWnd, nIDEvent, nElapse, lpTimerFunc);
		m_TimerMap.insert(std::make_pair(nIDEvent, timer));
	}
	
	LeaveCriticalSection(&m_timerLock);
	return nIDEvent;
}

BOOL CWnd::KillTimer(UINT nIDEvent)
{
	EnterCriticalSection(&m_timerLock);

	//	note: invalidate must be called from the thread timer installed.
	std::map<UINT,LPVOID>::iterator iter = m_TimerMap.find(nIDEvent);
	
	if (iter != m_TimerMap.end())
	{
		NSTimer* timer = (NSTimer*)(*iter).second;

		//	Notes: The NSRunLoop object removes and releases the timer!!!
		[TheDelegate HandleWndMsg:m_hWnd
							   Message:WM_KILLTIMER
								Param1:(long)timer
								Param2:0
						 WaitUntilDone:TRUE];
		m_TimerMap.erase(iter);

		LeaveCriticalSection(&m_timerLock);
		return TRUE;
	}

	LeaveCriticalSection(&m_timerLock);
	return FALSE;
}

BOOL CWnd::Create(
				  LPCTSTR lpszClassName,
				  LPCTSTR lpszWindowName,
				  DWORD dwStyle,
				  const CRect& rect,
				  CWnd* pParentWnd,
				  UINT nID)
{
	m_hWnd = this;
	return TRUE;
}

BOOL CWnd::IsWindow(HWND hWnd)
{
	return (hWnd != NULL);
}

BOOL CWnd::ShowWindow(int nCmdShow)
{
	return TRUE;
}

BOOL CWnd::DestroyWindow()
{
	return TRUE;
}

void CWnd::OnTimer(UINT)
{
}

void CWnd::OnClose()
{
}

/////////////////////////////////////////////////
/////////////////////////////////////////////////

#ifdef _ENABLE_LOG_
void DebugOutA(LPCSTR fmt, ...)
{
	va_list args;
	va_start(args, fmt);
	vfprintf(stderr, fmt, args);
	va_end(args);
}
void DebugOutW(LPCWSTR fmt, ...)
{
	//	change to UTF8 and output to console.
	wchar_t buf[512];
	va_list args;
	wchar_t* wfmt = wstrfmt_s2S(fmt);
	
	va_start(args, fmt);
	if (vswprintf(buf, 512-1, wfmt, args) < 0)
		wcscpy(buf+512-4, L"...");
	va_end(args);
	
	int len = (int)wcstombs(NULL, buf, wcslen(buf));
	char* utf8str = (char*)malloc(len+1);
	wcstombs(utf8str, buf, wcslen(buf));
	utf8str[len] = 0;
	
	fprintf(stderr, "%s", utf8str);
	free(utf8str);
	free(wfmt);
}
#endif	// _ENABLE_LOG_

void InitializeCriticalSection(LPCRITICAL_SECTION lpCriticalSection)
{
	pthread_mutexattr_t mutexattr;
	pthread_mutexattr_init(&mutexattr);
	pthread_mutexattr_settype(&mutexattr, PTHREAD_MUTEX_RECURSIVE);
	pthread_mutex_init(lpCriticalSection, &mutexattr);
	pthread_mutexattr_destroy(&mutexattr);
}

void DeleteCriticalSection(LPCRITICAL_SECTION lpCriticalSection)
{
	pthread_mutex_destroy(lpCriticalSection);
}

void EnterCriticalSection(LPCRITICAL_SECTION lpCriticalSection)
{
	pthread_mutex_lock(lpCriticalSection);
}

void LeaveCriticalSection(LPCRITICAL_SECTION lpCriticalSection)
{
	pthread_mutex_unlock(lpCriticalSection);
}


BOOL CloseHandle(HANDLE hObject)
{
	if (!hObject)
		return false;
	
	int ret = 0;
	if (hObject->type & tagTHREAD)
	{
		ret = pthread_mutex_destroy(&hObject->mutex);
		if (ret < 0)
			perror("destroy thread's mutex error");
	}
	if (hObject->type & tagMUTEX)
	{
		ret = pthread_mutex_destroy(&hObject->mutex);
		if (ret < 0)
			perror("destroy mutex error");
	}
	if (hObject->type & tagCONDITION)
	{
		ret = pthread_cond_destroy(&hObject->condition);
		if (ret < 0)
			perror("destroy condition error");
	}
	
	free(hObject);
	return true;
};

HANDLE CreateEvent(LPVOID attr, BOOL bManualReset, BOOL bInitialState, LPCTSTR lpName)
{
	HANDLE hd = (HANDLE)malloc(sizeof(HANDLE_CONTENT));
	hd->type = tagMUTEX|tagCONDITION;
	hd->signaled = bInitialState;
	hd->manualreset = bManualReset;
	pthread_mutex_init(&(hd->mutex), NULL);
	pthread_cond_init(&(hd->condition), NULL);
	
	return hd;
};

BOOL SetEvent(HANDLE hd)
{
	if (hd && (hd->type & tagCONDITION))
	{
		pthread_mutex_lock(&hd->mutex);
		if (hd->manualreset)
			pthread_cond_broadcast(&hd->condition);
		else
			pthread_cond_signal(&hd->condition);
		hd->signaled = true;
		pthread_mutex_unlock(&hd->mutex);
		
		return true;
	}
	return false;
}

BOOL ResetEvent(HANDLE hd)
{
	if (hd && (hd->type & tagCONDITION))
	{
		pthread_mutex_lock(&hd->mutex);
		hd->signaled = false;
		pthread_mutex_unlock(&hd->mutex);
		return true;
	}
	return false;
}

HANDLE CreateMutex(LPVOID attr, BOOL initialOwner, LPCTSTR lpName)
{	
	//	where to use lpname & initialOwner?
	//	the default attr should be private, and windows' is process shared.
	HANDLE hd = (HANDLE)malloc(sizeof(HANDLE_CONTENT));
	hd->type = tagMUTEX;
	pthread_mutexattr_t mutexattr;
	pthread_mutexattr_init(&mutexattr);
	pthread_mutexattr_settype(&mutexattr, PTHREAD_MUTEX_RECURSIVE);
	pthread_mutex_init(&hd->mutex, &mutexattr);
	pthread_mutexattr_destroy(&mutexattr);
	return hd;
}

BOOL ReleaseMutex(HANDLE hd)
{
	if (hd && (hd->type == tagMUTEX))
	{
		pthread_mutex_unlock(&hd->mutex);
		return true;
	}
	return false;
}

void* pthreadHelper(void* arg)
{
	winThreadFuncWrapper* wp = (winThreadFuncWrapper*)arg;
	DWORD dwRet = (*wp->fn)(wp->arg);
	*wp->psig = true;
	free(wp);
	return (void*)dwRet;
}

HANDLE _beginthreadex(LPVOID attr, DWORD stack_size, winThreadFunc wfn, 
							 LPVOID arglist, DWORD initflag, DWORD* thrdaddr)
{
	//	in data card tool's codes some args are always null, just ignore them.
	if (attr || stack_size || initflag)
		printf("some args unsupported in _beginthreadex wrapper~~~\n");
	
	HANDLE hd = (HANDLE)malloc(sizeof(HANDLE_CONTENT));
	hd->type = tagTHREAD;
	hd->signaled = false;
	//	20120323 - prevent multiple simultaneous calls to pthread_join
	pthread_mutexattr_t mutexattr;
	pthread_mutexattr_init(&mutexattr);
	pthread_mutexattr_settype(&mutexattr, PTHREAD_MUTEX_RECURSIVE);
	pthread_mutex_init(&hd->mutex, &mutexattr);
	pthread_mutexattr_destroy(&mutexattr);
	winThreadFuncWrapper* wp = (winThreadFuncWrapper*)malloc(sizeof(winThreadFuncWrapper));
	wp->fn = wfn;
	wp->arg = arglist;
	wp->psig = &hd->signaled;
	int tid = pthread_create(&hd->thread, NULL, pthreadHelper, wp);
	if (thrdaddr)
		*thrdaddr = tid;
	return hd;
}

BOOL TerminateThread(HANDLE hd, DWORD dwExitCode)
{
	if (hd && (hd->type == tagTHREAD))
	{
		pthread_cancel(hd->thread);
		return true;
	}
	return false;
}

DWORD WaitForSingleObject(HANDLE hd, DWORD dwMilliseconds)
{
	//	should the EINVAL be considered?
	DWORD dwRet = WAIT_OBJECT_0;
	
	if (hd && (hd->type & tagCONDITION)) 
	{
		pthread_mutex_lock(&hd->mutex);
		
		if (hd->signaled)	//	signaled before
			dwRet = WAIT_OBJECT_0;
		else if (dwMilliseconds == INFINITE)
			pthread_cond_wait(&hd->condition, &hd->mutex);
		else if (dwMilliseconds == 0)
			dwRet = WAIT_TIMEOUT;
		else
		{
			struct timespec ts;
			struct timeval	tv;
			gettimeofday(&tv, NULL);
			ts.tv_sec = tv.tv_sec + dwMilliseconds/1000;
			ts.tv_nsec = tv.tv_usec*1000 + ((double)(dwMilliseconds%1000))*1000000000;
			int err = pthread_cond_timedwait(&hd->condition, &hd->mutex, &ts);
			if (err == ETIMEDOUT)
				dwRet = WAIT_TIMEOUT;
		}
		
		if (hd->manualreset && dwRet == WAIT_OBJECT_0)
			hd->signaled = true;
		else
			hd->signaled = false;
		
		pthread_mutex_unlock(&hd->mutex);
	}
	else if (hd && (hd->type & tagMUTEX))
	{
		if (dwMilliseconds == INFINITE)
			pthread_mutex_lock(&hd->mutex);
		else if (dwMilliseconds == 0)
		{
			int err = pthread_mutex_trylock(&hd->mutex);
			if (err == EBUSY)
				dwRet = WAIT_TIMEOUT;
		}
		else	//	not support wait a mutex with timeout
			dwRet = WAIT_TIMEOUT;
	}
	else if (hd && (hd->type & tagTHREAD))
	{
		LPVOID ret;
		if (dwMilliseconds == INFINITE)
		{
			pthread_mutex_lock(&hd->mutex);
			pthread_join(hd->thread, &ret);
			pthread_mutex_unlock(&hd->mutex);
		}
		else if (dwMilliseconds == 0)
			dwRet = hd->signaled?WAIT_OBJECT_0:WAIT_TIMEOUT;
		else
		{
			//	cuz pthread_join doesnt support timeout, i have to 
			//	simulate it with nanosleep..
			//	someone say the accurate of nanosleep is 10ms, so..
			struct timespec ts;
			ts.tv_sec = 0;
			ts.tv_nsec = 10*1000000;	//	10ms
			while (!hd->signaled && dwMilliseconds > 10)
			{
				nanosleep(&ts, NULL);
				dwMilliseconds -= 10;
			}
			dwRet = hd->signaled?WAIT_OBJECT_0:WAIT_TIMEOUT;
		}
	}
	
	return dwRet;
}

DWORD WaitForMultipleObjects(DWORD nCount, const HANDLE* lpHandles, 
									BOOL bWaitAll, DWORD dwMilliseconds)
{
	DWORD dwRet = WAIT_FAILED;
	
	if (lpHandles == NULL)
		return dwRet;
	
	//	temp solution: use nanosleep.
	//	or we have to impliment another multi-event type, and re-define create, set & reset.
	//	test the performance carefully!!!
	int i;
	DWORD ret = WAIT_OBJECT_0;
	if (bWaitAll)
	{
		while (dwMilliseconds > 10)
		{
			for (i = 0; i < nCount; i ++)
			{
				ret = WaitForSingleObject(lpHandles[i], 0);
				if (ret != WAIT_OBJECT_0)
					break;
			}
			
			if (i == nCount && ret == WAIT_OBJECT_0)
			{
				dwRet = WAIT_OBJECT_0;
				break;
			}			
			struct timespec ts;
			ts.tv_sec = 0;
			ts.tv_nsec = 10*1000000;	//	10ms
			nanosleep(&ts, NULL);			
			dwMilliseconds -= 10;
		}
	}
	else
	{
		while (dwMilliseconds > 10)
		{
			for (i = 0; i < nCount; i ++)
			{
				ret = WaitForSingleObject(lpHandles[i], 0);
				if (ret == WAIT_OBJECT_0)
					break;
			}
			if (ret == WAIT_OBJECT_0)
			{
				dwRet = WAIT_OBJECT_0 + i;
				break;
			}
			struct timespec ts;
			ts.tv_sec = 0;
			ts.tv_nsec = 10*1000000;	//	10ms
			nanosleep(&ts, NULL);
			dwMilliseconds -= 10;
		}
	}
	
	if (dwMilliseconds <= 10)
		dwRet = WAIT_TIMEOUT;
	
	return dwRet;
}
