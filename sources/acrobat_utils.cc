//	
//  acrobat_utils.cc
//  Created by ptrckr on 24.03.18.	
//	

#include "acrobat_utils.h"

#include <vector>
#include <map>
#include <string>

#ifndef MAC_PLATFORM
#include "PIHeaders.h"
#endif

namespace repromatic {
namespace acrobat_utils {

void MenuUtil::Init() {
  if (app_menubar = AVAppGetMenubar()) {
    acrobat_main_menu = AVMenubarAcquireMenuByName(app_menubar, "PTRK:ReproMatic");

    if (!acrobat_main_menu) {
      acrobat_main_menu = AVMenuNew("ReproMatic", "PTRK:ReproMatic", gExtensionID);
      AVMenubarAddMenu(app_menubar, acrobat_main_menu, APPEND_MENU);
    }
  }
}

void MenuUtil::ReleaseMenus() {
  for(auto const &menu : created_menus) {
    AVMenuRelease(menu.second);
  }
}

void MenuUtil::RemoveMenuItems() {
  for (AVMenuItem &menu_item : created_menu_items) {
    AVMenuItemRemove(menu_item);
  }
}

AVMenu MenuUtil::CreateMenu(std::string menu_name) {
  if (menu_name == "") {
    return acrobat_main_menu;
  }

  auto menu_search = created_menus.find(menu_name);
  if (menu_search != created_menus.end()) {
    return menu_search->second;
  }

  AVMenu menu = AVMenuNew(menu_name.c_str(), (std::string("PTRK:") + menu_name).c_str(), gExtensionID);
  AVMenuItem menu_item = AVMenuItemNew(
    menu_name.c_str(),
    (std::string("PTRK:") + menu_name + "Item").c_str(),
    menu, true, NO_SHORTCUT, 0, NULL, gExtensionID
  );
  AVMenuAddMenuItem(acrobat_main_menu, menu_item, APPEND_MENUITEM);
    
  created_menus[menu_name] = menu;
  created_menu_items.push_back(menu_item);

  return menu;
}

void MenuUtil::AddMenuItemToMenu(
    std::string menu,
    std::string menu_item_name,
    AVExecuteProc menu_item_callback,
    AVComputeEnabledProc menu_item_is_enabled) {
  AVMenu parent_menu = CreateMenu(menu);
  AVMenuItem menu_item = AVMenuItemNew(
    menu_item_name.c_str(),
    (std::string("PTRK:") + menu_item_name).c_str(),
    NULL, true, NO_SHORTCUT, 0, NULL, gExtensionID
  );

  AVMenuItemSetExecuteProc(menu_item, menu_item_callback, NULL);
  AVMenuItemSetComputeEnabledProc(menu_item, menu_item_is_enabled, (void *)pdPermEdit);
  AVMenuAddMenuItem(parent_menu, menu_item, APPEND_MENUITEM);

  created_menu_items.push_back(menu_item);
}  

AVStatusMonitorProcsRec GetStatusMonitor() {
  AVStatusMonitorProcsRec status_monitor;
  memset(&status_monitor, 0, sizeof(status_monitor));
  status_monitor.size = sizeof(status_monitor);
  status_monitor.cancelProc = AVAppGetCancelProc(&status_monitor.cancelProcClientData);
  status_monitor.progMon = AVAppGetDocProgressMonitor(&status_monitor.progMonClientData);
  status_monitor.reportProc = AVAppGetReportProc(&status_monitor.reportProcClientData);

  if (status_monitor.progMon) {
    if (status_monitor.progMon->beginOperation) {
      status_monitor.progMon->beginOperation(status_monitor.progMonClientData);
    }

    if (status_monitor.progMon->setCurrValue) {
      status_monitor.progMon->setCurrValue(0, status_monitor.progMonClientData);
    }

    if (status_monitor.progMon->setDuration) {
      status_monitor.progMon->setDuration(100, status_monitor.progMonClientData);
    }
  }

  return status_monitor;
}

}  // acrobat_utils
}  // repromatic