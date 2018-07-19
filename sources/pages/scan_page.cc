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
#include "pages/page.h"
#include "convert_print_units.h"
#ifndef MAC_PLATFORM
  #include "PIHeaders.h"
#endif

namespace repromatic {

ScanPage::ScanPage(PDPage pd_page, int num) : Page(pd_page, num) {}

std::string ScanPage::GetKey() const {
  std::stringstream key;

  key << std::fixed << std::setprecision(0);
  key << ConvertPrintUnits::PointToMm(GetSpecificHeight()) / 10;  // cm
  key << " cm";

  return key.str();
}

std::string ScanPage::GetSize() const {
  std::stringstream page_size;

  page_size << std::fixed << std::setprecision(0);
  page_size << ConvertPrintUnits::PointToMm(GetSpecificWidth()) / 10;  // cm
  page_size << " × ";
  page_size << ConvertPrintUnits::PointToMm(GetSpecificHeight()) / 10;  // cm

  return page_size.str();
}

float ScanPage::GetSpecificWidth() const {
  return ConvertPrintUnits::MmToPoint(std::ceil(ConvertPrintUnits::PointToMm(format.width) / 100) * 100);
}

float ScanPage::GetSpecificHeight() const {
  float height;

  if (format.height <= ConvertPrintUnits::MmToPoint(300.0f)) {
    height = ConvertPrintUnits::MmToPoint(300.0f);
  } else if (format.height <= ConvertPrintUnits::MmToPoint(600.f)) {
    height = ConvertPrintUnits::MmToPoint(600.0f);
  } else if (format.height <= ConvertPrintUnits::MmToPoint(900.0f)) {
    height = ConvertPrintUnits::MmToPoint(900.0f);
  } else {
    height = ConvertPrintUnits::MmToPoint(1200.0f);
  }

  return height;
}

}  // repromatic
