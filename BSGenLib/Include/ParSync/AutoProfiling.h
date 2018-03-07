#pragma once

//this file contains macros and classes for automatic profiling of applications

//#define __PROFILING__				
//__PROFILING__ enables automatic profiling

//#define __PROFILING_FUNC_RDTSC	
//__PROFILING_FUNC_RDTSC enables use of faster but possibly incorrect RDTSC instruction
//see also: http://msdn2.microsoft.com/en-us/library/bb173458.aspx

//#define __PROFILING_LEVEL		0
//profiling level, denotes what to profile

#ifdef  __PROFILING_FUNC_RDTSC
#include <intrin.h>
#pragma intrinsic(__rdtsc)
#endif

//this class is used for the measuring of time spend in some function
//IMPORTANT NOTE 1: recursive functions are not supported
//IMPORTANT NOTE 2: class is not thread safe, i.e., it must be used only with 
//profiler that was initiated with bThreadProfiling = true, or 
//the caller has to use any mechanism to avoid concurrency
//IMPORTANT NOTE 3: if compiled with __PROFILING_FUNC_RDTSC, the overhead
//of this class is significantly lower, however, on multiprocessor
//with two different processors, the measured value may be incorrect
class CProfileFunction
{
protected:
	CProfileFuncTimer* m_pProf;
		
public:
	CProfileFunction(CProfiler* pProf, LPCTSTR lpszName)
	{
		pProf->m_cs.Lock();
		m_pProf = (CProfileFuncTimer*)pProf->GetProfVar(lpszName, CProfiler::ProfilerCategory::Functions);		
		pProf->m_cs.Unlock();

#ifdef __PROFILING_FUNC_RDTSC
		m_pProf->m_liStart.QuadPart = __rdtsc();		
#else		
		m_pProf->Start();
#endif
	}

	~CProfileFunction() 
	{		
#ifdef __PROFILING_FUNC_RDTSC
		m_pProf->m_liEnd.QuadPart = __rdtsc();
		m_pProf->m_liElapsed.QuadPart += m_pProf->m_liEnd.QuadPart - m_pProf->m_liStart.QuadPart;
		m_pProf->m_nCount++;
#else
		m_pProf->Stop();
#endif		
	}
};

#ifdef __PROFILING__
class CDefaultProfiler : public CProfiler {
public:
	CDefaultProfiler(bool bThreadProfiling = false) : 
		CProfiler(bThreadProfiling) {
		g_pDefaultProfiler = this;
	}

	~CDefaultProfiler() {
		SaveProfile(_T("Profile_Log.xml"));
		g_pDefaultProfiler = NULL;
	}
public:
	static CDefaultProfiler g_DefaultProfiler;	
	static CProfiler* g_pDefaultProfiler;	
};

#define __PROFILING_DECLARE_DEFAULT_PROFILER(bThreadProfiling) \
	CDefaultProfiler CDefaultProfiler::g_DefaultProfiler(bThreadProfiling);	\
	CProfiler* CDefaultProfiler::g_pDefaultProfiler = NULL;


//Macros for the profiling with the specified profiler and string variable (can be CString),
//i.e., the caller is supposed to format the variable
//e.g. EX_PROFILE_COUNTER_INC_S(pMyProfiler, _T("MyCounter"));
#define EX_PROFILE_COUNTER_INC_S(profiler, var)		if ((profiler) != NULL) (profiler)->Inc(var)
#define EX_PROFILE_COUNTER_DEC_S(profiler, var)		if ((profiler) != NULL) (profiler)->Dec(var)
#define EX_PROFILE_TIMER_START_S(profiler, var)		if ((profiler) != NULL) (profiler)->Start(var)
#define EX_PROFILE_TIMER_STOP_S(profiler, var)		if ((profiler) != NULL) (profiler)->Stop(var)
#define EX_PROFILE_FUNCTION_S(profiler, var)		CProfileFunction _prf_func(profiler, var)

//Macros for the profiling with the default profiler and string variable (can be CString),
//i.e., the caller is supposed to format the variable
//e.g. PROFILE_COUNTER_INC_S(_T("MyCounter"));
#define PROFILE_COUNTER_INC_S(var)	EX_PROFILE_COUNTER_INC_S(CDefaultProfiler::g_pDefaultProfiler, var)
#define PROFILE_COUNTER_DEC_S(var)	EX_PROFILE_COUNTER_DEC_S(CDefaultProfiler::g_pDefaultProfiler, var)
#define PROFILE_TIMER_START_S(var)	EX_PROFILE_TIMER_START_S(CDefaultProfiler::g_pDefaultProfiler, var)
#define PROFILE_TIMER_STOP_S(var)	EX_PROFILE_TIMER_STOP_S(CDefaultProfiler::g_pDefaultProfiler, var)
#define PROFILE_FUNCTION_S(var)		EX_PROFILE_FUNCTION_S(CDefaultProfiler::g_pDefaultProfiler, var)

//Macros for the profiling with the specified profiler and literal variable (it is automatically formatted)
//e.g., EX_PROFILE_COUNTER_INC(pMyProfiler, MyCounter)
#define EX_PROFILE_COUNTER_INC(profiler, var)		EX_PROFILE_COUNTER_INC_S(profiler, _T(#var))
#define EX_PROFILE_COUNTER_DEC(profiler, var)		EX_PROFILE_COUNTER_DEC_S(profiler, _T(#var))
#define EX_PROFILE_TIMER_START(profiler, var)		EX_PROFILE_TIMER_START_S(profiler, _T(#var))
#define EX_PROFILE_TIMER_STOP(profiler, var)		EX_PROFILE_TIMER_STOP_S(profiler, _T(#var))
#define EX_PROFILE_FUNCTION(profiler, var)			EX_PROFILE_FUNCTION_S(profiler, _T(#var "\t(") + Path::GetFileName(_T(__FILE__)) + _T(" [") MAKESTRING(__LINE__) _T("])"))
#define EX_PROFILE_THIS_FUNCTION(profiler)			EX_PROFILE_FUNCTION_S(profiler, _T(__FUNCTION__) _T("\t(") + Path::GetFileName(_T(__FILE__)) + _T(" [") MAKESTRING(__LINE__) _T("])"))

//Macros for the profiling with the default profiler and literal variable (it is automatically formatted)
//e.g., PROFILE_FUNCTION(CMyApp::CMyApp)
#define PROFILE_COUNTER_INC(var)	EX_PROFILE_COUNTER_INC(CDefaultProfiler::g_pDefaultProfiler, var)
#define PROFILE_COUNTER_DEC(var)	EX_PROFILE_COUNTER_DEC(CDefaultProfiler::g_pDefaultProfiler, var)
#define PROFILE_TIMER_START(var)	EX_PROFILE_TIMER_START(CDefaultProfiler::g_pDefaultProfiler, var)
#define PROFILE_TIMER_STOP(var)		EX_PROFILE_TIMER_STOP(CDefaultProfiler::g_pDefaultProfiler, var)
#define PROFILE_FUNCTION(var)		EX_PROFILE_FUNCTION(CDefaultProfiler::g_pDefaultProfiler, var)
#define PROFILE_THIS_FUNCTION()		EX_PROFILE_THIS_FUNCTION(CDefaultProfiler::g_pDefaultProfiler)

#else
#define __PROFILING_DECLARE_DEFAULT_PROFILER(bThreadProfiling)

#define EX_PROFILE_COUNTER_INC_S(profiler, var)		
#define EX_PROFILE_COUNTER_DEC_S(profiler, var)		
#define EX_PROFILE_TIMER_START_S(profiler, var)		
#define EX_PROFILE_TIMER_STOP_S(profiler, var)		
#define EX_PROFILE_FUNCTION_S(profiler, var)		

#define PROFILE_COUNTER_INC_S(var)	
#define PROFILE_COUNTER_DEC_S(var)	
#define PROFILE_TIMER_START_S(var)	
#define PROFILE_TIMER_STOP_S(var)	
#define PROFILE_FUNCTION_S(var)		

#define EX_PROFILE_COUNTER_INC(profiler, var)	
#define EX_PROFILE_COUNTER_DEC(profiler, var)	
#define EX_PROFILE_TIMER_START(profiler, var)	
#define EX_PROFILE_TIMER_STOP(profiler, var)	
#define EX_PROFILE_FUNCTION(profiler, var)		
#define EX_PROFILE_THIS_FUNCTION(profiler)		

#define PROFILE_COUNTER_INC(var)	
#define PROFILE_COUNTER_DEC(var)	
#define PROFILE_TIMER_START(var)	
#define PROFILE_TIMER_STOP(var)		
#define PROFILE_FUNCTION(var)		
#define PROFILE_THIS_FUNCTION()		
#endif

//#include "AutoProfiling_M.h"

#ifndef __PROFILING_LEVEL
#define __PROFILING_LEVEL 0
#endif

#if __PROFILING_LEVEL <= 0
#define PROFILE_L0(macro)	macro
#else
#define PROFILE_L0(macro)	__noop
#endif

#if __PROFILING_LEVEL <= 1
#define PROFILE_L1(macro)	macro
#else
#define PROFILE_L1(macro)	__noop
#endif

#if __PROFILING_LEVEL <= 2
#define PROFILE_L2(macro)	macro
#else
#define PROFILE_L2(macro)	__noop
#endif

#if __PROFILING_LEVEL <= 3
#define PROFILE_L3(macro)	macro
#else
#define PROFILE_L3(macro)	__noop
#endif

#if __PROFILING_LEVEL <= 4
#define PROFILE_L4(macro)	macro
#else
#define PROFILE_L4(macro)	__noop
#endif

#if __PROFILING_LEVEL <= 5
#define PROFILE_L5(macro)	macro
#else
#define PROFILE_L5(macro)	__noop
#endif

#if __PROFILING_LEVEL <= 6
#define PROFILE_L6(macro)	macro
#else
#define PROFILE_L6(macro)	__noop
#endif

#if __PROFILING_LEVEL <= 7
#define PROFILE_L7(macro)	macro
#else
#define PROFILE_L7(macro)	__noop
#endif

