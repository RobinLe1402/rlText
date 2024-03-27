#pragma once
#ifndef RLTEXTDLL_TYPES_CPP
#define RLTEXTDLL_TYPES_CPP





#include <string>
#include "Types.h"



namespace rlText
{

	using UTF8Codepoint  = rlText_UTF8Codepoint;
	using UTF16Codepoint = rlText_UTF16Codepoint;

#if __cplusplus >= 202002L // C++20
#define RLTEXT_UTF8STRING std::u8string
#else
#define RLTEXT_UTF8STRING std::string
#endif

#ifdef _WIN32
#define RLTEXT_UTF16STRING std::wstring
#else
#define RLTEXT_UTF16STRING std::u16string
#endif
	
}





#endif // RLTEXTDLL_TYPES_CPP