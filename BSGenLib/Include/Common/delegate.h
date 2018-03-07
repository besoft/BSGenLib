
#pragma once



class RMCMNAPI Delegate
{
//constructors
protected:
	Delegate() {
	}

	virtual ~Delegate() {
	}
};










//DELEG_TEMPL_PARAMETRY -- class T1, class T2, ...
//DELEG_TEMPL_PAR_ARGS -- T1, T2, ...
//DELEG_PARAMETRY -- T1 identif1, T2 identif2, ...
//DELEG_PAR_ARGS -- identif1, identif2, ...

	template <class PAR1>
	class _DelegateBase1 : public Delegate
	{
	public:
		virtual void Invoke(PAR1 par1) = 0;
		virtual _DelegateBase1* Clone() = 0;
		virtual bool Equals(_DelegateBase1* pDelegate) = 0;
	};

	
	template <class T, class PAR1>
	class _DelegateImpl1 : public _DelegateBase1<PAR1>
	{
		//attributes
	protected:
		typedef void (T::*MBFUNC)(PAR1 par1);
		typedef void (*STFUNC)(PAR1 par1);
		
		T* _target;
		MBFUNC _method;
		STFUNC _function;

	private:
		_DelegateImpl1()
		{
		}

	public:
		_DelegateImpl1(T* pObj, MBFUNC pMbFunc)
		{
			_target = pObj;
			_method = pMbFunc;
			_function = NULL;
		}

		_DelegateImpl1(STFUNC pStFunc)
		{
			_target = NULL;
			_method = NULL;
			_function = pStFunc;
		}

		virtual void Invoke(PAR1 par1)
		{				
			if (_method != NULL)			
				(this->_target->*this->_method)(par1);			
			else // if (_function != NULL)			
				_function(par1);
		}

		virtual _DelegateBase1* Clone() 
		{
			if (_method != NULL)
				return new _DelegateImpl1(_target, _method);
			else
				return new _DelegateImpl1(_function);
		}

		virtual bool Equals(_DelegateBase1* pDelegate) {
			_DelegateImpl1* pDel = (_DelegateImpl1*)pDelegate;
			return _method == pDel->_method && _target == pDel->_target &&
				_function == pDel->_function;
		}
	};

	template <class PAR1>
	class _EventHandlerImpl1
	{
	public:
		typedef _DelegateBase1<PAR1> CDelegate;
		typedef CList<_DelegateBase1<PAR1>*> CDelegateList;
		
	protected:
		 CDelegateList m_Delegate;	

	public:
		_EventHandlerImpl1() {
		}

		~_EventHandlerImpl1() {
			RemoveAll();			
		}
		
		void AddHandler(CDelegate* pDelegate) {
			m_Delegate.AddHead(pDelegate);
		}

		void AddHandler(CDelegateList* pDelegate) {
			POSITION pos = m_Delegate.GetHeadPosition();
			while (pos != NULL) {
				m_Delegate.AddHead(m_Delegate.GetNext(pos));
			}
		}

		void RemoveHandler(CDelegate* pDelegate) 
		{
			POSITION pos = m_Delegate.GetHeadPosition();
			while (pos != NULL)
			{
				POSITION posLast = pos;
				CDelegate* pDeleg = m_Delegate.GetNext(pos);

				if (pDeleg->Equals(pDelegate)) {
					m_Delegate.RemoveAt(pos);
					delete pDeleg;
				}
			}
			delete pDelegate;
		}

		void RemoveHandler(CDelegateList* pDelegate) {
			POSITION pos = m_Delegate.GetHeadPosition();
			while (pos != NULL) {
				RemoveHandler(m_Delegate.GetNext(pos));
			}			
		}

		void RemoveAll()
		{
			POSITION pos = m_Delegate.GetHeadPosition();
			while (pos != NULL) {
				delete m_Delegate.GetNext(pos);
			}
		}

		void Invoke(PAR1 par1)
		{
			POSITION pos = m_Delegate.GetHeadPosition();
			while (pos != NULL) {				
				m_Delegate.GetNext(pos)->Invoke(par1);
			}
		}

		inline _EventHandlerImpl1& operator += (CDelegate* pDelegate)
		{
			AddHandler(pDelegate);
			return *this;
		}

		inline _EventHandlerImpl1& operator += (CDelegateList* pDelegateList)
		{
			AddHandler(pDelegateList);
			return *this;
		}

		inline _EventHandlerImpl1& operator -= (CDelegate* pDelegate)
		{
			RemoveHandler(pDelegate);
			return *this;
		}

		inline _EventHandlerImpl1& operator -= (CDelegateList* pDelegateList)
		{
			RemoveHandler(pDelegateList);
			return *this;
		}

		inline _EventHandlerImpl1& operator = (CDelegate* pDelegate)
		{
			RemoveAll();
			AddHandler(pDelegate);
			return *this;
		}

		inline _EventHandlerImpl1& operator = (CDelegateList* pDelegateList)
		{
			RemoveAll();
			AddHandler(pDelegateList);
			return *this;
		}

		inline void operator () (PAR1 par1)
		{
			if (m_Delegate.GetHeadPosition() != NULL)
				Invoke(par1);
		}

		inline bool operator == (int Null) {			
			return m_Delegate.GetHeadPosition() == NULL;
		}

		inline bool operator != (int Null) {			
			return m_Delegate.GetHeadPosition() != NULL;
		}

		CDelegateList* GetInvocationList() 
		{
			CDelegateList* pRet = new CDelegateList();
			POSITION pos = m_Delegate.GetHeadPosition();
			while (pos != NULL) {				
				pRet->Add(m_Delegate.GetNext(pos)->Clone());
			}

			return pRet;
		}		
	};




















//DELEG_TEMPL_PARAMETRY -- class T1, class T2, ...
//DELEG_TEMPL_PAR_ARGS -- T1, T2, ...
//DELEG_PARAMETRY -- T1 identif1, T2 identif2, ...
//DELEG_PAR_ARGS -- identif1, identif2, ...

	template <class PAR1, class PAR2>
	class _DelegateBase2 : public Delegate
	{
	public:
		virtual void Invoke(PAR1 par1, PAR2 par2) = 0;
		virtual _DelegateBase2* Clone() = 0;
		virtual bool Equals(_DelegateBase2* pDelegate) = 0;
	};

	
	template <class T, class PAR1, class PAR2>
	class _DelegateImpl2 : public _DelegateBase2<PAR1, PAR2>
	{
		//attributes
	protected:
		typedef void (T::*MBFUNC)(PAR1 par1, PAR2 par2);
		typedef void (*STFUNC)(PAR1 par1, PAR2 par2);
		
		T* _target;
		MBFUNC _method;
		STFUNC _function;

	private:
		_DelegateImpl2()
		{
		}

	public:
		_DelegateImpl2(T* pObj, MBFUNC pMbFunc)
		{
			_target = pObj;
			_method = pMbFunc;
			_function = NULL;
		}

		_DelegateImpl2(STFUNC pStFunc)
		{
			_target = NULL;
			_method = NULL;
			_function = pStFunc;
		}

		virtual void Invoke(PAR1 par1, PAR2 par2)
		{				
			if (_method != NULL)			
				(this->_target->*this->_method)(par1, par2);			
			else // if (_function != NULL)			
				_function(par1, par2);
		}

		virtual _DelegateBase2* Clone() 
		{
			if (_method != NULL)
				return new _DelegateImpl2(_target, _method);
			else
				return new _DelegateImpl2(_function);
		}

		virtual bool Equals(_DelegateBase2* pDelegate) {
			_DelegateImpl2* pDel = (_DelegateImpl2*)pDelegate;
			return _method == pDel->_method && _target == pDel->_target &&
				_function == pDel->_function;
		}
	};

	template <class PAR1, class PAR2>
	class _EventHandlerImpl2
	{
	public:
		typedef _DelegateBase2<PAR1, PAR2> CDelegate;
		typedef CList<_DelegateBase2<PAR1, PAR2>*> CDelegateList;
		
	protected:
		 CDelegateList m_Delegate;	

	public:
		_EventHandlerImpl2() {
		}

		~_EventHandlerImpl2() {
			RemoveAll();			
		}
		
		void AddHandler(CDelegate* pDelegate) {
			m_Delegate.AddHead(pDelegate);
		}

		void AddHandler(CDelegateList* pDelegate) {
			POSITION pos = m_Delegate.GetHeadPosition();
			while (pos != NULL) {
				m_Delegate.AddHead(m_Delegate.GetNext(pos));
			}
		}

		void RemoveHandler(CDelegate* pDelegate) 
		{
			POSITION pos = m_Delegate.GetHeadPosition();
			while (pos != NULL)
			{
				POSITION posLast = pos;
				CDelegate* pDeleg = m_Delegate.GetNext(pos);

				if (pDeleg->Equals(pDelegate)) {
					m_Delegate.RemoveAt(pos);
					delete pDeleg;
				}
			}
			delete pDelegate;
		}

		void RemoveHandler(CDelegateList* pDelegate) {
			POSITION pos = m_Delegate.GetHeadPosition();
			while (pos != NULL) {
				RemoveHandler(m_Delegate.GetNext(pos));
			}			
		}

		void RemoveAll()
		{
			POSITION pos = m_Delegate.GetHeadPosition();
			while (pos != NULL) {
				delete m_Delegate.GetNext(pos);
			}
		}

		void Invoke(PAR1 par1, PAR2 par2)
		{
			POSITION pos = m_Delegate.GetHeadPosition();
			while (pos != NULL) {				
				m_Delegate.GetNext(pos)->Invoke(par1, par2);
			}
		}

		inline _EventHandlerImpl2& operator += (CDelegate* pDelegate)
		{
			AddHandler(pDelegate);
			return *this;
		}

		inline _EventHandlerImpl2& operator += (CDelegateList* pDelegateList)
		{
			AddHandler(pDelegateList);
			return *this;
		}

		inline _EventHandlerImpl2& operator -= (CDelegate* pDelegate)
		{
			RemoveHandler(pDelegate);
			return *this;
		}

		inline _EventHandlerImpl2& operator -= (CDelegateList* pDelegateList)
		{
			RemoveHandler(pDelegateList);
			return *this;
		}

		inline _EventHandlerImpl2& operator = (CDelegate* pDelegate)
		{
			RemoveAll();
			AddHandler(pDelegate);
			return *this;
		}

		inline _EventHandlerImpl2& operator = (CDelegateList* pDelegateList)
		{
			RemoveAll();
			AddHandler(pDelegateList);
			return *this;
		}

		inline void operator () (PAR1 par1, PAR2 par2)
		{
			if (m_Delegate.GetHeadPosition() != NULL)
				Invoke(par1, par2);
		}

		inline bool operator == (int Null) {			
			return m_Delegate.GetHeadPosition() == NULL;
		}

		inline bool operator != (int Null) {			
			return m_Delegate.GetHeadPosition() != NULL;
		}

		CDelegateList* GetInvocationList() 
		{
			CDelegateList* pRet = new CDelegateList();
			POSITION pos = m_Delegate.GetHeadPosition();
			while (pos != NULL) {				
				pRet->Add(m_Delegate.GetNext(pos)->Clone());
			}

			return pRet;
		}		
	};




















//DELEG_TEMPL_PARAMETRY -- class T1, class T2, ...
//DELEG_TEMPL_PAR_ARGS -- T1, T2, ...
//DELEG_PARAMETRY -- T1 identif1, T2 identif2, ...
//DELEG_PAR_ARGS -- identif1, identif2, ...

	template <class PAR1, class PAR2, class PAR3>
	class _DelegateBase3 : public Delegate
	{
	public:
		virtual void Invoke(PAR1 par1, PAR2 par2, PAR3 par3) = 0;
		virtual _DelegateBase3* Clone() = 0;
		virtual bool Equals(_DelegateBase3* pDelegate) = 0;
	};

	
	template <class T, class PAR1, class PAR2, class PAR3>
	class _DelegateImpl3 : public _DelegateBase3<PAR1, PAR2, PAR3>
	{
		//attributes
	protected:
		typedef void (T::*MBFUNC)(PAR1 par1, PAR2 par2, PAR3 par3);
		typedef void (*STFUNC)(PAR1 par1, PAR2 par2, PAR3 par3);
		
		T* _target;
		MBFUNC _method;
		STFUNC _function;

	private:
		_DelegateImpl3()
		{
		}

	public:
		_DelegateImpl3(T* pObj, MBFUNC pMbFunc)
		{
			_target = pObj;
			_method = pMbFunc;
			_function = NULL;
		}

		_DelegateImpl3(STFUNC pStFunc)
		{
			_target = NULL;
			_method = NULL;
			_function = pStFunc;
		}

		virtual void Invoke(PAR1 par1, PAR2 par2, PAR3 par3)
		{				
			if (_method != NULL)			
				(this->_target->*this->_method)(par1, par2, par3);			
			else // if (_function != NULL)			
				_function(par1, par2, par3);
		}

		virtual _DelegateBase3* Clone() 
		{
			if (_method != NULL)
				return new _DelegateImpl3(_target, _method);
			else
				return new _DelegateImpl3(_function);
		}

		virtual bool Equals(_DelegateBase3* pDelegate) {
			_DelegateImpl3* pDel = (_DelegateImpl3*)pDelegate;
			return _method == pDel->_method && _target == pDel->_target &&
				_function == pDel->_function;
		}
	};

	template <class PAR1, class PAR2, class PAR3>
	class _EventHandlerImpl3
	{
	public:
		typedef _DelegateBase3<PAR1, PAR2, PAR3> CDelegate;
		typedef CList<_DelegateBase3<PAR1, PAR2, PAR3>*> CDelegateList;
		
	protected:
		 CDelegateList m_Delegate;	

	public:
		_EventHandlerImpl3() {
		}

		~_EventHandlerImpl3() {
			RemoveAll();			
		}
		
		void AddHandler(CDelegate* pDelegate) {
			m_Delegate.AddHead(pDelegate);
		}

		void AddHandler(CDelegateList* pDelegate) {
			POSITION pos = m_Delegate.GetHeadPosition();
			while (pos != NULL) {
				m_Delegate.AddHead(m_Delegate.GetNext(pos));
			}
		}

		void RemoveHandler(CDelegate* pDelegate) 
		{
			POSITION pos = m_Delegate.GetHeadPosition();
			while (pos != NULL)
			{
				POSITION posLast = pos;
				CDelegate* pDeleg = m_Delegate.GetNext(pos);

				if (pDeleg->Equals(pDelegate)) {
					m_Delegate.RemoveAt(pos);
					delete pDeleg;
				}
			}
			delete pDelegate;
		}

		void RemoveHandler(CDelegateList* pDelegate) {
			POSITION pos = m_Delegate.GetHeadPosition();
			while (pos != NULL) {
				RemoveHandler(m_Delegate.GetNext(pos));
			}			
		}

		void RemoveAll()
		{
			POSITION pos = m_Delegate.GetHeadPosition();
			while (pos != NULL) {
				delete m_Delegate.GetNext(pos);
			}
		}

		void Invoke(PAR1 par1, PAR2 par2, PAR3 par3)
		{
			POSITION pos = m_Delegate.GetHeadPosition();
			while (pos != NULL) {				
				m_Delegate.GetNext(pos)->Invoke(par1, par2, par3);
			}
		}

		inline _EventHandlerImpl3& operator += (CDelegate* pDelegate)
		{
			AddHandler(pDelegate);
			return *this;
		}

		inline _EventHandlerImpl3& operator += (CDelegateList* pDelegateList)
		{
			AddHandler(pDelegateList);
			return *this;
		}

		inline _EventHandlerImpl3& operator -= (CDelegate* pDelegate)
		{
			RemoveHandler(pDelegate);
			return *this;
		}

		inline _EventHandlerImpl3& operator -= (CDelegateList* pDelegateList)
		{
			RemoveHandler(pDelegateList);
			return *this;
		}

		inline _EventHandlerImpl3& operator = (CDelegate* pDelegate)
		{
			RemoveAll();
			AddHandler(pDelegate);
			return *this;
		}

		inline _EventHandlerImpl3& operator = (CDelegateList* pDelegateList)
		{
			RemoveAll();
			AddHandler(pDelegateList);
			return *this;
		}

		inline void operator () (PAR1 par1, PAR2 par2, PAR3 par3)
		{
			if (m_Delegate.GetHeadPosition() != NULL)
				Invoke(par1, par2, par3);
		}

		inline bool operator == (int Null) {			
			return m_Delegate.GetHeadPosition() == NULL;
		}

		inline bool operator != (int Null) {			
			return m_Delegate.GetHeadPosition() != NULL;
		}

		CDelegateList* GetInvocationList() 
		{
			CDelegateList* pRet = new CDelegateList();
			POSITION pos = m_Delegate.GetHeadPosition();
			while (pos != NULL) {				
				pRet->Add(m_Delegate.GetNext(pos)->Clone());
			}

			return pRet;
		}		
	};




















//DELEG_TEMPL_PARAMETRY -- class T1, class T2, ...
//DELEG_TEMPL_PAR_ARGS -- T1, T2, ...
//DELEG_PARAMETRY -- T1 identif1, T2 identif2, ...
//DELEG_PAR_ARGS -- identif1, identif2, ...

	template <class PAR1, class PAR2, class PAR3, class PAR4>
	class _DelegateBase4 : public Delegate
	{
	public:
		virtual void Invoke(PAR1 par1, PAR2 par2, PAR3 par3, PAR4 par4) = 0;
		virtual _DelegateBase4* Clone() = 0;
		virtual bool Equals(_DelegateBase4* pDelegate) = 0;
	};

	
	template <class T, class PAR1, class PAR2, class PAR3, class PAR4>
	class _DelegateImpl4 : public _DelegateBase4<PAR1, PAR2, PAR3, PAR4>
	{
		//attributes
	protected:
		typedef void (T::*MBFUNC)(PAR1 par1, PAR2 par2, PAR3 par3, PAR4 par4);
		typedef void (*STFUNC)(PAR1 par1, PAR2 par2, PAR3 par3, PAR4 par4);
		
		T* _target;
		MBFUNC _method;
		STFUNC _function;

	private:
		_DelegateImpl4()
		{
		}

	public:
		_DelegateImpl4(T* pObj, MBFUNC pMbFunc)
		{
			_target = pObj;
			_method = pMbFunc;
			_function = NULL;
		}

		_DelegateImpl4(STFUNC pStFunc)
		{
			_target = NULL;
			_method = NULL;
			_function = pStFunc;
		}

		virtual void Invoke(PAR1 par1, PAR2 par2, PAR3 par3, PAR4 par4)
		{				
			if (_method != NULL)			
				(this->_target->*this->_method)(par1, par2, par3, par4);			
			else // if (_function != NULL)			
				_function(par1, par2, par3, par4);
		}

		virtual _DelegateBase4* Clone() 
		{
			if (_method != NULL)
				return new _DelegateImpl4(_target, _method);
			else
				return new _DelegateImpl4(_function);
		}

		virtual bool Equals(_DelegateBase4* pDelegate) {
			_DelegateImpl4* pDel = (_DelegateImpl4*)pDelegate;
			return _method == pDel->_method && _target == pDel->_target &&
				_function == pDel->_function;
		}
	};

	template <class PAR1, class PAR2, class PAR3, class PAR4>
	class _EventHandlerImpl4
	{
	public:
		typedef _DelegateBase4<PAR1, PAR2, PAR3, PAR4> CDelegate;
		typedef CList<_DelegateBase4<PAR1, PAR2, PAR3, PAR4>*> CDelegateList;
		
	protected:
		 CDelegateList m_Delegate;	

	public:
		_EventHandlerImpl4() {
		}

		~_EventHandlerImpl4() {
			RemoveAll();			
		}
		
		void AddHandler(CDelegate* pDelegate) {
			m_Delegate.AddHead(pDelegate);
		}

		void AddHandler(CDelegateList* pDelegate) {
			POSITION pos = m_Delegate.GetHeadPosition();
			while (pos != NULL) {
				m_Delegate.AddHead(m_Delegate.GetNext(pos));
			}
		}

		void RemoveHandler(CDelegate* pDelegate) 
		{
			POSITION pos = m_Delegate.GetHeadPosition();
			while (pos != NULL)
			{
				POSITION posLast = pos;
				CDelegate* pDeleg = m_Delegate.GetNext(pos);

				if (pDeleg->Equals(pDelegate)) {
					m_Delegate.RemoveAt(pos);
					delete pDeleg;
				}
			}
			delete pDelegate;
		}

		void RemoveHandler(CDelegateList* pDelegate) {
			POSITION pos = m_Delegate.GetHeadPosition();
			while (pos != NULL) {
				RemoveHandler(m_Delegate.GetNext(pos));
			}			
		}

		void RemoveAll()
		{
			POSITION pos = m_Delegate.GetHeadPosition();
			while (pos != NULL) {
				delete m_Delegate.GetNext(pos);
			}
		}

		void Invoke(PAR1 par1, PAR2 par2, PAR3 par3, PAR4 par4)
		{
			POSITION pos = m_Delegate.GetHeadPosition();
			while (pos != NULL) {				
				m_Delegate.GetNext(pos)->Invoke(par1, par2, par3, par4);
			}
		}

		inline _EventHandlerImpl4& operator += (CDelegate* pDelegate)
		{
			AddHandler(pDelegate);
			return *this;
		}

		inline _EventHandlerImpl4& operator += (CDelegateList* pDelegateList)
		{
			AddHandler(pDelegateList);
			return *this;
		}

		inline _EventHandlerImpl4& operator -= (CDelegate* pDelegate)
		{
			RemoveHandler(pDelegate);
			return *this;
		}

		inline _EventHandlerImpl4& operator -= (CDelegateList* pDelegateList)
		{
			RemoveHandler(pDelegateList);
			return *this;
		}

		inline _EventHandlerImpl4& operator = (CDelegate* pDelegate)
		{
			RemoveAll();
			AddHandler(pDelegate);
			return *this;
		}

		inline _EventHandlerImpl4& operator = (CDelegateList* pDelegateList)
		{
			RemoveAll();
			AddHandler(pDelegateList);
			return *this;
		}

		inline void operator () (PAR1 par1, PAR2 par2, PAR3 par3, PAR4 par4)
		{
			if (m_Delegate.GetHeadPosition() != NULL)
				Invoke(par1, par2, par3, par4);
		}

		inline bool operator == (int Null) {			
			return m_Delegate.GetHeadPosition() == NULL;
		}

		inline bool operator != (int Null) {			
			return m_Delegate.GetHeadPosition() != NULL;
		}

		CDelegateList* GetInvocationList() 
		{
			CDelegateList* pRet = new CDelegateList();
			POSITION pos = m_Delegate.GetHeadPosition();
			while (pos != NULL) {				
				pRet->Add(m_Delegate.GetNext(pos)->Clone());
			}

			return pRet;
		}		
	};




















//DELEG_TEMPL_PARAMETRY -- class T1, class T2, ...
//DELEG_TEMPL_PAR_ARGS -- T1, T2, ...
//DELEG_PARAMETRY -- T1 identif1, T2 identif2, ...
//DELEG_PAR_ARGS -- identif1, identif2, ...

	template <class PAR1, class PAR2, class PAR3, class PAR4, class PAR5>
	class _DelegateBase5 : public Delegate
	{
	public:
		virtual void Invoke(PAR1 par1, PAR2 par2, PAR3 par3, PAR4 par4, PAR5 par5) = 0;
		virtual _DelegateBase5* Clone() = 0;
		virtual bool Equals(_DelegateBase5* pDelegate) = 0;
	};

	
	template <class T, class PAR1, class PAR2, class PAR3, class PAR4, class PAR5>
	class _DelegateImpl5 : public _DelegateBase5<PAR1, PAR2, PAR3, PAR4, PAR5>
	{
		//attributes
	protected:
		typedef void (T::*MBFUNC)(PAR1 par1, PAR2 par2, PAR3 par3, PAR4 par4, PAR5 par5);
		typedef void (*STFUNC)(PAR1 par1, PAR2 par2, PAR3 par3, PAR4 par4, PAR5 par5);
		
		T* _target;
		MBFUNC _method;
		STFUNC _function;

	private:
		_DelegateImpl5()
		{
		}

	public:
		_DelegateImpl5(T* pObj, MBFUNC pMbFunc)
		{
			_target = pObj;
			_method = pMbFunc;
			_function = NULL;
		}

		_DelegateImpl5(STFUNC pStFunc)
		{
			_target = NULL;
			_method = NULL;
			_function = pStFunc;
		}

		virtual void Invoke(PAR1 par1, PAR2 par2, PAR3 par3, PAR4 par4, PAR5 par5)
		{				
			if (_method != NULL)			
				(this->_target->*this->_method)(par1, par2, par3, par4, par5);			
			else // if (_function != NULL)			
				_function(par1, par2, par3, par4, par5);
		}

		virtual _DelegateBase5* Clone() 
		{
			if (_method != NULL)
				return new _DelegateImpl5(_target, _method);
			else
				return new _DelegateImpl5(_function);
		}

		virtual bool Equals(_DelegateBase5* pDelegate) {
			_DelegateImpl5* pDel = (_DelegateImpl5*)pDelegate;
			return _method == pDel->_method && _target == pDel->_target &&
				_function == pDel->_function;
		}
	};

	template <class PAR1, class PAR2, class PAR3, class PAR4, class PAR5>
	class _EventHandlerImpl5
	{
	public:
		typedef _DelegateBase5<PAR1, PAR2, PAR3, PAR4, PAR5> CDelegate;
		typedef CList<_DelegateBase5<PAR1, PAR2, PAR3, PAR4, PAR5>*> CDelegateList;
		
	protected:
		 CDelegateList m_Delegate;	

	public:
		_EventHandlerImpl5() {
		}

		~_EventHandlerImpl5() {
			RemoveAll();			
		}
		
		void AddHandler(CDelegate* pDelegate) {
			m_Delegate.AddHead(pDelegate);
		}

		void AddHandler(CDelegateList* pDelegate) {
			POSITION pos = m_Delegate.GetHeadPosition();
			while (pos != NULL) {
				m_Delegate.AddHead(m_Delegate.GetNext(pos));
			}
		}

		void RemoveHandler(CDelegate* pDelegate) 
		{
			POSITION pos = m_Delegate.GetHeadPosition();
			while (pos != NULL)
			{
				POSITION posLast = pos;
				CDelegate* pDeleg = m_Delegate.GetNext(pos);

				if (pDeleg->Equals(pDelegate)) {
					m_Delegate.RemoveAt(pos);
					delete pDeleg;
				}
			}
			delete pDelegate;
		}

		void RemoveHandler(CDelegateList* pDelegate) {
			POSITION pos = m_Delegate.GetHeadPosition();
			while (pos != NULL) {
				RemoveHandler(m_Delegate.GetNext(pos));
			}			
		}

		void RemoveAll()
		{
			POSITION pos = m_Delegate.GetHeadPosition();
			while (pos != NULL) {
				delete m_Delegate.GetNext(pos);
			}
		}

		void Invoke(PAR1 par1, PAR2 par2, PAR3 par3, PAR4 par4, PAR5 par5)
		{
			POSITION pos = m_Delegate.GetHeadPosition();
			while (pos != NULL) {				
				m_Delegate.GetNext(pos)->Invoke(par1, par2, par3, par4, par5);
			}
		}

		inline _EventHandlerImpl5& operator += (CDelegate* pDelegate)
		{
			AddHandler(pDelegate);
			return *this;
		}

		inline _EventHandlerImpl5& operator += (CDelegateList* pDelegateList)
		{
			AddHandler(pDelegateList);
			return *this;
		}

		inline _EventHandlerImpl5& operator -= (CDelegate* pDelegate)
		{
			RemoveHandler(pDelegate);
			return *this;
		}

		inline _EventHandlerImpl5& operator -= (CDelegateList* pDelegateList)
		{
			RemoveHandler(pDelegateList);
			return *this;
		}

		inline _EventHandlerImpl5& operator = (CDelegate* pDelegate)
		{
			RemoveAll();
			AddHandler(pDelegate);
			return *this;
		}

		inline _EventHandlerImpl5& operator = (CDelegateList* pDelegateList)
		{
			RemoveAll();
			AddHandler(pDelegateList);
			return *this;
		}

		inline void operator () (PAR1 par1, PAR2 par2, PAR3 par3, PAR4 par4, PAR5 par5)
		{
			if (m_Delegate.GetHeadPosition() != NULL)
				Invoke(par1, par2, par3, par4, par5);
		}

		inline bool operator == (int Null) {			
			return m_Delegate.GetHeadPosition() == NULL;
		}

		inline bool operator != (int Null) {			
			return m_Delegate.GetHeadPosition() != NULL;
		}

		CDelegateList* GetInvocationList() 
		{
			CDelegateList* pRet = new CDelegateList();
			POSITION pos = m_Delegate.GetHeadPosition();
			while (pos != NULL) {				
				pRet->Add(m_Delegate.GetNext(pos)->Clone());
			}

			return pRet;
		}		
	};




















//DELEG_TEMPL_PARAMETRY -- class T1, class T2, ...
//DELEG_TEMPL_PAR_ARGS -- T1, T2, ...
//DELEG_PARAMETRY -- T1 identif1, T2 identif2, ...
//DELEG_PAR_ARGS -- identif1, identif2, ...

	template <class PAR1, class PAR2, class PAR3, class PAR4, class PAR5, class PAR6>
	class _DelegateBase6 : public Delegate
	{
	public:
		virtual void Invoke(PAR1 par1, PAR2 par2, PAR3 par3, PAR4 par4, PAR5 par5, PAR6 par6) = 0;
		virtual _DelegateBase6* Clone() = 0;
		virtual bool Equals(_DelegateBase6* pDelegate) = 0;
	};

	
	template <class T, class PAR1, class PAR2, class PAR3, class PAR4, class PAR5, class PAR6>
	class _DelegateImpl6 : public _DelegateBase6<PAR1, PAR2, PAR3, PAR4, PAR5, PAR6>
	{
		//attributes
	protected:
		typedef void (T::*MBFUNC)(PAR1 par1, PAR2 par2, PAR3 par3, PAR4 par4, PAR5 par5, PAR6 par6);
		typedef void (*STFUNC)(PAR1 par1, PAR2 par2, PAR3 par3, PAR4 par4, PAR5 par5, PAR6 par6);
		
		T* _target;
		MBFUNC _method;
		STFUNC _function;

	private:
		_DelegateImpl6()
		{
		}

	public:
		_DelegateImpl6(T* pObj, MBFUNC pMbFunc)
		{
			_target = pObj;
			_method = pMbFunc;
			_function = NULL;
		}

		_DelegateImpl6(STFUNC pStFunc)
		{
			_target = NULL;
			_method = NULL;
			_function = pStFunc;
		}

		virtual void Invoke(PAR1 par1, PAR2 par2, PAR3 par3, PAR4 par4, PAR5 par5, PAR6 par6)
		{				
			if (_method != NULL)			
				(this->_target->*this->_method)(par1, par2, par3, par4, par5, par6);			
			else // if (_function != NULL)			
				_function(par1, par2, par3, par4, par5, par6);
		}

		virtual _DelegateBase6* Clone() 
		{
			if (_method != NULL)
				return new _DelegateImpl6(_target, _method);
			else
				return new _DelegateImpl6(_function);
		}

		virtual bool Equals(_DelegateBase6* pDelegate) {
			_DelegateImpl6* pDel = (_DelegateImpl6*)pDelegate;
			return _method == pDel->_method && _target == pDel->_target &&
				_function == pDel->_function;
		}
	};

	template <class PAR1, class PAR2, class PAR3, class PAR4, class PAR5, class PAR6>
	class _EventHandlerImpl6
	{
	public:
		typedef _DelegateBase6<PAR1, PAR2, PAR3, PAR4, PAR5, PAR6> CDelegate;
		typedef CList<_DelegateBase6<PAR1, PAR2, PAR3, PAR4, PAR5, PAR6>*> CDelegateList;
		
	protected:
		 CDelegateList m_Delegate;	

	public:
		_EventHandlerImpl6() {
		}

		~_EventHandlerImpl6() {
			RemoveAll();			
		}
		
		void AddHandler(CDelegate* pDelegate) {
			m_Delegate.AddHead(pDelegate);
		}

		void AddHandler(CDelegateList* pDelegate) {
			POSITION pos = m_Delegate.GetHeadPosition();
			while (pos != NULL) {
				m_Delegate.AddHead(m_Delegate.GetNext(pos));
			}
		}

		void RemoveHandler(CDelegate* pDelegate) 
		{
			POSITION pos = m_Delegate.GetHeadPosition();
			while (pos != NULL)
			{
				POSITION posLast = pos;
				CDelegate* pDeleg = m_Delegate.GetNext(pos);

				if (pDeleg->Equals(pDelegate)) {
					m_Delegate.RemoveAt(pos);
					delete pDeleg;
				}
			}
			delete pDelegate;
		}

		void RemoveHandler(CDelegateList* pDelegate) {
			POSITION pos = m_Delegate.GetHeadPosition();
			while (pos != NULL) {
				RemoveHandler(m_Delegate.GetNext(pos));
			}			
		}

		void RemoveAll()
		{
			POSITION pos = m_Delegate.GetHeadPosition();
			while (pos != NULL) {
				delete m_Delegate.GetNext(pos);
			}
		}

		void Invoke(PAR1 par1, PAR2 par2, PAR3 par3, PAR4 par4, PAR5 par5, PAR6 par6)
		{
			POSITION pos = m_Delegate.GetHeadPosition();
			while (pos != NULL) {				
				m_Delegate.GetNext(pos)->Invoke(par1, par2, par3, par4, par5, par6);
			}
		}

		inline _EventHandlerImpl6& operator += (CDelegate* pDelegate)
		{
			AddHandler(pDelegate);
			return *this;
		}

		inline _EventHandlerImpl6& operator += (CDelegateList* pDelegateList)
		{
			AddHandler(pDelegateList);
			return *this;
		}

		inline _EventHandlerImpl6& operator -= (CDelegate* pDelegate)
		{
			RemoveHandler(pDelegate);
			return *this;
		}

		inline _EventHandlerImpl6& operator -= (CDelegateList* pDelegateList)
		{
			RemoveHandler(pDelegateList);
			return *this;
		}

		inline _EventHandlerImpl6& operator = (CDelegate* pDelegate)
		{
			RemoveAll();
			AddHandler(pDelegate);
			return *this;
		}

		inline _EventHandlerImpl6& operator = (CDelegateList* pDelegateList)
		{
			RemoveAll();
			AddHandler(pDelegateList);
			return *this;
		}

		inline void operator () (PAR1 par1, PAR2 par2, PAR3 par3, PAR4 par4, PAR5 par5, PAR6 par6)
		{
			if (m_Delegate.GetHeadPosition() != NULL)
				Invoke(par1, par2, par3, par4, par5, par6);
		}

		inline bool operator == (int Null) {			
			return m_Delegate.GetHeadPosition() == NULL;
		}

		inline bool operator != (int Null) {			
			return m_Delegate.GetHeadPosition() != NULL;
		}

		CDelegateList* GetInvocationList() 
		{
			CDelegateList* pRet = new CDelegateList();
			POSITION pos = m_Delegate.GetHeadPosition();
			while (pos != NULL) {				
				pRet->Add(m_Delegate.GetNext(pos)->Clone());
			}

			return pRet;
		}		
	};













