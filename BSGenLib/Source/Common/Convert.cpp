#include "stdafx.h"
#ifndef __BSGEN_PUREWIN
#include <atlenc.h>
#endif

/*static*/ CString Convert::TrueString = _T("True");
/*static*/ CString Convert::FalseString = _T("False");

/*static*/ CString Convert::TrueString_CZ = _T("Ano");
/*static*/ CString Convert::FalseString_CZ = _T("Ne");

#ifndef __BSGEN_PUREWIN
#ifdef __BSGEN_MFC
//Converts the value of an array of 8-bit unsigned integers to its 
//equivalent String representation encoded with base 64 digits.
/*static*/ CString Convert::ToBase64String(CByteArray& value)
{
	if (value.IsEmpty())
		return _T("");
	
	CString szRet;	
	int nLength = (int)value.GetCount();		
	int nWritten = Base64EncodeGetRequiredLength(nLength);	

#ifdef _UNICODE
	char* szBuf = (char*)_malloca(nWritten);
	Base64Encode(value.GetData(), nLength, szBuf, &nWritten);
	szRet = szBuf;
	_freea(szBuf);
#else
	Base64Encode(value.GetData(), nLength, szRet.GetBufferSetLength(nWritten), &nWritten);
#endif
	return szRet;
}
#endif

//Converts the value of an array of 8-bit unsigned integers to its 
//equivalent String representation encoded with base 64 digits.
/*static*/ CString Convert::ToBase64String(CArray<BYTE>& value)
{
	if (value.IsEmpty())
		return _T("");
	
	CString szRet;	
	int nLength = (int)value.GetCount();		
	int nWritten = Base64EncodeGetRequiredLength(nLength);	
#ifdef _UNICODE
	char* szBuf = (char*)_malloca(nWritten);
	Base64Encode(value.GetData(), nLength, szBuf, &nWritten);
	szRet = szBuf;
	_freea(szBuf);
#else
	Base64Encode(value.GetData(), nLength, szRet.GetBufferSetLength(nWritten), &nWritten);
#endif
	return szRet;
}

/*static*/ CString Convert::ToBase64String(BYTE* pData, int nCount)
{
	if (pData == NULL || nCount <= 0)
		return _T("");

	CString szRet;	
	int nWritten = Base64EncodeGetRequiredLength(nCount);
#ifdef _UNICODE
	char* szBuf = (char*)_malloca(nWritten);
	Base64Encode(pData, nCount, szBuf, &nWritten);
	szRet = szBuf;
	_freea(szBuf);
#else
	Base64Encode(pData, nCount, szRet.GetBufferSetLength(nWritten), &nWritten);
#endif
	
	return szRet;
}

//Converts the specified String, which encodes binary data as base 64 digits,
//to an equivalent 8-bit unsigned integer array. 
//RELEASE_NOTE: calling is not compatible with .NET
#ifdef __BSGEN_MFC
/*static*/ void Convert::FromBase64String(LPCTSTR lpszValue, CByteArray& value)
{
	if (lpszValue == NULL || *lpszValue == _T('\0'))	
		value.RemoveAll();			
	else
	{
		int nLength = (int)_tcslen(lpszValue);
		int nWritten = Base64DecodeGetRequiredLength(nLength);
		value.SetSize(nWritten);

		USES_CONVERSION;

		if (Base64Decode(T2CA(lpszValue), nLength, value.GetData(), &nWritten))
			value.SetSize(nWritten);
	}
}
#endif


//Converts the specified String, which encodes binary data as base 64 digits,
//to an equivalent 8-bit unsigned integer array. 
//RELEASE_NOTE: calling is not compatible with .NET
/*static*/ void Convert::FromBase64String(LPCTSTR lpszValue, CArray<BYTE>& value)
{
	if (lpszValue == NULL || *lpszValue == _T('\0'))	
		value.RemoveAll();			
	else
	{
		int nLength = (int)_tcslen(lpszValue);
		int nWritten = Base64DecodeGetRequiredLength(nLength);
		value.SetCount(nWritten);

		USES_CONVERSION;
		if (Base64Decode(T2CA(lpszValue), nLength, value.GetData(), &nWritten))
			value.SetCount(nWritten);
	}
}

/*static*/ void Convert::FromBase64String(LPCTSTR lpszValue, BYTE** pData, int* pnCount)
{
	if (lpszValue == NULL || *lpszValue == _T('\0'))
	{
		*pData = NULL;
		*pnCount = 0;
		return;
	}
	else
	{
		int nLength = (int)_tcslen(lpszValue);
		*pnCount = Base64DecodeGetRequiredLength(nLength);
		*pData = new BYTE[*pnCount];
		
		USES_CONVERSION;
		Base64Decode(T2CA(lpszValue), nLength, *pData, pnCount);
	}
}
#endif