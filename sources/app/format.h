#ifndef FORMAT_H
#define FORMAT_H

#include <vector>
#include <string>
#include <map>
#include <utility>  // std::pair

struct format_spec {
        std::wstring name;
        float width = -1.0;
        float height = -1.0;

        format_spec::format_spec(std::wstring spec);
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
};

struct formats { 
        static std::vector<int> predefined_formats;
        std::map<std::wstring, format> list;

        formats();
};

#endif