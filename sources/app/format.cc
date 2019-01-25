#include "app/format.h"

#include "utils/string.h"  // split()
#include "utils/win.h"  // alert(), get_wstr_resource()
#include "utils/convert.h"  // wide_to_narrow_str()
#include "utils/acro.h"  // get_user_format_path()
#include "resources.h"

#include <vector>
#include <string>  // std::stof()
#include <utility>  // std::pair
#include <stdexcept>
#include <fstream>  // std::ifstream

format_spec::format_spec(std::wstring spec)
{ 
        for (const std::wstring &kv_str : split(spec, L",")) {
                auto kv_pair = split_once(kv_str, L"=");
                auto key = kv_pair.first;
                auto value = kv_pair.second;

                if (key.size() == 0)
                        throw std::invalid_argument(std::string("Error parsing format_spec:") +
                                " `key' must not be empty in `" + wide_to_narrow_str(kv_str) + "'.");

                if (value.size() == 0)
                        throw std::invalid_argument(std::string("Error parsing format_spec:") +
                                " `value' must not be empty in `" + wide_to_narrow_str(kv_str) + "'.");

                if (key == L"name")
                        this->name = value;
                else if (key == L"width")
                        this->width = std::stof(value);
                else if (key == L"height")
                        this->height = std::stof(value);
        }

        if (this->name.size() == 0 || this->width == -1.0 || this->height == -1.0) {
                throw std::runtime_error(std::string("Error initializing format_spec: Not all required") +
                        " values were provided in `" + wide_to_narrow_str(spec) + "'.");
        }
}

format::format(std::wstring config)
{
        for (const std::wstring &token : split(config, L"\n")) {
                if (token.size() == 0)
                        continue;

                auto type_desc = split_once(token, L":");
                if (type_desc.first == L"format")
                        this->parse_format_line(type_desc.second);
                else if (type_desc.first == L"spec")
                        this->parse_spec_line(type_desc.second);
                else if (type_desc.first == L"file_size_display")
                        this->parse_file_size_display_line(type_desc.second);
        }
        
        if (this->name.size() == 0)
                throw std::invalid_argument(std::string("Error parsing format:") +
                        " No name is specified in `" + wide_to_narrow_str(config) + "'.");
}

void format::parse_format_line(std::wstring line)
{
        for (const std::wstring &desc : split(line, L",")) {
                auto key_value = split_once(desc, L"=");

                if (key_value.first == L"name")
                        this->name = key_value.second;
                else if(key_value.first == L"tolerance")
                        this->tolerance = std::stof(key_value.second);
                else
                        throw std::invalid_argument("Error while parsing format: `" +
                                wide_to_narrow_str(key_value.first) + "' is not a valid key.");
        } 
}

void format::parse_spec_line(std::wstring line)
{
        try {
                format_spec spec(line);
                this->specs.push_back(spec);
        } catch(const std::exception &e) {
                alert(narrow_to_wide_str(e.what()));    
        }  
}

void format::parse_file_size_display_line(std::wstring line)
{
        this->file_size_display = line;
}

std::vector<int> formats::predefined_formats = {
        // TODO: Add (more) standard formats.
        IDS_DIN_A
};

formats::formats()
{
        try {
                auto file = fs::wdirectory_iterator(get_user_format_path());
                for (file; file != fs::wdirectory_iterator(); ++file) {
                        if(std::ifstream stream{file->path().string(), std::ios::binary | std::ios::ate}) {
                                auto size = stream.tellg();
                                std::string contents(static_cast<unsigned int>(size), '\0');
                                stream.seekg(0);
                                if(stream.read(&contents[0], size)) {
                                        format format(narrow_to_wide_str(contents));
                                        this->list.emplace(format.name, format);
                                } else {
                                        throw std::runtime_error(std::string("Unable to read contents of `") +
                                                wide_to_narrow_str(file->path().string()) + "'.");
                                }
                        } else {
                                throw std::runtime_error(std::string("Unable to open file `") +
                                        wide_to_narrow_str(file->path().string()) + "' for reading.");
                        }
                }
        } catch(const std::exception &e) {
               alert(narrow_to_wide_str(e.what()));  
        }

        for (const int &id : formats::predefined_formats) {
                try {
                        format format(load_wstr(id));             
                        this->list.emplace(format.name, format);  
                } catch(const std::exception& e) {
                        alert(narrow_to_wide_str(e.what()));    
                }
        }

        try {
                this->set_active_format(load_wstr(IDS_DEFAULT_FORMAT));       
        } catch(const std::exception &e) {
                alert(narrow_to_wide_str(e.what()));
        }
}

void formats::set_active_format(std::wstring format)
{
        if (this->list.find(format) != this->list.end())
                this->active_format = format;
        else
                throw std::invalid_argument(std::string("Could not find format `") +
                        wide_to_narrow_str(format) + "'. Did not set active format.");
}

format formats::get_active_format() const
{
        return list.at(this->active_format);
}