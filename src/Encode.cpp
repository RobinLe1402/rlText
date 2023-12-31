#include "rlTextDLL/Encode.h"

#include "rlTextDLL/Unicode.h"
#include "include/CP1252.hpp"

#include <memory.h>



RLTEXT_API rlText_Bool RLTEXT_LIB rlText_EncodeASCII(
	char32_t ch,
	char    *pDest
)
{
	bool bResult = false;

	if (ch <= 0x7F)
	{
		*pDest  = char(ch);
		bResult = true;
	}
	
	if (!bResult)
		*pDest = '?';
	return bResult;
}

RLTEXT_API rlText_Bool RLTEXT_LIB rlText_EncodeCP1252(
	char32_t ch,
	char    *pDest
)
{
	bool bResult = false;

	if (ch < 0x80 || (ch >= 0xA0 && ch < 0xFF))
	{
		bResult = true;
		*pDest  = char(ch);
	}
	else if (ch > 0xFF)
	{
		for (int i = 0; i < 0x20; ++i)
		{
			if (ch == oCharsCP1252[i])
			{
				bResult = true;
				*pDest  = char(0x80 + i);
				break;
			}
		}
	}

	if (!bResult)
		*pDest = '?';
	return bResult;
}

RLTEXT_API rlText_Bool RLTEXT_LIB rlText_EncodeUTF8(
	char32_t              ch,
	rlText_UTF8Codepoint *pDest)
{
	bool bResult = true;
	memset(pDest, 0, sizeof(*pDest));


	if (rlText_IsNoncharacter(ch) || ch < 0)
		bResult = false;

	else if (ch < 0x7F) // 7 bit
	{
		pDest->count = 1;
		pDest->ch[0] = char(ch);
	}
	else if (ch < 0x7FF) // 5 + 6 = 11 bit
	{
		pDest->count = 2;
		pDest->ch[0] = char(0b1100'0000 | (ch >> 6));
		pDest->ch[1] = char(0b1000'0000 | (ch & 0x3F));
	}
	else if (ch < 0xFFFF) // 4 + 6 + 6 = 16 bit
	{
		pDest->count = 3;
		pDest->ch[0] = char(0b1110'0000 | ( ch >> 12));
		pDest->ch[1] = char(0b1000'0000 | ((ch >> 6 )) & 0x3F);
		pDest->ch[2] = char(0b1000'0000 | ( ch         & 0x3F));
	}
	else if (ch < 0x1FFFFF) // 3 + 6 + 6 + 6 = 21 bit
	{
		pDest->count = 4;
		pDest->ch[0] = char(0b1111'0000 | ( ch >> 18));
		pDest->ch[1] = char(0b1000'0000 | ((ch >> 12) & 0x3F));
		pDest->ch[2] = char(0b1000'0000 | ((ch >> 6)  & 0x3F));
		pDest->ch[3] = char(0b1000'0000 | ( ch        & 0x3F));
	}
	else
		bResult = false;


	if (!bResult)
	{
		// set to U+FFFD
		pDest->count = 3;
		pDest->ch[0] = char(0xEF);
		pDest->ch[0] = char(0xBF);
		pDest->ch[0] = char(0xBD);
	}
	return bResult;
}

RLTEXT_API rlText_Bool RLTEXT_LIB rlText_EncodeUTF16(
	char32_t               ch,
	rlText_UTF16Codepoint *pDest
)
{
	bool bResult = true;
	memset(pDest, 0, sizeof(*pDest));


	if (rlText_IsNoncharacter(ch) || ch < 0)
		bResult = false;

	else if (ch < 0x10000)
	{
		pDest->count = 1;
		pDest->ch[0] = char16_t(ch);
	}
	else if (ch < 0xFFFFF)
	{
		ch -= 0x10000;
		pDest->count = 2;
		pDest->ch[0] = char16_t(0b110110'00'00000000 | (ch >> 10));
		pDest->ch[1] = char16_t(0b110111'00'00000000 | (ch & 0x3FF));
	}
	else
		bResult = false;


	if (!bResult)
	{
		// set to U+FFFD
		pDest->count = 1;
		pDest->ch[0] = u'\uFFFD';
	}
	return bResult;
}
