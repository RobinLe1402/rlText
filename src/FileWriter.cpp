#include "rlText/FileWriter.hpp"

#include "rlText/Core.hpp"
#include "rlText/Unicode.hpp"
#include "../private/Constants.hpp"

namespace rlText
{

	FileWriter::FileWriter(const wchar_t* szFilePath, const FileInfo& oEncoding)
	{
		open(szFilePath, oEncoding);
	}

	FileWriter::~FileWriter() { close(); }

	void FileWriter::open(const wchar_t* szFilePath, const FileInfo& oEncoding)
	{
		close();

		m_oEncoding = oEncoding;
		m_oFile.open(szFilePath, std::ios::out | std::ios::binary);

		if (!m_oFile)
			return;


		// write the BOM (if applicable)
		if (m_oEncoding.bBOM)
		{
			bool bBigEndian = m_oEncoding.eEndian == Endian::BigEndian;

			uint8_t BOM[4]{};
			switch (m_oEncoding.eEncoding)
			{
			case Encoding::UTF8:
				m_oFile.write("\x00EF\x00BB\x00BF", 3);
				break;
			case Encoding::UTF16:
				if (bBigEndian)
					m_oFile.write("\x00FE\x00FF", 2);
				else
					m_oFile.write("\x00FF\x00FE", 2);
				break;
			case Encoding::UTF32:
				if (bBigEndian)
					m_oFile.write("\x0000\x0000\x00FE\x00FF", 4);
				else
					m_oFile.write("\x00FF\x00FE\x0000\x0000", 4);
				break;
			}
		}
	}

	void FileWriter::write(char32_t c)
	{
		if (!isOpen())
			return;

		if (Unicode::IsNoncharacter(c))
			throw "Tried to write a noncharacter value to a file";


		const bool bSwapEndian = m_oEncoding.eEndian != Endian::BigEndian;

		switch (m_oEncoding.eEncoding)
		{
		case Encoding::ASCII:
		{
			if (c > 0x7F)
				c = '?';

			char cEncoded = (char)c;
			m_oFile.write(&cEncoded, 1);
			break;
		}

		case Encoding::Codepage:
		{
			constexpr char iUnknown = '?';
			if (c >= 0x80 && c <= 0x9F)
				m_oFile.write(&iUnknown, 1);

			else if (c > 0xFF)
			{
				for (char i = 0; i < 0x2F; ++i)
				{
					if (cCP1252_Table[i] == c)
					{
						m_oFile.write(&i, 1);
						return;
					}
				}
				m_oFile.write(&iUnknown, 1);
			}
			else
			{
				char cOutput = (char)c;
				m_oFile.write(&cOutput, 1);
			}

			break;
		}

		case Encoding::UTF8:
		{
			// ASCII value --> no encoding
			if (c < 0x80)
			{
				char cEncoded = (char)c;
				m_oFile.write(&cEncoded, 1);
				return;
			}


			// count bits necessary to represent the number.
			// at least 8 bits are being used - otherwise, it would be an ASCII value; see above.
			uint8_t iUsedBits = 8;
			while (c >> iUsedBits)
				++iUsedBits;

			// compute count of bytes needed to represent the character
			uint8_t iByteCount = (iUsedBits / 6) + 1;
			if (iUsedBits % 6 > (7 - iByteCount))
				++iByteCount;



			auto cEncoded = std::make_unique<char[]>(iByteCount);

			cEncoded[0] = 0xFF << (8 - iByteCount); // write byte count bits
			cEncoded[0] |= c >> ((iByteCount - 1) * 6);
			for (uint8_t i = 1; i < iByteCount; ++i)
			{
				cEncoded[i] = 0x80 | ((c >> ((iByteCount - 1 - i) * 6)) & 0x3F);
			}
			m_oFile.write(cEncoded.get(), iByteCount);


			break;
		}



		case Encoding::UTF16:
		{
			if (c < 0x1'00'00)
			{
				char16_t cEncoded = (char16_t)c;
				if (bSwapEndian)
					cEncoded = SwapEndian16(cEncoded);

				char cOutput[2] ={ char(cEncoded >> 8), char(cEncoded) };
				m_oFile.write(cOutput, 2);
			}
			else
			{
				c -= 0x1'00'00;
				char16_t cEncoded[2] =
				{
					char16_t(0b1101'1000'0000'0000ui16 | (char16_t)(c >> 10)),
					char16_t(0b1101'1100'0000'0000ui16 | (char16_t)(c & 0x03FF))
				};
				if (bSwapEndian)
				{
					cEncoded[0] = SwapEndian16(cEncoded[0]);
					cEncoded[1] = SwapEndian16(cEncoded[1]);
				}

				char cOutput[4] =
				{
					char(cEncoded[0] >> 8),
					char(cEncoded[0]),
					char(cEncoded[1] >> 8),
					char(cEncoded[1])
				};
				m_oFile.write(cOutput, 4);
			}
			break;
		}



		case Encoding::UTF32:
		{
			if (bSwapEndian)
				c = SwapEndian32(c);

			char cOutput[4] =
			{
				char(c >> 24),
				char(c >> 16),
				char(c >> 8),
				char(c)
			};

			m_oFile.write(cOutput, 4);
		}

		}
	}

	void FileWriter::write(const wchar_t* szText, size_t len)
	{
		if (len == 0)
			len = wcslen(szText);

		for (size_t i = 0; i < len; ++i)
		{
			char32_t cRaw;
			if ((szText[i] & 0xFC00) == 0b1101'1000'0000'0000)
			{
				cRaw = szText[i] & 0x03FF;
				cRaw <<= 10;
				cRaw |= szText[i + 1] & 0x03FF;
				cRaw += 0x10000;

				++i;
			}
			else
				cRaw = szText[i];

			write(cRaw);
		}
	}

	void FileWriter::writeLine(const wchar_t* szText, size_t len)
	{
		write(szText, len);
		switch (m_oEncoding.eLineBreaks)
		{
		case LineBreak::Windows:
			write('\r');
			write('\n');
			break;
		case LineBreak::UNIX:
			write('\n');
			break;
		case LineBreak::Macintosh:
			write('\r');
			break;
		}
	}

	void FileWriter::writeLines(const std::vector<std::wstring>& oLines, bool bTrailingLinebreak)
	{
		if (oLines.size() > 1)
		{
			for (size_t i = 0; i < oLines.size() - 1; ++i)
			{
				writeLine(oLines[i]);
			}
			if (bTrailingLinebreak)
				writeLine(oLines[oLines.size() - 1]);
			else
				write(oLines[oLines.size() - 1]);
		}
	}

}