/* ${FILENAME}
** ${LASTMODIFIED}
**
** ${COPYRIGHT}
** ${LICENCE}
**
** DESCRIPTION: Abstract class for a collection of Events
*/

#include "stdafx.h"
//#include "memmng.h"

BYTE* CMemMngBucket::GetMem(DWORD dwBytes)
{
	if (GetMaxFreeBytes() < dwBytes)
		return NULL;	//this bucket is not capable to alloc it, return NULL
	
	CMemMngFreeMemBlock* pFreeMem;
	if (!IsLargestFreeBlockToBeUsed(dwBytes))
	{
		pFreeMem = m_pTail;
		while (pFreeMem != NULL)
		{
			if (pFreeMem->m_dwFreeBytes >= dwBytes)
				break;
			pFreeMem = pFreeMem->m_pLastFMB;
		}
	}
	else
		pFreeMem = m_pHead;		

	if (pFreeMem == NULL)
		return NULL;

	BYTE* pRet = pFreeMem->m_pFirstAddress;
	pFreeMem->m_pFirstAddress += dwBytes;		//next free block starts here
	pFreeMem->m_dwFreeBytes -= dwBytes;			//and is long

	SortBlock(pFreeMem);
	return pRet;
}

void CMemMngBucket::FreeMem(BYTE* pData, DWORD dwBytes)
{
	BYTE* pNextData = pData + dwBytes;
	CMemMngFreeMemBlock* pFree = m_pHead;	
	while (pFree != NULL)
	{
		BYTE* pNextFAddr = pFree->m_pFirstAddress + pFree->m_dwFreeBytes;	//addr of the first occupied byte
		if (pNextFAddr == pData)
		{	
			//there is a block of free memory adjacent from the left to the given block
			pFree->m_dwFreeBytes += dwBytes;
			break;
		}

		if (pFree->m_pFirstAddress == pNextData)
		{
			//there is a block of free memory adjacent from the right to the given block
			pFree->m_pFirstAddress = pData;
			pFree->m_dwFreeBytes += dwBytes;
			break;
		}

		pFree = pFree->m_pNextFMB; 
	}

	if (pFree == NULL)
	{
		//add new block of FreeMem
		pFree = new CMemMngFreeMemBlock(pData, dwBytes);
		pFree->m_pLastFMB = NULL;
		pFree->m_pNextFMB = m_pHead;
		m_pHead = pFree;
		if (m_pTail == NULL)
			m_pTail = pFree;
	}
	else
	{
		//check for the blocks to be merged
		BYTE* pNextFAddr = pFree->m_pFirstAddress + pFree->m_dwFreeBytes;	//addr of the first occupied byte
		CMemMngFreeMemBlock* pTmp = m_pHead;
		while (pTmp != NULL)
		{		
			if (pTmp->m_pFirstAddress == pNextFAddr || (pTmp->m_pFirstAddress + pTmp->m_dwFreeBytes) == pFree->m_pFirstAddress)
			{
				//merge
				if (pTmp->m_pLastFMB != NULL)					
					pTmp->m_pLastFMB->m_pNextFMB = pTmp->m_pNextFMB;
				else
					m_pHead = pTmp->m_pNextFMB;

				if (pTmp->m_pNextFMB != NULL)					
					pTmp->m_pNextFMB->m_pLastFMB = pTmp->m_pLastFMB;
				else
					m_pTail = pTmp->m_pLastFMB;
					
				if (pTmp->m_pFirstAddress != pNextFAddr)
					pFree->m_pFirstAddress = pTmp->m_pFirstAddress;
				pFree->m_dwFreeBytes += pTmp->m_dwFreeBytes;
				delete pTmp;
				break;
			}

			pTmp = pTmp->m_pNextFMB; 
		}
	}
	
	SortBlock(pFree);
}

void CMemMngBucket::SortBlock(CMemMngFreeMemBlock* pFreeMem)
{
	if (pFreeMem->m_dwFreeBytes == 0)
	{
		//this block is fully allocated - remove it from the FreeMemCollection
		if (pFreeMem->m_pLastFMB != NULL)
			pFreeMem->m_pLastFMB->m_pNextFMB = pFreeMem->m_pNextFMB;
		else
			m_pHead = pFreeMem->m_pNextFMB;

		if (pFreeMem->m_pNextFMB != NULL)
			pFreeMem->m_pNextFMB->m_pLastFMB = pFreeMem->m_pLastFMB;
		else
			m_pTail = pFreeMem->m_pLastFMB;		

		delete pFreeMem;
	}
	else		
	{
		//move the block of free space to the proper position (blocks are ordered by FreeSize DESC
		while (pFreeMem->m_pNextFMB != NULL)
		{			
			CMemMngFreeMemBlock* pNextBlock = pFreeMem->m_pNextFMB;
			if (pFreeMem->m_dwFreeBytes < pNextBlock->m_dwFreeBytes)
			{
				//swap needed HEAD -> ... FM -> NB -> ... -> TAIL
				//=> HEAD -> ... NB -> FM -> ... -> TAIL
				pFreeMem->m_pNextFMB = pNextBlock->m_pNextFMB;
				pNextBlock->m_pNextFMB = pFreeMem;				
				pNextBlock->m_pLastFMB = pFreeMem->m_pLastFMB;
				pFreeMem->m_pLastFMB = pNextBlock;

				if (pNextBlock->m_pLastFMB != NULL)				
					pNextBlock->m_pLastFMB->m_pNextFMB = pNextBlock;
				else
					m_pHead = pNextBlock;

				if (pFreeMem->m_pNextFMB != NULL)
					pFreeMem->m_pNextFMB->m_pLastFMB = pFreeMem;
				else				
					m_pTail = pFreeMem;										
			}
			else
				break;	//finished
		}

		while (pFreeMem->m_pLastFMB != NULL)
		{			
			CMemMngFreeMemBlock* pLastBlock = pFreeMem->m_pLastFMB;
			if (pFreeMem->m_dwFreeBytes > pLastBlock->m_dwFreeBytes)
			{
				//swap needed
				pLastBlock->m_pNextFMB = pFreeMem->m_pNextFMB;
				pFreeMem->m_pNextFMB = pLastBlock;
				pFreeMem->m_pLastFMB = pLastBlock->m_pLastFMB;
				pLastBlock->m_pLastFMB = pFreeMem;				

				if (pFreeMem->m_pLastFMB != NULL)				
					pFreeMem->m_pLastFMB->m_pNextFMB = pFreeMem;
				else
					m_pHead = pFreeMem;

				if (pLastBlock->m_pNextFMB != NULL)
					pLastBlock->m_pNextFMB->m_pLastFMB = pLastBlock;
				else				
					m_pTail = pLastBlock;
			}
			else
				break;	//finished
		}
	}
}

///////////////////////////////////////
// class CMemMng
BYTE* CMemMng::GetMem(DWORD dwBytes)
{
	CMemMngBucket* pBucket;
//	m_cs.Lock();
	if (m_pHead == NULL || m_pHead->GetMaxFreeBytes() < dwBytes)
	{
		pBucket = new CMemMngBucket((dwBytes < DEFAULT_MMB_SIZE ? DEFAULT_MMB_SIZE : dwBytes)); //new bucket
		pBucket->m_pLastBucket = NULL;
		pBucket->m_pNextBucket = m_pHead;
		if (m_pHead != NULL)
			m_pHead->m_pLastBucket = pBucket;
		m_pHead = pBucket;
		if (m_pTail == NULL)
			m_pTail = pBucket;

		BES_ASSERT(m_pHead->m_pLastBucket == NULL);
	}
	else
		pBucket = m_pHead;

	BYTE* pRet = pBucket->GetMem(dwBytes);
	SortBucket(pBucket);
//	m_cs.Unlock();
	return pRet;
}

void CMemMng::FreeMem(BYTE* pData, DWORD dwBytes)
{
	if (pData == NULL)
		return;

//	m_cs.Lock();
	//find bucket with this address
	CMemMngBucket* pBucket = m_pHead;
	while (pBucket != NULL)
	{		
		if (pData >= pBucket->m_pData && pData < pBucket->m_pData + pBucket->m_dwBucketSize)
		{
			pBucket->FreeMem(pData, dwBytes);			
			break;
		}
		pBucket = pBucket->m_pNextBucket;
	}

	SortBucket(pBucket);
//	m_cs.Unlock();
}

void CMemMng::SortBucket(CMemMngBucket* pBucket)
{
	if (pBucket->m_dwBucketSize == pBucket->GetMaxFreeBytes())
	{
		//remove it			
		if (pBucket->m_pLastBucket != NULL)
			pBucket->m_pLastBucket->m_pNextBucket = pBucket->m_pNextBucket;
		else
			m_pHead = pBucket->m_pNextBucket;

		if (pBucket->m_pNextBucket != NULL)
			pBucket->m_pNextBucket->m_pLastBucket = pBucket->m_pLastBucket;
		else
			m_pTail = pBucket->m_pLastBucket;
		
		BES_ASSERT(m_pHead == NULL || m_pHead->m_pLastBucket == NULL);

		delete pBucket;
	}
	else		
	{
		//move the block of free space to the proper position (blocks are ordered by FreeSize DESC
		while (pBucket->m_pNextBucket != NULL)
		{			
			CMemMngBucket* pNextBlock = pBucket->m_pNextBucket;
			if (pBucket->GetMaxFreeBytes() < pNextBlock->GetMaxFreeBytes())
			{
				//swap needed HEAD -> ... FM -> NB -> ... -> TAIL
				//=> HEAD -> ... NB -> FM -> ... -> TAIL
				pBucket->m_pNextBucket = pNextBlock->m_pNextBucket;
				pNextBlock->m_pNextBucket = pBucket;				
				pNextBlock->m_pLastBucket = pBucket->m_pLastBucket;
				pBucket->m_pLastBucket = pNextBlock;

				if (pNextBlock->m_pLastBucket != NULL)				
					pNextBlock->m_pLastBucket->m_pNextBucket = pNextBlock;
				else
					m_pHead = pNextBlock;

				if (pBucket->m_pNextBucket != NULL)
					pBucket->m_pNextBucket->m_pLastBucket = pBucket;
				else				
					m_pTail = pBucket;	

				BES_ASSERT(m_pHead->m_pLastBucket == NULL);
			}
			else
				break;	//finished
		}

		while (pBucket->m_pLastBucket != NULL)
		{			
			CMemMngBucket* pLastBlock = pBucket->m_pLastBucket;		
			if (pBucket->GetMaxFreeBytes() > pLastBlock->GetMaxFreeBytes())
			{
				//swap needed
				pLastBlock->m_pNextBucket = pBucket->m_pNextBucket;
				pBucket->m_pNextBucket = pLastBlock;
				pBucket->m_pLastBucket = pLastBlock->m_pLastBucket;
				pLastBlock->m_pLastBucket = pBucket;				

				if (pBucket->m_pLastBucket != NULL)				
					pBucket->m_pLastBucket->m_pNextBucket = pBucket;
				else
					m_pHead = pBucket;

				if (pLastBlock->m_pNextBucket != NULL)
					pLastBlock->m_pNextBucket->m_pLastBucket = pLastBlock;
				else				
					m_pTail = pLastBlock;						

				BES_ASSERT(m_pHead->m_pLastBucket == NULL);
			}
			else
				break;	//finished
		}
	}	
}