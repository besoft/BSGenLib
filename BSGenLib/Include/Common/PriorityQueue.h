#pragma once

#pragma region "SimplePriorityQueue"
//SimplePriorityQueue is suitable for a small number of elements, 
//or for algorithms that do not alter priorities of elements in the queue

template < typename T_PRIORITY, typename T_VALUE >
class CSimplePriorityQueue 
{
public:
	typedef struct PQ_ITEM
	{
		T_PRIORITY priority;
		T_VALUE value;

	} PQ_ITEM;

protected:
	CArray<PQ_ITEM> m_Data;
	
public:
	//ctor
	CSimplePriorityQueue()
	{
		PQ_ITEM head;
		memset(&head, 0, sizeof(head));

		m_Data.Add(head);
	}

public:
	//puts a new element into the queue
	inline void Put(T_PRIORITY priority, T_VALUE value);

	//gets the element at the head
	void Get(T_PRIORITY* priority, T_VALUE* value);

	//gets the element at the head
	inline T_VALUE Get(T_PRIORITY* priority);

	//gets the element at the head
	inline T_VALUE Get();

	//removes value from the queue, returns false if element not found
	bool Remove(T_VALUE value);

	//modifies the priority for the given value
	void ModifyPriority(T_VALUE value, T_PRIORITY newpriority);

	//.NET compatible method for Put
	inline void Enqueue(T_PRIORITY priority, T_VALUE value) {
		Put(priority, value);
	}

	//.NET compatible method for Get
	inline T_VALUE Dequeue() {		
		return Get();
	}

	//returns true if the queue is empty
	inline bool IsEmpty() {
		return m_Data.GetCount() == 1;
	}

	//returns number of elements in the queue
	inline UINT GetCount() {
		return (UINT)m_Data.GetCount() - 1;
	}

protected:
	//swaps two elements
	inline void SwapElements(int nIndex1, int nIndex2);

	//restore the heap in the direction from leaves to root
	//starting at index nIndex, ending at index 1
	void MoveUp(int nIndex);

	//restore the heap in the direction from root to leaves
	//starting at index nStartIndex, ending at index nEndIndex
	void MoveDown(int nStartIndex, int nEndIndex);

	//finds the index for val, returns -1 if not found
	int FindItem(T_VALUE& val);
};

//puts a new element into the queue
template < typename T_PRIORITY, typename T_VALUE >
inline void CSimplePriorityQueue< T_PRIORITY, T_VALUE >::Put(T_PRIORITY priority, T_VALUE value)
{
	PQ_ITEM item;
	item.priority = priority;
	item.value = value;
	
	MoveUp((int)m_Data.Add(item));
}

//gets the element at the head
template < typename T_PRIORITY, typename T_VALUE >
void CSimplePriorityQueue< T_PRIORITY, T_VALUE >::Get(T_PRIORITY* priority, T_VALUE* value)
{	
	int nIndex = (int)m_Data.GetCount() - 1;
	BES_ASSERT(nIndex >= 1);

	SwapElements(1, nIndex);
	MoveDown(1, nIndex - 1);

	PQ_ITEM& item = m_Data[nIndex];
	*priority = item.priority;
	*value = item.value;
	m_Data.RemoveAt(nIndex);
}

//gets the element at the head
template < typename T_PRIORITY, typename T_VALUE >
inline T_VALUE CSimplePriorityQueue< T_PRIORITY, T_VALUE >::Get(T_PRIORITY* priority)
{
	T_VALUE ret;
	Get(priority, &ret);
	return ret;
}

//gets the element at the head
template < typename T_PRIORITY, typename T_VALUE >
inline T_VALUE CSimplePriorityQueue< T_PRIORITY, T_VALUE >::Get()
{
	T_PRIORITY dummy;
	return Get(&dummy);
}

//swaps two elements
template < typename T_PRIORITY, typename T_VALUE >
inline void CSimplePriorityQueue< T_PRIORITY, T_VALUE >::SwapElements(int nIndex1, int nIndex2)
{
	PQ_ITEM tmp = m_Data[nIndex1];
	m_Data[nIndex1] = m_Data[nIndex2];
	m_Data[nIndex2] = tmp;
}

//removes value from the queue, returns false if element not found
template < typename T_PRIORITY, typename T_VALUE >
bool CSimplePriorityQueue< T_PRIORITY, T_VALUE >::Remove(T_VALUE value)
{
	int nIndex1 = FindItem(value);
	if (nIndex1 < 0)
		return false;

	T_PRIORITY pri1 = m_Data[nIndex1].priority;
	
	int nIndex2 = (int)m_Data.GetCount() - 1;	
	T_PRIORITY pri2 = m_Data[nIndex2].priority;

	SwapElements(nIndex1, nIndex2);
	if (pri1 < pri2)
		MoveUp(nIndex1);
	else
		MoveDown(nIndex1, nIndex2);

	m_Data.RemoveAt(nIndex2);
	return true;
}

//modifies the priority for the given value
template < typename T_PRIORITY, typename T_VALUE >
void CSimplePriorityQueue< T_PRIORITY, T_VALUE >::ModifyPriority(T_VALUE value, T_PRIORITY newpriority)
{
	int nIndex = FindItem(value);
	if (nIndex >= 0)
	{
		T_PRIORITY priOrig = m_Data[nIndex].priority;
		m_Data[nIndex].priority = newpriority;
		if (priOrig < newpriority)
			MoveUp(nIndex);
		else
			MoveDown(nIndex, (int)m_Data.GetCount() - 1);
	}
}

//restore the heap in the direction from leaves to root
//starting at index nIndex, ending at index 1
template < typename T_PRIORITY, typename T_VALUE >
void CSimplePriorityQueue< T_PRIORITY, T_VALUE >::MoveUp(int nIndex)
{	
	const PQ_ITEM* pData = m_Data.GetData();

	while (nIndex > 1 && pData[nIndex / 2].priority < pData[nIndex].priority) 
	{		
		SwapElements(nIndex, nIndex / 2);
		nIndex /= 2;
	}
}

//restore the heap in the direction from root to leaves
//starting at index nStartIndex, ending at index nEndIndex
template < typename T_PRIORITY, typename T_VALUE >
void CSimplePriorityQueue< T_PRIORITY, T_VALUE >::MoveDown(int nStartIndex, int nEndIndex)
{
	const PQ_ITEM* pData = m_Data.GetData();

	int nNextIndex = 2*nStartIndex;
	while (nNextIndex <= nEndIndex) 
	{
		//if both leaves exist, check their priorities		
		if (nNextIndex < nEndIndex &&
			pData[nNextIndex].priority < pData[nNextIndex + 1].priority)
			nNextIndex++;

		if (pData[nStartIndex].priority >= pData[nNextIndex].priority)
			break;	//we're ready
						
		SwapElements(nStartIndex, nNextIndex);
		nStartIndex = nNextIndex;
		nNextIndex = 2*nStartIndex;
	}
}	

//finds the index for val, returns -1 if not found
template < typename T_PRIORITY, typename T_VALUE >
int CSimplePriorityQueue< T_PRIORITY, T_VALUE >::FindItem(T_VALUE& val)
{
	PQ_ITEM* pData = m_Data.GetData();
	int nCount = (int)m_Data.GetCount();
	for (int i = 1; i < nCount; i++)	
	{
		if (pData[i].value == val)
			return i;
	}

	return -1;
}
#pragma endregion //"SimplePriorityQueue"

#pragma region "PriorityQueue"
//PriorityQueue is suitable for a larger number of elements with
//priorities that may alter whilst they are in the queue
template < typename T_PRIORITY, typename T_VALUE >
class CPriorityQueue
{
public:
	typedef struct PQ_ITEM
	{
		T_PRIORITY priority;
		T_VALUE value;

	} PQ_ITEM;

protected:
	CArray<PQ_ITEM> m_Data;
	CHashTable<T_VALUE, int> m_MapTable;
	
public:
	//ctor
	CPriorityQueue()
	{
		PQ_ITEM head;
		memset(&head, 0, sizeof(head));

		m_Data.Add(head);
	}

public:
	//puts a new element into the queue
	inline void Put(T_PRIORITY priority, T_VALUE value);

	//gets the element at the head
	void Get(T_PRIORITY* priority, T_VALUE* value);

	//gets the element at the head
	inline T_VALUE Get(T_PRIORITY* priority);

	//gets the element at the head
	inline T_VALUE Get();

	//gets the element at the head without removing it from the queue
	void Peek(T_PRIORITY* priority, T_VALUE* value);

	//gets the element at the head without removing it from the queue
	inline T_VALUE Peek(T_PRIORITY* priority);

	//gets the element at the head without removing it from the queue
	inline T_VALUE Peek();

	//removes value from the queue, returns false if element not found
	bool Remove(T_VALUE value);

	//modifies the priority for the given value
	void ModifyPriority(T_VALUE value, T_PRIORITY newpriority);

	//.NET compatible method for Put
	inline void Enqueue(T_PRIORITY priority, T_VALUE value) {
		Put(priority, value);
	}

	//.NET compatible method for Get
	inline T_VALUE Dequeue() {		
		return Get();
	}

	//returns true if the queue is empty
	inline bool IsEmpty() {
		return m_Data.GetCount() == 1;
	}

	//returns number of elements in the queue
	inline UINT GetCount() {
		return (UINT)m_Data.GetCount() - 1;
	}

protected:
	//swaps two elements
	inline void SwapElements(int nIndex1, int nIndex2);

	//restore the heap in the direction from leaves to root
	//starting at index nIndex, ending at index 1
	void MoveUp(int nIndex);

	//restore the heap in the direction from root to leaves
	//starting at index nStartIndex, ending at index nEndIndex
	void MoveDown(int nStartIndex, int nEndIndex);

	//finds the index for val, returns -1 if not found
	inline int FindItem(T_VALUE& val);
};

//puts a new element into the queue
template < typename T_PRIORITY, typename T_VALUE >
inline void CPriorityQueue< T_PRIORITY, T_VALUE >::Put(T_PRIORITY priority, T_VALUE value)
{
	PQ_ITEM item;
	item.priority = priority;
	item.value = value;
	
	int nIndex = (int)m_Data.Add(item);
	m_MapTable[value] = nIndex;
	MoveUp(nIndex);
}

//gets the element at the head
template < typename T_PRIORITY, typename T_VALUE >
void CPriorityQueue< T_PRIORITY, T_VALUE >::Get(T_PRIORITY* priority, T_VALUE* value)
{	
	int nIndex = (int)m_Data.GetCount() - 1;
	BES_ASSERT(nIndex >= 1);

	SwapElements(1, nIndex);
	MoveDown(1, nIndex - 1);

	PQ_ITEM& item = m_Data[nIndex];
	*priority = item.priority;
	*value = item.value;
	m_Data.RemoveAt(nIndex);
	m_MapTable.RemoveKey(*value);
}

//gets the element at the head
template < typename T_PRIORITY, typename T_VALUE >
inline T_VALUE CPriorityQueue< T_PRIORITY, T_VALUE >::Get(T_PRIORITY* priority)
{
	T_VALUE ret;
	Get(priority, &ret);
	return ret;
}

//gets the element at the head
template < typename T_PRIORITY, typename T_VALUE >
inline T_VALUE CPriorityQueue< T_PRIORITY, T_VALUE >::Get()
{
	T_PRIORITY dummy;
	return Get(&dummy);
}

//gets the element at the head without removing it from the queue
template < typename T_PRIORITY, typename T_VALUE >
void CPriorityQueue< T_PRIORITY, T_VALUE >::Peek(T_PRIORITY* priority, T_VALUE* value)
{	
	PQ_ITEM& item = m_Data[1];
	*priority = item.priority;
	*value = item.value;	
}

//gets the element at the head without removing it from the queue
template < typename T_PRIORITY, typename T_VALUE >
inline T_VALUE CPriorityQueue< T_PRIORITY, T_VALUE >::Peek(T_PRIORITY* priority)
{
	T_VALUE ret;
	Peek(priority, &ret);
	return ret;
}

//gets the element at the head without removing it from the queue
template < typename T_PRIORITY, typename T_VALUE >
inline T_VALUE CPriorityQueue< T_PRIORITY, T_VALUE >::Peek()
{
	T_PRIORITY dummy;
	return Peek(&dummy);
}

//swaps two elements
template < typename T_PRIORITY, typename T_VALUE >
inline void CPriorityQueue< T_PRIORITY, T_VALUE >::SwapElements(int nIndex1, int nIndex2)
{
	PQ_ITEM tmp = m_Data[nIndex1];
	m_Data[nIndex1] = m_Data[nIndex2];
	m_Data[nIndex2] = tmp;

	m_MapTable[m_Data[nIndex1].value] = nIndex1;
	m_MapTable[m_Data[nIndex2].value] = nIndex2;
}

//removes value from the queue, returns false if element not found
template < typename T_PRIORITY, typename T_VALUE >
bool CPriorityQueue< T_PRIORITY, T_VALUE >::Remove(T_VALUE value)
{
	int nIndex1 = FindItem(value);
	if (nIndex1 < 0)
		return false;

	T_PRIORITY pri1 = m_Data[nIndex1].priority;
	
	int nIndex2 = (int)m_Data.GetCount() - 1;	
	T_PRIORITY pri2 = m_Data[nIndex2].priority;

	SwapElements(nIndex1, nIndex2);
	if (pri1 < pri2)
		MoveUp(nIndex1);
	else
		MoveDown(nIndex1, nIndex2);

	m_Data.RemoveAt(nIndex2);
	m_MapTable.RemoveKey(value);
	return true;
}

//modifies the priority for the given value
template < typename T_PRIORITY, typename T_VALUE >
void CPriorityQueue< T_PRIORITY, T_VALUE >::ModifyPriority(T_VALUE value, T_PRIORITY newpriority)
{
	int nIndex = FindItem(value);
	if (nIndex >= 0)
	{
		T_PRIORITY priOrig = m_Data[nIndex].priority;
		m_Data[nIndex].priority = newpriority;
		if (priOrig < newpriority)
			MoveUp(nIndex);
		else
			MoveDown(nIndex, (int)m_Data.GetCount() - 1);
	}
}

//restore the heap in the direction from leaves to root
//starting at index nIndex, ending at index 1
template < typename T_PRIORITY, typename T_VALUE >
void CPriorityQueue< T_PRIORITY, T_VALUE >::MoveUp(int nIndex)
{	
	const PQ_ITEM* pData = m_Data.GetData();

	while (nIndex > 1 && pData[nIndex / 2].priority < pData[nIndex].priority) 
	{		
		SwapElements(nIndex, nIndex / 2);
		nIndex /= 2;
	}
}

//restore the heap in the direction from root to leaves
//starting at index nStartIndex, ending at index nEndIndex
template < typename T_PRIORITY, typename T_VALUE >
void CPriorityQueue< T_PRIORITY, T_VALUE >::MoveDown(int nStartIndex, int nEndIndex)
{
	const PQ_ITEM* pData = m_Data.GetData();

	int nNextIndex = 2*nStartIndex;
	while (nNextIndex <= nEndIndex) 
	{
		//if both leaves exist, check their priorities		
		if (nNextIndex < nEndIndex &&
			pData[nNextIndex].priority < pData[nNextIndex + 1].priority)
			nNextIndex++;

		if (pData[nStartIndex].priority >= pData[nNextIndex].priority)
			break;	//we're ready
						
		SwapElements(nStartIndex, nNextIndex);
		nStartIndex = nNextIndex;
		nNextIndex = 2*nStartIndex;
	}
}	

//finds the index for val, returns -1 if not found
template < typename T_PRIORITY, typename T_VALUE >
inline int CPriorityQueue< T_PRIORITY, T_VALUE >::FindItem(T_VALUE& val)
{
	int nRet = -1;
	m_MapTable.Lookup(val, nRet);
	return nRet;
}

#pragma endregion