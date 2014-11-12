/*
 *  Win2Mac.cpp
 *  Demo
 *
 *  Created by MBJ on 11-8-26.
 *  Copyright 2011 MediaTek Inc. All rights reserved.
 *
 */

#include "Win2Mac.h"
#include "afxstr.h"

void CString::Init()
{
	m_ptstr = (LPTSTR)malloc(sizeof(TCHAR));	//	ugly..
	m_ptstr[0] = 0;
	m_nDataLength = 0;
}
void CString::Empty()
{
	if (m_ptstr)
		free(m_ptstr);
	Init();
}

CString::CString(const CString& stringSrc)
{
	if (stringSrc.GetLength() == 0)
		Init();
	else {
		m_nDataLength = stringSrc.GetLength();
		m_ptstr = (LPTSTR)malloc((m_nDataLength+1)*sizeof(TCHAR));
		_tcsncpy(m_ptstr, stringSrc.m_ptstr, m_nDataLength);
		m_ptstr[m_nDataLength] = 0;
	}
}

CString::~CString()
{
	if (m_ptstr)
		free(m_ptstr);
	m_ptstr = NULL;
	m_nDataLength = 0;
}

CString::CString(LPCTSTR lpsz)
{
	if (lpsz && *lpsz)
	{
		m_nDataLength = _tcslen(lpsz);
		m_ptstr = (LPTSTR)malloc((m_nDataLength+1)*sizeof(TCHAR));
		_tcscpy(m_ptstr, lpsz);
	}
	else
		Init();
}

#ifdef _UNICODE
CString::CString(LPCSTR lpsz)
{
	if (lpsz && *lpsz)
	{
		m_nDataLength = mbstowcs(NULL, lpsz, strlen(lpsz));
		m_ptstr = (LPTSTR)malloc((m_nDataLength+1)*sizeof(TCHAR));
		mbstowcs(m_ptstr, lpsz, strlen(lpsz));
		m_ptstr[m_nDataLength] = 0;
	}
	else
		Init();
}
#else
CString::CString(LPCWSTR lpsz)
{
	if (lpsz && *lpsz)
	{
		m_nDataLength = wcstombs(NULL, lpsz, wcslen(lpsz));
		m_ptstr = (LPTSTR)malloc((m_nDataLength+1)*sizeof(TCHAR));
		wcstombs(m_ptstr, lpsz, wcslen(lpsz));
		m_ptstr[m_nDataLength] = 0;
	}
	else
		Init();
}
#endif

const CString& CString::operator=(const CString& stringSrc)
{
	if (this != &stringSrc)
	{
		if (stringSrc.GetLength() == 0) {	
			Empty();
			return *this;
		}
		
		if (stringSrc.GetLength() > m_nDataLength) {
			free(m_ptstr);
			m_ptstr = (LPTSTR)malloc((stringSrc.GetLength()+1)*sizeof(TCHAR));
			memset(m_ptstr, 0x0, (stringSrc.GetLength() + 1) * sizeof(TCHAR));
		}
		m_nDataLength = stringSrc.GetLength();
		_tcsncpy(m_ptstr, stringSrc.m_ptstr, m_nDataLength);
		m_ptstr[m_nDataLength] = 0;
	}
	return *this;
}

const CString& CString::operator=(LPCTSTR lpsz)
{
	if (m_ptstr != lpsz)
	{
		if (lpsz && *lpsz)
		{
			size_t len = _tcslen(lpsz);
			if (m_nDataLength < len)
			{
				free(m_ptstr);
				m_ptstr = (LPTSTR)malloc((len+1)*sizeof(TCHAR));
			}
			m_nDataLength = len;
			_tcscpy(m_ptstr, lpsz);
		}
		else
			Empty();
	}
	return *this;
}

#ifdef _UNICODE
const CString& CString::operator=(LPCSTR lpsz)
{
	if (lpsz && *lpsz)
	{
		int len = (int)mbstowcs(NULL, lpsz, strlen(lpsz));
		if (m_nDataLength < len)
		{
			free(m_ptstr);
			m_ptstr = (LPTSTR)malloc((len+1)*sizeof(TCHAR));
		}
		mbstowcs(m_ptstr, lpsz, strlen(lpsz));
		m_nDataLength = len;
		m_ptstr[m_nDataLength] = 0;
	}
	else
		Empty();
	
	return *this;
}
#else
const CString& CString::operator=(LPCWSTR lpsz)
{
	if (lpsz && *lpsz)
	{
		int len = wcstombs(NULL, lpsz, wcslen(lpsz));
		if (m_nDataLength < len)
		{
			free(m_ptstr);
			m_ptstr = (LPTSTR)malloc((len+1)*sizeof(TCHAR));
		}
		wcstombs(m_ptstr, lpsz, wcslen(lpsz));
		m_nDataLength = len;
		m_ptstr[m_nDataLength] = 0;
	}
	else
		Empty();
	return *this;
}
#endif


void CString::ConcatCopy(size_t nSrc1Len, LPCTSTR lpszSrc1Data,
						 size_t nSrc2Len, LPCTSTR lpszSrc2Data)
{
	m_nDataLength = nSrc1Len + nSrc2Len;
	if (m_nDataLength != 0)
	{
		m_ptstr = (LPTSTR)malloc((m_nDataLength+1)*sizeof(TCHAR));
		//	src1 or src2 may not end with 0.
		memcpy(m_ptstr, lpszSrc1Data, nSrc1Len*sizeof(TCHAR));
		memcpy(m_ptstr+nSrc1Len, lpszSrc2Data, nSrc2Len*sizeof(TCHAR));
		m_ptstr[m_nDataLength] = 0;
	}
}

CString operator+(const CString& string1, const CString& string2)
{
	CString s;
	s.ConcatCopy(string1.GetLength(), string1.m_ptstr,
				 string2.GetLength(), string2.m_ptstr);
	return s;
}

CString operator+(const CString& string, LPCTSTR lpsz)
{
	CString s;
	if (lpsz && *lpsz)
		s.ConcatCopy(string.GetLength(), string.m_ptstr,
					 _tcslen(lpsz), lpsz);
	else
		s = string;

	return s;
}

CString operator+(LPCTSTR lpsz, const CString& string)
{
	CString s;
	if (lpsz && *lpsz)
		s.ConcatCopy(_tcslen(lpsz), lpsz, 
				 string.GetLength(), string.m_ptstr);
	else
		s = string;
	
	return s;
}

void CString::ConcatInPlace(size_t nSrcLen, LPCTSTR lpszSrcData)
{
	if (nSrcLen == 0)
		return;
	
	TCHAR* p = m_ptstr;
	ConcatCopy(m_nDataLength, p, nSrcLen, lpszSrcData);
	free(p);
}

const CString& CString::operator+=(LPCTSTR lpsz)
{
	if (lpsz && *lpsz)
		ConcatInPlace(_tcslen(lpsz), lpsz);
	
	return *this;
}

const CString& CString::operator+=(const CString& string)
{
	ConcatInPlace(string.GetLength(), string.m_ptstr);
	return *this;
}

const CString& CString::operator+=(TCHAR ch)
{
	ConcatInPlace(1, &ch);
	return *this;
}

LPTSTR CString::GetBuffer(size_t nMinBufLength)
{
	if (nMinBufLength > m_nDataLength)
	{
		free(m_ptstr);
		m_nDataLength = nMinBufLength;
		m_ptstr = (LPTSTR)malloc((m_nDataLength+1)*sizeof(TCHAR));
		m_ptstr[0] = 0;
	}
	return m_ptstr;
}

void CString::ReleaseBuffer(size_t nNewLength)
{
}

ssize_t CString::Find(TCHAR ch, size_t nStart) const
{
	if (nStart >= m_nDataLength)
		return -1;
	
	// find first single character
	LPTSTR lpsz = _tcschr(m_ptstr + nStart, (_TUCHAR)ch);
	
	// return -1 if not found and index otherwise
	return (lpsz == NULL) ? -1 : (lpsz - m_ptstr);
}

ssize_t CString::Find(LPCTSTR lpszSub, size_t nStart) const
{
	if (lpszSub && *lpszSub && nStart <= m_nDataLength)
	{
		LPTSTR lpsz = _tcsstr(m_ptstr + nStart, lpszSub);
		return (lpsz == NULL) ? -1: (lpsz - m_ptstr);
	}
	return -1;
}

ssize_t CString::FindOneOf(LPCTSTR lpszCharSet) const
{
	if (lpszCharSet && *lpszCharSet)
	{
		LPTSTR lpsz = _tcspbrk(m_ptstr, lpszCharSet);
		return (lpsz == NULL) ? -1 : (lpsz - m_ptstr);
	}
	return -1;
}

ssize_t CString::ReverseFind(TCHAR ch) const
{	
	LPTSTR lpsz = _tcsrchr(m_ptstr, ch);
	return (lpsz == NULL) ? -1 : (lpsz - m_ptstr);
}

CString CString::Mid(size_t nFirst, size_t nCount) const
{
	CString dest;
	if(nFirst + nCount <= m_nDataLength)
	{
		if (dest.m_nDataLength < nCount)
		{
			free(dest.m_ptstr);
			dest.m_ptstr = (LPTSTR)malloc((nCount+1)*sizeof(TCHAR));
		}
		dest.m_nDataLength = nCount;
		_tcsncpy(dest.m_ptstr, m_ptstr + nFirst, nCount);
		m_ptstr[m_nDataLength] = 0;
	}
	return dest;
}

void CString::TrimRight()
{
	TCHAR* p = m_ptstr + m_nDataLength - 1;
	while (p >= m_ptstr)
	{
		if (*p == _T(' ')	||
			*p == _T('	')	||
			*p == _T('\n')	||
			*p == _T('\r'))
			p--;
		else 
			break;
	}
	
	m_nDataLength = m_ptstr<=p?p-m_ptstr+1:0;
	m_ptstr[m_nDataLength] = 0;
}

void CString::TrimLeft()
{	
	TCHAR* p = m_ptstr;
	while (*p && (*p == _T(' ')		||
				  *p == _T('	')	||
				  *p == _T('\n')	||
				  *p == _T('\r')))
		p++;
	if (m_ptstr != p)
	{
		TCHAR* old = m_ptstr;
	m_nDataLength -= p - m_ptstr;
		while (p && *p)
			*old++ = *p++;
		m_ptstr[m_nDataLength] = 0;
	}
}

size_t CString::Insert(size_t nIndex, TCHAR ch)
{	
	if (ch)
	{
		if (nIndex > m_nDataLength)
			nIndex = m_nDataLength;

		LPTSTR p = (LPTSTR)malloc((m_nDataLength+2)*sizeof(TCHAR));
		
		if (nIndex > 0)
			memcpy(p, m_ptstr, nIndex*sizeof(TCHAR));
		*(p+nIndex) = ch;
		if (nIndex != m_nDataLength)
			memcpy(p+nIndex+1, m_ptstr+nIndex, (m_nDataLength-nIndex)*sizeof(TCHAR));

		m_nDataLength ++;
		free(m_ptstr);
		m_ptstr = p;
		m_ptstr[m_nDataLength] = 0;
	}
	return m_nDataLength;
}

size_t CString::Insert(size_t nIndex, LPCTSTR pstr)
{
	if (pstr && *pstr)
	{
		if (nIndex > m_nDataLength)
			nIndex = m_nDataLength;
		size_t len = _tcslen(pstr);
		LPTSTR p = (LPTSTR)malloc((m_nDataLength+len+1)*sizeof(TCHAR));
		
		if (nIndex > 0)
			memcpy(p, m_ptstr, nIndex*sizeof(TCHAR));
		memcpy(p+nIndex, pstr, len*sizeof(TCHAR));
		if (nIndex != m_nDataLength)
			memcpy(p+nIndex+len, m_ptstr+nIndex, (m_nDataLength-nIndex)*sizeof(TCHAR));
		m_nDataLength += len;
		free(m_ptstr);
		m_ptstr = p;
		m_ptstr[m_nDataLength] = 0;
	}
	return m_nDataLength;
}

void CString::Format(LPCTSTR lpszFormat, ...)
{
	// guess total length
	LPCTSTR p;
	size_t nLen = 0;
	va_list arg;
	va_start(arg, lpszFormat);
	for (p = lpszFormat; *p != 0; p++)
	{
		if (*p != _T('%') || (*p == *(p+1) == _T('%')))
		{
			nLen++;
			continue;
		}
		
		BOOL done = FALSE;
		for (; *p && !done; p++)
		{
			switch (*p)
			{
				case _T('c'):
				case _T('C'):
				case _T('d'):
				case _T('i'):
				case _T('u'):
				case _T('x'):
				case _T('X'):
				case _T('o'):
					va_arg(arg, int);
					nLen += 32;
					done = TRUE;
					break;
				case _T('e'):
				case _T('g'):
				case _T('G'):
				case _T('f'):
					va_arg(arg, double);
					nLen += 128;
					done = TRUE;
					break;
				case _T('s'):
				{
					LPCTSTR tmp = va_arg(arg, LPCTSTR);
					if (tmp && *tmp)
						nLen += _tcslen(tmp)+1;
					done = TRUE;
				}
				break;
				default:
					nLen ++;
					done = FALSE;
				break;
			}
		}
		if (*p == 0)
			break;
	}
	
	free(m_ptstr);
	m_nDataLength = nLen;
	m_ptstr = (LPTSTR)malloc((m_nDataLength+1)*sizeof(TCHAR));
	va_start(arg, lpszFormat);
#ifdef _UNICODE
	wchar_t* newfmt = wstrfmt_s2S(lpszFormat);
	vswprintf(m_ptstr, m_nDataLength, newfmt, arg);
	free(newfmt);
#else
	vsprintf(m_ptstr, lpszFormat, arg);
#endif
	va_end(arg);
}
