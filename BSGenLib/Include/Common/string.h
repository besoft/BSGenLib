/* ${FILENAME}
** ${LASTMODIFIED}
**
** ${COPYRIGHT}
** ${LICENCE}
**
** DESCRIPTION: Wrapper of char*
*/

#ifndef BES_CCHARSTRING_H
#define BES_CCHARSTRING_H

#if defined(__BSGEN_PUREWIN)
#include <psapi.h>
#pragma comment(lib, "psapi.lib")

class CArrayOfStrings;		//forward declaration
#else
	#if defined(__BSGEN_MFC)
		#include <cstringt.h>
	#else
		#include <atlstr.h>
	#endif
	typedef CArray<CString, CStringElementTraits<CString> > CArrayOfStrings;
#endif

#define DEFAULT_CHARSTRING_BUFFER_MAXLEN		1024	//must be 2^k
class CStringEx
{
protected:
	LPTSTR m_pszData;
	int m_nSize;
	int m_nLength;
public:
#pragma region Constructors & destructor
	CStringEx() {
		m_pszData = NULL;
		m_nSize = m_nLength = 0;
	}

	CStringEx(LPCTSTR lpszText);
	CStringEx(const CStringEx& szText);

	~CStringEx() {
		free(m_pszData);
	}
#pragma endregion

#pragma region String buffer access - GETBUFFER, RELEASEBUFFER, operators (LPCTSTR), etc.
protected:
	//allocates buffer for at least nSize characters
	void AllocBuffer(int nSize);
public:
	inline LPTSTR GetBuffer(int nLength = DEFAULT_CHARSTRING_BUFFER_MAXLEN) {
		AllocBuffer(nLength);
		return m_pszData;
	}

	inline void ReleaseBuffer() {
		if (m_pszData != NULL)
			m_nLength = (int)_tcslen(m_pszData);
	}

	inline operator LPCTSTR() {
		return (LPCTSTR)m_pszData;
	}

	inline _TCHAR operator [] (int nIndex) const {
		BES_ASSERT(m_pszData != NULL && nIndex >= 0 && nIndex < m_nLength);
		return m_pszData[nIndex];
	}

	inline _TCHAR& operator [] (int nIndex) {
		BES_ASSERT(m_pszData != NULL && nIndex >= 0 && nIndex < m_nLength);
		return m_pszData[nIndex];
	}
#pragma endregion

#pragma region String buffer loading - EMPTY, SETNULL, operators =, LOADSTRING, etc.	
public:	
	//destroys the buffer (releases memory)
	inline void SetNull() {
		m_nSize = m_nLength = 0;
		free(m_pszData);
		m_pszData = NULL;
	}

	inline bool IsNull() const {
		return m_pszData == NULL;
	}

	inline void Empty() {
		if (m_pszData != NULL)
			m_pszData[m_nLength = 0] = _T('\0');
	}

	inline bool IsEmpty() const {
		return m_nLength == 0;
	}

	//Loads an existing CStringEx object from a Windows resource.	
	inline BOOL LoadString(HINSTANCE hInstance, UINT nID, WORD wLanguageID);
	inline BOOL LoadString(HINSTANCE hInstance, UINT nID);

#if defined(__BSGEN_MFC)
	inline	BOOL LoadString(UINT nID) {
		return LoadString(AfxFindStringResourceHandle(nID), nID);
	}
#elif defined(__BSGEN_ATL)
	inline	BOOL LoadString(UINT nID) {
		return LoadString(AtlFindStringResourceInstance(nID), nID);
	}
#else
	BOOL LoadString(UINT nID);	
#endif

	CStringEx& operator = (LPCTSTR lpszText);
	CStringEx& operator = (const CStringEx& szText);
protected:
	//Loads string with nID from hInstance/hResource into the CStringEx object
	BOOL LoadString(HINSTANCE hInstance, HRSRC hResource, UINT nID);
#pragma endregion

#pragma region String modifications - APPEND, INSERT, DELETE, operators +=
public:
	//Append - appends string or char to the buffer
	CStringEx& Append(_TCHAR ch);
	CStringEx& Append(LPCTSTR lpszText);	
	CStringEx& Append(const CStringEx& szText);

	//Inserts a single character or a substring at the given index within the string.
	//returns the length of the changed string.
	int Insert(int iIndex, LPCTSTR psz);
	int Insert(int iIndex, _TCHAR ch);

	//Replaces a character or substring with another.
	int Replace(LPCTSTR pszOld, LPCTSTR pszNew);
	int Replace(_TCHAR chOld, _TCHAR chNew);


	//Deletes a character or characters from a string starting with the character at the given index.
	//Returns: The length of the changed string.
	int Delete(int iIndex, int nCount = 1);

	inline CStringEx& operator += (_TCHAR ch) {
		return Append(ch);
	}

	inline CStringEx& operator += (LPCTSTR lpszStr) {
		return Append(lpszStr);
	}

	inline CStringEx& operator += (const CStringEx& szStr) {
		return Append(szStr);
	}

#pragma endregion

#pragma region String formatting - APPENDFORMAT, FORMAT, FORMATV, etc.
public:
	//AppendFormat
	CStringEx& AppendFormat(LPCTSTR pszFormat, ...);
	CStringEx& AppendFormat(UINT nFormatID, ...);

	//Writes formatted data to a CStringEx in the same way that sprintf formats data into a 
	//C-style character array. nFormatID is the string resource identifier that contains the 
	//format-control string.
	void Format(LPCTSTR pszFormat, ...);
	void Format(UINT nFormatID, ...);

	//Formats a message string using a variable argument list.
	void FormatV(LPCTSTR pszFormat,va_list args);
#pragma endregion

#pragma region Substrings - LEFT, MID, RIGHT, Substring etc.
public:
	//Extracts the leftmost nCount characters from this CStringEx object 
	//and returns a copy of the extracted substring
	inline CStringEx Left(int nLen) {
		return Substring(0, nLen);
	}

	//Extracts a substring of length nCount characters from this CStringEx object, 
	//starting at position iFirst (zero-based).
	inline CStringEx Mid(int iPos, int nLen) {
		return Substring(iPos, nLen);
	}

	//Extracts the rightmost nCount characters from this CStringEx object 
	//and returns a copy of the extracted substring
	inline CStringEx Right(int nLen) {
		if (nLen >= m_nLength)
			return m_pszData;
		else
			return Substring(m_nLength - nLen , nLen);
	}
	
	//Extracts a substring of length nCount characters from this CStringEx object, 
	//starting at position iFirst (zero-based). --- .NET compatible
	inline CStringEx Substring(int iPos) {
		return Substring(iPos, (int)(((UINT)-1) - 1));
	}

	//Extracts a substring of length nCount characters from this CStringEx object, 
	//starting at position iFirst (zero-based). --- .NET compatible
	CStringEx Substring(int iPos, int nLen);
#pragma endregion

#pragma region String trims - TRIMLEFT, TRIMRIGHT, etc.
public:
	//Trims all leading and trailing whitespace characters from the string.
	inline CStringEx& Trim() {
		return TrimLeft().TrimRight();
	}

	//Trims leading whitespace characters from the string.
	inline CStringEx& TrimLeft() {
		return TrimLeft(_T(" \t\r\n"));
	}

	//Trims trailing whitespace characters from the string.
	inline CStringEx& TrimRight() {
		return TrimRight(_T(" \t\r\n"));
	}

	inline CStringEx& Trim(_TCHAR ch) {
		_TCHAR szBuf[] = { ch, 0 };
		return TrimLeft(szBuf).TrimRight(szBuf);
	}

	//MFC compatible
	inline CStringEx& TrimLeft(_TCHAR ch) {
		_TCHAR szBuf[] = { ch, 0 };
		return TrimLeft(szBuf);
	}

	//MFC compatible
	inline CStringEx& TrimRight(_TCHAR ch){
		_TCHAR szBuf[] = { ch, 0 };
		return TrimRight(szBuf);
	}

	//All characters in szChars will be trimmed from the string
	inline CStringEx& Trim(LPCTSTR szChars) {
		return TrimLeft(szChars).TrimRight(szChars);
	}	

	//All characters in szChars will be trimmed from the beginning of string
	CStringEx& TrimLeft(LPCTSTR szChars);

	//All characters in szChars will be trimmed from the end of string
	CStringEx& TrimRight(LPCTSTR szChars);
	

	//.NET compatrible
	inline CStringEx TrimBoth() const {
		CStringEx szRet(m_pszData);
		return szRet.Trim();
	}

	inline CStringEx TrimStart() const {
		CStringEx szRet(m_pszData);
		return szRet.TrimLeft();
	}
	
	inline CStringEx TrimEnd() const {
		CStringEx szRet(m_pszData);
		return szRet.TrimRight();
	}

	inline CStringEx TrimBoth(_TCHAR ch) const {
		CStringEx szRet(m_pszData);
		return szRet.Trim(ch);
	}

	inline CStringEx TrimStart(_TCHAR ch) const  {
		CStringEx szRet(m_pszData);
		return szRet.TrimLeft(ch);
	}
	
	inline CStringEx TrimEnd(_TCHAR ch)  const {
		CStringEx szRet(m_pszData);
		return szRet.TrimRight(ch);
	}

	inline CStringEx TrimBoth(LPCTSTR lpszChars) const  {
		CStringEx szRet(m_pszData);
		return szRet.Trim(lpszChars);
	}

	inline CStringEx TrimStart(LPCTSTR lpszChars)  const {
		CStringEx szRet(m_pszData);
		return szRet.TrimLeft(lpszChars);
	}
	
	inline CStringEx TrimEnd(LPCTSTR lpszChars) const  {
		CStringEx szRet(m_pszData);
		return szRet.TrimRight(lpszChars);
	}

#pragma endregion

#pragma region String case conversions - MAKEUPPER, MAKELOWER, etc.
public:
#pragma warning (push)
#pragma warning (disable: 4996)		//_tcsupr and _tcslwr warnings
	//Converts the CStringEx object to a uppercase string.
	inline CStringEx& MakeUpper(){
		if (m_pszData != NULL)
			_tcsupr(m_pszData);
		return *this;
	}

	//Converts the CStringEx object to a lowercase string.
	inline CStringEx& MakeLower()	{
		if (m_pszData != NULL)
			_tcslwr(m_pszData);
		return *this;
	}
#pragma warning (pop)

	//.NET compatible
	inline CStringEx ToLower() const {
		CStringEx szRet(*this);
		szRet.MakeLower();
		return szRet;
	}

	//.NET compatible
	inline CStringEx ToUpper()  const {
		CStringEx szRet(*this);
		szRet.MakeUpper();
		return szRet;
	}
#pragma endregion

#pragma region String comparisons - COMPARE, COMPARENOCASE, operators ==, <, > ,<=, >=, etc.
public:
	//Compares two strings (case sensitive).
	inline int Compare(LPCTSTR psz) const {
		if (m_pszData == psz)
			return 0;
		else if (m_pszData == NULL)
			return -1;
		else if (psz == NULL)
			return 1;
		else
			return _tcscmp(m_pszData, psz);
	}

	//Compares two strings (case sensitive).
	inline int CompareNoCase(LPCTSTR psz) const {
		if (m_pszData == psz)
			return 0;
		else if (m_pszData == NULL)
			return -1;
		else if (psz == NULL)
			return 1;
		else
			return _tcsicmp(m_pszData, psz);
	}

#pragma region .NET Compatible Comparisions
	inline int CompareTo(LPCTSTR lpszText)
	{
		return Compare(lpszText);
	}

	inline int CompareTo(CStringEx& szObj)
	{
		return Compare(szObj.m_pszData);
	}

	inline int CompareTo(LPCTSTR lpszText, bool bIgnoreCase)
	{
		if (bIgnoreCase)
			return CompareNoCase(lpszText);
		return Compare(lpszText);
	}

	inline int CompareTo(CStringEx& szObj, bool bIgnoreCase)
	{
		if (bIgnoreCase)
			return CompareNoCase(szObj.m_pszData);
		return Compare(szObj.m_pszData);
	}
#pragma endregion
	friend bool operator == (CStringEx& str1, LPCTSTR str2);
	friend bool operator <= (CStringEx& str1, LPCTSTR str2);
	friend bool operator >= (CStringEx& str1, LPCTSTR str2);
	friend bool operator < (CStringEx& str1, LPCTSTR str2);
	friend bool operator > (CStringEx& str1, LPCTSTR str2);
	friend bool operator == (LPCTSTR str1, CStringEx& str2);
	friend bool operator <= (LPCTSTR str1, CStringEx& str2);
	friend bool operator >= (LPCTSTR str1, CStringEx& str2);
	friend bool operator < (LPCTSTR str1, CStringEx& str2);
	friend bool operator > (LPCTSTR str1, CStringEx& str2);
	friend bool operator == (CStringEx& str1, CStringEx& str2);
	friend bool operator <= (CStringEx& str1, CStringEx& str2);
	friend bool operator >= (CStringEx& str1, CStringEx& str2);
	friend bool operator < (CStringEx& str1, CStringEx& str2);
	friend bool operator > (CStringEx& str1, CStringEx& str2);
#pragma endregion

#pragma region String searching - INDEXOF, LASTINDEXOF, FIND, REVERSEFIND, etc.
public:
	//Searches this string for the first match of a substring.
	int Find(LPCTSTR pszSub, int iStart = 0) const;
	inline int IndexOf(LPCTSTR pszSub, int iStart = 0) { return Find(pszSub, iStart); }
		

	//Searches this string for the first match of a character.
	int Find(_TCHAR ch, int iStart = 0) const;	
	inline int IndexOf(_TCHAR ch, int iStart = 0) { return Find(ch, iStart); }

	//Finds the first matching character from a set pszSub.
	int FindOneOf(LPCTSTR pszSub, int iStart = 0);
 
	//Searches this CStringEx object for the last match of a character.
	int ReverseFind(_TCHAR ch);
	inline int LastIndexOf(_TCHAR ch) { return ReverseFind(ch); }
	
	//Searches this CStringEx object for the last match of a substring.
	int LastIndexOf(LPCTSTR lpszSub);

	inline bool StartsWith(LPCTSTR lpszSub) { return IndexOf(lpszSub) == 0; }
	inline bool EndsWith(LPCTSTR lpszSub) { return m_nLength != 0 && LastIndexOf(lpszSub) == m_nLength - 1; }
#pragma endregion

#pragma region Other operators - operators +
	friend CStringEx operator + (const CStringEx& str1, const CStringEx& str2);
	friend CStringEx operator + (const CStringEx& str1, LPCTSTR str2);
	friend CStringEx operator + (LPCTSTR str1, const CStringEx& str2);	
	friend CStringEx operator + (_TCHAR ch, const CStringEx& str2);
	friend CStringEx operator + (const CStringEx& str1, _TCHAR str2);
#pragma endregion

#pragma region Other string routines - SPLIT, GETLENGTH, MAKEREVERSE, READLINE, WRITELINE, etc.
public:
	//Returns a String array containing the substrings in this instance that are delimited 
	//by elements of a specified single character or a string 
	CArrayOfStrings* Split(_TCHAR ch, bool bRemoveEmptyEntries = false);
	CArrayOfStrings* Split(LPCTSTR lpszDelim, bool bRemoveEmptyEntries = false);

	inline int GetLength() const { return m_nLength; }	

	//Reverses the string.
	inline void MakeReverse() {	
		_tcsrev(m_pszData);
	}

	inline CStringEx Clone() const
	{
		CStringEx szRet(m_pszData);
		return szRet;
	}

	inline operator CStringEx&() {
		return *this;
	}

	inline bool WriteLine(FILE* f) {
		if (m_pszData == NULL)
			return false;
		return _ftprintf(f, _T("%s\n"), m_pszData) == m_nLength;
	}

	//reads next line from the given text file into the CStringEx object
	//newline characters are automatically trimmed,
	//returns number of read characters, -1 if we are at EOF
	inline int ReadLine(FILE* f)
	{
		_TCHAR* pResult = _fgetts(GetBuffer(DEFAULT_CHARSTRING_BUFFER_MAXLEN), 
			DEFAULT_CHARSTRING_BUFFER_MAXLEN, f);			
		ReleaseBuffer();

		if (pResult == NULL)
			return -1;
				
		if (m_pszData[m_nLength - 1] == _T('\n')) {
			m_pszData[m_nLength - 1] = _T('\0');
			m_nLength--;
		}

		if (m_pszData[m_nLength - 1] == _T('\r')) {
			m_pszData[m_nLength - 1] = _T('\0');
			m_nLength--;
		}

		return m_nLength;
	}

	inline int ReadTrimmedLine(FILE* f) 
	{
		if (ReadLine(f) < 0)
			return -1;

		Trim();
		return m_nLength;
	}
#pragma endregion
};

#pragma region Inlines
#pragma region LOADSTRING
//Loads an existing CStringEx object from a Windows resource.
//Written according to the description at http://support.microsoft.com/kb/q196774/
inline BOOL CStringEx::LoadString(HINSTANCE hInstance, UINT nID, WORD wLanguageID)
{
	Empty();
	//String resources are stored in a Win32 executable as blocks of strings. 
	//Each block can have up to sixteen strings and represents the smallest granularity 
	//of string resource that can be loaded
	HRSRC hResource = ::FindResourceEx((HMODULE)hInstance, MAKEINTRESOURCE((nID >> 4) + 1), RT_STRING, wLanguageID);
	if (hResource == NULL)
		return FALSE;
	
	return LoadString(hInstance, hResource, nID);

}

//Loads an existing CStringEx object from a Windows resource.
//Written according to the description at http://support.microsoft.com/kb/q196774/
inline BOOL CStringEx::LoadString(HINSTANCE hInstance, UINT nID)
{
	Empty();
	//String resources are stored in a Win32 executable as blocks of strings. 
	//Each block can have up to sixteen strings and represents the smallest granularity 
	//of string resource that can be loaded
	HRSRC hResource = ::FindResource((HMODULE)hInstance, MAKEINTRESOURCE((nID >> 4) + 1), RT_STRING);
	if (hResource == NULL)
		return FALSE;
	
	return LoadString(hInstance, hResource, nID);
}

#pragma endregion

#pragma region operators ==, <, > ,<=, >=, etc.
inline bool operator == (CStringEx& str1, LPCTSTR str2) {
	return str1.CompareTo(str2) == 0;
}
inline bool operator <= (CStringEx& str1, LPCTSTR str2) {
	return str1.CompareTo(str2) <= 0;
}
inline bool operator >= (CStringEx& str1, LPCTSTR str2) {
	return str1.CompareTo(str2) >= 0;
}
inline bool operator < (CStringEx& str1, LPCTSTR str2) {
	return str1.CompareTo(str2) < 0;
}
inline bool operator > (CStringEx& str1, LPCTSTR str2) {
	return str1.CompareTo(str2) > 0;
}
inline bool operator == (LPCTSTR str1, CStringEx& str2) {
	return str2.CompareTo(str1) == 0;
}
inline bool operator <= (LPCTSTR str1, CStringEx& str2) {
	return str2.CompareTo(str1) >= 0;
}
inline bool operator >= (LPCTSTR str1, CStringEx& str2) {
	return str2.CompareTo(str1) <= 0;
}
inline bool operator < (LPCTSTR str1, CStringEx& str2) {
	return str2.CompareTo(str1) > 0;
}
inline bool operator > (LPCTSTR str1, CStringEx& str2) {
	return str2.CompareTo(str1) < 0;
}
inline bool operator == (CStringEx& str1, CStringEx& str2) {
	return str1.CompareTo(str2) == 0;
}
inline bool operator <= (CStringEx& str1, CStringEx& str2) {
	return str1.CompareTo(str2) <= 0;
}
inline bool operator >= (CStringEx& str1, CStringEx& str2) {
	return str1.CompareTo(str2) >= 0;
}
inline bool operator < (CStringEx& str1, CStringEx& str2) {
	return str1.CompareTo(str2) < 0;
}
inline bool operator > (CStringEx& str1, CStringEx& str2) {
	return str1.CompareTo(str2) > 0;
}
#pragma endregion

#pragma region operators +
inline CStringEx operator + (const CStringEx& str1, const CStringEx& str2) {
	CStringEx szRet(str1);
	return szRet.Append(str2);
}

inline CStringEx operator + (LPCTSTR str1, const CStringEx& str2) {
	CStringEx szRet(str1);
	return szRet.Append(str2);
}

inline CStringEx operator + (const CStringEx& str1, LPCTSTR str2) {
	CStringEx szRet(str1);
	return szRet.Append(str2);
}

inline CStringEx operator + (_TCHAR ch, const CStringEx& str2) {
	_TCHAR szBuf[] = {ch, _T('\0') };
	CStringEx szRet(szBuf);
	return szRet.Append(str2);
}

inline CStringEx operator + (const CStringEx& str1, _TCHAR str2){
	CStringEx szRet(str1);
	return szRet.Append(str2);
}
#pragma endregion
#pragma endregion

#if defined(__BSGEN_PUREWIN)
typedef CStringEx CString;
#endif
#endif //BES_CCHARSTRING_H