#pragma once

//as there is no namespace for C++ enums, using of the same name in two 
//different enums that were declared in the same namespace is not allowed
//example: 
//
//class A { 
//typedef enum MODE_ENUM { Unknown = -1, Open=0, Create=1, } MODE_ENUM;
//typedef enum SHARE_ENUM { Unknown = -1, ShareNone=0, ShareAll=1, } SHARE_ENUM;
//
//Moreover, C++ enums does not allow to store combinations in its variables
//To overcome these drawbacks, use the following definition:
//
//typedef struct MODE_ENUM { DECLARE_ENUM_STRUCT(MODE_ENUM) enum { Unknown=-1, ...}; } MODE_ENUM;
//
//You may also use macros BEGIN_ENUM_STRUCT and END_ENUM_STRUCT but then your
//enum will not be recognised by the intellisense

#define DECLARE_ENUM_STRUCT(enum_name)	\
protected:\
    long m_nValue;\
public:\
    enum_name(long nValue = 0) : m_nValue(nValue) { } \
    inline enum_name& operator = (long nValue) { m_nValue = nValue;  return *this; } \
    inline operator long() const { return m_nValue; } \
    inline operator long&() { return m_nValue; }

#define BEGIN_ENUM_STRUCT(enum_name)	\
typedef struct enum_name {\
	DECLARE_ENUM_STRUCT(enum_name)	\
	enum 	{

#define END_ENUM_STRUCT(enum_name)	\
		};	\
	} enum_name