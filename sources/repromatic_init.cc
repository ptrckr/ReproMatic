//	
//  repromatic_init.cc
//  Created by ptrckr on 28.02.18.	
//	

#ifndef MAC_PLATFORM
  #include "PIHeaders.h"
#endif

// Menus
// =====

static AVMenuItem overviewMenu = NULL;
static AVMenuItem overviewMenuItem = NULL;

static AVMenuItem sortMenu = NULL;
static AVMenuItem sortMenuItem = NULL;

static AVMenuItem dividerMenu = NULL;
static AVMenuItem dividerMenuItem = NULL;

// Menu Items
// ==========

extern ACCB1 void ACCB2 SortExtended(void *clientData);
extern ACCB1 ASBool ACCB2 SortExtendedIsEnabled(void *clientData);

extern ACCB1 void ACCB2 OverviewExtended(void *clientData);
extern ACCB1 ASBool ACCB2 OverviewExtendedIsEnabled(void *clientData);

extern ACCB1 void ACCB2 Divider(void *clientData);
extern ACCB1 ASBool ACCB2 DividerIsEnabled(void *clientData);

// Plugin Setup
// ============

const char *ReproMaticExtensionName = "PTRK:ReproMaticPlugin";
ACCB1 ASBool ACCB2 ReproInit();

ASAtom GetExtensionName() {
  return ASAtomFromString(ReproMaticExtensionName);
}

ACCB1 ASBool ACCB2 PluginExportHFTs(void) {
  return true;
}

ACCB1 ASBool ACCB2 PluginImportReplaceAndRegister(void) {
  return true;
}

ACCB1 ASBool ACCB2 PluginInit(void) {
  return ReproInit();
}

ACCB1 ASBool ACCB2 PluginUnload(void) {
  if (overviewMenu) AVMenuItemRemove(overviewMenu);
  if (overviewMenuItem) AVMenuItemRemove(overviewMenuItem);

  if (sortMenu) AVMenuItemRemove(sortMenu);
  if (sortMenuItem) AVMenuItemRemove(sortMenuItem);

  if (dividerMenu) AVMenuItemRemove(dividerMenu);
  if (dividerMenuItem) AVMenuItemRemove(dividerMenuItem);

  return true;
}

ACCB1 ASBool ACCB2 PIHandshake(Uns32 handshakeVersion, void *handshakeData) {
  if (handshakeVersion == HANDSHAKE_V0200) {
    PIHandshakeData_V0200 *hsData = (PIHandshakeData_V0200 *)handshakeData;
    hsData->extensionName = GetExtensionName();
    hsData->exportHFTsCallback = (void*)ASCallbackCreateProto(PIExportHFTsProcType, &PluginExportHFTs);
    hsData->importReplaceAndRegisterCallback = (void*)ASCallbackCreateProto(PIImportReplaceAndRegisterProcType, &PluginImportReplaceAndRegister);
    hsData->initCallback = (void*)ASCallbackCreateProto(PIInitProcType, &PluginInit);
    hsData->unloadCallback = (void*)ASCallbackCreateProto(PIUnloadProcType, &PluginUnload);
    return true;
  }

  return false;
}

ACCB1 ASBool ACCB2 ReproInit() {
  AVMenubar menubar = AVAppGetMenubar();
  AVMenu volatile acrobatMainMenu = NULL;
  AVMenu volatile overviewMenuParent = NULL;
  AVMenu volatile sortMenuParent = NULL;
  AVMenu volatile dividerMenuParent = NULL;

  if (!menubar) return false;

  DURING
    acrobatMainMenu = AVMenubarAcquireMenuByName(menubar, "PTRK:ReproMatic");

    if (!acrobatMainMenu) {
      acrobatMainMenu = AVMenuNew("ReproMatic", "PTRK:ReproMatic", gExtensionID);
      AVMenubarAddMenu(menubar, acrobatMainMenu, APPEND_MENU);
    }

    // Overview Menu
    overviewMenuParent = AVMenuNew("overviewMenu", "PTRK:overviewMenuParent", gExtensionID);
    overviewMenu = AVMenuItemNew("Overview", "PTRK:overviewMenu", overviewMenuParent, true, NO_SHORTCUT, 0, NULL, gExtensionID);

    // Overview Extended
    overviewMenuItem = AVMenuItemNew("Overview Extended", "PTRK:Overview", NULL, true, NO_SHORTCUT, 0, NULL, gExtensionID);
    AVMenuItemSetExecuteProc(overviewMenuItem, ASCallbackCreateProto(AVExecuteProc, OverviewExtended), NULL);
    AVMenuItemSetComputeEnabledProc(overviewMenuItem, ASCallbackCreateProto(AVComputeEnabledProc, OverviewExtendedIsEnabled), (void *)pdPermEdit);
    AVMenuAddMenuItem(overviewMenuParent, overviewMenuItem, APPEND_MENUITEM);

    AVMenuAddMenuItem(acrobatMainMenu, overviewMenu, APPEND_MENUITEM);

    // Sort Menu
    sortMenuParent = AVMenuNew("sortMenu", "PTRK:sortMenu", gExtensionID);
    sortMenu = AVMenuItemNew("Sort", "PTRK:sortMenuItem", sortMenuParent, true, NO_SHORTCUT, 0, NULL, gExtensionID);

    // Sort Extended
    sortMenuItem = AVMenuItemNew("Sort Extended", "PTRK:Sort", NULL, true, NO_SHORTCUT, 0, NULL, gExtensionID);
    AVMenuItemSetExecuteProc(sortMenuItem, ASCallbackCreateProto(AVExecuteProc, SortExtended), NULL);
    AVMenuItemSetComputeEnabledProc(sortMenuItem, ASCallbackCreateProto(AVComputeEnabledProc, SortExtendedIsEnabled), (void *)pdPermEdit);
    AVMenuAddMenuItem(sortMenuParent, sortMenuItem, APPEND_MENUITEM);

    AVMenuAddMenuItem(acrobatMainMenu, sortMenu, APPEND_MENUITEM);

    // Divider Menu
    dividerMenuParent = AVMenuNew("dividerMenu", "PTRK:dividerMenu", gExtensionID);
    dividerMenu = AVMenuItemNew("Paper Divider", "PTRK:dividerMenuItem", dividerMenuParent, true, NO_SHORTCUT, 0, NULL, gExtensionID);

    // Divider
    dividerMenuItem = AVMenuItemNew("Create Paper Divider", "PTRK:Divider", NULL, true, NO_SHORTCUT, 0, NULL, gExtensionID);
    AVMenuItemSetExecuteProc(dividerMenuItem, ASCallbackCreateProto(AVExecuteProc, Divider), NULL);
    AVMenuItemSetComputeEnabledProc(dividerMenuItem, ASCallbackCreateProto(AVComputeEnabledProc, DividerIsEnabled), (void *)pdPermEdit);
    AVMenuAddMenuItem(dividerMenuParent, dividerMenuItem, APPEND_MENUITEM);

    AVMenuAddMenuItem(acrobatMainMenu, dividerMenu, APPEND_MENUITEM);

    AVMenuRelease(overviewMenuParent);
    AVMenuRelease(sortMenuParent);
    AVMenuRelease(dividerMenuParent);
    AVMenuRelease(acrobatMainMenu);

  HANDLER
    if (overviewMenuParent) AVMenuRelease(overviewMenuParent);
    if (sortMenuParent) AVMenuRelease(sortMenuParent);
    if (dividerMenuParent) AVMenuRelease(dividerMenuParent);
    if (acrobatMainMenu) AVMenuRelease(acrobatMainMenu);
      
    return false;

  END_HANDLER

  return true;
}
