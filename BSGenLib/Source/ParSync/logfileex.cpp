#include "StdAfx.h"
#include <share.h>
#include <time.h>
#include <sys/timeb.h>


/*static*/ _TCHAR* CLogFileMessageType::m_StringValues[] = {
	_T("Debug"), _T("Info"), _T("Warning"), _T("MinorError"),
	_T("Error"), _T("FatalError"),
};

//starts the logging
//lpszLogPathName = the full path name of log file (NULL for default = "error.log")
//lpszHead = string to be written into the log, lpszSeparator = string to be written
//into the log if the file is appended and already exists
//messages are buffered and written into the file when either dwFlushTimeOut ms elapses,
//or dwFlushMaxItems messages are in the buffer		
void CLogFileEx::Open(LPCTSTR lpszLogPathName, CLogFileOpenOptions OpenOptions,
		LPCTSTR lpszHead, LPCTSTR lpszSeparator, DWORD dwFlushTimeOut, DWORD dwFlushMaxItems)
{
	if (m_fLog != NULL)
		Close();

	m_dwFlushMaxItems = dwFlushMaxItems;
	m_dwFlushTimeOut = dwFlushTimeOut;

	if (lpszLogPathName == NULL || *lpszLogPathName == _T('\0'))
		lpszLogPathName = _T("error.log");
	
	if (0 == (OpenOptions & CLogFileOpenOptions::Append))
		::DeleteFile(lpszLogPathName);		//overwrite it

	//if file still exists, write separator
	bool bWriteSeparator = (::GetFileAttributes(lpszLogPathName) != (DWORD)-1);
	if (NULL == (m_fLog = _tfsopen(lpszLogPathName, _T("at"), _SH_DENYNO)))
	{
		TRACE(_T("Unable to open log '%s'"), lpszLogPathName);		
	}
	else
	{
		if (bWriteSeparator)
		{
			if (lpszSeparator != NULL)
				_ftprintf_s(m_fLog, lpszSeparator);
			else
				_ftprintf_s(m_fLog, _T("\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n*********************\n"));
		}
		
		if (lpszHead != NULL)
			_ftprintf_s(m_fLog, lpszHead);
	
		m_nMsgID = 0;
		m_bOnClose = false;	

		//create an infinite queue (initial capacity is dwFlushMaxItems)
		m_pList = new CSyncQueue<CString>(false, false, dwFlushMaxItems);
		m_hFlushThread = STARTTHREAD2(CLogFileEx, 2);
	}
}

//flushes data and closes the log file
void CLogFileEx::Close()
{
	if (m_fLog != NULL)
	{
		m_bOnClose = true;		//terminate flush
		m_hFlushEvnt.Set();		//flush data
		m_hFlushThread.Join();		
	
		fclose(m_fLog);
		m_fLog = NULL;

		delete m_pList;
	}	
}

#define LOGFILE_TAG_PID		0x1		// <PID>		ID of the calling process
#define LOGFILE_TAG_TID		0x2		// <TID>		ID of the calling thread
#define LOGFILE_TAG_DT		0x4		// <DT>			current date and time in specified format (see strftime)
#define LOGFILE_TAG_MS		0x8		// <MS>			ms in the current time (strftime does not operate with ms)
#define LOGFILE_TAG_TS		0x10	// <TS>			time stamp (integer value denoting the current time)
#define LOGFILE_TAG_MID		0x20	// <MID>		order number of the message
#define LOGFILE_TAG_MT		0x40	// <MT>			message type (number)
#define LOGFILE_TAG_MTS		0x80	// <MTS>		message type (string)

//returns a value that denotes which tags are used in lpszFormat
DWORD CLogFileEx::GetTagMask(LPCTSTR lpszFormat)
{
	DWORD dwRet = 0;

	CString szRet = lpszFormat;
	if (szRet.Find(_T("<PID>")) >= 0)
		dwRet |= LOGFILE_TAG_PID;
	if (szRet.Find(_T("<TID>")) >= 0)
		dwRet |= LOGFILE_TAG_TID;
	if (szRet.Find(_T("<DT>")) >= 0)
		dwRet |= LOGFILE_TAG_DT;
	if (szRet.Find(_T("<MS>")) >= 0)
		dwRet |= LOGFILE_TAG_MS;
	if (szRet.Find(_T("<TS>")) >= 0)
		dwRet |= LOGFILE_TAG_TS;
	if (szRet.Find(_T("<MID>")) >= 0)
		dwRet |= LOGFILE_TAG_MID;
	if (szRet.Find(_T("<MT>")) >= 0)
		dwRet |= LOGFILE_TAG_MT;
	if (szRet.Find(_T("<MTS>")) >= 0)
		dwRet |= LOGFILE_TAG_MTS;

	return dwRet;
}


//logs the given message
void CLogFileEx::LogEventV(CLogFileMessageType nMsgType, LPCTSTR pFormat, va_list& pArg)
{
#ifndef __BSGEN_PUREWIN
	if (m_IgnoredMsgs.Lookup((int)nMsgType) != NULL)
#else
	int nMsgType_i = (int)nMsgType;
	if (m_IgnoredMsgs.Lookup(nMsgType_i) != NULL)
#endif
		return;	//this type of message is ignored	

	CString szStr;
	CString szPrefix = m_szMsgPrefix;
	CString szSuffix = m_szMsgSuffix;

	if (m_dwTagMask & LOGFILE_TAG_PID)
	{
		// <PID>		ID of the calling process	
		szStr.Format(_T("%d"), GetCurrentProcessId());
		szPrefix.Replace(_T("<PID>"), szStr);
		szSuffix.Replace(_T("<PID>"), szStr);
	}

	if (m_dwTagMask & LOGFILE_TAG_TID)
	{
		// <TID>		ID of the calling thread
		szStr.Format(_T("%d"), GetCurrentThreadId());
		szPrefix.Replace(_T("<TID>"), szStr);
		szSuffix.Replace(_T("<TID>"), szStr);
	}

	if (m_dwTagMask & (LOGFILE_TAG_DT | LOGFILE_TAG_MS | LOGFILE_TAG_TS))
	{	
		struct _timeb curtm;
		_ftime64_s(&curtm);		
		
		if (m_dwTagMask & LOGFILE_TAG_DT)
		{
			// <DT>			current date and time in specified format (see strftime)
			struct tm loctime;
#if _MSC_VER < 1400
			loctime = *localtime(&curtm.time);
#else
			localtime_s(&loctime, &curtm.time);	
#endif
			_tcsftime(szStr.GetBuffer(MAX_PATH), MAX_PATH, m_szDateTimeFormat, &loctime);
			szStr.ReleaseBuffer();
			szPrefix.Replace(_T("<DT>"), szStr);
			szSuffix.Replace(_T("<DT>"), szStr);
		}


		if (m_dwTagMask & LOGFILE_TAG_MS)
		{
			// <MS>			ms in the current time (strftime does not operate with ms)
			szStr.Format(_T(".%02d"), curtm.millitm / 10);
			szPrefix.Replace(_T("<MS>"), szStr);
			szSuffix.Replace(_T("<MS>"), szStr);
		}

		if (m_dwTagMask & LOGFILE_TAG_TS)
		{
			// <TS>			time stamp (integer value denoting the current time)
			szStr.Format(_T("%d"), curtm.time);
			szPrefix.Replace(_T("<TS>"), szStr);
			szSuffix.Replace(_T("<TS>"), szStr);
		}
	}

	if (m_dwTagMask & LOGFILE_TAG_MID)
	{
		// <MID>		order number of the message		
		szStr.Format(_T("%d"), CInterlocked::Increment(&m_nMsgID));
		szPrefix.Replace(_T("<MID>"), szStr);
		szSuffix.Replace(_T("<MID>"), szStr);
	}

	if (m_dwTagMask & LOGFILE_TAG_MT)
	{
		// <MT>			message type (number)
		szStr.Format(_T("%d"), nMsgType);
		szPrefix.Replace(_T("<MT>"), szStr);
		szSuffix.Replace(_T("<MT>"), szStr);
	}

	if (m_dwTagMask & LOGFILE_TAG_MTS)
	{
		// <MTS>		message type (string)
		szStr = nMsgType.ToString();
		szPrefix.Replace(_T("<MTS>"), szStr);
		szSuffix.Replace(_T("<MTS>"), szStr);
	}	

	CString szMsg;
	szMsg.FormatV(pFormat, pArg);
	szMsg.Insert(0, szPrefix);
	szMsg += szSuffix;

	//add message to the list of messages - allows enlargement
	m_pList->Put(szMsg, true);
	if (m_pList->GetCount() >= m_dwFlushMaxItems)
		m_hFlushEvnt.Set();	//Flush items	
}

//forces the flush
void CLogFileEx::Flush()
{
	if (m_fLog != NULL)
	{
		m_cs.Lock();
		UINT nCount = m_pList->GetCount();
		while (nCount > 0)
		{
			_ftprintf_s(m_fLog, m_pList->Dequeue());
			nCount--;
		}

		fflush(m_fLog);
		m_cs.Unlock();
	}
}

//Thread loop
int CLogFileEx::DeferredFlush()
{
	while (true)
	{
		if (m_bOnClose)
			break;

		m_hFlushEvnt.WaitOne(m_dwFlushTimeOut);
		Flush();
	}

	return 0;
}	

