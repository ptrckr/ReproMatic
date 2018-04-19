//	
//  repromatic_init.cc
//  Created by ptrckr on 28.02.18.	
//	

#include <string>
#include <vector>

#ifndef MAC_PLATFORM
  #include "PIHeaders.h"
#endif

#include "acrobat_utils.h"

// Menu Manager
// ============

static repromatic::acrobat_utils::MenuUtil MenuManager;

// Menu Items
// ==========

extern ACCB1 void ACCB2 SortRecursive(void *clientData);  // repromatic_main.cc
extern ACCB1 ASBool ACCB2 SortRecursiveIsEnabled(void *clientData);  // repromatic_main.cc

extern ACCB1 void ACCB2 PdfFormatSummarySimple(void *clientData);  // repromatic_main.cc
extern ACCB1 void ACCB2 PdfFormatSummaryDetailed(void *clientData);  // repromatic_main.cc
extern ACCB1 ASBool ACCB2 PdfFormatSummaryIsEnabled(void *clientData);  // repromatic_main.cc

extern ACCB1 void ACCB2 ExtractA4(void *clientData);  // repromatic_main.cc
extern ACCB1 void ACCB2 ExtractA3(void *clientData);  // repromatic_main.cc
extern ACCB1 void ACCB2 ExtractPlans(void *clientData);  // repromatic_main.cc
extern ACCB1 ASBool ACCB2 ExtractIsEnabled(void *clientData);  // repromatic_main.cc

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
  MenuManager.Init();

  return ReproInit();
}

ACCB1 ASBool ACCB2 PluginUnload(void) {
  MenuManager.RemoveMenuItems();

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
  DURING
    MenuManager.CreateMenu(
      "PDF Format Summary",
      R"(ReproMaticIcons\summary.bmp)", 24, 6
    );

    MenuManager.AddMenuItemToMenu(
      "PDF Format Summary", "Simple",
      ASCallbackCreateProto(AVExecuteProc, PdfFormatSummarySimple),
      ASCallbackCreateProto(AVComputeEnabledProc, PdfFormatSummaryIsEnabled),
      R"(ReproMaticIcons\summary.bmp)", 24, 6
    );

    MenuManager.AddMenuItemToMenu(
      "PDF Format Summary", "Detailed",
      ASCallbackCreateProto(AVExecuteProc, PdfFormatSummaryDetailed),
      ASCallbackCreateProto(AVComputeEnabledProc, PdfFormatSummaryIsEnabled),
      R"(ReproMaticIcons\summary_detailed.bmp)", 24, 10
    );

    MenuManager.AddMenuItemToMenu(
      "", "Create Divider",
      ASCallbackCreateProto(AVExecuteProc, Divider),
      ASCallbackCreateProto(AVComputeEnabledProc, DividerIsEnabled)
    );

    MenuManager.AddMenuItemToMenu(
      "", "-", nullptr, nullptr
    );

    MenuManager.CreateMenu(
      "Sort Folder",
      R"(ReproMaticIcons\sort.bmp)", 20, 16
    );

    MenuManager.AddMenuItemToMenu(
      "Sort Folder", "Recursive",
      ASCallbackCreateProto(AVExecuteProc, SortRecursive),
      ASCallbackCreateProto(AVComputeEnabledProc, SortRecursiveIsEnabled)
    );

    MenuManager.AddMenuItemToMenu(
      "Extract", "DIN A4",
      ASCallbackCreateProto(AVExecuteProc, ExtractA4),
      ASCallbackCreateProto(AVComputeEnabledProc, ExtractIsEnabled)
    );

    MenuManager.AddMenuItemToMenu(
      "Extract", "DIN A3",
      ASCallbackCreateProto(AVExecuteProc, ExtractA3),
      ASCallbackCreateProto(AVComputeEnabledProc, ExtractIsEnabled)
    );

    MenuManager.AddMenuItemToMenu(
      "Extract", "Plans",
      ASCallbackCreateProto(AVExecuteProc, ExtractPlans),
      ASCallbackCreateProto(AVComputeEnabledProc, ExtractIsEnabled)
    );

    MenuManager.ReleaseMenus();
  HANDLER
    MenuManager.ReleaseMenus();

    return false;
  END_HANDLER

  return true;
}
