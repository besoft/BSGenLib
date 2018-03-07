/* ${FILENAME}
** ${LASTMODIFIED}
**
** ${COPYRIGHT}
** ${LICENCE}
**
** DESCRIPTION: Encapsulates Win32 HANDLE, functionality adopted from .NET 
*/

#include "stdafx.h"
//#include "waithandle.h"

bool CWaitHandle::WaitAll(HANDLE* pHandles, int nNumber, DWORD dwTimeOut)
{
#ifdef _WIN32

	DWORD dwRet;
	MSG msg;

	while(true)
	{
		dwRet = MsgWaitForMultipleObjects(nNumber, pHandles, FALSE, dwTimeOut, QS_ALLINPUT);
		if (dwRet == WAIT_TIMEOUT)
			return false;

		// There is one or more window message available. Dispatch them
		while(PeekMessage(&msg, 0, 0, 0, PM_NOREMOVE))
		{
			if (GetMessage(&msg, NULL, 0, 0))
			{
				TranslateMessage(&msg);				
				DispatchMessage(&msg);
			}
		}

		if (WaitForMultipleObjects(nNumber, pHandles, TRUE, 0) != WAIT_TIMEOUT)
			return true; // Events are now signaled.
	}
#else
#error 'not implemented'
#endif
	return false;
}

inline bool CWaitHandle::WaitAll(CWaitHandle* pHandles, int nNumber, DWORD dwTimeOut)
{
	HANDLE* pHndls = new HANDLE[nNumber];
	for (int i = 0; i < nNumber; i++)
	{
		pHndls[i] = pHandles[i].m_hHandle;
	}
	bool bRet = WaitAll(pHndls, nNumber, dwTimeOut);
	delete[] pHndls;
	return bRet;
}

bool CWaitHandle::WaitAny(HANDLE* pHandles, int nNumber, DWORD dwTimeOut)
{
#ifdef _WIN32

	DWORD dwRet;
	MSG msg;

	while(true)
	{
		dwRet = MsgWaitForMultipleObjects(nNumber, pHandles, FALSE, dwTimeOut, QS_ALLINPUT);
		if (dwRet == WAIT_TIMEOUT)
			return false;

		if (dwRet >= WAIT_OBJECT_0 && dwRet <= (WAIT_OBJECT_0 + nNumber))
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

		if (WaitForMultipleObjects(nNumber, pHandles, FALSE, 0) != WAIT_TIMEOUT)
			return true; // Event is now signaled.
	}
#else
#error 'not implemented'
#endif
	return false;
}

inline bool CWaitHandle::WaitAny(CWaitHandle* pHandles, int nNumber, DWORD dwTimeOut)
{
	HANDLE* pHndls = new HANDLE[nNumber];
	for (int i = 0; i < nNumber; i++)
	{
		pHndls[i] = pHandles[i].m_hHandle;
	}
	bool bRet = WaitAny(pHndls, nNumber, dwTimeOut);
	delete[] pHndls;
	return bRet;
}

CWaitHandle& CWaitHandle::operator = (CWaitHandle& hHandle)
{
	Close();
	m_hHandle = hHandle.m_hHandle;
	if (NULL != (m_pRefs = hHandle.m_pRefs))
		m_pRefs->AddRef();
	
	return *this;
}

CWaitHandle& CWaitHandle::operator = (HANDLE hHandle)
{
	Close();
	
	m_hHandle = hHandle;
	m_pRefs = new CSharedObj();
	m_pRefs->AddRef();	
	return *this;
}

//duplicates handle, returns -1, if an error occurs
HANDLE CWaitHandle::DuplicateHandle(HANDLE hHandle)
{
	HANDLE hProcess = ::GetCurrentProcess();

	HANDLE hRetHandle;
	if (::DuplicateHandle(hProcess, hHandle, hProcess, &hRetHandle, 
		0, TRUE, DUPLICATE_SAME_ACCESS))
		return hRetHandle;
	
	return INVALID_HANDLE_VALUE;
}