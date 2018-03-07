/* ${FILENAME}
** ${LASTMODIFIED}
**
** ${COPYRIGHT}
** ${LICENCE}
**
** DESCRIPTION: Some standard parallel stuff
*/

#ifndef BES_PARSTUFF_H
#define BES_PARSTUFF_H

typedef DWORD (__stdcall *THREADSTARTPROC_PTR)(void* lParam);
#define STARTTHREAD2(clsname, num)	CThread::Start(clsname::StartProc##num, this)
#define STARTTHREAD()		Start()
#define THREADSTARTPROC_DEF(x)	static DWORD __stdcall x(void* lParam)
#define THREADSTARTPROC_IMP(y, x)	static DWORD __stdcall y::x(void* lParam)
#define THREADSTARTPROC2(clsname, func, num) THREADSTARTPROC_DEF(StartProc##num) { \
						return (DWORD)((clsname*)lParam)->func(); }
#define THREADSTARTPROC(clsname, func)	THREADSTARTPROC_DEF(StartProc) { \
						return (DWORD)((clsname*)lParam)->func(); }

#include "waithandle.h"

class CThread : public CWaitHandle
{
//*** object self-live part
public: 
	CThread(CThread& hThread) : CWaitHandle((CWaitHandle&)hThread)
	{
	}

	CThread(CWaitHandle& hThread) : CWaitHandle(hThread)
	{
	}

	CThread(HANDLE hThread) : CWaitHandle(hThread)
	{		
	}

	CThread() {  
	}
	
	inline void Start()
	{
		Close();

#ifdef _WIN32
		DWORD dwDummy;
		m_hHandle = CreateThread(NULL, 0, StartProc, this, 0, &dwDummy);
		m_pRefs = new CSharedObj;
		m_pRefs->AddRef();
#else
#error 'not implemented'
#endif
	}

	//waits until the associated thread ends
	inline void Join() { 
		WaitOne(); 
	}

	//waits until the associated thread ends or dwWaitMilliseconds ms
	//returns false, if dwWaitMilliseconds elapsed (thread is still alive)
	inline bool Join(DWORD dwWaitMilliseconds) { 
		return WaitOne(dwWaitMilliseconds); 
	}

	//violently terminates the run of thread
	//RELEASE NOTE: avoid this function
	inline void Terminate() {
		::TerminateThread(m_hHandle, -1L);
	}

	//this method allows the controled termination of an associated thread:
	//it throws the _com_error exception E_ABORT, that can be handled by the 
	//associated thread in order to release allocated resources 
	//returns false, if the abortion could not be initiated, truw otherwise
	//NOTE: the implementation of code may make a thread immune to Abort
	bool Abort();

	//just an alias to Abort
	inline bool Harakiri() {
		return Abort();
	}


//operators
	inline operator HANDLE() {
		return DuplicateHandle(m_hHandle);		
	}

	inline CThread& operator = (CThread& hThread) {
		CWaitHandle::operator =((CWaitHandle&)hThread);
		return *this;
	}

	inline CThread& operator = (CWaitHandle& hHandle) {
		CWaitHandle::operator =(hHandle);
		return *this;
	}

	inline CThread& operator = (HANDLE hHandle) {
		CWaitHandle::operator =(hHandle);
		return *this;
	}
protected:
	THREADSTARTPROC(CThread, Run);

//overides
	virtual int Run() {BES_ASSERT(false); return 0; }


//*** object global part	
public:
	inline static CThread GetCurrentThread()
	{
#ifdef _WIN32
		HANDLE hRetHandle;
		HANDLE hThisHandle = ::GetCurrentThread();		
		BES_VERIFY(::DuplicateHandle(GetCurrentProcess(), hThisHandle, GetCurrentProcess(), &hRetHandle, 0, TRUE, DUPLICATE_SAME_ACCESS));
		return CThread(hRetHandle);
#else
#error 'not implemented'
#endif
	}
	
	inline static CThread Start(THREADSTARTPROC_PTR lpfnStartRoutine, void* lParam)
	{		
#ifdef _WIN32
		DWORD dwDummy;
		HANDLE hHandle = CreateThread(NULL, 0, lpfnStartRoutine, lParam, 0, &dwDummy);
		return hHandle;
#else
#error 'not implemented'		
#endif		
	}
	
	inline static void YieldMe() 
	{
#ifdef _DEBUG___
		_asm
		{
			nop;
		}
#else
		::Sleep(0);
#endif
	}

	inline static void Sleep(DWORD dwMilliseconds)
	{
#ifdef _WIN32
		::Sleep(dwMilliseconds);
#else
		GetCurrentThread().Join(dwMilliseconds);		
#endif
	}

	inline static void Wait(CWaitHandle& hEvent, DWORD dwWaitMilliseconds = INFINITE)
	{
		hEvent.WaitOne(dwWaitMilliseconds);
	}

	inline static void WaitPSA(volatile long* pVar, long pReqValue = 1)
	{
		while (pReqValue != CInterlocked::CompareExchange(pVar, pReqValue, pReqValue))
		{
			YieldMe();
		}
	}

protected:
	__declspec(noinline) static void throw_killing_exception() {		
		_com_issue_error(HRESULT_FROM_WIN32(E_ABORT));
	}
};
#endif //BES_PARSTUFF_H
