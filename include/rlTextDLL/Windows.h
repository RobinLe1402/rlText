#pragma once
#ifndef RLTEXTDLL_WINDOWS
#define RLTEXTDLL_WINDOWS





#include "ExportSpecs.h"
#include "Types.h"



#ifdef _WIN32
#define RLTEXT_UTF16CHAR wchar_t
#else
#define RLTEXT_UTF16CHAR char16_t
#endif



/// <summary>
/// Convert a UTF-8 string to a UTF-16 string (for Win32 API function calls).
/// </summary>
/// <param name="szUTF8">The UTF-8 string to convert to UTF-16. Cannot be zero.</param>
/// <param name="pBuf">
/// A pointer to the buffer that should receive the UTF-16 version of <c>szUTF8</c>.<para/>
/// Can be zero. In this case, the return value indicates the required buffer size (in characters).
/// </param>
/// <param name="iBufSize">
/// The length of the buffer pointed to by <c>pBuf</c>.<para/>
/// Can be zero. In this case, the return value indicates the required buffer size (in characters).
/// </param>
/// <returns>
/// If both <c>pBuf</c> and <c>iBufSize</c> are nonzero, the return value indicates the count of
/// characters written to the buffer, including the terminating zero.<para/>
/// If either <c>pBuf</c> or <c>iBufSize</c> is set to zero, the return value indicates the required
/// size of the buffer, in characters, including the terminating zero.<para/>
/// <para/>
/// It is always guaranteed that only valid UTF-16 values are written to <c>pBuf</c>, if the buffer
/// is not large enough to hold the 2nd surrogate, the 1st surrogate is not written either.
/// </returns>
RLTEXT_API rlText_Count RLTEXT_LIB rlText_UTF8toUTF16(
	const RLTEXT_UTF8CHAR  *szUTF8,
		  RLTEXT_UTF16CHAR *pBuf,
	      rlText_Count      iBufSize
);

/// <summary>
/// Convert a UTF-16 string to a UTF-8 string (for Win32 API function calls).
/// </summary>
/// <param name="szUTF16">The UTF-16 string to convert to UTF-8. Cannot be zero.</param>
/// <param name="pBuf">
/// A pointer to the buffer that should receive the UTF-8 version of <c>szUTF16</c>.<para/>
/// Can be zero. In this case, the return value indicates the required buffer size (in characters).
/// </param>
/// <param name="iBufSize">
/// The length of the buffer pointed to by <c>pBuf</c>.<para/>
/// Can be zero. In this case, the return value indicates the required buffer size (in characters).
/// </param>
/// <returns>
/// If both <c>pBuf</c> and <c>iBufSize</c> are nonzero, the return value indicates the count of
/// characters written to the buffer, including the terminating zero.<para/>
/// If either <c>pBuf</c> or <c>iBufSize</c> is set to zero, the return value indicates the required
/// size of the buffer, in characters, including the terminating zero.<para/>
/// <para/>
/// It is always guaranteed that only valid UTF-8 values are written to <c>pBuf</c>, if the buffer
/// is not large enough to hold some of the codeunits of a character, the first few codeunits are
/// not written either.
/// </returns>
RLTEXT_API rlText_Count RLTEXT_LIB rlText_UTF16toUTF8(
	const RLTEXT_UTF16CHAR *szUTF16,
		  RLTEXT_UTF8CHAR  *pBuf,
	      rlText_Count      iBufSize
);





#endif // RLTEXTDLL_WINDOWS