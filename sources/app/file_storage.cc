#include "app/file_storage.h"

#include "utils/convert.h"  // wide_to_narrow_str(), to_lowercase(), pt_to_mm()
#include "utils/acro.h"  // wstr_to_as_path()
#include "utils/win.h"  // alert()
#include "format.h"

#include <vector>
#include <string>
#include <map>
#include <filesystem>  // std::tr2::sys, fs::path, .string(), .extension(), .parent_path()
#include <algorithm>  // std::find_if(), std::swap()
#include <sstream>
#include <iomanip>  // std::setprecision
#include <ios>  // std::fixed
#pragma comment(lib, "shlwapi.lib")
#include "shlwapi.h"  // StrCmpLogicalW()
#include "PIHeaders.h"

namespace fs = std::tr2::sys;

bool string_logical_cmp::operator() (const std::wstring &lhs, const std::wstring &rhs) const {
        return StrCmpLogicalW(lhs.c_str(), rhs.c_str()) == -1 ? true : false;
}

page::page(PDPage pd_page, int number) : number(number)
{
        this->set_size(pd_page);
}

void page::set_size(PDPage pd_page)
{
        ASFixedRect cropbox;
        PDPageGetCropBox(pd_page, &cropbox);
        this->size = {
                pt_to_mm(ASFixedToFloat(cropbox.right - cropbox.left)),
                pt_to_mm(ASFixedToFloat(cropbox.top - cropbox.bottom))
        };

        // TODO: Set formatted size

        PDRotate rotation = PDPageGetRotate(pd_page);
        if (rotation == 90 || rotation == 270) {
                std::swap(this->size.width, this->size.height);
        }
}

int page::get_display_page_number() const
{
        return this->number + 1;
}

std::wstring page::to_string() const
{
        std::wstringstream tmp;
        tmp << std::fixed << std::setprecision(0);
        
        tmp << L"#" << this->number << L"{" << this->formatted_size.name << L"}";
        tmp << L" [" << this->size.width << L" x " << this->size.height << L"]";

        return tmp.str();
}

file::file(fs::wpath path) : path(path)
{
        as_path as_path(path);

        if (!as_path.is_valid()) {
                this->exists = false;
                alert(L"Couldn't create ASPathName from path `" + path.string() + L"`.");
                return;
        }

        DURING
                PDDoc pd_doc = PDDocOpen(as_path.path, NULL, NULL, true);
                
                for (int i = 0; i < PDDocGetNumPages(pd_doc); ++i) {
                        PDPage pd_page = PDDocAcquirePage(pd_doc, i);
                        page page(pd_page, i);
                        
                        this->pages.push_back(page);

                        PDPageRelease(pd_page);
                }

                PDDocClose(pd_doc);
        HANDLER
                this->is_damaged = true;

                // TODO: Handle error/damaged file.
                char buffer[1024];
                memset(buffer, 0, 1024);  
                const char *msg = ASGetErrorString(ERRORCODE, buffer, 1024);
                MessageBoxA(NULL, msg, "error", MB_OK);
        END_HANDLER
}

std::wstring file::to_string(int level) const
{
        std::wstring tmp;
        
        tmp += std::wstring(level, ' ') + L" | " + this->path.filename() + L": ";

         for (auto &it = this->pages.begin(); it != this->pages.end(); ++it) {
                tmp.append(it->to_string());

                if (it != --this->pages.end())
                        tmp.append(L", ");
        }

        tmp += L"\n";

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
                auto file = fs::wdirectory_iterator(path);
                for (file; file != fs::wdirectory_iterator(); ++file) {
                        this->add_file(file->path());        
                }
        }
}

void file_tree::iterate(std::map<std::wstring, folder,
        string_logical_cmp> &folders, std::function<void(folder&)> cb)
{
        for (auto &root : folders) {
                auto &root_folder = root.second;

                cb(root_folder);

                file_tree::iterate(root_folder.folders, cb);
        }
}

void file_tree::apply_format_to_pages(format format)
{
        file_tree::iterate(this->drives, [&format](folder &folder) {
                for (auto &file : folder.files) {
                        for (auto &page : file.second.pages) {
                                page.formatted_size = format::get_formatted_size(page.size, format);  
                        }
                }
        });
}