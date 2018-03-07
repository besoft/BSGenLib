#pragma once
class CFileUtils
{
public:

	//spusti externi soubor a pocka na jeho dokonceni pokud bWait == true
	static bool Exec(LPCTSTR lpszFile, LPCTSTR lpszArgs, LPCTSTR lpszDir = NULL, bool bMinimized = true, bool bWait = true);
};