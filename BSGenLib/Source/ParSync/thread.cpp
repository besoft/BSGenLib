/* ${FILENAME}
** ${LASTMODIFIED}
**
** ${COPYRIGHT}
** ${LICENCE}
**
** DESCRIPTION: Thread
*/

#include "StdAfx.h"
//#include "thread.h"

//this method allows the controled termination of an associated thread:
//it throws the _com_error exception E_ABORT, that can be handled by the 
//associated thread in order to release allocated resources 
//NOTE: the implementation of code may make a thread immune to Abort
bool CThread::Abort()
{
	bool bResult = false;	

#if !defined(_WIN64)
	//HARAKIRI	
	CONTEXT context;
	if (::SuspendThread(m_hHandle) != (DWORD)-1)		
	{
		context.ContextFlags = CONTEXT_CONTROL;		
		if (::GetThreadContext(m_hHandle, &context))
		{
			context.Eip = (DWORD)((DWORD_PTR)throw_killing_exception);
			if (::SetThreadContext(m_hHandle, &context))
				bResult = true;
		}

		::ResumeThread(m_hHandle);
	}
#endif

	return bResult;
}