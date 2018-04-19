//	
//  repromatic.h
//  Created by ptrckr on 24.03.18.	
//	

#ifndef REPROMATIC_H
#define REPROMATIC_H

#include <string>
#include <functional>
#include <vector>

#ifndef MAC_PLATFORM
#include "PIHeaders.h"
#endif

namespace repromatic {

bool IsRectWithinBounds(
  float src_width,
  float src_height,
  float ref_width,
  float ref_height,
  float tolerance
);

std::vector<std::string> QueryFolder(
  ASFileSys file_sys,
  ASFileSysItemType filter,
  ASPathName root, bool recursive = false,
  ASPathName folder = nullptr
);

void IterateFolder(
  ASFileSys file_system,
  ASPathName root,
  ASPathName folder,
  std::function<void(
    ASFileSys file_system,
    ASPathName root,
    ASPathName item_path,
    std::string item_type
  )> callback,
  std::function<bool()> continue_iteration = []() -> bool {return true;}
);

int GetItemCount(
  ASPathName root,
  ASFileSys file_system, 
  std::string filter = ""
);

ASPathName GetFirstPdfFileInFolder(
  ASFileSys file_system,
  ASPathName folder
);

}  // repromatic

#endif