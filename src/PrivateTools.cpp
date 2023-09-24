#include "include/PrivateTools.hpp"

#ifdef _WIN32
#include "rlTextDLL/Windows.h"
#endif

#ifdef _WIN32
std::wstring UTF8toUTF16(const char8_t *szUTF8) noexcept
{
	std::wstring sResult;
	const auto iRequiredBufSize = rlText_UTF8toUTF16(szUTF8, 0, 0);
	
	if (!iRequiredBufSize)
		return sResult;

	sResult.resize(iRequiredBufSize - 1);
	rlText_UTF8toUTF16(szUTF8, reinterpret_cast<char16_t *>(sResult.data()), iRequiredBufSize);

	return sResult;
}
#endif


bool ByteOrderFlipNecessary(rlText_Encoding iEndianFlag) noexcept
{
	return
		((bHostIsBigEndian && iEndianFlag == RLTEXT_FILEENCODING_FLAG_LITTLE_ENDIAN) ||
		 (!bHostIsBigEndian && iEndianFlag == RLTEXT_FILEENCODING_FLAG_BIG_ENDIAN));
}

char16_t FlipByteOrder(char16_t ch) noexcept
{
	return
		((ch >> 8) & 0x00FF) |
		((ch << 8) & 0xFF00);
}

char32_t FlipByteOrder(char32_t ch) noexcept
{
	return
		((ch >> 24) & 0x000000FF) |
		((ch >> 8 ) & 0x0000FF00) |
		((ch << 8 ) & 0x00FF0000) |
		((ch << 24) & 0xFF000000);
}

unsigned BOMLen(rlText_Encoding iEncoding) noexcept
{
	switch (iEncoding)
	{
	case RLTEXT_FILEENCODING_UTF8BOM:
		return 3;

	case RLTEXT_FILEENCODING_UTF16BE:
	case RLTEXT_FILEENCODING_UTF16LE:
		return 2;

	case RLTEXT_FILEENCODING_UTF32BE:
	case RLTEXT_FILEENCODING_UTF32LE:
		return 4;

	default:
		return 0;
	}
}
