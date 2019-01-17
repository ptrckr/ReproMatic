#include "convert.h"

#include <locale>  // std::wstring_convert
#include <codecvt>  // std::codecvt_utf8
#include <algorithm>  // std::transform
#include <ctype.h>  // towlower
#include <string>

#include "windows.h"

std::string wide_to_narrow_str(std::wstring wide_str) {
        size_t buffer_size = WideCharToMultiByte(CP_ACP, 0, wide_str.c_str(), -1, NULL, 0, NULL, NULL);
        std::string narrow_str(buffer_size, 0);
        WideCharToMultiByte(CP_ACP, 0, wide_str.c_str(), -1, &narrow_str[0], buffer_size, NULL, NULL);

        return narrow_str;
}

std::wstring to_lowercase(std::wstring s)
{
	std::transform(s.begin(), s.end(), s.begin(), towlower);
	return s;
}

std::string to_lowercase(std::string s)
{
	std::transform(s.begin(), s.end(), s.begin(), tolower);
	return s;
}