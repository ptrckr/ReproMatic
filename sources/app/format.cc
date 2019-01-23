#include "app/format.h"

#include "utils/string.h"  // split()
#include "utils/win.h"  // alert(), get_wstr_resource()
#include "utils/convert.h"  // wide_to_narrow_str()
#include "resources.h"

#include <vector>
#include <string>  // std::stof()
#include <utility>  // std::pair
#include <stdexcept>

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
                        continue; // TODO
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
        } catch(const std::exception& e) {
                alert(narrow_to_wide_str(e.what()));    
        }  
}

void format::parse_file_size_display_line(std::wstring line)
{
        // TODO
}

std::vector<int> formats::predefined_formats = {
        IDS_DIN_A
};

formats::formats()
{
        for (const int &id : formats::predefined_formats) {
                try {
                        // TODO: Read user defined formats from installation path.
                        // TODO: Add (more) standard formats.
                        format format(load_wstr(id));             
                        this->list.emplace(format.name, format);  
                } catch(const std::exception& e) {
                        alert(narrow_to_wide_str(e.what()));    
                }
        }
}