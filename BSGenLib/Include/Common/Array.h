/* ${FILENAME}
** ${LASTMODIFIED}
**
** ${COPYRIGHT}
** ${LICENCE}
**
** DESCRIPTION: Template for dynamic arrays
*/

#ifndef BES_ARRAY_H
#define BES_ARRAY_H

#ifndef __BSGEN_PUREWIN
#define CArray CAtlArray
#pragma message("#define CArray CAtlArray")
#else

#define DEFAULT_ARR_GROWBY	1024

#pragma deprecated(CArray)

template<typename TYPE, class ARG_TYPE = const TYPE& >
class CArray
{
public:
	CArray(int nGrowBy = DEFAULT_ARR_GROWBY)
	{
		BES_ASSERT(nGrowBy > 0);

		m_pData = NULL;
		m_nSize = m_nMaxSize = 0;
		m_nGrowBy = nGrowBy;
	}
	
	CArray(CArray& src, int nGrowBy = DEFAULT_ARR_GROWBY)
	{
		BES_ASSERT(nGrowBy > 0);

		m_pData = NULL;
		m_nSize = m_nMaxSize = 0;
		m_nGrowBy = nGrowBy;
		
		Add(src);
	}

	virtual ~CArray()
	{
		Empty();
	}
	
	inline int GetSize() { return m_nSize; }	
	inline int GetCount() { return m_nSize; }
	inline bool IsEmpty(){ return m_nSize == 0; }	
	inline TYPE* GetData(){ return m_pData; }
	inline TYPE GetAt(int nIndex) {return m_pData[nIndex];}
	inline ARG_TYPE operator[](int nIndex) {return m_pData[nIndex];}
	inline void SetAt(int nIndex, TYPE nElem) { m_pData[nIndex] = nElem; }		
	
	inline void Empty()
	{
		delete[] m_pData;
		m_pData = NULL;
		m_nMaxSize = m_nSize = 0;
	}

	inline void RemoveAll() {
		Empty();
	}

	int Add(ARG_TYPE elem);	
	int Add(TYPE* elems, int nCount);
	int Add(CArray& src);
	inline int Append(CArray& src) {
		return Add(src);
	}

	void Insert(int nIndex, ARG_TYPE elem);
	void Insert(int nIndex, TYPE* elems, int nCount);
	void Insert(int nIndex, CArray& src);
	
	void Delete(int nIndex, int nCount = 1);
	inline void RemoveAt(int nIndex, int nCount = 1) {
		Delete(nIndex, nCount);
	}
	int Compare(CArray& cmpar2);
//OPERATORS
	CArray<TYPE, ARG_TYPE>& operator = (CArray<TYPE, ARG_TYPE>& src);
	CArray<TYPE, ARG_TYPE>& operator = (ARG_TYPE elem);	
	CArray<TYPE, ARG_TYPE>& operator += (CArray& src);
	CArray<TYPE, ARG_TYPE>& operator += (ARG_TYPE src);


protected:
	void SetSize(int nNewSize);
	void CopyElements(TYPE* pDest, TYPE* pSrc, int nCount);	
	
// attributes
protected:
	TYPE* m_pData;
	int m_nSize;     // elements
	int m_nMaxSize;  // max allocated
	int m_nGrowBy;   // grow amount
};

template<typename TYPE, class ARG_TYPE>
inline CArray<TYPE, ARG_TYPE>& CArray<TYPE, ARG_TYPE>::operator += (CArray& src)
{
	Add(src);
	return *this;
}

template<typename TYPE, class ARG_TYPE>
inline CArray<TYPE, ARG_TYPE>& CArray<TYPE, ARG_TYPE>::operator += (ARG_TYPE src)
{
	Add(src);
	return *this;
}

template<typename TYPE, class ARG_TYPE>
inline CArray<TYPE, ARG_TYPE> operator + (CArray<TYPE, ARG_TYPE>& src1, CArray<TYPE, ARG_TYPE>& src2)
{
	CArray<TYPE, ARG_TYPE> ret(src1);
	ret.Add(src2);
	return ret;
}

template<typename TYPE, class ARG_TYPE>
inline CArray<TYPE, ARG_TYPE> operator + (CArray<TYPE, ARG_TYPE>& src1, ARG_TYPE src2)
{
	CArray<TYPE, ARG_TYPE> ret(src1);
	ret.Add(src2);
	return ret;
}

template<typename TYPE, class ARG_TYPE>
inline CArray<TYPE, ARG_TYPE> operator + (ARG_TYPE src1, CArray<TYPE, ARG_TYPE>& src2)
{
	CArray ret;
	ret.Add(src1);
	ret.Add(src2);
	return ret;
}

template<typename TYPE, class ARG_TYPE>
inline CArray<TYPE, ARG_TYPE>& CArray<TYPE, ARG_TYPE>::operator = (CArray& src)
{
	Empty();
	Add(src);
	return *this;
}

template<typename TYPE, class ARG_TYPE>
inline CArray<TYPE, ARG_TYPE>& CArray<TYPE, ARG_TYPE>::operator = (ARG_TYPE elem)
{
	Empty();
	Add(elem);
	return *this;
}

template<typename TYPE, class ARG_TYPE>
inline bool operator < (CArray<TYPE, ARG_TYPE>& src1, CArray<TYPE, ARG_TYPE>& src2)
{
	return src1.Compare(src2) < 0;
}

template<typename TYPE, class ARG_TYPE>
inline bool operator > (CArray<TYPE, ARG_TYPE>& src1, CArray<TYPE, ARG_TYPE>& src2)
{
	return src1.Compare(src2) > 0;
}

template<typename TYPE, class ARG_TYPE>
inline bool operator <= (CArray<TYPE, ARG_TYPE>& src1, CArray<TYPE, ARG_TYPE>& src2)
{
	return src1.Compare(src2) <= 0;
}

template<typename TYPE, class ARG_TYPE>
inline bool operator >= (CArray<TYPE, ARG_TYPE>& src1, CArray<TYPE, ARG_TYPE>& src2)
{
	return src1.Compare(src2) >= 0;
}

template<typename TYPE, class ARG_TYPE>
inline bool operator == (CArray<TYPE, ARG_TYPE>& src1, CArray<TYPE, ARG_TYPE>& src2)
{
	return src1.Compare(src2) == 0;
}

template<typename TYPE, class ARG_TYPE>
inline bool operator != (CArray<TYPE, ARG_TYPE>& src1, CArray<TYPE, ARG_TYPE>& src2)
{
	return src1.Compare(src2) != 0;
}

template<typename TYPE, class ARG_TYPE>
inline int CArray<TYPE, ARG_TYPE>::Add(ARG_TYPE elem)
{	
	int nIndex = m_nSize;
	if ((++m_nSize) > m_nMaxSize)
		SetSize(m_nSize);
	m_pData[nIndex] = elem;
	return nIndex;
}

template<typename TYPE, class ARG_TYPE>
inline int CArray<TYPE, ARG_TYPE>::Add(TYPE* elems, int nCount)
{	
	int nIndex = m_nSize;
	m_nSize += nCount;
	if (m_nSize > m_nMaxSize)
		SetSize(m_nSize);
	CopyElements(m_pData + nIndex, elems, nCount);
	return nIndex;
}

template<typename TYPE, class ARG_TYPE>
inline int CArray<TYPE, ARG_TYPE>::Add(CArray& src)
{	
	return Add(src.m_pData, src.m_nSize);	
}

template<typename TYPE, class ARG_TYPE>
inline void CArray<TYPE, ARG_TYPE>::Insert(int nIndex, ARG_TYPE elem)
{
	int nOldSize = m_nSize;
	if ((++m_nSize) > m_nMaxSize)
		SetSize(m_nSize);
	CopyElements(m_pData + nIndex + 1, m_pData + nIndex, nOldSize - nIndex);
	m_pData[nIndex] = elem;
}

template<typename TYPE, class ARG_TYPE>
void CArray<TYPE, ARG_TYPE>::Insert(int nIndex, TYPE* elems, int nCount)
{
	int nOldSize = m_nSize;
	m_nSize += nCount;
	if (m_nSize > m_nMaxSize)
		SetSize(m_nSize);
	CopyElements(m_pData + nIndex + nCount, m_pData + nIndex, nOldSize - nIndex);
	CopyElements(m_pData + nIndex, elems, nCount);
}

template<typename TYPE, class ARG_TYPE>
void CArray<TYPE, ARG_TYPE>::Insert(int nIndex, CArray& src)
{
	Insert(nIndex, src.m_pData, src.m_nSize);
}


template<typename TYPE, class ARG_TYPE>
void CArray<TYPE, ARG_TYPE>::Delete(int nIndex, int nCount)
{
	if (nIndex + nCount > m_nSize)
		nCount = m_nSize - nIndex;

	CopyElements(m_pData + nIndex, m_pData + nIndex + nCount, m_nSize - nCount - nIndex);
	SetSize(m_nSize - nCount);
}

template<typename TYPE, class ARG_TYPE>
void CArray<TYPE, ARG_TYPE>::SetSize(int nNewSize)
{
	if (nNewSize == 0)
	{		
		m_nSize = m_nMaxSize = 0;
		delete[] m_pData;
		m_pData = NULL;
		return;
	}
	
//we are increasing or decresing only by multiplies of m_nGrowBy constant
	int nNewMaxSize = m_nMaxSize;
	if (nNewSize > m_nMaxSize)
		nNewMaxSize += m_nGrowBy*((nNewSize / m_nGrowBy) + (0 != (nNewSize % m_nGrowBy))); 
	else if (nNewSize < m_nMaxSize)
		nNewMaxSize -= m_nGrowBy*((m_nMaxSize - nNewSize) / m_nGrowBy); 

	if (nNewMaxSize != m_nMaxSize)
	{	
		//need to alloc new block		
		TYPE* pData = new TYPE[nNewMaxSize];
		
		if (m_pData != NULL)
		{
			CopyElements(pData, m_pData, (m_nMaxSize > nNewMaxSize ? nNewMaxSize : m_nMaxSize));
			delete[] m_pData;			
		}

		m_pData = pData;		
		m_nMaxSize = nNewMaxSize;
	}	

	m_nSize = nNewSize;
}

template<typename TYPE, class ARG_TYPE>
int CArray<TYPE, ARG_TYPE>::Compare(CArray<TYPE, ARG_TYPE>& cmpar2)
{	
	TYPE* pSrc1 = m_pData;
	int nCount1 = m_nSize;
	TYPE* pSrc2 = cmpar2.m_pData;
	int nCount2 = cmpar2.m_nSize;

	int nCount = nCount1;
	if (nCount2 < nCount) 
		nCount = nCount2;
	//now check nCount elements
	while (nCount)
	{
		if (!(*pSrc1 == *pSrc2))		
		{
			if (*pSrc1 < *pSrc2)
				return -1;
			else
				return 1;
			break;
		}

		pSrc1++;
		pSrc2++;
		nCount--;
	}

	//first nCount elements were the same
	return (nCount1 - nCount2);
}

template<typename TYPE, class ARG_TYPE>
inline void CArray<TYPE, ARG_TYPE>::CopyElements(TYPE* pDest, TYPE* pSrc, int nCount)
{
	while (nCount--)
		*pDest++ = *pSrc++;
}
#endif

#endif		//BES_ARRAY_H