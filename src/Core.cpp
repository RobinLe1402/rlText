#include "rlText/Core.hpp"

#include "rlText/FileReader.hpp"
#include "rlText/FileWriter.hpp"
#include "rlText/Unicode.hpp"

#include <fstream>

namespace rlText
{

	bool GetFileInfo(const wchar_t* szFilePath, FileInfo& oDest, unsigned iFlags) noexcept
	{
#define FILE_EOF() (file.eof() || file.peek() == EOF)

		oDest = {};

		std::basic_ifstream<uint8_t> file(szFilePath, std::ios::binary);
		if (!file)
			return false; // couldn't open file

		if (FILE_EOF()) // file empty --> assume ASCII with OS linebreaks
		{
			oDest.eEncoding = Encoding::ASCII;
			return true;
		}





		//------------------------------------------------------------------------------------------
		// CHECK BOM

		uint8_t iByte = 0;

		// read the first byte
		file.read(&iByte, 1);

		if (FILE_EOF())
		{
			// only ASCII (part of UTF-8) and codepages support single-byte characters.

			if (iByte & 0x80)
				oDest.eEncoding = Encoding::Codepage;
			else
				oDest.eEncoding = Encoding::ASCII;

			return true;
		}

		/*
		  BOM POSSIBILITIES:

		  0xFE 0xFF           --> UTF-16 BE
		  0xEF 0xBB 0xBF      --> UTF-8 BOM
		  0xFF 0xFE 0x00 0x00 --> UTF-32 LE
		  0xFF 0xFE           --> UTF-16 LE
		  0x00 0x00 0xFE 0xFF --> UTF-32 BE
		*/

		switch (iByte)
		{
		case 0xFE: // UTF-16 BE?
			file.read(&iByte, 1);
			if (iByte == 0xFF)
			{
				oDest.bBOM      = true;
				oDest.eEncoding = Encoding::UTF16;
				oDest.eEndian   = Endian::BigEndian;
			}
			else
				oDest.eEncoding = Encoding::Codepage;
			break;
		case 0xEF:
			file.read(&iByte, 1);
			if (iByte == 0xBB)
			{
				if (FILE_EOF())
				{
					oDest.eEncoding = Encoding::Codepage;
					break;
				}
				file.read(&iByte, 1);
				if (iByte == 0xBF)
				{
					oDest.bBOM      = true;
					oDest.eEncoding = Encoding::UTF8;
					oDest.eEndian   = Endian::OS;
				}
				else
					oDest.eEncoding = Encoding::Codepage;
			}
			else
				oDest.eEncoding = Encoding::Codepage;
			break;
		case 0xFF:
			file.read(&iByte, 1);
			if (FILE_EOF() || iByte != 0xFE)
			{
				oDest.eEncoding = Encoding::Codepage;
				break;
			}

			// might be UTF-16 LE or UTF-32 LE

			if (FILE_EOF())
			{
				oDest.bBOM      = true;
				oDest.eEncoding = Encoding::UTF16;
				oDest.eEndian   = Endian::LittleEndian;
				return true; // whole file was already checked anyways
			}
			file.read(&iByte, 1);
			if (FILE_EOF())
			{
				// The file was 3 bytes long.
				// Neither UTF-16 nore UTF-32 support single-byte characters
				// --> must be a codepage
				oDest.eEncoding = Encoding::Codepage;
				return true; // whole file was already checked anyways
			}
			if (iByte != 0x00)
			{
				oDest.bBOM      = true;
				oDest.eEncoding = Encoding::UTF16;
				oDest.eEndian   = Endian::LittleEndian;
				break;
			}
			file.read(&iByte, 1);
			if (iByte != 0x00)
			{
				oDest.bBOM      = true;
				oDest.eEncoding = Encoding::UTF16;
				oDest.eEndian   = Endian::LittleEndian;
				break;
			}
			oDest.bBOM      = true;
			oDest.eEncoding = Encoding::UTF32;
			oDest.eEndian   = Endian::LittleEndian;
			break;
		case 0x00:
			// only valid BOM would be UTF-32 BE
			file.read(&iByte, 1);
			if (FILE_EOF() || iByte != 0x00)
			{
				oDest.eEncoding = Encoding::Codepage;
				break;
			}
			file.read(&iByte, 1);
			if (FILE_EOF() || iByte != 0xFE)
			{
				oDest.eEncoding = Encoding::Codepage;
				break;
			}
			file.read(&iByte, 1);
			if (iByte != 0xFF)
			{
				oDest.eEncoding = Encoding::Codepage;
				break;
			}
			oDest.bBOM      = true;
			oDest.eEncoding = Encoding::UTF32;
			oDest.eEndian   = Endian::BigEndian;
			break;
		default:
			// ASCII as placeholder --> check later if ASCII, UTF-8 or Codepage
			oDest.eEncoding = Encoding::ASCII;
		}





		//------------------------------------------------------------------------------------------
		// CHECK FILESIZE RESTRICTIONS

		file.seekg(0, std::ios::end);
		const size_t iFilesize = file.tellg();

		switch (oDest.eEncoding)
		{
		case Encoding::UTF32:
			if (iFilesize % 4)
			{
				if (iFilesize % 2 || oDest.eEndian == Endian::BigEndian)
					oDest.eEncoding = Encoding::Codepage;
				else
					oDest.eEncoding = Encoding::UTF16; // might be UTF-16 LE instead
			}
			break;
		case Encoding::UTF16:
			if (iFilesize % 2)
				oDest.eEncoding = Encoding::Codepage;
			break;
		}





		if (oDest.eEncoding != Encoding::ASCII && (iFlags & GFI_MINCHECK))
			return true;





		//------------------------------------------------------------------------------------------
		// CHECK FILE CONTENTS (FORMALLY)

		// skip BOM
		uint8_t lenBOM;
		switch (oDest.eEncoding)
		{
		case Encoding::UTF8:
			lenBOM = 3;
			break;
		case Encoding::UTF16:
			lenBOM = 2;
			break;
		case Encoding::UTF32:
			lenBOM = 4;
			break;

		default:
			lenBOM = 0;
		}
		file.clear();
		file.seekg(lenBOM);
		if (FILE_EOF())
			return true; // nothing more to check


		// check if the endian must be swapped
		const bool bSwapEndian = oDest.eEndian != Endian::OS;


		// check file contents
		// if a codepage was detected, the contents are valid anyways
		switch (oDest.eEncoding)
		{
		case Encoding::ASCII:
			while (!FILE_EOF())
			{
				const auto pos = file.tellg();
				file.read(&iByte, 1);
				if (iByte & 0x80)
				{
					if (FILE_EOF())
					{
						oDest.eEncoding = Encoding::Codepage;
						break; // while
					}
					oDest.eEncoding = Encoding::UTF8;
					file.clear();
					file.seekg(pos);
					break; // while
				}
			}
			if (oDest.eEncoding != Encoding::UTF8)
				break;
			[[fallthrough]]; // fallthrough: check if UTF-8
		case Encoding::UTF8:
		{
			uint8_t iRemainingBytes = 0;
			while (!FILE_EOF())
			{
				file.read(&iByte, 1);
				if (iRemainingBytes == 0)
				{
					if (iByte & 0x80)
					{
						if ((iByte & 0xC0) == 0x80)
						{
							// no proper UTF-8
							oDest.eEncoding = Encoding::Codepage;
							break; // while
						}

						iRemainingBytes = 1;
						while (iRemainingBytes <= 3 && (iByte >> (6 - iRemainingBytes)) & 1)
							++iRemainingBytes;

						if (iRemainingBytes > 3)
						{
							// no proper UTF-8
							oDest.eEncoding = Encoding::Codepage;
							break; // while
						}
					}
				}
				else
				{
					// is marked as "sub-byte"
					if ((iByte & 0xC0) == 0x80)
						--iRemainingBytes;

					// no proper UTF-8
					else
					{
						oDest.eEncoding = Encoding::Codepage;
						break; // while
					}
				}
			}
			if (iRemainingBytes > 0)
			{
				// no proper UTF-8
				oDest.eEncoding = Encoding::Codepage;
				break; // while
			}
			break;
		}

		// UTF-32 LE is the only encoding checked for illegal characters at this point, since it
		// might be UTF-16 LE instead - both can start with FFFE, the first character in an UTF-16
		// text file might be NULL. This would make the first two words FFFE 0000, which is also the
		// BOM of UTF-16 LE
		case Encoding::UTF32:
			if (oDest.eEndian == Endian::BigEndian)
				break; // only check little endian

			{
				bool     bError = false;
				uint32_t iBuf   = 0;

				while (!FILE_EOF())
				{
					file.read(reinterpret_cast<uint8_t*>(&iBuf), 4);
					if constexpr (bOSisBigEndian) // only little endian is checked
						iBuf = SwapEndian32(iBuf);

					if (Unicode::IsNoncharacter(iBuf))
					{
						bError = true;
						break; // while
					}
				}

				if (!bError)
					break;
			}

			file.clear();
			file.seekg(lenBOM);
			[[fallthrough]]; // fallthrough: might be UTF-16 LE instead
		case Encoding::UTF16:
		{
			uint16_t iBuf = 0;
			while (!FILE_EOF())
			{
				file.read(reinterpret_cast<uint8_t*>(&iBuf), 2);
				if (bSwapEndian)
					iBuf = SwapEndian16(iBuf);

				uint16_t iSurrogateHeader = iBuf & 0xFC00;
				if (iSurrogateHeader == 0b1101'1100'0000'0000)
				{
					// high surrogate missing --> Codepage/binary data
					oDest.eEncoding = Encoding::Codepage;
					break; // while
				}
				if (iSurrogateHeader == 0b1101'1000'0000'0000)
				{
					bool bLowSurrogate = false;
					if (!FILE_EOF())
					{
						file.read(reinterpret_cast<uint8_t*>(&iBuf), 2);
						if (bSwapEndian)
							iBuf = SwapEndian16(iBuf);

						if ((iBuf & 0xFC00) == 0b1101'1100'0000'0000)
							bLowSurrogate = true;
					}

					if (!bLowSurrogate)
					{
						// low surrogate missing --> codepage/binary data
						oDest.eEncoding = Encoding::Codepage;
						break; // while
					}
				}
			}
			break;
		}
		}





		if (iFlags & GFI_MINCHECK)
			return true;





		//------------------------------------------------------------------------------------------
		// CHECK FILE CONTENTS (VALUES)


		file.clear();
		file.seekg(lenBOM);
		// switch for all Unicode encodings (for ASCII and Codepage, see below)
		LineBreak lb{};
		bool bConsequentLinebreaks = true;
		bool bLinebreakRead = false;
		switch (oDest.eEncoding)
		{
		case Encoding::UTF8:
		{
			uint8_t iBuf = 0;
			uint32_t cRaw = 0;

			while (!FILE_EOF())
			{
				file.read(&iBuf, 1);
				if (iBuf & 0x80) // multi-byte value
				{
					uint8_t iByteCount = 2;
					while (iByteCount < 4 && (iBuf >> (7 - iByteCount)) & 1)
						++iByteCount;

					cRaw = (iBuf & (0xFF >> (iByteCount + 1))) << ((iByteCount - 1) * 6);
					for (uint8_t i = 1; i < iByteCount; ++i)
					{
						file.read(&iBuf, 1);
						cRaw |= (iBuf & 0x3F) << ((iByteCount - 1 - i) * 6);
					}

					if (Unicode::IsNoncharacter(cRaw))
					{
						oDest.eEncoding = Encoding::Codepage;
						break; // while
					}
				}
				else // ASCII value --> check linebreaks (common linebreaks are ASCII compatible)
				{
					cRaw = iBuf;

					switch (cRaw)
					{
					case '\n': // UNIX linebreak
						lb = LineBreak::UNIX;
						break;

					case '\r': // Macintosh/Windows linebreak
						if (file.peek() == '\n')
						{
							lb = LineBreak::Windows;
							file.seekg(+1, std::ios::cur);
						}
						else
							lb = LineBreak::Macintosh;
						break;

					default:
						continue; // there are no noncharacters in the ASCII range
					}

					if (bConsequentLinebreaks && bLinebreakRead && lb != oDest.eLineBreaks)
						bConsequentLinebreaks = false;

					oDest.eLineBreaks = lb;
					bLinebreakRead = true;
				}
			}

			break;
		}

		case Encoding::UTF16:
		{
			uint16_t iBuf = 0;
			while (!FILE_EOF() && file.peek() != EOF)
			{
				file.read(reinterpret_cast<uint8_t*>(&iBuf), 2);
				if (bSwapEndian)
					iBuf = SwapEndian16(iBuf);
				if ((iBuf & 0xFC00) == 0b1101'1000'0000'0000) // high surrogate
				{
					char32_t cRaw = char32_t(iBuf & 0x03FF) << 10;
					file.read(reinterpret_cast<uint8_t*>(&iBuf), 2);
					if (bSwapEndian)
						iBuf = SwapEndian16(iBuf);
					cRaw |= iBuf & 0x03FF;
					cRaw += 0x10000;

					if (Unicode::IsNoncharacter(cRaw))
					{
						oDest.eEncoding = Encoding::Codepage;
						break; // while
					}
				}
				else // BMP value --> check linebreaks
				{
					switch (iBuf)
					{
					case '\n': // UNIX linebreak
						lb = LineBreak::UNIX;
						break; // switch

					case '\r': // Macintosh/Windows linebreak
						if (!FILE_EOF())
						{
							const auto pos = file.tellg();
							file.read(reinterpret_cast<uint8_t*>(&iBuf), 2);
							if (bSwapEndian)
								iBuf = SwapEndian16(iBuf);

							if (iBuf == '\n')
								lb = LineBreak::Windows;
							else
							{
								file.clear();
								file.seekg(pos);
							}
						}
						else
							lb = LineBreak::Macintosh;

						break; // switch

					default:
						if (Unicode::IsNoncharacter(iBuf))
						{
							oDest.eEncoding = Encoding::Codepage;
							break; // switch
						}
						continue;
					}

					if (oDest.eEncoding == Encoding::Codepage)
						break; // while

					if (bConsequentLinebreaks && bLinebreakRead && lb != oDest.eLineBreaks)
						bConsequentLinebreaks = false;

					oDest.eLineBreaks = lb;
					bLinebreakRead = true;
				}
			}
			break;
		}

		case Encoding::UTF32:
		{
			char32_t cRaw = 0;
			while (!FILE_EOF())
			{
				file.read(reinterpret_cast<uint8_t*>(&cRaw), 4);
				if (bSwapEndian)
					cRaw = SwapEndian32(cRaw);

				switch (cRaw)
				{
				case '\n': // UNIX linebreak
					lb = LineBreak::UNIX;
					break;

				case '\r': // Macintosh/Windows linebreak
					if (!FILE_EOF())
					{
						char32_t cRaw2 = 0;
						const auto pos = file.tellg();
						file.read(reinterpret_cast<uint8_t*>(&cRaw2), 4);
						if (bSwapEndian)
							cRaw2 = SwapEndian32(cRaw2);
						if (cRaw2 == '\n')
							lb = LineBreak::Windows;
						else
						{
							if (Unicode::IsNoncharacter(cRaw2))
							{
								oDest.eEncoding = Encoding::Codepage;
								break; // switch
							}
							else
								lb = LineBreak::Macintosh;

							file.clear();
							file.seekg(pos);
						}
					}
					else
						lb = LineBreak::Macintosh;
					break;

				default:
					if (Unicode::IsNoncharacter(cRaw))
					{
						oDest.eEncoding = Encoding::Codepage;
						break; // switch
					}
					continue; // no linebreak
				}

				if (oDest.eEncoding == Encoding::Codepage)
					break; // while

				if (bConsequentLinebreaks && bLinebreakRead && lb != oDest.eLineBreaks)
					bConsequentLinebreaks = false;

				oDest.eLineBreaks = lb;
				bLinebreakRead = true;
			}
			break;
		}

		}

		// not part of the switch because another encoding might have contained noncharacters,
		// making it a codepage guess.
		if (oDest.eEncoding == Encoding::ASCII || oDest.eEncoding == Encoding::Codepage)
		{
			file.clear();
			file.seekg(0);

			// ASCII has no Unicode noncharacters --> only linebreaks are checked
			// Codepages are assumed to have no invalid values --> only linebreaks are checked

			uint8_t iBuf = 0;
			bLinebreakRead = false;
			bConsequentLinebreaks = true;
			while (!FILE_EOF())
			{
				file.read(&iBuf, 1);

				switch (iBuf)
				{
				case '\n': // UNIX linebreak
					lb = LineBreak::UNIX;
					break;

				case '\r': // Macintosh/Windows linebreak
					if (file.peek() == '\n')
					{
						lb = LineBreak::Windows;
						file.seekg(+1, std::ios::cur);
					}
					else
						lb = LineBreak::Macintosh;
					break;

				default:
					continue;
				}

				if (bConsequentLinebreaks && bLinebreakRead && lb != oDest.eLineBreaks)
					bConsequentLinebreaks = false;

				oDest.eLineBreaks = lb;
				bLinebreakRead = true;
			}
		}

		return true;
	}

	bool ReadAllLines(const wchar_t* szFilePath, std::vector<std::wstring>& oLines,
		const FileInfo& oEncoding)
	{
		FileReader reader(szFilePath, oEncoding);
		if (!reader)
			return false;

		reader.readLines(oLines);
		return true;
	}

	bool ReadAllLines(const wchar_t* szFilePath, std::vector<std::wstring>& oLines)
	{
		FileReader reader(szFilePath);
		if (!reader)
			return false;

		reader.readLines(oLines);
		return true;
	}

	bool WriteTextFile(const wchar_t* szFilePath, const std::vector<std::wstring>& oLines,
		const FileInfo& oEncoding, bool bTrailingLineBreak)
	{
		FileWriter writer(szFilePath, oEncoding);
		if (!writer)
			return false;

		writer.writeLines(oLines, bTrailingLineBreak);
		return true;
	}

}