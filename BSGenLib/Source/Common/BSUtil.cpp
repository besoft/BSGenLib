#include "stdafx.h"

//formats time given in milliseconds
CString CBSUtil::FormatTime(double time)
{	
	CString ret;
	if (time < 1000)						//neni to ani 1 sekunda
		ret.Format(_T("%.02f ms"), time);	// postaci 2 desetina mista -> mereni takovych kratkych useku 
	else									// neni beztak dostatecne presne
	{
		if (time < 60000)
			ret.Format(_T("%.02f s"), time / 1000);	// sekundy (posledni minuta)
		else 
		{
			DWORD dwTime = (DWORD)(time / 1000); // setiny sekundy jsou jiz k nicemu
			DWORD dwMins = dwTime / 60;
			if (dwMins < 60)
				ret.Format(_T("%02d:%02d min:sec"), dwMins, dwTime % 60);
			else
				ret.Format(_T("%02d:%02d:%02d hrs:min:sec"), dwMins / 60, dwMins % 60, dwTime % 60);
		}
	}
	
	return ret;
}

//naformatuje velikost udanou v Bytech
CString CBSUtil::FormatSize(DWORD dwBytes)
{
	CString szRet;
	if (dwBytes >= 1024*1024)
		szRet.Format(_T("%d MB"), dwBytes / (1024*1024));
	else if (dwBytes >= 1024)
		szRet.Format(_T("%d KB"), dwBytes / 1024);
	else
		szRet.Format(_T("%d Bytes"), dwBytes);

	return szRet;
}

