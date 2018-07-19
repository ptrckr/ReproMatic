//
//  dictionaries/dictionary.cc
//  Created by Patrick Rügheimer on 24.06.18.
//

#include "dictionaries/dictionary.h"
#include <vector>  // std::vector
#include <string>  // std::string
#include <sstream>  // std::stringstream
#include "pages/page.h"
#include "pages/din_page.h"
#include "pages/scan_page.h"
#ifndef MAC_PLATFORM
  #include "PIHeaders.h"
#endif

namespace repromatic {

template<typename T>
void Dictionary<T>::AddPagesFrom(PDDoc pd_doc) {
  for (int i = 0; i < PDDocGetNumPages(pd_doc); ++i) {
    PDPage pd_page = PDDocAcquirePage(pd_doc, i);
    T page = {pd_page, i};

    pages.push_back(page);

    PDPageRelease(pd_page);
  }
}

template<typename T>
std::string Dictionary<T>::Stringify() const {
  bool print_sizes = true;
  bool print_pages = true;

  std::map<std::string, std::map<std::string, std::vector<int>>> sorted_pages;

  for (const T &page : pages) {
    sorted_pages[page.GetKey()][page.GetSize()].push_back(
      page.GetDisplayPageNumber()
    );
  }

  std::stringstream out;

  for (auto i = sorted_pages.begin(); i != sorted_pages.end(); ++i) {
    const std::string &page_key = i->first;
    const auto &sizes = i->second;

    int size_page_count = 0;
    for (const auto &size_pages : sizes) {
      size_page_count += static_cast<int>(size_pages.second.size());
    }

    if (!print_sizes) out << "• ";
    out << page_key << ": " << size_page_count << " Page";
    out << (size_page_count > 1 ? "s" : "") << std::endl;

    if (print_sizes) {
      for (auto j = sizes.begin(); j != sizes.end(); ++j) {
        const std::string &j_key = j->first;
        const auto &j_value = j->second;

        out << "• " << j_key << " (" << j_value.size() << ")";

        if (print_pages) {
          out << " @ [";

          int start = *j_value.begin();
          auto next = ++j_value.begin();
          int range = 0;
          for (auto k = j_value.begin(); k != j_value.end(); ++k) {
            if (next != j_value.end() && start + 1 + range == *next) {
              ++range;
              ++next;
              continue;
            }

            out << start;
            if (range > 0) out << "-" << start + range;
            if (k != --j_value.end()) out << ", ";

            if (next != j_value.end()) start = *next++;
            range = 0;
          }

          out << "]";
        }

        if (sizes.size() > 20) {
          if (j != --sizes.end()) {
            out << ", ";
          } else {
            out << std::endl;
          }
        } else{
          out << std::endl;
        }

        if (i != --sorted_pages.end() && j == --sizes.end()) out << std::endl;
      }
    }
  }

  return out.str();
}

template<typename T>
std::vector<T> Dictionary<T>::GetPages() const {
  return pages;
}

template class Dictionary<Page>;
template class Dictionary<DinPage>;
template class Dictionary<ScanPage>;

}  // repromatic
