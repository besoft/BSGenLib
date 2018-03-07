/* ${FILENAME}
** ${LASTMODIFIED}
**
** ${COPYRIGHT}
** ${LICENCE}
**
** DESCRIPTION: Template - Dynamic Pool of items
*/

#ifndef BES_DYNPOOL_H
#define BES_DYNPOOL_H

#include "memmng.h"

template<typename E>
class CDynPool
{
public:
 	CDynPool(void){ m_pMemMng = NULL; InitPool();}
	~CDynPool(void){DestroyPool();}	

	inline E* GetNewElement() 
	{ 
		return (E*) m_pMemMng->GetMem(sizeof(E)); 
	}

	inline E* GetBlockOfNewElements(int nElems)
	{
		BYTE* pData = m_pMemMng->GetMem(sizeof(int) + sizeof(E)*nElems); 
		(*(int*)pData) = nElems;
		return (E*)(pData + sizeof(int))
	}

	inline void DestroyElement(E* pElem) 
	{ 
		if (pElem == NULL)
			return;
		m_pMemMng->FreeMem((BYTE*)pElem, sizeof(E));
	}

	inline void DestroyBlockOfElements(E* pElems)
	{
		if (pElems == NULL)
			return;
		BYTE* pData = ((BYTE*)pElems) - sizeof(int);
		m_pMemMng->FreeMem(pData, sizeof(int) + sizeof(E)*(*((int*)pData)));		
	}

	inline void InitPool()
	{
		if (m_pMemMng == NULL)
			m_pMemMng = new CMemMng();
	}

	inline void DestroyPool()
	{
		delete m_pMemMng;
		m_pMemMng = NULL;
	}

protected:
	CMemMng* m_pMemMng;	
};

#endif //BES_DYNPOOL_H