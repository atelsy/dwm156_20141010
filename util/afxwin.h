/*
 *  cwnd.h
 *  MacAgent
 *
 *  Created by MBJ on 11-8-23.
 *  Copyright 2011 MediaTek Inc. All rights reserved.
 *
 */
#ifndef _DCT_AFXWIN_H_
#define _DCT_AFXWIN_H_


typedef pthread_mutex_t CRITICAL_SECTION,*LPCRITICAL_SECTION;

void InitializeCriticalSection(LPCRITICAL_SECTION lpCriticalSection);
void DeleteCriticalSection(LPCRITICAL_SECTION lpCriticalSection);
void EnterCriticalSection(LPCRITICAL_SECTION lpCriticalSection);
void LeaveCriticalSection(LPCRITICAL_SECTION lpCriticalSection);


typedef DWORD __stdcall (*winThreadFunc) (LPVOID);

typedef struct {
	winThreadFunc	fn;
	LPVOID			arg;
	BOOL*			psig;
}	winThreadFuncWrapper;

typedef struct {
	BYTE				type;
	pthread_t			thread;
	pthread_mutex_t		mutex;
	pthread_cond_t		condition;
	BOOL				signaled;
	BOOL				manualreset;
}	HANDLE_CONTENT,		*HANDLE;

BOOL CloseHandle(HANDLE hObject);
HANDLE CreateEvent(LPVOID attr, BOOL bManualReset, BOOL bInitialState, LPCTSTR lpName);
BOOL SetEvent(HANDLE hd);
BOOL ResetEvent(HANDLE hd);
HANDLE CreateMutex(LPVOID attr, BOOL initialOwner, LPCTSTR lpName);
BOOL ReleaseMutex(HANDLE hd);
void* pthreadHelper(void* arg);
HANDLE _beginthreadex(LPVOID attr, DWORD stack_size, winThreadFunc wfn, 
					  LPVOID arglist, DWORD initflag, DWORD* thrdaddr);
BOOL TerminateThread(HANDLE hd, DWORD dwExitCode);
DWORD WaitForSingleObject(HANDLE hd, DWORD dwMilliseconds);
DWORD WaitForMultipleObjects(DWORD nCount, const HANDLE* lpHandles, 
							 BOOL bWaitAll, DWORD dwMilliseconds);
class CTimeSpan
{
public:
	CTimeSpan()	{}
	CTimeSpan(time_t time)	{ m_timeSpan = time; }
	CTimeSpan(LONG lDays, int nHours, int nMins, int nSecs)
	{ m_timeSpan = nSecs + 60 * (nMins + 60 * (nHours + 24 * (lDays))); }
	CTimeSpan(const CTimeSpan& timeSpanSrc)	
	{ m_timeSpan = timeSpanSrc.m_timeSpan; }
	const CTimeSpan& operator=(const CTimeSpan& timeSpanSrc)
	{ m_timeSpan = timeSpanSrc.m_timeSpan; return *this; }
	
	LONG GetDays() const
	{ return m_timeSpan / (24*3600L); }
	LONG GetTotalHours() const
	{ return m_timeSpan/3600; }
	int GetHours() const
	{ return (int)(GetTotalHours() - GetDays()*24); }
	LONG GetTotalMinutes() const
	{ return m_timeSpan/60; }
	int GetMinutes() const
	{ return (int)(GetTotalMinutes() - GetTotalHours()*60); }
	LONG GetTotalSeconds() const
	{ return m_timeSpan; }
	int GetSeconds() const
	{ return (int)(GetTotalSeconds() - GetTotalMinutes()*60); }
	
	CTimeSpan operator-(CTimeSpan timeSpan) const
	{ return CTimeSpan(m_timeSpan - timeSpan.m_timeSpan); }
	CTimeSpan operator+(CTimeSpan timeSpan) const
	{ return CTimeSpan(m_timeSpan + timeSpan.m_timeSpan); }
	const CTimeSpan& operator+=(CTimeSpan timeSpan)
	{ m_timeSpan += timeSpan.m_timeSpan; return *this; }
	const CTimeSpan& operator-=(CTimeSpan timeSpan)
	{ m_timeSpan -= timeSpan.m_timeSpan; return *this; }
	BOOL operator==(CTimeSpan timeSpan) const
	{ return m_timeSpan == timeSpan.m_timeSpan; }
	BOOL operator!=(CTimeSpan timeSpan) const
	{ return m_timeSpan != timeSpan.m_timeSpan; }
	BOOL operator<(CTimeSpan timeSpan) const
	{ return m_timeSpan < timeSpan.m_timeSpan; }
	BOOL operator>(CTimeSpan timeSpan) const
	{ return m_timeSpan > timeSpan.m_timeSpan; }
	BOOL operator<=(CTimeSpan timeSpan) const
	{ return m_timeSpan <= timeSpan.m_timeSpan; }
	BOOL operator>=(CTimeSpan timeSpan) const
	{ return m_timeSpan >= timeSpan.m_timeSpan; }
private:
	time_t m_timeSpan;
	friend class CTime;
};

class CTime
{
public:
	static CTime GetCurrentTime()
	{ return CTime(time(NULL)); }
	
	CTime() {}
	CTime(time_t time)	{ m_time = time; }
	CTime(int nYear, int nMonth, int nDay, int nHour, int nMin, 
		  int nSec, int nDST = -1)
	{
		struct tm atm;
		atm.tm_sec = nSec;
		atm.tm_min = nMin;
		atm.tm_hour = nHour;
		ASSERT(nDay >= 1 && nDay <= 31);
		atm.tm_mday = nDay;
		ASSERT(nMonth >= 1 && nMonth <= 12);
		atm.tm_mon = nMonth - 1;		// tm_mon is 0 based
		ASSERT(nYear >= 1970);
		atm.tm_year = nYear - 1970;	 // tm_year is 1900 based
		atm.tm_isdst = nDST;
		m_time = mktime(&atm);
		ASSERT(m_time != -1);	   // indicates an illegal input time
	}
	CTime(const CTime& timeSrc)	{ m_time = timeSrc.m_time; }
	
	const CTime& operator=(const CTime& timeSrc)
	{ m_time = timeSrc.m_time; return *this; }
	const CTime& operator=(time_t t)
	{ m_time = t; return *this; }
	
	time_t GetTime() const	{ return m_time; }
	int GetYear() const
	{ 
		struct tm *time;  
		time = localtime(&m_time);
		return localtime(&m_time)->tm_year + 1900;
	}
	int GetMonth() const	{ return localtime(&m_time)->tm_mon + 1; }
	int GetDay() const	{ return localtime(&m_time)->tm_mday; }
	int GetHour() const	{ return localtime(&m_time)->tm_hour; }
	int GetMinute() const	{ return localtime(&m_time)->tm_min; }
	int GetSecond() const	{ return localtime(&m_time)->tm_sec; }
	int GetDayOfWeek() const	{ return localtime(&m_time)->tm_wday + 1; }
	
	// Operations
	// time math
	CTimeSpan operator-(CTime time) const
	{ return CTimeSpan(m_time - time.m_time); }
	CTime operator-(CTimeSpan timeSpan) const
	{ return CTime(m_time - timeSpan.m_timeSpan); }
	CTime operator+(CTimeSpan timeSpan) const
	{ return CTime(m_time + timeSpan.m_timeSpan); }
	const CTime& operator+=(CTimeSpan timeSpan)
	{ m_time += timeSpan.m_timeSpan; return *this; }
	const CTime& operator-=(CTimeSpan timeSpan)
	{ m_time -= timeSpan.m_timeSpan; return *this; }
	BOOL operator==(CTime time) const
	{ return m_time == time.m_time; }
	BOOL operator!=(CTime time) const
	{ return m_time != time.m_time; }
	BOOL operator<(CTime time) const
	{ return m_time < time.m_time; }
	BOOL operator>(CTime time) const
	{ return m_time > time.m_time; }
	BOOL operator<=(CTime time) const
	{ return m_time <= time.m_time; }
	BOOL operator>=(CTime time) const
	{ return m_time >= time.m_time; }
private:
	time_t m_time;
};


class CRect {
public:
	CRect(double l, double t, double r, double b)
	{left = l; top = t; right = r; bottom = b; }
	
	double left;
	double top;
	double right;
	double bottom;
};

#define HWND			LPVOID
#define WM_USER			0x0400
#define WS_CHILD		0x40000000L
#define SW_HIDE			0
#define WPARAM			LONG
#define LPARAM			LONG
#define MAKEWPARAM(l,h)	(WPARAM)MAKELONG(l,h)


class CWnd;

typedef void (CWnd::*MSGFUNC)(void);

typedef struct
{
	int		msg;
	MSGFUNC	func;

} MSGMAP;

#define DECLARE_MESSAGE_MAP()	\
public:	\
	static MSGMAP m_MsgMap[];	\
	virtual MSGMAP* GetMsgMap();

#define BEGIN_MESSAGE_MAP(theClass,baseClass)	\
	typedef theClass _THISCLASS_;	\
	MSGMAP* theClass::GetMsgMap() { return theClass::m_MsgMap; }	\
	MSGMAP theClass::m_MsgMap[] = {

#define ON_MESSAGE(msg, func)	\
	{msg, (MSGFUNC)&_THISCLASS_::func},

#define END_MESSAGE_MAP()	\
	{0, NULL}};

#define WM_TIMER	0x0113
#define WM_CLOSE	0x0110
#define WM_KILLTIMER	WM_USER + 892

#define ON_WM_CLOSE()	\
	{WM_CLOSE, (MSGFUNC)&_THISCLASS_::OnClose},

#define ON_WM_TIMER()	\
	{WM_TIMER, (MSGFUNC)&_THISCLASS_::OnTimer},

/*	Notes:
 *	1 these two functions post/send message to UI thread.
 *	2 for both C++ & objc class, set hwnd to receiver class' pointer 
 *	  (C++ or Objc) as void pointer.
 *	3 C++ class MUST inherite from CWnd. same usage as in windows 
 *	  (use message mapping macros).
 *	4 objc class will receive a notification named by message number.
 *	  register your notification selector to default center.
 */
BOOL PostMessage(HWND hWnd, UINT msg, WPARAM wparam, LPARAM lparam);
BOOL SendMessage(HWND hWnd, UINT msg, WPARAM wparam, LPARAM lparam);

/*	Notes: 
 *	1 this function add a timer in UI thread and return nstimer pointer.
 *	2 elapse in millionseconds.
 *	3 for objc class: set lpTimerFunc as your timer selector as void pointer.
 *	4 for c++ class:  OnTimer will be called, doesn't support 
 *	  application-defined timer function.
 */
LPVOID SetTimer(HWND hWnd, UINT nIDEvent, UINT nElapse, LPVOID lpTimerFunc);

const unsigned int CPPCLASS_MAGIC = 0x60702891;

/*	Notes:
 *	1 All C++ classes that want to receive message from the UI thread 
 *	  MUST inherit from CWnd!!!
 *	2 use MagicNumber to separate from objc class.
 *	  keep this uint the first member after virtual function table.
 *	3 use a map to link NSTimer pointer with nIDEvent.
 *	  in windows we use id to control a timer, but in objc we must use 
 *	  NSTimer instance's pointer. so..
 */
class CWnd
{
protected:
	unsigned int			m_MagicNumber;
	HWND					m_hWnd;
	std::map<UINT,LPVOID>	m_TimerMap;
	CRITICAL_SECTION		m_timerLock;

public:
	CWnd();
	virtual ~CWnd();
	
public:
	BOOL Create(
				LPCTSTR lpszClassName,
				LPCTSTR lpszWindowName,
				DWORD dwStyle,
				const CRect& rect,
				CWnd* pParentWnd,
				UINT nID);
	
	BOOL IsWindow(HWND hWnd);
	
	BOOL ShowWindow(int nCmdShow);
	
	BOOL DestroyWindow();
	
	UINT SetTimer(UINT nIDEvent, UINT nElapse, LPVOID lpTimerFunc);
	
	BOOL KillTimer(UINT nIDEvent);
	
	BOOL PostMessage(UINT msg, WPARAM wparam = 0, LPARAM lparam = 0)
	{
		return ::PostMessage(m_hWnd, msg, wparam, lparam);
	}
	
	BOOL SendMessage(UINT msg, WPARAM wparam = 0, LPARAM lparam = 0)
	{
		return ::SendMessage(m_hWnd, msg, wparam, lparam);
	}
	
	void OnTimer(UINT);
	void OnClose();
	
	DECLARE_MESSAGE_MAP()
	
	static void HandleMsg(CWnd* pWnd, UINT msg, WPARAM wp, LPARAM lp);
};

//	temp solution:
inline CWnd* AfxGetMainWnd()	{ return NULL; }


class CAutoLock
{
public:
	CAutoLock(LPCRITICAL_SECTION pSec)
	{
		m_pSec = pSec;
		EnterCriticalSection(m_pSec);
	}
	
	~CAutoLock()
	{
		LeaveCriticalSection(m_pSec);
	}
	
private:
	LPCRITICAL_SECTION	m_pSec;
};


#endif	// _DCT_AFXWIN_H_
