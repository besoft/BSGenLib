/* ${FILENAME}
** ${LASTMODIFIED}
**
** ${COPYRIGHT}
** ${LICENCE}
**
** DESCRIPTION: Sorted List 
*/

#include "stdafx.h"
#include "../../Include/Common/SimpleList.h"

//INSERT SORT
void CSimpleSortedList::SortItem(CSimpleListItem* pItem)
{	
	BES_ASSERT(pItem != NULL);
	
	while (pItem->m_pNext != NULL)
	{			
		CSimpleListItem* pNextBlock = pItem->m_pNext;
		if (CompareValues(pItem, pNextBlock) < 0)
		{
			//swap needed HEAD -> ... FM -> NB -> ... -> TAIL
			//=> HEAD -> ... NB -> FM -> ... -> TAIL
			pItem->m_pNext = pNextBlock->m_pNext;
			pNextBlock->m_pNext = pItem;				
			pNextBlock->m_pLast = pItem->m_pLast;
			pItem->m_pLast = pNextBlock;

			if (pNextBlock->m_pLast != NULL)				
				pNextBlock->m_pLast->m_pNext = pNextBlock;
			else
				m_pHead = pNextBlock;

			if (pItem->m_pNext != NULL)
				pItem->m_pNext->m_pLast = pItem;
			else				
				m_pTail = pItem;										
		}
		else
			break;	//finished
	}

	while (pItem->m_pLast != NULL)
	{			
		CSimpleListItem* pLastBlock = pItem->m_pLast;
		if (CompareValues(pItem, pLastBlock) > 0)
		{
			//swap needed
			pLastBlock->m_pNext = pItem->m_pNext;
			pItem->m_pNext = pLastBlock;
			pItem->m_pLast = pLastBlock->m_pLast;
			pLastBlock->m_pLast = pItem;				

			if (pItem->m_pLast != NULL)				
				pItem->m_pLast->m_pNext = pItem;
			else
				m_pHead = pLastBlock;

			if (pLastBlock->m_pNext != NULL)
				pLastBlock->m_pNext->m_pLast = pLastBlock;
			else				
				m_pTail = pLastBlock;										
		}
		else
			break;	//finished
	}
}

void CSimpleSortedList::SortAllItems()
{
	CSimpleListItem* pItem = m_pHead;
	m_pHead = m_pTail = NULL;

	while (pItem != NULL)
	{
		CSimpleListItem* pItemNext = pItem->m_pNext;
		pItem->m_pNext = pItem->m_pLast = NULL;
		CSimpleList::Add(pItem);
		SortItem(pItem);
		pItem = pItemNext;
	}
}