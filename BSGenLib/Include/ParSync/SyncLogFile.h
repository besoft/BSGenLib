#pragma once
#include "../Common/LogFile.h"
#include "ShortCS.h"

//Synchronized version of CLogFile
//It avoids simultaneous logging
class CSyncLogFile
{
public:
	inline static void LogEvent(LPCTSTR pFormat, ...)
	{
		va_list pArg;
		va_start(pArg, pFormat);
		LogEventV(_T("logfile.log"), pFormat, pArg);
		va_end(pArg);
	}

	inline static void LogEventEx(LPCTSTR lpszLogFile, LPCTSTR pFormat, ...)
	{
		va_list pArg;
		va_start(pArg, pFormat);
		LogEventV(lpszLogFile, pFormat, pArg);
		va_end(pArg);
	}

	inline static void LogEventV(LPCTSTR lpszLogFile, LPCTSTR pFormat, va_list& pArg) 
	{
		static volatile long cs = 0;
		CShortCS::Lock(&cs);
		
		CLogFile::LogEventEx(lpszLogFile, pFormat, pArg);

		CShortCS::Unlock(&cs);
	}
};