#ifndef BES_ARRAYSDEFS_H
#define BES_ARRAYSDEFS_H

#include "Array.h"
#include "String.h"

#if defined(__BSGEN_PUREWIN)	//otherwise it was defined in CString

class CArrayOfStrings : public CArray<CString>
{
public:
	inline void SetAt(int nIndex, LPCTSTR lpszData) { m_pData[nIndex] = lpszData; }	

	inline int Add(LPCTSTR elem) { CString s(elem); return CArray<CString>::Add(s); }	
	inline int Add(const CString& elem) { return CArray<CString>::Add(elem); }
	inline int Add(CString* elems, int nCount) { return CArray<CString>::Add(elems, nCount); }
	inline int Add(CArrayOfStrings& src) { return CArray<CString>::Add(src); }

	inline void Insert(int nIndex, LPCTSTR elem) { CString s(elem); CArray<CString>::Insert(nIndex, s); }	
	inline void Insert(int nIndex, CString& elem) { CArray<CString>::Insert(nIndex, elem); }
	inline void Insert(int nIndex, CString* elems, int nCount) { CArray<CString>::Insert(nIndex, elems, nCount); }
	inline void Insert(int nIndex, CArrayOfStrings& src)  { CArray<CString>::Insert(nIndex, src); }
};

#endif

#endif //BES_ARRAYSDEFS_H