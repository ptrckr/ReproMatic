//
//  dictionaries/dictionary.h
//  Created by Patrick Rügheimer on 24.06.18.
//

#ifndef REPROMATIC_DICTIONARY_H
#define REPROMATIC_DICTIONARY_H

#include <vector>
#include <string>
#include "pages/page.h"
#ifndef MAC_PLATFORM
  #include "PIHeaders.h"
#endif

namespace repromatic {

template<typename T>
class Dictionary {
protected:
  std::vector<T> pages;

public:
  void AddPagesFrom(PDDoc pd_doc);
  virtual std::string GetKey() const = 0;
  virtual bool Contains(std::string key) const = 0;
  std::string Stringify(bool print_page_number = true, bool print_page_format = true) const;
  std::vector<T> GetPages() const;
};

}  // repromatic

#endif
