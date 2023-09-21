#include <rlTextDLL/Encode.h>
#include <rlTextDLL/Decode.h>

#include <memory.h>
#include <stdio.h>

unsigned PrintHex(unsigned long iVal, unsigned iDigits)
{
	unsigned iResult = 0;

	const char cHEX[] = "0123456789ABCDEF";
	for (unsigned i = 0; i < iDigits; ++i)
	{
		iResult += printf("%c", cHEX[(iVal >> ((iDigits - 1 - i) * 4)) & 0xF]);
	}

	return iResult;
}

unsigned PrintHex_8(rlText_ByteChar   c) { return PrintHex(c, 2); }
unsigned PrintHex_16(rlText_UTF16Char c) { return PrintHex(c, 4); }
unsigned PrintHex_32(rlText_UTF32Char c) { return PrintHex(c, 8); }

void PadWithSpaces(unsigned iCountOutOf12)
{
	if (iCountOutOf12 >= 12)
		return;
	for (unsigned i = iCountOutOf12; i < 12; ++i)
	{
		printf(" ");
	}
}

int test(rlText_UTF32Char ch, const char *szCharDescr)
{
	rlText_UTF32Char cDecoded;

	rlText_ByteChar       c;
	rlText_UTF8Codepoint  u8;
	rlText_UTF16Codepoint u16;
	int iResult = 1;
	char szUnicodeHex[9];

	sprintf_s(szUnicodeHex, 9, "%04X", ch);


	printf("Testing U+%s (%s)...\n", szUnicodeHex, szCharDescr);

	printf("  CP1252: ");
	if (!rlText_EncodeCP1252(ch, &c))
	{
		if (ch <= 0xFF && (ch < 0x80 || ch >= 0xA0))
		{
			iResult = 0;
			printf("failed.\n");
		}
		else
			printf("<outside supported range>\n");
	}
	else
	{
		//     "(X chars) "
		printf("          ");
		PadWithSpaces(PrintHex_8(c));

		if (!rlText_DecodeCP1252(c, &cDecoded) || cDecoded != ch)
		{
			iResult = 0;
			printf("(decoding failed)\n");
		}
		else
			printf("(decoding OK)\n");
	}

	printf("  UTF-8:  ");
	if (!rlText_EncodeUTF8(ch, &u8))
	{
		iResult = 0;
		printf("failed.\n");
	}
	else
	{
		printf("(%d char%s) ", u8.count, (u8.count == 1) ? " " : "s");
		unsigned iOutputChars = 0;
		for (unsigned i = 0; i < u8.count; ++i)
		{
			iOutputChars += PrintHex_8(u8.ch[i]);
			iOutputChars += printf(" ");
		}
		PadWithSpaces(iOutputChars);


		rlText_ByteChar u8Buffered[5];
		memset(u8Buffered, 0, sizeof(u8Buffered));
		memcpy_s(u8Buffered, sizeof(u8Buffered, u8), u8.ch, sizeof(u8.ch));

		if (!rlText_DecodeUTF8(u8Buffered, &cDecoded) || cDecoded != ch)
		{
			iResult = 0;
			printf("(decoding failed)\n");
		}
		else
			printf("(decoding OK)\n");
	}

	printf("  UTF-16: ");
	if (!rlText_EncodeUTF16(ch, &u16))
	{
		iResult = 0;
		printf("failed.\n");
	}
	else
	{
		printf("(%d char%s) ", u16.count, (u16.count == 1) ? " " : "s");
		unsigned iOutputChars = 0;
		for (unsigned i = 0; i < u16.count; ++i)
		{
			iOutputChars += PrintHex_16(u16.ch[i]);
			iOutputChars += printf(" ");
		}
		PadWithSpaces(iOutputChars);

		rlText_UTF16Char u16Buffered[3];
		memset(u16Buffered, 0, sizeof(u16Buffered));
		memcpy_s(u16Buffered, sizeof(u16Buffered, u16), u16.ch, sizeof(u16.ch));

		if (!rlText_DecodeUTF16(u16Buffered, &cDecoded) || cDecoded != ch)
		{
			iResult = 0;
			printf("(decoding failed)\n");
		}
		else
			printf("(decoding OK)\n");
	}

	printf("\n");

	return iResult;
}

int main(int argc, char* argv[])
{
	if (!test(U'\U000000F6', "o-Umlaut") ||
		!test(U'\U00002122', "TM sign") ||
		!test(U'\U00002764', "Heart") ||
		!test(U'\U0001F642', "Emoji \"Slightly Smiling Face\""))
		printf("\nFAIL.\n");
	else
		printf("\nSUCCESS.\n");

	return 0;
}
