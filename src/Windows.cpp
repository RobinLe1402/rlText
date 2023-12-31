#include "rlTextDLL/Windows.h"

#include "rlTextDLL/Decode.h"
#include "rlTextDLL/Encode.h"



RLTEXT_API rlText_Count RLTEXT_LIB rlText_UTF8toUTF16(
	const RLTEXT_UTF8CHAR  *szUTF8,
		  RLTEXT_UTF16CHAR *pBuf,
	      rlText_Count      iBufSize
)
{
	char32_t        ch         = 0;
	rlText_Count    iResult    = 0;
	rlText_Unsigned iReadBytes = 0;

	if (!pBuf || !iBufSize)
	{
		// "calculate needed size" mode

		iResult = 1; // terminating zero
		while (*szUTF8)
		{
			iReadBytes = rlText_DecodeUTF8(szUTF8, &ch);
			if (!iReadBytes)
			{
				// decoding failed --> cancel, return 0
				iResult = 0;
				break; // while
			}

			if (ch < 0x10000)
				iResult += 1;
			else
				iResult += 2;

			szUTF8 += iReadBytes;
		}
	}
	else
	{
		// copy mode

		iResult = 1;
		while (*szUTF8 && iResult < iBufSize)
		{
			iReadBytes = rlText_DecodeUTF8(szUTF8, &ch);
			if (!iReadBytes)
			{
				// decoding failed --> cancel, return 0
				iResult = 0;
				break; // while
			}

			if (ch < 0x10000)
			{
				*pBuf = char16_t(ch);

				pBuf    += 1;
				iResult += 1;
			}
			else
			{
				if (iBufSize - iResult == 1)
					break; // cancel copy, not enough memory in buffer for both surrogates.
				ch -= 0x10000;

				pBuf[0] = 0xD800 | (ch >> 10);
				pBuf[1] = 0xDC00 | (ch & 0x03FF);

				pBuf    += 2;
				iResult += 2;
			}

			szUTF8 += iReadBytes;
		}
		*pBuf = 0; // terminating zero
	}

	return iResult;
}

RLTEXT_API rlText_Count RLTEXT_LIB rlText_UTF16toUTF8(
	const RLTEXT_UTF16CHAR *szUTF16,
		  RLTEXT_UTF8CHAR  *pBuf,
		  rlText_Count      iBufSize
)
{
	char32_t        ch         = 0;
	rlText_Count    iResult    = 0;
	rlText_Unsigned iReadWords = 0;

	if (!pBuf || !iBufSize)
	{
		// "calculate needed size" mode

		iResult = 1; // terminating zero
		while (*szUTF16)
		{
			iReadWords = rlText_DecodeUTF16(reinterpret_cast<const char16_t *>(szUTF16), &ch);
			if (!iReadWords)
			{
				// decoding failed --> cancel, return 0
				iResult = 0;
				break; // while
			}

			if (ch <= 0x7F) // single codeunit (7 bits)
				iResult += 1;
			else if (ch <= 0x7FF) // two codeunits (5 + 6 = 11 bits)
				iResult += 2;
			else if (ch <= 0xFFFF) // three codeunits (4 + 6 + 6 = 16 bits)
				iResult += 3;
			else // four codeunits (3 + 6 + 6 + 6 = 21 bits)
				iResult += 4;

			szUTF16 += iReadWords;
		}
	}
	else
	{
		// copy mode

		iResult = 1;
		while (*szUTF16 && iResult < iBufSize)
		{
			iReadWords = rlText_DecodeUTF16(reinterpret_cast<const char16_t *>(szUTF16), &ch);
			if (!iReadWords)
			{
				// decoding failed --> cancel, return 0
				iResult = 0;
				break; // while
			}

			if (ch <= 0x7F) // single codeunit (7 bits)
			{
				*pBuf    = char(ch);

				pBuf    += 1;
				iResult += 1;
			}
			else if (ch <= 0x7FF) // two codeunits (5 + 6 = 11 bits)
			{
				if (iBufSize - iResult < 2)
					break; // cancel copy, not enough memory in buffer for all code units.

				pBuf[0] = 0xC0 | (ch >> 6);
				pBuf[1] = 0x80 | (ch & 0x3F);

				pBuf    += 2;
				iResult += 2;
			}
			else if (ch <= 0xFFFF) // three codeunits (4 + 6 + 6 = 16 bits)
			{
				if (iBufSize - iResult < 3)
					break; // cancel copy, not enough memory in buffer for all code units.

				pBuf[0] = 0xE0 |  (ch >> 12);
				pBuf[1] = 0x80 | ((ch >> 6 ) & 0x3F);
				pBuf[2] = 0x80 | ( ch        & 0x3F);

				pBuf    += 3;
				iResult += 3;
			}
			else // four codeunits (3 + 6 + 6 + 6 = 21 bits)
			{
				if (iBufSize - iResult < 4)
					break; // cancel copy, not enough memory in buffer for all code units.

				pBuf[0] = 0xF0 |  (ch >> 18);
				pBuf[1] = 0x80 | ((ch >> 12) & 0x3F);
				pBuf[2] = 0x80 | ((ch >> 6 ) & 0x3F);
				pBuf[3] = 0x80 |  (ch        & 0x3F);

				pBuf    += 4;
				iResult += 4;
			}

			szUTF16 += iReadWords;
		}
		*pBuf = 0; // terminating zero
	}

	return iResult;
}
