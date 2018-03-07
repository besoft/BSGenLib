/* ${FILENAME}
** ${LASTMODIFIED}
**
** ${COPYRIGHT}
** ${LICENCE}
**
** DESCRIPTION: Template for a round robin with possible growing factor, pseudo active waiting
*/

#ifndef BES_HASHTABLE_H
#define BES_HASHTABLE_H

#include "dynpool.h"


#pragma region Jenkins Hash Function
/* Default hash function. This is Bob Jenkins' well-known hash
   function.  See comments below for details. */
/*
--------------------------------------------------------------------
mix -- mix 3 32-bit values reversibly.
For every delta with one or two bits set, and the deltas of all three
  high bits or all three low bits, whether the original value of a,b,c
  is almost all zero or is uniformly distributed,
* If mix() is run forward or backward, at least 32 bits in a,b,c
  have at least 1/4 probability of changing.
* If mix() is run forward, every bit of c will change between 1/3 and
  2/3 of the time.  (Well, 22/100 and 78/100 for some 2-bit deltas.)
mix() was built out of 36 single-cycle latency instructions in a 
  structure that could supported 2x parallelism, like so:
      a -= b; 
      a -= c; x = (c>>13);
      b -= c; a ^= x;
      b -= a; x = (a<<8);
      c -= a; b ^= x;
      c -= b; x = (b>>13);
      ...
  Unfortunately, superscalar Pentiums and Sparcs can't take advantage 
  of that parallelism.  They've also turned some of those single-cycle
  latency instructions into multi-cycle latency instructions.  Still,
  this is the fastest good hash I could find.  There were about 2^^68
  to choose from.  I only looked at a billion or so.
--------------------------------------------------------------------
*/

#define mix(a,b,c)				\
{						\
  a -= b; a -= c; a ^= (c>>13);			\
  b -= c; b -= a; b ^= (a<<8);			\
  c -= a; c -= b; c ^= (b>>13);			\
  a -= b; a -= c; a ^= (c>>12);			\
  b -= c; b -= a; b ^= (a<<16);			\
  c -= a; c -= b; c ^= (b>>5);			\
  a -= b; a -= c; a ^= (c>>3);			\
  b -= c; b -= a; b ^= (a<<10);			\
  c -= a; c -= b; c ^= (b>>15);			\
}

/*
--------------------------------------------------------------------
hash() -- hash a variable-length key into a 32-bit value
  k       : the key (the unaligned variable-length array of bytes)
  len     : the length of the key, counting by bytes
  initval : can be any 4-byte value
Returns a 32-bit value.  Every bit of the key affects every bit of
the return value.  Every 1-bit and 2-bit delta achieves avalanche.
About 6*len+35 instructions.

The best hash table sizes are powers of 2.  There is no need to do
mod a prime (mod is sooo slow!).  If you need less than 32 bits,
use a bitmask.  For example, if you need only 10 bits, do
  h = (h & hashmask(10));
In which case, the hash table should have hashsize(10) elements.

If you are hashing n strings (ub1 **)k, do it like this:
  for (i=0, h=0; i<n; ++i) h = hash( k[i], len[i], h);

By Bob Jenkins, 1996.  bob_jenkins@burtleburtle.net.  You may use this
code any way you wish, private, educational, or commercial.  It's free.

See http://burtleburtle.net/bob/hash/evahash.html
Use for hash table lookup, or anything where one collision in 2^^32 is
acceptable.  Do NOT use for cryptographic purposes.
--------------------------------------------------------------------
*/
inline DWORD GetJenkinsHash(void* pKey, int nLen)
{
    DWORD a,b,c,len;
    char *k = (char*)pKey;

    /* Set up the internal state */
    len = nLen;
    a = b = 0x9e3779b9;  /* the golden ratio; an arbitrary value */
    c = 0;               /* the previous hash value */

    /*---------------------------------------- handle most of the key */
    while(len >= 12){
		a += (k[0] +((DWORD)k[1]<<8) +((DWORD)k[2]<<16) +((DWORD)k[3]<<24));
		b += (k[4] +((DWORD)k[5]<<8) +((DWORD)k[6]<<16) +((DWORD)k[7]<<24));
		c += (k[8] +((DWORD)k[9]<<8) +((DWORD)k[10]<<16)+((DWORD)k[11]<<24));
		mix(a,b,c);
		k += 12; len -= 12;
    }

    /*------------------------------------- handle the last 11 bytes */
    c += nLen;
    switch(len){              /* all the case statements fall through */
		case 11: c+=((DWORD)k[10]<<24);
		case 10: c+=((DWORD)k[9]<<16);
		case 9 : c+=((DWORD)k[8]<<8);
		/* the first byte of c is reserved for the length */
		case 8 : b+=((DWORD)k[7]<<24);
		case 7 : b+=((DWORD)k[6]<<16);
		case 6 : b+=((DWORD)k[5]<<8);
		case 5 : b+=k[4];
		case 4 : a+=((DWORD)k[3]<<24);
		case 3 : a+=((DWORD)k[2]<<16);
		case 2 : a+=((DWORD)k[1]<<8);
		case 1 : a+=k[0];
	/* case 0: nothing left to add */
    }
    mix(a,b,c);
    /*-------------------------------------------- report the result */
    return c;
}
/* End of code from Jenkins */
#pragma endregion

#ifndef __BSGEN_PUREWIN
#define CHashTable CAtlMap
#else
#pragma deprecated(CHashNode, CHashTable)

#pragma region HashTable Node
template< typename K, typename V>
class CHashNodeE
{
public:
	CHashNodeE() {}

	void Construct(K& key, DWORD dwHash)
	{
		m_Key = key;
		m_Hash = dwHash;
		m_pNextNode = NULL;
	}

	inline K& GetKey() { return m_Key; }
	inline V& GetValue() { return m_Value; }
	inline DWORD GetHash() { return m_Hash; }
	inline CHashNodeE* GetNextNode() { return m_pNextNode; }
//RELEASE NOTE: friend class not supported in this template
public:
	K m_Key;
	V m_Value;
	DWORD m_Hash;

	CHashNodeE* m_pNextNode;	
};
#pragma endregion

#pragma region HashTable
#define DEFAULT_HASH_TBLSIZE		1031
#define DEFAULT_HASH_TBLLOADKOEF	0.7
#define HASH_TBLMAXSIZE				2147483647

//This is a general hashtable template, it uses Jenkins hash function
//See: http://burtleburtle.net/bob/hash/evahash.html
template<typename K, typename V, class NODE = CHashNodeE< K, V > >
class CHashTable
{
public:
	typedef typename NODE THISHASHNODE;

	CHashTable(int nTblSize = DEFAULT_HASH_TBLSIZE, float fLoad = DEFAULT_HASH_TBLLOADKOEF, 
		bool bFastCtorDtor = true)
	{
		m_bFastCtorDtor = bFastCtorDtor;
		m_bAutoRehash = true;
		m_pHashTbl = NULL;
		m_nElements = 0;

		InitHashTable(nTblSize, fLoad);	
	}

	~CHashTable(void)
	{
		RemoveAll();
	}

	inline bool IsEmpty() { return m_nElements == 0;}
	inline int GetCount() { return m_nElements; }
	inline int GetHashTableSize() { return m_nHashTblSize; }
	inline void EnableAutoRehash() { m_bAutoRehash = true; }
	inline void DisableAutoRehash() { m_bAutoRehash = false; }
	
	inline void InitHashTable(int nTblSize)
	{
		BES_ASSERT(m_pHashTbl == NULL);
		BES_ASSERT(nTblSize > 0);

		m_nHashTblSize = nTblSize;		
		m_nMaxLoadElems = (int)(m_nHashTblSize*m_fLoadKoef);
	}

	inline void InitHashTable(int nTblSize, float fLoad)
	{
		BES_ASSERT(m_pHashTbl == NULL);
		BES_ASSERT(nTblSize > 0 && fLoad > 0.0 && fLoad <= 1.0);

		m_nHashTblSize = nTblSize;
		m_fLoadKoef = fLoad;
		m_nMaxLoadElems = (int)(m_nHashTblSize*m_fLoadKoef);
	}

	inline void Add(K& key, V& value)
	{
		(*this)[key] = value;
	}
	void Remove(K& key);
	inline void RemoveKey(K& key) {
		Remove(key);
	}

	inline void RemoveAll()
	{		
		m_pNodePool.DestroyPool();
		if (!m_bFastCtorDtor)
			RemoveAllStd();
		delete[] m_pHashTbl;
		m_pHashTbl = NULL;
		m_nElements = 0;
	}

	void Rehash(int nNewTblSize = 0);
	bool Lookup(K& key, V& value);	
	V* Lookup(K& key);
	void* GetNextItem(void* pLastItem, K& key, V& value);
//operators	
	V& operator[](K& key);

protected:
//overides
	virtual DWORD GetHash(K& key);
	virtual int CompareKeys(K& key1, K& key2);
protected:
	THISHASHNODE* CreateHashNode(K& key, DWORD dwHash);
	THISHASHNODE* GetHashNode(K& key, DWORD dwHash);
	int GetNextTblSize();
	//DWORD GetJenkinsHash(void* pKey, int nLen);
	void RemoveAllStd();

protected:
	int m_nElements;			//number of elements in the hashtable (includes number of elements in buckets)
	int m_nHashTblSize;			//size of the m_pHashTbl
	int m_nMaxLoadElems;		//number of elements that are necessary to invoke automatic rehash
	bool m_bAutoRehash;			//true - automatic rehash is enabled
	float m_fLoadKoef;			//loading koeficient
	bool m_bFastCtorDtor;		//true -> use DynPool otherwise use std means (slower but has ctor and dtor as well)

	THISHASHNODE** m_pHashTbl;
	CDynPool<THISHASHNODE> m_pNodePool;
//for searching
	int m_iBrowsePos;
};

template<typename K, typename V, class NODE>
typename CHashTable<K, V, NODE>::THISHASHNODE* CHashTable<K, V, NODE>::CreateHashNode(K& key, DWORD dwHash)
{	
	if(m_pHashTbl == NULL)
	{
		m_pHashTbl = new CHashTable<K, V, NODE>::THISHASHNODE*[m_nHashTblSize];
		memset(m_pHashTbl, 0, sizeof(CHashTable<K, V, NODE>::THISHASHNODE*) * m_nHashTblSize);

		m_pNodePool.InitPool();
	}

	if ((++m_nElements) > m_nMaxLoadElems && m_bAutoRehash)
		Rehash(GetNextTblSize());
			
	CHashTable<K, V, NODE>::THISHASHNODE* pNode;
	if (m_bFastCtorDtor)	
		pNode = m_pNodePool.GetNewElement();	
	else
		pNode = new CHashTable<K, V, NODE>::THISHASHNODE;
	pNode->Construct(key, dwHash);	//initialize
	
	CHashTable<K, V, NODE>::THISHASHNODE** pRoot = &m_pHashTbl[dwHash % m_nHashTblSize];
	if (*pRoot == NULL)
		*pRoot = pNode;
	else
	{
		pNode->m_pNextNode = *pRoot;
		*pRoot = pNode;
	}

	return pNode;
}

template<typename K, typename V, class NODE>
int CHashTable<K, V, NODE>::GetNextTblSize()
{	
	// List of primes such that s_anPrimes[i] is the smallest prime greater than 2^(5+i/3)
	// adopted from Microsoft ATL library
	static int s_anPrimes[] =
	{
		17, 23, 29, 37, 41, 53, 67, 83, 103, 131, 163, 211, 257, 331, 409, 521, 647, 821, 
		1031, 1291, 1627, 2053, 2591, 3251, 4099, 5167, 6521, 8209, 10331, 
		13007, 16411, 20663, 26017, 32771, 41299, 52021, 65537, 82571, 104033, 
		131101, 165161, 208067, 262147, 330287, 416147, 524309, 660563, 
		832291, 1048583, 1321139, 1664543, 2097169, 2642257, 3329023, 4194319, 
		5284493, 6658049, 8388617, 10568993, 13316089,  HASH_TBLMAXSIZE,
	};

	
	float fSize = m_nElements / m_fLoadKoef;
	if (fSize >= HASH_TBLMAXSIZE)
		return HASH_TBLMAXSIZE;

	int nIdealSize = (int)fSize;

	// Find the smallest prime greater than our estimate
	int iPrime = 0;
	while(nIdealSize > s_anPrimes[iPrime])
	{
		iPrime++;
	}

	return s_anPrimes[iPrime];
}


template<typename K, typename V, class NODE>
void CHashTable<K, V, NODE>::Rehash(int nNewTblSize)
{
	if (nNewTblSize == 0)
		nNewTblSize = GetNextTblSize();

	if (nNewTblSize == m_nHashTblSize)
		return;

	CHashTable<K, V, NODE>::THISHASHNODE** pNewHashTbl = new CHashTable<K, V, NODE>::THISHASHNODE*[nNewTblSize];
	memset(pNewHashTbl, 0, sizeof(CHashTable<K, V, NODE>::THISHASHNODE*) * nNewTblSize);	

	for (int i = 0; i < m_nHashTblSize; i++)
	{
		CHashTable<K, V, NODE>::THISHASHNODE* pNode = m_pHashTbl[i];
		while (pNode != NULL)
		{
			CHashTable<K, V, NODE>::THISHASHNODE* pTmp = pNode->m_pNextNode;
			CHashTable<K, V, NODE>::THISHASHNODE** pRoot = &pNewHashTbl[pNode->m_Hash % nNewTblSize];
			if (*pRoot == NULL)
			{
				pNode->m_pNextNode = NULL;
				*pRoot = pNode;
			}
			else
			{
				pNode->m_pNextNode = *pRoot;
				*pRoot = pNode;
			}
			pNode = pTmp;
		}
	}

	delete[] m_pHashTbl;
	m_pHashTbl = NULL;
	InitHashTable(nNewTblSize);	
	m_pHashTbl = pNewHashTbl;
}

template<typename K, typename V, class NODE>
typename CHashTable<K, V, NODE>::THISHASHNODE* CHashTable<K, V, NODE>::GetHashNode(K& key, DWORD dwHash)
{
	if (m_pHashTbl == NULL)
		return NULL;

	CHashTable<K, V, NODE>::THISHASHNODE* pNode = m_pHashTbl[dwHash % m_nHashTblSize];
	while (pNode != NULL)
	{
		if (CompareKeys(key, pNode->m_Key) == 0)
			break;

		pNode = pNode->m_pNextNode;
	}

	return pNode;
}

template<typename K, typename V, class NODE>
inline V& CHashTable<K, V, NODE>::operator[](K& key)
{
	DWORD dwHash = GetHash(key);
	CHashTable<K, V, NODE>::THISHASHNODE* pNode = GetHashNode(key, dwHash);	
	if (pNode == NULL)	
		pNode = CreateHashNode(key, dwHash );	

	return pNode->m_Value;
}

template<typename K, typename V, class NODE>
bool CHashTable<K, V, NODE>::Lookup(K& key, V& value) 
{
	DWORD dwHash = GetHash(key);
	CHashTable<K, V, NODE>::THISHASHNODE* pNode = GetHashNode(key, dwHash);	
	if (pNode == NULL)	
		return false;

	value = pNode->m_Value;
	return true;
}

template<typename K, typename V, class NODE>
V* CHashTable<K, V, NODE>::Lookup(K& key) 
{
	DWORD dwHash = GetHash(key);
	CHashTable<K, V, NODE>::THISHASHNODE* pNode = GetHashNode(key, dwHash);	
	if (pNode == NULL)	
		return NULL;

	return &pNode->m_Value;	
}

template<typename K, typename V, class NODE>
void CHashTable<K, V, NODE>::Remove(K& key)
{
	if (m_pHashTbl == NULL)
		return;

	DWORD dwHash = GetHash(key);

	CHashTable<K, V, NODE>::THISHASHNODE** pPrevLink = &m_pHashTbl[dwHash % m_nHashTblSize];
	CHashTable<K, V, NODE>::THISHASHNODE* pNode = *pPrevLink;
	while (pNode != NULL)
	{
		if (CompareKeys(key, pNode->m_Key) == 0)
			break;

		pPrevLink = &pNode->m_pNextNode;
		pNode = pNode->m_pNextNode;
	}

	if (pNode != NULL)
	{
		//founded - remove it
		*pPrevLink = pNode->m_pNextNode;
		if (m_bFastCtorDtor)
			m_pNodePool.DestroyElement(pNode);
		else
			delete pNode;

		m_nElements--;
	}
}

template<typename K, typename V, class NODE>
void CHashTable<K, V, NODE>::RemoveAllStd()
{
	if (m_pHashTbl == NULL)
		return;

	for (int iBr = 0; iBr < m_nHashTblSize; iBr++)
	{
		CHashTable<K, V, NODE>::THISHASHNODE* pNode = m_pHashTbl[iBr];
		while (pNode != NULL)
		{
			CHashTable<K, V, NODE>::THISHASHNODE* pNodeNext = pNode->m_pNextNode;
			delete pNode;
			pNode = pNodeNext;
		}
	}	
}

template<typename K, typename V, class NODE>
void* CHashTable<K, V, NODE>::GetNextItem(void* pLastItem, K& key, V& value)
{
	if (m_pHashTbl == NULL)
		return NULL;

	CHashTable<K, V, NODE>::THISHASHNODE* pNode = (CHashTable<K, V, NODE>::THISHASHNODE*)pLastItem;
	if (pNode == NULL)
		m_iBrowsePos = -1;
	else
		pNode = pNode->m_pNextNode;

	if (pNode == NULL)
	{
		while ((++m_iBrowsePos) < m_nHashTblSize)
		{
			if (m_pHashTbl[m_iBrowsePos] != NULL)
			{
				pNode = m_pHashTbl[m_iBrowsePos];
				break;
			}
		}
	}

	if (pNode != NULL)
	{
		key = pNode->m_Key;
		value = pNode->m_Value;
	}
	return (void*)pNode;
}

template<typename K, typename V, class NODE>
int CHashTable<K, V, NODE>::CompareKeys(K& key1, K& key2)
{
	if (key1 < key2)
		return -1;
	else if (key1 > key2)
		return 1;
	else
		return 0;
}

template<typename K, typename V, class NODE>
DWORD CHashTable<K, V, NODE>::GetHash(K& key)
{
	return GetJenkinsHash(&key, sizeof(key));
}
#pragma endregion
#endif

#endif //BES_HASHTABLE_H