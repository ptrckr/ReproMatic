#ifndef UTILS_WIN_H
#define UTILS_WIN_H

#include <string>
#include <functional>
#include "windows.h"

#define alert(msg) _alert((msg), __FILE__, __LINE__)
void _alert(std::wstring msg, const char *file, const long line);

#define assert_rv(func_name, ret_val) _assert_rv((func_name), (ret_val), __FILE__, __LINE__)
void _assert_rv(std::wstring func_name, BOOL ret_val, const char *file, const long line);

#define assert_rv_cb(func_name, ret_val, callback) _assert_rv_cb((func_name), (ret_val), (callback), __FILE__, __LINE__)
void _assert_rv_cb(std::wstring func_name, BOOL ret_val, std::function<void()> callback, const char *file, const long line);

std::wstring load_wstr(int resource_id);

#endif