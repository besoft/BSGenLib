/* ${FILENAME}
** ${LASTMODIFIED}
**
** ${COPYRIGHT}
** ${LICENCE}
**
** DESCRIPTION: Not-Sorted List 
*/

#ifndef BES_LIST_H
#define BES_LIST_H

#ifdef __BSGEN_MFC
//CSimpleList is already defined in MFC (although it is undocumented)
#define CSimpleList CSimpleListBS
#endif 

class CSimpleListItem
{
public:
	CSimpleListItem()
	{
		m_pLast = NULL;
		m_pNext = NULL;
	}

	virtual ~CSimpleListItem() {
	}

public:
	CSimpleListItem* m_pLast;
	CSimpleListItem* m_pNext;	
};

class CSimpleList
{
public:

	CSimpleList(void)
	{
		m_pHead = NULL;
		m_pTail = NULL;
		m_nItems = 0;
	}

	~CSimpleList(void)
	{
		RemoveAll(true);
	}
	
public:
	inline void Add(CSimpleListItem* pItem)
	{
		BES_ASSERT(pItem != NULL);
		BES_ASSERT(pItem->m_pLast == NULL);
		BES_ASSERT(pItem->m_pNext == NULL);
		
		if (m_pHead != NULL)
			m_pHead->m_pLast = pItem;
		else
			m_pTail = pItem;
		pItem->m_pNext = m_pHead;
		m_pHead = pItem;
		m_nItems++;	
	}

	inline void Remove(CSimpleListItem* pItem)
	{
		BES_ASSERT(pItem != NULL);

		if (pItem->m_pLast != NULL)
			pItem->m_pLast->m_pNext = pItem->m_pNext;
		else
			m_pHead = pItem->m_pNext;

		if (pItem->m_pNext != NULL)
			pItem->m_pNext->m_pLast = pItem->m_pLast;
		else
			m_pTail = pItem->m_pLast;

		m_nItems--;
	}

	inline void RemoveAll(bool bFreeMem = false)
	{
		if (bFreeMem)
		{
			CSimpleListItem* p;
			while (m_pHead != NULL)
			{
				p = m_pHead->m_pNext;
				delete m_pHead;	//will free memory
				m_pHead = p;
			}
		}
		
		m_pHead = NULL;
		m_pTail = NULL;
		m_nItems = 0;
	}

	inline int GetCount() { return m_nItems;}
	
	inline CSimpleListItem* GetNextItem(CSimpleListItem* pRoot)
	{
		if (pRoot == NULL)
			return m_pHead;
		else
			return pRoot->m_pNext;
	}

	inline CSimpleListItem* GetPrevItem(CSimpleListItem* pRoot)
	{
		if (pRoot == NULL)
			return m_pTail;
		else
			return pRoot->m_pLast;
	}

protected:
	CSimpleListItem* m_pHead;
	CSimpleListItem* m_pTail;
	int m_nItems;
};

class CSimpleSortedList : public CSimpleList
{
public:
	CSimpleSortedList()
	{
		m_bAutoSort = true;
	}

	inline void Add(CSimpleListItem* pItem)
	{
		CSimpleList::Add(pItem);	
		if (m_bAutoSort)
			SortItem(pItem);
	}

	void SortItem(CSimpleListItem* pItem);
	void SortAllItems();

	inline void EnableAutoSort() { m_bAutoSort = true; }
	inline void DisableAutoSort() { m_bAutoSort = false; }
protected:	
	virtual int CompareValues(CSimpleListItem* pValue1, CSimpleListItem* pValue2) = 0;
//attributes
protected:
	bool m_bAutoSort;
};


#endif //BES_LIST_H