//	
//  din_wrapper.cc
//  Created by ptrckr on 24.03.18.	
//	

#include "din_wrapper.h"
#include <map>
#include <vector>
#include <string>

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

std::string DinWrapper::ToString(bool print_pages) {
  std::string output;

  std::map<std::string, std::map<std::string, std::vector<int>>>::iterator it;
  for (it = formats.begin(); it != formats.end(); it++) {
    output += it->first + ", ";

    int format_total_count = 0;
    std::map<std::string, std::vector<int>>::iterator it2;
    for (it2 = formats[it->first].begin(); it2 != formats[it->first].end(); it2++) {
      format_total_count += it2->second.size();
    }

    output += std::to_string(format_total_count) + " Page" + (format_total_count > 1 ? "s" : "") + "\n";

    for (it2 = formats[it->first].begin(); it2 != formats[it->first].end(); it2++) {
      output += "• " + it2->first + " (" + std::to_string(it2->second.size()) + ")";

      if (print_pages) {
        output += " @ [";

        int start = 0;
        int steps = 0;
        for (unsigned int i = 0; i < it2->second.size(); i++) {
          if (i < it2->second.size() - 1 && it2->second[start] + steps == it2->second[i + 1] - 1) {
            steps++;
          } else {
            if (steps > 0) {
              output += std::to_string(it2->second[i - steps]) + "-" + std::to_string(it2->second[i]);
            } else {
              output += std::to_string(it2->second[i]);
            }

            if (i < it2->second.size() - 1) output += ", ";

            start = i + 1;
            steps = 0;
          }
        }

        output += "]";
      }

      output += "\n";
    }

    if (it != --formats.end()) output += "\n";
  }

  return output;
}

}  // repromatic