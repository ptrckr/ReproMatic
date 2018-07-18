//
//  main.cc
//  Created by Patrick Rügheimer on 28.02.18.
//

#include "acrobat_utils.h"
#include "menu_methods/overview.h"
#include "resources.h"
#ifndef MAC_PLATFORM
  #include "PIHeaders.h"
#endif

// Menu Manager
static repromatic::acrobat_utils::MenuUtil MenuManager;

ACCB1 ASBool ACCB2 ReproInit() {
  DURING
    MenuManager.CreateMenu("Format Overview", IDB_SUMMARY);

    MenuManager.AddMenuItemToMenu(
      "Format Overview", "DIN",
      ASCallbackCreateProto(AVExecuteProc, OverviewDinCallback),
      ASCallbackCreateProto(AVComputeEnabledProc, OverviewDinCallbackIsEnabled),
      IDB_SUMMARY_SIMPLE
    );

    MenuManager.AddMenuItemToMenu(
      "Format Overview", "Scan",
      ASCallbackCreateProto(AVExecuteProc, OverviewScanCallback),
      ASCallbackCreateProto(AVComputeEnabledProc, OverviewScanCallbackIsEnabled),
      IDB_SUMMARY_SIMPLE
    );

  /*
    // Summary
    // =======
    MenuManager.CreateMenu("PDF Format Summary", IDB_SUMMARY);

    MenuManager.AddMenuItemToMenu(
      "PDF Format Summary", "Simple",
      ASCallbackCreateProto(AVExecuteProc, PdfFormatSummarySimple),
      ASCallbackCreateProto(AVComputeEnabledProc, PdfFormatSummaryIsEnabled),
      IDB_SUMMARY_SIMPLE
    );

    MenuManager.AddMenuItemToMenu(
      "PDF Format Summary", "Detailed",
      ASCallbackCreateProto(AVExecuteProc, PdfFormatSummaryDetailed),
      ASCallbackCreateProto(AVComputeEnabledProc, PdfFormatSummaryIsEnabled),
      IDB_SUMMARY
    );

    MenuManager.AddMenuItemToMenu(
      "PDF Format Summary", "Plan Scans",
      ASCallbackCreateProto(AVExecuteProc, PdfFormatSummaryPlanScans),
      ASCallbackCreateProto(AVComputeEnabledProc, PdfFormatSummaryIsEnabled),
      IDB_SUMMARY
    );

    MenuManager.AddMenuItemToMenu(
      "", "Create Divider",
      ASCallbackCreateProto(AVExecuteProc, Divider),
      ASCallbackCreateProto(AVComputeEnabledProc, DividerIsEnabled),
      IDB_DIVIDER
    );

    MenuManager.AddMenuItemToMenu("", "-", nullptr, nullptr);

    // Sort
    // ====
    MenuManager.CreateMenu("Sort Folder", IDB_SORT);

    MenuManager.AddMenuItemToMenu(
      "Sort Folder", "Recursive",
      ASCallbackCreateProto(AVExecuteProc, SortRecursive),
      ASCallbackCreateProto(AVComputeEnabledProc, SortRecursiveIsEnabled),
      IDB_SORT_RECURSIVE
    );

    // Extract
    // =======
    MenuManager.CreateMenu("Extract", IDB_EXTRACT);

    MenuManager.AddMenuItemToMenu(
      "Extract", "DIN A4",
      ASCallbackCreateProto(AVExecuteProc, ExtractA4),
      ASCallbackCreateProto(AVComputeEnabledProc, ExtractIsEnabled),
      IDB_EXTRACT
    );

    MenuManager.AddMenuItemToMenu(
      "Extract", "DIN A3",
      ASCallbackCreateProto(AVExecuteProc, ExtractA3),
      ASCallbackCreateProto(AVComputeEnabledProc, ExtractIsEnabled),
      IDB_EXTRACT
    );

    MenuManager.AddMenuItemToMenu(
      "Extract", "Plans",
      ASCallbackCreateProto(AVExecuteProc, ExtractPlans),
      ASCallbackCreateProto(AVComputeEnabledProc, ExtractIsEnabled),
      IDB_EXTRACT
    );
*/
    MenuManager.ReleaseMenus();

  HANDLER
    MenuManager.ReleaseMenus();

    return false;
  END_HANDLER

  return true;
}

ASAtom GetExtensionName() {
  return ASAtomFromString("PTRK:Repromatic2Plugin");
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
    hsData->exportHFTsCallback = (void*)ASCallbackCreateProto(PIExportHFTsProcType,&PluginExportHFTs);
    hsData->importReplaceAndRegisterCallback = (void*)ASCallbackCreateProto(PIImportReplaceAndRegisterProcType,&PluginImportReplaceAndRegister);
    hsData->initCallback = (void*)ASCallbackCreateProto(PIInitProcType, &PluginInit);
    hsData->unloadCallback = (void*)ASCallbackCreateProto(PIUnloadProcType, &PluginUnload);
    return true;
  }

  return false;
}
