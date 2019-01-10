#ifndef FILE_STORAGE_H
#define FILE_STORAGE_H

#include <vector>
#include <string>
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
};

struct folder {
	fs::wpath path;
	std::vector<file> files;

	bool contains_all_files;
};

struct file_storage {
	std::vector<folder> folders;

	void add_file(std::wstring _path);
	std::wstring GetAddedFiles();
};

#endif