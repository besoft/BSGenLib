#include "stdafx.h"
#include <shellapi.h>


//spusti externi soubor a pocka na jeho dokonceni pokud bWait == true
/*static*/ bool CFileUtils::Exec(LPCTSTR lpszFile, LPCTSTR lpszArgs, LPCTSTR lpszDir, bool bMinimized, bool bWait)
{
	CString szStr;
	::GetFullPathName(lpszFile, MAX_PATH, szStr.GetBuffer(MAX_PATH), NULL);
	szStr.ReleaseBuffer();

	SHELLEXECUTEINFO si;
	memset(&si, 0, sizeof(si));
	si.cbSize = sizeof(si);
	si.fMask = SEE_MASK_NOCLOSEPROCESS;
	si.lpVerb = _T("open");
	si.lpFile = szStr;
	si.lpParameters = lpszArgs;
	si.lpDirectory = lpszDir;
	si.nShow = bMinimized ? SW_SHOWMINNOACTIVE : SW_SHOWNORMAL;

	if (!::ShellExecuteEx(&si)) 
		return false;

	if (bWait)
	{
		DWORD dwCode;
		do
		{		
			GetExitCodeProcess(/*pi*/si.hProcess, &dwCode);
			if (dwCode == STILL_ACTIVE) 
				::Sleep(500);
		} 
		while (dwCode == STILL_ACTIVE);
		CloseHandle(si.hProcess);	//BES 25.9.2003 - viz MSDN
	}

	return true;
}