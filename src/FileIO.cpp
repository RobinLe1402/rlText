#include "rlTextDLL/FileIO.h"

#include "rlTextDLL/Decode.h"
#include "rlTextDLL/Encode.h"
#include "rlTextDLL/Unicode.h"
#include "include/HostEndian.hpp"

#include <fstream>
#include <functional>



rlText_UTF16Char FlipByteOrder(rlText_UTF16Char ch)
{
	return (ch >> 8) | (ch << 8);
}

rlText_UTF32Char FlipByteOrder(rlText_UTF32Char ch)
{
	return
		((ch >> 24)) |
		((ch >> 8 ) & 0x0000FF00) |
		((ch << 8 ) & 0x00FF0000) |
		((ch << 24));
}



unsigned ReadUpToXBytesFromFile(std::ifstream &file, size_t iFileSize,
	unsigned iBytesToRead, void *pDest)
{
	iBytesToRead = (unsigned)std::min<size_t>(iBytesToRead, iFileSize - file.tellg());
	
	file.read(reinterpret_cast<char*>(pDest), iBytesToRead);

	return iBytesToRead;
}

rlText_UTF32Char ReadCharFromFile(std::ifstream &file, unsigned iEncoding, bool bFlipByteOrder,
	size_t iFileSize)
{
	rlText_UTF32Char chResult = 0;
	int iUnusedReadBytes      = 0;

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
		const int iRead = ReadUpToXBytesFromFile(file, iFileSize, 4, c);
		iUnusedReadBytes = iRead -
			rlText_DecodeUTF8(reinterpret_cast<const rlText_ByteChar *>(c), &chResult);
		break;
	}

	case RLTEXT_FILEENCODING_UTF16:
	{
		rlText_UTF16Char chBuf[2]{};
		const int iRead = (int)ReadUpToXBytesFromFile(file, iFileSize, sizeof(chBuf), chBuf);
		if (bFlipByteOrder)
		{
			chBuf[0] = FlipByteOrder(chBuf[0]);
			chBuf[1] = FlipByteOrder(chBuf[1]);
		}
		iUnusedReadBytes = iRead - rlText_DecodeUTF16(chBuf, &chResult) * sizeof(rlText_UTF16Char);
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

RLTEXT_API rlText_Bool EXPORT rlText_GetFileInfo(
	const wchar_t               *szFilepath,
	uint8_t                     *pEncoding,
	rlText_FileStatisticsStruct *pStatistics,
	uint8_t                      iFlags
)
{
	if (pStatistics)
		memset(pStatistics, 0, sizeof(*pStatistics));

	std::ifstream file(szFilepath, std::ios::binary);
	if (!file)
		return false;

	uint8_t iEncoding = 0;

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
			(bHostIsBigEndian && (iEncoding & RLTEXT_FILEENCODING_FLAG_LITTLE_ENDIAN)) ||
			(!bHostIsBigEndian && (iEncoding & RLTEXT_FILEENCODING_FLAG_BIG_ENDIAN));

		const auto iEncID = iEncoding & RLTEXT_FILEENCODING_MASK_ENCODING;

		// go back to right after BOM
		switch (iEncID)
		{
		case RLTEXT_FILEENCODING_UTF32:
			break;
		case RLTEXT_FILEENCODING_UTF8:
			file.seekg(3, std::ios::beg);
			break;
		case RLTEXT_FILEENCODING_UTF16:
			file.seekg(2, std::ios::beg);
			break;
		default:
			return false; // unknown BOM (bug)
		}

		file.clear(std::ios::eofbit);


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

		rlText_ByteChar oBuf[5]{}; // maximum byte count per codepoint is 4
		std::function<unsigned(const rlText_ByteChar *)> fnDecode;
		rlText_UTF32Char chDummy = 0;

		// assign a decoding lambda
		switch (iEncID)
		{
		case RLTEXT_FILEENCODING_UTF8:
			fnDecode = [&](const rlText_ByteChar *pBuf) -> unsigned
			{
				return rlText_DecodeUTF8(pBuf, &chDummy);
			};
			break;
			
		case RLTEXT_FILEENCODING_UTF16:
			fnDecode = [&](const rlText_ByteChar *pBuf) -> unsigned
			{
				rlText_UTF16Char chBuf[2]{};
				if (!bFlipByteOrder)
				{
					chBuf[0] = (rlText_UTF16Char(pBuf[0]) << 8) | pBuf[1];
					chBuf[1] = (rlText_UTF16Char(pBuf[2]) << 8) | pBuf[3];
				}
				else
				{
					chBuf[0] = (rlText_UTF16Char(pBuf[1]) << 8) | pBuf[0];
					chBuf[1] = (rlText_UTF16Char(pBuf[3]) << 8) | pBuf[2];
				}

				return rlText_DecodeUTF16(chBuf, &chDummy) * sizeof(rlText_UTF16Char);
			};
			break;

		case RLTEXT_FILEENCODING_UTF32:
			fnDecode = [&](const rlText_ByteChar *pBuf) -> unsigned
			{
				rlText_UTF32Char ch;

				if (!bFlipByteOrder)
					ch =
						(rlText_UTF32Char(pBuf[0]) << 24) |
						(rlText_UTF32Char(pBuf[1]) << 16) |
						(rlText_UTF32Char(pBuf[2]) << 8 ) |
						                  pBuf[3];
				else
					ch =
						(rlText_UTF32Char(pBuf[3]) << 24) |
						(rlText_UTF32Char(pBuf[2]) << 16) |
						(rlText_UTF32Char(pBuf[1]) << 8)  |
						                  pBuf[0];

				if (rlText_IsNoncharacter(ch))
					return 0;
				else
					return sizeof(rlText_UTF32Char);
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
		rlText_UTF32Char ch = 0;

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
							(rlText_UTF32Char(c[0] & 0x1F) << 6) |
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
							(rlText_UTF32Char(c[0] & 0x0F) << 12) |
							(rlText_UTF32Char(c[1] & 0x3F) << 6) |
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
							(rlText_UTF32Char(c[0] & 0x07) << 18) |
							(rlText_UTF32Char(c[1] & 0x3F) << 12) |
							(rlText_UTF32Char(c[2] & 0x3F) << 6) |
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

		// if there's a BOM, go back to right after the BOM.
		// otherwise, go back to the start of the file.
		switch (iEncoding & RLTEXT_FILEENCODING_MASK_ENCODING)
		{
		case RLTEXT_FILEENCODING_UTF16:
			file.seekg(2, std::ios::beg);
			break;

		case RLTEXT_FILEENCODING_UTF32:
			file.seekg(4, std::ios::beg);
			break;

		case RLTEXT_FILEENCODING_UTF8:
			if (iEncoding & RLTEXT_FILEENCODING_FLAG_BOM)
			{
				file.seekg(3, std::ios::beg);
				break;
			}

			[[fallthrough]];

		default:
			file.seekg(0, std::ios::beg);
		}

		rlText_UTF32Char chCur = 0;
		rlText_UTF32Char chPrv = 0;
		const bool bFlipByteOrder =
			(bHostIsBigEndian && (iEncoding & RLTEXT_FILEENCODING_FLAG_LITTLE_ENDIAN)) ||
			(!bHostIsBigEndian && (iEncoding & RLTEXT_FILEENCODING_FLAG_BIG_ENDIAN));
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