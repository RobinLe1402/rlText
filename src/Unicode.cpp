#include "rlTextDLL/Unicode.h"

RLTEXT_API rlText_Bool EXPORT rlText_IsPrivateUse(rlText_UTF32Char ch)
{
	return ((ch >= 0xE000 && ch <= 0xF8FF) /* Private Use Zone */ ||
		(ch >= 0xF0000 && ch < 0x110000) /* Private Use Planes */);
}

RLTEXT_API rlText_Bool EXPORT rlText_IsNoncharacter(rlText_UTF32Char ch)
{
	return ((ch >= 0xFDD0 && ch <= 0xFDEF) /* noncharacter range in BMP */ ||
		(ch & 0xFFFF) >= 0xFFFE /* last two code points of each plane */);
}