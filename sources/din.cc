//
//  din.cc
//  Created by ptrckr on 24.03.18.	
//

#include "din.h"
#include <cmath>
#include <map>
#include <algorithm>
#include "convert_print_units.h"

namespace repromatic {
namespace din {

const float tolerance = ConvertPrintUnits::MmToPoint(10);

const std::map<int, Format> formats = []{
  std::map<int, Format> m;

  // Source: https://www.cl.cam.ac.uk/~mgk25/iso-paper.c
  float width = 1000.0f / std::sqrt(std::sqrt(2.0f)) + 0.5f;
  float height = std::sqrt(std::sqrt(2.0f)) * 1000.0f + 0.5f;

  for (int size = 0; size <= 10; ++size) {
    m.insert(std::pair<int, Format>(size, {
      ConvertPrintUnits::MmToPoint(std::floor(width)),
      ConvertPrintUnits::MmToPoint(std::floor(height)),
      size,
      false
    }));

    std::swap(width, height);
    width /= 2;
  }

  return m;
}();

bool GetFormat(float width, float height, Format &format_ref) {
  // TODO: Add exact tolerance values for every dimension.
  float tolerance = ConvertPrintUnits::MmToPoint(5);

  for (const auto &i : formats) {
    const Format &v = i.second;

    // Portrait Orientation
    if (width - tolerance < v.width && width + tolerance > v.width &&
      height - tolerance < v.height && height + tolerance > v.height) {
      format_ref = v;
      return true;
    }

    // Landscape Orientation
    if (width - tolerance < v.height && width + tolerance > v.height &&
      height - tolerance < v.width && height + tolerance > v.width) {
      format_ref = v;
      format_ref.landscape = true;
      return true;
    }
  }

  return false;
}

bool GetFormatBySize(int size, Format &format_ref) {
  std::map<int, Format>::const_iterator result = formats.find(size);

  if (result != formats.end()) {
    format_ref = result->second;
    return true;
  }

  return false;
}

}  // din
}  // repromatic
