#ifndef FORMAT_H
#define FORMAT_H

#include <vector>
#include <string>
#include <map>
#include <utility>  // std::pair

struct format_spec {
        std::wstring name = L"";
        float width = -1.0;
        float height = -1.0;

        format_spec::format_spec() {};
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
private:
        std::wstring active_format;
        static std::vector<int> predefined_formats;
        std::map<std::wstring, format> list;

public:
        formats();
        void set_active_format(std::wstring format);
        format get_active_format() const;
};

#endif