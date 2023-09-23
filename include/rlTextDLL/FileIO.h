#pragma once
#ifndef RLTEXTDLL_FILEIO
#define RLTEXTDLL_FILEIO





#include "ExportSpecs.h"
#include "Types.h"

#include <stdint.h>
#include <wchar.h>



#define RLTEXT_FILEENCODING_FLAG_BOM           0x10
#define RLTEXT_FILEENCODING_FLAG_LITTLE_ENDIAN 0x20
#define RLTEXT_FILEENCODING_FLAG_BIG_ENDIAN    0x40
#define RLTEXT_FILEENCODING_FLAG_NONSTANDARD   0x80

#define RLTEXT_FILEENCODING_MASK_ENCODING    0x0F
#define RLTEXT_FILEENCODING_MASK_FLAGS       0xF0
#define RLTEXT_FILEENCODING_MASK_FLAG_ENDIAN 0x60

#define RLTEXT_FILEENCODING_ASCII   0x01
#define RLTEXT_FILEENCODING_CP1252  \
	(RLTEXT_FILEENCODING_ASCII | RLTEXT_FILEENCODING_FLAG_NONSTANDARD)
#define RLTEXT_FILEENCODING_UTF8    0x02
#define RLTEXT_FILEENCODING_UTF8BOM \
	(RLTEXT_FILEENCODING_UTF8  | RLTEXT_FILEENCODING_FLAG_BOM)
#define RLTEXT_FILEENCODING_UTF16   0x04
#define RLTEXT_FILEENCODING_UTF16LE \
	(RLTEXT_FILEENCODING_UTF16 | RLTEXT_FILEENCODING_FLAG_LITTLE_ENDIAN)
#define RLTEXT_FILEENCODING_UTF16BE \
	(RLTEXT_FILEENCODING_UTF16 | RLTEXT_FILEENCODING_FLAG_BIG_ENDIAN)
#define RLTEXT_FILEENCODING_UTF32   0x08
#define RLTEXT_FILEENCODING_UTF32LE \
	(RLTEXT_FILEENCODING_UTF32 | RLTEXT_FILEENCODING_FLAG_LITTLE_ENDIAN)
#define RLTEXT_FILEENCODING_UTF32BE \
	(RLTEXT_FILEENCODING_UTF32 | RLTEXT_FILEENCODING_FLAG_BIG_ENDIAN)


#define RLTEXT_LINEBREAK_UNIX      0 // \n
#define RLTEXT_LINEBREAK_WINDOWS   1 // \r\n
#define RLTEXT_LINEBREAK_MACINTOSH 2 // \r


#define RLTEXT_GETFILEINFO_CHECKMINIMUM   0 // only check the bare minimum, e.g. BOM and filesize.
#define RLTEXT_GETFILEINFO_CHECKWHOLEFILE 1 // always check the whole file, even if a BOM is found.
#define RLTEXT_GETFILEINFO_ASSUMEUTF8     2 // if there is no BOM, assume UTF-8.



typedef uint8_t  rlText_Encoding;
typedef uint8_t  rlText_Linebreak;
typedef uint8_t  rlText_Flags1;

typedef struct
{
	rlText_Count iCharCount;         // the total count of Unicode codepoints in the file.
	rlText_Count iLineBreakCount[3]; // the count of linebreaks for all of the linebreak types.
} rlText_FileStatisticsStruct;



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
/// If the function succeeds, it returns nonzero.<para />
/// If it fails, it returns zero.
/// </returns>
RLTEXT_API rlText_Bool EXPORT rlText_GetFileInfo(
	const char                  *szFilepath,
	rlText_Encoding             *pEncoding,
	rlText_FileStatisticsStruct *pStatistics,
	rlText_Flags1                iFlags
);



typedef struct
{
	int unused;
} *rlText_File;



/// <summary>Read and decode a text file.</summary>
/// <param name="szFilepath">[UTF-8] The filepath of the file to decode.</param>
/// <param name="iEncoding">
/// The encoding to use; one of the following values:<para />
/// <ul>
/// <li><c>RLTEXT_FILEENCODING_ASCII</c></li>
/// <li><c>RLTEXT_FILEENCODING_CP1252</c></li>
/// <li><c>RLTEXT_FILEENCODING_UTF8</c></li>
/// <li><c>RLTEXT_FILEENCODING_UTF8BOM</c></li>
/// <li><c>RLTEXT_FILEENCODING_UTF16</c> (uses the native endian)</li>
/// <li><c>RLTEXT_FILEENCODING_UTF16LE</c></li>
/// <li><c>RLTEXT_FILEENCODING_UTF16BE</c></li>
/// <li><c>RLTEXT_FILEENCODING_UTF32</c> (uses the native endian)</li>
/// <li><c>RLTEXT_FILEENCODING_UTF32LE</c></li>
/// <li><c>RLTEXT_FILEENCODING_UTF32BE</c></li>
/// </ul>
/// Can be zero. In this case, the encoding is automatically determined.
/// </param>
/// <returns>
/// If the function fails, it returns zero.<para />
/// If the function succeeds, it returns a handle to the text data.
/// Once you don't need it anymore, make sure to free it via <c>rlText_FileFree</c>.
/// </returns>
RLTEXT_API rlText_File EXPORT rlText_FileOpen(
	const char     *szFilepath,
	rlText_Encoding iEncoding
);

/// <summary>Create text data from scratch.</summary>
/// <param name="iEncoding">
/// The encoding to use; one of the following values:<para />
/// <ul>
/// <li><c>RLTEXT_FILEENCODING_ASCII</c></li>
/// <li><c>RLTEXT_FILEENCODING_CP1252</c></li>
/// <li><c>RLTEXT_FILEENCODING_UTF8</c></li>
/// <li><c>RLTEXT_FILEENCODING_UTF8BOM</c></li>
/// <li><c>RLTEXT_FILEENCODING_UTF16</c> (uses the native endian)</li>
/// <li><c>RLTEXT_FILEENCODING_UTF16LE</c></li>
/// <li><c>RLTEXT_FILEENCODING_UTF16BE</c></li>
/// <li><c>RLTEXT_FILEENCODING_UTF32</c> (uses the native endian)</li>
/// <li><c>RLTEXT_FILEENCODING_UTF32LE</c></li>
/// <li><c>RLTEXT_FILEENCODING_UTF32BE</c></li>
/// </ul>
/// Cannot be zero.
/// </param>
/// <param name="iLinebreakStyle">
/// One of the following values:
/// <ul>
/// <li><c>RLTEXT_LINEBREAK_UNIX</c> (\n)</li>
/// <li><c>RLTEXT_LINEBREAK_WINDOWS</c> (\r\n)</li>
/// <li><c>RLTEXT_LINEBREAK_MACINTOSH</c> (\r)</li>
/// </ul><para />
/// Can also be zero, in which case the OS-native linebreak type will be used.
/// </param>
/// <returns>
/// If the function fails, it returns zero.<para />
/// If the function succeeds, it returns a handle to the text data.
/// Once you don't need it anymore, make sure to free it via <c>rlText_FileFree</c>.
/// </returns>
RLTEXT_API rlText_File EXPORT rlText_FileCreate(rlText_Linebreak iLinebreakStyle);

/// <summary>Save text data to a file.</summary>
/// <param name="oFile">A handle to the text data to process.</param>
/// <param name="szFilepath">[UTF-8] The filepath to save the text data to.</param>
/// <param name="iEncoding">
/// The encoding to use; one of the following values:<para />
/// <ul>
/// <li><c>RLTEXT_FILEENCODING_ASCII</c></li>
/// <li><c>RLTEXT_FILEENCODING_CP1252</c></li>
/// <li><c>RLTEXT_FILEENCODING_UTF8</c></li>
/// <li><c>RLTEXT_FILEENCODING_UTF8BOM</c></li>
/// <li><c>RLTEXT_FILEENCODING_UTF16</c> (uses the native endian)</li>
/// <li><c>RLTEXT_FILEENCODING_UTF16LE</c></li>
/// <li><c>RLTEXT_FILEENCODING_UTF16BE</c></li>
/// <li><c>RLTEXT_FILEENCODING_UTF32</c> (uses the native endian)</li>
/// <li><c>RLTEXT_FILEENCODING_UTF32LE</c></li>
/// <li><c>RLTEXT_FILEENCODING_UTF32BE</c></li>
/// </ul>
/// Can be zero. In this case, the default encoding (UTF-8 BOM) will be used.
/// </param>
/// <param name="bTrailingLinebreak">
/// If this value is nonzero, a final linebreak will be written to the end of the file.
/// </param>
/// <returns>
/// If the function succeeds, it returns nonzero.<para />
/// Otherwise, it returns zero.
/// </returns>
RLTEXT_API rlText_Bool EXPORT rlText_FileSave(
	rlText_File     oFile,
	const char     *szFilepath,
	rlText_Encoding iEncoding,
	rlText_Bool     bTrailingLinebreak
);

/// <summary>Free text data.</summary>
/// <param name="oFile">The handle of the text data to free.</param>
RLTEXT_API void EXPORT rlText_FileFree(rlText_File oFile);

/// <summary>Get the count of lines of text data.</summary>
/// <param name="oFile">The handle to the text data.</param>
/// <returns>
/// If the function succeeds, it returns the count of lines in the text data (which might be zero).
/// <para />
/// Otherwise, it returns zero.
/// </returns>
RLTEXT_API rlText_Count EXPORT rlText_FileGetLineCount(rlText_File oFile);

/// <summary>Get a single line of text from text data.</summary>
/// <param name="oFile">The handle to the text data.</param>
/// <param name="iLine">The zero-based index of the line.</param>
/// <param name="pBuf">
/// A pointer to the buffer that should receive the [UTF-8 encoded] contents of the line with the
/// index <c>iLine</c>.<para/>
/// Can be zero. In this case, the return value indicates the required buffer size (in characters).
/// </param>
/// <param name="iBufSize">
/// The size, in bytes, of the buffer pointed to by <c>pBuf</c>.<para/>
/// Can be zero. In this case, the return value indicates the required buffer size (in characters).
/// </param>
/// <returns>
/// If both <c>pBuf</c> and <c>iBufSize</c> are nonzero, the return value indicates the count of
/// characters written to the buffer, including the terminating zero.<para />
/// If either <c>pBuf</c> or <c>iBufSize</c> is set to zero, the return value indicates the required
/// size of the buffer, in characters, including the terminating zero.
/// </returns>
RLTEXT_API rlText_Count EXPORT rlText_FileGetLine(
	rlText_File  oFile,
	rlText_Count iLine,
	char        *pBuf,
	rlText_Count iBufSize
);

/// <summary>Insert into or replace a line from text data.</summary>
/// <param name="oFile">The handle of the text data.</param>
/// <param name="iLine">The zero-based index of the line.</param>
/// <param name="szLine">
/// The [UTF-8 encoded] string to insert.<para/>
/// If linebreaks are detected in this string, the string will be split up into multiple lines.
/// </param>
/// <param name="bReplace">
/// If this value is nonzero, <c>iLine</c> must be a valid index, and the line at this index will be
/// replaced by <c>szLine</c>.<para />
/// If this value is zero, <c>szLine</c> will be inserted at index <c>iLine</c>. If <c>iLine</c> is
/// greater than the count of lines in the text data, <c>szLine</c> will be inserted right after the
/// last line.
/// </param>
/// <returns>
/// If the function succeeds, it returns the nonzero count of lines inserted (strings will be
/// automatically split into multiple lines if they include linebreaks).<para />
/// If the function fails, it returns zero.
/// </returns>
RLTEXT_API rlText_Count EXPORT rlText_FileSetLine(
	rlText_File  oFile,
	rlText_Count iLine,
	const char  *szLine,
	rlText_Bool  bReplace
);

/// <summary>Delete a single line from text data.</summary>
/// <param name="oFile">The handle of the text data.</param>
/// <param name="iLine">The zero-based index of the line to delete.</param>
/// <returns>
/// If the function succeeds, it returns nonzero.<para/>
/// Otherwise, it returns zero.
/// </returns>
RLTEXT_API rlText_Bool EXPORT rlText_FileDeleteLine(
	rlText_File  oFile,
	rlText_Count iLine
);

/// <summary>
/// Get the type of linebreak detected on load/that will be used when saving the text data to a
/// file.
/// </summary>
/// <param name="oFile">The handle of the text data.</param>
/// <returns>
/// If the function fails, it returns zero.<para />
/// Otherwise, it returns one of the following values:
/// <ul>
/// <li><c>RLTEXT_LINEBREAK_UNIX</c> (\n)</li>
/// <li><c>RLTEXT_LINEBREAK_WINDOWS</c> (\r\n)</li>
/// <li><c>RLTEXT_LINEBREAK_MACINTOSH</c> (\r)</li>
/// </ul>
/// </returns>
RLTEXT_API rlText_Linebreak EXPORT rlText_FileGetLinebreakType(rlText_File oFile);

/// <summary>
/// Set the type of linebreak that will be used when saving the text data to a file.
/// </summary>
/// <param name="oFile"></param>
/// <param name="iLinebreakType">
/// One of the following values:
/// <ul>
/// <li><c>RLTEXT_LINEBREAK_UNIX</c> (\n)</li>
/// <li><c>RLTEXT_LINEBREAK_WINDOWS</c> (\r\n)</li>
/// <li><c>RLTEXT_LINEBREAK_MACINTOSH</c> (\r)</li>
/// </ul><para />
/// Can also be zero, in which case the OS-native linebreak type will be used.
/// </param>
/// <returns>
/// If the function succeds, it returns nonzero.<para />
/// Otherwise, it returns zero.
/// </returns>
RLTEXT_API rlText_Bool EXPORT rlText_FileSetLinebreakType(
	rlText_File      oFile,
	rlText_Linebreak iLinebreakType
);

/// <summary>Get the entire contents of text data as a single string.</summary>
/// <param name="oFile">The handle to the text data.</param>
/// <param name="pBuf">
/// A pointer to the buffer that should receive the [UTF-8 encoded] contents of the text data.
/// <para/>
/// The linebreak style indicated by <c>rlText_FileGetLinebreakType</c> will be used.
/// <para/>
/// Can be zero. In this case, the return value indicates the required buffer size (in characters).
/// </param>
/// <param name="iBufSize">
/// The size, in bytes, of the buffer pointed to by <c>pBuf</c>.<para/>
/// Can be zero. In this case, the return value indicates the required buffer size (in characters).
/// </param>
/// <returns>
/// If both <c>pBuf</c> and <c>iBufSize</c> are nonzero, the return value indicates the count of
/// characters written to the buffer, including the terminating zero.<para />
/// If either <c>pBuf</c> or <c>iBufSize</c> is set to zero, the return value indicates the required
/// size of the buffer, in characters, including the terminating zero.
/// </returns>
RLTEXT_API rlText_Count EXPORT rlText_FileGetAsSingleString(
	rlText_File  oFile,
	char        *pBuf,
	rlText_Count iBufSize
);

/// <summary>Set the entire contents of text data from a single string.</summary>
/// <param name="oFile">The handle to the text data.</param>
/// <param name="sz">
/// The [UTF-8 encoded] new contents of the text data.<para/>
/// The previous style of linebreaks will be kept and all linebreaks will be converted accordingly.
/// </param>
/// <returns>
/// If the function succeeds, it returns nonzero.<para/>
/// Otherwise, it returns zero.
/// </returns>
RLTEXT_API rlText_Bool EXPORT rlText_FileSetAsSingleString(
	rlText_File oFile,
	const char *sz
);

/// <summary>Delete all lines from text data.</summary>
/// <param name="oFile">The handle of the text data.</param>
/// <returns>
/// If the function succeeds, it returns nonzero.<para/>
/// Otherwise, it returns zero.
/// </returns>
RLTEXT_API void EXPORT rlText_FileClear(
	rlText_File oFile
);






#endif // RLTEXTDLL_FILEIO