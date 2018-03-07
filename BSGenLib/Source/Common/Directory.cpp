#include "stdafx.h"

//gets all files  in the given directory that match the given mask
CArrayOfStrings* Directory::GetFiles(LPCTSTR lpszDirectory, LPCTSTR lpszMask, bool bRecursive)
{
	CArrayOfStrings* pRet = Directory::GetFiles(lpszDirectory, lpszMask);

	if (bRecursive)
	{		
		CObjPtr<CArrayOfStrings> pSubDirs = Directory::GetDirectories(lpszDirectory);
		for (int i = 0; i < (int)pSubDirs->GetCount(); i++)
		{
			CArrayOfStrings* pFiles = Directory::GetFiles(pSubDirs->GetAt(i), lpszMask, bRecursive);
			if (pFiles != NULL)
			{
				if (pRet == NULL)
					pRet = pFiles;
				else
				{
					pRet->Append(*pFiles);
					delete pFiles;
				}
			}
		}
	}

	return pRet;
}

//gets all directories in the given directory
CArrayOfStrings* GetDirectories(LPCTSTR lpszDirectory, bool bRecursive)
{
	CArrayOfStrings* pRet = Directory::GetDirectories(lpszDirectory);
	if (bRecursive)
	{
		int i = 0;
		while (i < (int)pRet->GetCount())
		{
			CArrayOfStrings* pDirs = Directory::GetDirectories(pRet->GetAt(i));
			if (pDirs != NULL)
			{
				if (pRet == NULL)
					pRet = pDirs;
				else
				{
					pRet->Append(*pDirs);
					delete pDirs;
				}
			}

			i++;
		}		
	}	

	return pRet;
}

CArrayOfStrings* Directory::GetFiles(LPCTSTR lpszDirectory, LPCTSTR lpszMask)
{
	CArrayOfStrings* pretArr = new CArrayOfStrings();

	WIN32_FIND_DATA FindFileData;
	HANDLE hFind;
	
	CString szFld = lpszDirectory; 
	szFld.TrimRight(_T('\\'));
	szFld += _T('\\');

	hFind = FindFirstFile(szFld + lpszMask, &FindFileData);
	if (hFind != INVALID_HANDLE_VALUE)
	{
		do
		{	 
			if ((FindFileData.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) != FILE_ATTRIBUTE_DIRECTORY)			
				pretArr->Add(szFld + FindFileData.cFileName);
		} 
		while (FindNextFile(hFind, &FindFileData));
		FindClose(hFind);
	}	

	return pretArr;
}

CArrayOfStrings* Directory::GetDirectories(LPCTSTR lpszDirectory)
{
	CArrayOfStrings* pretArr = new CArrayOfStrings();

	WIN32_FIND_DATA FindFileData;
	HANDLE hFind;
	
	CString szFld = lpszDirectory; 
	szFld.TrimRight(_T('\\'));
	szFld += _T('\\');

	hFind = FindFirstFile(szFld + _T("*"), &FindFileData);
	if (hFind != INVALID_HANDLE_VALUE)
	{
		do
		{	 
			if ((FindFileData.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) == FILE_ATTRIBUTE_DIRECTORY && 
				_tcscmp(FindFileData.cFileName, _T(".")) != 0 && 
				_tcscmp(FindFileData.cFileName, _T("..")) != 0)
				pretArr->Add(szFld + FindFileData.cFileName);
		} 
		while (FindNextFile(hFind, &FindFileData));
		FindClose(hFind);
	}	

	return pretArr;
}

/////////////////////////////////////////////////////////////////
// zalozi adresar na lpszPath (resp vytvori celou cestu)
// je-li bUnique = TRUE je nepripustne, aby jiz adresar existovat
// a musi byt vytvoren novy pridanim cislovky k nazvu adresare
CString Directory::CreateDirectory(LPCTSTR lpszDirectory, bool bUnique)
{	
	CString szPath = lpszDirectory;
	CString szRet;
	CString szDir;
	int level = 0;
	
	while (!szPath.IsEmpty())
	{
		int pos = szPath.Find('\\');
		if (pos >= 0) 
		{
			szDir = szPath.Left(pos);
			szPath.Delete(0, pos + 1);
		}
		else 
		{
			szDir = szPath;
			szPath.Empty();
		}

		szRet += szDir;		

		if (level > 0 || szDir[szDir.GetLength() - 1] != _T(':'))
		{
			/// neni to specifikace adresare, C:\		
			::SetLastError(0);
			::CreateDirectory(szRet, (LPSECURITY_ATTRIBUTES) NULL);
			if (szPath.IsEmpty())
			{
				DWORD dwErr = GetLastError();
				if (bUnique && dwErr == ERROR_ALREADY_EXISTS)
				{
					CString szPom = szRet;
					int n = 1;					
					do
					{
						szRet.Format(_T("%s (%d)"), szPom, ++n);
						::SetLastError(0);
						::CreateDirectory(szRet,(LPSECURITY_ATTRIBUTES)NULL);
					} while ((dwErr = GetLastError()) == ERROR_ALREADY_EXISTS);
				}

				if (dwErr == 0 || dwErr == ERROR_ALREADY_EXISTS) return szRet;
				return _T("");
			}
		}
		else if (level == 0 && szPath.IsEmpty()) 
			return szRet; /// ten debil top chce na root 
		level++;

		szRet += _T('\\');
	}

	return _T("");
}

void Directory::Delete(LPCTSTR lpszDirectory, bool bRecursive)
{
	BOOL bOK = TRUE;
	WIN32_FIND_DATA FindFileData;
	HANDLE hFind;

	CString szFld = lpszDirectory; 
	szFld += _T("\\");

	hFind = FindFirstFile(szFld + _T("*"), &FindFileData);
	if (hFind != INVALID_HANDLE_VALUE)
	{
		do
		{	 
			if ((FindFileData.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) == FILE_ATTRIBUTE_DIRECTORY)
			{
				if (!(_tcscmp(FindFileData.cFileName, _T(".")) == 0 ||
					_tcscmp(FindFileData.cFileName, _T("..")) == 0))
					Delete(szFld + FindFileData.cFileName, bRecursive);
			}
			else 
				::DeleteFile(szFld + FindFileData.cFileName);

		} while (FindNextFile(hFind, &FindFileData));
		FindClose(hFind);
	}

	Delete(lpszDirectory);
}

bool Directory::Copy(LPCTSTR lpszFromPath, LPCTSTR lpszToPath)
{
	CreateDirectory(lpszToPath);
	CString szTo = lpszToPath;
	szTo += _T('\\');

	CObjPtr<CArrayOfStrings> pArray = GetFiles(lpszFromPath);
	for (int i = 0; i < (int)pArray->GetCount(); i++)
	{
		CString szFrom = pArray->GetAt(i);
		if (!CopyFile(szFrom, szTo + Path::GetFileName(szFrom), FALSE))
			return false;		
	}
	
	pArray = GetDirectories(lpszFromPath);
	for (int i = 0; i < (int)pArray->GetCount(); i++)
	{
		CString szFrom = pArray->GetAt(i);
		if (!Copy(szFrom, szTo + Path::GetFileName(szFrom)))
			return false;
	}

	return true;
}

bool Directory::Move(LPCTSTR lpszFromPath, LPCTSTR lpszToPath)
{
	CreateDirectory(lpszToPath);
	CString szTo = lpszToPath;
	szTo += _T('\\');

	CObjPtr<CArrayOfStrings> pArray = GetFiles(lpszFromPath);
	for (int i = 0; i < (int)pArray->GetCount(); i++)
	{
		CString szFrom = pArray->GetAt(i);
		if (!MoveFile(szFrom, szTo + Path::GetFileName(szFrom)))
			return false;		
	}
	
	pArray = GetDirectories(lpszFromPath);
	for (int i = 0; i < (int)pArray->GetCount(); i++)
	{
		CString szFrom = pArray->GetAt(i);
		if (!Move(szFrom, szTo + Path::GetFileName(szFrom)))
			return false;
	}

	return true;
}

//Sets the application's current working directory to the directory containing
//the currently running application
/*static*/ void Directory::SetCurrentDirectoryToAppDir()
{
	char buf[MAX_PATH];

	//RELEASE NOTE: na W98 tohle nefunguje jako UNICODE
	::GetModuleFileNameA(NULL, buf, MAX_PATH);
	strrchr(buf, '\\')[0] = '\0';
	SetCurrentDirectoryA(buf);
}