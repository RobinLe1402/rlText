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
#define RLTEXT_FILEENCODING_UTF16LE (RLTEXT_FILEENCODING_UTF16 | RLTEXT_FILEENCODING_FLAG_LITTLE_ENDIAN)
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



typedef struct
{
	uintmax_t iCharCount;         // the total count of Unicode codepoints in the file.
	uintmax_t iLineBreakCount[3]; // the count of linebreaks for all of the linebreak types.
} rlText_FileStatisticsStruct;



/// <summary>Get information about a text file.</summary>
/// <param name="szFilepath">The path of the text file.</param>
/// <param name="pEncoding">
/// If this parameter is not zero, the pointed-to variable will be set to the encoding of the text
/// file.
/// </param>
/// <param name="pStatistics">
/// If this parameter is not zero, the pointed-to variable will receive statistical data of the text
/// file.
/// </param>
/// <returns>
/// If the method succeeds, it returns nonzero.<para />
/// If it fails, it returns zero.
/// </returns>
RLTEXT_API rlText_Bool EXPORT rlText_GetFileInfo(
	const wchar_t               *szFilepath,
	uint8_t                     *pEncoding,
	rlText_FileStatisticsStruct *pStatistics,
	uint8_t                      iFlags
);






#endif // RLTEXTDLL_FILEIO