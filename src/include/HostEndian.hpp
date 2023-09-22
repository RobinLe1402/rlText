#pragma once
#ifndef RLTEXTDLL_HOSTENDIAN
#define RLTEXTDLL_HOSTENDIAN



constexpr bool bHostIsBigEndian = (const char&)0x00FF == 0x00;



#endif // RLTEXTDLL_HOSTENDIAN