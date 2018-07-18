//
//  pages/scan_page.h
//  Created by Patrick Rügheimer on 25.06.18.
//

#ifndef REPROMATIC_SCAN_PAGE_H
#define REPROMATIC_SCAN_PAGE_H

#include <string>
#include "pages/page.h"
#ifndef MAC_PLATFORM
  #include "PIHeaders.h"
#endif

namespace repromatic {

struct ScanPage : public Page {
public:
  ScanPage(PDPage pd_page, int num);
  std::string GetKey() const;
  std::string GetSize() const;
  float GetSpecificHeight() const;
  float GetSpecificWidth() const;
};

}  // repromatic

#endif
