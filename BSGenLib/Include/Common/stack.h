#pragma once

#include "Array.h"

#if !defined(__BSGEN_PUREWIN)
template < typename TYPE, class ARG_TYPE = CElementTraits< TYPE > >
#else
template < typename TYPE, class ARG_TYPE = const TYPE& >
#endif
class CStack 
{
public:
#if !defined(__BSGEN_PUREWIN)
	typedef typename ARG_TYPE::INARGTYPE INARGTYPE;
	typedef typename ARG_TYPE::OUTARGTYPE OUTARGTYPE;
#else
	typedef TYPE INARGTYPE;
	typedef TYPE& OUTARGTYPE;
#endif

protected:
	CArray<TYPE, ARG_TYPE> m_Array;

public:
	CStack()
	{
	}

public:
	inline void Push(INARGTYPE value)
	{
		m_Array.Add(value);
	}	

	inline void Peek(OUTARGTYPE value) {
		value = m_Array.GetAt(m_Array.GetCount() - 1);
	}

	inline TYPE Peek() {
		return m_Array.GetAt(m_Array.GetCount() - 1);
	}

	inline void Pop(OUTARGTYPE value)
	{
		value = Peek();		
		m_Array.RemoveAt(m_Array.GetCount() - 1);
	}

	inline TYPE Pop()
	{
		TYPE retValue = m_Array.GetAt(m_Array.GetCount() - 1);		
		m_Array.RemoveAt(m_Array.GetCount() - 1);
		return retValue;
	}
	
	inline bool IsEmpty() {
		return m_Array.IsEmpty();
	}

	inline void Empty() {
		m_Array.RemoveAll();
	}
};

#if !defined(__BSGEN_PUREWIN)
template < typename TYPE, class ARG_TYPE = CElementTraits< TYPE > >
#else
template < typename TYPE, class ARG_TYPE = const TYPE& >
#endif
class CFixedStack
{
#if !defined(__BSGEN_PUREWIN)
public:
	typedef typename ARG_TYPE::INARGTYPE INARGTYPE;
	typedef typename ARG_TYPE::OUTARGTYPE OUTARGTYPE;
#else
	typedef TYPE INARGTYPE;
	typedef ARG_TYPE OUTARGTYPE;
#endif

protected:
	TYPE* m_Stack;
	int m_nSize;
	int m_SP;
public:
	CFixedStack(int nSize) {
		m_Stack = new TYPE[nSize];
		m_nSize = nSize;
		m_SP = 0;
	}

	~CFixedStack() {
		delete[] m_Stack;
	}

public:
	inline void Push(INARGTYPE value) {
		m_Stack[m_SP] = value;
		m_SP++;
	}

	inline void Peek(OUTARGTYPE value) {
		value = m_Stack[m_SP - 1];
	}

	inline TYPE Peek() {
		return m_Stack[m_SP - 1];
	}

	inline void Pop(OUTARGTYPE value) {		
		value = m_Stack[--m_SP];
	}

	inline TYPE Pop() {
		return m_Stack[--m_SP];
	}
	
	inline bool IsEmpty() {
		return m_SP == 0;
	}

	inline bool IsFull() {
		return m_SP == m_nSize;
	}

	inline void Empty() {
		m_SP = 0;
	}
};
