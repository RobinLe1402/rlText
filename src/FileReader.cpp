#include "rlText/FileReader.hpp"

#include "rlText/Core.hpp"
#include "../private/Constants.hpp"

namespace rlText
{

	FileReader::FileReader(const wchar_t* szFilePath) { open(szFilePath); }

	FileReader::FileReader(const wchar_t* szFilePath, const FileInfo& oEncoding)
	{
		open(szFilePath, oEncoding);
	}

	FileReader::~FileReader() { close(); }

	void FileReader::open(const wchar_t* szFilePath)
	{
		close();

		FileInfo oEncoding{};
		if (!GetFileInfo(szFilePath, oEncoding))
			return;

		open(szFilePath, oEncoding);
	}

	void FileReader::open(const wchar_t* szFilePath, const FileInfo& oEncoding)
	{
		close();
		m_oFile.open(szFilePath, std::ios::binary);
		if (!m_oFile)
			return; // couldn't open the file

		size_t lenBOM = 0;
		if (oEncoding.bBOM)
		{
			switch (oEncoding.eEncoding)
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
			}
		}

		m_oFile.seekg(lenBOM, std::ios::beg);
		if (m_oFile.tellg() != lenBOM)
		{
			m_oFile.close();
			return; // file was shorter than the expected BOM length
		}

		m_oEncoding = oEncoding;
		m_bTrailingLinebreak = false;
	}

	void FileReader::read(char32_t& cDest)
	{
		if (eof())
		{
			cDest = 0;
			return;
		}

		const bool bSwapEndian = m_oEncoding.eEndian != Endian::OS;
		switch (m_oEncoding.eEncoding)
		{
		case Encoding::ASCII:
		{
			uint8_t cTMP = 0;
			m_oFile.read(&cTMP, 1);
			cDest = cTMP;

			break;
		}

		case Encoding::Codepage:
		{
			uint8_t cEnc = 0;
			m_oFile.read(&cEnc, 1);
			if (cEnc >= 0x80 && cEnc <= 0x9F)
			{
				cDest = cCP1252_Table[cEnc - 0x80];
				if (cDest == 0)
					cDest = '?';
			}
			else
				cDest = cEnc;

			break;
		}

		case Encoding::UTF8:
		{
			uint8_t iBuf[4]{};
			m_oFile.read(iBuf, 1);

			if (iBuf[0] & 0x80)
			{
				uint8_t iByteCount = 1;
				while (iByteCount <= 4 && ((iBuf[0] >> (7 - iByteCount)) & 1))
					++iByteCount;

				if (iByteCount > 4)
					throw "Invalid UTF-8: Initial bit sequence was larger than 4";

				m_oFile.read(iBuf + 1, (size_t)iByteCount - 1);
				cDest = (iBuf[0] & (0xFF >> (iByteCount + 1))) << ((iByteCount - 1) * 6);
				for (uint8_t i = 1; i < iByteCount; ++i)
				{
					cDest |= (iBuf[i] & 0x3F) << ((iByteCount - 1 - i) * 6);
				}

			}
			else
				cDest = iBuf[0];


			break;
		}

		case Encoding::UTF16:
		{
			uint8_t iBuf[2]{};
			uint16_t cEnc = 0;

			m_oFile.read(iBuf, 2);
			cEnc = *reinterpret_cast<const uint16_t*>(iBuf);
			if (bSwapEndian)
				cEnc = SwapEndian16(cEnc);

			if ((cEnc & 0xFC00) == 0b1101'1000'0000'0000)
			{
				cDest = (cEnc & 0x03FF) << 10;
				m_oFile.read(iBuf, 2);
				cEnc = *reinterpret_cast<const uint16_t*>(iBuf);
				if (bSwapEndian)
					cEnc = SwapEndian16(cEnc);

				cDest |= cEnc & 0x03FF;
				cDest += 0x10000;

			}
			else
				cDest = cEnc;

			break;
		}

		case Encoding::UTF32:
		{
			uint8_t iBuf[4];
			m_oFile.read(iBuf, 4);
			cDest = *reinterpret_cast<const uint32_t*>(iBuf);
			if (bSwapEndian)
				cDest = SwapEndian32(cDest);

			break;
		}

		default:
			throw "Reading unknown text encoding";
		}
	}

	void FileReader::read(std::wstring& sDest, size_t len)
	{
		sDest.clear();
		if (eof() || len == 0)
			return;

		sDest.reserve(len);

		while (!eof() && sDest.length() < len)
		{
			char32_t c = 0;
			read(c);

			if (c < 0x01'00'00)
				sDest += (wchar_t)c;
			else
			{
				c -= 0x01'00'00;
				sDest += 0b1101'1000'0000'0000 | (c >> 10);
				sDest += 0b1101'1100'0000'0000 | (c & 0x03FF);
			}
		}
	}

	void FileReader::readLine(std::wstring& sDest)
	{
		sDest.clear();

		if (eof())
			return;

		auto pos = m_oFile.tellg();

		char32_t c = 0;
		size_t len = 0;
		do
		{
			read(c);
			++len;
		} while (!eof() && c != '\r' && c != '\n');
		--len;

		m_oFile.clear(); // clear eofbit
		m_oFile.seekg(pos);
		read(sDest, len);

		if (m_oFile.peek() == EOF/*eof()*/)
			m_bTrailingLinebreak = false;
		else
		{
			read(c);
			if (c == '\n')
				m_oEncoding.eLineBreaks = LineBreak::UNIX;
			if (c == '\r')
			{
				if (!eof())
				{
					pos = m_oFile.tellg();
					read(c);
					if (c == '\n')
						m_oEncoding.eLineBreaks = LineBreak::Windows;
					else
					{
						m_oEncoding.eLineBreaks = LineBreak::Macintosh;
						m_oFile.clear(); // clear eofbit
						m_oFile.seekg(pos);
					}
				}
				else
					m_oEncoding.eLineBreaks = LineBreak::Macintosh;
			}

			m_bTrailingLinebreak = eof();
		}
	}

	void FileReader::readLines(std::vector<std::wstring>& oLines)
	{
		oLines.clear();

		while (!eof())
		{
			std::wstring s;
			readLine(s);
			oLines.push_back(std::move(s));
		}
	}

}