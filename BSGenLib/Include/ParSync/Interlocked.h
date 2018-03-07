/* ${FILENAME}
** ${LASTMODIFIED}
**
** ${COPYRIGHT}
** ${LICENCE}
**
** DESCRIPTION: Encapsulates Atomic Instructions, functionality adopted from .NET 
** NOTE:		The variables to change must be aligned on a 32-bit boundary; 
				otherwise, the functions will fail on multiprocessor x86 systems and any non-x86 systems				
*/

#ifndef BES_INTERLOCKED_NET_H
#define BES_INTERLOCKED_NET_H

#if _MSC_VER >= 1400
#include <intrin.h>
#endif

#if _MSC_VER >= 1400
#pragma intrinsic (_InterlockedIncrement)
#pragma intrinsic (_InterlockedDecrement)
#pragma intrinsic (_InterlockedExchange)
#pragma intrinsic (_InterlockedExchangeAdd)
#pragma intrinsic (_InterlockedCompareExchange)
#pragma intrinsic (_InterlockedOr)
#pragma intrinsic (_InterlockedXor)
#pragma intrinsic (_InterlockedAnd)
#pragma intrinsic (_interlockedbittestandreset)
#pragma intrinsic (_interlockedbittestandset)
#else
#define _InterlockedIncrement  InterlockedIncrement
#define _InterlockedDecrement InterlockedDecrement
#define _InterlockedExchangeAdd	InterlockedExchangeAdd
#define _InterlockedCompareExchange InterlockedCompareExchange
#define _InterlockedExchange InterlockedExchange
#endif

class CInterlocked
{
public:
	//Adds two integers and replaces the first integer with the sum, as an atomic operation. 
	//returns the new value stored at lVar	 
	__forceinline static long Add(volatile long* lVar, long nValue) {		
		return _InterlockedExchangeAdd(lVar, nValue) + nValue;
	}

	//Compares two values for equality and, if they are equal, replaces one of the values. 
	//The returned value is the previous value
	__forceinline static void* CompareExchange(volatile void** lVar, void* lPtrValue, void* lPtrComparand) {
		return InterlockedCompareExchangePointer(
//#if _MSC_VER < 1400
			(volatile PVOID*)
//#endif
			lVar, lPtrValue, lPtrComparand);
	}

	//Compares two values for equality and, if they are equal, replaces one of the values. 
	//The returned value is the previous value
	__forceinline static long CompareExchange(volatile long* lVar, long lPtrValue, long lPtrComparand){
		return _InterlockedCompareExchange(lVar, lPtrValue, lPtrComparand);
	}


	//Decrements a specified variable and stores the result, as an atomic operation. 
	//The function returns the resulting decremented value. 
	__forceinline static long Decrement(volatile long* lVar) {
		return _InterlockedDecrement(
#if _MSC_VER < 1400
			(LONG*)
#endif
			lVar);
	}

	//Sets a variable to a specified value as an atomic operation. 
	//The function returns the original value. 
	__forceinline static long Exchange(volatile long* lVar, long lPtrValue) {		
		return _InterlockedExchange(lVar, lPtrValue);		
	}

	//Increments a specified variable and stores the result, as an atomic operation. 
	//The function returns the resulting incremented value. 
	__forceinline static long Increment(volatile long* lVar){
		return _InterlockedIncrement(
#if _MSC_VER < 1400
			(LONG*)
#endif
			lVar);
	}

#if _MSC_VER >= 1400
	//Used to perform an atomic operation (in this case, the AND operation) 
	//on a variable shared by multiple threads.
	//The function returns the original value. 
	__forceinline static long And(volatile long* lVar, long lPtrValue) {
		return _InterlockedAnd(lVar, lPtrValue);
	}

	//Used to perform an atomic operation (in this case, the OR operation) 
	//on a variable shared by multiple threads.
	//The function returns the original value. 
	__forceinline static long Or(volatile long* lVar, long lPtrValue) {
		return _InterlockedOr(lVar, lPtrValue);
	}

	//Used to perform an atomic operation (in this case, the XOR operation) 
	//on a variable shared by multiple threads.
	//The function returns the original value. 
	__forceinline static long Xor(volatile long* lVar, long lPtrValue) {
		return _InterlockedXor(lVar, lPtrValue);
	}

	//Sets bit b of the address lVar to 0 and returns its original value.
	__forceinline static BYTE BitReset(long* lVar, BYTE b) {
		return _interlockedbittestandreset(lVar, b);
	}

	//Sets bit b of the address lVar to 1 and returns its original value.
	__forceinline static BYTE BitSet(long* lVar, BYTE b) {
		return _interlockedbittestandset(lVar, b);
	}	
#endif
};

#endif //BES_INTERLOCKED_NET_H
