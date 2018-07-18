//
//  dictionaries/din_dictionary.h
//  Created by Patrick Rügheimer on 24.06.18.
//

#ifndef REPROMATIC_DIN_DICTIONARY_H
#define REPROMATIC_DIN_DICTIONARY_H

#include <string>  // std::string
#include "dictionaries/dictionary.h"
#include "pages/din_page.h"
#ifndef MAC_PLATFORM
  #include "PIHeaders.h"
#endif

namespace repromatic {

class DinDictionary : public Dictionary<repromatic::DinPage> {
public:
  std::string GetKey() const ;
  bool Contains(std::string key) const;
};

}  // repromatic

#endif
