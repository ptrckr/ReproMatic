#include "acrobat_utils.h"

#include <vector>
#include <map>
#include <string>
#include <exception>

#include "plugin_data.h"
#include "utils/convert.h"  // WideToNarrowString

#ifndef MAC_PLATFORM
  #include "PIMain.h"
#endif

void MenuManager::Init() {
  if (app_menubar = AVAppGetMenubar()) {
    acrobat_main_menu = AVMenubarAcquireMenuByName(
      app_menubar,
      WideToNarrowString(PluginData::FULL_DEV_PLUGIN_NAME).c_str()
    );

    if (!acrobat_main_menu) {
      acrobat_main_menu = AVMenuNew(
        WideToNarrowString(PluginData::FULL_PLUGIN_NAME).c_str(),
        WideToNarrowString(PluginData::FULL_DEV_PLUGIN_NAME).c_str(),
        gExtensionID
      );

      AVMenubarAddMenu(app_menubar, acrobat_main_menu, APPEND_MENU);
    }

    created_menus[L"acrobat_main_menu"] = acrobat_main_menu;
  }
}

void MenuManager::ReleaseMenus() {
  for(const auto &menu : created_menus) {
    AVMenuRelease(menu.second);
  }
}

void MenuManager::RemoveMenuItems() {
  for (AVMenuItem &menu_item : created_menu_items) {
    AVMenuItemRemove(menu_item);
  }
}

AVMenu MenuManager::CreateMenu(std::wstring menu_name) {
  if (menu_name.empty()) {
    return acrobat_main_menu;
  }

  auto menu_search = created_menus.find(menu_name);
  if (menu_search != created_menus.end()) {
    return menu_search->second;
  }

  AVMenu menu = AVMenuNew(
    WideToNarrowString(menu_name).c_str(),
    WideToNarrowString(PluginData::DEVELOPER_PREFIX + menu_name).c_str(),
    gExtensionID
  );

  AVMenuItem menu_item = AVMenuItemNew(
    WideToNarrowString(menu_name).c_str(),
    WideToNarrowString(PluginData::DEVELOPER_PREFIX + menu_name).c_str(),
    menu, true, NO_SHORTCUT, 0,
    NULL,
    gExtensionID
  );

  AVMenuAddMenuItem(acrobat_main_menu, menu_item, APPEND_MENUITEM);

  created_menus[menu_name] = menu;
  created_menu_items.push_back(menu_item);

  return menu;
}

void MenuManager::AddMenuItemToMenu(
    std::wstring menu,
    std::wstring menu_item_name,
    AVExecuteProc menu_item_callback,
    AVComputeEnabledProc menu_item_is_enabled) {
  AVMenu parent_menu = CreateMenu(menu);
  AVMenuItem menu_item = AVMenuItemNew(
    WideToNarrowString(menu_item_name).c_str(),
    WideToNarrowString(PluginData::DEVELOPER_PREFIX + menu_item_name).c_str(),
    NULL, true, NO_SHORTCUT, 0, NULL,
    gExtensionID
  );

  if (menu_item_callback != nullptr) {
    AVMenuItemSetExecuteProc(menu_item, menu_item_callback, NULL);
  }

  if (menu_item_is_enabled != nullptr) {
    AVMenuItemSetComputeEnabledProc(menu_item, menu_item_is_enabled, (void*)pdPermEdit);
  }

  AVMenuAddMenuItem(parent_menu, menu_item, APPEND_MENUITEM);

  created_menu_items.push_back(menu_item);
}