#ifndef UTILS_STRING_H
#define UTILS_STRING_H

#include <vector>
#include <string>

std::vector<std::wstring> split(std::wstring input, std::wstring delimiter);
std::pair<std::wstring, std::wstring> split_once(std::wstring input, std::wstring delimiter);

#endif