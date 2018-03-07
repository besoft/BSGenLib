#pragma once

#pragma region CList<TYPE, ARG_TYPE>
#ifndef __BSGEN_PUREWIN
#define CList CAtlList
#pragma message("#define CList CAtlArray")
#else

//this template class is supposed to be used for LRU approach
//items in the list are automatically ordered according to the access to them,
//the tail contains the tail item
#pragma deprecated(CList)
template< typename TYPE, class ARG_TYPE = const TYPE& >
class CList
{
protected:
	class CListItem
	{
	public:		
		TYPE m_key;		

		//pointers to previous and next items
		CListItem* m_pPrev;
		CListItem* m_pNext;
	public:
		CListItem(ARG_TYPE key) : m_key(key)
		{			
			m_pPrev = m_pNext = NULL;
		}
	};

	//list of items
	CListItem* m_pHead;
	CListItem* m_pTail;
	int m_nCount;
	
public:
	CList()
	{
		m_pHead = m_pTail = NULL;
		m_nCount = 0;
	}

	~CList()
	{
		RemoveAll();		
	}

//operations
public:
	//adds new value
	inline POSITION AddHead(ARG_TYPE key);
	inline POSITION AddTail(ARG_TYPE key);

	//adds new value
	inline POSITION Add(ARG_TYPE key) {
		AddTail(key);
	}

	//adds list
	void AddHead(CList<TYPE, ARG_TYPE>& list);
	void AddTail(CList<TYPE, ARG_TYPE>& list);

	//removes the head item
	inline void RemoveHead();

	//removes the tail item
	inline void RemoveTail();

	//removes the item at the position pos
	inline void RemoveAt(POSITION pos);

	//removes all items
	inline void RemoveAll();

	//searches the list for the specified element.
	POSITION Find(ARG_TYPE element, POSITION posStartAfter = NULL); 

	//inserts a new element into the list after the specified position
	POSITION InsertAfter(POSITION pos, ARG_TYPE key); 

	//inserts a new element into the list before the specified position
	POSITION InsertBefore(POSITION pos, ARG_TYPE key); 

	//gets the head position
	inline POSITION GetHeadPosition();

	//gets the tail position
	inline POSITION GetTailPosition();

	//gets the position of the following item
	inline POSITION GetNextPosition(POSITION pos);

	//gets the position of the following item
	inline POSITION GetPrevPosition(POSITION pos);

	//gets the key of the item at pos and moves to the next item
	inline ARG_TYPE GetNext(POSITION& pos);

	//gets the key and value of the item at pos and moves to the next item
	inline void GetNext(POSITION& pos, ARG_TYPE key);

	//gets the key of the item at pos and moves to the prev item
	inline ARG_TYPE GetPrev(POSITION& pos);

	//gets the key and value of the item at pos and moves to the prev item
	inline void GetPrev(POSITION& pos, ARG_TYPE key);
	
	//gets the key of the item at pos
	inline ARG_TYPE GetAt(POSITION pos);
	
	//gets the key of the item at pos
	inline void SetAt(POSITION pos, ARG_TYPE key);

	//returns the key of the head item
	inline ARG_TYPE GetHead();	

	//returns the key of the tail item
	inline ARG_TYPE GetTail();	

	//moves the given item to the head of list
	inline void SetAsHead(POSITION pos);
	inline void MoveToHead(POSITION pos) {
		SetAsHead(pos);
	}

	//moves the given item to the head of list
	inline void SetAsTail(POSITION pos);
	inline void MoveToTail(POSITION pos) {
		SetAsTail(pos);
	}

	//returns true if the list is empty
	inline bool IsEmpty() {
		return m_pHead == NULL;
	}

	//returns number of items in the list
	inline UINT GetCount(){
		return (UINT)m_nCount;
	}

protected:
	//removes the item from the list
	inline void Remove(CListItem* p);

	//connects the given item to the head of list
	inline void LinkItemHead(CListItem* p);

	//connects the given item to the tail of list
	inline void LinkItemTail(CListItem* p);

	//connects the given item to the head of list
	inline void LinkItem(CListItem* p) {
		LinkItemHead(p);
	}

	//disconnects the given item from the list
	inline void UnlinkItem(CListItem* p);
};

//searches the list for the specified element.
template< typename TYPE, class ARG_TYPE >
POSITION CList< TYPE, ARG_TYPE >::Find(ARG_TYPE element, POSITION posStartAfter)
{
	CListItem* pItem = (CListItem*)posStartAfter;
	if (posStartAfter != NULL)
		pItem = pItem->m_pNext;
	else
		pItem = m_pHead;
	while (pItem != NULL) 
	{
		if (pItem->m_key == element)
			return (POSITION)pItem;
		pItem = pItem->m_pNext;
	}

	return NULL;
}

//adds new value
template< typename TYPE, class ARG_TYPE >
inline POSITION CList< TYPE, ARG_TYPE >::AddHead(ARG_TYPE key) {
	LinkItemHead(new CListItem(key));
	m_nCount++;
	return (POSITION)m_pHead;
}

//adds new value
template< typename TYPE, class ARG_TYPE >
inline POSITION CList< TYPE, ARG_TYPE >::AddTail(ARG_TYPE key) {
	LinkItemTail(new CListItem(key));
	m_nCount++;
	return (POSITION)m_pTail;
}

//inserts a new element into the list after the specified position
template< typename TYPE, class ARG_TYPE >
POSITION CList< TYPE, ARG_TYPE >::InsertAfter(POSITION pos, ARG_TYPE key)
{
	if( pos == NULL )
		return AddTail(key); // insert after nothing -> tail of the list

	CListItem* pItem = (CListItem*)pos;
	CListItem* pNewItem = new CListItem(key);	
	if (NULL != (pNewItem->m_pNext = pItem->m_pNext))
		pNewItem->m_pNext->m_pPrev = pNewItem;
	else
		m_pTail = pNewItem;
	pItem->m_pNext = pNewItem;
	pNewItem->m_pPrev = pItem;

	m_nCount++;
	return (POSITION)pNewItem;
}

//inserts a new element into the list before the specified position
template< typename TYPE, class ARG_TYPE >
POSITION CList< TYPE, ARG_TYPE >::InsertBefore(POSITION pos, ARG_TYPE key)
{
	if( pos == NULL )
		return AddHead(key); // insert after nothing -> tail of the list

	CListItem* pItem = (CListItem*)pos;
	CListItem* pNewItem = new CListItem(key);
	if (NULL != (pNewItem->m_pPrev = pItem->m_pPrev))
		pNewItem->m_pPrev->m_pNext = pNewItem;	
	else
		m_pHead = pNewItem;
	pItem->m_pPrev = pNewItem;
	pNewItem->m_pNext = pItem;

	m_nCount++;
	return (POSITION)pNewItem;
}

//removes the head item
template< typename TYPE, class ARG_TYPE >
inline void CList< TYPE, ARG_TYPE >::RemoveHead()
{
	if (m_pHead != NULL)
		Remove(m_pHead);
}

//removes the tail item
template< typename TYPE, class ARG_TYPE >
inline void CList< TYPE, ARG_TYPE >::RemoveTail()
{
	if (m_pTail != NULL)
		Remove(m_pTail);
}

//removes the item at the position pos
template< typename TYPE, class ARG_TYPE >
inline void CList < TYPE, ARG_TYPE >::RemoveAt(POSITION pos)
{
	if (pos != NULL)
		Remove((CListItem*)pos);
}

//removes the item from the list
template< typename TYPE, class ARG_TYPE >
inline void CList< TYPE, ARG_TYPE >::Remove(CListItem* p)
{	
	UnlinkItem(p);
	delete p;
	m_nCount--;
}

//removes all items
template< typename TYPE, class ARG_TYPE >
inline void CList< TYPE, ARG_TYPE >::RemoveAll()
{
	while (m_pHead != NULL)
	{
		m_pTail = m_pHead->m_pNext;
		delete m_pHead;

		m_pHead = m_pTail;
	}

	m_nCount = 0;
}

//returns the key of the head item
template< typename TYPE, class ARG_TYPE >
inline ARG_TYPE CList < TYPE, ARG_TYPE >::GetHead()
{
	return m_pHead->m_key;
}

//returns the key of the tail item
template< typename TYPE, class ARG_TYPE >
inline ARG_TYPE CList < TYPE, ARG_TYPE >::GetTail()
{
	return m_pTail->m_key;
}

//gets the key of the item at pos
template< typename TYPE, class ARG_TYPE >
inline ARG_TYPE CList < TYPE, ARG_TYPE >::GetAt(POSITION pos)
{
	return ((CListItem*)pos)->m_key;
}


//gets the key of the item at pos
template< typename TYPE, class ARG_TYPE >
inline void CList< TYPE, ARG_TYPE >::SetAt(POSITION pos, ARG_TYPE key) {	
	((CListItem*)pos)->m_key = key;	
}

//gets the head position
template< typename TYPE, class ARG_TYPE >
inline POSITION CList < TYPE, ARG_TYPE >::GetHeadPosition()
{
	return (POSITION)m_pHead;
}

//gets the tail position
template< typename TYPE, class ARG_TYPE >
inline POSITION CList < TYPE, ARG_TYPE >::GetTailPosition()
{
	return (POSITION)m_pTail;
}

//gets the position of the following item
template< typename TYPE, class ARG_TYPE >
inline POSITION CList < TYPE, ARG_TYPE >::GetNextPosition(POSITION pos)
{
	return (POSITION)((CListItem*)pos)->m_pNext;
}

//gets the position of the following item
template< typename TYPE, class ARG_TYPE >
inline POSITION CList < TYPE, ARG_TYPE >::GetPrevPosition(POSITION pos)
{
	return (POSITION)((CListItem*)pos)->m_pPrev;
}

//gets the key of the item at pos and moves to the next item
template< typename TYPE, class ARG_TYPE >
inline ARG_TYPE CList < TYPE, ARG_TYPE >::GetNext(POSITION& pos)
{
	CListItem* pItem = (CListItem*)pos;
	pos = (POSITION)pItem->m_pNext;
	return pItem->m_key;
}

//gets the value of the item at pos and moves to the prev item
template< typename TYPE, class ARG_TYPE >
inline ARG_TYPE CList < TYPE, ARG_TYPE >::GetPrev(POSITION& pos)
{
	CListItem* pItem = (CListItem*)pos;
	pos = (POSITION)pItem->m_pPrev;
	return pItem->m_key;
}

//connects the given item to the head of list
template< typename TYPE, class ARG_TYPE >
inline void CList < TYPE, ARG_TYPE >::LinkItemHead(CListItem* p)
{
	p->m_pNext = m_pHead;
	if (m_pHead != NULL)
		m_pHead->m_pPrev = p;
	else
		m_pTail = p;
	m_pHead = p;	
}

//connects the given item to the tail of list
template< typename TYPE, class ARG_TYPE >
inline void CList < TYPE, ARG_TYPE >::LinkItemTail(CListItem* p)
{
	p->m_pPrev = m_pTail;
	if (m_pTail != NULL)
		m_pTail->m_pNext = p;
	else
		m_pHead = p;
	m_pTail = p;	
}

//disconnects the given item from the list
template< typename TYPE, class ARG_TYPE >
inline void CList < TYPE, ARG_TYPE >::UnlinkItem(CListItem* p)
{
	if (p->m_pPrev != NULL)
		p->m_pPrev->m_pNext = p->m_pNext;
	else
		m_pHead = p->m_pNext;

	if (p->m_pNext != NULL)
		p->m_pNext->m_pPrev = p->m_pPrev;
	else
		m_pTail = p->m_pPrev;

	p->m_pNext = p->m_pPrev = NULL;
}

//moves the given item to the head of list
template< typename TYPE, class ARG_TYPE >
inline void CList < TYPE, ARG_TYPE >::SetAsHead(POSITION pos)
{
	UnlinkItem((CListItem*)pos);
	LinkItemHead((CListItem*)pos);
}

//moves the given item to the head of list
template< typename TYPE, class ARG_TYPE >
inline void CList < TYPE, ARG_TYPE >::SetAsTail(POSITION pos)
{
	UnlinkItem((CListItem*)pos);
	LinkItemTail((CListItem*)pos);		
}

//adds list
template< typename TYPE, class ARG_TYPE >
void CList < TYPE, ARG_TYPE >::AddHead(CList<TYPE, ARG_TYPE>& list) 
{
	CListItem* pItem = list.m_pTail;
	while (pItem != NULL)
	{
		AddHead(pItem->m_key);
		pItem = pItem->m_pPrev;
	}
}

template< typename TYPE, class ARG_TYPE >
void CList < TYPE, ARG_TYPE >::AddTail(CList<TYPE, ARG_TYPE>& list) 
{
	CListItem* pItem = list.m_pHead;
	while (pItem != NULL)
	{
		AddTail(pItem->m_key);
		pItem = pItem->m_pNext;
	}
}
#endif

#pragma endregion //CList
#pragma region CSortedList<TYPE, ARG_TYPE>

#if !defined(__BSGEN_PUREWIN)
template< typename TYPE, class ARG_TYPE = CElementTraits< TYPE > >
#else
template< typename TYPE, class ARG_TYPE = const TYPE& >
#endif
class CSortedList : public CList<TYPE, ARG_TYPE>
{
//attributes
protected:
	bool m_bAutoSort;

public:
	CSortedList(bool bAutoSort = false) {
		m_bAutoSort = bAutoSort;
	}

	//adds new value
	inline POSITION AddHead(ARG_TYPE key);
	inline POSITION AddTail(ARG_TYPE key);

	//adds new value
	inline POSITION Add(ARG_TYPE key) {
		AddTail(key);
	}

	//adds list
	void AddHead(CList<TYPE, ARG_TYPE>& list);
	void AddTail(CList<TYPE, ARG_TYPE>& list);
	

	//gets the key of the item at pos
	inline void SetAt(POSITION pos, ARG_TYPE key);

	//moves the item at pos to the appropriate position	
	//NOTE: the other items must be already sorted!
	void SortItem(POSITION pos);

	//orders the entire list in the ascening fashion
	void SortAllItems();

	inline void EnableAutoSort() { 
		m_bAutoSort = true; 
	}
	
	inline void DisableAutoSort() { 
		m_bAutoSort = false; 
	}
};

//adds new value
template< typename TYPE, class ARG_TYPE >
inline POSITION CSortedList<TYPE, ARG_TYPE>::AddHead(ARG_TYPE key)
{
	POSITION pos = CList<TYPE, ARG_TYPE>::AddHead(key);	
	if (m_bAutoSort)
		SortItem(pos);
	return pos;
}

//adds new value
template< typename TYPE, class ARG_TYPE >
inline POSITION CSortedList<TYPE, ARG_TYPE>::AddTail(ARG_TYPE key)
{
	POSITION pos = CList<TYPE, ARG_TYPE>::AddTail(key);	
	if (m_bAutoSort)
		SortItem(pos);
	return pos;
}

//gets the key of the item at pos
template< typename TYPE, class ARG_TYPE >
inline void CSortedList < TYPE, ARG_TYPE >::SetAt(POSITION pos, ARG_TYPE key)
{
	CList<TYPE, ARG_TYPE>::SetAt(pos, key);
	if (m_bAutoSort)
		SortItem(pos);
}

//adds list
template< typename TYPE, class ARG_TYPE >	
void CSortedList < TYPE, ARG_TYPE >::AddHead(CList<TYPE, ARG_TYPE>& list)
{
	POSITION pos = list.GetTailPosition();
	while (pos != NULL)	
		AddHead(list.GetPrev(pos));	
}

template< typename TYPE, class ARG_TYPE >	
void CSortedList < TYPE, ARG_TYPE >::AddTail(CList<TYPE, ARG_TYPE>& list)
{
	POSITION pos = list.GetHeadPosition();
	while (pos != NULL)	
		AddTail(list.GetNext(pos));
}

//moves the item at pos to the appropriate position	
//NOTE: the other items must be already sorted!
template< typename TYPE, class ARG_TYPE >
void CSortedList < TYPE, ARG_TYPE >::SortItem(POSITION pos)
{
	BES_ASSERT(pos != NULL);

	POSITION posOriginal = pos;	
	const TYPE& value = GetNext(pos);
	
	while (pos != NULL)
	{
		POSITION posPrev = pos;
		const TYPE& newValue = GetNext(pos);
		if (newValue >= value)
		{
			//value should be placed after posPrev
			if (posOriginal != posPrev)
			{
				InsertAfter(posPrev, value);
				RemoveAt(posOriginal);
				return;
			}
		}
	}

	pos = posOriginal;	
	GetPrev(pos);

	while (pos != NULL)
	{
		POSITION posPrev = pos;
		const TYPE& newValue = GetPrev(pos);
		if (newValue <= value)
		{
			//value should be placed after posPrev
			if (posOriginal != posPrev)
			{
				InsertAfter(posPrev, value);
				RemoveAt(posOriginal);
				return;
			}
		}
	}
}

//orders the entire list in the ascending fashion
//bubble sort is used
template< typename TYPE, class ARG_TYPE >
void CSortedList < TYPE, ARG_TYPE >::SortAllItems()
{
	if (IsEmpty())
		return;		//empty liat => exit

	bool bSwapped;

	do
	{
		bSwapped = false;
		POSITION pos1 = GetHeadPosition();		
		POSITION posTmp = pos1;
		const TYPE& val1 = GetNext(posTmp);
		while (posTmp != NULL)
		{
			POSITION pos2 = posTmp;
			const TYPE& val2 = GetNext(posTmp);
			if (val1 > val2)	//pos1 and pos2 must be swapped
			{
#ifndef __BSGEN_PUREWIN
				SwapElements(pos1, pos2);
#else
				InsertAfter(pos2, val2);
				RemoveAt(pos1);
#endif
				bSwapped = true;
			}

			pos1 = pos2;
		} //endwhile (posTmp != NULL)
		
	}while (bSwapped);
}
#pragma endregion	//CSortedList