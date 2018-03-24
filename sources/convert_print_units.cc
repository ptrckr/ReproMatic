//	
//  convert_print_units.cc
//  Created by ptrckr on 24.03.18.	
//	

#include "convert_print_units.h"

namespace repromatic {

float ConvertPrintUnits::PointToMm(float pt) {
  return pt * 0.352777778f;
}

float ConvertPrintUnits::MmToPoint(float mm) {
  return mm * 2.83464567f;
}

}  // repromatic
