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

    created_menus["acrobat_main_menu"] = acrobat_main_menu;
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

AVMenu MenuUtil::CreateMenu(std::string menu_name, std::string icon, int icon_w, int icon_h) {
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
    (std::string("PTRK:") + menu_name).c_str(),
    menu, true, NO_SHORTCUT, 0,
    (icon != "" ? (HBITMAP)LoadImage(
      NULL, icon.c_str(),
      IMAGE_BITMAP, icon_w, icon_h, LR_LOADFROMFILE
    ) : NULL),
    gExtensionID
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
    AVComputeEnabledProc menu_item_is_enabled,
    std::string icon,
    int icon_w,
    int icon_h) {
  AVMenu parent_menu = CreateMenu(menu);
  AVMenuItem menu_item = AVMenuItemNew(
    menu_item_name.c_str(),
    (std::string("PTRK:") + menu_item_name).c_str(),
    NULL, true, NO_SHORTCUT, 0, 
    (icon != "" ? (HBITMAP)LoadImage(
      NULL, icon.c_str(),
      IMAGE_BITMAP, icon_w, icon_h, LR_LOADFROMFILE
    ) : NULL),
    gExtensionID
  );

  if (menu_item_callback != nullptr) {
    AVMenuItemSetExecuteProc(menu_item, menu_item_callback, NULL);
  }

  if (menu_item_is_enabled != nullptr) {
    AVMenuItemSetComputeEnabledProc(menu_item, menu_item_is_enabled, (void *)pdPermEdit);
  }

  AVMenuAddMenuItem(parent_menu, menu_item, APPEND_MENUITEM);

  created_menu_items.push_back(menu_item);
}  

StatusMonitorUtil::StatusMonitorUtil() {
  pddoc = PDDocCreate();
  PDPageRelease(
    PDDocCreatePage(pddoc, PDBeforeFirstPage, ASFixedRect{
      fixedZero, fixedZero, fixedTen, fixedTen
    })
  );

  avdoc = AVDocOpenFromPDDoc(pddoc, NULL);

  memset(&monitor, 0, sizeof(monitor));
  monitor.size = sizeof(monitor);
  monitor.cancelProc = AVAppGetCancelProc(&monitor.cancelProcClientData);
  monitor.progMon = AVAppGetDocProgressMonitor(&monitor.progMonClientData);
  monitor.reportProc = AVAppGetReportProc(&monitor.reportProcClientData);

  if (monitor.progMon) {
    if (monitor.progMon->beginOperation) {
      monitor.progMon->beginOperation(monitor.progMonClientData);
    }

    if (monitor.progMon->setCurrValue) {
      monitor.progMon->setCurrValue(0, monitor.progMonClientData);
    }

    if (monitor.progMon->setDuration) {
      monitor.progMon->setDuration(100, monitor.progMonClientData);
    }
  }
}

void StatusMonitorUtil::SetDuration(int duration) {
  if (monitor.progMon->setDuration) {
    monitor.progMon->setDuration(duration, monitor.progMonClientData);
  }
}

void StatusMonitorUtil::SetText(std::string text) {
  if (monitor.progMon && monitor.progMon->setText) {
    monitor.progMon->setText(
      ASTextFromEncoded(text.c_str(), AVAppGetLanguageEncoding()),
      monitor.progMonClientData
    );
  }
}

void StatusMonitorUtil::SetValue(int value) {
  if (monitor.progMon && monitor.progMon->setCurrValue) {
    monitor.progMon->setCurrValue(
      value, monitor.progMonClientData
    );
  }
}

void StatusMonitorUtil::EndOperation() {
  if (monitor.progMon && monitor.progMon->endOperation) {
    monitor.progMon->endOperation(monitor.progMonClientData);
  }

  AVDocClose(avdoc, true);
}

ASBool GetFolderByDialog(ASFileSys &file_system, ASPathName &folder) {
  AVOpenSaveDialogParamsRec params;
  memset(&params, NULL, sizeof(AVOpenSaveDialogParamsRec));
  params.size = sizeof(AVOpenSaveDialogParamsRec);

  return AVAppChooseFolderDialog(&params, &file_system, &folder);
};

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