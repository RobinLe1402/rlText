#pragma once
#ifndef RLTEXTDLL_UNICODE_CPP
#define RLTEXTDLL_UNICODE_CPP





#include "Unicode.h"



namespace rlText
{
	/// <summary>
	/// Is a Unicode character located inside of a private use area?
	/// </summary>
	bool IsPrivateUse(char32_t ch) noexcept { return rlText_IsPrivateUse(ch); }

	/// <summary>
	/// Is a value a noncharacter in Unicode?
	/// </summary>
	bool IsNoncharacter(char32_t ch) noexcept { return rlText_IsNoncharacter(ch); }

}





#endif // RLTEXTDLL_UNICODE_CPP