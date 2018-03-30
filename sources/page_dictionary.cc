//	
//  page_dictionary.cc
//  Created by ptrckr on 24.03.18.	
//	

#include "page_dictionary.h"
#include <vector>
#include <string>
#include <algorithm>
#include <sstream>
#include "page.h"

namespace repromatic {

void PageDictionary::AddPagesFrom(PDDoc pd_doc) {
  for (int i = 0; i < PDDocGetNumPages(pd_doc); ++i) {
    PDPage pd_page = PDDocAcquirePage(pd_doc, i);
    Page page = {pd_page, i};

    pages.push_back(page);

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

std::string PageDictionary::ToString(bool print_sizes, bool print_pages) {
  std::map<std::string, std::map<std::string, std::vector<int>>> ordered_pages;

  for (const Page &page : pages) {
    ordered_pages[page.GetDinKey()][page.GetSizeKey()].push_back(page.GetDisplayPageNumber());
  }

  std::stringstream o;
  
  for (auto i = ordered_pages.begin(); i != ordered_pages.end(); ++i) {
    const std::string &i_key = i->first;
    const auto &i_value = i->second;
    
    int page_count = 0;
    for (const auto &pages : i_value) {
      page_count += pages.second.size();
    }
    
    if (!print_sizes) o << "• ";
    o << i_key << ": " << page_count << " Page";
    o << (page_count > 1 ? "s" : "") << std::endl;
    
    if (print_sizes) {
      for (auto j = i_value.begin(); j != i_value.end(); ++j) {
        const std::string &j_key = j->first;
        const auto &j_value = j->second;
        
        o << "• " << j_key << " (" << j_value.size() << ")";
        
        if (print_pages) {
          o << " @ [";
          
          int start = *j_value.begin();
          auto next = ++j_value.begin();
          int range = 0;
          for (auto k = j_value.begin(); k != j_value.end(); ++k) {
            if (next != j_value.end() && start + 1 + range == *next) {
              ++range;
              ++next;
              continue;
            }
            
            o << start;
            if (range > 0) o << "-" << start + range;
            if (k != --j_value.end()) o << ", ";
            
            if (next != j_value.end()) start = *next++;
            range = 0;
          }
          
          o << "]";
        }
        
        if (i_value.size() > 20) {
          if (j != --i_value.end()) {
            o << ", ";
          } else {
            o << std::endl;
          }
        } else{
          o << std::endl;
        }
  
        if (i != --ordered_pages.end() && j == --i_value.end()) o << std::endl;
      }
    }
  }
  
  return o.str();
}

}  // repromatic