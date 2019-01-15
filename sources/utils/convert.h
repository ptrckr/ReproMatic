#ifndef CONVERT_H
#define CONVERT_H

#include <string>

std::string wide_to_narrow_str(std::wstring wide_string);
std::wstring to_lowercase(std::wstring input);
std::string to_lowercase(std::string input);

#endif