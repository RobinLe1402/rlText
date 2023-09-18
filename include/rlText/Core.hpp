#pragma once
#ifndef RLTEXT_CORE
#define RLTEXT_CORE





#include "Constants.hpp"

#include <string>
#include <vector>



namespace rlText
{

	constexpr char16_t SwapEndian16(char16_t c) noexcept
	{
		return (c << 8) | (unsigned char)(c >> 8);
	}
	constexpr char32_t SwapEndian32(char32_t c) noexcept
	{
		return
			((c << 24)) |
			((c << 8)  & 0x00FF0000) |
			((c >> 8)  & 0x0000FF00) |
			((c >> 24) & 0x000000FF);
	}





	// Only check the minimum required for detecting the encoding.
	// Doesn't check the linebreak style.
	// When a BOM is found, it's assumed to be correct.
	constexpr unsigned GFI_MINCHECK = 1;

	/// <summary>
	/// Get information about a text file.
	/// </summary>
	/// <param name="iFlags">
	/// Either zero or a combination of the <c>rlText::GFI_[...]</c> constants.
	/// </param>
	/// <returns>Did the check succeed?</returns>
	bool GetFileInfo(const wchar_t* szFilePath, FileInfo& oDest, unsigned iFlags = 0) noexcept;





	/// <summary>
	/// Read an entire text file, use a certain encoding.
	/// </summary>
	/// <param name="szFilePath">The text file to read.</param>
	/// <param name="oLines">The variable the lines should be written to.</param>
	/// <param name="oEncoding">
	/// The text encoding to use. Member <c>eLineBreaks</c> is ignored.
	/// </param>
	/// <returns>Could the text be read?</returns>
	bool ReadAllLines(const wchar_t* szFilePath, std::vector<std::wstring>& oLines,
		const FileInfo& oEncoding);

	/// <summary>
	/// Read an entire text file, automatically determine the encoding.
	/// </summary>
	/// <param name="szFilePath">The text file to read.</param>
	/// <param name="oLines">The variable the lines should be written to.</param>
	/// <returns>Could the text be read?</returns>
	bool ReadAllLines(const wchar_t* szFilePath, std::vector<std::wstring>& oLines);

	/// <summary>
	/// Write a text file from strings, use a certain encoding.
	/// </summary>
	/// <param name="szFilePath">The text file to create.</param>
	/// <param name="oLines">The text to write.</param>
	/// <param name="oEncoding">The encoding to use.</param>
	/// <param name="bTrailingLineBreak">Should the text file end on an empty line?</param>
	/// <returns>Could the text file be created?</returns>
	bool WriteTextFile(const wchar_t* szFilePath, const std::vector<std::wstring>& oLines,
		const FileInfo& oEncoding = UTF8BOM, bool bTrailingLineBreak = false);

}





#endif // RLTEXT_CORE