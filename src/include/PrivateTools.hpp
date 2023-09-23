#pragma once
#ifndef RLTEXTDLL_PRIVATETOOLS
#define RLTEXTDLL_PRIVATETOOLS






#include "rlTextDLL/FileIO.h"
#include "rlTextDLL/Types.h"
#include "HostEndian.hpp"



#ifdef _WIN32

#include <string>

std::wstring UTF8toUTF16(const char *szUTF8) noexcept;

#endif



bool ByteOrderFlipNecessary(rlText_Encoding iEndianFlag) noexcept;


rlText_UTF16Char FlipByteOrder(rlText_UTF16Char ch) noexcept;

rlText_UTF32Char FlipByteOrder(rlText_UTF32Char ch) noexcept;

unsigned BOMLen(rlText_Encoding iEncoding) noexcept;





#endif // RLTEXTDLL_PRIVATETOOLS