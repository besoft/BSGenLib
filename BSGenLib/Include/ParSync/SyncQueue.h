/* ${FILENAME}
** ${LASTMODIFIED}
**
** ${COPYRIGHT}
** ${LICENCE}
**
** DESCRIPTION: Template for a round robin with possible growing factor, pseudo active waiting
*/

#ifndef BES_SYNCQUEUE_H
#define BES_SYNCQUEUE_H

#include "CS.h"
#include "syncevnts.h"
#include "thread.h"
#include "../Common/Queue.h"

//This is the synchronized version of CQueue
//It operates in several modes - see ctor
//WARNING: as Get and Put operations are not virtual, do not downcast instances of this class  to CQueue<E>
template<typename E>
class CSyncQueue : public CQueue<E>
{
//attributes
protected:
	CCS m_cs;
	bool m_bWaitEvntMode;
	bool m_bProConMode;
	CSyncEvent m_Written;
	CSyncEvent m_Read;

public:
	//ctor - bProConMode = sets the queue into Producer-Consumer mode, i.e.,
	//if the queue is empty, the Get operation will block and if the queue
	//is full, the Put operation will block (unless the enlargment is allowed)
	//- bWaitEnvMode denotes whether threads wait passively (true) or pseudoactively (false)
	//Note: pseudoactive waiting requires less initial overhead but takes some computer time whilst
	//passive waiting requires an immense initial overhead but does not take any computer time
	//=> pseudoactive waiting should be used for tasks where the speed of the production is almost 
	// equivalent to the speed of the consumption
	//- nBlockSize denotes the size of one block
	CSyncQueue(bool bProConMode = true, 
			bool bWaitEnvMode = false,
			int nBlockSize = DEFAULT_QUEUE_BLKSIZE) :
		CQueue<E>(nBlockSize)
	{			
		m_bWaitEvntMode = bWaitEnvMode;
		m_bProConMode = bProConMode;
	}


	//puts a new element into the queue, bAllowGrowing specifies whether queue will be enlarged,
	//if there is no room for the new element, or the element will replace the head element (PCMode=false)
	//or the calling thread will wait
	//NOTE: Put can be call simultaneously by more threads
	void Put(E& element, bool bAllowGrowing = false)
	{			
		for (;;)
		{
			m_cs.Lock();
			if (!IsFull() || !m_bProConMode)
			{
				CQueue<E>::Put(element, bAllowGrowing);
				if (m_bWaitEvntMode)
					m_Written.Set();	//resume thread waiting in Get, it may be call several times
				m_cs.Unlock();
				break;	//we're ready
			}
			
			//we have to wait
			m_cs.Unlock();
			if (m_bWaitEvntMode)
				m_Read.WaitOne();	//wait
			
			//this Put waits pseudo-actively because we does not expect wait for long time
			//and use of Suspend, Resume mechanism rapidly influences performance
			while (IsFull())
			{
				CThread::YieldMe();
			}
		} //for
	}

	//.NET compatible method for Put
	inline void Enqueue(E& element) {
		Put(element);
	}

	//gets the element at the beginning of the queue
	void Get(E* pRet)
	{
		for (;;)
		{
			if (m_bWaitEvntMode && IsEmpty())
				m_Written.WaitOne();	//wait if necessary
			while (IsEmpty()) {
				CThread::YieldMe();
			}
			
			m_cs.Lock();
			if (!IsEmpty())
			{
				CQueue<E>::Get(pRet);

				if (m_bWaitEvntMode)
					m_Read.Set();

				m_cs.Unlock();
				break;
			}
			m_cs.Unlock();
		}		
	}

	//peeks the element at the beginning of the queue
	void Peek(E* pRet)
	{
		for (;;)
		{
			if (m_bWaitEvntMode && IsEmpty())
				m_Written.WaitOne();	//wait if necessary
			while (IsEmpty()) {
				CThread::YieldMe();
			}
			
			m_cs.Lock();
			if (!IsEmpty())
			{
				CQueue<E>::Peek(pRet);

				if (m_bWaitEvntMode)
					m_Read.Set();

				m_cs.Unlock();
				break;
			}
			m_cs.Unlock();
		}
	}
	

	//.NET compatible method for Get
	inline E Dequeue() {
		E element;
		Get(&element);
		return element;
	}
};

#endif