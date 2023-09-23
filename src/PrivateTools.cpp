#include "include/PrivateTools.hpp"

#ifdef _WIN32
#include "rlTextDLL/Windows.hpp"
#endif

#ifdef _WIN32
std::wstring UTF8toUTF16(const char *szUTF8) noexcept
{
	std::wstring sResult;
	const auto iRequiredBufSize = rlText_UTF8toUTF16((const rlText_ByteChar *)szUTF8, 0, 0);
	
	if (!iRequiredBufSize)
		return sResult;

	sResult.resize(iRequiredBufSize - 1);
	rlText_UTF8toUTF16(
		(const rlText_ByteChar *)szUTF8,
		(rlText_UTF16Char *)sResult.data(),
		iRequiredBufSize);

	return sResult;
}
#endif


bool ByteOrderFlipNecessary(rlText_Encoding iEndianFlag) noexcept
{
	return
		((bHostIsBigEndian && iEndianFlag == RLTEXT_FILEENCODING_FLAG_LITTLE_ENDIAN) ||
		 (!bHostIsBigEndian && iEndianFlag == RLTEXT_FILEENCODING_FLAG_BIG_ENDIAN));
}

rlText_UTF16Char FlipByteOrder(rlText_UTF16Char ch) noexcept
{
	return (ch >> 8) | (ch << 8);
}

rlText_UTF32Char FlipByteOrder(rlText_UTF32Char ch) noexcept
{
	return
		((ch >> 24)) |
		((ch >> 8) & 0x0000FF00) |
		((ch << 8) & 0x00FF0000) |
		((ch << 24));
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
