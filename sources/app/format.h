#ifndef FORMAT_H
#define FORMAT_H

#include "size.h"

#include <string>
#include <vector>

namespace repromatic {
namespace format {

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

class size_convertion {
        std::wstring width;
        std::wstring height;
        std::wstring orientation;

        float convert(std::wstring convertion, float in) const;

public:
        size_convertion() : width(L""), height(L""), orientation(L"") {};
        size_convertion(std::wstring config);
        repromatic::size apply_to(repromatic::size size) const;
        std::wstring get_orientation() const;
};

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

struct converted_size {
        std::wstring series_size_name;
        repromatic::size converted;
};

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

struct tolerance {
        float from;
        float to;
        float variation;

        tolerance(std::vector<std::wstring> vals);
};

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

typedef class series_size {
        std::wstring name;
        repromatic::size size;

public:
        static const int IGNORE_VALUE = -1;
        bool use_series_convertion = true;
        size_convertion convertion;

        series_size(std::wstring line);
        series_size(std::wstring name, float width, float height) :
                name(name), size{width, height} {};

        std::wstring get_name() const;
        repromatic::size get_size() const;
        float get_width() const;
        float get_height() const;

        bool fits(repromatic::size size, repromatic::size tolerance,
                bool rotation_allowed) const;
        bool surrounds(repromatic::size size, repromatic::size tolerance,
                bool rotation_allowed) const;
        bool fits_variable(repromatic::size size, repromatic::size tolerance,
                bool rotation_allowed) const;
        bool surrounds_variable(repromatic::size size,
                repromatic::size tolerance, bool rotation_allowed) const;
} size;

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

class series {
        std::wstring name;
        std::vector<repromatic::format::size> sizes;
        std::vector<tolerance> tolerances;
        size_convertion convertion;

        series(std::wstring config);
        void parse_tolerance_line(std::wstring line);
        void parse_size_line(std::wstring line);
        void parse_convertion_line(std::wstring line);
        float get_tolerance_variation(float val) const;

public:
        static series from_str(std::wstring config);
        converted_size convert_size(repromatic::size size);
        std::wstring get_name() const;
};

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

}
}

#endif