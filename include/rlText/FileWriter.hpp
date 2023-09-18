#pragma once
#ifndef RLTEXT_FILEWRITER
#define RLTEXT_FILEWRITER





#include "Constants.hpp"
#include "Types.hpp"

#include <fstream>
#include <vector>



namespace rlText
{

	/// <summary>
	/// A writer for text files.
	/// </summary>
	class FileWriter
	{
	public: // operators

		inline bool operator!() { return !m_oFile; }


	public: // methods

		FileWriter() = default;
		FileWriter(const wchar_t* szFilePath, const FileInfo& oEncoding);
		virtual ~FileWriter();

		void open(const wchar_t* szFilePath, const FileInfo& oEncoding);
		inline void close() { m_oFile.close(); }


		/// <summary>
		/// Write a single character.
		/// </summary>
		/// <param name="c">The character to be written, as UTF-32 (= raw unicode value).</param>
		void write(char32_t c);

		/// <summary>
		/// Write a string.
		/// </summary>
		/// <param name="szText">The string to be written.</param>
		/// <param name="len">
		/// The length, in characters, of the string pointed to by the <c>szText</c> parameter.
		/// <para/>
		/// Can be zero, in which case the string is assumed to be null-terminated.
		/// </param>
		void write(const wchar_t* szText, size_t len = 0);

		/// <summary>
		/// Write a string.
		/// </summary>
		inline void write(const std::wstring& sText) { write(sText.c_str(), sText.length()); }

		/// <summary>
		/// Write a string, followed by a linebreak.
		/// </summary>
		/// <param name="szText">The string to be written.</param>
		/// <param name="len">
		/// The length, in characters, of the string pointed to by the <c>szText</c> parameter.
		/// <para/>
		/// Can be zero, in which case the string is assumed to be null-terminated.
		/// </param>
		void writeLine(const wchar_t* szText, size_t len = 0);

		/// <summary>
		/// Write a string, followed by a linebreak.
		/// </summary>
		inline void writeLine(const std::wstring& sText)
		{
			writeLine(sText.c_str(), sText.length());
		}

		/// <summary>
		/// Write multiple strings as lines.
		/// </summary>
		/// <param name="oLines">
		/// The text to be written.<para/> Each string is treated as a line.
		/// </param>
		/// <param name="bTrailingLinebreak">
		/// Should a linebreak be written after the last line?
		/// </param>
		void writeLines(const std::vector<std::wstring>& oLines, bool bTrailingLinebreak = true);


		/// <summary>
		/// Is a file currently opened for writing?
		/// </summary>
		inline bool isOpen() { return m_oFile.is_open(); }

		/// <summary>
		/// The encoding used for writing to the file.
		/// </summary>
		inline auto encoding() { return m_oEncoding; }


	private: // variables

		FileInfo m_oEncoding = UTF8BOM;
		std::ofstream m_oFile;
	};

}





#endif // RLTEXT_FILEWRITER