#include "utils/acro.h"

#include "utils/convert.h"  // wide_to_narrow_str()

#include <string>
#include <algorithm>  // std::erase()
#include <filesystem>  // std::tr2::sys, fs::wpath, .parent_path()
#include "PIHeaders.h"

namespace fs = std::tr2::sys;

as_path::as_path(std::wstring wstr)
{
        this->path = ASFileSysCreatePathName(
                NULL,
                ASAtomFromString("Cstring"),
                wide_to_narrow_str(wstr).c_str(),
                NULL
        );
}

bool as_path::is_valid()
{
        return this->path != NULL;
}

as_path::~as_path()
{
        ASFileSysReleasePath(NULL, this->path);
}

fs::wpath get_user_format_path()
{
        size_t buffer_size = MAX_PATH;
        std::wstring buffer(buffer_size, '\0');
        DWORD data_copied_size = static_cast<DWORD>(buffer_size);

        auto retval = RegGetValueW(HKEY_CLASSES_ROOT, L"SOFTWARE\\Adobe\\Acrobat\\Exe", L"",
                RRF_RT_REG_SZ, nullptr, static_cast<void*>(&buffer[0]), &data_copied_size);

        if (retval == ERROR_SUCCESS) {
                buffer.resize(static_cast<size_t>(data_copied_size / sizeof(wchar_t)));
                buffer.erase(std::remove(buffer.begin(), buffer.end(), '"'), buffer.end());
                fs::wpath path(buffer);                
                std::wstring plug_ins_dir = L"plug_ins";
                path.remove_filename().append(plug_ins_dir.begin(), plug_ins_dir.end());

                if (fs::exists(path) && fs::is_directory(path))  {
                        std::wstring user_formats = L"repromatic_formats";
                        path.append(user_formats.begin(), user_formats.end());

                        return path;
                } else {
                        throw std::runtime_error(std::string("Error calling get_user_format_path,") +
                                " the constructed path `" + wide_to_narrow_str(path.string()) +
                                "' does not exist or is not a valid directory.");
                }
        } else {
                throw std::runtime_error(std::string("Error calling RegGetValueW,") +
                        " the windows error code is `" + std::to_string(retval) + "'.");
        }
}