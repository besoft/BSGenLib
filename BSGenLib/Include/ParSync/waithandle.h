/* ${FILENAME}
** ${LASTMODIFIED}
**
** ${COPYRIGHT}
** ${LICENCE}
**
** DESCRIPTION: Encapsulates Win32 HANDLE, functionality adopted from .NET 
*/

#ifndef BES_WAITHANDLE_NET_H
#define BES_WAITHANDLE_NET_H

#if !defined(HANDLE)
typedef void* HANDLE;
#endif

#include "SharedObj.h"

class CWaitHandle
{
public:
#ifdef _WIN32
	CWaitHandle(HANDLE hHandle)
	{
		m_hHandle = hHandle;
		m_pRefs = new CSharedObj();
		m_pRefs->AddRef();
	}
#endif

	CWaitHandle(CWaitHandle& hHandle)
	{		
		m_hHandle = hHandle.m_hHandle;
		m_pRefs = hHandle.m_pRefs;
		if (m_pRefs != NULL)
			m_pRefs->AddRef();
	}

protected:
	CWaitHandle(void)
	{
		m_hHandle = NULL;
		m_pRefs = NULL;
	}

public:
	virtual ~CWaitHandle(void)
	{
		Close();
	}

	inline void Close()	//releases all resources handles by this object
	{
		if (m_pRefs != NULL)
		{
			if (0 == m_pRefs->Release())
			{
#ifdef _WIN32
				CloseHandle(m_hHandle);	
#else
#error 'not implemented'
#endif
				delete m_pRefs;
				m_pRefs = NULL;
			}
		}
	}

	//returns true, if an object signalled
	inline bool WaitOne(DWORD dwTimeOut = INFINITE)
	{
#ifdef _WIN32
		DWORD dwRet = WaitForSingleObject(m_hHandle, dwTimeOut);
		return dwRet == WAIT_OBJECT_0;
#else
#error 'not implemented'
#endif
	}

	inline bool WaitOneMsg(DWORD dwTimeOut = INFINITE)
	{
#ifdef _WIN32

		DWORD dwRet;
		MSG msg;

		while(true)
		{
			dwRet = MsgWaitForMultipleObjects(1, &m_hHandle, FALSE, dwTimeOut, QS_ALLINPUT);
			if (dwRet == WAIT_TIMEOUT)
				return false;

			if (dwRet == WAIT_OBJECT_0)
				return true;    // The event was signaled		

			// There is one or more window message available. Dispatch them
			while(PeekMessage(&msg, 0, 0, 0, PM_NOREMOVE))
			{
				if (GetMessage(&msg, NULL, 0, 0))
				{
					TranslateMessage(&msg);				
					DispatchMessage(&msg);
				}
			}

			if (WaitForSingleObject(m_hHandle, 0) == WAIT_OBJECT_0)
				return true; // Event is now signaled.
		}
#else
#error 'not implemented'
#endif
		return false;
	}
	
	static bool WaitAll(CWaitHandle* pHandles, int nNumber, DWORD dwTimeOut = INFINITE);
	static bool WaitAny(CWaitHandle* pHandles, int nNumber, DWORD dwTimeOut = INFINITE);
	
	//duplicates handle, returns -1, if an error occurs
	static HANDLE DuplicateHandle(HANDLE hHandle);

//operators
	CWaitHandle& operator = (CWaitHandle& hHandle);
	CWaitHandle& operator = (HANDLE hHandle);

	inline operator HANDLE() {
		return DuplicateHandle(m_hHandle);
	}
protected:
	static bool WaitAll(HANDLE* pHandles, int nNumber, DWORD dwTimeOut = INFINITE);
	static bool WaitAny(HANDLE* pHandles, int nNumber, DWORD dwTimeOut = INFINITE);
//attributes
public:
	HANDLE m_hHandle;
	CSharedObj* m_pRefs;
};

#endif //BES_WAITHANDLE_NET_H