#pragma once

//class Directory - C++ light version of .NET class System.Path
//-> see MSDN, how to use methods compatible with .NET
//-> for other methods, see comments
class Directory
{
private:
	Directory(void) {}
public:
	//gets all files  in the given directory that match the given mask
	static CArrayOfStrings* GetFiles(LPCTSTR lpszDirectory, LPCTSTR lpszMask);
	static CArrayOfStrings* GetFiles(LPCTSTR lpszDirectory, LPCTSTR lpszMask, bool bRecursive);
	inline static CArrayOfStrings* GetFiles(LPCTSTR lpszDirectory);
	inline static CArrayOfStrings* GetFiles(LPCTSTR lpszDirectory, bool bRecursive);

	//gets all directories in the given directory
	static CArrayOfStrings* GetDirectories(LPCTSTR lpszDirectory); 
	static CArrayOfStrings* GetDirectories(LPCTSTR lpszDirectory, bool bRecursive); 

	//Creates all the directories in a specified path, if bUnique is true
	//and the specified directory already exists, new directory is created under
	//different name (numbered) and this new name is returned
	static CString CreateDirectory(LPCTSTR lpszDirectory, bool bUnique = false);

	//Deletes a specified directory. 
	inline static void Delete(LPCTSTR lpszDirectory);

	//Deletes the specified directory and, if indicated by bRecursive, 
	//any subdirectories in the directory. 
	static void Delete(LPCTSTR lpszDirectory, bool bRecursive);

	//Determines whether the given path refers to an existing directory on disk. 
	inline static bool Exists(LPCTSTR lpszPathName);
		
	//Moves a file or a directory and its contents to a new location. 
	//In the case of error, it returns false and GetLastError() can be used	
	//to find what error has occurred.
	static bool Move(LPCTSTR lpszFromPath, LPCTSTR lpszToPath);

	//Copies a file or a directory and its contents to a new location. 
	//In the case of error, it returns false and GetLastError() can be used	
	//to find what error has occurred.
	static bool Copy(LPCTSTR lpszFromPath, LPCTSTR lpszToPath);

	//Gets the current working directory of the application. 
	inline static CString GetCurrentDirectory();

	//Sets the application's current working directory to the specified directory.
	inline static void SetCurrentDirectory(LPCTSTR lpszPath);

	//Sets the application's current working directory to the directory containing
	//the currently running application
	static void SetCurrentDirectoryToAppDir();
};

inline void Directory::Delete(LPCTSTR lpszDirectory)
{
	CString szDir = lpszDirectory;
	szDir.TrimRight(_T('\\'));
	::RemoveDirectory(szDir);
}

inline bool Directory::Exists(LPCTSTR lpszPathName) {
	DWORD dwRet = ::GetFileAttributes(lpszPathName);
	return (dwRet != (DWORD)-1 && (dwRet & FILE_ATTRIBUTE_DIRECTORY) == FILE_ATTRIBUTE_DIRECTORY);
}

inline CString Directory::GetCurrentDirectory()
{
	CString szRet;
	::GetCurrentDirectory(MAX_PATH, szRet.GetBuffer(MAX_PATH));
	szRet.ReleaseBuffer();
	return szRet;
}

inline void Directory::SetCurrentDirectory(LPCTSTR lpszPath) {
	::SetCurrentDirectory(lpszPath);
}

inline CArrayOfStrings* Directory::GetFiles(LPCTSTR lpszDirectory) {
	return GetFiles(lpszDirectory, _T("*"));
}

inline CArrayOfStrings* Directory::GetFiles(LPCTSTR lpszDirectory, bool bRecursive) {
	return GetFiles(lpszDirectory, _T("*"), bRecursive);
}