//
//  pages/scan_page.cc
//  Created by Patrick Rügheimer on 25.06.18.
//

#include "pages/scan_page.h"
#include <cmath>  // std::ceil
#include <string>  // std::to_string
#include <iomanip>  // std::setprecision
#include <ios>  // std::fixed
#include <sstream>  // std::stringstream
#include <utility>  // std::swap
#include <algorithm>  // std::max
#include "pages/page.h"
#include "convert_print_units.h"
#include "repromatic_utils.h"  // repromatic::IsRectWithinBounds
#ifndef MAC_PLATFORM
  #include "PIHeaders.h"
#endif

namespace repromatic {

ScanPage::ScanPage(PDPage pd_page, int num) : tolerance(15.0f), Page(pd_page, num) {}

std::string ScanPage::GetKey() const {
  float width = ConvertPrintUnits::PointToMm(format.width);
  float height = ConvertPrintUnits::PointToMm(format.height);

  // Width must always be larger than height
  if (height > width) {
    std::swap(width, height);
  }

  if (repromatic::IsRectWithinBounds(width, height, 297.0f, 210.0f, tolerance)) {
    return "DIN A4";
  } else if (repromatic::IsRectWithinBounds(width, height, 420.0f, 297.0f, tolerance)) {
    return "DIN A3";
  } else {
    std::stringstream key;
    key << std::fixed << std::setprecision(0);
    key << GetSpecificHeight(height) / 10.0f;
    key << " cm";

    return key.str();
  }
}

std::string ScanPage::GetSize() const {
  std::stringstream page_size;

  float width = ConvertPrintUnits::PointToMm(format.width);
  float height = ConvertPrintUnits::PointToMm(format.height);

  // Width must always be larger than height
  if (height > width) {
    std::swap(width, height);
  }

  if (repromatic::IsRectWithinBounds(width, height, 297.0f, 210.0f, tolerance)) {
    // Does fit in A4?
  } else if (repromatic::IsRectWithinBounds(width, height, 420.0f, 297.0f, tolerance)) {
    // Does fit in A3?
  } else {
    width = GetSpecificWidth(width) / 10.0f;
    height = GetSpecificHeight(height) / 10.0f;
  }

  page_size << std::fixed << std::setprecision(0);
  page_size << width;  // cm
  page_size << " × ";
  page_size << height;  // cm

  return page_size.str();
}

float ScanPage::GetSpecificWidth(float width) const {
  return (std::max)(std::ceil((width - tolerance) / 100) * 100, 600.0f);
}

float ScanPage::GetSpecificHeight(float height) const {
  if (height - tolerance <= 297.0f) {
    return 300.0f;
  } else if (height - tolerance <= 594.0f) {
    return 600.0f;
  } else if (height - tolerance <= 914.0f) {
    return 900.0f;
  } else {
    return 1060.0f;
  }
}

}  // repromatic
