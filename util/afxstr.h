//
//  afxstr.h
//  DCT
//
//  Created by MBJ on 12-7-7.
//  Copyright 2012 MediaTek Inc. All rights reserved.
//

#ifndef _DCT_AFX_STRING_H_
#define _DCT_AFX_STRING_H_

#include "basestr.h"

class CString
{
public:
	~CString();
	CString()	{ Init(); }
	CString(const CString& stringSrc);
	CString(LPCSTR lpsz);
	CString(LPCWSTR lpsz);
	
	size_t GetLength() const	{ return m_nDataLength; }
	BOOL IsEmpty() const	{ return m_nDataLength == 0; }
	void Empty();
	TCHAR GetAt(size_t nIndex) const
	{
		ASSERT(nIndex >= 0 && nIndex < m_nDataLength);
		return m_ptstr[nIndex];
	}
	TCHAR operator[](size_t nIndex) const
	{
		// same as GetAt
		ASSERT(nIndex >= 0 && nIndex < m_nDataLength);
		return m_ptstr[nIndex];
	}
	operator LPCTSTR() const	{ return m_ptstr; }
	
	const CString& operator=(const CString& stringSrc);
	const CString& operator=(LPCSTR lpsz);
	const CString& operator=(LPCWSTR lpsz);	
	const CString& operator+=(const CString& string);
	const CString& operator+=(LPCTSTR lpsz);
	const CString& operator+=(TCHAR ch);
	
	friend CString operator+(const CString& string1, const CString& string2);
	friend CString operator+(const CString& string, LPCTSTR lpsz);
	friend CString operator+(LPCTSTR lpsz, const CString& string);
	
	int Compare(LPCTSTR lpsz) const	{ return _tcscmp(m_ptstr, lpsz); }	// MBCS/Unicode aware
	
	CString Mid(size_t nFirst, size_t nCount) const;
	CString Mid(size_t nFirst) const	{ return Mid(nFirst, m_nDataLength - nFirst); }
	CString Left(size_t nCount) const	{ return Mid(0, nCount); }
	CString Right(size_t nCount) const	{ return Mid(m_nDataLength - nCount, nCount); }
	
	void MakeUpper()	{ _tcsupr(m_ptstr); }
	void MakeLower()	{ _tcslwr(m_ptstr); }
	void TrimRight();
	void TrimLeft();
	
	ssize_t Find(TCHAR ch, size_t nStart) const;
	ssize_t Find(TCHAR ch) const	{ return Find(ch, 0); }
	ssize_t Find(LPCTSTR lpszSub, size_t nStart) const;
	ssize_t Find(LPCTSTR lpszSub) const	{ return Find(lpszSub, 0); }
	ssize_t FindOneOf(LPCTSTR lpszCharSet) const;
	ssize_t ReverseFind(TCHAR ch) const;
	size_t Insert(size_t nIndex, TCHAR ch);
	size_t Insert(size_t nIndex, LPCTSTR pstr);
	void Format(LPCTSTR lpszFormat, ...);
	//	BOOL LoadString(UINT nID);
	
	LPTSTR GetBuffer(size_t nMinBufLength);
	void ReleaseBuffer(size_t nNewLength = -1);
	
protected:
	LPTSTR m_ptstr;   // pointer to ref counted string data
	size_t m_nDataLength;	//	number of characters. poor name...
	void Init();
	void ConcatCopy(size_t nSrc1Len, LPCTSTR lpszSrc1Data, size_t nSrc2Len, LPCTSTR lpszSrc2Data);
	void ConcatInPlace(size_t nSrcLen, LPCTSTR lpszSrcData);
};

inline bool operator==(const CString& s1, const CString& s2)
{ return s1.Compare(s2) == 0; }
inline bool operator==(const CString& s1, LPCTSTR s2)
{ return s1.Compare(s2) == 0; }
inline bool operator==(LPCTSTR s1, const CString& s2)
{ return s2.Compare(s1) == 0; }
inline bool operator!=(const CString& s1, const CString& s2)
{ return s1.Compare(s2) != 0; }
inline bool operator!=(const CString& s1, LPCTSTR s2)
{ return s1.Compare(s2) != 0; }
inline bool operator!=(LPCTSTR s1, const CString& s2)
{ return s2.Compare(s1) != 0; }
inline bool operator<(const CString& s1, const CString& s2)
{ return s1.Compare(s2) < 0; }
inline bool operator<(const CString& s1, LPCTSTR s2)
{ return s1.Compare(s2) < 0; }
inline bool operator<(LPCTSTR s1, const CString& s2)
{ return s2.Compare(s1) > 0; }
inline bool operator>(const CString& s1, const CString& s2)
{ return s1.Compare(s2) > 0; }
inline bool operator>(const CString& s1, LPCTSTR s2)
{ return s1.Compare(s2) > 0; }
inline bool operator>(LPCTSTR s1, const CString& s2)
{ return s2.Compare(s1) < 0; }
inline bool operator<=(const CString& s1, const CString& s2)
{ return s1.Compare(s2) <= 0; }
inline bool operator<=(const CString& s1, LPCTSTR s2)
{ return s1.Compare(s2) <= 0; }
inline bool operator<=(LPCTSTR s1, const CString& s2)
{ return s2.Compare(s1) >= 0; }
inline bool operator>=(const CString& s1, const CString& s2)
{ return s1.Compare(s2) >= 0; }
inline bool operator>=(const CString& s1, LPCTSTR s2)
{ return s1.Compare(s2) >= 0; }
inline bool operator>=(LPCTSTR s1, const CString& s2)
{ return s2.Compare(s1) <= 0; }


#endif // _DCT_AFX_STRING_H_