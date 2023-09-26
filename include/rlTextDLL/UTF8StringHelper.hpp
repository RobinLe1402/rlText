#pragma once
#ifndef RLTEXTDLL_UTF8STRINGHELPER_CPP
#define RLTEXTDLL_UTF8STRINGHELPER_CPP





#include <memory>
#include <string>
#include "Types.hpp"



namespace rlText
{

#if __cplusplus >= 202002L // C++20


	inline std::string ToString(const std::u8string s) noexcept
	{
		std::string sResult(s.length(), 0);
		for (size_t i = 0; i < s.length(); ++i)
		{
			sResult[i] = s[i];
		}
		return sResult;
	}

	inline std::u8string ToU8String(const std::string s) noexcept
	{
		std::u8string sResult(s.length(), 0);
		for (size_t i = 0; i < s.length(); ++i)
		{
			sResult[i] = s[i];
		}
		return sResult;
	}


#else

	inline std::string&& ToString(const std::string&& s) noexcept
	{
		return std::move(s);
	}

	inline std::string&& ToU8String(const std::string&& s) noexcept
	{
		return std::move(s);
	}

#endif

}





#endif // RLTEXTDLL_UTF8STRINGHELPER_CPP