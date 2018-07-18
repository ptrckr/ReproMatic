//
//  pages/din_page.h
//  Created by Patrick Rügheimer on 25.06.18.
//

#ifndef REPROMATIC_DIN_PAGE_H
#define REPROMATIC_DIN_PAGE_H

#include <string>  // std::string
#include "pages/page.h"
#ifndef MAC_PLATFORM
  #include "PIHeaders.h"
#endif

namespace repromatic {

struct DinPage : public Page {
public:
  DinPage(PDPage pd_page, int num);
  std::string GetKey() const;
};

}  // repromatic

#endif
