#pragma once
#ifndef RLTEXT_WINDOWS_CPP
#define RLTEXT_WINDOWS_CPP





#include "Windows.h"
#include "Types.hpp"
#include <string>



namespace rlText
{

	/// <summary>
	/// Convert a UTF-8 string to a UTF-16 string (for Win32 API function calls).
	/// </summary>
	/// <param name="szUTF8">The UTF-8 string to convert to UTF-16. Cannot be zero.</param>
	inline RLTEXT_UTF16STRING UTF8toUTF16(const RLTEXT_UTF8CHAR *szUTF8) noexcept
	{
		const auto iSize = rlText_UTF8toUTF16(szUTF8, nullptr, 0);
		if (iSize <= 1)
			return RLTEXT_UTF16STRING{};

		RLTEXT_UTF16STRING sResult(iSize - 1, 0);
		rlText_UTF8toUTF16(szUTF8, sResult.data(), sResult.length() + 1);

		return sResult;
	}

	/// <summary>
	/// Convert a UTF-16 string to a UTF-8 string (for Win32 API function calls).
	/// </summary>
	/// <param name="szUTF16">The UTF-16 string to convert to UTF-8. Cannot be zero.</param>
	inline RLTEXT_UTF8STRING UTF16toUTF8(const RLTEXT_UTF16CHAR *szUTF16)
	{
		const auto iSize = rlText_UTF16toUTF8(szUTF16, nullptr, 0);
		if (iSize <= 1)
			return RLTEXT_UTF8STRING{};

		RLTEXT_UTF8STRING sResult(iSize - 1, 0);
		rlText_UTF16toUTF8(szUTF16, reinterpret_cast<RLTEXT_UTF8CHAR*>(sResult.data()),
			sResult.length() + 1);

		return sResult;
	}

}





#endif // RLTEXT_WINDOWS_CPP