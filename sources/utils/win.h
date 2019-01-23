#ifndef UTILS_WIN_H
#define UTILS_WIN_H

#include <string>
#include "windows.h"

#define alert(msg) _alert((msg), __FILE__, __LINE__)
void _alert(std::wstring msg, const char *file, const long line);

std::wstring load_wstr(int resource_id);

#endif