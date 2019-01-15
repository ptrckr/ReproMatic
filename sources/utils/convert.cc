#include "convert.h"

#include <locale>  // std::wstring_convert
#include <codecvt>  // std::codecvt_utf8
#include <algorithm>  // std::transform
#include <ctype.h>  // towlower
#include <string>

std::string wide_to_narrow_str(std::wstring wide_string) {
  using convert_type = std::codecvt_utf8<wchar_t>;
  std::wstring_convert<convert_type, wchar_t> converter;

  return converter.to_bytes(wide_string);
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