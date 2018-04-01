//	
//  acrobat_utils.h
//  Created by ptrckr on 24.03.18.	
//	

#ifndef REPROMATIC_ACROBAT_UTILS_H
#define REPROMATIC_ACROBAT_UTILS_H

#include <vector>
#include <map>
#include <string>

#ifndef MAC_PLATFORM
#include "PIHeaders.h"
#endif

namespace repromatic {
namespace acrobat_utils {

class MenuUtil {
private:
  AVMenubar app_menubar;
  AVMenu acrobat_main_menu;

  std::map<std::string, AVMenu> created_menus;
  std::vector<AVMenuItem> created_menu_items;

public:
  void Init();
  void ReleaseMenus();
  void RemoveMenuItems();
  AVMenu CreateMenu(std::string menu_name);
  void AddMenuItemToMenu(
    std::string menu,
    std::string menu_item_name,
    AVExecuteProc menu_item_callback, 
    AVComputeEnabledProc menu_item_is_enabled
  );
};

AVStatusMonitorProcsRec GetStatusMonitor();

}  // acrobat_utils
}  // repromatic

#endif