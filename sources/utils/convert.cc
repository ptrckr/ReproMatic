﻿#include "convert.h"

#include <locale>  // std::wstring_convert
#include <codecvt>  // std::codecvt_utf8
#include <algorithm>  // std::transform, std::remove()
#include <ctype.h>  // towlower
#include <string>

#include "windows.h"

std::string wide_to_narrow_str(std::wstring wstr) {
        size_t buffer_size = WideCharToMultiByte(CP_ACP, 0, wstr.c_str(), -1, NULL, 0, NULL, NULL);
        char *buffer = new char[buffer_size];
        WideCharToMultiByte(CP_ACP, 0, wstr.c_str(), -1, buffer, buffer_size, NULL, NULL);
        std::string str(buffer);
        delete[] buffer;

        return str;
}

std::wstring narrow_to_wide_str(std::string str) {
        size_t buffer_size = MultiByteToWideChar(CP_ACP, 0, str.c_str(), -1, NULL, 0);
        wchar_t *buffer = new wchar_t[buffer_size];
        MultiByteToWideChar(CP_ACP, 0, str.c_str(), -1, buffer, buffer_size);
        std::wstring wstr(buffer);
        delete[] buffer;

        return wstr;
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

float pt_to_mm(float pt) {
        return pt * 0.352777778f;
}

float mm_to_pt(float mm) {
        return mm * 2.83464567f;
}