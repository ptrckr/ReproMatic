//
//  pages/din_page.cc
//  Created by Patrick Rügheimer on 25.06.18.
//

#include "pages/din_page.h"
#include <string>  // std::string, std::to_string
#include "pages/page.h"
#ifndef MAC_PLATFORM
  #include "PIHeaders.h"
#endif

namespace repromatic {

DinPage::DinPage(PDPage pd_page, int num) : Page(pd_page, num) {}

std::string DinPage::GetKey() const {
  if (format.fits_in_din_size != -1 && format.fits_in_din_size >= 3) {
    return std::string("A") + std::to_string(format.fits_in_din_size);
  } else {
    return "Plan";
  }
}

}  // repromatic
