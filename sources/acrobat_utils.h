#ifndef REPROMATIC_ACROBAT_UTILS_H
#define REPROMATIC_ACROBAT_UTILS_H

#include <vector>
#include <map>
#include <string>

#ifndef MAC_PLATFORM
  #include "PIHeaders.h"
#endif

class MenuManager {
private:
  AVMenubar app_menubar;
  AVMenu acrobat_main_menu;

  std::map<std::wstring, AVMenu> created_menus;
  std::vector<AVMenuItem> created_menu_items;

public:
  void Init();
  void ReleaseMenus();
  void RemoveMenuItems();

  AVMenu CreateMenu(std::wstring menu_name);

  void AddMenuItemToMenu(
    std::wstring menu,
    std::wstring menu_item_name,
    AVExecuteProc menu_item_callback,
    AVComputeEnabledProc menu_item_is_enabled
  );
};

#endif
