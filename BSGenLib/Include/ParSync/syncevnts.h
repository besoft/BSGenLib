/* ${FILENAME}
** ${LASTMODIFIED}
**
** ${COPYRIGHT}
** ${LICENCE}
**
** CSyncEvent
** ----------
** DESCRIPTION: Encapsulates Win32 Event, functionality adopted from .NET 
**
**
** CSyncMultiEvent
** ---------------
** DESCRIPTION: Allows to accept multiple Signal -> let us asssume two threads T1 and T2, T1 is running
**				and T2 Signals an Event for T1, as T1 is running, the event is signaled, then T2 again
**				reaches the situation that it should signal, thus we have 2 signals for T1, however,
**				standard Event support only state "Signaled" vs. "Not Signaled" => T1 can call Wait
**				only once, the second will hang it; this class handles this problem
** NOTE: supposed to have one or more signalers but only one thread for accept
*/

#ifndef BES_SYNCEVNTS_NET_H
#define BES_SYNCEVNTS_NET_H

#include "waithandle.h"
#include "interlocked.h"

class CSyncEvent : public CWaitHandle 
{
public:
	CSyncEvent(bool bAutoReset = true, bool bInitialStatus = false)
	{
#ifdef _WIN32
		m_hHandle = CreateEvent(NULL, (BOOL)!bAutoReset, (BOOL)bInitialStatus, NULL);
#else
#error 'not implemented'
#endif	
		m_pRefs = new CSharedObj();
		m_pRefs->AddRef();
	}

	inline void Set()
	{
#ifdef _WIN32
		SetEvent(m_hHandle);
#else
#error 'not implemented'
#endif
	}

	inline void Reset()
	{
#ifdef _WIN32
		ResetEvent(m_hHandle);
#else
#error 'not implemented'
#endif
	}

	inline void Pulse()
	{
#ifdef _WIN32
		PulseEvent(m_hHandle);
#else
#error 'not implemented'
#endif
	}
};


class CSyncMultiEvent
{
public:
	CSyncMultiEvent(void)
	{
		m_nSignals = 0;
	}

	inline void Reset()
	{
		m_hEvent.Pulse();
		m_nSignals = 0;
	}

	inline void Signal()
	{
		if (CInterlocked::Increment(&m_nSignals) == 1)
			m_hEvent.Set();
	}

	inline bool IsSignaled()
	{
		return m_nSignals > 0;
	}

	inline bool WaitForSignal(DWORD dwTimeOut = -1)
	{	
		int nSignals = m_nSignals;
		bool bRet = nSignals > 0;
		if (nSignals == 0)
			bRet = m_hEvent.WaitOne(dwTimeOut);

		if (m_nSignals > 0)
			CInterlocked::Decrement(&m_nSignals);
		return bRet;
	}

protected:
	volatile long m_nSignals;
	CSyncEvent m_hEvent;
};


#endif //BES_SYNCEVNTS_NET_H