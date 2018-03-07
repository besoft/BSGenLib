/* ${FILENAME}
** ${LASTMODIFIED}
**
** ${COPYRIGHT}
** ${LICENCE}
**
** DESCRIPTION: Logging routines
*/

#ifndef BES_LOGFILEEX_H
#define BES_LOGFILEEX_H

//////////////////////////////////////
// CLogFileEx allows the logging of messages into the specified file
// Messages are buffered to speed-up the processing
// Prefix and suffix of every message to be logged can be set
// The class can be configured to log only some kind of messages

//Type of logged message
//note: messages are always logged by default unless their types are enlisted in IgnoredMsgs
typedef struct CLogFileMessageType {	
	//RELEASE NOTE: check m_StringValues if codes are changed
	DECLARE_ENUM_STRUCT(CLogFileMessageType)
	enum {		
		DebugMsg = 0,			//debug message
		Information = 1,		//some information, e.g., "Version: 1.2.13.2" or "Source file: moje.txt"
		Warning = 2,			//warning, e.g., "Max value > 1000" or "Not enough physical memory" 
		MinorError  = 3,		//error that "does not influence" the program
		Error = 4,				//error that strongly influences the program
		FatalError = 5,			//error that causes the program to be terminated
		
		//User messages have codes >= UserMsg
		UserMsg = 6
	};

	inline CString ToString() {		
		if (m_nValue < UserMsg)
			return m_StringValues[m_nValue];
		else
		{
			CString szStr;
			szStr.Format(_T("UserMsg(%d)"), m_nValue);
			return szStr;
		}
	}
private:
	static _TCHAR* m_StringValues[];
} CLogFileMessageType;

//Flags used in CLogFileEx::Open 
typedef struct CLogFileOpenOptions {
	DECLARE_ENUM_STRUCT(CLogFileOpenOptions)
	enum {
		OverWrite = 1,			//file is overwritten if exists
		Append = 2,				//file is open for append if exists		
	};
} CLogFileOpenOptions;


#define LOGFILEEX_DEFERRED_FLUSH_TO			  10000		//flush each 10 seconds
#define LOGFILEEX_DEFERRED_FLUSH_MAXITEMS		500		//or when 500 events should be written

class CLogFileEx
{
//attributes
protected:	
	//these two variables defines the limits for the buffering
	DWORD m_dwFlushMaxItems;
	DWORD m_dwFlushTimeOut;

	CHashTable<int, bool> m_IgnoredMsgs;		//list of messages to be ignored

	FILE* m_fLog;								//log file
	CSyncQueue<CString>* m_pList;				//buffer with messages

	//formatted prefixes and sufixes of messages - see below
	CString m_szMsgPrefix;
	CString m_szMsgSuffix;

	// Each message has prefix and suffix, strings that may contain these tags:
	// <PID>		ID of the calling process
	// <TID>		ID of the calling thread
	// <DT>			current date and time in specified format (see strftime)
	// <MS>			ms in the current time (strftime does not operate with ms)
	// <TS>			time stamp (integer value denoting the current time)
	// <MID>		order number of the message
	// <MT>			message type (number)
	// <MTS>		message type (string)
	DWORD m_dwTagMask;				//to speed-up the processing (see LogEventV)

	CString m_szDateTimeFormat;		//format string for strftime

	CThread m_hFlushThread;			//handle of thread performing the physical write
	CSyncEvent m_hFlushEvnt;		//event that starts the flushing (i.e., the physical write)
	bool m_bOnClose;				//used to terminate FlushThread
	CShortCS m_cs;					//CS used for flushing

	volatile long m_nMsgID;			//ID of written message
public:
	//ctor
	CLogFileEx()
	{
		m_fLog = NULL;
		m_szMsgPrefix = _T("<DT><MS>\t");
		m_szMsgSuffix = _T("\n");
		m_dwTagMask = GetTagMask(m_szMsgPrefix) | GetTagMask(m_szMsgSuffix);
		m_szDateTimeFormat = _T("%Y-%m-%d %H:%M:%S");
	}

	//dtor
	~CLogFileEx(void)
	{ 
		Close(); 
	}

public:
	//starts the logging
	//lpszLogPathName = the full path name of log file (NULL for default = "error.log")
	//lpszHead = string to be written into the log, lpszSeparator = string to be written
	//into the log if the file is appended and already exists
	//messages are buffered and written into the file when either dwFlushTimeOut ms elapses,
	//or dwFlushMaxItems messages are in the buffer		
	void Open(LPCTSTR lpszLogPathName = NULL, 
		CLogFileOpenOptions OpenOptions = CLogFileOpenOptions::Append, LPCTSTR lpszHead = NULL, 
		LPCTSTR lpszSeparator = NULL, DWORD dwFlushTimeOut = LOGFILEEX_DEFERRED_FLUSH_TO,
		DWORD dwFlushMaxItems = LOGFILEEX_DEFERRED_FLUSH_MAXITEMS);

	//flushes data and closes the log file
	void Close();

	//forces the flush
	void Flush();
		
	//gets the current flush time out
	inline DWORD GetFlushTimeOut() {
		return m_dwFlushTimeOut;
	}

	//modifies flush time out, may not have the immediate effect
	inline void SetFlushTimeOut(DWORD dwFlushTimeOut) {
		m_dwFlushTimeOut = dwFlushTimeOut;
	}

	//gets the current flush time max items
	inline DWORD GetFlushTimeMaxItems() {
		return m_dwFlushMaxItems;
	}

	//modifies  flush time max items, may not have the immediate effect
	inline void SetFlushTimeMaxItems(DWORD dwFlushMaxItems) {
		m_dwFlushMaxItems = dwFlushMaxItems;
	}

	//returns prefix for messages
	inline CString GetMsgPrefix() {
		return m_szMsgPrefix;
	}

	//sets a new prefix for messages
	inline void SetMsgPrefix(LPCTSTR lpszPrefix) {
		m_szMsgPrefix = lpszPrefix;
		m_dwTagMask = GetTagMask(m_szMsgPrefix) | GetTagMask(m_szMsgSuffix);
	}

	//returns suffix for messages
	inline CString GetMsgSuffix() {
		return m_szMsgSuffix;
	}

	//returns suffix for messages
	inline void SetMsgSuffix(LPCTSTR lpszSuffix) {
		m_szMsgSuffix = lpszSuffix;
		m_dwTagMask = GetTagMask(m_szMsgPrefix) | GetTagMask(m_szMsgSuffix);
	}

	//returns date time format string
	inline CString GetDateTimeFormat() {
		return m_szDateTimeFormat;
	}

	//sets date time format string
	inline void SetDateTimeFormat(LPCTSTR lpszDateTimeFormat) {
		m_szDateTimeFormat = lpszDateTimeFormat;
	}

	//updates the list of ignored message types; every message whose type is enlisted
	//will be ignoret, i.e., it won't be logged into the file
	inline void UpdateMsgTypesList(CLogFileMessageType msg_type, bool bIgnore = true) {
#ifndef __BSGEN_PUREWIN
		if (bIgnore)
			m_IgnoredMsgs[(int)msg_type] = true;
		else
			m_IgnoredMsgs.RemoveKey((int)msg_type);	

#else
		int nMsgType = (int)msg_type;
		if (bIgnore)			
			m_IgnoredMsgs[nMsgType] = true;		
		else
			m_IgnoredMsgs.RemoveKey(nMsgType);
#endif		
	}


	//simple LogEvent without specified message type
	inline void LogEvent(LPCTSTR pFormat, ...)
	{
		va_list pArg;
		va_start(pArg, pFormat);
		LogEventV(-1, pFormat, pArg);
		va_end(pArg);
	}

	//simple LogEvent without specified message type
	inline void LogEventV(LPCTSTR pFormat, va_list& pArg)  {
		LogEventV(-1, pFormat, pArg);
	}

	//logs the given message
	inline void LogEvent(CLogFileMessageType nMsgType, LPCTSTR pFormat, ...) 
	{
		va_list pArg;
		va_start(pArg, pFormat);
		LogEventV(nMsgType, pFormat, pArg);
		va_end(pArg);
	}

	//logs the given message
	void LogEventV(CLogFileMessageType nMsgType, LPCTSTR pFormat, va_list& pArg);
protected:
	//returns a value that denotes which tags are used in lpszFormat
	DWORD GetTagMask(LPCTSTR lpszFormat);

	////////////////////
	// Thread
	THREADSTARTPROC2(CLogFileEx, DeferredFlush, 2);	//thread
	int DeferredFlush();							//Loop
};

#endif //BES_LOGFILEEX_H