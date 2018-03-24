//	
//  page_dictionary.h
//  Created by ptrckr on 24.03.18.	
//	

#ifndef REPROMATIC_PAGE_DICTIONARY_H
#define REPROMATIC_PAGE_DICTIONARY_H

#include <vector>
#include <string>
#include "page.h"
#include "din_wrapper.h"

namespace repromatic {

class PageDictionary {
  std::vector<Page> pages;

  public:
    DinWrapper dinWrapper;

    void AddPagesFrom(PDDoc pd_doc);

    std::string GetDinKey();
};

}  // repromatic

#endif