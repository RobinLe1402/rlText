#pragma once
#ifndef RLTEXTDLL_DECODE_CPP
#define RLTEXTDLL_DECODE_CPP





#include "Decode.h"
#include "Types.hpp"



namespace rlText
{
	
	/// <summary>
	/// Try to decode a Windows Codepage 1252 character.
	/// </summary>
	/// <param name="cEncoded">The single encoded character.</param>
	/// <param name="cDecoded">
	/// The destination for the encoded data<para/>
	/// If the function succeed, the value is the decoded character.<para/>
	/// If the function fails, the value is the replacement character <c>U+FFFD</c>.
	/// </param>
	/// <returns>Did the function succeed?</returns>
	bool DecodeCP1252(
		char      cEncoded,
		char32_t &cDecoded
	) noexcept
	{
		return rlText_DecodeCP1252(cEncoded, &cDecoded);
	}

	/// <summary>
	/// Try to decode a UTF-8 character.
	/// </summary>
	/// <param name="pEncoded">
	/// A pointer to a zero-terminated string. This function will attempt to decode the first
	/// character.
	/// </param>
	/// <param name="cDecoded">
	/// The destination for the encoded data<para/>
	/// If the function succeed, the value is the decoded character.<para/>
	/// If the function fails, the value is the replacement character <c>U+FFFD</c>.
	/// </param>
	/// <returns>
	/// If the function succeeds, it returns the number of codeunits read from <c>pEncoded</c>.
	/// <para/>
	/// If the function fails, it returns zero.
	/// </returns>
	unsigned DecodeUTF8(
		const RLTEXT_UTF8CHAR *pEncoded,
		      char32_t    &cDecoded
	) noexcept
	{
		return rlText_DecodeUTF8(pEncoded, &cDecoded);
	}

	/// <summary>
	/// Try to decode a UTF-16 character.
	/// </summary>
	/// <param name="pEncoded">
	/// A pointer to a zero-terminated string. This function will attempt to decode the first
	/// character.
	/// </param>
	/// <param name="cDecoded">
	/// The destination for the encoded data<para/>
	/// If the function succeed, the value is the decoded character.<para/>
	/// If the function fails, the value is the replacement character <c>U+FFFD</c>.
	/// </param>
	/// <returns>
	/// If the function succeeds, it returns the number of codeunits read from <c>pEncoded</c>.
	/// <para/>
	/// If the function fails, it returns zero.
	/// </returns>
	unsigned DecodeUTF16(
		const char16_t *pEncoded,
		      char32_t &cDecoded
	) noexcept
	{
		return rlText_DecodeUTF16(pEncoded, &cDecoded);
	}
	
}





#endif // RLTEXTDLL_DECODE_CPP