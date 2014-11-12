/*
 *  Win2Mac.h
 *  MacAgent
 *
 *  Created by MBJ on 11-8-23.
 *  Copyright 2011 MediaTek Inc. All rights reserved.
 *
 */
#ifndef _WIN2MAC_H_
#define _WIN2MAC_H_

#undef __DEPRECATED


#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include <memory.h>
#include <wchar.h>
#include <errno.h>
#include <assert.h>
#include <pthread.h>
#include <sys/time.h> 
#include <map>

#define _UNICODE	//	UCS2

#define _ENABLE_LOG_

#define BOOL	signed char
#define LPVOID	void*
#ifndef TRUE
#define TRUE	true
#endif
#ifndef FALSE
#define FALSE	false
#endif
#define UINT	unsigned int
#define INT32	int
#define LONG	long
#define LONGLONG	unsigned long long
#define WORD	unsigned short
#define DWORD	unsigned int
#define BYTE	unsigned char
#define LPCSTR	const char*
#define LPSTR	char*
#define LPCWSTR	const wchar_t*
#define LPWSTR	wchar_t*

#ifdef _UNICODE
#define	TCHAR	wchar_t
#define _TUCHAR	TCHAR
#define LPTSTR	LPWSTR
#define LPCTSTR	LPCWSTR
#define	_T(x)	L ## x
#define _tcslen	wcslen
#define _tcscmp	wcscmp
#define _tcsncmp wcsncmp
#define _tcschr	wcschr
#define _tcsrchr	wcsrchr
#define _tcsstr	wcsstr
#define _tcscat wcscat
#define _tcscpy	wcscpy
#define _tcsncpy	wcsncpy
#define _tcspbrk	wcspbrk
#define _stscanf	swscanf
#define _fgetts		fgetws

#ifdef _ENABLE_LOG_
#define DebugOut	DebugOutW
#endif

#else
#define	TCHAR	char
#define _TUCHAR	unsigned char
#define LPTSTR	LPSTR
#define LPCTSTR	LPCSTR
#define	_T(x)	x
#define _tcslen	strlen
#define _tcscmp	strcmp
#define _tcsncmp strcmp
#define _tcschr	strchr
#define _tcsrchr	strrchr
#define _tcsstr	strstr
#define _tcscat strcat
#define _tcscpy	strcpy
#define _tcsncpy	strncpy
#define _tcspbrk	strpbrk
#define _stscanf	sscanf
#define _fgetts		fgets

#ifdef _ENABLE_LOG_
#define DebugOut	DebugOutA
#endif

#endif

#define _TCHAR	TCHAR
#define MAKELONG(a, b)	((int)(((WORD)(a)) | ((int)((WORD)(b))) << 16))
#define MAKEWORD(a, b)	((WORD)(((BYTE)(a)) | ((WORD)((BYTE)(b))) << 8))
#define LOWORD(l)	((WORD)(l))
#define HIWORD(l)	((WORD)(((DWORD)(l) >> 16) & 0xFFFF))
#define WAIT_TIMEOUT	0x102
#define WAIT_OBJECT_0	0x0
#define WAIT_FAILED		0xffffffff
#define INFINITE		0xffffffff
#define __stdcall
#define afx_msg
#define LRESULT		LONG
#define WINAPI		__stdcall
#define CALLBACK	__stdcall
//#define ASSERT(x)	assert(x)
#define _wcstombsz	wcstombs
#define Sleep(x)	usleep((x)*1000)
#define TRACE(x)	DebugOut(x)

#define CBR_110		110
#define CBR_300		300
#define CBR_600		600
#define CBR_1200	1200
#define CBR_2400	2400
#define CBR_4800	4800
#define CBR_9600	9600
#define CBR_14400	14400
#define CBR_19200	19200
#define CBR_38400	38400
#define CBR_57600	57600

#define CBR_56000	56000
#define CBR_115200	115200
#define CBR_128000	128000
#define CBR_256000	256000

#define	tagTHREAD		(1)
#define tagMUTEX		(1 << 1)
#define tagCONDITION	(1 << 2)

#define CP_ACP  0
#define MB_PRECOMPOSED  0x01


#ifdef DEBUG
#define _DEBUG
#define ASSERT assert
#define VERIFY assert
#else
#define ASSERT
#define VERIFY(expr) expr
#endif

#ifdef _ENABLE_LOG_
void DebugOutA(LPCSTR fmt, ...);
void DebugOutW(LPCWSTR fmt, ...);
#endif

#define TheDelegate ((DCTAppDelegate*)[NSApp delegate])

#define INVALID_HANDLE_VALUE (-1)

#include "basestr.h"
#include "afxstr.h"
#include "afxwin.h"

#endif
