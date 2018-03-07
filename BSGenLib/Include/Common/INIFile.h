#pragma once

class CIniFile
{
protected:
	CString m_szFileName;
public:
	CIniFile(LPCTSTR lpszFileName) : m_szFileName(Path::GetFullPath(lpszFileName))
	{
	}

	CString GetString(LPCTSTR lpszSection, LPCTSTR lpszValue, LPCTSTR lpszDefault = _T(""))
	{
		CString szRet;
		GetPrivateProfileString(lpszSection, lpszValue, lpszDefault, szRet.GetBuffer(1024), 1024, m_szFileName);
		szRet.ReleaseBuffer();
		return szRet;
	}
#ifndef __BSGEN_PUREWIN
	COleDateTime GetDate(LPCTSTR lpszSection, LPCTSTR lpszValue)
	{
		COleDateTime dtError;
		dtError.m_status = COleDateTime::invalid;
		return GetDate(lpszSection, lpszValue, dtError);
	}

	COleDateTime GetDate(LPCTSTR lpszSection, LPCTSTR lpszValue, COleDateTime dtDefault)
	{
		CString szRet = GetString(lpszSection, lpszValue);
		if (szRet.IsEmpty())
			return dtDefault;

		COleDateTime dtRet;
		dtRet.ParseDateTime(szRet, 0, 1033);	//English - United States
		return dtRet;
	}
#endif

	bool GetBool(LPCTSTR lpszSection, LPCTSTR lpszValue, bool bDefault = false)
	{
#pragma warning(disable: 4800)
		return (bool)GetInt(lpszSection, lpszValue, (int)bDefault);
#pragma warning(default: 4800)
	}

	int GetInt(LPCTSTR lpszSection, LPCTSTR lpszValue, int nDefault = 0)
	{
		return GetPrivateProfileInt(lpszSection, lpszValue, nDefault, m_szFileName);
	}

	double GetDouble(LPCTSTR lpszSection, LPCTSTR lpszValue, double dblDefault = 0.0f)
	{
		CString szRet = GetString(lpszSection, lpszValue);
		if (szRet.IsEmpty())
			return dblDefault;
		
		return _tstof(szRet);
	}
	
	void Write(LPCTSTR lpszSection, LPCTSTR lpszValue, LPCTSTR lpszVal)
	{	
		WritePrivateProfileString(lpszSection, lpszValue, lpszVal, m_szFileName);		
	}

	void Write(LPCTSTR lpszSection, LPCTSTR lpszValue, bool bValue)
	{
#pragma warning(disable: 4800)
		Write(lpszSection, lpszValue, (int)bValue);
#pragma warning(default: 4800)
	}

	void Write(LPCTSTR lpszSection, LPCTSTR lpszValue, int nValue)
	{
		CString szStr;
		szStr.Format(_T("%d"), nValue);
		WritePrivateProfileString(lpszSection, lpszValue, szStr, m_szFileName);
	}

#ifndef __BSGEN_PUREWIN
	void Write(LPCTSTR lpszSection, LPCTSTR lpszValue, COleDateTime& dtValue)
	{		
		CString szStr;
		if (dtValue.m_status != COleDateTime::invalid)
			szStr = dtValue.Format(_T("%m/%d/%Y %H:%M:%s"));
		WritePrivateProfileString(lpszSection, lpszValue, szStr , m_szFileName);
	}
#endif

	void Write(LPCTSTR lpszSection, LPCTSTR lpszValue, double dblValue)
	{	
		CString szStr;
		szStr.Format(_T("%f"), dblValue);
		WritePrivateProfileString(lpszSection, lpszValue, szStr, m_szFileName);
	}
};
