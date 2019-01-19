#ifndef CONVERT_H
#define CONVERT_H

#include <string>

std::string wide_to_narrow_str(std::wstring wstr);
std::wstring narrow_to_wide_str(std::string str);
std::wstring to_lowercase(std::wstring input);
std::string to_lowercase(std::string input);

#endif