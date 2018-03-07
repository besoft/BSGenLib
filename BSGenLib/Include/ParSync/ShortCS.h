/* ${FILENAME}
** ${LASTMODIFIED}
**
** ${COPYRIGHT}
** ${LICENCE}
**
** DESCRIPTION: Wrapper for short Critical Section
*/

#ifndef BES_SYNC_SHORTCS_H
#define BES_SYNC_SHORTCS_H

#include "thread.h"
#include "interlocked.h"

//simple and fast critical section with pseudo-active waiting
//RELEASE NOTE: important! if used recursivelly, the application will hang up
class CShortCS
{
public:

	CShortCS(void)
	{
		m_cs = 0;
	}

	~CShortCS(void) {}

	static inline void Lock(volatile long* pCS)
	{
		while (0 != CInterlocked::CompareExchange(pCS, 1, 0))
		{
			CThread::YieldMe();
		}

	}

	inline void Lock() 
	{
		CShortCS::Lock(&m_cs);
	}
	inline void Unlock() { m_cs = 0; }	

	static inline void Unlock(volatile long* pCS)
	{
		*pCS = 0;
	}

protected:
	volatile long m_cs;
};

//simple and fast critical section with pseudo-active waiting
//and a wide range of values (suitable for memory locks where every thread has one bit)
//RELEASE NOTE: important! if used recursivelly, the application will hang up
class CBitCS
{
public:
	CBitCS()
	{
		m_cs = 0;
	}
	
	~CBitCS() {}

	inline static void Lock(volatile long* pCS, long nMask)
	{
		while (true)
		{
			long nValue = *pCS;
			long nNewValue = nValue | nMask;
			nValue &= ~nMask;

			if (nValue == CInterlocked::CompareExchange(pCS, nNewValue, nValue))
				return;

			CThread::YieldMe();
		}
	}

	inline void Lock(long nMask)
	{
		CBitCS::Lock(&m_cs, nMask);
	}

	inline static void Unlock(volatile long* pCS, long nMask)
	{
		while (true)
		{
			long nValue = *pCS;
			long nNewValue = nValue & ~nMask;			

			if (nValue == CInterlocked::CompareExchange(pCS, nNewValue, nValue))
				return;

			CThread::YieldMe();
		}		
	}

	inline void Unlock(long nMask)
	{
		CBitCS::Unlock(&m_cs, nMask);
	}

protected:
	volatile long m_cs;
};

//extended ShortCS, it supports recursive calling from one thread
class CShortCSEx
{
protected:
	volatile long m_cs;
	long m_locks;

public:

	CShortCSEx() 
	{
		m_cs = 0;
		m_locks = 0;
	}
	
	inline void Lock() 
	{
		long nCurrThreadID = (long)::GetCurrentThreadId();
		if (m_cs != nCurrThreadID)		
		{		
			while (0 != CInterlocked::CompareExchange(&m_cs, nCurrThreadID, 0))
			{
				CThread::YieldMe();
			}
		}

		m_locks++;
	}

	inline void Unlock() 
	{
		if ((--m_locks) == 0)
			m_cs = 0;		
	}
};

#endif //BES_SYNC_SHORTCS_H