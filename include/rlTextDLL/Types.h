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


typedef struct
{
	char16_t ch[2];
	uint16_t count;
} rlText_UTF16Codepoint;

typedef struct
{
#if defined(__cplusplus) && __cplusplus >= 202002L // C++20
	char8_t ch[4];
#else
	char    ch[4];
#endif
	uint8_t count;
} rlText_UTF8Codepoint;





#endif // RLTEXTDLL_TYPES