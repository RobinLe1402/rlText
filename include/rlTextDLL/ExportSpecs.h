#pragma once
#ifndef RLTEXTDLL_EXPORTSPECS
#define RLTEXTDLL_EXPORTSPECS





#ifdef __cplusplus
#define RLTEXT_API extern "C"
#else
#define RLTEXT_API extern
#endif

#if defined RLTEXT_COMPILE
#define RLTEXT_LIB __declspec(dllexport) __stdcall
#else
#define RLTEXT_LIB __declspec(dllimport)
#endif





#endif // RLTEXTDLL_EXPORTSPECS