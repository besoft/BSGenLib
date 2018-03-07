/* ${FILENAME}
** ${LASTMODIFIED}
**
** ${COPYRIGHT}
** ${LICENCE}
**
** DESCRIPTION: This class is predetermined to be used when some data is to be protected
*/

#ifndef BES_SHARED_OBJ
#define BES_SHARED_OBJ

#include "Interlocked.h"
#include "../Common/RefObj.h"

//synchronized implementation of IRefObj
//See CRefObj "../Common/RefObj.h"
//NOTE: if needed generally, can be used CSynchronizedRefObj<IRefObj>
template < class T_IFACE >
class CSyncRefObj : public T_IFACE
{
protected:
	ULONG m_nRefs;

public:
	CSyncRefObj() : m_nRefs(1) {}
	virtual ~CSyncRefObj() {}

public:
	virtual ULONG AddRef() {
		return CInterlocked::Increment(&m_nRefs);
	}

	virtual ULONG Release() {
		ULONG nRet = CInterlocked::Decrement(&m_nRefs);
		if (nRet == 0)
			delete this;

		return nRet;
	}	
};


//This class is used as a synchronized life counter for external objects
//NOTE: always consider the use of CSynchronizedRefObj if possible
//#pragma deprecated(CSharedObj)
class CSharedObj
{
public:
	CSharedObj(void)
	{
		m_nRefs = 0;
	}

	~CSharedObj(void)
	{
	}

	inline ULONG AddRef()	//increase the reference counter + returns the current count
	{
		return CInterlocked::Increment(&m_nRefs);
	}

	inline long Release() //decrease the reference counter + returns the current count
	{
		return CInterlocked::Decrement(&m_nRefs);
	}

	inline long GetRefs()
	{
		return m_nRefs;
	}
protected:
	volatile long m_nRefs;
};

#endif //BES_SHARED_OBJ