//	
//  page.cc
//  Created by ptrckr on 24.03.18.	
//	

#include "page.h"
#include <string>
#include <sstream>
#include <algorithm>
#include <iomanip>

#ifndef MAC_PLATFORM
  #include "PIHeaders.h"
#endif

#include "repromatic_utils.h"
#include "convert_print_units.h"
#include "din.h"

namespace repromatic {

Page::Page(PDPage pd_page, int number) : number(number) {
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

std::string Page::GetDinKey() const {
  if (format.fits_in_din_size != -1 && format.fits_in_din_size >= 3) {
    return std::string("A") + std::to_string(format.fits_in_din_size);
  } else {
    return "Plan";
  }
}

std::string Page::GetSizeKey() const {
  std::stringstream size_key;

  size_key << std::fixed << std::setprecision(0);
  size_key << ConvertPrintUnits::PointToMm(format.width);
  size_key << " × ";
  size_key << ConvertPrintUnits::PointToMm(format.height);
  size_key << " mm";

  return size_key.str();
}

int Page::GetDisplayPageNumber() const {
  return number + 1;
}

}  // repromatic