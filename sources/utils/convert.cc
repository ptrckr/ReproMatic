#include "convert.h"

#include <locale>  // std::wstring_convert
#include <codecvt>  // std::codecvt_utf8, 
#include <string>

std::string WideToNarrowString(std::wstring wide_string) {
  using convert_type = std::codecvt_utf8<wchar_t>;
  std::wstring_convert<convert_type, wchar_t> converter;

  return converter.to_bytes(wide_string);
}