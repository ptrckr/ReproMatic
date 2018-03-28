//	
//  din_wrapper.h
//  Created by ptrckr on 24.03.18.	
//	

#ifndef REPROMATIC_DIN_WRAPPER_H
#define REPROMATIC_DIN_WRAPPER_H

#include <map>
#include <vector>

namespace repromatic {

struct DinWrapper {
  std::map<std::string, std::map<std::string, std::vector<int>>> formats;

  void AddFormat(std::string din_key, std::string size_key, int page_num);

  std::string ToString(bool print_sizes = true, bool print_pages = true);
};

}  // repromatic

#endif