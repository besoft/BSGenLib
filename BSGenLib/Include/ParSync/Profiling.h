#pragma once
#include "ShortCS.h"
#include "../Common/Hashtable.h"
#include "../Common/String.h"

//#define __PROFILING__				
//__PROFILING__ enables automatic profiling

//#define __PROFILING_FUNC_RDTSC	
//__PROFILING_FUNC_RDTSC enables use of faster but possibly incorrect RDTSC instruction
//see also: http://msdn2.microsoft.com/en-us/library/bb173458.aspx

//#define __PROFILING_LEVEL		0
//profiling level, denotes what to profile

//class for profiler nodes (category)
class CProfileNode
{
public:
	class CProfileNodeEnumerator
	{
	protected:
		POSITION m_pCurrentPos;		//the current position
		CProfileNode* m_pNode;		//node being enumerated
		CProfileNode* m_pCurrent;
	public:
		CProfileNodeEnumerator(CProfileNode* pNode) {
			m_pNode = pNode; Reset();
		}

		//gets the current node (it calls AddRef upon its retrieval)
		inline CProfileNode* GetCurrent() {			
			return m_pCurrent;
		}

		//moves the position to the next node
		//returns  false if the enumerator has passed the end of the collection
		bool MoveNext();

		//resets the position
		void Reset();			
	};

protected:
	//name of the node (should be unique)
	CString m_szName;

	//list of nodes
	CStringHashTable <CProfileNode*> m_SubNodes;

public:
	//ctor
	CProfileNode(LPCTSTR lpszName) : m_szName(lpszName) {		
	}	

	virtual ~CProfileNode();

public:	
	//returns unique key
	inline CString GetName() {
		return m_szName;
	}

	//resets the associated value
	virtual void ResetValue() {
	}

	//returns value of this node
	virtual CString GetStrValue() {
		return _T("");
	}

	//returns XML string of this node and its subnodes (recursive function)
	virtual CString ToString();	

	//adds the given node into the collection
	//note: it calls AddRef upon the addition
	void AddSubNode(CProfileNode* pNode);

	//removes the subnode with the given name from the collection	
	CProfileNode* RemoveSubNode(LPCTSTR lpszName);

	//searches the collection for the node with lpszName
	//returns NULL if not found	
	CProfileNode* GetSubNode(LPCTSTR lpszName);


	//creates a new enumerator for this node that can be used to
	//browse its subnodes, the caller has to delete the returned object
	virtual CProfileNodeEnumerator* GetEnumerator() {
		return new CProfileNodeEnumerator(this);
	}

	//creates a clone of this node and its subnodes
	CProfileNode* Clone();	

	//creates a clone of this node
	inline virtual CProfileNode* CloneNode() {
		return new CProfileNode(m_szName);
	}

	friend class CProfileNodeEnumerator;
};

//this class provides the caller with "text node"
class CProfileInfo : public CProfileNode
{
protected:
	CString m_szValue;	//value

public:
	CProfileInfo(LPCTSTR lpszName) : CProfileNode(lpszName) {		
	}

	CProfileInfo(LPCTSTR lpszName, LPCTSTR lpszValue) : CProfileNode(lpszName) {
		m_szValue = lpszValue;
	}

public:
	//gets the current value
	inline CString GetValue() {
		return m_szValue;
	}

	//assign new value
	inline void SetValue(LPCTSTR lpszValue) {
		m_szValue = lpszValue;
	}

	//resets the associated value
	virtual void ResetValue() {
		m_szValue.Empty();
	}

	//returns value of this node
	virtual CString GetStrValue() {
		return m_szValue;
	}

	//creates a clone of this node
	inline virtual CProfileNode* CloneNode() {
		return new CProfileInfo(m_szName, m_szValue);
	}
};

//basic counter
//it cannot be instanced nor created (it has no public ctor)
class CProfileCounter : public CProfileNode
{
protected:
	int m_Value;		//value

public:
	CProfileCounter(LPCTSTR lpszName) : CProfileNode(lpszName) {
		m_Value = 0;
	}

	CProfileCounter(LPCTSTR lpszName, int value) : CProfileNode(lpszName) {
		m_Value = value;
	}
public:
	//gets the current value
	inline int GetValue() {
		return m_Value;
	}

	//assign new value
	inline void SetValue(int newValue) {
		m_Value = newValue;
	}

	//increment value
	inline void Increment() {
		m_Value += 1;
	}

	//decrement value
	inline void Decrement() {
		m_Value -= 1;
	}

	//increment value by some constant
	inline void IncrementBy(int inc) {
		m_Value += inc;
	}

	//decrement value by some constant
	inline void DecrementBy(int dec) {
		m_Value -= dec;
	}

	//resets the associated value
	virtual void ResetValue() {
		m_Value = 0;
	}

	//returns value of this node
	virtual CString GetStrValue() {
		CString szRet;
		szRet.Format(_T("%d"), m_Value);
		return szRet;
	}

	//creates a clone of this node
	inline virtual CProfileNode* CloneNode() {
		return new CProfileCounter(m_szName, m_Value);
	}
};

// this class can be used for the precise measuring of time intervals
// it can measure the time needed for one operation (just call Start + Stop)
// or the total time elapsed while performing several operations 
// (call Start + call Stop repeatedly to measure individual time for operations)
#pragma pack(push)
#pragma pack(4)
class CProfileTimer : public CProfileNode
{
protected:
	LARGE_INTEGER m_liElapsed;
	int m_nCount;

	LARGE_INTEGER m_liFreq;
	LARGE_INTEGER m_liStart;
	LARGE_INTEGER m_liEnd;	
	bool m_bRunning;
public:
	//ctor	
	CProfileTimer(LPCTSTR lpszName = _T("")) : CProfileNode(lpszName) {
		QueryPerformanceFrequency(&m_liFreq);
		Reset();
	}

	virtual ~CProfileTimer() {
	}



	//resets the counter
	inline void Reset() {
		m_liStart.QuadPart = m_liEnd.QuadPart = m_liElapsed.QuadPart = 0;
		m_nCount = 0; m_bRunning = false;
	}

	//starts the measuring of a new time interval
	//note: the total time counter is not reset
	inline void Start() {
		QueryPerformanceCounter(&m_liStart);
		m_bRunning = true;
	}

	//saves the time that has elapsed since the previous call of Start method
	//and increases the total time counter by that amount
	//Note: the method can be called multiple times
	inline void Stop()
	{
		QueryPerformanceCounter(&m_liEnd);
		m_nCount++;

		m_liElapsed.QuadPart += m_liEnd.QuadPart - m_liStart.QuadPart;
		m_bRunning = false;		
	}

	//stops the measuring of previous time interval (see Stop) and
	//starts the measuring of a new time interval
	//Note: this is more precise version of Stop() and Start() calls
	inline void StopAndStart()
	{
		Stop();	m_liStart.QuadPart = m_liEnd.QuadPart; 
		m_bRunning = true;
	}

	//returns true, if the timer is running
	inline bool IsRunning() {
		return m_bRunning;
	}

	//returns number of milliseconds that elapsed during the previous time interval
	inline double GetElapsedTimeInMs() {		
		return (1000*(m_liEnd.QuadPart - m_liStart.QuadPart)) / ((double)m_liFreq.QuadPart);
	}

	//returns number of ticks that elapsed during the previous time interval
	inline LONGLONG GetElapsedTime() {	
		return m_liEnd.QuadPart - m_liStart.QuadPart;
	}

	//returns number of milliseconds that elapsed during all previous time intervals
	inline double GetTotalElapsedTimeInMs() {		
		return (1000*(m_liElapsed.QuadPart)) / ((double)m_liFreq.QuadPart);	
	}

	//returns number of ticks that elapsed during all previous time intervals
	inline LONGLONG GetTotalElapsedTime() {
		return m_liElapsed.QuadPart;
	}

	//returns number of counted time intervals
	inline int GetCount() {
		return m_nCount;
	}

	//returns the performance counter frequency
	inline LARGE_INTEGER GetFrequency() {
		return m_liFreq;
	}
public:
	//returns value of this node
	virtual CString GetStrValue();	
	
	//returns XML string of this node and its subnodes (recursive function)
	virtual CString ToString();	

	//resets the associated value
	virtual void ResetValue() {
		Reset();
	}	

	//creates a clone of this node
	virtual CProfileNode* CloneNode();
};

#pragma pack(pop)

//this class is suitable for measuring of function calls
class CProfileFuncTimer : public CProfileTimer
{
public:
	CProfileFuncTimer(LPCTSTR lpszName = _T("")) : CProfileTimer(lpszName)
	{
	}
public:
	//returns value of this node
	virtual CString GetStrValue();

	//creates a clone of this node
	virtual CProfileNode* CloneNode();		

	friend class CProfileFunction;
};


//this class contains measured profiles
//it is thread free
class CProfiler
{
public:
	typedef struct ProfilerCategory {
		DECLARE_ENUM_STRUCT(ProfilerCategory)

		enum {
			Counters = 0,
			Timers = 1,
			Functions = 2,
			Others = 3,
		};

		const static _TCHAR* m_Strings[];
		inline static const _TCHAR** GetStrings() {
			return &m_Strings[0];
		}
		inline CString ToString() {
			return m_Strings[m_nValue];
		}		
	} ProfilerCategory;

protected:	
	CShortCSEx m_cs;
	CProfileNode* m_pRootNode;
	bool m_bThreadProfiling;

	typedef struct CACHE_ITEM 
	{
		ProfilerCategory categ;
		CProfileNode* pNode;
		DWORD dwThreadID;
	} CACHE_ITEM;

	typedef CStringHashTable<CACHE_ITEM> CCacheMap;
	CCacheMap m_Cache;

public:
	//bThreadProfiling denotes whether counters, timers and fuctions are 
	//profiled individualy per each thread - default is false
	CProfiler(bool bThreadProfiling = false) {
		m_pRootNode = new CProfileNode(_T("ROOT"));	
		m_bThreadProfiling = bThreadProfiling;		
	}

	~CProfiler() {
		delete m_pRootNode;			
	}

	//appends subtree under the given category
	void Append(CProfileNode* pNode, LPCTSTR lpszCategory);

	//appends subtree under the given category (without ROOT)
	void Append(CProfiler* pProfiler, LPCTSTR lpszCategory);

	//gets the node (or creates it, if it does not exist)
	inline CProfileNode* GetProfileNode(LPCTSTR lpszProfVar, ProfilerCategory categ) {
		m_cs.Lock();
		CProfileNode* pRet = GetProfVar(lpszProfVar, categ);
		m_cs.Unlock();
		return pRet;
	}

	//sets value for info node
	inline void SetValue(LPCTSTR lpszProfVar, LPCTSTR lpszValue) {	
		m_cs.Lock();
		((CProfileInfo*)GetProfVar(lpszProfVar, ProfilerCategory::Others))->SetValue(lpszValue);
		m_cs.Unlock();
	}

	//sets value for counter node
	inline void SetValue(LPCTSTR lpszProfVar, int nValue) {	
		m_cs.Lock();
		((CProfileCounter*)GetProfVar(lpszProfVar, ProfilerCategory::Counters))->SetValue(nValue);
		m_cs.Unlock();
	}

	//increments the counter with the name path lpszProfVar
	inline void Inc(LPCTSTR lpszProfVar, int nIncBy = 1) {
		m_cs.Lock();
		((CProfileCounter*)GetProfVar(lpszProfVar, ProfilerCategory::Counters))->IncrementBy(nIncBy);
		m_cs.Unlock();
	}

	//decrements the counter with the name path lpszProfVar
	inline void Dec(LPCTSTR lpszProfVar, int nDecBy = 1) {
		Inc(lpszProfVar, -nDecBy);
	}

	//starts timer with the name path lpszProfVar
	inline void Start(LPCTSTR lpszProfVar) {	
		m_cs.Lock();
		((CProfileTimer*)GetProfVar(lpszProfVar, ProfilerCategory::Timers))->Start();
		m_cs.Unlock();
	}

	//stops timer with the name path lpszProfVar
	inline void Stop(LPCTSTR lpszProfVar) {		
		m_cs.Lock();
		((CProfileTimer*)GetProfVar(lpszProfVar, ProfilerCategory::Timers))->Stop();
		m_cs.Unlock();
	}

	//starts function timer with the name path lpszProfVar
	inline void StartFunc(LPCTSTR lpszProfVar) {
		m_cs.Lock();
		((CProfileTimer*)GetProfVar(lpszProfVar, ProfilerCategory::Functions))->Start();
		m_cs.Unlock();
	}

	//stops timer with the name path lpszProfVar
	inline void StopFunc(LPCTSTR lpszProfVar) {
		m_cs.Lock();
		((CProfileTimer*)GetProfVar(lpszProfVar, ProfilerCategory::Functions))->Stop();
		m_cs.Unlock();
	}
	
	//resets all profiled values in one category
	void ResetProfile(ProfilerCategory categ);
	inline void ResetProfile() {
		ResetProfile(ProfilerCategory::Counters);
		ResetProfile(ProfilerCategory::Functions);
		ResetProfile(ProfilerCategory::Timers);
		ResetProfile(ProfilerCategory::Others);		
	}

	//removes all profiled objects in one category
	void RemoveProfile(ProfilerCategory categ);
	inline void RemoveProfile() {
		RemoveProfile(ProfilerCategory::Counters);
		RemoveProfile(ProfilerCategory::Functions);
		RemoveProfile(ProfilerCategory::Timers);
		RemoveProfile(ProfilerCategory::Others);		
	}
	
	//saves the profile into the file (XML format)
	//returns false, if it cannot be saved
	bool SaveProfile(LPCTSTR lpszPathName);

protected:
	//returns node for the given lpszProfVar, if it does not exist, it is 
	//created (either timer or counter)
	//Note: lpszProVar contains full address (including categories),
	//categories are delimited by ::
	//e.g. ROOT::Timers::MyCounter
	inline CProfileNode* GetProfVar(LPCTSTR lpszProfVar, ProfilerCategory categ);
	CProfileNode* GetProfVar2(LPCTSTR lpszProfVar, ProfilerCategory categ, bool bAmbiguous);
	
	//returns the full address of the local variable name
	CString GetFullProfVarAddr(LPCTSTR lpszProfVar, ProfilerCategory categ);

	friend class CProfileFunction;
};

inline CProfileNode* CProfiler::GetProfVar(LPCTSTR lpszProfVar, ProfilerCategory categ)
{	
#ifdef __BSGEN_PUREWIN
	CString szProfVar = lpszProfVar;
	CACHE_ITEM* pCurVal = m_Cache.Lookup(szProfVar);
#else
	CCacheMap::CPair* pCurVal = m_Cache.Lookup(lpszProfVar);
#endif

	if (pCurVal == NULL)
		return GetProfVar2(lpszProfVar, categ, false);

#ifdef __BSGEN_PUREWIN
	CACHE_ITEM& value = *pCurVal;
#else
	CACHE_ITEM& value = pCurVal->m_value;
#endif

	if (m_bThreadProfiling)
	{
		if (value.dwThreadID != ::GetCurrentThreadId())
			return GetProfVar2(lpszProfVar, categ, true);
	}
			
	if (value.categ == categ)
		return value.pNode;
	
	return GetProfVar2(lpszProfVar, categ, true);
}

#include "AutoProfiling.h"