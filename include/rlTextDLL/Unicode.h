#pragma once
#ifndef RLTEXTDLL_UNICODE
#define RLTEXTDLL_UNICODE





#include "ExportSpecs.h"
#include "Types.h"



/// <summary>
/// Is a Unicode character located inside of a private use area?
/// </summary>
RLTEXT_API rlText_Bool EXPORT rlText_IsPrivateUse(rlText_UTF32Char ch);

/// <summary>
/// Is a value a noncharacter in Unicode?
/// </summary>
RLTEXT_API rlText_Bool EXPORT rlText_IsNoncharacter(rlText_UTF32Char ch);





#endif // RLTEXTDLL_UNICODE