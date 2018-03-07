#pragma once
#ifndef BES_SYNCLOCK
#define BES_SYNCLOCK

#include "interlocked.h"
#include "syncevnts.h"
class CSyncLock
{
public:

	CSyncLock(void)
	{
		m_nLock = 0;
	}

	~CSyncLock(void)
	{
		if (m_nLock < 0)
			m_hEvent.Set();	//release waiting thread
	}

	inline void Signal();
	inline void WaitForSignal();

protected:
	CSyncEvent m_hEvent;
	volatile long m_nLock;
};

void CSyncLock::Signal()
{
	if (0 == CInterlocked::Increment(&m_nLock))
		m_hEvent.Set();
}

void CSyncLock::WaitForSignal()
{
	if (-1 == CInterlocked::Decrement(&m_nLock))
		m_hEvent.WaitOne();
}

#endif //BES_SYNCLOCK