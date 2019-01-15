#include "app/classes/file_storage.h"

#include "utils/convert.h"  // wide_to_narrow_str(), to_lowercase()

#include <vector>
#include <string>
#include <map>
#include <filesystem>  // std::tr2::sys, fs::path, .string(), .extension(), .parent_path()
#include <algorithm>  // std::find_if()

namespace fs = std::tr2::sys;

#ifndef MAC_PLATFORM
#include "PIHeaders.h"
#pragma comment(lib, "shlwapi.lib")
#include "shlwapi.h"  // StrCmpLogicalW()
#endif

bool string_logical_cmp::operator() (const std::wstring &lhs, const std::wstring &rhs) const {
        return StrCmpLogicalW(lhs.c_str(), rhs.c_str()) == -1 ? true : false;
}

file::file(fs::wpath path) : path(path)
{
        ASPathName as_path = ASFileSysCreatePathName(
                ASGetDefaultUnicodeFileSys(),
                ASAtomFromString("Cstring"),
                wide_to_narrow_str(path.string()).c_str(),
                NULL
        );

        if (as_path == NULL) {
                MessageBoxW(NULL, L"Could not create acrobat path.", L"", MB_OK);
                return;
        }

        DURING
                PDDoc pd_doc = PDDocOpen(as_path, ASGetDefaultUnicodeFileSys(), NULL, true);
                
                ASInt32 page_count = PDDocGetNumPages(pd_doc);

                MessageBoxW(
                        NULL,
                        (std::wstring(L"Document has ") + std::to_wstring(page_count) + L" pages.").c_str(),
                        path.string().c_str(),
                        MB_OK                        
                );

                PDDocClose(pd_doc);
        HANDLER
                // TODO: Handle error/damged file.
                char buffer[1024];
                memset(buffer, 0, 1024);  
                const char *msg = ASGetErrorString(ERRORCODE, buffer, 1024);
                MessageBoxA(NULL, msg, std::to_string(ERRORCODE).c_str(), MB_OK);
        END_HANDLER

        ASFileSysReleasePath(nullptr, as_path);
}

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
        auto folders = this->folders;

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
                        auto &root_sub_folders =
                                (component == path.begin() ? this->drives : root_folder->second.folders);

                        auto folder_exists = root_sub_folders.find(*component);
                        if (folder_exists == root_sub_folders.end()) {
                                auto tmp = root_sub_folders.emplace(*component, current_path);
                                root_folder = tmp.first;
                        } else {
                                root_folder = folder_exists;
                        }
                } else if (fs::is_regular_file(current_path) && to_lowercase(path.extension()) == L".pdf") {
                        auto &root_sub_files = root_folder->second.files;

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