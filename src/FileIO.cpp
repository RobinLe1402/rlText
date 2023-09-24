#include "rlTextDLL/FileIO.h"

#include "rlTextDLL/Decode.h"
#include "rlTextDLL/Encode.h"
#include "rlTextDLL/Unicode.h"
#include "rlTextDLL/Windows.h"
#include "include/HostEndian.hpp"
#include "include/PrivateTools.hpp"
#include "include/TextFile.hpp"

#include <fstream>
#include <functional>



unsigned ReadUpToXBytesFromFile(std::ifstream &file, size_t iFileSize,
	unsigned iBytesToRead, void *pDest)
{
	iBytesToRead = (unsigned)std::min<size_t>(iBytesToRead, iFileSize - file.tellg());
	
	file.read(reinterpret_cast<char*>(pDest), iBytesToRead);

	return iBytesToRead;
}

char32_t ReadCharFromFile(std::ifstream &file, unsigned iEncoding, bool bFlipByteOrder,
	size_t iFileSize)
{
	char32_t chResult         = 0;
	int      iUnusedReadBytes = 0;

	switch (iEncoding & RLTEXT_FILEENCODING_MASK_ENCODING)
	{
	case RLTEXT_FILEENCODING_ASCII:
	{
		// CP1252
		if (iEncoding & RLTEXT_FILEENCODING_FLAG_NONSTANDARD)
		{
			char c;
			file.read(&c, 1);
			rlText_DecodeCP1252(c, &chResult);
			break; // case
		}

		// ASCII
		else
		{
			char c;
			file.read(&c, 1);
			return c;
		}
	}

	case RLTEXT_FILEENCODING_UTF8:
	{
		char c[4]{};
		const int iRead  = ReadUpToXBytesFromFile(file, iFileSize, 4, c);
		iUnusedReadBytes = iRead - rlText_DecodeUTF8(c, &chResult);
		break;
	}

	case RLTEXT_FILEENCODING_UTF16:
	{
		char16_t chBuf[2]{};
		const int iRead = (int)ReadUpToXBytesFromFile(file, iFileSize, sizeof(chBuf), chBuf);
		if (bFlipByteOrder)
		{
			chBuf[0] = FlipByteOrder(chBuf[0]);
			chBuf[1] = FlipByteOrder(chBuf[1]);
		}
		iUnusedReadBytes = iRead - rlText_DecodeUTF16(chBuf, &chResult) * sizeof(char16_t);
		break;
	}
		
	case RLTEXT_FILEENCODING_UTF32:
	{
		file.read(reinterpret_cast<char*>(&chResult), sizeof(chResult));
		if (bFlipByteOrder)
			chResult = FlipByteOrder(chResult);
		break;
	}
	}

	if (iUnusedReadBytes)
	{
		file.clear();
		file.seekg(-iUnusedReadBytes, std::ios::cur);
	}

	return chResult;
}

RLTEXT_API rlText_Bool RLTEXT_LIB rlText_GetFileInfo(
	const char                  *szFilepath,
	rlText_Encoding             *pEncoding,
	rlText_FileStatisticsStruct *pStatistics,
	rlText_Flags1                iFlags
)
{
	if (pStatistics)
		memset(pStatistics, 0, sizeof(*pStatistics));

#ifdef _WIN32
	// convert filename from UTF-8 to UTF-16
	const auto iReqSize = rlText_UTF8toUTF16(szFilepath, 0, 0);
	auto up_szUTF16     = std::make_unique<char16_t[]>(iReqSize);
	rlText_UTF8toUTF16(szFilepath, up_szUTF16.get(), iReqSize);

	std::ifstream file((wchar_t *)up_szUTF16.get(), std::ios::binary);
#else
	std::ifstream file(szFilepath, std::ios::binary);
#endif

	if (!file)
		return false;

	rlText_Encoding iEncoding = 0;

	const size_t iFileSize = file.seekg(0, std::ios::end).tellg();
	file.clear();
	file.seekg(0, std::ios::beg);


	const bool bAssumeUTF8     = iFlags & RLTEXT_GETFILEINFO_ASSUMEUTF8;
	const bool bCheckWholeFile = pStatistics || iFlags & RLTEXT_GETFILEINFO_CHECKWHOLEFILE;



	char cBOM[5]{};
	file.clear();
	file.read(cBOM, 4);
	
	bool bBOM = false;

	if ((iFileSize % 4) == 0)
	{
		if (memcmp(cBOM, "\xFF\xFE\x00\x00", 4) == 0)
		{
			iEncoding = RLTEXT_FILEENCODING_UTF32LE;
			bBOM      = true;
		}
		else if (memcmp(cBOM, "\x00\x00\xFE\xFF", 4) == 0)
		{
			iEncoding = RLTEXT_FILEENCODING_UTF32BE;
			bBOM      = true;
		}
	}
	if (!bBOM && (iFileSize % 2) == 0)
	{
		if (memcmp(cBOM, "\xFF\xFE", 2) == 0)
		{
			iEncoding = RLTEXT_FILEENCODING_UTF16LE;
			bBOM      = true;
		}
		else if (memcmp(cBOM, "\xFE\xFF", 2) == 0)
		{
			iEncoding = RLTEXT_FILEENCODING_UTF16BE;
			bBOM      = true;
		}
	}
	if (!bBOM && memcmp(cBOM, "\xEF\xBB\xBF", 3) == 0)
	{
		iEncoding = RLTEXT_FILEENCODING_UTF8BOM;
		bBOM      = true;
	}

	if (!bBOM && !bCheckWholeFile && bAssumeUTF8)
	{
		if (pEncoding)
			*pEncoding = RLTEXT_FILEENCODING_UTF8;
		return true;
	}

	if (bBOM && !bCheckWholeFile)
	{
		if (pEncoding)
			*pEncoding = iEncoding;
		return true;
	}



	// now check the whole file
	file.clear();

	// check if file is valid in encoding indicated by BOM
	if (bBOM)
	{
		const bool bFlipByteOrder =
			ByteOrderFlipNecessary(iEncoding & RLTEXT_FILEENCODING_MASK_FLAG_ENDIAN);

		const auto iEncID = iEncoding & RLTEXT_FILEENCODING_MASK_ENCODING;

		// go back to right after BOM
		file.clear();
		file.seekg(BOMLen(iEncoding), std::ios::beg);


		// Here's a compatibility list between BOMs and encodings without a mandatory BOM.
		// 
		//  BOM -> |   UTF-8   | UTF-16 LE | UTF-16 BE | UTF-32 LE | UTF-32 BE |
		// --------+-----------+-----------+-----------+-----------+-----------+
		//  ASCII  |   error   |   error   |   error   |   error   |   error   |
		//  CP1252 |    OK     |    OK     |    OK     |    OK     |    OK     |
		//  UTF-8  |     -     |   error   |   error   |   error   |   error   |
		// --------+-----------+-----------+-----------+-----------+-----------+
		// 
		// => if there's a BOM, the file can only be the encoding indicated by the BOM, or Codepage
		//    1252, as it basically is always valid, even when provided with binary data.
		

		// check the entire file for illegal characters

		char oBuf[5]{}; // maximum byte count per codepoint is 4
		std::function<unsigned(const char *)> fnDecode;
		char32_t chDummy = 0;

		// assign a decoding lambda
		switch (iEncID)
		{
		case RLTEXT_FILEENCODING_UTF8:
			fnDecode = [&](const char *pBuf) -> unsigned
			{
				return rlText_DecodeUTF8(pBuf, &chDummy);
			};
			break;
			
		case RLTEXT_FILEENCODING_UTF16:
			fnDecode = [&](const char *pBuf) -> unsigned
			{
				char16_t chBuf[2]{};
				if (!bFlipByteOrder)
				{
					chBuf[0] = (char16_t(pBuf[0]) << 8) | pBuf[1];
					chBuf[1] = (char16_t(pBuf[2]) << 8) | pBuf[3];
				}
				else
				{
					chBuf[0] = (char16_t(pBuf[1]) << 8) | pBuf[0];
					chBuf[1] = (char16_t(pBuf[3]) << 8) | pBuf[2];
				}

				return rlText_DecodeUTF16(chBuf, &chDummy) * sizeof(char16_t);
			};
			break;

		case RLTEXT_FILEENCODING_UTF32:
			fnDecode = [&](const char *pBuf) -> unsigned
			{
				char32_t ch;

				if (!bFlipByteOrder)
					ch = *reinterpret_cast<const char32_t *>(pBuf);
				else
					ch = FlipByteOrder(*reinterpret_cast<const char32_t *>(pBuf));

				if (rlText_IsNoncharacter(ch))
					return 0;
				else
					return sizeof(char32_t);
			};
			break;

		default:
			return false; // invalid encoding (bug)
		}

		unsigned iValidChars = 0;
		while (true)
		{
			if (iValidChars < 4)
			{
				iValidChars += ReadUpToXBytesFromFile(file, iFileSize, (size_t)4 - iValidChars,
					oBuf + iValidChars);

				if (iValidChars == 0)
					break; // while: EOF reached

				if (iValidChars < 4)
				{
					memset(oBuf + iValidChars, 0, (size_t)4 - iValidChars);
				}
			}

			const auto iReadBytes = fnDecode(oBuf);
			if (!iReadBytes)
			{
				iEncoding = RLTEXT_FILEENCODING_CP1252;
				break; // while
			}
			else if (file.eof() || file.peek() == EOF)
				break; // while

			iValidChars -= iReadBytes;

			// move valid bytes to front of buffer
			switch (iReadBytes)
			{
			case 1:
				oBuf[0] = oBuf[1];
				oBuf[1] = oBuf[2];
				oBuf[2] = oBuf[3];
				break;

			case 2:
				oBuf[0] = oBuf[2];
				oBuf[1] = oBuf[3];
				break;

			case 3:
				oBuf[0] = oBuf[3];
				break;
			}
		}
	}

	// no BOM found --> check for ASCII and UTF-8 compatibility
	else
	{
		file.seekg(0, std::ios::beg);

		bool bNonASCII  = false;
		bool bValidUTF8 = true;
		char32_t ch     = 0;

		while (!file.eof() && file.peek() != EOF)
		{
			char c[4]{};
			file.read(c, 1);
			if (c[0] & 0x80)
			{
				bNonASCII = true;
				if ((c[0] & 0x40) == 0) // 0b10XX XXXX --> invalid UTF-8
				{
					bValidUTF8 = false;
				}
				else if ((c[0] & 0x20) == 0) // 0b110X XXXX --> two UTF-8 codeunits
				{
					bValidUTF8 =
						!file.read(c + 1, 1).eof() &&
						(c[1] & 0xC0) == 0x80 &&
						!rlText_IsNoncharacter(
							(char32_t(c[0] & 0x1F) << 6) |
							         (c[1] & 0x3F)
						);
				}
				else if ((c[0] & 0x10) == 0) // 0b1110 XXXX --> three UTF-8 codeunits
				{
					bValidUTF8 =
						!file.read(c + 1, 2).eof() &&
						(c[1] & 0xC0) == 0x80 &&
						(c[2] & 0xC0) == 0x80 &&
						!rlText_IsNoncharacter(
							(char32_t(c[0] & 0x0F) << 12) |
							(char32_t(c[1] & 0x3F) << 6 ) |
							          c[2] & 0x3F
						);
				}
				else if ((c[0] & 0x08) == 0) // 0b1111 0XXX --> four UTF-8 codeunits
				{
					bValidUTF8 =
						!file.read(c + 1, 3).eof() &&
						(c[1] & 0xC0) == 0x80 &&
						(c[2] & 0xC0) == 0x80 &&
						(c[3] & 0xC0) == 0x80 &&
						!rlText_IsNoncharacter(
							(char32_t(c[0] & 0x07) << 18) |
							(char32_t(c[1] & 0x3F) << 12) |
							(char32_t(c[2] & 0x3F) << 6 ) |
							          c[3] & 0x3F
						);
				}
				else
					bValidUTF8 = false;

				if (bNonASCII && !bValidUTF8)
					break; // while: must be CP1252
			}
		}

		if (!bNonASCII)
			iEncoding = RLTEXT_FILEENCODING_ASCII;
		else if (bValidUTF8)
			iEncoding = RLTEXT_FILEENCODING_UTF8;
		else
			iEncoding = RLTEXT_FILEENCODING_CP1252;
	}



	// collect statistics
	if (pStatistics)
	{
		file.clear();
		file.seekg(BOMLen(iEncoding), std::ios::beg);

		char32_t chCur = 0;
		char32_t chPrv = 0;
		const bool bFlipByteOrder =
			ByteOrderFlipNecessary(iEncoding & RLTEXT_FILEENCODING_MASK_FLAG_ENDIAN);

		while (file.peek() != EOF)
		{
			++pStatistics->iCharCount;
			chCur = ReadCharFromFile(file, iEncoding, bFlipByteOrder, iFileSize);

			switch (chCur)
			{
			case '\n':
				if (chPrv == '\r')
				{
					--pStatistics->iLineBreakCount[RLTEXT_LINEBREAK_MACINTOSH];
					++pStatistics->iLineBreakCount[RLTEXT_LINEBREAK_WINDOWS];
				}
				else
					++pStatistics->iLineBreakCount[RLTEXT_LINEBREAK_UNIX];
				break;

			case '\r':
				++pStatistics->iLineBreakCount[RLTEXT_LINEBREAK_MACINTOSH];
				break;
			}

			chPrv = chCur;
		}
	}



	if (pEncoding)
		*pEncoding = iEncoding;
	return true;
}



RLTEXT_API rlText_File RLTEXT_LIB rlText_FileOpen(
	const char     *szFilepath,
	rlText_Encoding iEncoding
)
{
	return (rlText_File)TextFile::Open(szFilepath, iEncoding);
}

RLTEXT_API rlText_File RLTEXT_LIB rlText_FileCreate(rlText_Linebreak iLinebreakStyle)
{
	return (rlText_File)TextFile::Create(iLinebreakStyle);
}

RLTEXT_API rlText_Bool RLTEXT_LIB rlText_FileSave(
	rlText_File     oFile,
	const char     *szFilepath,
	rlText_Encoding iEncoding,
	rlText_Bool     bTrailingLinebreak
)
{
	const auto &obj = *(TextFile*)oFile;
	return obj.saveToFile(szFilepath, iEncoding, bTrailingLinebreak);
}

RLTEXT_API void RLTEXT_LIB rlText_FileFree(rlText_File oFile)
{
	delete (TextFile*)oFile;
}

RLTEXT_API rlText_Count RLTEXT_LIB rlText_FileGetLineCount(rlText_File oFile)
{
	return (*(const TextFile*)oFile).getLineCount();
}

RLTEXT_API rlText_Count RLTEXT_LIB rlText_FileGetLine(
	rlText_File  oFile,
	rlText_Count iLine,
	char        *pBuf,
	rlText_Count iBufSize
)
{
	const auto &obj = *(const TextFile*)oFile;

	if (iLine >= obj.getLineCount())
		return 0;

	const auto& sLine = obj.getLine(iLine);

	const auto iStrSize = sLine.length() + 1;

	if (!pBuf || !iBufSize)
		return iStrSize;

	if (iBufSize < iStrSize)
		return 0;

	memcpy_s(pBuf, iBufSize, sLine.c_str(), iStrSize);
	return iStrSize;
}

RLTEXT_API rlText_Count RLTEXT_LIB rlText_FileSetLine(
	rlText_File  oFile,
	rlText_Count iLine,
	const char  *szLine,
	rlText_Bool  bReplace
)
{
	return (*(TextFile*)oFile).setLine(iLine, szLine, bReplace);
}

RLTEXT_API rlText_Bool RLTEXT_LIB rlText_FileDeleteLine(
	rlText_File  oFile,
	rlText_Count iLine
)
{
	return (*(TextFile*)oFile).deleteLine(iLine);
}

RLTEXT_API rlText_Linebreak RLTEXT_LIB rlText_FileGetLinebreakType(rlText_File oFile)
{
	return (*(TextFile*)oFile).getLinebreakType();
}

RLTEXT_API rlText_Bool RLTEXT_LIB rlText_FileSetLinebreakType(
	rlText_File      oFile,
	rlText_Linebreak iLinebreakType
)
{
	return (*(TextFile*)oFile).setLinebreakType(iLinebreakType);
}

RLTEXT_API rlText_Count RLTEXT_LIB rlText_FileGetAsSingleString(
	rlText_File  oFile,
	char        *pBuf,
	rlText_Count iBufSize
)
{
	auto &obj = *(TextFile*)oFile;

	auto iTotalLength = obj.getTotalLength();

	if (!pBuf || !iBufSize)
		return iTotalLength;

	if (iBufSize < iTotalLength)
		return 0;


	auto sText = obj.getAsText();
	memcpy_s(pBuf, iBufSize, sText.c_str(), iTotalLength);
	return iTotalLength;
}

RLTEXT_API rlText_Bool RLTEXT_LIB rlText_FileSetAsSingleString(
	rlText_File oFile,
	const char *sz
)
{
	return (*(TextFile*)oFile).setText(sz);
}

RLTEXT_API void RLTEXT_LIB rlText_FileClear(
	rlText_File oFile
)
{
	return (*(TextFile*)oFile).clear();
}
