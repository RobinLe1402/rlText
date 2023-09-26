#ifndef RLTEXTDLL_ENCODE_CPP
#define RLTEXTDLL_ENCODE_CPP





#include "Encode.h"
#include "Types.hpp"



namespace rlText
{

	/// <summary>
	/// Try to encode a character as ASCII.
	/// </summary>
	/// <param name="ch">The character to encode.</param>
	/// <param name="cDest">
	/// The destination for the encoded data<para/>
	/// If the function succeed, the value is the encoded character.<para/>
	/// If the function fails, the value is the replacement character <c>'?'</c>.
	/// </param>
	/// <returns>Did the function succeed?</returns>
	bool EncodeASCII(char32_t ch, char &cDest) noexcept
	{
		return rlText_EncodeASCII(ch, &cDest);
	}

	/// <summary>
	/// Try to encode a character as Windows Codepage 1252.
	/// </summary>
	/// <param name="ch">The character to encode.</param>
	/// <param name="pDest">
	/// The destination for the encoded data<para/>
	/// If the function succeed, the value is the encoded character.<para/>
	/// If the function fails, the value is the replacement character <c>'?'</c>.
	/// </param>
	/// <returns>
	/// If the function succeeds, it returns nonzero.<para/>
	/// If the function fails, it returns zero.
	/// </returns>
	bool EncodeCP1252(char32_t ch, char &cDest) noexcept
	{
		return rlText_EncodeCP1252(ch, &cDest);
	}

	/// <summary>
	/// Try to encode a character as UTF-8.
	/// </summary>
	/// <param name="ch">The character to encode.</param>
	/// <param name="pDest">
	/// The destination for the encoded data.<para/>
	/// If the function succeed, the value is the encoded character.<para/>
	/// If the function fails, the value is the replacement character <c>U+FFFD</c>.<para/>
	/// The unused entries of the <c>ch</c> array will always be set to zero.
	/// </param>
	/// <returns>Did the function succeed?</returns>
	bool EncodeUTF8(char32_t ch, rlText_UTF8Codepoint &oDest) noexcept
	{
		return rlText_EncodeUTF8(ch, &oDest);
	}

	bool EncodeUTF16(char32_t ch, rlText_UTF16Codepoint &oDest) noexcept
	{
		return rlText_EncodeUTF16(ch, &oDest);
	}

}





#endif // RLTEXTDLL_ENCODE_CPP