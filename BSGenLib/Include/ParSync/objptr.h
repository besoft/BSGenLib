#pragma once
#include "interlocked.h"

class CObjPtrBase
{
protected:
	typedef CHashTable<LPVOID, long> CMyMap;
	static CMyMap m_mapObjToRefCount;
};


template < class T >
class CObjPtr : public CObjPtrBase
{
protected:	
	T* m_pPointer;

public:
	CObjPtr()
	{
		m_pPointer = NULL;
	}

	CObjPtr(int Null)
	{
		BES_ASSERT(Null == NULL);
    m_pPointer = NULL;		
	}

	CObjPtr(T* pPointer, bool bAddRef = false)
	{
		m_pPointer = pPointer;
		if (!bAddRef)
			AddRef();
	}

	CObjPtr(CObjPtr< T >& ptr) 
	{
		m_pPointer = ptr.m_pPointer;
		AddRef();
	}

	~CObjPtr() {
		Release();
	}	

	//zvysi pocet referenci aktualniho pointeru
	long AddRef() {
		return _AddRef(m_pPointer);
	}
	
	//snizi pocet referenci aktualniho pointeru
	long Release() {
		return _Release(m_pPointer);		
	}

	// Saves/sets the interface without AddRef() if bAddRef is false. This call
	// will release any previously acquired interface.    
	void Attach(T* pPtr, bool fAddRef = false)
	{
		Release();
		m_pPointer = pPtr;

		if (!fAddRef)
			AddRef();
	}

	// Simply NULL the pointer so that it isn't Released()'ed.    
    T* Detach() throw()
    {
        T* pRet = m_pPointer;
        m_pPointer = NULL;
        return pRet;
    }

	//operatory
	CObjPtr< T >& operator = (T* pPtr)
    {
        if (m_pPointer != pPtr) 
		{
            Release();
			
            m_pPointer = pPtr;
            
			AddRef();
        }

        return *this;
    }

	CObjPtr< T >& operator = (CObjPtr< T >& ptr) {
		return operator = (ptr.m_pPointer);		
    }

	CObjPtr< T >& operator = (int Null)
    {
        BES_ASSERT(Null == NULL);

        return operator = (reinterpret_cast<T*>(NULL));
    }

	// Return the Pointer. This value may be NULL.    
    operator T*() {
        return m_pPointer; 
    }

    // Queries for the unknown and return it
    operator T&() {
        return *m_pPointer; 
    }

    // Allows an instance of this class to act as though it were the reference pointer    
    T& operator*() {
        return *m_pPointer; 
    }

    // Returns the address of the Pointer pointer contained in this
    // class. 
    T** operator&()
    {
        Release();
        m_pPointer = NULL;
        return &m_pPointer;
    }

    // Allows this class to be used as the Pointer itself.
    // Also provides simple error checking.
    //
    T* operator->() {
        return m_pPointer; 
    }

    // This operator is provided so that simple boolean expressions will
    // work.  For example: "if (p) ...".
    // Returns TRUE if the pointer is not NULL.
    //
    operator bool() {
        return m_pPointer != NULL; 
    }

	// For comparison to NULL    
    bool operator == (int Null)
    {
		BES_ASSERT(Null == NULL);

        return m_pPointer == NULL;
    }

	// For comparison to NULL    
    bool operator != (int Null)
    {
		BES_ASSERT(Null == NULL);

        return m_pPointer != NULL;
    }

		
protected:
	//zvysi pocet referenci
	long _AddRef(T*& pPtr)
	{
		if (pPtr == NULL)
			return 0;

#if defined(__BSGEN_PUREWIN)
		long* pCurVal = m_mapObjToRefCount.Lookup((LPVOID&)pPtr);
		if (pCurVal != NULL)
			return CInterlocked::Increment(pCurVal);
		else
		{
			long val = 1;
			m_mapObjToRefCount.Add((LPVOID&)pPtr, val);
			return val;
		}
#else
		CMyMap::CPair* pCurVal = m_mapObjToRefCount.Lookup(pPtr);
		if (pCurVal != NULL)
			return CInterlocked::Increment(&pCurVal->m_value);
		else
		{
			m_mapObjToRefCount[pPtr] = 1;
			return 1;
		}
#endif
	}

	//snizi pocet refenci
	long _Release(T*& pPtr)
	{
		if (pPtr == NULL)
			return 0;

#if defined(__BSGEN_PUREWIN)
		long* pCurVal = m_mapObjToRefCount.Lookup((LPVOID&)pPtr);
		BES_ASSERT(pCurVal != NULL);
		
		long nRet = CInterlocked::Decrement(pCurVal);
		if (0 == nRet)
		{
			m_mapObjToRefCount.RemoveKey((LPVOID&)pPtr);
#else
		CMyMap::CPair* pCurVal = m_mapObjToRefCount.Lookup(pPtr);
		BES_ASSERT(pCurVal != NULL);
		
		long nRet = CInterlocked::Decrement(&pCurVal->m_value);
		if (0 == nRet)
		{
			m_mapObjToRefCount.RemoveKey(pPtr);
#endif
			delete pPtr;
			pPtr = NULL;
		}

		return nRet;
	}
};