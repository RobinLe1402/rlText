#pragma once
#ifndef RLTEXT_FILEIO_CPP
#define RLTEXT_FILEIO_CPP





#include "FileIO.h"
#include "Types.hpp"



namespace rlText
{

	using Encoding             = rlText_Encoding;
	using Linebreak            = rlText_Linebreak;
	using Flags1               = rlText_Flags1;
	using FileStatisticsStruct = rlText_FileStatisticsStruct;



	/// <summary>Get information about a text file.</summary>
	/// <param name="szFilepath">[UTF-8] The path of the text file.</param>
	/// <param name="pEncoding">
	/// If this parameter is not zero, the pointed-to variable will be set to the encoding of the text
	/// file.
	/// </param>
	/// <param name="pStatistics">
	/// If this parameter is not zero, the pointed-to variable will receive statistical data of the text
	/// file.
	/// </param>
	/// <returns>
	/// Did the function succeed?
	/// </returns>
	inline bool GetFileInfo(
		const RLTEXT_UTF8CHAR *szFilepath,
		Encoding              *pEncoding,
		FileStatisticsStruct  *pStatistics,
		Flags1                 iFlags
	) noexcept
	{
		return rlText_GetFileInfo(szFilepath, pEncoding, pStatistics, iFlags);
	}



	class File final
	{
	public: // methods

		File() = default;

		File(const RLTEXT_UTF8CHAR *szFilepath, Encoding iEncoding = 0)
		{
			open(szFilepath, iEncoding);
		}

		File(Linebreak iLinebreakStyle)
		{
			create(iLinebreakStyle);
		}

		~File()
		{
			freeHandle();
		}

		explicit operator bool() const noexcept { return  m_oHandle; }
		bool operator!()         const noexcept { return !m_oHandle; }

		bool open(const RLTEXT_UTF8CHAR *szFilepath, Encoding iEncoding = 0) noexcept
		{
			freeHandle();
			m_oHandle = rlText_FileOpen(szFilepath, iEncoding);
			return m_oHandle;
		}

		bool create(Linebreak iLinebreakStyle) noexcept
		{
			freeHandle();
			m_oHandle = rlText_FileCreate(iLinebreakStyle);
			return m_oHandle;
		}

		bool save(const RLTEXT_UTF8CHAR *szFilepath,
			Encoding iEncoding = RLTEXT_FILEENCODING_UTF8BOM, bool bTrailingLinebreak = false)
			const noexcept
		{
			if (!m_oHandle)
				return false;

			return rlText_FileSave(m_oHandle, szFilepath, iEncoding, bTrailingLinebreak);
		}

		size_t getLineCount() const noexcept
		{
			if (!m_oHandle)
				return 0;

			return rlText_FileGetLineCount(m_oHandle);
		}

		RLTEXT_UTF8STRING getLine(size_t iLine) const noexcept
		{
			if (!m_oHandle)
				return RLTEXT_UTF8STRING();

			const auto len = rlText_FileGetLine(m_oHandle, iLine, nullptr, 0);
			if (len <= 1)
				return RLTEXT_UTF8STRING();

			RLTEXT_UTF8STRING sResult(len - 1, 0);
			rlText_FileGetLine(m_oHandle, iLine, sResult.data(), sResult.length() + 1);

			return sResult;
		}

		size_t setLine(size_t iLine, const RLTEXT_UTF8CHAR *szLine, bool bReplace) noexcept
		{
			if (!m_oHandle)
				return 0;

			return rlText_FileSetLine(m_oHandle, iLine, szLine, bReplace);
		}

		bool deleteLine(size_t iLine) noexcept
		{
			if (!m_oHandle)
				return false;

			return rlText_FileDeleteLine(m_oHandle, iLine);
		}

		Linebreak getLinebreakType() const noexcept
		{
			if (!m_oHandle)
				return 0;

			return rlText_FileGetLinebreakType(m_oHandle);
		}

		bool setLinebreakType(Linebreak iLinebreakType) noexcept
		{
			if (!m_oHandle)
				return false;

			return rlText_FileSetLinebreakType(m_oHandle, iLinebreakType);
		}

		RLTEXT_UTF8STRING getAsSingleString() const noexcept
		{
			if (!m_oHandle)
				return RLTEXT_UTF8STRING();

			const auto len = rlText_FileGetAsSingleString(m_oHandle, nullptr, 0);
			if (len <= 1)
				return RLTEXT_UTF8STRING();

			RLTEXT_UTF8STRING sResult(len - 1, 0);
			rlText_FileGetAsSingleString(m_oHandle, sResult.data(), sResult.length() + 1);

			return sResult;
		}

		bool setAsSingleString(const RLTEXT_UTF8CHAR *sz) noexcept
		{
			if (!m_oHandle)
				return false;

			return rlText_FileSetAsSingleString(m_oHandle, sz);
		}

		void clear() noexcept
		{
			if (m_oHandle)
				rlText_FileClear(m_oHandle);
		}


	private: // methods

		void freeHandle()
		{
			if (m_oHandle)
			{
				rlText_FileFree(m_oHandle);
				m_oHandle = nullptr;
			}
		}


	private: // variables

		rlText_File m_oHandle = nullptr;

	};

}





#endif // RLTEXT_FILEIO_CPP