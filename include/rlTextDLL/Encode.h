#pragma once
#ifndef RLTEXTDLL_ENCODE
#define RLTEXTDLL_ENCODE





#include "ExportSpecs.h"
#include "Types.h"



/// <summary>
/// Try to encode a character as ASCII.
/// </summary>
/// <param name="ch">The character to encode.</param>
/// <param name="pDest">
/// The destination for the encoded data. Must always be a valid pointer.<para />
/// If the function succeed, the value is the encoded character.<para />
/// If the function fails, the value is the replacement character <c>'?'</c>.
/// </param>
/// <returns>
/// If the function succeeds, it returns nonzero.<para />
/// If the function fails, it returns zero.
/// </returns>
RLTEXT_API rlText_Bool EXPORT rlText_EncodeASCII(
	rlText_UTF32Char ch,
	rlText_ByteChar *pDest
);

/// <summary>
/// Try to encode a character as Windows Codepage 1252.
/// </summary>
/// <param name="ch">The character to encode.</param>
/// <param name="pDest">
/// The destination for the encoded data. Must always be a valid pointer.<para />
/// If the function succeed, the value is the encoded character.<para />
/// If the function fails, the value is the replacement character <c>'?'</c>.
/// </param>
/// <returns>
/// If the function succeeds, it returns nonzero.<para />
/// If the function fails, it returns zero.
/// </returns>
RLTEXT_API rlText_Bool EXPORT rlText_EncodeCP1252(
	rlText_UTF32Char ch,
	rlText_ByteChar *pDest
);

/// <summary>
/// Try to encode a character as UTF-8.
/// </summary>
/// <param name="ch">The character to encode.</param>
/// <param name="pDest">
/// The destination for the encoded data. Must always be a valid pointer.<para />
/// If the function succeed, the value is the encoded character.<para />
/// If the function fails, the value is the replacement character <c>U+FFFD</c>.<para />
/// The unused entries of the <c>ch</c> array will always be set to zero.
/// </param>
/// <returns>
/// If the function succeeds, it returns nonzero.<para />
/// If the function fails, it returns zero.
/// </returns>
RLTEXT_API rlText_Bool EXPORT rlText_EncodeUTF8(
	rlText_UTF32Char      ch,
	rlText_UTF8Codepoint *pDest
);

/// <summary>
/// Try to encode a character as UTF-16.
/// </summary>
/// <param name="ch">The character to encode.</param>
/// <param name="pDest">
/// The destination for the encoded data. Must always be a valid pointer.<para />
/// If the function succeed, the value is the encoded character.<para />
/// If the function fails, the value is the replacement character <c>U+FFFD</c>.<para />
/// The unused entries of the <c>ch</c> array will always be set to zero.
/// </param>
/// <returns>
/// If the function succeeds, it returns nonzero.<para />
/// If the function fails, it returns zero.
/// </returns>
RLTEXT_API rlText_Bool EXPORT rlText_EncodeUTF16(
	rlText_UTF32Char       ch,
	rlText_UTF16Codepoint *pDest
);





#endif // RLTEXTDLL_ENCODE