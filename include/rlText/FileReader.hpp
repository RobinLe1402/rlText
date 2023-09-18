#pragma once
#ifndef RLTEXT_FILEREADER
#define RLTEXT_FILEREADER





#include "Types.hpp"

#include <fstream>
#include <vector>



namespace rlText
{

	/// <summary>
	/// A reader for text files.
	/// </summary>
	class FileReader
	{
	public: // operators

		inline bool operator!() const noexcept { return !m_oFile; }
		inline explicit operator bool() const noexcept { return (bool)m_oFile; }


	public: // methods

		FileReader() = default;
		FileReader(const wchar_t* szFilePath);
		FileReader(const wchar_t* szFilePath, const FileInfo& oEncoding);
		virtual ~FileReader();

		/// <summary>
		/// Open a text file, guess the encoding.
		/// </summary>
		void open(const wchar_t* szFilePath);
		/// <summary>
		/// Open a text file using an explicit encoding.
		/// </summary>
		void open(const wchar_t* szFilePath, const FileInfo& oEncoding);
		/// <summary>
		/// Close an opened text file.
		/// </summary>
		inline void close() { m_oFile.close(); }

		/// <summary>
		/// Read a single character.
		/// </summary>
		void read(char32_t& cDest);
		/// <summary>
		/// Read a defined number of characters into a <c>std::wstring</c>.
		/// </summary>
		void read(std::wstring& sDest, size_t len);
		/// <summary>
		/// Read until the EOF/the next linebreak.
		/// </summary>
		void readLine(std::wstring& sDest);
		/// <summary>
		/// Read all lines until the EOF.
		/// </summary>
		void readLines(std::vector<std::wstring>& oLines);

		/// <summary>
		/// Has the EOF been reached?
		/// </summary>
		inline bool eof() { return m_oFile.eof(); }
		/// <summary>
		/// Get the encoding used for reading the file.<para/>
		/// Was either explicitly set or automatically detected.
		/// </summary>
		inline auto encoding() { return m_oEncoding; }
		inline bool trailingLinebreak() { return m_bTrailingLinebreak; }


	private: // variables

		FileInfo m_oEncoding{};
		bool m_bTrailingLinebreak = false;
		std::basic_ifstream<uint8_t> m_oFile;
	};

}





#endif // RLTEXT_FILEREADER