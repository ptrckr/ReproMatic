#include "app/format.h"

#include "utils/err.h"
#include "utils/string.h"
#include "utils/convert.h"
#include "utils/win.h"
#include "size.h"

#include <stdexcept>
#include <string>
#include <algorithm>
#include <cmath>
#include <limits>
#include <utility>

namespace repromatic {
namespace format {

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

float size_convertion::convert(std::wstring convertion, float to_convert) const
{
        if (convertion.empty())
                return to_convert;

        for (const auto &kvp_str : split(convertion, L"|")) {
                const auto kvp = split_once(kvp_str, L" ");
                const auto key = kvp.first;
                const auto val = kvp.second;

                if (key == L"divide_by")
                        to_convert /= std::stof(val);
                else if (key == L"times")
                        to_convert *= std::stof(val);
                else if (key == L"min")
                        to_convert = (std::max)(std::stof(val), to_convert);
                else if (key == L"ceil")
                        to_convert = std::ceil(to_convert);
                else if (key == L"steps") {
                        for (const auto &step : split(val, L"/")) {
                                float fstep = std::stof(step);

                                if (to_convert <= fstep) {
                                        to_convert = fstep;
                                        break;
                                }
                        }
                }

        }

        return to_convert;
}

size_convertion::size_convertion(std::wstring config) : size_convertion()
{
        for (const std::wstring &kvp_str : split(config, L",")) {
                auto kvp = split_once(kvp_str, L"=");
                auto key = kvp.first;
                auto val = kvp.second;

                if (key == L"width")
                        this->width = val;
                else if (key == L"height")
                        this->height = val;
                else if (key == L"orientation")
                        this->orientation = val;
                else
                        throw std::invalid_argument(
                                std::string("Error constructing ") +
                                "format::size_convertion: `" + w_to_str(key) +
                                "' is not a valid key."
                        );
        }
}

repromatic::size size_convertion::apply_to(repromatic::size size) const
{
        repromatic::size converted = size;

        BEGIN_ASSERT
        converted.width = this->convert(this->width, size.width);
        converted.height = this->convert(this->height, size.height);
        CATCH_AND_ALERT

        return converted;
}

std::wstring size_convertion::get_orientation() const
{
        return this->orientation;
}

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

tolerance::tolerance(std::vector<std::wstring> vals)
{
        if (vals.size() != 3)
                throw std::invalid_argument(std::string("Error constructing ") +
                        "tolerance, must provide three arguments.");

        from = std::stof(vals.at(0));
        to = vals.at(1) == L"-1" ?
                (std::numeric_limits<float>::max)() : std::stof(vals.at(1));
        variation = std::stof(vals.at(2));
}

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

series_size::series_size(std::wstring line) : name(L""), size{0.0f, 0.0f}
{ 
        for (const std::wstring &kvp_str : split(line, L",")) {
                auto kvp = split_once(kvp_str, L"=");
                auto key = kvp.first;
                auto val = kvp.second;

                if (key.empty() || val.empty())
                        throw std::invalid_argument(
                                std::string("Error constructing format::size") +
                                ", key and value `" + w_to_str(kvp_str) +
                                "' must not be empty."
                        );

                if (key == L"name")
                        this->name = val;
                else if (key == L"width")
                        this->size.width = std::stof(val);
                else if (key == L"height")
                        this->size.height = std::stof(val);
                else if (key == L"convertion") {
                        this->use_series_convertion = false;

                        if (val != L"false")
                                this->convertion = {val};
                }
        }

        if (this->name.empty() || this->size.width == 0.0f ||
                this->size.height == 0.0f) {
                throw std::runtime_error(
                        std::string("Error constructing format::size, ") +
                        "missing parameters in `" + w_to_str(line) + "'."
                );
        }
}

std::wstring series_size::get_name() const
{
        return this->name;
}

repromatic::size series_size::get_size() const
{
        return this->size;
}

float series_size::get_width() const
{
        return this->size.width;
}

float series_size::get_height() const
{
      return this->size.height;  
}

bool series_size::fits(repromatic::size size, repromatic::size tolerance,
        bool rotation_allowed) const
{        
        if (size.width - tolerance.width <= this->size.width &&
                size.width + tolerance.width >= this->size.width &&
                size.height - tolerance.height <= this->size.height &&
                size.height + tolerance.height >= this->size.height) {
                return true;
        } else if (rotation_allowed &&
                size.width - tolerance.width <= this->size.height &&
                size.width + tolerance.width >= this->size.height &&
                size.height - tolerance.height <= this->size.width &&
                size.height + tolerance.height >= this->size.width) {
                return true;
        }

        return false;
}

bool series_size::surrounds(repromatic::size size, repromatic::size tolerance,
        bool rotation_allowed) const
{
        if (size.width - tolerance.width <= this->size.width &&
                size.height - tolerance.height <= this->size.height) {
                return true;
        } else if (rotation_allowed &&
                size.width - tolerance.width <= this->size.height &&
                size.height - tolerance.height <= this->size.width) {
                return true;
        }

        return false;
}

bool series_size::fits_variable(repromatic::size size,
        repromatic::size tolerance, bool rotation_allowed) const
{
        if (this->size.width == format::size::IGNORE_VALUE &&
                this->size.height == format::size::IGNORE_VALUE) {
                return true;
        }

        if (this->size.height == format::size::IGNORE_VALUE) {
                if (size.width - tolerance.width <= this->size.width &&
                        size.width + tolerance.width >= this->size.width) {
                        return true;
                } else if (rotation_allowed &&
                        size.width - tolerance.width <= this->size.height &&
                        size.width + tolerance.width >= this->size.height) {
                        return true;
                }
        }

        if (this->size.width == format::size::IGNORE_VALUE) {
                if (size.height - tolerance.height <= this->size.height &&
                        size.height + tolerance.height >= this->size.height) {
                        return true;
                } else if (rotation_allowed &&
                        size.height - tolerance.height <= this->size.width &&
                        size.height + tolerance.height >= this->size.width) {
                        return true;
                }
        }

        return false;
}

bool series_size::surrounds_variable(repromatic::size size,
        repromatic::size tolerance, bool rotation_allowed) const
{
        if (this->size.width == format::size::IGNORE_VALUE &&
                this->size.height == format::size::IGNORE_VALUE) {
                return true;
        }

        if (this->size.height == format::size::IGNORE_VALUE) {
                if (size.width - tolerance.width <= this->size.width) {
                        return true;
                } else if (rotation_allowed &&
                        size.width - tolerance.width <= this->size.height) {
                        return true;
                }
        }

        if (this->size.width == format::size::IGNORE_VALUE) {
                if (size.height - tolerance.height <= this->size.height) {
                        return true;
                } else if (rotation_allowed &&
                        size.height - tolerance.height <= this->size.width) {
                        return true;
                }
        }

        return false;
}

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

series::series(std::wstring config)
{
        for (const std::wstring &line : split(config, L"\n")) {
                if (line.empty())
                        continue;

                auto kvp = split_once(line, L":");
                auto key = kvp.first;
                auto val = kvp.second;

                if (key == L"name")
                        this->name = val;
                else if (key == L"tolerance")
                        this->parse_tolerance_line(val);
                else if (key == L"size")
                        this->parse_size_line(val);
                else if (key == L"convertion")
                        this->parse_convertion_line(val);
                else
                        throw std::invalid_argument("Error parsing series: `" +
                                w_to_str(key) + "' is not a valid key.");
        }
        
        if (this->name.empty())
                throw std::invalid_argument(
                        std::string("Error parsing series: ") +
                        "No name is specified in `" + w_to_str(config) + "'."
                );
}

void series::parse_tolerance_line(std::wstring line)
{
        for (const auto &token : split(line, L",")) {
                BEGIN_ASSERT
                this->tolerances.push_back(split(token, L" "));
                CATCH_AND_ALERT
        }
}

void series::parse_size_line(std::wstring line)
{
        BEGIN_ASSERT
        this->sizes.push_back(line);
        CATCH_AND_ALERT
}

void series::parse_convertion_line(std::wstring line)
{
        BEGIN_ASSERT
        this->convertion = {line};
        CATCH_AND_ALERT
}

float series::get_tolerance_variation(float val) const
{
        for (const auto &tolerance : this->tolerances) {
                if (val >= tolerance.from && val <= tolerance.to)
                        return tolerance.variation;
        }

        return 0.0f;
}

series series::from_str(std::wstring config)
{
        return series(config);
}

converted_size series::convert_size(repromatic::size size)
{
        converted_size converted{L"UNDEFINED", size};

        for (const auto &series_size : this->sizes) {
                repromatic::size tmp = size;
                bool rotation_allowed = false;
                size_convertion tmp_convertion = 
                        series_size.use_series_convertion ?
                        this->convertion : series_size.convertion;

                if (tmp_convertion.get_orientation() == L"width_larger") {
                        if (tmp.width < tmp.height)
                                std::swap(tmp.width, tmp.height);
                } else if (tmp_convertion.get_orientation() == L"height_larger") {
                        if (tmp.height < tmp.width)
                                std::swap(tmp.width, tmp.height);
                } else if (tmp_convertion.get_orientation() == L"unlocked") {
                        rotation_allowed = true;
                }

                repromatic::size tolerance = {
                        this->get_tolerance_variation(series_size.get_width()),
                        this->get_tolerance_variation(series_size.get_height())
                };

                if (series_size.fits(size, tolerance, rotation_allowed) ||
                    series_size.surrounds(size, tolerance, rotation_allowed) ||
                    series_size.fits_variable(size, tolerance, rotation_allowed) ||
                    series_size.surrounds_variable(size, tolerance, rotation_allowed)) {
                        converted.series_size_name = series_size.get_name();
                        converted.converted = tmp_convertion.apply_to(size);
                        break;
                }
        }

        return converted;
}

std::wstring series::get_name() const
{
        return this->name;
}

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

}
}