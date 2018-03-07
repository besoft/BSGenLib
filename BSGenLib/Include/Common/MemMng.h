/* ${FILENAME}
** ${LASTMODIFIED}
**
** ${COPYRIGHT}
** ${LICENCE}
**
** DESCRIPTION: Memory Management, Fast Allocation, Fast deallocation (like Garbage collector)
*/

#ifndef BES_MEMMNG_CS_H
#define BES_MEMMNG_CS_H


#define DEFAULT_MMB_SIZE		16384*1024	//page is equal to 16 MB 
#define DEFAULT_MMB_LSFB_THRS	10			//use largest freeblock if 10% of Bytes should be allocated

//this class stores information about a free block of memory
class CMemMngFreeMemBlock
{
public:
	CMemMngFreeMemBlock(BYTE* pStart, DWORD dwSize){m_pFirstAddress = pStart; m_dwFreeBytes = dwSize;}
	~CMemMngFreeMemBlock(){}
protected:
	CMemMngFreeMemBlock* m_pLastFMB;
	CMemMngFreeMemBlock* m_pNextFMB;

	BYTE* m_pFirstAddress;
	DWORD m_dwFreeBytes;

	friend class CMemMngBucket;
};

//this class stores info about a continuous block of memory
class CMemMngBucket
{
public:
	CMemMngBucket(DWORD dwBuckSize = DEFAULT_MMB_SIZE) 
	{
		BES_ASSERT(dwBuckSize > 0);

		m_dwBucketSize = dwBuckSize;			
		while (m_dwBucketSize != 0)
		{
			m_pData = new BYTE[m_dwBucketSize];
			if (m_pData != NULL)
				break;
			m_dwBucketSize /= 2;	//we are almost out of memory, try smaller block
		}		
		
		m_pHead = new CMemMngFreeMemBlock(m_pData, m_dwBucketSize);
		m_pTail = m_pHead;		

		if (m_pData == NULL || m_pHead == NULL)
			throw 0;

		m_pHead->m_pLastFMB = m_pHead->m_pNextFMB = NULL;
	}	

	~CMemMngBucket() 
	{		
		while (m_pHead != NULL)
		{
			CMemMngFreeMemBlock* p = m_pHead->m_pNextFMB;
			delete m_pHead;
			m_pHead = p;
		}
		delete[] m_pData;
	}
	
	BYTE* GetMem(DWORD dwBytes);
	void FreeMem(BYTE* pData, DWORD dwBytes);
	inline DWORD GetMaxFreeBytes() { return m_pHead->m_dwFreeBytes; }

protected:
	CMemMngFreeMemBlock* m_pHead;
	CMemMngFreeMemBlock* m_pTail;
	
	DWORD m_dwBucketSize;		//size in Bytes	
	BYTE* m_pData;

	CMemMngBucket* m_pLastBucket;
	CMemMngBucket* m_pNextBucket;

protected:	
	inline bool IsLargestFreeBlockToBeUsed(DWORD dwToAlloc) { return ((100*dwToAlloc / GetMaxFreeBytes()) > DEFAULT_MMB_LSFB_THRS);}
	void SortBlock(CMemMngFreeMemBlock* pFreeMem);

	friend class CMemMng;
};

class CMemMng
{
public:
	CMemMng()
	{
		m_pHead = NULL;
		m_pTail = NULL;
	}

	~CMemMng()
	{
		FreeAllMem();
	}

	BYTE* GetMem(DWORD dwBytes);
	void FreeMem(BYTE* pData, DWORD dwBytes);
	inline void FreeAllMem()
	{
		CMemMngBucket* p;
		while (m_pHead != NULL)
		{
			p = m_pHead->m_pNextBucket;
			delete m_pHead;	//will free memory
			m_pHead = p;
		}
		m_pTail = NULL;
	}
protected:
	CMemMngBucket* m_pHead;
	CMemMngBucket* m_pTail;	

	void SortBucket(CMemMngBucket* pBucket);
};

#endif //BES_MEMMNG_CS_H