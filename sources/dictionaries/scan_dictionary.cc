//
//  dictionaries/scan_dictionary.cc
//  Created by Patrick Rügheimer on 24.06.18.
//

#include "dictionaries/scan_dictionary.h"
#include <string>  // std::string
#ifndef MAC_PLATFORM
  #include "PIHeaders.h"
#endif

namespace repromatic {

std::string ScanDictionary::GetKey() const {
  return "foo";
};

bool ScanDictionary::Contains(std::string key) const {
  return true;
};

}  // repromatic
