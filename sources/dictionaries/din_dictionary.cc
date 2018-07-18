//
//  dictionaries/din_dictionary.cc
//  Created by Patrick Rügheimer on 24.06.18.
//

#include "dictionaries/din_dictionary.h"
#include "pages/din_page.h"
#include <string>  // std::string
#include <vector>  // std::vector
#include <algorithm>  // std::unique, std::sorts
#ifndef MAC_PLATFORM
  #include "PIHeaders.h"
#endif

namespace repromatic {

std::string DinDictionary::GetKey() const {
  std::string key;
  std::vector<std::string> keys;

  for (const DinPage &page : pages) {
    keys.push_back(page.GetKey());
  }

  std::sort(keys.begin(), keys.end());
  keys.erase(std::unique(keys.begin(), keys.end()), keys.end());

  for (auto i = keys.begin(); i != keys.end(); ++i) {
    key += *i;

    if (i != --keys.end()) key += ", ";
  }

  return key;
};

bool DinDictionary::Contains(std::string key) const {
  std::vector<std::string> keys;

  for (const DinPage &page : pages) {
    if (page.GetKey() == key) {
      return true;
    }
  }

  return false;
};

}  // repromatic
