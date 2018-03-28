//	
//  din_wrapper.cc
//  Created by ptrckr on 24.03.18.	
//	

#include "din_wrapper.h"
#include <map>
#include <vector>
#include <string>
#include <sstream>

namespace repromatic {

void DinWrapper::AddFormat(std::string din_key, std::string size_key, int page_num) {
  if (formats.find(din_key) != formats.end()) {
    std::map<std::string, std::vector<int>> size_key_map;

    formats.insert(
      std::pair<std::string, std::map<std::string, std::vector<int>>>(
        din_key, size_key_map
      )
    );
  }

  if (formats[din_key].find(size_key) != formats[din_key].end()) {
    std::vector<int> page_numbers;

    formats[din_key].insert(
      std::pair<std::string, std::vector<int>>(
        size_key, page_numbers
      )
    );
  }

  formats[din_key][size_key].push_back(page_num);
}

std::string DinWrapper::ToString(bool print_sizes, bool print_pages) {
  std::stringstream o;
  
  for (auto i = formats.begin(); i != formats.end(); ++i) {
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
            if (start + 1 + range == *next) {
              ++range;
              ++next;
              continue;
            }
            
            o << start;
            if (range > 0) o << "-" << start + range;
            if (k != --j_value.end()) o << ", ";
            
            start = *next++;
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
  
        if (i != --formats.end() && j == --i_value.end()) o << std::endl;
      }
    }
  }
  
  return o.str();
}

}  // repromatic