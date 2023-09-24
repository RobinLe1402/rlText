#pragma once
#ifndef RLTEXTDLL_TYPES
#define RLTEXTDLL_TYPES





#include <stdint.h>
#ifndef __cplusplus 
// C doesn't have static_assert, bool and char16_t/char32_t built-in.
// To use those, the following headers are required.
#include <assert.h>  // static_assert
#include <stdbool.h> // bool
#include <uchar.h>   // char16_t, char32_t
#endif

static_assert(sizeof(char16_t) == 2, "char16_t is not 2 bytes big => cannot compile rlText");
static_assert(sizeof(char32_t) == 4, "char32_t is not 4 bytes big => cannot compile rlText");


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
	char    ch[4];
	uint8_t count;
} rlText_UTF8Codepoint;





#endif // RLTEXTDLL_TYPES