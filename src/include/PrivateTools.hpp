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


char16_t FlipByteOrder(char16_t ch) noexcept;

char32_t FlipByteOrder(char32_t ch) noexcept;

unsigned BOMLen(rlText_Encoding iEncoding) noexcept;





#endif // RLTEXTDLL_PRIVATETOOLS