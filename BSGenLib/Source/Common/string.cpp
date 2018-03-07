#include "stdafx.h"
#include "../../Include/Common/String.h"

//constructor
CStringEx::CStringEx(LPCTSTR lpszText) 
{
	m_pszData = NULL;
	m_nSize = 0;
	if (lpszText == NULL)
		m_nLength = 0;
	else
	{
		m_nLength = (int)_tcslen(lpszText);
		AllocBuffer(m_nLength);
		memcpy(m_pszData, lpszText, (m_nLength + 1)*sizeof(_TCHAR));
	}
}

CStringEx::CStringEx(const CStringEx& szText) 
{
	m_pszData = NULL;
	m_nSize = 0;
	m_nLength = szText.m_nLength;

	if (szText.m_pszData != 0) {
		BES_ASSERT((int)_tcslen(szText.m_pszData) == m_nLength);

		AllocBuffer(m_nLength);
		memcpy(m_pszData, szText.m_pszData, (m_nLength + 1)*sizeof(_TCHAR));
	}
}

//operator =, assigns a new string
CStringEx& CStringEx::operator = (LPCTSTR lpszText)
{
	if (lpszText == NULL)
		SetNull();
	else
	{
		Empty();
		Append(lpszText);
	}

	return *this;
}

//operator =, assigns a new string
CStringEx& CStringEx::operator = (const CStringEx& szText)
{
	if (szText.m_pszData == NULL)
		SetNull();
	else
	{
		Empty();
		Append(szText);
	}

	return *this;
}

// Appends string or char to the buffer
CStringEx&  CStringEx::Append(_TCHAR ch)
{
	AllocBuffer(m_nLength + 1);
	m_pszData[m_nLength] = ch;
	m_pszData[++m_nLength] = _T('\0');
	return *this;
}	

// Appends string or char to the buffer
CStringEx&  CStringEx::Append(LPCTSTR lpszText)
{
	if (lpszText != NULL) 
	{	
		int nLen = (int)_tcslen(lpszText);
		AllocBuffer(m_nLength + nLen);
		memcpy(&m_pszData[m_nLength], lpszText, sizeof(_TCHAR)*(nLen + 1));
		m_nLength += nLen;
	}
	return *this;
}

// Appends string or char to the buffer
CStringEx& CStringEx::Append(const CStringEx& szText)
{
	if (!szText.IsEmpty())
	{		
		AllocBuffer(m_nLength + szText.m_nLength);
		memcpy(&m_pszData[m_nLength], szText.m_pszData, sizeof(_TCHAR)*(szText.m_nLength + 1));
		m_nLength += szText.m_nLength;
	}
	return *this;
}

//allocates buffer for at least nSize characters
void CStringEx::AllocBuffer(int nSize)
{
	nSize++;					//add NULL terminate
	if (nSize > m_nSize)
	{
		//alignment to the nearest k*DEFAULT_CHARSTRING_BUFFER_MAXLEN
		m_nSize = (nSize + DEFAULT_CHARSTRING_BUFFER_MAXLEN - 1) & ~(DEFAULT_CHARSTRING_BUFFER_MAXLEN - 1);		
		m_pszData = (LPTSTR)realloc(m_pszData, m_nSize*sizeof(_TCHAR));
		
		BES_ASSERT(m_nSize >= nSize);
		BES_ASSERT(m_pszData != NULL);
	}
}

//Appends formatted data to an existing CStringEx object.
CStringEx& CStringEx::AppendFormat(LPCTSTR pszFormat, ...)
{
	va_list marker;
	va_start(marker, pszFormat);

	CStringEx szTmp;
	szTmp.FormatV(pszFormat, marker);
	Append(szTmp);
	
	va_end(marker);
	return *this;
}

//Appends formatted data to an existing CStringEx object.
//nFormatID = The string resource identifier that contains the format-control string.
CStringEx& CStringEx::AppendFormat(UINT nFormatID, ...)
{
	CStringEx szFormat;
	szFormat.LoadString(nFormatID);

	va_list marker;
	va_start(marker, nFormatID);

	CStringEx szTmp;
	szTmp.FormatV(szFormat, marker);
	Append(szTmp);
	
	va_end(marker);
	return *this;
}

//Writes formatted data to a CStringEx in the same way that sprintf formats data into a 
//C-style character array. nFormatID is the string resource identifier that contains the 
//format-control string.
void CStringEx::Format(LPCTSTR pszFormat, ...)
{
	va_list marker;
	va_start(marker, pszFormat);

	FormatV(pszFormat, marker);		
	va_end(marker);
}

void CStringEx::Format(UINT nFormatID, ...)
{
	CStringEx szFormat;
	szFormat.LoadString(nFormatID);

	va_list marker;
	va_start(marker, nFormatID);

	FormatV(szFormat, marker);		
	va_end(marker);
}

#pragma warning (push)
#pragma warning (disable: 4996)

//Formats a message string using a variable argument list.
void CStringEx::FormatV(LPCTSTR pszFormat, va_list args)
{
	BES_ASSERT(pszFormat != NULL);
	BES_ASSERT(args != NULL);
	
	int nSize = DEFAULT_CHARSTRING_BUFFER_MAXLEN;
	AllocBuffer(nSize);

	int nLen = 0;
	while ((nLen = _vsntprintf(m_pszData, m_nSize - 1, pszFormat, args)) < 0)
	{		
		nSize += DEFAULT_CHARSTRING_BUFFER_MAXLEN;
		AllocBuffer(nSize);
	}

	m_nLength = nLen;
}

#pragma warning(pop)

//Deletes a character or characters from a string starting with the character at the given index.
//Returns: The length of the changed string.
//If nCount is longer than the string, the remainder of the string will be removed.
int CStringEx::Delete(int iIndex, int nCount)
{
	BES_ASSERT(iIndex >= 0 && iIndex <= m_nLength);

	if (iIndex + nCount > m_nLength)
		nCount = m_nLength - iIndex;
	if (nCount > 0)
	{
		memmove(&m_pszData[iIndex], &m_pszData[iIndex + nCount], 
			(m_nLength - (iIndex + nCount) + 1)*sizeof(_TCHAR));
		m_nLength -= nCount;
	}

	return m_nLength;
}

//Searches this string for the first match of a substring.
int CStringEx::Find(LPCTSTR pszSub, int iStart) const
{
	BES_ASSERT(iStart >= 0 && iStart <= m_nLength);
	BES_ASSERT(pszSub != NULL);

	if (m_pszData == NULL)
		return -1;
	else
	{
		LPTSTR pszFound = _tcsstr(&m_pszData[iStart], pszSub);
		if (pszFound == NULL)
			return -1;

		return (int)(pszFound - m_pszData);
	}		
}

//Searches this string for the first match of a character.
int CStringEx::Find(_TCHAR ch, int iStart) const
{
	BES_ASSERT(iStart >= 0 && iStart <= m_nLength);
	
	if (m_pszData == NULL)
		return -1;
	else
	{
		LPTSTR pszFound = _tcschr(&m_pszData[iStart], ch);
		if (pszFound == NULL)
			return -1;

		return (int)(pszFound - m_pszData);
	}
}

//Finds the first matching character from a set pszSub.
int CStringEx::FindOneOf(LPCTSTR pszSub, int iStart)
{
	BES_ASSERT(iStart >= 0 && iStart <= m_nLength);
	
	if (m_pszData == NULL)
		return -1;
	
	int nRet = (int)_tcscspn(&m_pszData[iStart], pszSub);
	return (nRet == m_nLength ? -1 : nRet);
}

//Searches this CStringEx object for the last match of a character.
int CStringEx::ReverseFind(_TCHAR ch)
{
	if (m_pszData == NULL)
		return -1;
	else
	{
		LPTSTR pszFound = _tcsrchr(m_pszData, ch);
		if (pszFound == NULL)
			return -1;

		return (int)(pszFound - m_pszData);
	}
}

//Searches this CStringEx object for the last match of a substring.
int CStringEx::LastIndexOf(LPCTSTR lpszSub)
{
	BES_ASSERT(lpszSub != NULL);
	if (m_pszData == NULL)
		return -1;
	else
	{
		CStringEx szMask(lpszSub);
		CStringEx szThis(*this);
		
		szMask.MakeReverse();
		szThis.MakeReverse();
		int nResult = szThis.IndexOf(szMask);
		if (nResult >= 0)
			return m_nLength - nResult - 1;
		else
			return -1;
	}
}

//Inserts a single character at the given index within the string.
//returns the length of the changed string.
int CStringEx::Insert(int iIndex, _TCHAR ch)
{
	BES_ASSERT(iIndex >= 0 && iIndex <= m_nLength);

	AllocBuffer(m_nLength + 1);
	int nCount = m_nLength - iIndex + 1;	
	memmove(&m_pszData[iIndex], &m_pszData[iIndex + 1], nCount*sizeof(_TCHAR));
	m_pszData[iIndex] = ch;
	return m_nLength += 1;	
}

//Inserts a substring at the given index within the string.
//returns the length of the changed string.
int CStringEx::Insert(int iIndex, LPCTSTR psz)
{
	BES_ASSERT(iIndex >= 0 && iIndex <= m_nLength);
	BES_ASSERT(psz != NULL);

	int nLen = (int)_tcslen(psz);
	AllocBuffer(m_nLength + nLen);
	int nCount = m_nLength - iIndex + 1;	//+1 = term. zero
	memmove(&m_pszData[iIndex + nLen], &m_pszData[iIndex], nCount*sizeof(_TCHAR));
	memcpy(&m_pszData[iIndex], psz, nLen*sizeof(_TCHAR));
	return m_nLength += nLen;
}

//Replaces a substring with another. 
//Returns he number of replaced instances of the string
int CStringEx::Replace(LPCTSTR pszOld, LPCTSTR pszNew)
{
	BES_ASSERT(pszOld != NULL);
	BES_ASSERT(pszNew != NULL);

	if (m_pszData == NULL)
		return 0;
	
	int nLen = (int)_tcslen(pszOld);
	int nLen2 = (int)_tcslen(pszNew);

	int iPos = -1;
	int nRet = 0;
	while ((iPos = IndexOf(pszOld, iPos + 1)) >= 0)
	{
		Delete(iPos, nLen);
		Insert(iPos, pszNew);
		iPos += nLen2;
	}

	return nRet;
}

//Replaces a character with another. 
//Returns he number of replaced instances of the character
int CStringEx::Replace(_TCHAR chOld, _TCHAR chNew)
{
	if (m_pszData == NULL)
		return 0;
	
	LPTSTR pszData = m_pszData;
	
	int nRet = 0;
	while (*pszData != '\0')
	{
		if (*pszData == chOld)
		{
			*pszData = chNew;
			nRet++;
		}		
		pszData++;
	}
	return nRet;
}

//Loads string with nID from hInstance/hResource into the CStringEx object
BOOL CStringEx::LoadString(HINSTANCE hInstance, HRSRC hResource, UINT nID)
{
	HGLOBAL hGlobal = ::LoadResource(hInstance, hResource);
	if (hGlobal == NULL)
		return FALSE;

	BOOL bResult = FALSE;
	UINT nIndex = nID & 0xF;						//index into hResource
	BYTE* pData = (BYTE*)::LockResource(hGlobal);
	BYTE* pDataEnd = pData + ::SizeofResource(hInstance, hResource);
	while(nIndex > 0 && pData < pDataEnd)
	{
		//Each block has sixteen strings, each represented as an ordered pair (LENGTH, TEXT). 
		//LENGTH is a WORD that specifies the size, in terms of the number of characters, 
		//in the TEXT that follows. TEXT follows LENGTH and contains the string in Unicode 
		//without the NULL terminating character. There may be no characters in TEXT, 
		//in which case LENGTH is zero. 

		pData += sizeof(WORD) + (*((WORD*)pData))*sizeof(WCHAR);		
		nIndex--;
	}

	if (pData < pDataEnd)
	{
		AllocBuffer(m_nLength = (int)*((WORD*)pData));
		m_pszData[m_nLength] = _T('\0');

		if (m_nLength != 0)
		{
		#ifdef _UNICODE
			memcpy(m_pszData, ((WORD*)pData) + 1, m_nLength*sizeof(WCHAR));
		#else
			::WideCharToMultiByte(CP_ACP, WC_DEFAULTCHAR, (LPCWSTR)(((WORD*)pData) + 1), m_nLength,
				m_pszData, m_nSize, NULL, NULL);
		#endif
			
			bResult = TRUE;
		}
	}
	
	::FreeResource(hGlobal);
	return bResult;
}

#if defined(__BSGEN_PUREWIN) || defined(__BSGEN_NO_ATLMFC_STRINGS)
//Loads an existing CStringEx object from a Windows resource.
BOOL CStringEx::LoadString(UINT nID)
{	
	BOOL bResult = FALSE;
	DWORD dwNeeded = 0;
	EnumProcessModules(::GetCurrentProcess(), NULL, 0, &dwNeeded);

	int nModules = dwNeeded / sizeof(HMODULE);
	HMODULE* pModules = new HMODULE[nModules];
	if (!EnumProcessModules(::GetCurrentProcess(), pModules, dwNeeded, &dwNeeded))
		Empty();
	else
	{
		for (int i = 0; i < nModules; i++)
		{
			if (FALSE != (bResult = LoadString((HINSTANCE)pModules[i], nID)))
				break;	//we are ready
		}
	}		

	delete[] pModules;
	return bResult;
}
#endif

//Extracts a substring of length nCount characters from this CStringEx object, 
//starting at position iFirst (zero-based).
CStringEx CStringEx::Substring(int iPos, int nLen)
{
	BES_ASSERT(iPos >= 0  && iPos <= m_nLength);
	BES_ASSERT(nLen >= 0);	

	CStringEx szRet;
	if (m_pszData != NULL)
	{
		if (iPos + nLen > m_nLength)
			nLen = m_nLength - iPos;
	
		if (nLen > 0)
		{
			szRet.AllocBuffer(nLen);			
			szRet.m_nLength = nLen;
			szRet.m_pszData[nLen] = _T('\0');
			memcpy(szRet.m_pszData, &m_pszData[iPos], nLen * sizeof(_TCHAR));			
		}
	}

	return szRet;
}


//All characters in szChars will be trimmed from the beginning of string
CStringEx& CStringEx::TrimLeft(LPCTSTR szChars)
{
	if (IsEmpty() || szChars == NULL || *szChars == _T('\0'))
		return *this;

	int nIndex = 0;
	while (_tcschr(szChars, m_pszData[nIndex]) != NULL && nIndex <= m_nLength)
		nIndex++;

	if (nIndex > 0)
		Delete(0, nIndex);
	return *this;
}

//All characters in szChars will be trimmed from the end of string
CStringEx& CStringEx::TrimRight(LPCTSTR szChars)
{
	if (IsEmpty() || szChars == NULL || *szChars == _T('\0'))
		return *this;

	int nIndex = m_nLength - 1;
	while (_tcschr(szChars, m_pszData[nIndex]) != NULL && nIndex >= 0)
		nIndex--;

	if ((++nIndex) < m_nLength)
		Delete(nIndex, m_nLength);
	return *this;
}

//Returns a String array containing the substrings in this instance that are delimited 
//by elements of a specified single character or a string 
CArrayOfStrings* CStringEx::Split(_TCHAR ch, bool bRemoveEmptyEntries)
{
	_TCHAR szBuf[] = { ch, _T('\0') };
	return Split(szBuf, bRemoveEmptyEntries);
}

CArrayOfStrings* CStringEx::Split(LPCTSTR lpszDelim, bool bRemoveEmptyEntries)
{
	CArrayOfStrings* pArray = new CArrayOfStrings;
	int nDelimLen = (int)_tcslen(lpszDelim);
	int iStart = 0;
	int iPos;

	CStringEx szTmp;		
	while ((iPos = Find(lpszDelim, iStart)) > 0)
	{		
		szTmp = Mid(iStart, iPos - iStart);
		if (!szTmp.IsEmpty() || !bRemoveEmptyEntries)
			pArray->Add(szTmp);
		iStart = iPos + nDelimLen;
	}
	//and the rest
	szTmp = Mid(iStart, GetLength() - iStart);
	pArray->Add(szTmp);
	return pArray;
}