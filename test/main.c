#include <rlTextDLL/Encode.h>

#include <memory.h>
#include <stdio.h>

void PrintHex(unsigned long iVal, unsigned iDigits)
{
	const char cHEX[] = "0123456789ABCDEF";
	for (unsigned i = 0; i < iDigits; ++i)
	{
		printf("%c", cHEX[(iVal >> ((iDigits - 1 - i) * 4)) & 0xF]);
	}
}

void PrintHex_8(rlText_ByteChar   c) { PrintHex(c, 2); }
void PrintHex_16(rlText_UTF16Char c) { PrintHex(c, 4); }
void PrintHex_32(rlText_UTF32Char c) { PrintHex(c, 8); }

int test(rlText_UTF32Char ch, const char *szCharDescr)
{
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
		PrintHex_8(c);
		printf("\n");
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
		for (unsigned i = 0; i < u8.count; ++i)
		{
			PrintHex_8(u8.ch[i]);
			printf(" ");
		}
		printf("\n");
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
		for (unsigned i = 0; i < u16.count; ++i)
		{
			PrintHex_16(u16.ch[i]);
			printf(" ");
		}
		printf("\n");
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
