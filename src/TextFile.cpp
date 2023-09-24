#include "include/TextFile.hpp"

#include "rlTextDLL/Decode.h"
#include "rlTextDLL/Encode.h"
#include "include/HostEndian.hpp"
#include "include/PrivateTools.hpp"

#include <fstream>
#include <functional>



namespace
{

	bool CheckEncodingID(rlText_Encoding &iEncoding, bool bDefaultToUTF8BOM) noexcept
	{
		// check if the encoding is valid, add BOM flag if needed
		switch (iEncoding)
		{
		case RLTEXT_FILEENCODING_ASCII:
		case RLTEXT_FILEENCODING_CP1252:
		case RLTEXT_FILEENCODING_UTF8:
		case RLTEXT_FILEENCODING_UTF8BOM:
		case RLTEXT_FILEENCODING_UTF16BE:
		case RLTEXT_FILEENCODING_UTF16LE:
		case RLTEXT_FILEENCODING_UTF32BE:
		case RLTEXT_FILEENCODING_UTF32LE:
			break; // OK

		case RLTEXT_FILEENCODING_UTF16:
		case RLTEXT_FILEENCODING_UTF32:
			// needs endian

			if constexpr (bHostIsBigEndian)
				iEncoding |= RLTEXT_FILEENCODING_FLAG_BIG_ENDIAN;
			else
				iEncoding |= RLTEXT_FILEENCODING_FLAG_LITTLE_ENDIAN;

			break;

		case 0:
			if (bDefaultToUTF8BOM)
			{
				iEncoding = RLTEXT_FILEENCODING_UTF8BOM;
				break;
			}

			[[fallthrough]];
		default:
			return false; // invalid encoder ID given
		}

		return true;
	}

	bool CheckLinebreakType(rlText_Linebreak iLinebreakType)
	{
		switch (iLinebreakType)
		{
		case RLTEXT_LINEBREAK_UNIX:
		case RLTEXT_LINEBREAK_WINDOWS:
		case RLTEXT_LINEBREAK_MACINTOSH:
			return true;
		default:
			return false;
		}
	}

	void AppendUTF8Codepoint(std::string &sDest, const rlText_UTF8Codepoint &cp)
	{
		char8_t sz[5] =
		{
			cp.ch[0],
			cp.ch[1],
			cp.ch[2],
			cp.ch[3],
			0
		};
		sDest += (const char *)sz;
	}

}



TextFile *TextFile::Open(const char8_t *szFilepath, rlText_Encoding iEncoding) noexcept
{
	if (!iEncoding)
	{
		if (!rlText_GetFileInfo(szFilepath, &iEncoding, nullptr, RLTEXT_GETFILEINFO_CHECKMINIMUM))
			return nullptr; // error on encoding detection
	}
	else
	{
		if (!CheckEncodingID(iEncoding, false))
			return nullptr;
	}



#ifdef _WIN32
	std::ifstream file(UTF8toUTF16(szFilepath), std::ios::binary);
#else
	std::ifstream file(szFilepath, std::ios::binary);
#endif

	if (!file)
		return nullptr;

	const auto iBOMLen = BOMLen(iEncoding);
	const size_t iFilesize = (size_t)file.seekg(0, std::ios::end).tellg() - iBOMLen;
	file.seekg(iBOMLen, std::ios::beg);

	if (!iFilesize) // file was empty
		return new TextFile{};

	auto up_oFileData = std::make_unique<uint8_t[]>(iFilesize);
	if (!up_oFileData.get())
		return nullptr; // some internal error (mostly to avoid "could be 0" warnings)
	file.read((char*)up_oFileData.get(), iFilesize);
	file.close();
	const union DataPtr
	{
		char     *pByte;
		char16_t *pU16;
		char32_t *pU32;
	} fileData = { reinterpret_cast<char *>(up_oFileData.get()) };


	const bool bFlipByteOrder =
		ByteOrderFlipNecessary(iEncoding & RLTEXT_FILEENCODING_MASK_FLAG_ENDIAN);

	// convert the entire file to a single UTF-8 string

	std::string sUTF8;
	sUTF8.reserve(iFilesize); // sufficient, it's very unlikely that UTF-8 > current encoding

	char32_t ch;
	size_t iRemainingBytes = iFilesize;
	rlText_UTF8Codepoint cp;
	switch (iEncoding)
	{
	case RLTEXT_FILEENCODING_ASCII:
	case RLTEXT_FILEENCODING_UTF8:
	case RLTEXT_FILEENCODING_UTF8BOM:
	{
		sUTF8.resize(iFilesize);
		memcpy_s(sUTF8.data(), sUTF8.length() + 1, fileData.pByte, iFilesize);
		break;
	}

	case RLTEXT_FILEENCODING_CP1252:
	{
		auto pCurrent = fileData.pByte;
		for (size_t i = 0; i < iFilesize; ++i)
		{
			rlText_DecodeCP1252(*pCurrent, &ch);
			rlText_EncodeUTF8(ch, &cp);

			AppendUTF8Codepoint(sUTF8, cp);
			++pCurrent;
		}
		break;
	}
		
	case RLTEXT_FILEENCODING_UTF16BE:
	case RLTEXT_FILEENCODING_UTF16LE:
	{
		const auto iCharCount = iFilesize / sizeof(char16_t);
		if (bFlipByteOrder)
		{
			for (size_t i = 0; i < iCharCount; ++i)
			{
				fileData.pU16[i] = FlipByteOrder(fileData.pU16[i]);
			}
		}

		auto p16 = fileData.pU16;
		for (size_t i = 0; i < iCharCount; ++i)
		{
			const auto iReadWords = rlText_DecodeUTF16(p16, &ch);
			if (!iReadWords)
				return nullptr; // encoding error in file

			rlText_EncodeUTF8(ch, &cp);

			AppendUTF8Codepoint(sUTF8, cp);
			p16 += iReadWords;
		}

		break;
	}
		
	case RLTEXT_FILEENCODING_UTF32BE:
	case RLTEXT_FILEENCODING_UTF32LE:
	{
		const auto iCharCount = iFilesize / sizeof(char32_t);
		if (bFlipByteOrder)
		{
			for (size_t i = 0; i < iCharCount; ++i)
			{
				fileData.pU32[i] = FlipByteOrder(fileData.pU32[i]);
			}
		}

		auto p32 = fileData.pU32;
		for (size_t i = 0; i < iCharCount; ++i)
		{
			rlText_EncodeUTF8(*p32, &cp);
			AppendUTF8Codepoint(sUTF8, cp);
			++p32;
		}

		break;
	}
		
	}

	// count the different linebreak types
	size_t iLinebreakCount[3]{};
	for (size_t i = 0; i < sUTF8.length(); ++i)
	{
		switch (sUTF8[i])
		{
		case '\n':
			++iLinebreakCount[RLTEXT_LINEBREAK_UNIX];
			break;

		case '\r':
			if (i + 1 < sUTF8.length() && sUTF8[i + 1] == '\n')
			{
				++iLinebreakCount[RLTEXT_LINEBREAK_WINDOWS];
				++i;
			}
			else
				++iLinebreakCount[RLTEXT_LINEBREAK_MACINTOSH];
			break;
		}

	}

	// determine the linebreak type to save
	// note: the lower indexes are preferred
	rlText_Linebreak iLinebreakType = 0;
	if (iLinebreakCount[0] >= iLinebreakCount[1])
	{
		if (iLinebreakCount[0] >= iLinebreakCount[2])
			iLinebreakType = 0;
		else
			iLinebreakType = 2;
	}
	else
	{
		if (iLinebreakCount[1] >= iLinebreakCount[2])
			iLinebreakType = 1;
		else
			iLinebreakType = 2;
	}

	auto pFile = new TextFile();
	pFile->setText(sUTF8.c_str());
	pFile->m_iLinebreakType = iLinebreakType;
	return pFile;
}

TextFile *TextFile::Create(rlText_Linebreak iLinebreakType) noexcept
{
	if (!CheckLinebreakType(iLinebreakType))
		return nullptr;

	TextFile *pResult = new TextFile{};
	pResult->m_iLinebreakType = iLinebreakType;
	return pResult;
}

bool TextFile::saveToFile(const char8_t *szFilepath, rlText_Encoding iEncoding,
	bool bTrailingLinebreak) const noexcept
{
	if (!CheckEncodingID(iEncoding, true))
		return false;

#ifdef _WIN32
	std::ofstream file(UTF8toUTF16(szFilepath).c_str(), std::ios::binary);
#else
	std::ofstream file(szFilepath, std::ios::binary);
#endif

	if (!file)
		return false;


	const auto sText = getAsText(bTrailingLinebreak);

	std::function<void(char32_t)> fnWriteChar;
	switch (iEncoding)
	{
	case RLTEXT_FILEENCODING_ASCII:
		fnWriteChar = [&](char32_t ch)
		{
			char chASCII;
			rlText_EncodeASCII(ch, &chASCII);
			file.write((const char*)&chASCII, 1);
		};
		break;

	case RLTEXT_FILEENCODING_CP1252:
		fnWriteChar = [&](char32_t ch)
		{
			char chCP1252;
			rlText_EncodeCP1252(ch, &chCP1252);
			file.write((const char*)&chCP1252, 1);
		};
		break;

	case RLTEXT_FILEENCODING_UTF8BOM:
		file.write("\xEF\xBB\xBF", 3);
		[[fallthrough]];
	case RLTEXT_FILEENCODING_UTF8:
		file.write(reinterpret_cast<const char *>(sText.c_str()), sText.length());
		return true;

	case RLTEXT_FILEENCODING_UTF16BE:
		fnWriteChar = [&](char32_t ch)
		{
			rlText_UTF16Codepoint cp;
			rlText_EncodeUTF16(ch, &cp);

			switch (cp.count)
			{
			case 2:
				if constexpr (!bHostIsBigEndian)
					cp.ch[1] = FlipByteOrder(cp.ch[1]);
				[[fallthrough]];
			case 1:
				if constexpr (!bHostIsBigEndian)
					cp.ch[0] = FlipByteOrder(cp.ch[0]);
			}

			file.write((const char*)cp.ch, cp.count * sizeof(char16_t));
		};
		break;

	case RLTEXT_FILEENCODING_UTF16LE:
		fnWriteChar = [&](char32_t ch)
		{
			rlText_UTF16Codepoint cp;
			rlText_EncodeUTF16(ch, &cp);

			switch (cp.count)
			{
			case 2:
				if constexpr (bHostIsBigEndian)
					cp.ch[1] = FlipByteOrder(cp.ch[1]);
				[[fallthrough]];
			case 1:
				if constexpr (bHostIsBigEndian)
					cp.ch[0] = FlipByteOrder(cp.ch[0]);
			}

			file.write((const char*)cp.ch, cp.count * sizeof(char16_t));
		};
		break;

	case RLTEXT_FILEENCODING_UTF32BE:
		fnWriteChar = [&](char32_t ch)
		{
			if constexpr (!bHostIsBigEndian)
				ch = FlipByteOrder(ch);
			file.write((const char*)&ch, sizeof(ch));
		};
		break;

	case RLTEXT_FILEENCODING_UTF32LE:
		fnWriteChar = [&](char32_t ch)
		{
			if constexpr (bHostIsBigEndian)
				ch = FlipByteOrder(ch);
			file.write((const char*)&ch, sizeof(ch));
		};
		break;
	}


	char32_t ch;
	for (size_t i = 0; i < sText.length(); ++i)
	{
		i += (size_t)rlText_DecodeUTF8(sText.c_str() + i, &ch) - 1;
		fnWriteChar(ch);
	}

	
	return true;
}

rlText_Count TextFile::setLine(rlText_Count iLine, const char *szLine, bool bReplace) noexcept
{
	if (bReplace && iLine >= m_oLines.size())
		return false;

	// check for a valid UTF-8 encoding
	auto p = szLine;
	for (; *p; ++p)
	{
		if ((*p & 0x80) == 0)
			continue;
		else if ((*p & 0xE0) == 0xC0) // two codepoints
		{
			if ((p[1] & 0xC0) != 0x80)
				return 0;

			p += 1;
		}
		else if ((*p & 0xF0) == 0xE0) // three codepoints
		{
			if ((p[1] & 0xC0) != 0x80 ||
				(p[2] & 0xC0) != 0x80)
				return 0;

			p += 2;
		}
		else if ((*p & 0xF8) == 0xF0) // four codepoints
		{
			if ((p[1] & 0xC0) != 0x80 ||
				(p[2] & 0xC0) != 0x80 ||
				(p[3] & 0xC0) != 0x80)
				return 0;

			p += 3;
		}
	}

	// count the lines
	p = szLine;
	size_t iCount = 1;
	for (; *p; ++p)
	{
		switch (*p)
		{
		case '\n':
			++iCount;
			break;

		case '\r':
			++iCount;
			if (p[1] == '\n')
				++p; // skip the \n of \r\n
		}
	}

	std::vector<std::u8string> oNewLines;
	oNewLines.reserve(iCount);

	p = szLine;
	while (*p)
	{
		std::u8string sLine;
		// count the characters in the line
		auto p2 = p;
		iCount  = 0;

		while (*p2 && *p2 != '\r' && *p2 != '\n')
		{
			++p2;
			++iCount;
		}

		sLine.reserve(iCount);
		for (; p < p2; ++p)
		{
			sLine += *p;
		}
		if (*p == '\r' && p[1] == '\n')
			++p; // skip the \n in \r\n

		// if the next character is a linebreak, skip it (otherwise it's the end of the string)
		if (*p)
			++p;

		oNewLines.push_back(std::move(sLine));
	}


	const rlText_Count iResult = oNewLines.size();

	if (bReplace)
	{
		m_oLines[iLine] = std::move(oNewLines[0]);

		for (size_t i = 1; i < oNewLines.size(); ++i)
		{
			m_oLines.insert(m_oLines.begin() + iLine + i, std::move(oNewLines[i]));
		}
	}
	else
	{
		if (iLine > m_oLines.size())
			iLine = m_oLines.size();
		for (size_t i = 0; i < oNewLines.size(); ++i)
		{
			m_oLines.insert(m_oLines.begin() + iLine + i, std::move(oNewLines[i]));
		}
	}

	return iResult;
}

bool TextFile::deleteLine(rlText_Count iLine) noexcept
{
	if (iLine >= m_oLines.size())
		return false;

	m_oLines.erase(m_oLines.begin() + iLine);
	return true;
}

bool TextFile::setLinebreakType(rlText_Linebreak iLinebreakType) noexcept
{
	if (!CheckLinebreakType(iLinebreakType))
		return false;

	m_iLinebreakType = iLinebreakType;
	return true;
}

rlText_Count TextFile::getTotalLength() const noexcept
{
	rlText_Count iLen = 1; // terminating zero

	for (const auto &sLine : m_oLines)
	{
		iLen += sLine.length() + getLinebreakLen();
	}
	if (iLen > 1)
		iLen -= getLinebreakLen();

	return iLen;
}

std::u8string TextFile::getAsText(bool bTrailingLinebreak) const noexcept
{
	std::u8string s;
	s.reserve(getTotalLength() - 1 + getLinebreakLen());

	for (size_t iLine = 0; iLine < m_oLines.size(); ++iLine)
	{
		s += m_oLines[iLine] + getLinebreak();
	}
	s.erase(s.length() - getLinebreakLen()); // remove trailing linebreak

	return s;
}

bool TextFile::setText(const char *sz) noexcept
{
	if (!*sz)
	{
		clear();
		return true;
	}

	TextFile oTemp;
	if (!oTemp.setLine(0, sz, false))
		return 0;

	*this = std::move(oTemp);
	return true;
}
