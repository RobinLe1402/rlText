#include "rlText/Core.hpp"

int main(int argc, char* argv[])
{
	namespace Encoding = rlText::DefaultFileInfo;

	struct EncodingData
	{
		rlText::FileInfo fi;
		const char *szEncodingName;
		bool bUnicode;
	};

	constexpr EncodingData TestEncodings[] =
	{
		{ Encoding::ASCII,    "ASCII",     false },
		{ Encoding::Codepage, "Codepage",  false },
		{ Encoding::UTF8,     "UTF-8",     true  },
		{ Encoding::UTF8BOM,  "UTF-8 BOM", true  },
		{ Encoding::UTF16,    "UTF-16",    true  },
		{ Encoding::UTF16BE,  "UTF-16 BE", true  },
		{ Encoding::UTF16LE,  "UTF-16 LE", true  },
		{ Encoding::UTF32,    "UTF-32",    true  },
		{ Encoding::UTF32BE,  "UTF-32 BE", true  },
		{ Encoding::UTF32LE,  "UTF-32 LE", true  }
	};

	const std::vector<std::wstring> oTestText =
	{
		L"Hello Unicode!",
		L"\u00E4\u00F6\u00FC\u00DF", // lowercase Umlaute and Eszett
		L"\u00C4\u00D6\u00DC\u1E9E", // uppercase Umlaute and Eszett
		L"\U0001F600" // grinning face emoji
	};

	constexpr wchar_t szTestFilePath[] = LR"(E:\[TempDel]\test.txt)";

	for (const auto &enc : TestEncodings)
	{
		printf("Writing %s file...\n", enc.szEncodingName);
		if (!rlText::WriteTextFile(szTestFilePath, oTestText, enc.fi))
		{
			printf("  Write failed.\n\n");
			continue;
		}

		rlText::FileInfo fi{};
		rlText::GetFileInfo(szTestFilePath, fi);
		if (fi != enc.fi)
			printf("  Warning: Different format detected.\n");
		else
			printf("  Correct format detected.\n");

		std::vector<std::wstring> oReadStrings;
		rlText::ReadAllLines(szTestFilePath, oReadStrings, fi);
		bool bIdentical = true;
		for (int i = 0; i < oReadStrings.size(); ++i)
		{
			if (oTestText[i] != oReadStrings[i])
			{
				printf("  Contents were different from input ");
				if (!enc.bUnicode)
					printf("[encoding doesn't support Unicode --> OK]\n\n");
				else
					printf("[encoding supports unicode --> ERROR!]\n\n");

				bIdentical = false;
				break;
			}
		}
		if (bIdentical)
			printf("  Contents were identical to input --> OK\n\n");
	}

	return 0;
}
