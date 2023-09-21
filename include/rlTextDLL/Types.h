#pragma once
#ifndef RLTEXTDLL_TYPES
#define RLTEXTDLL_TYPES





typedef int             rlText_Bool;
typedef unsigned        rlText_Unsigned;
typedef unsigned char   rlText_ByteChar;
typedef unsigned short  rlText_UTF16Char;
typedef unsigned        rlText_UTF32Char;


typedef struct
{
	rlText_UTF16Char ch[2];
	unsigned short   count;
} rlText_UTF16Codepoint;

typedef struct
{
	rlText_ByteChar ch[4];
	unsigned char   count;
} rlText_UTF8Codepoint;





#endif // RLTEXTDLL_TYPES