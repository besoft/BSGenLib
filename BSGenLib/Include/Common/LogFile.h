#pragma once

class CLogFile
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

	static void LogEventV(LPCTSTR lpszLogFile, LPCTSTR pFormat, va_list& pArg);
};