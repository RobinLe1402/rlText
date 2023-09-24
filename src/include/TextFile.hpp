#pragma once
#ifndef RLTEXTDLL_TEXTFILE
#define RLTEXTDLL_TEXTFILE





#include "rlTextDLL/FileIO.h"

#include <string>
#include <vector>



namespace
{

	constexpr char8_t szLINEBREAK[3][3] =
	{
		u8"\n",
		u8"\r\n",
		u8"\r"
	};

	constexpr unsigned iLINEBREAK_LEN[3] =
	{
		1, // \n
		2, // \r\n
		1  // \r
	};

}



class TextFile final
{
public: // static functions

	static TextFile *Open(const char8_t *szFilepath, rlText_Encoding iEncoding) noexcept;
	static TextFile *Create(rlText_Linebreak iLinebreakType) noexcept;


public: // functions

	bool saveToFile(const char8_t *szFilepath, rlText_Encoding iEncoding,
		bool bTrailingLinebreak) const noexcept;
	rlText_Count getLineCount() const noexcept { return m_oLines.size(); }
	const std::u8string &getLine(rlText_Count iLine) const noexcept { return m_oLines[iLine]; }
	rlText_Count setLine(rlText_Count iLine, const char *szLine, bool bReplace) noexcept;
	bool deleteLine(rlText_Count iLine) noexcept;
	rlText_Linebreak getLinebreakType() const noexcept { return m_iLinebreakType; }
	bool setLinebreakType(rlText_Linebreak iLinebreakType) noexcept;
	rlText_Count getTotalLength() const noexcept;
	std::u8string getAsText(bool bTrailingLinebreak = false) const noexcept;
	bool setText(const char *sz) noexcept;
	void clear() noexcept { m_oLines.clear(); }


private: // methods

	const char8_t *getLinebreak() const noexcept { return szLINEBREAK[m_iLinebreakType]; }
	unsigned getLinebreakLen() const noexcept { return iLINEBREAK_LEN[m_iLinebreakType]; }


private: // variables

	std::vector<std::u8string> m_oLines;
	rlText_Linebreak m_iLinebreakType =
#ifdef _WIN32
		RLTEXT_LINEBREAK_WINDOWS; // \r\n
#else
		RLTEXT_LINEBREAK_UNIX; // \n
#endif

};





#endif // RLTEXTDLL_TEXTFILE