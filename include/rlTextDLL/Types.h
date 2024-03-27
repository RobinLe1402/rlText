#pragma once
#ifndef RLTEXTDLL_TYPES
#define RLTEXTDLL_TYPES





#include <stdint.h>

#ifndef __cplusplus  // C
#include <stdbool.h> // bool
#include <uchar.h>   // char16_t, char32_t
#endif


typedef uint32_t rlText_Bool;
typedef uint32_t rlText_Unsigned;
typedef uint64_t rlText_Count;


#if defined(__cplusplus) && __cplusplus >= 202002L // C++20
#define RLTEXT_UTF8CHAR char8_t
#else
#define RLTEXT_UTF8CHAR char
#endif


typedef struct
{
	char16_t ch[2];
	uint16_t count;
} rlText_UTF16Codepoint;

typedef struct
{
	RLTEXT_UTF8CHAR ch[4];
	uint8_t         count;
} rlText_UTF8Codepoint;





#endif // RLTEXTDLL_TYPES