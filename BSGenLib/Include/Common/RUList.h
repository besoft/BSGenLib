#pragma once

//this template class is supposed to be used for LRU approach
//items in the list are automatically ordered according to the access to them,
//the tail contains the least recently used item

template< typename T_KEY, typename T_VALUE >
class CRUList
{
public:
	typedef const T_KEY& T_KEY_ARG_IN;
	typedef const T_VALUE& T_VAL_ARG_IN;	
	typedef T_KEY& T_KEY_ARG_OUT;	
	typedef T_VALUE& T_VAL_ARG_OUT;	
	typedef T_KEY* T_KEY_PARG_OUT;	
	typedef T_VALUE* T_VAL_PARG_OUT;	

protected:
	class CRUListItem
	{
	public:
		//key and associated value
		T_KEY m_key;
		T_VALUE m_value;

		//pointers to previous and next items
		CRUListItem* m_pPrev;
		CRUListItem* m_pNext;
	public:
		CRUListItem(T_KEY_ARG_IN key, T_VAL_ARG_IN value) : 
		  m_key(key), m_value(value)
		{			
			m_pPrev = m_pNext = NULL;
		}
	};

	//list of items
	CRUListItem* m_pHead;
	CRUListItem* m_pTail;

	//hash table used to lookup items
	CHashTable<T_KEY, CRUListItem*> m_mapItems;	
public:
	CRUList()
	{
		m_pHead = m_pTail = NULL;
	}

	~CRUList()
	{
		RemoveAll();		
	}

//operations
public:
	//adds new value
	inline void Add(T_KEY_ARG_IN key, T_VAL_ARG_IN value);	

	//search for an item identified by key, returns false if not found,
	//otherwise true + the value associated to this key
	//the found item is automatically moved to the top of the list
	bool Lookup(T_KEY_ARG_IN key, T_VAL_ARG_OUT value);

	//search for an item identified by key, returns NULL if not found,
	//otherwise pointer to the value associated to this key
	//the found item is automatically moved to the top of the list
	T_VAL_PARG_OUT PLookup(T_KEY_ARG_IN key);

	//removes the specified item
	inline void RemoveKey(T_KEY_ARG_IN key);

	//removes the recently used item
	inline void RemoveRecentlyUsed();

	//removes the least recently used item
	inline void RemoveLeastRecentlyUsed();

	//removes the item at the position pos
	inline void RemoveAt(POSITION pos);

	//removes all items
	inline void RemoveAll();

	//gets the recently used position
	inline POSITION GetRecentlyUsedPosition();

	//gets the least recently used position
	inline POSITION GetLeastRecentlyUsedPosition();

	//gets the position of the following item
	inline POSITION GetNextPosition(POSITION pos);

	//gets the position of the following item
	inline POSITION GetPrevPosition(POSITION pos);

	//gets the key of the item at pos and moves to the next item
	inline T_KEY_ARG_OUT GetNextKey(POSITION& pos);

	//gets the value of the item at pos and moves to the next item
	inline T_VAL_ARG_OUT GetNextValue(POSITION& pos);

	//gets the key of the item at pos and moves to the next item
	inline T_KEY_PARG_OUT GetNextPKey(POSITION& pos);

	//gets the value of the item at pos and moves to the next item
	inline T_VAL_PARG_OUT GetNextPValue(POSITION& pos);

	//gets the key and value of the item at pos and moves to the next item
	inline void GetNext(POSITION& pos, T_KEY_ARG_OUT key, T_VAL_ARG_OUT value);

	//gets the key of the item at pos and moves to the prev item
	inline T_KEY_ARG_OUT GetPrevKey(POSITION& pos);

	//gets the value of the item at pos and moves to the prev item
	inline T_VAL_ARG_OUT GetPrevValue(POSITION& pos);

	//gets the key of the item at pos and moves to the prev item
	inline T_KEY_PARG_OUT GetPrevPKey(POSITION& pos);

	//gets the value of the item at pos and moves to the prev item
	inline T_VAL_PARG_OUT GetPrevPValue(POSITION& pos);

	//gets the key and value of the item at pos and moves to the prev item
	inline void GetPrev(POSITION& pos, T_KEY_ARG_OUT key, T_VAL_ARG_OUT value);
	
	//gets the key of the item at pos
	inline T_KEY_ARG_OUT GetKeyAt(POSITION pos);

	//gets the value of the item at pos
	inline T_VAL_ARG_OUT GetValueAt(POSITION pos);

	//gets the key of the item at pos
	inline void SetKeyAt(POSITION pos, T_KEY_ARG_IN key);

	//sets the value of the item at pos
	inline void SetValueAt(POSITION pos, T_VAL_ARG_IN value);

	//gets the key of the item at pos
	inline T_KEY_PARG_OUT GetPKeyAt(POSITION pos);

	//gets the value of the item at pos
	inline T_VAL_PARG_OUT GetPValueAt(POSITION pos);

	//gets the key and value of the item at pos
	inline void GetAt(POSITION pos, T_KEY_ARG_OUT key, T_VAL_ARG_OUT value);

	//returns the key of the recently used item
	inline T_KEY_ARG_OUT GetRecentlyUsedKey();	

	//returns the key of the least recently used item
	inline T_KEY_ARG_OUT GetLeastRecentlyUsedKey();	

	//returns the value of the recently used item
	inline T_VAL_ARG_OUT GetRecentlyUsedValue();

	//returns the value of the least recently used item
	inline T_VAL_ARG_OUT GetLeastRecentlyUsedValue();

	//returns the key of the recently used item
	inline T_KEY_PARG_OUT GetRecentlyUsedPKey();	

	//returns the key of the least recently used item
	inline T_KEY_PARG_OUT GetLeastRecentlyUsedPKey();	

	//returns the value of the recently used item
	inline T_VAL_PARG_OUT GetRecentlyUsedPValue();

	//returns the value of the least recently used item
	inline T_VAL_PARG_OUT GetLeastRecentlyUsedPValue();

	//returns the key and value of the recently used item
	inline void GetRecentlyUsed(T_KEY_ARG_OUT key, T_VAL_ARG_OUT value);

	//returns the key and value of the least recently used item
	inline void GetLeastRecentlyUsed(T_KEY_ARG_OUT key, T_VAL_ARG_OUT value);

	//moves the given item to the head of list
	inline void SetAsRecentlyUsed(POSITION pos);

	//returns true if the list is empty
	inline bool IsEmpty() {
		return m_pHead == NULL;
	}

	//returns number of items in the list
	inline UINT GetCount(){
		return (UINT)m_mapItems.GetCount();
	}

protected:
	//removes the item from the list
	inline void Remove(CRUListItem* p);

	//connects the given item to the head of list
	inline void LinkItem(CRUListItem* p);

	//disconnects the given item from the list
	inline void UnlinkItem(CRUListItem* p);
};

//adds new value
template < typename T_KEY, typename T_VALUE >
inline void CRUList< T_KEY, T_VALUE >::Add(T_KEY_ARG_IN key, T_VAL_ARG_IN value)
{
	CRUListItem* p = new CRUListItem(key, value);	
	m_mapItems[const_cast<T_KEY&>(key)] = p;

	LinkItem(p);
}

//search for an item identified by key, returns false if not found,
//otherwise true + the value associated to this key
template < typename T_KEY, typename T_VALUE >
typename CRUList< T_KEY, T_VALUE >::T_VAL_PARG_OUT CRUList< T_KEY, T_VALUE >::PLookup(T_KEY_ARG_IN key)
{
	CRUListItem* p;
	if (!m_mapItems.Lookup(const_cast<T_KEY&>(key), p))
		return NULL;

	if (p->m_pPrev != NULL)
	{
		UnlinkItem(p);
		LinkItem(p);
	}

	return &p->m_value;	
}

//search for an item identified by key, returns false if not found,
//otherwise true + the value associated to this key
template < typename T_KEY, typename T_VALUE >
bool CRUList< T_KEY, T_VALUE >::Lookup(T_KEY_ARG_IN key, T_VAL_ARG_OUT value)
{
	T_VAL_PARG_OUT* p = PLookup(key);
	if (p == NULL)
		return false;

	value = *p;
	return true;
}

//removes the specified item
template < typename T_KEY, typename T_VALUE >
inline void CRUList< T_KEY, T_VALUE >::RemoveKey(T_KEY_ARG_IN key)
{
	CRUListItem* p;
	if (m_mapItems.Lookup(const_cast<T_KEY&>(key), p))
		Remove(p);
}

//removes the recently used item
template < typename T_KEY, typename T_VALUE >
inline void CRUList< T_KEY, T_VALUE >::RemoveRecentlyUsed()
{
	if (m_pHead != NULL)
		Remove(m_pHead);
}

//removes the least recently used item
template < typename T_KEY, typename T_VALUE >
inline void CRUList< T_KEY, T_VALUE >::RemoveLeastRecentlyUsed()
{
	if (m_pTail != NULL)
		Remove(m_pTail);
}

//removes the item at the position pos
template < typename T_KEY, typename T_VALUE >
inline void CRUList < T_KEY, T_VALUE >::RemoveAt(POSITION pos)
{
	if (pos != NULL)
		Remove((CRUListItem*)pos);
}

//removes the item from the list
template < typename T_KEY, typename T_VALUE >
inline void CRUList< T_KEY, T_VALUE >::Remove(CRUListItem* p)
{
	m_mapItems.RemoveKey(p->m_key);

	UnlinkItem(p);
	delete p;	
}

//removes all items
template < typename T_KEY, typename T_VALUE >
inline void CRUList< T_KEY, T_VALUE >::RemoveAll()
{
	while (m_pHead != NULL)
	{
		m_pTail = m_pHead->m_pNext;
		delete m_pHead;

		m_pHead = m_pTail;
	}

	m_mapItems.RemoveAll();
}

//returns the key of the recently used item
template < typename T_KEY, typename T_VALUE >
inline typename CRUList< T_KEY, T_VALUE >::T_KEY_ARG_OUT CRUList < T_KEY, T_VALUE >::GetRecentlyUsedKey()
{
	return m_pHead->m_key;
}
//returns the key of the least recently used item
template < typename T_KEY, typename T_VALUE >
inline typename CRUList< T_KEY, T_VALUE >::T_KEY_ARG_OUT CRUList < T_KEY, T_VALUE >::GetLeastRecentlyUsedKey()
{
	return m_pTail->m_key;
}

//returns the value of the recently used item
template < typename T_KEY, typename T_VALUE >
inline typename CRUList< T_KEY, T_VALUE >::T_VAL_ARG_OUT CRUList < T_KEY, T_VALUE >::GetRecentlyUsedValue()
{
	return m_pHead->m_value;
}

//returns the value of the least recently used item
template < typename T_KEY, typename T_VALUE >
inline typename CRUList< T_KEY, T_VALUE >::T_VAL_ARG_OUT CRUList < T_KEY, T_VALUE >::GetLeastRecentlyUsedValue()
{	
	return m_pTail->m_value;
}

//returns the key of the recently used item
template < typename T_KEY, typename T_VALUE >
inline typename CRUList< T_KEY, T_VALUE >::T_KEY_PARG_OUT CRUList < T_KEY, T_VALUE >::GetRecentlyUsedPKey()
{
	if (m_pHead != NULL)
		return &m_pHead->m_key;
	
	return NULL;
}

//returns the key of the least recently used item
template < typename T_KEY, typename T_VALUE >
inline typename CRUList< T_KEY, T_VALUE >::T_KEY_PARG_OUT CRUList < T_KEY, T_VALUE >::GetLeastRecentlyUsedPKey()
{
	if (m_pTail != NULL)		
		return &m_pTail->m_key;

	return NULL;
}

//returns the value of the recently used item
template < typename T_KEY, typename T_VALUE >
inline typename CRUList< T_KEY, T_VALUE >::T_VAL_PARG_OUT CRUList < T_KEY, T_VALUE >::GetRecentlyUsedPValue()
{
	if (m_pHead != NULL)		
		return &m_pHead->m_value;

	return NULL;
}

//returns the value of the least recently used item
template < typename T_KEY, typename T_VALUE >
inline typename CRUList< T_KEY, T_VALUE >::T_VAL_PARG_OUT CRUList < T_KEY, T_VALUE >::GetLeastRecentlyUsedPValue()
{
	if (m_pTail != NULL)
		return &m_pTail->m_value;

	return NULL;
}

//returns the key and value of the recently used item
template < typename T_KEY, typename T_VALUE >
inline void CRUList < T_KEY, T_VALUE >::GetRecentlyUsed(T_KEY_ARG_OUT key, T_VAL_ARG_OUT value)
{
	key = m_pHead->m_key;
	value = m_pHead->m_value;
}

//returns the key and value of the least recently used item
template < typename T_KEY, typename T_VALUE >
inline void CRUList < T_KEY, T_VALUE >::GetLeastRecentlyUsed(T_KEY_ARG_OUT key, T_VAL_ARG_OUT value)
{
	key = m_pTail->m_key;
	value = m_pTail->m_value;
}

//gets the key of the item at pos
template < typename T_KEY, typename T_VALUE >
inline typename CRUList< T_KEY, T_VALUE >::T_KEY_ARG_OUT CRUList < T_KEY, T_VALUE >::GetKeyAt(POSITION pos)
{
	return ((CRUListItem*)pos)->m_key;
}

//gets the value of the item at pos
template < typename T_KEY, typename T_VALUE >
inline typename CRUList< T_KEY, T_VALUE >::T_VAL_ARG_OUT CRUList < T_KEY, T_VALUE >::GetValueAt(POSITION pos)
{
	return ((CRUListItem*)pos)->m_value;
}

//gets the key of the item at pos
template < typename T_KEY, typename T_VALUE >
inline typename CRUList< T_KEY, T_VALUE >::T_KEY_PARG_OUT CRUList < T_KEY, T_VALUE >::GetPKeyAt(POSITION pos)
{
	return &((CRUListItem*)pos)->m_key;
}

//gets the value of the item at pos
template < typename T_KEY, typename T_VALUE >
inline typename CRUList< T_KEY, T_VALUE >::T_VAL_PARG_OUT CRUList < T_KEY, T_VALUE >::GetPValueAt(POSITION pos)
{
	return &((CRUListItem*)pos)->m_value;
}

//gets the key and value of the item at pos
template < typename T_KEY, typename T_VALUE >
inline void CRUList < T_KEY, T_VALUE >::GetAt(POSITION pos, T_KEY_ARG_OUT key, T_VAL_ARG_OUT value)
{
	key =((CRUListItem*)pos)->m_key;
	value =((CRUListItem*)pos)->m_value;
}

//gets the key of the item at pos
template < typename T_KEY, typename T_VALUE >
inline void CRUList< T_KEY, T_VALUE >::SetKeyAt(POSITION pos, T_KEY_ARG_IN key)
{
	CRUListItem* pItem = (CRUListItem*)pos;
	m_mapItems.RemoveKey(pItem->m_key);
	pItem->m_key = key;

	m_mapItems[const_cast<T_KEY&>(key)] = pItem;
}

//sets the value of the item at pos
template < typename T_KEY, typename T_VALUE >
inline void CRUList< T_KEY, T_VALUE >::SetValueAt(POSITION pos, T_VAL_ARG_IN value)
{	
	((CRUListItem*)pos)->m_value = value;
}

//gets the recently used position
template < typename T_KEY, typename T_VALUE >
inline POSITION CRUList < T_KEY, T_VALUE >::GetRecentlyUsedPosition()
{
	return (POSITION)m_pHead;
}

//gets the least recently used position
template < typename T_KEY, typename T_VALUE >
inline POSITION CRUList < T_KEY, T_VALUE >::GetLeastRecentlyUsedPosition()
{
	return (POSITION)m_pTail;
}

//gets the position of the following item
template < typename T_KEY, typename T_VALUE >
inline POSITION CRUList < T_KEY, T_VALUE >::GetNextPosition(POSITION pos)
{
	return (POSITION)((CRUListItem*)pos)->m_pNext;
}

//gets the position of the following item
template < typename T_KEY, typename T_VALUE >
inline POSITION CRUList < T_KEY, T_VALUE >::GetPrevPosition(POSITION pos)
{
	return (POSITION)((CRUListItem*)pos)->m_pPrev;
}

//gets the key of the item at pos and moves to the next item
template < typename T_KEY, typename T_VALUE >
inline typename CRUList< T_KEY, T_VALUE >::T_KEY_ARG_OUT CRUList < T_KEY, T_VALUE >::GetNextKey(POSITION& pos)
{
	CRUListItem* pItem = (CRUListItem*)pos;
	pos = (POSITION)pItem->m_pNext;
	return pItem->m_key;
}

//gets the value of the item at pos and moves to the next item
template < typename T_KEY, typename T_VALUE >
inline typename CRUList< T_KEY, T_VALUE >::T_VAL_ARG_OUT CRUList < T_KEY, T_VALUE >::GetNextValue(POSITION& pos)
{
	CRUListItem* pItem = (CRUListItem*)pos;
	pos = (POSITION)pItem->m_pNext;
	return pItem->m_value;
}

//gets the key of the item at pos and moves to the next item
template < typename T_KEY, typename T_VALUE >
inline typename CRUList< T_KEY, T_VALUE >::T_KEY_PARG_OUT CRUList < T_KEY, T_VALUE >::GetNextPKey(POSITION& pos)
{
	CRUListItem* pItem = (CRUListItem*)pos;
	pos = (POSITION)pItem->m_pNext;
	return &pItem->m_key;
}

//gets the value of the item at pos and moves to the next item
template < typename T_KEY, typename T_VALUE >
inline typename CRUList< T_KEY, T_VALUE >::T_VAL_PARG_OUT CRUList < T_KEY, T_VALUE >::GetNextPValue(POSITION& pos)
{
	CRUListItem* pItem = (CRUListItem*)pos;
	pos = (POSITION)pItem->m_pNext;
	return &pItem->m_value;
}

//gets the key and value of the item at pos and moves to the next item
template < typename T_KEY, typename T_VALUE >
inline void CRUList < T_KEY, T_VALUE >::GetNext(POSITION& pos, T_KEY_ARG_OUT key, T_VAL_ARG_OUT value)
{
	CRUListItem* pItem = (CRUListItem*)pos;
	pos = (POSITION)pItem->m_pNext;
	key = pItem->m_key;
	value = pItem->m_value;
}

//gets the key of the item at pos and moves to the prev item
template < typename T_KEY, typename T_VALUE >
inline typename CRUList< T_KEY, T_VALUE >::T_KEY_ARG_OUT CRUList < T_KEY, T_VALUE >::GetPrevKey(POSITION& pos)
{
	CRUListItem* pItem = (CRUListItem*)pos;
	pos = (POSITION)pItem->m_pPrev;
	return pItem->m_key;
}

//gets the value of the item at pos and moves to the prev item
template < typename T_KEY, typename T_VALUE >
inline typename CRUList< T_KEY, T_VALUE >::T_VAL_ARG_OUT CRUList < T_KEY, T_VALUE >::GetPrevValue(POSITION& pos)
{
	CRUListItem* pItem = (CRUListItem*)pos;
	pos = (POSITION)pItem->m_pPrev;
	return pItem->m_value;
}

//gets the key of the item at pos and moves to the prev item
template < typename T_KEY, typename T_VALUE >
inline typename CRUList< T_KEY, T_VALUE >::T_KEY_PARG_OUT CRUList < T_KEY, T_VALUE >::GetPrevPKey(POSITION& pos)
{
	CRUListItem* pItem = (CRUListItem*)pos;
	pos = (POSITION)pItem->m_pPrev;
	return &pItem->m_key;
}

//gets the value of the item at pos and moves to the prev item
template < typename T_KEY, typename T_VALUE >
inline typename CRUList< T_KEY, T_VALUE >::T_VAL_PARG_OUT CRUList < T_KEY, T_VALUE >::GetPrevPValue(POSITION& pos)
{
	CRUListItem* pItem = (CRUListItem*)pos;
	pos = (POSITION)pItem->m_pPrev;
	return &pItem->m_value;
}

//gets the key and value of the item at pos and moves to the prev item
template < typename T_KEY, typename T_VALUE >
inline void CRUList < T_KEY, T_VALUE >::GetPrev(POSITION& pos, T_KEY_ARG_OUT key, T_VAL_ARG_OUT value)
{
	CRUListItem* pItem = (CRUListItem*)pos;
	pos = (POSITION)pItem->m_pPrev;
	key = pItem->m_key;
	value = pItem->m_value;
}

//connects the given item to the head of list
template < typename T_KEY, typename T_VALUE >
inline void CRUList < T_KEY, T_VALUE >::LinkItem(CRUListItem* p)
{
	p->m_pNext = m_pHead;
	if (m_pHead != NULL)
		m_pHead->m_pPrev = p;
	else
		m_pTail = p;
	m_pHead = p;
}

//disconnects the given item from the list
template < typename T_KEY, typename T_VALUE >
inline void CRUList < T_KEY, T_VALUE >::UnlinkItem(CRUListItem* p)
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
template < typename T_KEY, typename T_VALUE >
inline void CRUList < T_KEY, T_VALUE >::SetAsRecentlyUsed(POSITION pos)
{
	UnlinkItem((CRUListItem*)pos);
	LinkItem((CRUListItem*)pos);
}