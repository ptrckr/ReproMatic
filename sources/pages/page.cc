//
//  pages/page.cc
//  Created by ptrckr on 24.03.18.
//

#include "pages/page.h"
#include <string>
#include <sstream>
#include <algorithm>
#include <iomanip>  // std::setprecision
#include <ios>  // std::fixed
#include "repromatic_utils.h"
#include "convert_print_units.h"
#include "din.h"
#ifndef MAC_PLATFORM
  #include "PIHeaders.h"
#endif

namespace repromatic {

Page::Page(PDPage pd_page, int num) : number(num) {
  PDRotate rotated_by = PDPageGetRotate(pd_page);

  ASFixedRect cropbox;
  PDPageGetCropBox(pd_page, &cropbox);
  format.width = ASFixedToFloat(cropbox.right - cropbox.left);
  format.height = ASFixedToFloat(cropbox.top - cropbox.bottom);

  if (rotated_by == 90 || rotated_by == 270) {
    std::swap(format.width, format.height);
  }

  din::Format cropbox_din_format;
  if (din::GetFormat(format.width, format.height, cropbox_din_format)) {
    // Page conforms to a din format.
    format.is_din = true;
    format.din_size = cropbox_din_format.size;
    format.fits_in_din_size = format.din_size;
  } else {
    ASFixedRect visible_bbox;
    PDPageGetVisibleBBox(pd_page, NULL, true, &visible_bbox);
    float visible_bbox_width = ASFixedToFloat(visible_bbox.right - visible_bbox.left);
    float visible_bbox_height = ASFixedToFloat(visible_bbox.top - visible_bbox.bottom);

    if (rotated_by == 90 || rotated_by == 270) {
      std::swap(visible_bbox_width, visible_bbox_height);
    }

    din::Format visible_bbox_din_format;
    if (din::GetFormat(visible_bbox_width, visible_bbox_height, visible_bbox_din_format)) {
      // Page with whitespace cropped out conforms to a din format.
      format.width = visible_bbox_width;
      format.height = visible_bbox_height;
      format.is_din = true;
      format.din_size = visible_bbox_din_format.size;
      format.fits_in_din_size = format.din_size;
    } else {
      // Page does not conform to any din format.
      format.is_din = false;
      format.fits_in_din_size = -1;

      for (auto din_iterator = din::formats.rbegin(); din_iterator != din::formats.rend(); ++din_iterator) {
        const din::Format &din_format = din_iterator->second;

        if (IsRectWithinBounds(format.width, format.height, din_format.width, din_format.height, din::tolerance)) {
          format.fits_in_din_size = din_format.size;
          break;
        }
      }
    }
  }

  if (format.width > format.height) {
    format.is_landscape = true;
  } else {
    format.is_landscape = false;
  }
}

int Page::GetDisplayPageNumber() const {
  return number + 1;
}

std::string Page::GetKey() const {
  return "TODO";
}

std::string Page::GetSize() const {
  std::stringstream page_size;

  page_size << std::fixed << std::setprecision(0);
  page_size << ConvertPrintUnits::PointToMm(format.width);
  page_size << " × ";
  page_size << ConvertPrintUnits::PointToMm(format.height);
  page_size << " mm";

  return page_size.str();
}

float Page::GetSpecificWidth() const {
  return format.width;
}

float Page::GetSpecificHeight() const {
  return format.height;
}

}  // repromatic
