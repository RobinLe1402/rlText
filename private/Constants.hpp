#pragma once
#ifndef RLTEXT_PRIVATE_CONSTANTS
#define RLTEXT_PRIVATE_CONSTANTS





namespace rlText
{

	/// <summary>
	/// A table of the Unicode values of values 0x80 through 0x9F of Codepage 1252
	/// </summary>
	constexpr wchar_t cCP1252_Table[] =
	{
		L'\u20AC', L'\u0000', L'\u201A', L'\u0192', L'\u201E', L'\u2026', L'\u2020', L'\u2021',
		L'\u02C6', L'\u2030', L'\u0160', L'\u2039', L'\u0152', L'\u0000', L'\u017D', L'\u0000',
		L'\u0000', L'\u2018', L'\u2019', L'\u201C', L'\u201D', L'\u2022', L'\u2013', L'\u2014',
		L'\u02DC', L'\u2122', L'\u0161', L'\u203A', L'\u0153', L'\u0000', L'\u017E', L'\u0178'
	};

}





#endif // RLTEXT_PRIVATE_CONSTANTS