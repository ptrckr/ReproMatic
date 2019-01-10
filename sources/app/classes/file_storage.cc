#include "app/classes/file_storage.h"

#include "utils/convert.h"

#include <vector>
#include <string>
#include <filesystem>  // string, extension, parent_path

namespace fs = std::tr2::sys;

// TODO: Check if a folder/file already got added.
void file_storage::add_file(std::wstring _path)
{
	folder folder;
	fs::wpath path(_path);

	if (fs::is_directory(path)) {
		folder.path = path;
		folder.contains_all_files = true;
	} else if (to_lowercase(path.extension()) == L".pdf") {
		folder.path = path.parent_path();
		// TODO: Count .pdf files and check if there is only one.
		folder.contains_all_files = false;

		file file;
		file.path = path;

		folder.files.push_back(file);
	} else {
		folder.path = path.parent_path();
	}

	this->folders.push_back(folder);
}

std::wstring file_storage::GetAddedFiles()
{
	std::wstring acc(L"");

	for (const auto& s : this->folders) {
		acc += L"Folder: " + s.path.string() + L"\n";

		for (const auto& f : s.files) {
			acc += L"File: " + f.path.string() + L"\n";
		}
	}

	return acc;
}