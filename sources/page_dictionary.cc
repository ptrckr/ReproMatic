//	
//  page_dictionary.cc
//  Created by ptrckr on 24.03.18.	
//	

#include "page_dictionary.h"
#include <vector>
#include <string>
#include <algorithm>
#include "page.h"

namespace repromatic {

void PageDictionary::AddPagesFrom(PDDoc pd_doc) {
  for (int i = 0; i < PDDocGetNumPages(pd_doc); ++i) {
    PDPage pd_page = PDDocAcquirePage(pd_doc, i);
    Page page = {pd_page, i};

    pages.push_back(page);
    dinWrapper.AddFormat(
      page.GetDinKey(),
      page.GetSizeKey(),
      page.GetDisplayPageNumber()
    );

    PDPageRelease(pd_page);
  }
}

std::string PageDictionary::GetDinKey() {
  std::vector<std::string> keys;
  std::string key;

  for (std::vector<Page>::iterator i = pages.begin(); i != pages.end(); ++i) {
    keys.push_back((*i).GetDinKey());
  }

  std::sort(keys.begin(), keys.end());
  keys.erase(std::unique(keys.begin(), keys.end()), keys.end());

  for (std::vector<std::string>::iterator i = keys.begin(); i != keys.end(); ++i) {
    key += *i;

    if (i != --keys.end()) key += ", ";
  }

  return key;
}

}  // repromatic