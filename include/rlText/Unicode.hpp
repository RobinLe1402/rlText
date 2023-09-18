#pragma once
#ifndef RLTEXT_UNICODE
#define RLTEXT_UNICODE





namespace rlText
{

	namespace Unicode
	{

		/// <summary>
		/// Is a Unicode character located inside of a private use area?
		/// </summary>
		constexpr inline bool IsPrivateUse(char32_t ch) noexcept
		{
			return ((ch >= 0xE000 && ch <= 0xF8FF) /* Private Use Zone */ ||
				(ch >= 0xF0000 && ch < 0x110000) /* Private Use Planes */);
		}

		/// <summary>
		/// Is a value a noncharacter in Unicode?
		/// </summary>
		constexpr inline bool IsNoncharacter(char32_t ch) noexcept
		{
			return ((ch >= 0xFDD0 && ch <= 0xFDEF) /* noncharacter range in BMP */ ||
				(ch & 0xFFFF) >= 0xFFFE /* last two code points of each plane */);
		}

	}

}





#endif // RLTEXT_UNICODE