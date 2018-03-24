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

struct Page {
  int number;
  float width;
  float height;
  bool is_din;
  din::Format din;
    
  Page(PDPage pd_page, int page_num);
  std::string GetDinKey();
  std::string GetSizeKey();
  int GetDisplayPageNumber();
};

}  // repromatic

#endif