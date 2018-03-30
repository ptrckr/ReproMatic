//	
//  page.h
//  Created by ptrckr on 24.03.18.	
//	

#ifndef REPROMATIC_PAGE_H
#define REPROMATIC_PAGE_H

#include <string>
#include "din.h"

#ifndef MAC_PLATFORM
#include "PIHeaders.h"
#endif  

namespace repromatic {

struct PageFormat {
  float width;
  float height;
  int fits_in_din_size;
  bool is_landscape;

  bool is_din;
  int din_size;
};

struct Page {
  int number;
  PageFormat format;
    
  Page(PDPage pd_page, int number);
  std::string GetDinKey() const;
  std::string GetSizeKey() const;
  int GetDisplayPageNumber() const;
};

}  // repromatic

#endif