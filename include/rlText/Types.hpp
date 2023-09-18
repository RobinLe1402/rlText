#pragma once
#ifndef RLTEXT_TYPES
#define RLTEXT_TYPES





namespace rlText
{

	/// <summary>
	/// The most common text encodings.
	/// </summary>
	enum class Encoding
	{
		/// <summary>
		/// Codepage/binary data.<para/>
		/// <para/>
		/// When detected, the data wasn't valid with any other encoding. Might also
		/// indicate a binary file.<para/>
		/// When encoding or decoding with this value, Codepage 1252 is used.
		/// </summary>
		Codepage,
		/// <summary>
		/// ASCII.
		/// </summary>
		ASCII,
		/// <summary>
		/// UTF-8.
		/// </summary>
		UTF8,
		/// <summary>
		/// UTF-16.
		/// </summary>
		UTF16,
		/// <summary>
		/// UTF-32.
		/// </summary>
		UTF32
	};





	/// <summary>
	/// The most common types of linebreak.
	/// </summary>
	enum class LineBreak
	{
		/// <summary>
		/// Windows linebreak (<c>\r</c><c>\n</c>).
		/// </summary>
		Windows,
		/// <summary>
		/// UNIX linebreak (<c>\n</c>).
		/// </summary>
		UNIX,
		/// <summary>
		/// Macintosh linebreak (<c>\r</c>).
		/// </summary>
		Macintosh,

		/// <summary>
		/// Linebreak of the current operating system.
		/// </summary>
		OS =
#if defined(_WIN32) || defined(_WIN64)
		Windows
#else // all modern mainstream operating systems except Windows use the UNIX linebreak ("\n").
		UNIX
#endif
	};





	/// <summary>
	/// Does the current operating system use big endian?
	/// </summary>
	constexpr bool bOSisBigEndian = (const char &)0x00FF == 0x00;

	/// <summary>
	/// Text encoding endian types.
	/// </summary>
	enum class Endian
	{
		LittleEndian,
		BigEndian,
		OS = bOSisBigEndian ? BigEndian : LittleEndian
	};





	/// <summary>
	/// Text file encoding/formatting info.
	/// </summary>
	struct FileInfo
	{
		bool      bBOM;
		Encoding  eEncoding;
		Endian    eEndian     = Endian::OS;
		LineBreak eLineBreaks = LineBreak::OS;

		bool operator==(const FileInfo &other) const
		{
			return
				this->bBOM        == other.bBOM &&
				this->eEncoding   == other.eEncoding &&
				this->eEndian     == other.eEndian &&
				this->eLineBreaks == other.eLineBreaks;
		}

		bool operator!=(const FileInfo &other) const { return !(*this == other); }
	};

}





#endif // RLTEXT_TYPES