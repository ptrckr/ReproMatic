//
//  pages/page.h
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
  bool is_landscape;

  bool is_din;
  int din_size;
  int fits_in_din_size;
};

struct Page {
protected:
  int number;
  PageFormat format;

public:
  Page(PDPage pd_page, int num);
  int GetDisplayPageNumber() const;
  std::string GetKey() const;
  std::string GetSize() const;
  float GetSpecificHeight() const;
  float GetSpecificWidth() const;
};

}  // repromatic

#endif
