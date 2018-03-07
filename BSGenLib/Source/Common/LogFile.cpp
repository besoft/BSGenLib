#include "stdafx.h"
//#include "../logfile.h"
#include <time.h>

#pragma warning (push)
#pragma warning (disable: 4996)

void CLogFile::LogEventV(LPCTSTR lpszLogFile, LPCTSTR pFormat, va_list& pArg)
{
	FILE* f = _tfopen(lpszLogFile, _T("at"));
	if (f != NULL)	
	{
		//COleDateTime tm = COleDateTime::GetCurrentTime();
		struct tm now;
		time_t ltime;
		time(&ltime);
#if _MSC_VER < 1400
		now = *localtime(&ltime);
#else
		_localtime64_s(&now, &ltime);
#endif

		_TCHAR szBuf[20];		//20 is sufficient buffer
		_tcsftime(szBuf, 20, _T("%m/%d/%Y %H:%M:%S - "), &now);
	
		CString szReport;
		szReport.FormatV(pFormat, pArg);
		szReport.Insert(0, szBuf);
		_ftprintf(f, szReport + _T('\n'));
		fclose(f);			

	}	
}

#pragma warning (pop)