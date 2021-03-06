﻿#ifndef FILE_STORAGE_H
#define FILE_STORAGE_H

#include "format.h"
#include "size.h"

#include <vector>
#include <string>
#include <map>
#include <filesystem>
#include <functional>
#include "PIHeaders.h"

namespace fs = std::tr2::sys;

struct string_logical_cmp {
        bool operator() (const std::wstring &lhs, const std::wstring &rhs) const;
};

struct page {
        int number;
        size size;
        format_spec formatted_size;

        page(PDPage pd_page, int number);
        
        void set_size(PDPage pd_page);
        int get_display_page_number() const;
        std::wstring to_string() const;
};

struct file {
        fs::wpath path;
        std::vector<page> pages;

        bool exists = true;
        bool is_damaged = false;
        bool is_protected;

        file(fs::wpath path);
        file(std::wstring path) : file(fs::wpath(path)) {};

        std::wstring to_string(int level) const;
};

struct folder {
        fs::wpath path;
        std::map<std::wstring, folder, string_logical_cmp> folders;
        std::map<std::wstring, file, string_logical_cmp> files;

        folder(fs::wpath path) : path(path) {};
        folder(std::wstring path) : folder(fs::wpath(path)) {};

        std::wstring to_string(int level = 0) const;
};

struct file_tree {
        std::map<std::wstring, folder, string_logical_cmp> drives;
        void add_file(std::wstring _path);
        static void iterate(std::map<std::wstring, folder,
                string_logical_cmp> &map, std::function<void(folder&)> cb);
        void apply_format_to_pages(format format);
};

#endif