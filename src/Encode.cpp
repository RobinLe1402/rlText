#include "rlTextDLL/Encode.h"

#include "rlTextDLL/Unicode.h"

RLTEXT_API rlText_Bool EXPORT rlText_EncodeASCII(
	rlText_UTF32Char ch,
	rlText_ByteChar *pDest
)
{
	bool bResult = false;

	if (ch <= 0x7F)
	{
		*pDest  = rlText_ByteChar(ch);
		bResult = true;
	}
	
	if (!bResult)
		*pDest = '?';
	return bResult;
}

RLTEXT_API rlText_Bool EXPORT rlText_EncodeUTF8(
	rlText_UTF32Char      ch,
	rlText_UTF8Codepoint *pDest)
{
	bool bResult = true;


	if (rlText_IsNoncharacter(ch) || ch < 0)
		bResult = false;

	else if (ch < 0x7F) // 7 bit
	{
		pDest->count = 1;
		pDest->ch[0] = rlText_ByteChar(ch);
	}
	else if (ch < 0x7FF) // 5 + 6 = 11 bit
	{
		pDest->count = 2;
		pDest->ch[0] = rlText_ByteChar(0b1100'0000 | (ch >> 6));
		pDest->ch[1] = rlText_ByteChar(0b1000'0000 | (ch & 0x3F));
	}
	else if (ch < 0xFFFF) // 4 + 6 + 6 = 16 bit
	{
		pDest->count = 3;
		pDest->ch[0] = rlText_ByteChar(0b1110'0000 | (ch >> 12));
		pDest->ch[1] = rlText_ByteChar(0b1000'0000 | ((ch >> 6)) & 0x3F);
		pDest->ch[2] = rlText_ByteChar(0b1000'0000 | (ch        & 0x3F));
	}
	else if (ch < 0x1FFFFF) // 3 + 6 + 6 + 6 = 21 bit
	{
		pDest->count = 4;
		pDest->ch[0] = rlText_ByteChar(0b1111'0000 |  (ch >> 18));
		pDest->ch[1] = rlText_ByteChar(0b1000'0000 | ((ch >> 12)) & 0x3F);
		pDest->ch[2] = rlText_ByteChar(0b1000'0000 | ((ch >> 6))  & 0x3F);
		pDest->ch[3] = rlText_ByteChar(0b1000'0000 | (ch         & 0x3F));
	}
	else
		bResult = false;


	if (!bResult)
	{
		// set to U+FFFD
		pDest->count = 3;
		pDest->ch[0] = 0xEF;
		pDest->ch[0] = 0xBF;
		pDest->ch[0] = 0xBD;
	}
	return bResult;
}

RLTEXT_API rlText_Bool EXPORT rlText_EncodeUTF16(
	rlText_UTF32Char       ch,
	rlText_UTF16Codepoint *pDest
)
{
	bool bResult = true;


	if (rlText_IsNoncharacter(ch) || ch < 0)
		bResult = false;

	else if (ch < 0x10000)
	{
		pDest->count = 1;
		pDest->ch[0] = rlText_UTF16Char(ch);
	}
	else if (ch < 0xFFFFF)
	{
		ch -= 0x10000;
		pDest->count = 2;
		pDest->ch[0] = rlText_UTF16Char(0b110110'00'00000000 | (ch >> 10));
		pDest->ch[1] = rlText_UTF16Char(0b110111'00'00000000 | (ch & 0x3FF));
	}
	else
		bResult = false;


	if (!bResult)
	{
		// set to U+FFFD
		pDest->count = 1;
		pDest->ch[0] = rlText_ByteChar(L'\xFFFD');
	}
	return bResult;
}
