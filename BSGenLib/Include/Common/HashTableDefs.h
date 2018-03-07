#pragma once
/*
template <typename A, typename E>
class CArrayHashTableEx : public CHashTableEx<CArray<A>, E>
{
public:
	CArrayHashTable()
	{
		m_bFastCtorDtor = false;
	}
protected:	
	virtual DWORD GetHash(CArray<A>& key)
	{
		return GetJenkinsHash(key.GetData(), sizeof(A)*key.GetSize());
	}	
};

template <typename E>
class CStringHashTableEx : public CHashTableEx<CString, E>
{
public:
	CCharStringHashTable()
	{
		m_bFastCtorDtor = false;
	}
protected:	
	virtual DWORD GetHash(CCharString& key)
	{
		return GetJenkinsHash(key.GetData(), key.GetSize());
	}

	virtual int CompareKeys(CCharString& key1, CCharString& key2)
	{	
		return key1.Compare((char*)key2);
	}
};

*/
/*

*/

#if !defined(__BSGEN_PUREWIN)
//This ElementTrait should be used for structures
template < typename T >
class CStructElementTraits : public CElementTraitsBase<T>
{
public:
	static ULONG Hash( const T& element ) throw()
	{
		return (ULONG)GetJenkinsHash(&element, sizeof(T));
	}

	//Call this function to compare two elements for equality
	static bool CompareElements( const T& element1, const T& element2 )
	{
		return CompareElementsOrdered(element1, element2) == 0;
	}

	//Call this function to determine the greater and lesser element.
	static int CompareElementsOrdered( const T& element1, const T& element2 )
	{
		return memcmp(&element1, &element2, sizeof(T));
	}
};

template< typename K, typename V >
class CSingleStructHashTable : public CAtlMap<K, V, CStructElementTraits<K> > {

};

template< typename V>
class CStringHashTable : public CAtlMap<CString, V, CStringElementTraits<CString> > {

};

#else
template< typename K, typename V, class NODE = CHashNode< K, V > >
class CSingleStructHashTable : public CHashTable<K, V, NODE>
{
protected:
	virtual DWORD GetHash(K& key)
	{
		return GetJenkinsHash(key, sizeof(key));
	}

	virtual int CompareKeys(K& key1, K& key2)
	{
		return memcmp(key1, key2, sizeof(key1));
	}
};

template <typename E>
class CStringHashTable : public CHashTable<CString, E>
{
protected:	
	virtual DWORD GetHash(CString& key)
	{
		return GetJenkinsHash((void*)((LPCTSTR)key), key.GetLength());
	}

	virtual int CompareKeys(CString& key1, CString& key2)
	{	
		return key1.CompareTo(key2);
	}
};
#endif