//
//  basestr.h
//  DCT
//
//  Created by MBJ on 12-7-7.
//  Copyright 2012 MediaTek Inc. All rights reserved.
//

#ifndef _DCT_BASE_STRING_H_
#define _DCT_BASE_STRING_H_

//#include "Win2Mac.h"


int WideCharToMultiByte(
						int codePage, 
						DWORD dwFlags, 
						LPCWSTR lpWideCharStr, 
						size_t cchWideChar, 
						LPSTR lpMultiByteStr,  
						size_t cbMultiByte, 
						LPCSTR lpDefaultChar, 
						BOOL *lpUsedDefaultChar);

int MultiByteToWideChar(
						int codePage, 
						DWORD dwFlags, 
						LPCSTR lpMultiByteStr, 
						size_t cchMultiByte, 
						LPWSTR lpWidecharStr, 
						size_t chWideChar);

//convert between char* and wchar*
char* convertWidecharToChar(wchar_t *wstr);

wchar_t* convertCharToWidechar(char *str);

wchar_t* wstrfmt_s2S(LPCWSTR fmt);

int _stprintf(TCHAR* buf, LPCTSTR fmt, ...);

int stricmp(const char* dst, const char* src);

TCHAR* _tcsupr(TCHAR* stringSrc);

TCHAR* _tcslwr(TCHAR* stringSrc);

#endif // _DCT_BASE_STRING_H_