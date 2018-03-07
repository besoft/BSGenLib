/* ${FILENAME}
** ${LASTMODIFIED}
**
** ${COPYRIGHT}
** ${LICENCE}
**
** DESCRIPTION: Wrapper for standard Critical Section
*/

#ifndef BES_SYNC_CS_H
#define BES_SYNC_CS_H

#ifdef _WIN32

class CCS
{
public:

	CCS(void)
	{
		InitializeCriticalSection(&m_cs);
	}

	~CCS(void)
	{
		DeleteCriticalSection(&m_cs);
	}

	inline void Lock() { EnterCriticalSection(&m_cs);}
	inline void Unlock() { LeaveCriticalSection(&m_cs);}

protected:
	CRITICAL_SECTION m_cs;
};

#else
#error 'NOT SUPPORTED'
#endif

#endif //BES_SYNC_CS_H