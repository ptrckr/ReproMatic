//	
//  page_dictionary.h
//  Created by ptrckr on 24.03.18.	
//	

#ifndef REPROMATIC_PAGE_DICTIONARY_H
#define REPROMATIC_PAGE_DICTIONARY_H

#include <vector>
#include <string>
#include "page.h"

namespace repromatic {

class PageDictionary {
  std::vector<Page> pages;

  public:
    void AddPagesFrom(PDDoc pd_doc);

    std::string GetDinKey();

    std::string ToString(bool print_sizes = true, bool print_pages = true);
};

}  // repromatic

#endif