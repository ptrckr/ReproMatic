//	
//  convert_print_units.h	
//  Created by ptrckr on 24.03.18.	
//	

#ifndef REPROMATIC_CONVERT_PRINT_UNITS_H
#define REPROMATIC_CONVERT_PRINT_UNITS_H

namespace repromatic {

struct ConvertPrintUnits {
  static float PointToMm(float pt);
  static float MmToPoint(float mm);
};

}  // repromatic

#endif