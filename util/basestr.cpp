//
//  string.mm
//  DCT
//
//  Created by MBJ on 12-7-7.
//  Copyright 2012 MediaTek Inc. All rights reserved.
//

#include "Win2Mac.h"
#include "basestr.h"

int WideCharToMultiByte(
						int codePage, 
						DWORD dwFlags, 
						LPCWSTR lpWideCharStr, 
						size_t cchWideChar, 
						LPSTR lpMultiByteStr,  
						size_t cbMultiByte, 
						LPCSTR lpDefaultChar, 
						BOOL *lpUsedDefaultChar)
{
	setlocale(LC_ALL, ".ACP");
	
	int n = (int)wcstombs(lpMultiByteStr, lpWideCharStr, cbMultiByte);
	
	if(lpMultiByteStr)
		lpMultiByteStr[n] = 0;
	
	return n;
}

int MultiByteToWideChar(
						int codePage,
						DWORD dwFlags,
						LPCSTR lpMultiByteStr,
						size_t cchMultiByte, 
						LPWSTR lpWidecharStr,
						size_t chWideChar)
{
	setlocale(LC_ALL, ".ACP");
	
	int n = (int)mbstowcs(lpWidecharStr, lpMultiByteStr, chWideChar);
	
	if(lpWidecharStr)
		lpWidecharStr[n] = 0;
	
	return n;
}

//convert between char* and wchar*
char* convertWidecharToChar(wchar_t *wstr)
{
	int sizeNeed = WideCharToMultiByte(0, 0, wstr, -1, NULL, 0, NULL, NULL);
	char *str = new char[sizeNeed + 1];
	WideCharToMultiByte(0, 0, wstr, -1, str, sizeNeed, NULL, NULL);
	
	return str;
}

wchar_t* convertCharToWidechar(char *str)
{
	int sizeNeed = MultiByteToWideChar(0, 0, str, -1, NULL, 0);
	wchar_t *wstr = new wchar_t[sizeNeed+1];
	MultiByteToWideChar(0, 0, str, -1, wstr, sizeNeed);
	
	return wstr;
}

wchar_t* wstrfmt_s2S(LPCWSTR fmt)
{
	wchar_t* newfmt = (wchar_t*)malloc((wcslen(fmt)+1)*sizeof(wchar_t));
	wcscpy(newfmt, fmt);
	wchar_t *p = newfmt;
	for (; *p != 0; p++)
	{
		if (*p == L'%') {
			p++;
			if (*p == L's') *p = L'S';
			else if (*p == L'c') *p = L'C';
		}
	}
	return newfmt;
}

int _stprintf(TCHAR* buf, LPCTSTR fmt, ...)
{
	int ret;
	va_list args;
#ifdef _UNICODE
	wchar_t* wfmt = wstrfmt_s2S(fmt);
	va_start(args, fmt);
	
	//	WARNING!!! 512 assumes buf is big enough 
	//	and buf len < 512
	ret = swprintf(buf, 512, wfmt, args);
	free(wfmt);
#else
	ret = sprintf(buf, fmt, args);
#endif
	va_end(args);
	return ret;
}

int stricmp(const char* dst, const char* src)
{
	int d, s;
	do {
		d = *dst; s = *src;
		if (d >= 'A' && d <= 'Z')
			d -= ('A' - 'a');
		if (s >= 'A' && s <= 'Z')
			s -= ('A' - 'a');
		dst++; src++;
	} while (d && (d == s));
	
	return (d - s);
}

TCHAR* _tcsupr(TCHAR* stringSrc)
{
	TCHAR* cp;
	for (cp = stringSrc; *cp; ++cp)
		if (_T('a') <= *cp && *cp <= _T('z'))
			*cp += 'A' - 'a';
	return stringSrc;
}

TCHAR* _tcslwr(TCHAR* stringSrc)
{	
	TCHAR* cp;
	for (cp = stringSrc; *cp; ++cp)
		if (_T('A') <= *cp && *cp <= _T('Z'))
			*cp += 'a' - 'A';
	return stringSrc;
}
