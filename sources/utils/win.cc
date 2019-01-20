#include "utils/win.h"

#include "utils/convert.h"  // narrow_to_wide_str()

#include <string>
#include <filesystem>  // std::tr2::sys, fs::path, .filename()

#include "windows.h"

namespace fs = std::tr2::sys;

void _alert(std::wstring msg, const char *file, const long line)
{
        fs::wpath _file(narrow_to_wide_str(std::string(file)));
        std::wstring _line = std::to_wstring(line);

        MessageBoxW(NULL, (L"[" + _file.filename() + L"#" + _line + L"]\n\n" + msg).c_str(), L"Alert", MB_OK);   
}