//
//  dictionaries/scan_dictionary.h
//  Created by Patrick Rügheimer on 24.06.18.
//

#ifndef REPROMATIC_SCAN_DICTIONARY_H
#define REPROMATIC_SCAN_DICTIONARY_H

#include <string>  // std::string
#include "dictionaries/dictionary.h"
#include "pages/scan_page.h"
#ifndef MAC_PLATFORM
  #include "PIHeaders.h"
#endif

namespace repromatic {

class ScanDictionary : public Dictionary<repromatic::ScanPage> {
public:
  std::string GetKey() const ;
  bool Contains(std::string key) const;
};

}  // repromatic

#endif
