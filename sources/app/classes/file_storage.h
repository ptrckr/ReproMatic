#ifndef FILE_STORAGE_H
#define FILE_STORAGE_H

#include <vector>
#include <string>
#include <map>
#include <filesystem>

namespace fs = std::tr2::sys;

#ifndef MAC_PLATFORM
#include "PIHeaders.h"
#endif

struct page {
        int number;
};

struct file {
        fs::wpath path;
        std::vector<page> pages;

        bool is_damaged;
        bool is_protected;

        file(std::wstring path) : path(path) {}
        file(fs::wpath path) : path(path) {}

        std::wstring to_string(int level) const;
};

struct folder {
        fs::wpath path;
        std::map<std::wstring, folder> folders;
        std::map<std::wstring, file> files;

        folder(std::wstring path) : path(path) {}
        folder(fs::wpath path) : path(path) {}

        bool contains_all_files;
        std::wstring to_string(int level = 0) const;
};

struct file_tree {
        std::map<std::wstring, folder> drives;

        void add_file(std::wstring _path);
};


#endif