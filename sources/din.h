//	
//  din.h
//  Created by ptrckr on 24.03.18.	
//	

#ifndef REPROMATIC_DIN_H
#define REPROMATIC_DIN_H

#include <map>
#include "convert_print_units.h"

namespace repromatic {
namespace din {

extern const float tolerance;

struct Format {
  float width;
  float height;
  int size;
  bool landscape;
};

extern const std::map<int, Format> formats;

bool GetFormat(float width, float height, Format &format_ref);
bool GetFormatBySize(int size, Format &format_ref);

}  // din
}  // repromatic

#endif
