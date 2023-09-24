#pragma once
#ifndef RLTEXTDLL_UNICODE
#define RLTEXTDLL_UNICODE





#include "ExportSpecs.h"
#include "Types.h"



/// <summary>
/// Is a Unicode character located inside of a private use area?
/// </summary>
RLTEXT_API rlText_Bool RLTEXT_LIB rlText_IsPrivateUse(char32_t ch);

/// <summary>
/// Is a value a noncharacter in Unicode?
/// </summary>
RLTEXT_API rlText_Bool RLTEXT_LIB rlText_IsNoncharacter(char32_t ch);





#endif // RLTEXTDLL_UNICODE