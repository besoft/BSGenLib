#pragma once

///////////////////////////////////////////////////////////////
// RED-BLACK Tree
//
// Implemented with help of http://www.csua.berkeley.edu/~emin/source_code/cpp_trees/RedBlackTree.cc code

#define RB_NODE_INNER		0
#define RB_NODE_ROOT		1
#define RB_NODE_EXTERNAL	-1

//Red-Black tree template class
template < typename T_KEY, typename T_VALUE >
class CRedBlackTree
{
public:
	typedef const T_KEY& T_KEY_ARG_IN;
	typedef const T_VALUE& T_VAL_ARG_IN;
	typedef const T_KEY* T_KEY_ARG_PIN;
	typedef const T_VALUE* T_VAL_ARG_PIN;
	typedef T_KEY& T_KEY_ARG_OUT;	
	typedef T_VALUE& T_VAL_ARG_OUT;
	typedef T_KEY* T_KEY_ARG_POUT;
	typedef T_VALUE* T_VAL_ARG_POUT;

protected:
	//this class represents both inner and external nodes
	class CRedBlackTreeItem
	{
	protected:
		//one of RB_NODE_xxx
		int m_nNodeType;

		//color of inner node, true if it is red, false otherwise
		bool m_bRedNode;
		
		//key and associated value - valid only for inner nodes
		T_KEY m_key;
		T_VALUE m_value;

		//parent
		CRedBlackTreeItem* m_pParent;

		//pointers to the left and right children
		CRedBlackTreeItem* m_pLeft;
		CRedBlackTreeItem* m_pRight;

		//pointers to previous and next inner nodes
		CRedBlackTreeItem* m_pPrev;
		CRedBlackTreeItem* m_pNext;


	public:
		CRedBlackTreeItem(int nNodeType, bool bRedNode)
		{
			m_nNodeType = nNodeType;
			m_bRedNode = bRedNode;
			m_pParent = NULL;
			m_pLeft = m_pRight = NULL;
		}

		CRedBlackTreeItem(T_KEY_ARG_IN key, T_VAL_ARG_IN value) : 
		  m_key(key), m_value(value)
		{	
			m_nNodeType = RB_NODE_INNER;
			m_bRedNode = true;
			m_pParent = NULL;
			m_pLeft = m_pRight = NULL;			
			m_pPrev = m_pNext = NULL;
		}

		//returns types of node
		inline bool IsRootNode() { return m_nNodeType == RB_NODE_ROOT; }
		inline bool IsInnerNode() { return m_nNodeType == RB_NODE_INNER; }
		inline bool IsExternalNode() { return m_nNodeType == RB_NODE_EXTERNAL; }
		

		//returns whether node is red
		inline bool IsRedNode() { return m_bRedNode; }
		inline bool IsBlackNode() { return !m_bRedNode; }

		//methods used for setting colour
		inline void SetNodeRed() { m_bRedNode = true; }
		inline void SetNodeBlack() { m_bRedNode = false; }
		inline void SetNodeColour(bool bRed) { m_bRedNode = bRed; }

		//returns parent
		inline CRedBlackTreeItem* GetParentNode() { return m_pParent; }

		//sets new parent node
		inline void SetParentNode(CRedBlackTreeItem* pParent) { m_pParent = pParent; }	

		//browsing x setting children
		inline CRedBlackTreeItem* GetLeftNode() { return m_pLeft; }
		inline CRedBlackTreeItem* GetRightNode() { return m_pRight; }
		inline void SetLeftNode(CRedBlackTreeItem* p) { m_pLeft = p; }
		inline void SetRightNode(CRedBlackTreeItem* p) { m_pRight = p; }	

		//returns key
		inline T_KEY_ARG_IN GetKey() { return m_key; }
		inline T_KEY_ARG_PIN GetPKey() { return &m_key; }

		//gets x sets value
		inline T_VAL_ARG_OUT GetValue() { return m_value; }
		inline T_VAL_ARG_POUT GetPValue() { return &m_value; }
		inline void SetValue(T_VAL_ARG_IN value) { m_value = value; }		

		//browsing x setting links
		inline CRedBlackTreeItem* GetPrevNode() { return m_pPrev; }
		inline CRedBlackTreeItem* GetNextNode() { return m_pNext; }
		inline void SetPrevNode(CRedBlackTreeItem* p) { m_pPrev = p; }
		inline void SetNextNode(CRedBlackTreeItem* p) { m_pNext = p; }	
	};	

	
//attributes
protected:
	CRedBlackTreeItem* m_pRoot;
	UINT m_nCount;	//the number of elements in the tree

public:
	CRedBlackTree(void)
	{	
		InitRBTree();
	}

	~CRedBlackTree(void)
	{
		RemoveAll();

		delete m_pRoot->GetLeftNode();
		delete m_pRoot->GetRightNode();
		delete m_pRoot;
	}

//operations
public:
	//adds new value
	inline void Add(T_KEY_ARG_IN key, T_VAL_ARG_IN value);

	//search for an item identified by key, returns false if not found,
	//otherwise true + the value associated to this key
	inline bool Lookup(T_KEY_ARG_IN key, T_VAL_ARG_OUT value);

	//remove the specified item
	inline void RemoveKey(T_KEY_ARG_IN key);

	//removes all items
	inline void RemoveAll();

	//gets the number of elements in the tree
	inline UINT GetCount() {
		return m_nCount;
	}

	//adds new item at proper position (searching from position pos)
	inline POSITION AddAt(POSITION pos, T_KEY_ARG_IN key, T_VAL_ARG_IN value);

	//search for key in the tree, starting at posParent (may be NULL if the search should start at root)
	//and returns position of the item
	inline POSITION FindNode(T_KEY_ARG_IN key);

	//adds new value at the given position
	inline void RemoveAt(POSITION pos);
	
	//returns key for the given position, valid only for inner nodes
	inline void GetKeyAt(POSITION pos, T_KEY_ARG_OUT key);

	//returns key for the given position, valid only for inner nodes
	inline T_KEY_ARG_POUT GetPKeyAt(POSITION pos);

	//returns value for the given position, valid only for inner nodes
	inline void GetValueAt(POSITION pos, T_VAL_ARG_OUT value);

	//returns value for the given position, valid only for inner nodes
	inline T_VAL_ARG_POUT GetPValueAt(POSITION pos);

	//returns value for the given position, valid only for inner nodes
	inline void GetAssocAt(POSITION pos, T_KEY_ARG_OUT key, T_VAL_ARG_OUT value);

	//returns value for the given position, valid only for inner nodes
	inline void GetPAssocAt(POSITION pos, T_KEY_ARG_POUT key, T_VAL_ARG_POUT value);

	//returns wheter the node denoted by the given pos is a leaf or not
	inline bool IsInnerNode(POSITION pos);

	//finds the first node in list, pos may be NULL
	POSITION GetFirstNode(POSITION pos);

	//finds the last node in list, pos may be NULL
	POSITION GetLastNode(POSITION pos);

	//finds the first node in list, pos may not be NULL
	inline POSITION GetNextNode(POSITION pos);

	//finds the last node in list, pos may not be NULL
	inline POSITION GetPreviousNode(POSITION pos);

	//finds the left node of pos, pos may not be NULL
	inline POSITION GetLeftNode(POSITION pos);

	//finds the right node of pos, pos may not be NULL
	inline POSITION GetRightNode(POSITION pos);

	//finds the parent node of pos, pos may not be NULL
	inline POSITION GetParentNode(POSITION pos);

	//finds the root, all inner nodes are in the left subtree
	inline POSITION GetRootNode();

	//finds the root of user data, returns NULL if no such node exists
	inline POSITION GetRootInnerNode();

	//returns true if there is no data in the tree, i.e., there is no inner node
	inline bool IsEmpty();

//operations
protected:
	//this routine searches for the inner node with given key, if there is no match, the routine returns external node
	CRedBlackTreeItem* Find(T_KEY_ARG_IN key);	

	//adds new item at proper position (searching from position pos)
	CRedBlackTreeItem* AddAt(CRedBlackTreeItem* pItem, T_KEY_ARG_IN key, T_VAL_ARG_IN value);
	
	//adds new value at the given position
	void RemoveAt(CRedBlackTreeItem* pItem);

	//this routine rebalances the tree (if necessary) after adding an item pItem
	void FixupAdd(CRedBlackTreeItem* pItem);

	//this routine rebalances the tree (if necessary) after removal of an item
	void FixupRemove(CRedBlackTreeItem* pItem);

	//this routine performs the left rotation, i.e. basically this routine
	//makes the parent of pItem be to the left of pItem, pItem the parent of
	//its parent before the rotation and fixes other pointers accordingly
	void LeftRotate(CRedBlackTreeItem* pItem);

	//this routine performs the right rotation, i.e. basically this routine 
	//makes the parent of x be to the left of x, x the parent of its parent 
	//before the rotation and fixes other pointers accordingly. 
	void RightRotate(CRedBlackTreeItem* pItem);

	//recursive function to simple free data
	void RemoveTree(CRedBlackTreeItem* pRoot);

	//creates initial configuration
	void InitRBTree();
	

#ifdef _DEBUG
	//this routine is used to check tree
	int AssertValid(CRedBlackTreeItem* pRoot = m_pRoot);
#endif
};

//this routine searches for the inner node with given key, if there is no match, the routine returns external node
template < typename T_KEY, typename T_VALUE >
typename CRedBlackTree< T_KEY, T_VALUE >::CRedBlackTreeItem* CRedBlackTree< T_KEY, T_VALUE >::Find(T_KEY_ARG_IN key)
{
	CRedBlackTreeItem* pRet = m_pRoot->GetLeftNode();
	while (!pRet->IsExternalNode())
	{
		T_KEY_ARG_IN keyNode = ((CRedBlackTreeItem*)pRet)->GetKey();
		if (key < keyNode)
			pRet = pRet->GetLeftNode();		
		else if (key > keyNode)		
			pRet = pRet->GetRightNode();		
		else
			return pRet;
	}

	return pRet;
}

//search for an item identified by key, returns false if not found,
//otherwise true + the value associated to this key
template < typename T_KEY, typename T_VALUE >
inline bool CRedBlackTree< T_KEY, T_VALUE >::Lookup(T_KEY_ARG_IN key, T_VAL_ARG_OUT value)
{	
	CRedBlackTreeItem* pItem = Find(key);
	if (pItem->IsExternalNode())
		return false;
	else
	{
		value = ((CRedBlackTreeItem*)pItem)->GetValue();
		return true;
	}
}

//search for key in the tree, starting at posParent (may be NULL if the search should start at root)
//and returns position of the item
template < typename T_KEY, typename T_VALUE >
inline POSITION CRedBlackTree< T_KEY, T_VALUE >::FindNode(T_KEY_ARG_IN key)
{
	return (POSITION)Find(key);
}

//removes all items
template < typename T_KEY, typename T_VALUE >
void CRedBlackTree< T_KEY, T_VALUE >::RemoveAll()
{
	RemoveTree(m_pRoot);
	InitRBTree();
}

//recursive function to simple free data
template < typename T_KEY, typename T_VALUE >
void CRedBlackTree< T_KEY, T_VALUE >::RemoveTree(CRedBlackTreeItem* pRoot)
{
	if (pRoot != NULL)
	{		
		RemoveTree(pRoot->GetLeftNode());
		RemoveTree(pRoot->GetRightNode());	
		delete pRoot;
	}
}

//initialization of the RB tree, constructs root and its associated external nodes
//all data is going to be placed in its left subtree
template < typename T_KEY, typename T_VALUE >
void CRedBlackTree< T_KEY, T_VALUE >::InitRBTree()
{
	m_pRoot = new CRedBlackTreeItem(RB_NODE_ROOT, false);

	CRedBlackTreeItem* pLeftNil = new CRedBlackTreeItem(RB_NODE_EXTERNAL, false);
	m_pRoot->SetLeftNode(pLeftNil);
	pLeftNil->SetParentNode(m_pRoot);

	CRedBlackTreeItem* pRightNil = new CRedBlackTreeItem(RB_NODE_EXTERNAL, false);		
	m_pRoot->SetRightNode(pRightNil);		
	pRightNil->SetParentNode(m_pRoot);

	m_nCount = 0;
}

//adds new item at proper position (searching from position pos)
template < typename T_KEY, typename T_VALUE >
typename CRedBlackTree< T_KEY, T_VALUE >::CRedBlackTreeItem* CRedBlackTree< T_KEY, T_VALUE >::AddAt(CRedBlackTreeItem* pItem, T_KEY_ARG_IN key, T_VAL_ARG_IN value)
{
	if (pItem->IsInnerNode())			//a node with the same key already exists => just replace value
	{
		((CRedBlackTreeItem*)pItem)->SetValue(value);	
		return pItem;
	}
	else 
	{		
		//let us construct a new internal node and also one new external node
		CRedBlackTreeItem* pNewItem = new CRedBlackTreeItem(key, value);
		CRedBlackTreeItem* pNewExtItem = new CRedBlackTreeItem(RB_NODE_EXTERNAL, false);		
		
		//join new inner node with pItem and pNewExtItem
		pNewItem->SetLeftNode(pNewExtItem);
		pNewExtItem->SetParentNode(pNewItem);		
	
		CRedBlackTreeItem* pParent = pItem->GetParentNode();
		pNewItem->SetRightNode(pItem);
		pItem->SetParentNode(pNewItem);

		//join new inner node to pParent
		pNewItem->SetParentNode(pParent);
		if (pItem == pParent->GetLeftNode())
		{
			//we need to join to pParent from left side
			pParent->SetLeftNode(pNewItem);

			if (pParent->IsInnerNode())
			{
				//and link item into list of external data				
				CRedBlackTreeItem* pPrev = pParent->GetPrevNode();
				pNewItem->SetNextNode(pParent);
				pParent->SetPrevNode(pNewItem);

				if (pPrev != NULL)
				{
					pNewItem->SetPrevNode(pPrev);
					pPrev->SetNextNode(pNewItem);
				}
			}
		}
		else
		{
			//join to pParent from right side
			pParent->SetRightNode(pNewItem);

			if (pParent->IsInnerNode())
			{
				//and link item into list of external data				
				CRedBlackTreeItem* pNext = pParent->GetNextNode();
				pNewItem->SetPrevNode(pParent);
				pParent->SetNextNode(pNewItem);

				if (pNext != NULL)
				{
					pNewItem->SetNextNode(pNext);
					pNext->SetPrevNode(pNewItem);
				}
			}
		}

		//we have to rebalance tree if it is necessary
		FixupAdd(pNewItem);		
		m_pRoot->GetLeftNode()->SetNodeBlack();		//root must be black

	#ifdef _DEBUG
		AssertValid(m_pRoot->GetLeftNode());
	#endif

		m_nCount++;
		return pNewItem;
	}
}


//adds new value
template < typename T_KEY, typename T_VALUE >
inline void CRedBlackTree< T_KEY, T_VALUE >::Add(T_KEY_ARG_IN key, T_VAL_ARG_IN value)
{
	AddAt(Find(key), key, value);
}

//adds new value at proper position and returns position of new uitem
template < typename T_KEY, typename T_VALUE >
inline POSITION CRedBlackTree< T_KEY, T_VALUE >::AddAt(POSITION pos, T_KEY_ARG_IN key, T_VAL_ARG_IN value)
{
	if (pos == NULL)
		return (POSITION)AddAt(Find(key), key, value);
	else
		return (POSITION)AddAt((CRedBlackTreeItem*)pos, key, value);
}
		

//this routine rebalances the tree (if necessary) after adding an item pItem
template < typename T_KEY, typename T_VALUE >
void CRedBlackTree< T_KEY, T_VALUE >::FixupAdd(CRedBlackTreeItem* pItem)
{
	CRedBlackTreeItem* pPar;	//parent
	while ((pPar = pItem->GetParentNode())->IsRedNode())
	{
		//get grandparent - it must exists because root must be black		
		CRedBlackTreeItem* pGrandPar = pPar->GetParentNode();
		if (pPar == pGrandPar->GetLeftNode())
		{
			//we are in the left subtree
			CRedBlackTreeItem* pParSibling = pGrandPar->GetRightNode();
			if (pParSibling->IsRedNode())
			{
				//simple recoloring				
				pParSibling->SetNodeBlack();
				pItem = pGrandPar;				
			}
			else
			{
				if (pItem == pPar->GetRightNode())
				{
					//pItem, pPar and pGrandPar does not form a simple list
					//we need to rotate them first to the left
					//i.e. pItem and pPar will swap its position => the tripple now forms list  
					LeftRotate(pPar);
					
					pItem = pPar;
					pPar = pItem->GetParentNode();
				}

				//pItem, pPar and pGrandPar form a simple list
				RightRotate(pGrandPar);			
			}

			//recolor the rest
			pPar->SetNodeBlack();
			pGrandPar->SetNodeRed();
		}
		else
		{
			//we are in the right subtree - mirrored actions must be performed
			CRedBlackTreeItem* pParSibling = pGrandPar->GetLeftNode();
			if (pParSibling->IsRedNode())
			{
				//simple recoloring				
				pParSibling->SetNodeBlack();
				pItem = pGrandPar;				
			}
			else
			{
				if (pItem == pPar->GetLeftNode())
				{
					//pItem, pPar and pGrandPar does not form a simple list
					//we need to rotate them first to the left
					//i.e. pItem and pPar will swap its position => the tripple now forms list  
					RightRotate(pPar);
					
					pItem = pPar;
					pPar = pItem->GetParentNode();
				}

				//pItem, pPar and pGrandPar form a simple list
				LeftRotate(pGrandPar);			
			}

			//recolor the rest
			pPar->SetNodeBlack();
			pGrandPar->SetNodeRed();
		}
	}
}


//this routine performs the left rotation, i.e. basically this routine
//makes the parent of pItem be to the left of pItem, pItem the parent of
//its parent before the rotation and fixes other pointers accordingly
template < typename T_KEY, typename T_VALUE >
void CRedBlackTree< T_KEY, T_VALUE >::LeftRotate(CRedBlackTreeItem* pItem)
{
	//pChild must exist, pParent may not	
	CRedBlackTreeItem* pChild = pItem->GetRightNode();
	CRedBlackTreeItem* pParent = pItem->GetParentNode(); 
	if (pItem == pParent->GetLeftNode())
		pParent->SetLeftNode(pChild);
	else
		pParent->SetRightNode(pChild);

	pChild->SetParentNode(pParent);	

	//pGrandChild may be an external node
	CRedBlackTreeItem* pGrandChild = pChild->GetLeftNode();	
	pGrandChild->SetParentNode(pItem);
	pItem->SetRightNode(pGrandChild);
	
	pChild->SetLeftNode(pItem);
	pItem->SetParentNode(pChild);
}

//this routine performs the right rotation, i.e. basically this routine 
//makes the parent of x be to the left of x, x the parent of its parent 
//before the rotation and fixes other pointers accordingly. 
template < typename T_KEY, typename T_VALUE >
void CRedBlackTree< T_KEY, T_VALUE >::RightRotate(CRedBlackTreeItem* pItem)
{
	//pChild must exist, pParent mmay not
	CRedBlackTreeItem* pChild = pItem->GetLeftNode();
	CRedBlackTreeItem* pParent = pItem->GetParentNode(); 	
	if (pItem == pParent->GetLeftNode())
		pParent->SetLeftNode(pChild);
	else
		pParent->SetRightNode(pChild);
	
	pChild->SetParentNode(pParent);	

	//pGrandChild may be an external node
	CRedBlackTreeItem* pGrandChild = pChild->GetRightNode();	
	pGrandChild->SetParentNode(pItem);
	pItem->SetLeftNode(pGrandChild);
	
	pChild->SetRightNode(pItem);
	pItem->SetParentNode(pChild); 
}

//remove item at the given position
template < typename T_KEY, typename T_VALUE >
void CRedBlackTree< T_KEY, T_VALUE >::RemoveAt(CRedBlackTreeItem* pItem)
{
	if (pItem->IsInnerNode())
	{
		CRedBlackTreeItem* pChild;			//child of pReplacement
		CRedBlackTreeItem* pReplacement;	//this node will be used instead of pItem
		pReplacement = pItem;
		
		if (pItem->GetRightNode()->IsExternalNode())
		{
			pChild = pItem->GetLeftNode();
			delete pItem->GetRightNode();			//remove external node
		}
		else
		{
			if (pItem->GetLeftNode()->IsInnerNode())
			{
				//find successor of pItem
				pReplacement = pReplacement->GetRightNode();
				while (pReplacement->GetLeftNode()->IsInnerNode())
				{
					pReplacement = pReplacement->GetLeftNode();
				}
			}

			pChild = pReplacement->GetRightNode();
			delete pReplacement->GetLeftNode();		//remove external node
		}

		//pReplacement will go instead of pItem (unless pReplacement == pItem)		
		CRedBlackTreeItem* pReplParent = pReplacement->GetParentNode();
		if (pReplacement == pReplParent->GetLeftNode())
			pReplParent->SetLeftNode(pChild);
		else
			pReplParent->SetRightNode(pChild);
		pChild->SetParentNode(pReplParent);	
		
		if (pReplacement == pItem)
		{			
			if (pReplacement->IsBlackNode())
				FixupRemove(pChild);			
		}
		else
		{
			//move pReplacement to pItem position
			CRedBlackTreeItem* pTmp = pItem->GetLeftNode();
			pReplacement->SetLeftNode(pTmp);
			pTmp->SetParentNode(pReplacement);

			pTmp = pItem->GetRightNode();
			pReplacement->SetRightNode(pTmp);
			pTmp->SetParentNode(pReplacement);
			
			CRedBlackTreeItem* pParent = pItem->GetParentNode();
			if (pParent->GetLeftNode() == pItem)
				pParent->SetLeftNode(pReplacement);
			else
				pParent->SetRightNode(pReplacement);
			pReplacement->SetParentNode(pParent);

			bool bDoubleBlack = pReplacement->IsBlackNode();
			pReplacement->SetNodeColour(pItem->IsRedNode());
			
			if (bDoubleBlack)
				FixupRemove(pChild);
		}
		
		if (pItem->IsInnerNode())
		{
			//remove item from linked list			
			if (pItem->GetPrevNode() != NULL)
				pItem->GetPrevNode()->SetNextNode(pItem->GetNextNode());
			if (pItem->GetNextNode() != NULL)
				pItem->GetNextNode()->SetPrevNode(pItem->GetPrevNode());
		}
		
		delete pItem;
		m_nCount--;
	} //end if pItem != NULL

#ifdef _DEBUG
	AssertValid(m_pRoot->GetLeftNode());
#endif
}


//remove item at given position
template < typename T_KEY, typename T_VALUE >
inline void CRedBlackTree< T_KEY, T_VALUE >::RemoveAt(POSITION pos)
{	
	RemoveAt((CRedBlackTreeItem*)pos);
}

//remove the specified item
template < typename T_KEY, typename T_VALUE >
inline void CRedBlackTree< T_KEY, T_VALUE >::RemoveKey(T_KEY_ARG_IN key)
{	
	RemoveAt(Find(key));
}

//this routine rebalances the tree (if necessary) after removal of an item
template < typename T_KEY, typename T_VALUE >
void CRedBlackTree< T_KEY, T_VALUE >::FixupRemove(CRedBlackTreeItem* pItem)
{	
	CRedBlackTreeItem* pRoot = m_pRoot->GetLeftNode();

	//while pItem is black but it is not the root node	
	while (pItem->IsBlackNode() && pItem != pRoot)
	{
		CRedBlackTreeItem* pParent = pItem->GetParentNode();
		if (pItem == pParent->GetLeftNode())
		{
			CRedBlackTreeItem* pSibling = pParent->GetRightNode();
			if (pSibling->IsRedNode())
			{
				pSibling->SetNodeBlack();
				pParent->SetNodeRed();
				LeftRotate(pParent);

				//take new sibling
				pSibling = pParent->GetRightNode();
			}

			if (pSibling->GetLeftNode()->IsBlackNode() && pSibling->GetRightNode()->IsBlackNode())
			{
				//if both children are black, set pSibling red
				pSibling->SetNodeRed();
				pItem = pParent;
			}
			else
			{
				//i.e. at least one ancestor of pSibling is red (and thus exists) 
				CRedBlackTreeItem* pChild = pSibling->GetRightNode();
				if (pChild->IsBlackNode())
				{						
					pSibling->GetLeftNode()->SetNodeBlack();
					pSibling->SetNodeRed();

					RightRotate(pSibling);

					//take new sibling
					pSibling = pParent->GetRightNode();
				}

				pSibling->SetNodeColour(pParent->IsRedNode());
				pParent->SetNodeBlack();

				pSibling->GetRightNode()->SetNodeBlack();

				LeftRotate(pParent);
				pItem = pRoot;
			}
		}
		else		
		{
			CRedBlackTreeItem* pSibling = pParent->GetLeftNode();
			if (pSibling->IsRedNode())
			{
				pSibling->SetNodeBlack();
				pParent->SetNodeRed();
				RightRotate(pParent);

				//take new sibling
				pSibling = pParent->GetLeftNode();
			}


			if (pSibling->GetLeftNode()->IsBlackNode() && pSibling->GetRightNode()->IsBlackNode())
			{
				//if both children are black, set pSibling red
				pSibling->SetNodeRed();
				pItem = pParent;
			}
			else
			{
				//i.e. at least one ancestor of pSibling is red (and thus exists) 
				CRedBlackTreeItem* pChild = pSibling->GetLeftNode();
				if (pChild->IsBlackNode())
				{
					pSibling->GetRightNode()->SetNodeBlack();

					pSibling->SetNodeRed();
					LeftRotate(pSibling);

					//take new sibling
					pSibling = pParent->GetLeftNode();
				}

				pSibling->GetLeftNode()->SetNodeBlack();
				pSibling->SetNodeColour(pParent->IsRedNode());
				pParent->SetNodeBlack();					

				RightRotate(pParent);
				pItem = pRoot;
			}			
		}
	} //end while

	pItem->SetNodeBlack();
}


//returns key for the given position, valid only for inner nodes
template < typename T_KEY, typename T_VALUE >
inline void CRedBlackTree< T_KEY, T_VALUE >::GetKeyAt(POSITION pos, T_KEY_ARG_OUT key)
{
	CRedBlackTreeItem* pItem = (CRedBlackTreeItem*)pos;
	_ASSERT(pItem != NULL && pItem->IsInnerNode());

	key = pItem->GetKey();
}
//returns key for the given position, valid only for inner nodes
template < typename T_KEY, typename T_VALUE >
inline typename CRedBlackTree< T_KEY, T_VALUE >::T_KEY_ARG_POUT CRedBlackTree< T_KEY, T_VALUE >::GetPKeyAt(POSITION pos)
{
	CRedBlackTreeItem* pItem = (CRedBlackTreeItem*)pos;
	_ASSERT(pItem != NULL && pItem->IsInnerNode());

	return pItem->GetPKey();
}

//returns value for the given position, valid only for inner nodes
template < typename T_KEY, typename T_VALUE >
inline void CRedBlackTree< T_KEY, T_VALUE >::GetValueAt(POSITION pos, T_VAL_ARG_OUT value)
{
	CRedBlackTreeItem* pItem = (CRedBlackTreeItem*)pos;
	_ASSERT(pItem != NULL && pItem->IsInnerNode());
	
	value = pItem->GetValue();
}

//returns value for the given position, valid only for inner nodes
template < typename T_KEY, typename T_VALUE >
inline typename CRedBlackTree< T_KEY, T_VALUE >::T_VAL_ARG_POUT CRedBlackTree< T_KEY, T_VALUE >::GetPValueAt(POSITION pos)
{
	CRedBlackTreeItem* pItem = (CRedBlackTreeItem*)pos;
	_ASSERT(pItem != NULL && pItem->IsInnerNode());
	
	return pItem->GetPValue();
}

//returns value for the given position, valid only for inner nodes
template < typename T_KEY, typename T_VALUE >
inline void CRedBlackTree< T_KEY, T_VALUE >::GetAssocAt(POSITION pos, T_KEY_ARG_OUT key, T_VAL_ARG_OUT value)
{
	CRedBlackTreeItem* pItem = (CRedBlackTreeItem*)pos;
	_ASSERT(pItem != NULL && pItem->IsInnerNode());

	key = pItem->GetKey();
	value = pItem->GetValue();
}

//returns value for the given position, valid only for inner nodes
template < typename T_KEY, typename T_VALUE >
inline void CRedBlackTree< T_KEY, T_VALUE >::GetPAssocAt(POSITION pos, T_KEY_ARG_POUT key, T_VAL_ARG_POUT value)
{
	CRedBlackTreeItem* pItem = (CRedBlackTreeItem*)pos;
	_ASSERT(pItem != NULL && pItem->IsInnerNode());

	key = pItem->GetPKey();
	value = pItem->GetPValue();
}

//returns wheter the node denoted by the given pos is a leaf or not
template < typename T_KEY, typename T_VALUE >
inline bool CRedBlackTree< T_KEY, T_VALUE >::IsInnerNode(POSITION pos)
{
	_ASSERT(pos != NULL);
	
	return ((CRedBlackTreeItem*)pos)->IsInnerNode();
}

//finds the first node in list, pos may be NULL
template < typename T_KEY, typename T_VALUE >
POSITION CRedBlackTree< T_KEY, T_VALUE >::GetFirstNode(POSITION pos)
{
	CRedBlackTreeItem* pItem = (CRedBlackTreeItem*)pos;
	if (pItem == NULL)
	{
		pItem = m_pRoot->GetLeftNode();
		if (pItem->IsExternalNode())
			return NULL;

		CRedBlackTreeItem* pTmp;
		while ((pTmp = pItem->GetLeftNode())->IsInnerNode())
			pItem = pTmp;
	}
	else
	{
		CRedBlackTreeItem* pTmp;
		while ((pTmp = pItem->GetPrevNode()) != NULL)
			pItem = pTmp;
	}
	
	return (POSITION)pItem;
}

//finds the last node in list, pos may be NULL
template < typename T_KEY, typename T_VALUE >
POSITION CRedBlackTree< T_KEY, T_VALUE >::GetLastNode(POSITION pos)
{
	CRedBlackTreeItem* pItem = (CRedBlackTreeItem*)pos;
	if (pItem == NULL)
	{
		pItem = m_pRoot->GetLeftNode();
		if (pItem->IsExternalNode)
			return NULL;

		CRedBlackTreeItem* pTmp;
		while ((pTmp = pItem->GetRightNode())->IsInnerNode())
			pItem = pTmp;
	}
	else
	{
		CRedBlackTreeItem* pTmp;
		while ((pTmp = pItem->GetNextNode()) != NULL)
			pItem = pTmp;
	}
	
	return (POSITION)pItem;
}

//finds the first node in list, pos may not be NULL
template < typename T_KEY, typename T_VALUE >
inline POSITION CRedBlackTree< T_KEY, T_VALUE >::GetNextNode(POSITION pos)
{
	CRedBlackTreeItem* pItem = (CRedBlackTreeItem*)pos;
	_ASSERT(pItem != NULL && pItem->IsInnerNode());
	return (POSITION)pItem->GetNextNode();
}

//finds the last node in list, pos may not be NULL
template < typename T_KEY, typename T_VALUE >
inline POSITION CRedBlackTree< T_KEY, T_VALUE >::GetPreviousNode(POSITION pos)
{
	CRedBlackTreeItem* pItem = (CRedBlackTreeItem*)pos;
	_ASSERT(pItem != NULL && pItem->IsInnerNode());
	return (POSITION)pItem->GetPrevNode();
}

//finds the left node of pos, pos may not be NULL
template < typename T_KEY, typename T_VALUE >
inline POSITION CRedBlackTree< T_KEY, T_VALUE >::GetLeftNode(POSITION pos)
{
	_ASSERT(pos != NULL);
	return (POSITION)(((CRedBlackTreeItem*)pos)->GetLeftNode());
}

//finds the right node of pos, pos may not be NULL
template < typename T_KEY, typename T_VALUE >
inline POSITION CRedBlackTree< T_KEY, T_VALUE >::GetRightNode(POSITION pos)
{
	_ASSERT(pos != NULL);
	return (POSITION)(((CRedBlackTreeItem*)pos)->GetRightNode());
}

//finds the parent node of pos, pos may not be NULL
template < typename T_KEY, typename T_VALUE >
inline POSITION CRedBlackTree< T_KEY, T_VALUE >::GetParentNode(POSITION pos)
{
	_ASSERT(pos != NULL);
	return (POSITION)(((CRedBlackTreeItem*)pos)->GetParentNode());
}

//finds the root, all inner nodes are in the left subtree
template < typename T_KEY, typename T_VALUE >
inline POSITION CRedBlackTree< T_KEY, T_VALUE >::GetRootNode()
{
	return (POSITION)m_pRoot;
}

//finds the root of user data, returns NULL if no such node exists
template < typename T_KEY, typename T_VALUE >
inline POSITION CRedBlackTree< T_KEY, T_VALUE >::GetRootInnerNode()
{
	CRedBlackTreeItem* pRet = m_pRoot->GetLeftNode();
	if (pRet->IsInnerNode())
		return (POSITION)pRet;
	else
		return NULL;
}

//returns true if there is no data in the tree, i.e., there is no inner node
template < typename T_KEY, typename T_VALUE >
inline bool CRedBlackTree< T_KEY, T_VALUE >::IsEmpty()
{
	return m_pRoot->GetLeftNode()->IsExternalNode() && 
		m_pRoot->GetRightNode()->IsExternalNode();
}

#ifdef _DEBUG
//this routine is used to check tree
template < typename T_KEY, typename T_VALUE >
int CRedBlackTree< T_KEY, T_VALUE >::AssertValid(CRedBlackTreeItem* pRoot)
{
	if (pRoot == NULL)
		return 0;
	else
	{
		int nLeftBlacks = 0;
		if (pRoot->GetLeftNode() != NULL)
		{
			if (pRoot->IsRedNode())
				_ASSERT(!pRoot->GetLeftNode()->IsRedNode());

			nLeftBlacks = AssertValid(pRoot->GetLeftNode());
		}

		int nRightBlacks = 0;
		if (pRoot->GetRightNode() != NULL)
		{
			if (pRoot->IsRedNode())
				_ASSERT(!pRoot->GetRightNode()->IsRedNode());

			nRightBlacks = AssertValid(pRoot->GetRightNode());
		}

		_ASSERT(nLeftBlacks == nRightBlacks);
		if (pRoot->IsRedNode())
			return nLeftBlacks;
		else
			return nLeftBlacks + 1;

		if (pRoot == m_pRoot->GetLeftNode())
			_ASSERTE(!pRoot->IsRedNode());
	}
}
#endif