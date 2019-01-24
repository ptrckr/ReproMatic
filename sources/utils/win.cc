#include "utils/win.h"

#include "utils/convert.h"  // narrow_to_wide_str()
#include "resources.h"

#include <string>
#include <filesystem>  // std::tr2::sys, fs::path, .filename()
#include <stdexcept>
#include "windows.h"  // MessageBoxW(), LoadStringW()
#include "PIMain.h"  // gHINSTANCE

namespace fs = std::tr2::sys;

void _alert(std::wstring msg, const char *file, const long line)
{
        fs::wpath _file(narrow_to_wide_str(std::string(file)));
        std::wstring _line = std::to_wstring(line);

        MessageBoxW(NULL, msg.c_str(),
                (L"[" + _file.filename() + L"#" + _line + L"]").c_str(), MB_OK);   
}

std::wstring load_wstr(int resource_id)
{
        const wchar_t *buffer;
        int buffer_size = LoadStringW(gHINSTANCE, resource_id,
                reinterpret_cast<wchar_t*>(&buffer), 0);

        if(buffer_size == 0)
                throw std::runtime_error(std::string("Resource string with id `") +
                        std::to_string(resource_id) + "' could not be loaded.");

        return std::wstring(buffer, buffer_size);
}