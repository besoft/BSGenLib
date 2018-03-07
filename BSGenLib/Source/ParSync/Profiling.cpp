#include "stdafx.h"
//#include "profiling.h"

//resets the position
/*virtual*/ void CProfileNode::CProfileNodeEnumerator::Reset()
{
#ifdef __BSGEN_PUREWIN
	m_pCurrentPos = NULL;
#else
	m_pCurrentPos = m_pNode->m_SubNodes.GetStartPosition();
#endif
}

//moves the position to the next node
//returns  false if the enumerator has passed the end of the collection
/*virtual*/ bool CProfileNode::CProfileNodeEnumerator::MoveNext()
{
#ifdef __BSGEN_PUREWIN
	if (m_pCurrentPos == (POSITION)-1)
		return FALSE;	//EOF

	CString szName;	
	LPVOID lpSave = (LPVOID)m_pCurrentPos;
	m_pCurrentPos = (POSITION)m_pNode->m_SubNodes.GetNextItem(lpSave, szName, m_pCurrent);
	if (m_pCurrentPos == NULL)
		m_pCurrentPos = (POSITION)-1;	//EOF
#else
	if (m_pCurrentPos == NULL)
		return false;

	m_pCurrent = m_pNode->m_SubNodes.GetNextValue(m_pCurrentPos);	
#endif
	return true;
}

//dtor - the entire collection is to be removed
CProfileNode::~CProfileNode()
{
#ifdef __BSGEN_PUREWIN
	if (!m_SubNodes.IsEmpty())
	{
		CString szName;	
		CProfileNode* pNode;
		LPVOID lPos = NULL;
		while (NULL != (lPos = m_SubNodes.GetNextItem(lPos, szName, pNode)))
		{
			delete pNode;
		}
	}

#else
	POSITION pos = m_SubNodes.GetStartPosition();
	while (pos != NULL)
	{
    CProfileNode* pNode = m_SubNodes.GetNextValue(pos);
		delete pNode;
	}
#endif
}

//adds the given node into the collection
//note: it calls AddRef upon the addition
/*virtual*/ void CProfileNode::AddSubNode(CProfileNode* pNode)
{
	CString szName = pNode->GetName();
	if (szName.IsEmpty())
		szName.Format(_T("#Ref_%x"), ::GetTickCount());
	else
	{
		CProfileNode* pExistingNode = GetSubNode(szName);
		if (pExistingNode != NULL)
			szName.AppendFormat(_T(" #Ref_%x"), ::GetTickCount());		
	}

	m_SubNodes[szName] = pNode;
}

//removes the subnode with the given name from the collection	
/*virtual*/ CProfileNode* CProfileNode::RemoveSubNode(LPCTSTR lpszName)
{
	CProfileNode* pRet = GetSubNode(lpszName);
	if (pRet != NULL)
	{
#ifdef __BSGEN_PUREWIN
		CString szName = lpszName;
		m_SubNodes.Remove(szName);	
#else
		m_SubNodes.RemoveKey(lpszName);
#endif
	}
	return pRet;
}

//searches the collection for the node with lpszName
//returns NULL if not found 
/*virtual*/ CProfileNode* CProfileNode::GetSubNode(LPCTSTR lpszName)
{
	CProfileNode* pRet = NULL;
#ifdef __BSGEN_PUREWIN
	CString szName = lpszName;
	pRet = *m_SubNodes.Lookup(szName);	
#else
	m_SubNodes.Lookup(lpszName, pRet);
#endif	
	return pRet;
}

//creates a clone of this node and its subnodes
CProfileNode* CProfileNode::Clone()
{
	CProfileNode* pRet = CloneNode();
	CProfileNodeEnumerator* pEnum = GetEnumerator();
	while (pEnum->MoveNext())
	{
		CProfileNode* pNode = pEnum->GetCurrent();
		pRet->AddSubNode(pNode->Clone());
	}	

	delete pEnum;
	return pRet;
}


//returns XML string of this node and its subnodes (recursive function)
/*virtual*/ CString CProfileNode::ToString()
{
	CString szRet;
	szRet.Format(_T("<item name=\"%s\" value=\"%s\""), GetName(), GetStrValue());

	CProfileNodeEnumerator* pEnum = GetEnumerator();
	if (!pEnum->MoveNext())
		szRet += _T("/>\r\n");
	else
	{
		szRet += _T('>');

		do
		{
			CProfileNode* pNode = pEnum->GetCurrent();		
			CString szStr = _T("\r\n") + pNode->ToString();
			szStr.Replace(_T("\n"), _T(" "));
			szRet += szStr;		
		}
		while (pEnum->MoveNext());

		szRet += _T("</item>");
	}

	delete pEnum;
	return szRet;
}

//creates a clone of this node
/*virtual*/ CProfileNode* CProfileTimer::CloneNode()
{
	CProfileTimer* pClone = new CProfileTimer(m_szName);

	pClone->m_liElapsed = m_liElapsed;
	pClone->m_nCount = m_nCount;

	pClone->m_liFreq = m_liFreq;
	pClone->m_liStart = m_liStart;
	pClone->m_liEnd = m_liEnd;
	pClone->m_bRunning = m_bRunning;
	return pClone;
}


//returns value of this node
/*virtual*/ CString CProfileTimer::GetStrValue()
{	
	return CBSUtil::FormatTime(GetTotalElapsedTimeInMs());	
}

//returns XML string of this node and its subnodes (recursive function)
/*virtual*/ CString CProfileTimer::ToString()
{
	CString szRet;
	szRet.Format(_T("<item name=\"%s\" value=\"%s\">\r\n")
		_T(" <subitem name=\"TotalTime\" value=\"%.2f\"/>\r\n")
		_T(" <subitem name=\"LastTime\" value=\"%.2f\"/>\r\n")
		_T(" <subitem name=\"Count\" value=\"%d\"/>\r\n"),
		GetName(), GetStrValue(),
		/*CBSUtil::FormatTime(*/GetTotalElapsedTimeInMs(), 
		/*CBSUtil::FormatTime(*/GetElapsedTimeInMs(), GetCount());

	CProfileNodeEnumerator* pEnum = GetEnumerator();
	while (pEnum->MoveNext())
	{
		CProfileNode* pNode = pEnum->GetCurrent();
		CString szStr = _T("\r\n") + pNode->ToString();
		szStr.Replace(_T("\n"), _T(" "));
		szRet += szStr;		
	}
	delete pEnum;

	szRet += _T("</item>");	
	return szRet;	
}

/*virtual*/ CString CProfileFuncTimer::GetStrValue()
{
	CString szRet;
	szRet.Format(_T("%s in %d calls (%s per call)"), 
		CBSUtil::FormatTime(GetTotalElapsedTimeInMs()), GetCount(), 
		CBSUtil::FormatTime(GetTotalElapsedTimeInMs() / GetCount()));
	return szRet;
}

//creates a clone of this node
/*virtual*/ CProfileNode* CProfileFuncTimer::CloneNode()
{
	CProfileFuncTimer* pClone = new CProfileFuncTimer(m_szName);

	pClone->m_liElapsed = m_liElapsed;
	pClone->m_nCount = m_nCount;

	pClone->m_liFreq = m_liFreq;
	pClone->m_liStart = m_liStart;
	pClone->m_liEnd = m_liEnd;
	pClone->m_bRunning = m_bRunning;
	return pClone;
}

const _TCHAR* CProfiler::ProfilerCategory::m_Strings[] = {_T("Counters"), _T("Timers"), 
_T("Functions"), _T("Others"), NULL};

//appends subtree under the given category
void CProfiler::Append(CProfileNode* pNode, LPCTSTR lpszCategory)
{
	CProfileNode* pCateg = GetProfVar(lpszCategory, ProfilerCategory::Others);
	pCateg->AddSubNode(pNode);
}

//appends subtree under the given category (without ROOT)
void CProfiler::Append(CProfiler* pProfiler, LPCTSTR lpszCategory)
{
	CProfileNode* pCateg = m_pRootNode;
	if (lpszCategory != NULL && *lpszCategory != _T('\0'))
		pCateg = GetProfVar(lpszCategory, ProfilerCategory::Others);

	CProfileNode::CProfileNodeEnumerator* pEnum = pProfiler->m_pRootNode->GetEnumerator();
	while (pEnum->MoveNext())
	{
		CProfileNode* pNode = pEnum->GetCurrent();
		pCateg->AddSubNode(pNode->Clone());
	}
	delete pEnum;
}

//returns the full address of the local variable name
CString CProfiler::GetFullProfVarAddr(LPCTSTR lpszProfVar, ProfilerCategory categ)
{
	CString szRet = lpszProfVar;
	if (szRet.Find(_T("ROOT|")) == 0)
		return szRet;	//the given address is the full

	//test category
	const _TCHAR** pCurCateg = ProfilerCategory::GetStrings();
	while (*pCurCateg != NULL)
	{
		if (szRet.Find(*pCurCateg) == 0)
		{
			//we found it
			szRet.Insert(0, _T("ROOT|"));
			return szRet;
		}

		pCurCateg++;
	}

	//not found
	szRet.Format(_T("ROOT|%s|%s"), categ.ToString(), lpszProfVar);
	return szRet;
}

//returns node for the given lpszProfVar, if it does not exist, it is 
//created (either timer or counter)
//Note: lpszProVar contains full address (including categories),
//categories are delimited by ::
//e.g. ROOT::Timers::MyCounter
CProfileNode* CProfiler::GetProfVar2(LPCTSTR lpszProfVar, ProfilerCategory categ, bool bAmbiguous)
{
	//either lpszProfVar was not found in the cache or its location was ambiguous
	CString szID = lpszProfVar;
	if (bAmbiguous)
	{
		szID += (_TCHAR)(_T(' ') + categ);
#ifndef __BSGEN_PUREWIN
		CCacheMap::CPair* pCurVal = m_Cache.Lookup(szID);
		if (pCurVal != NULL)
			return pCurVal->m_value.pNode;
#else
		CACHE_ITEM* pCurVal = m_Cache.Lookup(szID);
		if (pCurVal != NULL)
			return pCurVal->pNode;
#endif
	}

	//not found	in cache
	CStringEx szProfVar = GetFullProfVarAddr(lpszProfVar, categ);
	szProfVar.Replace(_T("<"), _T("&lt;"));
	szProfVar.Replace(_T(">"), _T("&gt;"));

	CArrayOfStrings* pArrs = szProfVar.Split(_T("|"));

	CString szName;
	CProfileNode* pNextNode;
	CProfileNode* pRoot = m_pRootNode;
	for (int i = 1; i < (int)pArrs->GetCount() - 1; i++)
	{
		szName = pArrs->GetAt(i);
		pNextNode = pRoot->GetSubNode(szName);
		if (pNextNode == NULL)
		{
			//this category does not exist, create it
			pNextNode = new CProfileNode(szName);
			pRoot->AddSubNode(pNextNode);
		}

		pRoot = pNextNode;
	}

	//pRoot denotes the category
	DWORD dwThreadID = 0;
	szName = pArrs->GetAt(pArrs->GetCount() - 1);	
	if (m_bThreadProfiling)
		szName.AppendFormat(_T("@THREAD_%04x"), dwThreadID = ::GetCurrentThreadId());	

	pNextNode = pRoot->GetSubNode(szName);
	if (pNextNode == NULL)
	{
		//object does not exit
		switch (categ)
		{
		case ProfilerCategory::Timers:
			pNextNode = new CProfileTimer(szName); break;
		case ProfilerCategory::Counters:
			pNextNode = new CProfileCounter(szName); break;
		case ProfilerCategory::Functions:
			pNextNode = new CProfileFuncTimer(szName); break;
		default:			
			pNextNode = new CProfileInfo(szName); break;
		}

		pRoot->AddSubNode(pNextNode);
	}

	delete pArrs;

	//put the item into the cache
	CACHE_ITEM item;
	item.categ = categ;
	item.pNode = pNextNode;
	item.dwThreadID = dwThreadID;
	
	m_Cache[szID] = item;

	return pNextNode;
}

//removes all profiled objects in one category
void CProfiler::RemoveProfile(ProfilerCategory categ)
{	
	m_cs.Lock();
	delete m_pRootNode->RemoveSubNode(categ.ToString());
	m_Cache.RemoveAll();
	m_cs.Unlock();
}

//resets all profiled values in one category
void CProfiler::ResetProfile(ProfilerCategory categ)
{
	m_cs.Lock();

	CStack<CProfileNode*> stack;
	stack.Push(m_pRootNode->GetSubNode(categ.ToString()));

	CProfileNode* pNode;
	while (!stack.IsEmpty())
	{		
		stack.Pop(pNode);

		if (pNode != NULL)
		{
			CProfileNode::CProfileNodeEnumerator* pEnum = pNode->GetEnumerator();
			while (pEnum->MoveNext())			
				stack.Push(pEnum->GetCurrent());

			delete pEnum;

			pNode->ResetValue();
		}
	}

	m_cs.Unlock();
}

//saves the profile into the file (XML format)
//returns false, if it cannot be saved
bool CProfiler::SaveProfile(LPCTSTR lpszPathName)
{
	m_cs.Lock();

	FILE* fp = NULL;
#if _MSC_VER >= 1400
	if (0 != _tfopen_s(&fp, lpszPathName, _T("wt"))) //, ccs=UTF-8")))
#else
	if (NULL == (fp = _tfopen(lpszPathName, _T("wt"))))
#endif
	{
		m_cs.Unlock();
		return false;
	}

	_ftprintf_s(fp, _T("<?xml version=\"1.0\" encoding=\"utf-8\"?>\r\n"));
	_ftprintf_s(fp, m_pRootNode->ToString());

	fclose(fp);
	m_cs.Unlock();
	return true;
}