/* ${FILENAME}
** ${LASTMODIFIED}
**
** ${COPYRIGHT}
** ${LICENCE}
**
** DESCRIPTION: Template for a round robin with possible growing factor
** HISTORY: version 2.0 - rewritten
*/

#ifndef BES_QUEUE_H
#define BES_QUEUE_H

#define DEFAULT_QUEUE_BLKSIZE	4096		//4096 items in one block

template<typename E>
class CQueue
{
//attributes
protected:
	CArray<E*> m_arBlocks;			//array of blocks
	E** m_pBlocks;					//ptr obtained from m_arBlocks.GetData()
	
	UINT m_iPos_R;					//read pointer
	UINT m_iPos_WR;					//write pointer	
	UINT m_nBlockSize;				//size of one block
	UINT m_nQueueSize;				//size of the whole queue = m_arBlocks.GetSize()*m_nBlockSize

public:
	CQueue(int nBlockSize = DEFAULT_QUEUE_BLKSIZE)
	{	
		InitQueue(nBlockSize);		
	}

	~CQueue()
	{
		DestroyQueue();
	}	

	void InitQueue(int nBlockSize)
	{
		m_arBlocks.Add(new E[m_nQueueSize = m_nBlockSize = nBlockSize]);
		m_pBlocks = m_arBlocks.GetData();
		m_iPos_R = 0;						//queue is empty
		m_iPos_WR = 0;
	}

	void DestroyQueue()
	{
		for (int i = 0; i < (int)m_arBlocks.GetCount(); i++)
			delete[] m_pBlocks[i];
		m_arBlocks.RemoveAll();
		m_nQueueSize = 0;
	}

	//puts a new element into the queue, bAllowGrowing specifies whether queue will be enlarged,
	//if there is no room for the new element, or the element will replace the head element	
	void Put(E& element, bool bAllowGrowing = false)
	{	
		m_iPos_WR = (m_iPos_WR % m_nQueueSize);		//recalculate W pointer
		UINT nWBlock = m_iPos_WR / m_nBlockSize;	//compute index of block currently being written
		UINT nWPos = m_iPos_WR % m_nBlockSize;		//and the wr-offset

		if (IsFull() && bAllowGrowing)
		{
			//the queue is FULL, but we can enlarge it. The queue is:
			//             WR			- W = WR pointer, R = R pointer
			// ... AAAAAAAAABBBBBB x CCCCCCCCCCCCC .... FFFFFFFFFFFF
			//1) add a new block into the array =>
			// ... AAAAAAAAABBBBBB x CCCCCCCCCCCCC .... FFFFFFFFFFFF x 000000000
			//2) get the array data and move the ptr to the new block to the correct position:
			// ... AAAAAAAAABBBBBB x 0000000000000 x CCCCCCCCCCCCC .... FFFFFFFFFFFF
			//3) copy data to the correct place
			// ... AAAAAAAAAbbbbbb x 00000000BBBBB x CCCCCCCCCCCCC .... FFFFFFFFFFFF
			//4) recalculate W and R pointers =>
			//             W                 R 
			// ... AAAAAAAAAbbbbbb x 00000000BBBBB x CCCCCCCCCCCCC .... FFFFFFFFFFFF
			E* pNewBlock = new E[m_nBlockSize];
			m_arBlocks.Add(pNewBlock);
			m_pBlocks = m_arBlocks.GetData();			

			m_iPos_R = (m_iPos_R % m_nQueueSize);		//recalculate R pointer
			UINT nRBlock = m_iPos_R / m_nBlockSize;		//compute index of block currently being read
								
			UINT nBlocks = (UINT)m_arBlocks.GetCount();
			if (nWBlock + 2 != nBlocks)
				memmove(&m_pBlocks[nWBlock + 1], &m_pBlocks[nWBlock + 2], (nBlocks - nWBlock - 1)*sizeof(E*));
			m_pBlocks[nBlocks + 1] = pNewBlock;
			
			if (nRBlock == nWBlock)	//otherwise we are ready
			{
				//shallow copy of data only	(it is OK, as we do not duplicate them)
				UINT nCount = m_nBlockSize - nWPos - 1;
				memcpy(&m_pBlocks[nWBlock][nWPos + 1], &pNewBlock[m_nBlockSize - nCount], nCount*sizeof(E));
			}
#ifdef _DEBUG
			else {
				BES_ASSERT(m_iPos_R % m_nBlockSize == 0);
			}
#endif
			
			//we need to recalculate indices
			m_nQueueSize += m_nBlockSize;
			m_iPos_R = m_iPos_WR + m_nBlockSize;
		}

		m_pBlocks[nWBlock][nWPos] = element;
		m_iPos_WR++;			
	}

	//gets the element at the beginning of the queue
	inline void Get(E* pRet)
	{
		Peek(pRet);
		m_iPos_R++;
	}

	//peeks the element at the beginning of the queue
	inline void Peek(E* pRet)
	{
		m_iPos_R = (m_iPos_R % m_nQueueSize);		//recalculate R pointer
		UINT nRBlock = m_iPos_R / m_nBlockSize;		//compute index of block currently being read
		UINT nRPos = m_iPos_R % m_nBlockSize;		//and the wr-offset
		
		*pRet = m_pBlocks[nRBlock][nRPos];
	}

	//.NET compatible method for Put
	inline void Enqueue(E& element) {
		Put(element);
	}

	//.NET compatible method for Get
	inline E Dequeue() {
		E element;
		Get(&element);
		return element;
	}

	//returns true if the queue is empty
	inline bool IsEmpty() {
		return m_iPos_R == m_iPos_WR;
	}

	//returns true, if the queue is full
	inline bool IsFull() {
		return m_iPos_R % m_nQueueSize == (m_iPos_WR + 1) % m_nQueueSize;
	}

	//returns number of elements in the queue
	inline UINT GetCount() {
		UINT a = m_iPos_WR % m_nQueueSize;
		UINT b = m_iPos_R % m_nQueueSize;
		if (a >= b)
			return a - b;
		else
			return m_nQueueSize - (b - a);
	}
};
#endif