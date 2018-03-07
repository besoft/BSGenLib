#pragma once

//class Path - C++ light version of .NET class System.Path
//-> see MSDN, how to use methods compatible with .NET
//-> for other methods, see comments
class RMCMNAPI Path
{
private:
	Path(void) {}
public:
	//Returns the file name and extension of the specified path string. 
	//Ex. C:\Temp\File.Ext -> File.Ext
	static CString GetFileName(LPCTSTR lpszFullPath);

	//Returns the file name and extension of the specified path string. 
	//Ex. C:\Temp\File.Ext -> File
	inline static CString GetFileNameWithoutExtension(LPCTSTR lpszFullPath);

	//Returns the file name and extension of the specified path string. 
	//Ex. C:\Temp\File.Ext -> C:\Temp\File
	static CString GetPathWithoutExtension(LPCTSTR lpszFullPath);

	//Combines two path strings. 
	static CString Combine(LPCTSTR lpszPath1, LPCTSTR lpszPath2); 

	//Changes the extension of a path string. 
	static CString ChangeExtension(LPCTSTR lpszPath, LPCTSTR lpszExtension);

	//Gets a value indicating whether the specified path string contains absolute or relative path information. 
	inline static bool IsPathRooted(LPCTSTR lpszPath);

	//Returns the absolute path for the specified path string. 
	inline static CString GetFullPath(LPCTSTR lpszPath);

	//Returns the path of the current system's temporary folder. 
	inline static CString GetTempPath();	

	//Creates a uniquely named, zero-byte temporary file on disk and returns the 
	//full path of that file. 
	//RELEASE_NOTE: in my implementation, the file is not created, this method
	//returns just full path
	static CString GetTempFileName();

	//Returns the path of a temporary directory to be created in the current system's 
	//temporary folder. Prefix will be a part of path
	static CString GetTempDirectory(LPCTSTR lpszPrefix = _T(""));
	
	//Returns the directory information for the specified path string
	//RELASE NOTE: unlike .NET, it does not return NULL if root folder is given
	//in lpszFullPath parameter, but it returns ""
	//Ex. "C:\Temp\File.Ext" -> "C:\Temp\" 
	static CString GetDirectoryName(LPCTSTR lpszFullPath);

	//Gets the root directory information of the specified path. 
	static CString GetPathRoot(LPCTSTR lpszPath);

	//Returns the extension of the specified path string. 
	//Ex. C:\Temp\File.Ext -> .Ext
	static CString GetExtension(LPCTSTR lpszPath);
	

	//Converts the path given in lpszPath in such a manner that it is relative to lpszRefPath. 
	//if lpszRefPath is NULL, the function returns lpszPath relative to the current
	//directory. e.g., for "C:\Moje\Data", "C:\Moje" it returns "Data"	
	//Note: Both lpszPath and lpszRefPath may be relative to the current directory		
	static CString GetRelativePath(LPCTSTR lpszPath, LPCTSTR lpszRefPath = NULL);
};

//implementation
inline CString Path::GetFullPath(LPCTSTR lpszPath)
{
	CString szRet;
	::GetFullPathName(lpszPath, MAX_PATH, szRet.GetBuffer(MAX_PATH), NULL);
	szRet.ReleaseBuffer();
	return szRet;
}

//Creates a uniquely named, zero-byte temporary file on disk and returns the 
//full path of that file. 
//RELEASE_NOTE: in my implementation, the file is not created, this method
//returns just full path
inline CString Path::GetTempFileName()
{
	CString szRet;
	::GetTempFileName(GetTempPath(), _T("BSNet"), 0, szRet.GetBuffer(MAX_PATH));
	szRet.ReleaseBuffer();
	return szRet;
}
inline CString Path::GetTempPath()
{
	CString szRet;
	::GetTempPath(MAX_PATH, szRet.GetBuffer(MAX_PATH));
	szRet.ReleaseBuffer();
	return szRet;
}
inline bool Path::IsPathRooted(LPCTSTR lpszPath)
{
	if (lpszPath == NULL || lpszPath[0] == _T('\0') || lpszPath[1] == _T('\0'))	
		return false;
	else
		return (lpszPath[0] == _T('\\') && lpszPath[1] == _T('\\')) || lpszPath[1] == _T(':');
}

//Returns the file name and extension of the specified path string. 
//Ex. C:\Temp\File.Ext -> File
inline /*static*/ CString Path::GetFileNameWithoutExtension(LPCTSTR lpszFullPath) {
	return GetPathWithoutExtension(GetFileName(lpszFullPath));
}