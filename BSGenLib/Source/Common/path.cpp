#include "stdafx.h"
//#include "..///Common/path.h"

CString Path::Combine(LPCTSTR lpszPath1, LPCTSTR lpszPath2)
{
	if (*lpszPath1 == _T('\0'))
		return lpszPath2;

	if (*lpszPath2 == _T('\0'))
		return lpszPath1;

	if (IsPathRooted(lpszPath2))
		return lpszPath2;

	CString szPath1 = lpszPath1;
	szPath1.TrimRight(_T('\\'));
	return (szPath1 + _T('\\')) + lpszPath2;
}

/*static*/ CString Path::GetFileName(LPCTSTR lpszFullPath)
{
	CString szRet = lpszFullPath;
	int iPos = szRet.ReverseFind(_T('\\'));
	if (iPos >= 0)
		szRet.Delete(0, iPos + 1);

	return szRet;
}

//Returns the directory information for the specified path string
//RELASE NOTE: unlike .NET, it does not return NULL if root folder is given
//in lpszFullPath parameter, but it returns ""
/*static*/ CString Path::GetDirectoryName(LPCTSTR lpszFullPath)
{
	//invalid call
	if (lpszFullPath == NULL || lpszFullPath[0] == _T('\0'))
		return _T("");

	//check if root (X:\) is specified
	if (lpszFullPath[1] == _T(':') && lpszFullPath[2] == _T('\\') &&
		lpszFullPath[3] == _T('\0'))
		return _T("");

	//check if network root (\\SERVER_NAME\DRIVE) is specified
	if (lpszFullPath[0] == _T('\\') && lpszFullPath[1] == _T('\\'))
	{
		int nSlashes = 0;
		LPCTSTR lpszCur = lpszFullPath;
		while (*lpszCur)
		{
			if (*lpszCur == _T('\\'))
				nSlashes++;
			lpszCur++;
		}

		if (nSlashes == 3)
			return _T("");
	}

	CString szFullPath = lpszFullPath;	
	int iPos = szFullPath.ReverseFind(_T('\\'));
	if (iPos < 0)
		szFullPath.Empty();
	else
		szFullPath = szFullPath.Left(iPos);	

	return szFullPath;
}

//Returns the path of a temporary directory to be created in the current system's 
//temporary folder. Prefix will be a part of path
/*static*/ CString Path::GetTempDirectory(LPCTSTR lpszPrefix)
{
	CString szDirName = lpszPrefix;
	szDirName.AppendFormat(_T("_%x"), ::GetTickCount());
	return Combine(GetTempPath(), szDirName);
}

//Returns the extension of the specified path string. 
/*static*/ CString Path::GetExtension(LPCTSTR lpszPath)
{
	CString szExt = lpszPath;
	int iPos = szExt.ReverseFind(_T('.'));
	if (iPos >= 0)	
		szExt.Delete(0, iPos);
	else
		szExt.Empty();

	return szExt;
}

//Returns the file name and extension of the specified path string. 
//Ex. C:\Temp\File.Ext -> C:\Temp\File
/*static*/ CString Path::GetPathWithoutExtension(LPCTSTR lpszFullPath)
{
	CString szRet = lpszFullPath;
	int iPos = szRet.ReverseFind(_T('.'));
	if (iPos < 0)
		return szRet;
	
	return szRet.Left(iPos);	
}

//Gets the root directory information of the specified path. 
/*static*/ CString Path::GetPathRoot(LPCTSTR lpszPath)
{
	if (lpszPath == NULL || lpszPath[0] == _T('\0'))
		return _T("");
	
	if (lpszPath[0] == _T('\\'))
	{
		//\ or \\SERVER\SHARE
		if (lpszPath[1] != _T('\\'))
			return _T("\\");

		CString szRet = lpszPath;
		return szRet.Left(szRet.Find(_T('\\'), szRet.Find(_T('\\'), 2) + 1));
	}

	if (lpszPath[1] == _T(':'))
	{
		//X:\ or X:
		CString szRet = lpszPath;
		if (szRet.GetLength() >= 2 && szRet[2] == _T('\\'))
			return szRet.Left(3);
		else
			return szRet.Left(2);
	}	
	
	return _T("");
}

//Converts the path given in lpszPath in such a manner that it is relative to lpszRefPath. 
//if lpszRefPath is NULL, the function returns lpszPath relative to the current
//directory. e.g., for "C:\Moje\Data", "C:\Moje" it returns "Data"	
//Note: Both lpszPath and lpszRefPath may be relative to the current directory			
/*static*/ CString Path::GetRelativePath(LPCTSTR lpszPath, LPCTSTR lpszRefPath)
{
	CString szPath = GetFullPath(lpszPath);
	szPath.MakeLower();

	CString szRefPath;
	if (lpszRefPath != NULL)
		szRefPath = GetFullPath(lpszRefPath);
	else
	{
		::GetCurrentDirectory(MAX_PATH, szRefPath.GetBuffer(MAX_PATH));
		szRefPath.ReleaseBuffer();
	}
	szRefPath.MakeLower();

	//check if the drives are the same
	//Note: as paths in szPath and szRefPath are absolute it must be rooted
	if (GetPathRoot(szPath) != GetPathRoot(szRefPath))
		return szPath;

	CString szTest = szRefPath;
	while (szPath.Find(szTest) != 0)	//in the worst case only drive remains
	{
		szTest = szTest.Left(szTest.ReverseFind(_T('\\')));
	}

	szRefPath.Delete(0, szTest.GetLength());
	szPath.Delete(0, szTest.GetLength());
	szRefPath.TrimLeft(_T('\\'));
	szPath.TrimLeft(_T('\\'));

	if (!szRefPath.IsEmpty())
	{
		//some ../ must be added
		int nCount = szRefPath.Replace(_T('\\'), _T('/'));
		while (nCount >= 0)
		{
			szPath.Insert(0, _T("..\\"));
			nCount--;
		}
	}		

	return szPath;
}

//Changes the extension of a path string. 
/*static*/ CString Path::ChangeExtension(LPCTSTR lpszPath, LPCTSTR lpszExtension)
{
	CString szRet = lpszPath;
	int iPos = szRet.ReverseFind(_T('.'));
	if (iPos >= 0)	
		szRet.Delete(iPos, szRet.GetLength() - iPos);
	
	if (*lpszExtension != _T('.'))
		szRet += _T('.');

	return szRet + lpszExtension;
}