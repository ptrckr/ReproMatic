#include "app/classes/file_storage.h"

#include "utils/convert.h"  // to_lowercase()

#include <vector>
#include <string>
#include <map>
#include <filesystem>  // std::tr2::sys, fs::path, .string(), .extension(), .parent_path()
#include <algorithm>  // std::find_if()

namespace fs = std::tr2::sys;

std::wstring file::to_string(int level) const
{
        std::wstring tmp;
        
        tmp += std::wstring(level, ' ') + L" | " + this->path.filename() + L"\n";

        return tmp;
}

std::wstring folder::to_string(int level) const
{
        ++level;
        std::wstring tmp;
        std::map<std::wstring, folder> folders = this->folders;

        for(auto folder = folders.cbegin(); folder != folders.cend(); ++folder) {
                tmp += std::wstring(level, L'-') + L" " + std::wstring(folder->first) + L"\n";
                
                auto files = folder->second.files;
                for (auto file = files.cbegin(); file != files.cend(); ++file) {
                        tmp += file->second.to_string(level);
                }

                tmp += folder->second.to_string(level);
        }

        return tmp;
}

void file_tree::add_file(std::wstring _path)
{
        fs::wpath path(_path);
        fs::wpath current_path;
        std::map<std::wstring, folder>::iterator root_folder;

        for (auto component = path.begin(); component != path.end(); ++component) {
                if (*component == L"/" || *component == L"\\")
                        continue;

                current_path.append((*component).begin(), (*component).end());
                if (fs::is_directory(path) || component != --path.end())
                current_path /= L"/";

                if (fs::is_directory(current_path)) {
                        std::map<std::wstring, folder> &root_sub_folders =
                                (component == path.begin() ? this->drives : root_folder->second.folders);

                        auto folder_exists = root_sub_folders.find(*component);
                        if (folder_exists == root_sub_folders.end()) {
                                auto tmp = root_sub_folders.emplace(*component, current_path);
                                root_folder = tmp.first;
                        } else {
                                root_folder = folder_exists;
                        }
                } else if (fs::is_regular_file(current_path) && to_lowercase(path.extension()) == L".pdf") {
                        std::map<std::wstring, file> &root_sub_files = root_folder->second.files;

                        if (root_sub_files.find(*component) == root_sub_files.end()) {
                                root_sub_files.emplace(*component, current_path);
                        }
                }
        }

        if (fs::is_directory(path)) {
                for (auto file = fs::wdirectory_iterator(path); file != fs::wdirectory_iterator(); ++file) {
                        this->add_file(file->path());        
                }
        }
}