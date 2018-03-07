#pragma once

//abstract class for references
//for description of ATL_NO_VTABLE see:
//http://msdn.microsoft.com/msdnmag/issues/0300/c/
interface ATL_NO_VTABLE IRefObj
{
public:
	virtual ULONG AddRef() = 0;
	virtual ULONG Release() = 0;
};

//implementation of IRefObj
//typical use: 
//
//interface IMyIface : IRefObj { ... }
//interface IMyIface2 : IRefObj { ... }
//class CProvider : CRefObj<IMyIface>, IMyIface2 { ... }
//
//NOTE: if needed generally, can be used CRefObj<IRefObj>
template < class T_IFACE >
class ATL_NO_VTABLE CRefObj : public virtual T_IFACE
{
protected:
	ULONG m_nRefs;

protected:
	CRefObj() : m_nRefs(1) {}

public:
	virtual ~CRefObj() {}

public:
	virtual ULONG AddRef() {
		return ++m_nRefs;
	}

	virtual ULONG Release() {
		ULONG nRet = --m_nRefs;
		if (nRet == 0)
			delete this;

		return nRet;
	}	
};
