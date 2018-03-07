#pragma once
#include "RUList.h"

#define LARGEARRAY_BUFFER_FAILMINSIZE		64*1024			//the buffer size does no drop below 64 KB
#define LARGEARRAY_BUFFER_DEFAULTSIZE		64*1024*1024	//by the default the buffer is long 64MB
#define LARGEARRAY_PAGESIZE_DEFAULTSIZE		4096			//4096 KB

//CLargeArray class supports the storage of arrays that are larger than is the amount 
//of available physical memory and even larger than is the addressing ability of 32-bits computer.
//Recently accessed data is stored in the local memory while least recently used data is stored in 
//a swap file (note: if the array is small enough, the file is empty). Elements are accessed 
//through smart indexer operator.
template < typename TYPE, typename ARG_TYPE = const TYPE&, typename TI = ULONGLONG >
class CLargeArray
{	
protected:
	typedef struct DATA_BLOCK_ITEM
	{
		ULONGLONG ulFileOffset;	//offset to the file

		TYPE* pPage;			//page data	
		UINT nSize;				//size of page
		bool bDirty;			//true if the data is written but not commited
	} DATA_BLOCK_ITEM;

protected:	
	bool m_bReadOnly;			//read-only protection

	//FILE
	CString m_szPathName;		//path name
	HANDLE m_hFile;				//handle to the opened file
	ULONGLONG m_ulStartOffset;	//offset to the first byte
	TI m_nFileSize;				//current filesize
	bool m_bAttachedFile;		//attached file cannot be deleted	

	//BUFFER	
	TYPE* m_pBuffer;			//buffer for our data
	UINT m_nBufferSize;			//the total size of buffer
	UINT m_nBufferLength;		//the current occupied size of buffer
	int m_nDirtyPages;			//at least one page is dirty

	//MAPPINGS
	CRUList<UINT, DATA_BLOCK_ITEM>	m_mapPages;	//LRU ordered list of pages	
	UINT m_nPageSize;				//length of page in elements	
	UINT m_nLastPage;				//last accessed page
	DATA_BLOCK_ITEM* m_pLastPage;	//last accessed page	
	
	UINT m_nShortPage;				//last page in the file may be shorter
	UINT m_nShortPageSize;			//size of this page


//construction & destruction
public:	
	CLargeArray() : m_hFile(INVALID_HANDLE_VALUE), m_pBuffer(NULL), 
		m_nDirtyPages(0), m_nShortPage(-1)
	{
		m_bAttachedFile = false;		
	}
	
	~CLargeArray() {
		Delete();
	}
	
public:
	//Construction of a new large array upon existing part of file.
	//If bAttachOnly is false, file will be deleted in dtor.
	//If ulEndPos == 0 the all data starting at ulStartPos and ending at EOF are taken in account.
	//If nBufferSize == 0 the size of buffer is automatically chosen appropriately to the amount of data
	//but not exceeding LARGEARRAY_BUFFER_DEFAULTSIZE constant
	//If there is not nBufferSize bytes available, the first suitable lower size is chosen.
	void New(LPCTSTR lpszSwapFile, bool bAttachOnly = true,
		ULONGLONG ulStartPos = 0, ULONGLONG ulEndPos = 0,		
		UINT nBufferSize = 0, UINT nPageSize = 0);

	//Construction of a new large array 
	//If nBufferSize == 0 the size of buffer is automatically chosen appropriately to the amount of data
	//but not exceeding LARGEARRAY_BUFFER_DEFAULTSIZE constant
	//If there is not nBufferSize bytes available, the first suitable lower size is chosen.
	void New(TI nSize, UINT nBufferSize = 0, LPCTSTR lpszSwapFile = NULL, UINT nPageSize = 0);		

	//Destroys the array
	void Delete();

	//Flushes all data into the file, if file doesn't exist, it is created
	void Flush();

	//Changes the size of an array to the specified new size. 
	//If bResizeBuffer is true and the array is being enlarged, the buffer is also enlarged
	//Throws an exception, if it fails.
	void Resize(TI nNewSize, bool bResizeBuffer = true);

	//Changes the size of underlaying buffer to the specified new size. 
	//Throws an exception, if an error occurs.
	void ResizeBuffer(UINT nNewBufferSize);

	//returns the current size
	inline TI GetSize() { return m_nFileSize; }

	//returns the current buffer size
	inline UINT GetBufferSize() { return m_nBufferSize; }

	//sets/clears the read-only protection (default is off)
	inline void SetReadOnly(bool bReadOnly = true) { m_bReadOnly = bReadOnly; }	

	//gets one byte at nIndex	
	ARG_TYPE GetAt(TI nIndex, bool bSetDirty = false);

	//sets one bytes starting at nIndex	
	void SetAt(TI nIndex, ARG_TYPE nValue);
	
	//operators
	inline const TYPE operator [] (TI nIndex) const;
	inline TYPE& operator [] (TI nIndex);
	inline bool operator == (int Null);
	
	//gets one or more elements starting at nIndex
	//throws an exception if data cannot be read
	void GetData(TI nIndex, TYPE* lpData, UINT nCount);

	//sets one or more bytes starting at nIndex
	//throws an exception if data cannot be written
	void SetData(TI nIndex, TYPE* lpData, UINT nCount);

	//memset
	inline void Fill(ARG_TYPE nValue) {
		Fill(0, nValue, GetSize());
	}

	//memset
	void Fill(TI nIndex, ARG_TYPE nValue, TI nCount);

	//copies data into another LargeArray
	inline void CopyTo(CLargeArray& laDest){
		CopyTo(0, GetSize(), laDest, 0);
	}

	//copies data into another LargeArray
	inline void CopyTo(TI nIndex, TI nCount, CLargeArray& laDest) {
		CopyTo(nIndex, nCount, laDest, 0);
	}

	//copies data into another LargeArray laDest (must be large enough)
	void CopyTo(TI nIndex, TI nCount, CLargeArray& laDest, TI nDestIndex);

protected:
	//creates a new file / opens an existing file with name specified in lpszPathName
	//if no pathname is specified, temporary pathname is created
	void OpenFile(LPCTSTR lpszPathName, bool bOpenExisting = false);

	//constructs an empty buffer for the large array, throws an exception if something goes wrong
	void CreateBuffer(UINT nBufferSize, UINT nPageSize);

	//reads page data from the underlaying file stream
	void ReadPage(DATA_BLOCK_ITEM* pItem);

	//flushes page data into the file stream
	void WritePage(DATA_BLOCK_ITEM* pItem);

	//splits the given ulFileOffset into the page nFilePage and the offset nPageOffset in this page
	inline void MapFileOffsetToFilePage(TI ulFileOffset, UINT& nFilePage, UINT& nPageOffset)
	{
		nFilePage = (UINT)(ulFileOffset / m_nPageSize);
		nPageOffset = (UINT)(ulFileOffset % m_nPageSize);
	}

	//maps the given FilePage into memory block, returns NULL + false if the page is not cached
	inline bool MapFilePageToMemoryPage(UINT nFilePage, DATA_BLOCK_ITEM** pMemPage) {
		return (*pMemPage = m_mapPages.PLookup(nFilePage)) != NULL;
	}

	//get an empty memory page to be used for following read or write operations, 
	//if the buffer is not fully occupied, it is created, otherwise some page has to be swapped
	DATA_BLOCK_ITEM* GetPage(UINT nFilePage);

	//gets memory page and reads the data
	inline DATA_BLOCK_ITEM* GetPage(UINT nFilePage, bool bReadPage);

	//gets min(nCount, available bytes)
	inline UINT GetValidLength(DATA_BLOCK_ITEM* pMemPage, UINT nOffset, TI nCount);

	//aligns the given nValue to the nearest larger k*nPageSize value
	inline UINT PageAlign(UINT nValue, UINT nPageSize) {
		BES_ASSERT(nPageSize != 0 && nValue != 0);

		if (nValue % nPageSize == 0)
			return nValue;
		else 
			return ((nValue / nPageSize) + 1)*nPageSize;
	}
};

//Construction of a new large array upon existing part of file.
//If bAttachOnly is false, file will be deleted in dtor.
//If ulEndPos == 0 the all data starting at ulStartPos and ending at EOF are taken in account.
//If nBufferSize == 0 the size of buffer is automatically chosen appropriately to the amount of data
//but not exceeding LARGEARRAY_BUFFER_DEFAULTSIZE constant
//If there is not nBufferSize bytes available, the first suitable lower size is chosen.
template < typename TYPE, typename ARG_TYPE, typename TI >
void CLargeArray< TYPE, ARG_TYPE, TI >::New(LPCTSTR lpszSwapFile, bool bAttachOnly,
		ULONGLONG ulStartPos, ULONGLONG ulEndPos,
		UINT nBufferSize, UINT nPageSize)
{
	//destroy the previously constructed array
	Delete();	

	//open the file
	OpenFile(lpszSwapFile, true);
	m_bAttachedFile = bAttachOnly;

	try
	{
		//determine the size of array
		m_ulStartOffset = ulStartPos;
		if (ulEndPos != 0)
			m_nFileSize = (TI)((ulEndPos - m_ulStartOffset + 1) / sizeof(TYPE));
		else
		{
			LARGE_INTEGER liSize;
			if (!::GetFileSizeEx(m_hFile, &liSize))
				_com_issue_error(HRESULT_FROM_WIN32(::GetLastError()));

			m_nFileSize = (TI)((liSize.QuadPart - m_ulStartOffset) / sizeof(TYPE));
		}		

		//construct an empty buffer for the large array
		CreateBuffer(nBufferSize, nPageSize);		
	}
	catch(_com_error& e)
	{
		Delete();

		//raise the error
		_com_issue_error(e.Error());
	}
}

//Construction of a new large array 
//If nBufferSize == 0 the size of buffer is automatically chosen appropriately to the amount of data
//but not exceeding LARGEARRAY_BUFFER_DEFAULTSIZE constant
//If there is not nBufferSize bytes available, the first suitable lower size is chosen.
template < typename TYPE, typename ARG_TYPE, typename TI >
void CLargeArray< TYPE, ARG_TYPE, TI >::New(TI nSize, UINT nBufferSize, LPCTSTR lpszSwapFile, UINT nPageSize)
{
	//destroy the previously constructed array
	Delete();	

	//determine the size of array
	m_ulStartOffset = 0;
	m_nFileSize = nSize;
	m_bAttachedFile = false;
	
	try
	{
		//construct an empty buffer for the large array
		CreateBuffer(nBufferSize, nPageSize);

		//if the array doesn't fit the memory, open the file	
		if (m_nFileSize > m_nBufferSize)
			OpenFile(lpszSwapFile, false);
		else if (lpszSwapFile != NULL)
			m_szPathName = Path::GetFullPath(lpszSwapFile);
	}
	catch(_com_error& e)
	{
		Delete();

		//raise the error
		_com_issue_error(e.Error());
	}
}

//Destroys the array
template < typename TYPE, typename ARG_TYPE, typename TI >
void CLargeArray< TYPE, ARG_TYPE, TI >::Delete()
{
	if (m_bAttachedFile && m_nDirtyPages > 0)
		Flush();
	
	m_mapPages.RemoveAll();

	free(m_pBuffer);
	m_pBuffer = NULL;
	
	CloseHandle(m_hFile);
	m_hFile = INVALID_HANDLE_VALUE;

	if (!m_szPathName.IsEmpty() && m_bAttachedFile == false)
		DeleteFile(m_szPathName);

	m_szPathName.Empty();
}

//creates a new file / opens an existing file with name specified in lpszPathName
//if no pathname is specified, temporary pathname is created
template < typename TYPE, typename ARG_TYPE, typename TI >
void CLargeArray< TYPE, ARG_TYPE, TI >::OpenFile(LPCTSTR lpszPathName, bool bOpenExisting)
{
	if (lpszPathName != NULL && *lpszPathName != _T('\0'))
		m_szPathName = Path::GetFullPath(lpszPathName);	
	else
	{
		//generate temporary pathname
		_TCHAR szBuf[MAX_PATH];
		::GetTempPath(MAX_PATH, szBuf);
		::GetTempFileName(szBuf, _T("LA"), 0, m_szPathName.GetBuffer(MAX_PATH));
		m_szPathName.ReleaseBuffer();
	}
	
	m_hFile = ::CreateFile(m_szPathName, GENERIC_READ | GENERIC_WRITE, FILE_SHARE_READ | FILE_SHARE_WRITE, NULL, 
		(bOpenExisting ? OPEN_EXISTING : OPEN_ALWAYS), FILE_ATTRIBUTE_NORMAL | FILE_FLAG_RANDOM_ACCESS, NULL);
	
	if (m_hFile == INVALID_HANDLE_VALUE)
		_com_issue_error(HRESULT_FROM_WIN32(::GetLastError()));
}

//constructs an empty buffer for the large array, throws an exception if something goes wrong
//Note: it always create buffer that is capable to hold k pages
template < typename TYPE, typename ARG_TYPE, typename TI >
void CLargeArray< TYPE, ARG_TYPE, TI >::CreateBuffer(UINT nBufferSize, UINT nPageSize)
{
	//determine the page size
	if ((m_nPageSize = nPageSize) == 0)
	{
		m_nPageSize = LARGEARRAY_PAGESIZE_DEFAULTSIZE / sizeof(TYPE);
		if (m_nPageSize == 0)
			m_nPageSize = 1;	//at least one element must be in page
	}

	//determine the buffer size, page must be aligned 
	if (m_nFileSize == 0)
	{
		if ((m_nBufferSize = nBufferSize) == 0)
			m_nBufferSize = m_nPageSize;
	}
	else
	{
		if ((m_nBufferSize = nBufferSize) == 0)
		{
			m_nBufferSize = LARGEARRAY_BUFFER_DEFAULTSIZE / sizeof(TYPE);
			if (m_nBufferSize == 0)
				m_nBufferSize = m_nPageSize;		//at least one element must be buffered 
		}
	
		if ((TI)m_nBufferSize > m_nFileSize)
			m_nBufferSize = (UINT)m_nFileSize;
	}

	//align the buffer size
	m_nBufferSize = PageAlign(m_nBufferSize, m_nPageSize);
	
	//allocate the buffer
	while (NULL == (m_pBuffer = (TYPE*)calloc(m_nBufferSize, sizeof(TYPE))))
	{		
		m_nBufferSize /= 2;
		
		//align the buffer size
		m_nBufferSize = PageAlign(m_nBufferSize, m_nPageSize);

		if (m_nBufferSize*sizeof(TYPE) < LARGEARRAY_BUFFER_FAILMINSIZE)
			_com_issue_error(E_OUTOFMEMORY);
	}
	
	m_nBufferLength = 0;
	m_nDirtyPages = 0;	
	m_bReadOnly = false;

	m_nLastPage = (UINT)-1;
	m_pLastPage = NULL;

	//get short page
	m_nShortPage = (UINT)(m_nFileSize / m_nPageSize);
	m_nShortPageSize = (UINT)(m_nFileSize % m_nPageSize);
}


//Flushes all data into the file, if file doesn't exist, it is created
template < typename TYPE, typename ARG_TYPE, typename TI >
void CLargeArray< TYPE, ARG_TYPE, TI >::Flush()
{
	//check for file existence
	if (m_hFile == INVALID_HANDLE_VALUE)
		OpenFile(m_szPathName, false);
	
	//we need to save all data
	POSITION pos = m_mapPages.GetRecentlyUsedPosition();
	while (pos != NULL)
	{
		DATA_BLOCK_ITEM* pItem = m_mapPages.GetNextPValue(pos);
		if (pItem->bDirty)
			WritePage(pItem);
	}	
}

//Changes the size of an array to the specified new size. 
//Throws an exception, if it fails.
template < typename TYPE, typename ARG_TYPE, typename TI >
void CLargeArray< TYPE, ARG_TYPE, TI >::Resize(TI nNewSize, bool bResizeBuffer)
{
	if (nNewSize >= m_nFileSize)
	{
		m_nFileSize = nNewSize;

		//enlarge buffer if possible
		if (bResizeBuffer)
		{			
			UINT nBufferSize = LARGEARRAY_BUFFER_DEFAULTSIZE / sizeof(TYPE);
			if ((TI)nBufferSize > nNewSize)
				nBufferSize = (UINT)nNewSize;

			if (nBufferSize != m_nBufferSize)
				ResizeBuffer(nBufferSize);
		}

		//if the array doesn't fit the memory, open the file
		if (m_hFile == INVALID_HANDLE_VALUE && m_nFileSize > (TI)m_nBufferSize)
			OpenFile(m_szPathName, false);
	}
	else
	{
		//shrinkage
		m_nFileSize = nNewSize;

		if (m_hFile == INVALID_HANDLE_VALUE)		
			ResizeBuffer((UINT)nNewSize);	//the all data is in the memory
		else
		{
			//buffer may contain invalid pages
			Flush();

			m_mapPages.RemoveAll();
			m_nBufferLength = 0;			

			if ((TI)m_nBufferSize > m_nFileSize)
				ResizeBuffer((UINT)m_nFileSize);

			//shrink the file, if it is the temporary one
			if (!m_bAttachedFile)
			{
				//shrink the file
				LARGE_INTEGER liPos;
				liPos.QuadPart = nNewSize*sizeof(TYPE);

				if (!::SetFilePointerEx(m_hFile, liPos, NULL, FILE_BEGIN) || !::SetEndOfFile(m_hFile))
					_com_issue_error(HRESULT_FROM_WIN32(::GetLastError()));
			}
		}
	}

	//now handle the last short page	
	//get a new short page
	UINT nNewShortPage = (UINT)(m_nFileSize / m_nPageSize);
	UINT nNewShortPageSize = (UINT)(m_nFileSize % m_nPageSize);

	if (m_nShortPageSize < m_nPageSize)
	{
		DATA_BLOCK_ITEM* pMemPage;
		if (MapFilePageToMemoryPage(m_nShortPage, &pMemPage))
		{
			if (nNewShortPage == m_nShortPage)
			{
				//shrinkage or enlargment within one page
				BES_ASSERT(nNewShortPageSize != 0);	//it would have been solved in ResizeBuffer

				pMemPage->nSize = nNewShortPageSize;
				if (nNewShortPageSize < m_nShortPageSize)
					memset(pMemPage->pPage + pMemPage->nSize, 0, 
						(m_nShortPageSize - nNewShortPageSize)*sizeof(TYPE));				
			}
			else
			{
				//enlarge page
				BES_ASSERT(nNewShortPage > m_nShortPage);

				pMemPage->nSize = m_nPageSize;				
			}
		}
	}

	//set a new short page
	m_nShortPage = nNewShortPage;
	m_nShortPageSize = nNewShortPageSize;

	if (m_nShortPageSize < m_nPageSize)
	{
		DATA_BLOCK_ITEM* pMemPage;
		if (MapFilePageToMemoryPage(m_nShortPage, &pMemPage))
		{			
			pMemPage->nSize = nNewShortPageSize;			
			memset(pMemPage->pPage + pMemPage->nSize, 0, 
				(m_nPageSize - m_nShortPageSize)*sizeof(TYPE));			
		}
	}
}

//Changes the size of underlaying buffer to the specified new size. 
//Throws an exception, if an error occurs.
template < typename TYPE, typename ARG_TYPE, typename TI >
void CLargeArray< TYPE, ARG_TYPE, TI >::ResizeBuffer(UINT nNewBufferSize)
{
	if (nNewBufferSize == 0)
	{
		nNewBufferSize = LARGEARRAY_BUFFER_DEFAULTSIZE / sizeof(TYPE);
		if (nNewBufferSize == 0)
			nNewBufferSize = 1;		//at least one element must be buffered
	}

	if ((TI)nNewBufferSize > m_nFileSize)
		nNewBufferSize = (UINT)m_nFileSize;

	//align the buffer size
	nNewBufferSize = PageAlign(nNewBufferSize, m_nPageSize);

	TYPE* pNewPtr = m_pBuffer;
	if (nNewBufferSize > m_nBufferSize)
	{		
		//enlarge the buffer (if it is possible)		
		while (nNewBufferSize > m_nBufferSize && 
			NULL == (pNewPtr = (TYPE*)realloc(m_pBuffer, nNewBufferSize*sizeof(TYPE))))
		{
			nNewBufferSize /= 2;

			//align the buffer size
			nNewBufferSize = PageAlign(nNewBufferSize, m_nPageSize);
		}
	}
	else if (nNewBufferSize < m_nBufferSize)
	{
		//shrink the buffer
		if (nNewBufferSize < m_nBufferLength)
		{
			//there are some pages to be deleted
			//if the array doesn't fit the new buffer size, open the file
			if (m_hFile == INVALID_HANDLE_VALUE && m_nFileSize > (ULONGLONG)nNewBufferSize)
				OpenFile(m_szPathName, false);	

			m_nBufferLength = nNewBufferSize;

			TYPE* pFirstInvalidByte = m_pBuffer + nNewBufferSize;
			POSITION pos = m_mapPages.GetRecentlyUsedPosition();
			while (pos != NULL)
			{
				POSITION posPrev = pos;
				DATA_BLOCK_ITEM* pItem = m_mapPages.GetNextPValue(pos);
				if ((pItem->pPage + pItem->nSize) >= pFirstInvalidByte)
				{
					//this page is invalid
					if (pItem->bDirty)
						WritePage(pItem);

					BES_ASSERT(pItem->pPage >= pFirstInvalidByte);
					
					m_mapPages.RemoveAt(posPrev);	//the whole page is to be removed					
				}								
			}
			
		}
		
		pNewPtr = (TYPE*)realloc(m_pBuffer, nNewBufferSize*sizeof(TYPE));
	}

	//alter pointers to pages
	if (m_pBuffer != pNewPtr)
	{
		UINT nShift = (UINT)((BYTE*)pNewPtr - (BYTE*)m_pBuffer);
		POSITION pos = m_mapPages.GetRecentlyUsedPosition();
		while (pos != NULL)
		{
			DATA_BLOCK_ITEM* pItem = m_mapPages.GetNextPValue(pos);
			pItem->pPage = (TYPE*)(((BYTE*)pItem->pPage) + nShift);
		}
	} // if (m_pBuffer != pNewPtr)	
	
	m_pBuffer = pNewPtr;
	m_nBufferSize = nNewBufferSize;
}

//reads page data from the underlaying file stream
template < typename TYPE, typename ARG_TYPE, typename TI >
void CLargeArray< TYPE, ARG_TYPE, TI >::ReadPage(DATA_BLOCK_ITEM* pItem)
{
	LARGE_INTEGER liPos;
	liPos.QuadPart = pItem->ulFileOffset;

	DWORD dwRead;
	if (!::SetFilePointerEx(m_hFile, liPos, NULL, FILE_BEGIN) ||
		!::ReadFile(m_hFile, pItem->pPage, pItem->nSize*sizeof(TYPE), &dwRead, NULL)
		)
		_com_issue_error(HRESULT_FROM_WIN32(::GetLastError()));	

	if (dwRead < pItem->nSize*sizeof(TYPE))
		memset(((BYTE*)pItem->pPage) + dwRead, 0, pItem->nSize*sizeof(TYPE) - dwRead);

	//TRACE(_T("READPAGE: %I64d - %I64d -> %p - %p, size: %d\n"),
	//	pItem->ulFileOffset, pItem->ulFileOffset + pItem->nSize,
	//	pItem->pPage, pItem->pPage + pItem->nSize, pItem->nSize);
}

//flushes page data into the file stream
template < typename TYPE, typename ARG_TYPE, typename TI >
void CLargeArray< TYPE, ARG_TYPE, TI >::WritePage(DATA_BLOCK_ITEM* pItem)
{
	LARGE_INTEGER liPos;
	liPos.QuadPart = pItem->ulFileOffset;

	DWORD dwWritten;
	if (!::SetFilePointerEx(m_hFile, liPos, NULL, FILE_BEGIN) ||
		!::WriteFile(m_hFile, pItem->pPage, pItem->nSize*sizeof(TYPE), &dwWritten, NULL)
		)
		_com_issue_error(HRESULT_FROM_WIN32(::GetLastError()));
	
	pItem->bDirty = false;
	m_nDirtyPages--;
}

//get an empty memory page to be used for following read or write operations, 
//if the buffer is not fully occupied, it is created, otherwise some page has to be swapped
template < typename TYPE, typename ARG_TYPE, typename TI >
typename CLargeArray< TYPE, ARG_TYPE, TI >::DATA_BLOCK_ITEM* CLargeArray< TYPE, ARG_TYPE, TI >::GetPage(UINT nPage)
{	
	if (m_nBufferLength == m_nBufferSize)
	{
		//all pages are occupied => take the least recently used page and reuse it
		POSITION pos = m_mapPages.GetLeastRecentlyUsedPosition();
		DATA_BLOCK_ITEM* pItem = m_mapPages.GetPValueAt(pos);
		if (pItem->bDirty)
			WritePage(pItem);

		m_mapPages.SetKeyAt(pos, nPage);
		m_mapPages.SetAsRecentlyUsed(pos);
		return pItem;
	}
	else
	{
		//buffer contains still some unmapped pages		
		DATA_BLOCK_ITEM ssItem;
		ssItem.pPage = &m_pBuffer[m_nBufferLength];
		ssItem.nSize = nPage == m_nShortPage ? m_nShortPageSize : m_nPageSize;	//default
		ssItem.bDirty = false;

		m_mapPages.Add(nPage, ssItem);
		DATA_BLOCK_ITEM* pItem = m_mapPages.GetRecentlyUsedPValue();

		m_nBufferLength += m_nPageSize;
		BES_ASSERT(m_nBufferLength <= m_nBufferSize);						
		return pItem;
	}		
}



//gets memory page and reads the data
template < typename TYPE, typename ARG_TYPE, typename TI >
inline typename CLargeArray< TYPE, ARG_TYPE, TI >::DATA_BLOCK_ITEM* CLargeArray< TYPE, ARG_TYPE, TI >::GetPage(UINT nFilePage, bool bReadPage)
{
	//perhaps we may avoid MapFilePageToMemoryPage
	//if data is read sequentially (usually), this slightly improves the overall time (about 2%)	
	if (m_nLastPage == nFilePage)
		return m_pLastPage;

	DATA_BLOCK_ITEM* pMemPage;
	if (!MapFilePageToMemoryPage(nFilePage, &pMemPage))
	{
		//page is not mapped into the memory, we need to construct memory page
		pMemPage = GetPage(nFilePage);
		pMemPage->ulFileOffset = m_ulStartOffset + ((TI)nFilePage) * m_nPageSize * sizeof(TYPE);

		if (bReadPage)
			ReadPage(pMemPage);		//we have to read original data
	}

	m_nLastPage = nFilePage;
	m_pLastPage = pMemPage;
	return pMemPage;
}

//gets min(nCount, available bytes)
template < typename TYPE, typename ARG_TYPE, typename TI >
inline UINT CLargeArray< TYPE, ARG_TYPE, TI >::GetValidLength(DATA_BLOCK_ITEM* pMemPage, UINT nOffset, TI nCount)
{
	UINT nValidLength;
	
	if ((nOffset + nCount) >= pMemPage->nSize)
		nValidLength = pMemPage->nSize - nOffset;
	else
		nValidLength = pMemPage->nSize;

	if (nValidLength > nCount)
		nValidLength = (UINT)nCount;

	return nValidLength;
}

//gets one or more bytes starting at nIndex
//throws an exception if data cannot be read
template < typename TYPE, typename ARG_TYPE, typename TI >
void CLargeArray< TYPE, ARG_TYPE, TI >::GetData(TI nIndex, TYPE* pData, UINT nCount)
{
	if (nIndex + nCount > m_nFileSize)
		_com_issue_error(E_INVALIDARG);

	//get the file page info	
	UINT nFilePage, nOffset;
	MapFileOffsetToFilePage(nIndex, nFilePage, nOffset);

	BES_ASSERT(nIndex == nFilePage*m_nPageSize + nOffset);

	//read data	
	while (nCount > 0)
	{
		DATA_BLOCK_ITEM* pMemPage = GetPage(nFilePage, m_hFile != INVALID_HANDLE_VALUE);
		UINT nValidLength = GetValidLength(pMemPage, nOffset, nCount);
				
		//copy data
		memcpy(pData, pMemPage->pPage + nOffset, nValidLength*sizeof(TYPE));

		pData += nValidLength;
		nCount -= nValidLength;
		nFilePage++;

		nOffset = 0;	//except for the first page, all others pages has zero offset
	}
}

//sets one or more bytes starting at nIndex
//throws an exception if data cannot be written
template < typename TYPE, typename ARG_TYPE, typename TI >
void CLargeArray< TYPE, ARG_TYPE, TI >::SetData(TI nIndex, TYPE* pData, UINT nCount)
{
	if (nIndex + nCount > m_nFileSize)
		_com_issue_error(E_INVALIDARG);
	
	if (m_bReadOnly)
		_com_issue_error(E_ACCESSDENIED);
	
	//get the file page info	
	UINT nFilePage, nOffset;
	MapFileOffsetToFilePage(nIndex, nFilePage, nOffset);
	
	//write pages	
	while (nCount > 0)
	{
		DATA_BLOCK_ITEM* pMemPage = GetPage(nFilePage, 
			(nOffset != 0 || nCount < m_nPageSize) && m_hFile != INVALID_HANDLE_VALUE);
		UINT nValidLength = GetValidLength(pMemPage, nOffset, nCount);
		
		//copy data
		memcpy(pMemPage->pPage + nOffset, pData, nValidLength*sizeof(TYPE));
	
		if (!pMemPage->bDirty)
		{
			pMemPage->bDirty = true;
			m_nDirtyPages++;
		}

		pData += nValidLength;
		nCount -= nValidLength;
		nFilePage++;

		nOffset = 0;	//except for the first page, all others pages has zero offset
	}
}

//memset
template < typename TYPE, typename ARG_TYPE, typename TI >
void CLargeArray< TYPE, ARG_TYPE, TI >::Fill(TI nIndex, ARG_TYPE nValue, TI nCount)
{
	if (nIndex + nCount > m_nFileSize)
		_com_issue_error(E_INVALIDARG);

	//get the file page info	
	UINT nFilePage, nOffset;
	MapFileOffsetToFilePage(nIndex, nFilePage, nOffset);

	//write data
	while (nCount > 0)
	{
		DATA_BLOCK_ITEM* pMemPage = GetPage(nFilePage, m_hFile != INVALID_HANDLE_VALUE);
		UINT nValidLength = GetValidLength(pMemPage, nOffset, nCount);
		
		//fill data
		for (UINT i = 0; i < nValidLength; i++)
			pMemPage->pPage[nOffset + i] = nValue;		
	
		if (!pMemPage->bDirty)
		{
			pMemPage->bDirty = true;
			m_nDirtyPages++;
		}

		nCount -= nValidLength;
		nFilePage++;
		nOffset = 0;	//except for the first page, all others pages has zero offset
	}
}

//copies data into another LargeArray laDest (must be large enough)
template < typename TYPE, typename ARG_TYPE, typename TI >
void CLargeArray< TYPE, ARG_TYPE, TI >::CopyTo(TI nIndex, TI nCount, CLargeArray& laDest, TI nDestIndex)
{
	if (nIndex + nCount > m_nFileSize)
		_com_issue_error(E_INVALIDARG);

	//get the file page info	
	UINT nFilePage, nOffset;
	MapFileOffsetToFilePage(nIndex, nFilePage, nOffset);

	//read data		
	while (nCount > 0)
	{
		DATA_BLOCK_ITEM* pMemPage = GetPage(nFilePage, m_hFile != INVALID_HANDLE_VALUE);
		UINT nValidLength = GetValidLength(pMemPage, nOffset, nCount);
		
		//copy data
		laDest.SetData(nDestIndex, pMemPage->pPage + nOffset, nValidLength);

		nDestIndex += nValidLength;
		nCount -= nValidLength;
		nFilePage++;

		nOffset = 0;	//except for the first page, all others pages has zero offset
	}
}

//gets one byte at nIndex
template < typename TYPE, typename ARG_TYPE, typename TI >
ARG_TYPE CLargeArray< TYPE, ARG_TYPE, TI >::GetAt(TI nIndex, bool bSetDirty)
{
	if (nIndex >= m_nFileSize)
		_com_issue_error(E_INVALIDARG);

	//get the file page info	
	UINT nFilePage, nOffset;
	MapFileOffsetToFilePage(nIndex, nFilePage, nOffset);

	DATA_BLOCK_ITEM* pMemPage = GetPage(nFilePage, m_hFile != INVALID_HANDLE_VALUE);
	if (bSetDirty && !pMemPage->bDirty)
	{
		pMemPage->bDirty = true;
		m_nDirtyPages++;
	}

	return pMemPage->pPage[nOffset];
}

//sets one bytes starting at nIndex	
template < typename TYPE, typename ARG_TYPE, typename TI >
void CLargeArray< TYPE, ARG_TYPE, TI >::SetAt(TI nIndex, ARG_TYPE nValue)
{
	if (nIndex >= m_nFileSize)
		_com_issue_error(E_INVALIDARG);

	//get the file page info	
	UINT nFilePage, nOffset;
	MapFileOffsetToFilePage(nIndex, nFilePage, nOffset);

	DATA_BLOCK_ITEM* pMemPage = GetPage(nFilePage, m_hFile != INVALID_HANDLE_VALUE);
	pMemPage->pPage[nOffset] = nValue;

	if (!pMemPage->bDirty)
	{
		pMemPage->bDirty = true;
		m_nDirtyPages++;
	}
}
	
//indexer operator - get
template < typename TYPE, typename ARG_TYPE, typename TI >
inline const TYPE CLargeArray< TYPE, ARG_TYPE, TI >::operator [] (TI nIndex) const
{
	return GetAt(nIndex);
}

//indexer operator - set
template < typename TYPE, typename ARG_TYPE, typename TI >
inline TYPE& CLargeArray< TYPE, ARG_TYPE, TI >::operator [] (TI nIndex)
{
	if (nIndex >= m_nFileSize)
		_com_issue_error(E_INVALIDARG);

	//get the file page info	
	UINT nFilePage, nOffset;
	MapFileOffsetToFilePage(nIndex, nFilePage, nOffset);

	DATA_BLOCK_ITEM* pMemPage = GetPage(nFilePage, m_hFile != INVALID_HANDLE_VALUE);
	if (!pMemPage->bDirty)
	{
		pMemPage->bDirty = true;
		m_nDirtyPages++;
	}

	return pMemPage->pPage[nOffset];
}

template < typename TYPE, typename ARG_TYPE, typename TI >
inline bool CLargeArray< TYPE, ARG_TYPE, TI >::operator == (int Null)
{
	_ASSERTE(Null == NULL);
	return m_pBuffer == NULL;
}

////////////////////////////////////////////
// some predefined classes
typedef CLargeArray<BYTE, BYTE> CByteLargeArrayL;
typedef CLargeArray<BYTE, BYTE, UINT> CByteLargeArray;
