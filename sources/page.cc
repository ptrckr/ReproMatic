//	
//  page.cc
//  Created by ptrckr on 24.03.18.	
//	

#include "page.h"
#include <string>
#include <sstream>
#include <algorithm>
#include <iomanip>
#include "repromatic.h"
#include "convert_print_units.h"
#include "din.h"

#ifndef MAC_PLATFORM
  #include "PIHeaders.h"
#endif

namespace repromatic {

Page::Page(PDPage pd_page, int page_num) : number(page_num) {
  PDRotate rotated_by = PDPageGetRotate(pd_page);

  ASFixedRect cropbox;
  PDPageGetCropBox(pd_page, &cropbox);
  width = ASFixedToFloat(cropbox.right - cropbox.left);
  height = ASFixedToFloat(cropbox.top - cropbox.bottom);

  if (rotated_by == 90 || rotated_by == 270) {
    std::swap(width, height);
  }

  din::Format cropbox_din_format;
  if (din::GetFormat(width, height, cropbox_din_format)) {
    // Page conforms to a din format.
    is_din = true;
    din = cropbox_din_format;
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
      is_din = true;
      din = visible_bbox_din_format;
      width = visible_bbox_width;
      height = visible_bbox_height;
    } else {
      // Page does not conform to any din format.
      is_din = false;
    }
  }
}

std::string Page::GetDinKey() {
  float tolerance = ConvertPrintUnits::MmToPoint(5);

  din::Format A3;
  if (din::GetFormatBySize(3, A3)) {
    if (is_din) {
      if (din.size >= 3) {
        return "A" + std::to_string(din.size) + (din.landscape ? " Landscape" : "");
      } else {
        return "Plan";
      }
    } else {
      if (IsRectWithinBounds(width, height, A3.width, A3.height, tolerance)) {
        return "Small Format";
      } else {
        return "Plan";
      }
    }
  } else {
    return "Unknown";
  }
}

std::string Page::GetSizeKey() {
  std::stringstream size_key;

  size_key << std::fixed << std::setprecision(0);
  size_key << ConvertPrintUnits::PointToMm(width);
  size_key << " × ";
  size_key << ConvertPrintUnits::PointToMm(height);
  size_key << " mm";

  return size_key.str();
}

int Page::GetDisplayPageNumber() {
  return number + 1;
}

}  // repromatic