#ifndef FORMAT_H
#define FORMAT_H

#include "size.h"

#include <vector>
#include <string>
#include <map>
#include <utility>  // std::pair

struct format_spec {
        std::wstring name;
        float width;
        float height;

        format_spec() {};
        format_spec(std::wstring spec);
};

struct format {
        std::wstring name;
        float tolerance = 0.0;
        std::vector<format_spec> specs;
        std::wstring file_size_display;

        format(std::wstring config);
        void parse_format_line(std::wstring line);
        void parse_spec_line(std::wstring line);
        void parse_file_size_display_line(std::wstring line);

        static format_spec get_formatted_size(size size, format format);
};

struct formats {
private:
        std::wstring active_format;
        static std::vector<int> predefined_formats;
        std::map<std::wstring, format> list;

public:
        formats();
        void load_predefined_formats();
        void load_user_defined_formats();

        void set_active_format(std::wstring format);
        format get_active_format() const;
        std::vector<std::wstring> get_formats() const;
};

#endif