#pragma once
#ifndef RLTEXTDLL_DECODE
#define RLTEXTDLL_DECODE





#include "ExportSpecs.h"
#include "Types.h"



/// <summary>
/// Try to decode a Windows Codepage 1252 character.
/// </summary>
/// <param name="cEncoded">The single encoded character.</param>
/// <param name="pDecoded">
/// The destination for the decoded data. Must always be a valid pointer.<para/>
/// If the function succeed, the value is the decoded character.<para/>
/// If the function fails, the value is the replacement character <c>U+FFFD</c>.
/// </param>
/// <returns>
/// If the function succeeds, it returns nonzero.<para/>
/// If the function fails, it returns zero.
/// </returns>
RLTEXT_API rlText_Bool RLTEXT_LIB rlText_DecodeCP1252(
	char      cEncoded,
	char32_t *pDecoded
);

/// <summary>
/// Try to decode a UTF-8 character.
/// </summary>
/// <param name="pEncoded">
/// A pointer to a zero-terminated string. This function will attempt to decode the first character.
/// </param>
/// <param name="pDecoded">
/// The destination for the decoded data. Must always be a valid pointer.<para/>
/// If the function succeed, the value is the decoded character.<para/>
/// If the function fails, the value is the replacement character <c>U+FFFD</c>.
/// </param>
/// <returns>
/// If the function succeeds, it returns the number of codeunits read from <c>pEncoded</c>.<para/>
/// If the function fails, it returns zero.
/// </returns>
RLTEXT_API rlText_Unsigned RLTEXT_LIB rlText_DecodeUTF8(
#if defined(__cplusplus) && __cplusplus >= 202002L // C++20
	const char8_t  *pEncoded,
#else
	const char     *pEncoded,
#endif
	      char32_t *pDecoded
);

/// <summary>
/// Try to decode a UTF-16 character.
/// </summary>
/// <param name="pEncoded">
/// A pointer to a zero-terminated string. This function will attempt to decode the first character.
/// </param>
/// <param name="pDecoded">
/// The destination for the decoded data. Must always be a valid pointer.<para/>
/// If the function succeed, the value is the decoded character.<para/>
/// If the function fails, the value is the replacement character <c>U+FFFD</c>.
/// </param>
/// <returns>
/// If the function succeeds, it returns the number of codeunits read from <c>pEncoded</c>.<para/>
/// If the function fails, it returns zero.
/// </returns>
RLTEXT_API rlText_Unsigned RLTEXT_LIB rlText_DecodeUTF16(
	const char16_t *pEncoded,
	      char32_t *pDecoded
);





#endif // RLTEXTDLL_DECODE