#pragma once
#ifndef RLTEXTDLL_EXPORTSPECS
#define RLTEXTDLL_EXPORTSPECS





#ifdef __cplusplus
#define RLTEXT_API extern "C"
#else
#define RLTEXT_API extern
#endif

#if defined RLTEXT_EXPORT
#define EXPORT __declspec(dllexport) __stdcall
#else
#define EXPORT __declspec(dllimport)
#endif





#endif // RLTEXTDLL_EXPORTSPECS