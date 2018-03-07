#pragma once

//class Convert - C++ light version of .NET class System.Convert
//-> see MSDN, how to use methods compatible with .NET
//-> for other methods, see comments
class RMCMNAPI Convert
{
public:
	static CString TrueString;
	static CString FalseString;

	static CString TrueString_CZ;
	static CString FalseString_CZ;
public:
	//Converts a specified value to an equivalent Boolean value. 
	
	//Converts the specified String representation of a logical value to its Boolean equivalent.
	inline static bool ToBoolean(LPCTSTR lpszValue, LPCTSTR lpszLocalTrueString = TrueString_CZ);

	//Converts the specified String representation of a number to an equivalent 8-bit unsigned integer. 
	//if pbSucceded version is used, it returns true or false depending on the result of conversion
	inline static BYTE ToByte(LPCTSTR lpszValue);
	inline static BYTE ToByte(LPCTSTR lpszValue, bool* pbSucceded);

#ifndef __BSGEN_PUREWIN
	//Converts the specified String representation of a date and time to an equivalent COleDateTime. 
	inline static COleDateTime ToDateTime(LPCTSTR lpszValue, LCID lcid = LANG_USER_DEFAULT);
#endif

	//Converts a specified value to a 16-bit signed integer.
	inline static short ToInt16(LPCTSTR lpszValue);
	inline static short ToInt16(LPCTSTR lpszValue, bool* pbSucceded);

	//Converts a specified value to a 32-bit signed integer.
	inline static long ToInt32(LPCTSTR lpszValue);
	inline static long ToInt32(LPCTSTR lpszValue, bool* pbSucceded);

	//Converts the specified String representation of a number to an equivalent double-precision floating point number. 
	//if pbSucceded version is used, it returns true or false depending on the result of conversion
	inline static double ToDouble(LPCTSTR lpszValue);
	inline static double ToDouble(LPCTSTR lpszValue, bool* pbSucceded);

	//Converts a specified value to a single-precision floating point number. 
	//if pbSucceded version is used, it returns true or false depending on the result of conversion
	inline static float ToSingle(LPCTSTR lpszValue);
	inline static float ToSingle(LPCTSTR lpszValue, bool* pbSucceded);

	//Converts a specified value to a 16-bit unsigned integer.
	//if pbSucceded version is used, it returns true or false depending on the result of conversion
	inline static WORD ToUInt16(LPCTSTR lpszValue);
	inline static WORD ToUInt16(LPCTSTR lpszValue, bool* pbSucceded);

	//Converts a specified value to a 32-bit unsigned integer.
	//if pbSucceded version is used, it returns true or false depending on the result of conversion
	inline static DWORD ToUInt32(LPCTSTR lpszValue);
	inline static DWORD ToUInt32(LPCTSTR lpszValue, bool* pbSucceded);

	//Converts the specified value to its equivalent String representation. 
	inline static CString ToString(bool bVal);
	inline static CString ToString(BYTE byVal);
#ifndef __BSGEN_PUREWIN
	inline static CString ToString(COleDateTime& dtVal, LPCTSTR lpszFormat = _T("%m/%d/%Y %H:%M:%S"));
#endif
	inline static CString ToString(double value);
	inline static CString ToString(float value);
	inline static CString ToString(short value);
	inline static CString ToString(WORD value);
	inline static CString ToString(long value);
	inline static CString ToString(DWORD value);
	inline static CString ToString(int value) {
		return ToString((long)value);
	}
	inline static CString ToString(UINT value) {
		return ToString((DWORD)value);
	}


#ifndef __BSGEN_PUREWIN
	//Converts the value of an array of 8-bit unsigned integers to its 
	//equivalent String representation encoded with base 64 digits.
#ifdef __BSGEN_MFC
	static CString ToBase64String(CByteArray& value);
#endif
	static CString ToBase64String(CArray<BYTE>& value);
	static CString ToBase64String(BYTE* pData, int nCount);

	//Converts the specified String, which encodes binary data as base 64 digits,
	//to an equivalent 8-bit unsigned integer array. 
	//RELEASE_NOTE: calling is not compatible with .NET
#ifdef __BSGEN_MFC
	static void FromBase64String(LPCTSTR lpszValue, CByteArray& value);
#endif
	static void FromBase64String(LPCTSTR lpszValue, CArray<BYTE>& value);
	static void FromBase64String(LPCTSTR lpszValue, BYTE** pData, int* pnCount);
#endif
};

//Converts the specified String representation of a logical value to its Boolean equivalent.
inline bool Convert::ToBoolean(LPCTSTR lpszValue, LPCTSTR lpszLocalTrueString)
{
	if (!_tcsicmp(lpszValue, TrueString) ||		
		!_tcsicmp(lpszValue, lpszLocalTrueString) || 
		!_tcsicmp(lpszValue, _T("1")))
		return true;
	
	return false;
}

//Converts the specified String representation of a number to an equivalent 8-bit unsigned integer. 
inline BYTE Convert::ToByte(LPCTSTR lpszValue)
{
	return (BYTE)ToUInt32(lpszValue);
}

inline BYTE Convert::ToByte(LPCTSTR lpszValue, bool* pbSucceded)
{
	return (BYTE)ToUInt32(lpszValue, pbSucceded);
}

//Converts a specified value to a 16-bit signed integer.
inline short Convert::ToInt16(LPCTSTR lpszValue)
{
	return (short)ToInt32(lpszValue);
}

inline short Convert::ToInt16(LPCTSTR lpszValue, bool* pbSucceded)
{
	return (short)ToInt32(lpszValue, pbSucceded);
}

//Converts a specified value to a 32-bit signed integer.
inline long Convert::ToInt32(LPCTSTR lpszValue)
{
	_TCHAR* endptr;
	return _tcstol(lpszValue, &endptr, 0);
}

inline long Convert::ToInt32(LPCTSTR lpszValue, bool* pbSucceded)
{
	_TCHAR* endptr;
	long nRet = _tcstol(lpszValue, &endptr, 0);
	*pbSucceded = *endptr == _T('\0');
	return nRet;
}

#ifndef __BSGEN_PUREWIN
//Converts the specified String representation of a date and time to an equivalent COleDateTime. 
inline COleDateTime Convert::ToDateTime(LPCTSTR lpszValue, LCID lcid)
{
	COleDateTime dtRet;
	dtRet.ParseDateTime(lpszValue, 0, lcid);
	return dtRet;
}
#endif

//Converts the specified String representation of a number to an equivalent double-precision floating point number. 
inline double Convert::ToDouble(LPCTSTR lpszValue)
{
	_TCHAR* endptr;
	return _tcstod(lpszValue, &endptr);
}

inline double Convert::ToDouble(LPCTSTR lpszValue, bool* pbSucceded)
{
	_TCHAR* endptr;
	double dblRet = _tcstod(lpszValue, &endptr);	
	*pbSucceded = *endptr == _T('\0');
	return dblRet;
}

//Converts the specified String representation of a number to an equivalent single-precision floating point number. 
inline float Convert::ToSingle(LPCTSTR lpszValue)
{
	return (float)ToDouble(lpszValue);	
}

inline float Convert::ToSingle(LPCTSTR lpszValue, bool* pbSucceded)
{
	return (float)ToDouble(lpszValue, pbSucceded);	
}

//Converts a specified value to a 16-bit unsigned integer.
inline WORD Convert::ToUInt16(LPCTSTR lpszValue)
{
	return (WORD)ToUInt32(lpszValue);
}

inline WORD Convert::ToUInt16(LPCTSTR lpszValue, bool* pbSucceded)
{
	return (WORD)ToUInt32(lpszValue, pbSucceded);
}

//Converts a specified value to a 32-bit unsigned integer.
inline DWORD Convert::ToUInt32(LPCTSTR lpszValue)
{
	_TCHAR* endptr;
	return _tcstoul(lpszValue, &endptr, 0);
}

inline DWORD Convert::ToUInt32(LPCTSTR lpszValue, bool* pbSucceded)
{
	_TCHAR* endptr;
	long nRet = _tcstoul(lpszValue, &endptr, 0);
	*pbSucceded = *endptr == _T('\0');
	return nRet;
}

//Converts the specified value to its equivalent String representation. 
inline CString Convert::ToString(bool bVal)
{
	if (bVal)
		return TrueString;
	else
		return FalseString;
}

inline CString Convert::ToString(BYTE byVal)
{
	return ToString((DWORD)byVal);
}

inline CString Convert::ToString(WORD value)
{
	return ToString((DWORD)value);
}

inline CString Convert::ToString(DWORD value)
{	
	CString szRet;
	szRet.Format(_T("%d"), value);
	return szRet;
}

inline CString Convert::ToString(short value)
{
	return ToString((long)value);
}

inline CString Convert::ToString(long value)
{
	CString szRet;
	szRet.Format(_T("%d"), value);
	return szRet;
}

#ifndef __BSGEN_PUREWIN
inline CString Convert::ToString(COleDateTime& dtVal, LPCTSTR lpszFormat){
	return dtVal.Format(lpszFormat);
}
#endif

inline CString Convert::ToString(float value) {
	return ToString((double)value);
}

inline CString Convert::ToString(double value)
{
	CString szRet;
	szRet.Format(_T("%f"), value);
	return szRet;
}