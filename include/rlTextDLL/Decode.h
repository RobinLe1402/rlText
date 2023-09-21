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
/// The destination for the decoded data. Must always be a valid pointer.<para />
/// If the function succeed, the value is the decoded character.<para />
/// If the function fails, the value is the replacement character <c>U+FFFD</c>.
/// </param>
/// <returns>
/// If the function succeeds, it returns nonzero.<para />
/// If the function fails, it returns zero.
/// </returns>
RLTEXT_API rlText_Bool EXPORT rlText_DecodeCP1252(
	rlText_ByteChar   cEncoded,
	rlText_UTF32Char *pDecoded
);

/// <summary>
/// Try to decode a UTF-8 character.
/// </summary>
/// <param name="cEncoded">
/// A pointer to a zero-terminated string. This function will attempt to decode the first character.
/// </param>
/// <param name="pDecoded">
/// The destination for the decoded data. Must always be a valid pointer.<para />
/// If the function succeed, the value is the decoded character.<para />
/// If the function fails, the value is the replacement character <c>U+FFFD</c>.
/// </param>
/// <returns>
/// If the function succeeds, it returns the number of codeunits read from <c>pEncoded</c>.<para />
/// If the function fails, it returns zero.
/// </returns>
RLTEXT_API rlText_Unsigned EXPORT rlText_DecodeUTF8(
	rlText_ByteChar  *pEncoded,
	rlText_UTF32Char *pDecoded
);

/// <summary>
/// Try to decode a UTF-16 character.
/// </summary>
/// <param name="cEncoded">
/// A pointer to a zero-terminated string. This function will attempt to decode the first character.
/// </param>
/// <param name="pDecoded">
/// The destination for the decoded data. Must always be a valid pointer.<para />
/// If the function succeed, the value is the decoded character.<para />
/// If the function fails, the value is the replacement character <c>U+FFFD</c>.
/// </param>
/// <returns>
/// If the function succeeds, it returns the number of codeunits read from <c>pEncoded</c>.<para />
/// If the function fails, it returns zero.
/// </returns>
RLTEXT_API rlText_Unsigned EXPORT rlText_DecodeUTF16(
	rlText_UTF16Char *pEncoded,
	rlText_UTF32Char *pDecoded
);





#endif // RLTEXTDLL_DECODE