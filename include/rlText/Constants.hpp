#pragma once
#ifndef RLTEXT_CONSTANTS
#define RLTEXT_CONSTANTS





#include "Types.hpp"



namespace rlText
{

	namespace DefaultFileInfo
	{

		/// <summary>
		/// The ASCII encoding with default linebreaks.
		/// </summary>
		constexpr FileInfo ASCII =
		{
			.bBOM      = false,
			.eEncoding = Encoding::ASCII
		};



		/// <summary>
		/// A codepage encoding with default linebreaks.
		/// </summary>
		constexpr FileInfo Codepage =
		{
			.bBOM      = false,
			.eEncoding = Encoding::Codepage
		};



		/// <summary>
		/// The UTF-8 encoding with default linebreaks and without a BOM.
		/// </summary>
		constexpr FileInfo UTF8 =
		{
			.bBOM      = false,
			.eEncoding = Encoding::UTF8
		};



		/// <summary>
		/// The UTF-8 encoding with default linebreaks and a BOM.
		/// </summary>
		constexpr FileInfo UTF8BOM =
		{
			.bBOM      = true,
			.eEncoding = Encoding::UTF8
		};



		/// <summary>
		/// The UTF-16 encoding with default linebreaks and endian.
		/// </summary>
		constexpr FileInfo UTF16 =
		{
			.bBOM      = true,
			.eEncoding = Encoding::UTF16
		};



		/// <summary>
		/// The UTF-16 encoding with default linebreaks and big endian.
		/// </summary>
		constexpr FileInfo UTF16BE =
		{
			.bBOM      = true,
			.eEncoding = Encoding::UTF16,
			.eEndian   = Endian::BigEndian
		};



		/// <summary>
		/// The UTF-16 encoding with default linebreaks and little endian.
		/// </summary>
		constexpr FileInfo UTF16LE =
		{
			.bBOM      = true,
			.eEncoding = Encoding::UTF16,
			.eEndian   = Endian::LittleEndian
		};



		/// <summary>
		/// The UTF-32 encoding with default linebreaks and endian.
		/// </summary>
		constexpr FileInfo UTF32 =
		{
			.bBOM      = true,
			.eEncoding = Encoding::UTF32
		};



		/// <summary>
		/// The UTF-32 encoding with default linebreaks and big endian.
		/// </summary>
		constexpr FileInfo UTF32BE =
		{
			.bBOM      = true,
			.eEncoding = Encoding::UTF32,
			.eEndian   = Endian::BigEndian
		};



		/// <summary>
		/// The UTF-32 encoding with default linebreaks and little endian.
		/// </summary>
		constexpr FileInfo UTF32LE =
		{
			.bBOM      = true,
			.eEncoding = Encoding::UTF32,
			.eEndian   = Endian::LittleEndian
		};

	}

	using namespace DefaultFileInfo;

}





#endif // RLTEXT_CONSTANTS