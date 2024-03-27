#include "rlTextDLL/Decode.h"

#include "rlTextDLL/Unicode.h"
#include "include/CP1252.hpp"

RLTEXT_API rlText_Bool RLTEXT_LIB rlText_DecodeCP1252(
	char      cEncoded,
	char32_t *pDecoded
)
{
	const unsigned char cEncodedU = cEncoded;

	if (cEncodedU >= 0x80 && cEncodedU < 0xA0)
		*pDecoded = oCharsCP1252[cEncodedU - 0x80];
	else
		*pDecoded = cEncodedU;

	return *pDecoded != 0xFFFD;
}

RLTEXT_API rlText_Unsigned RLTEXT_LIB rlText_DecodeUTF8(
	const char8_t  *pEncoded,
	      char32_t *pDecoded
)
{
	rlText_Unsigned iResult = 0;

	auto c = *pEncoded;
	if ((c & 0x80) == 0)
	{
		iResult   = 1;
		*pDecoded = c;
	}
	else if ((c & 0xE0) == 0xC0) // 0b110X'XXXX => 2 codeunits
	{
		iResult = ((pEncoded[1] & 0xC0) == 0x80) ? 2 : 0;
		if (iResult)
		{
			*pDecoded  = (c & 0x1F) << 6;
			*pDecoded |= pEncoded[1] & 0x3F;
		}
	}
	else if ((c & 0xF0) == 0xE0) // 0b1110'XXXX => 3 codeunits
	{
		iResult = (
			(pEncoded[1] & 0xC0) == 0x80 &&
			(pEncoded[2] & 0xC0) == 0x80)
			? 3 : 0;

		if (iResult)
		{
			*pDecoded  = (c           & 0x0F) << 12;
			*pDecoded |= (pEncoded[1] & 0x3F) << 6;
			*pDecoded |=  pEncoded[2] & 0x3F;
		}
	}
	else if ((c & 0xF8) == 0xF0) // 0b1111'0XXX => 4 codeunits
	{
		iResult = (
			(pEncoded[1] & 0xC0) == 0x80 &&
			(pEncoded[2] & 0xC0) == 0x80 &&
			(pEncoded[3] & 0xC0) == 0x80)
			? 4 : 0;

		if (iResult)
		{
			*pDecoded  = (c           & 0x07) << 18;
			*pDecoded |= (pEncoded[1] & 0x3F) << 12;
			*pDecoded |= (pEncoded[2] & 0x3F) << 6;
			*pDecoded |=  pEncoded[3] & 0x3F;
		}
	}

	if (iResult && rlText_IsNoncharacter(*pDecoded))
		iResult = 0;
	if (!iResult)
		*pDecoded = 0xFFFD;
	return iResult;
}

RLTEXT_API rlText_Unsigned RLTEXT_LIB rlText_DecodeUTF16(
	const char16_t *pEncoded,
	      char32_t *pDecoded
)
{
	rlText_Unsigned iResult = 0;

	auto c = *pEncoded;

	if ((c & 0xFC00) == 0xD800)
	{
		iResult = ((pEncoded[1] & 0xFC00) == 0xDC00) ? 2 : 0;
		if (iResult)
		{
			*pDecoded = (char32_t(c & 0x3FF) << 10) | (pEncoded[1] & 0x3FF);
			*pDecoded += 0x10000;
		}
	}
	else
	{
		iResult   = 1;
		*pDecoded = c;
	}

	if (iResult && rlText_IsNoncharacter(*pDecoded))
		iResult = 0;
	if (!iResult)
		*pDecoded = 0xFFFD;
	return iResult;
}