#ifndef UTILS_ACRO_H
#define UTILS_ACRO_H

#include <string>

#include <filesystem>  // std::tr2::sys, fs::path, .string()

#ifndef MAC_PLATFORM
#include "PIHeaders.h"
#endif

namespace fs = std::tr2::sys;

struct as_path {
        ASPathName path;

        as_path(std::wstring wstr);
        as_path(fs::wpath wpath) : as_path(wpath.string()) {};

        bool is_valid();

        ~as_path();
};

#endif