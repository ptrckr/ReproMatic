//
//  menu_methods/overview.cc
//  Created by Patrick Rügheimer on 25.06.18.
//

#include "overview.h"
#include <sstream>
#include <string>
#include "dictionaries/dictionary.h"
#include "dictionaries/din_dictionary.h"
#include "dictionaries/scan_dictionary.h"
#include "repromatic_utils.h"
#include "acrobat_utils.h"
#ifndef MAC_PLATFORM
  #include "PIHeaders.h"
#endif

template<typename T>
void Overview() {
  T dictionary;
  AVDoc av_active_document = AVAppGetActiveDoc();

  if (av_active_document) {
    PDDoc pd_active_document = AVDocGetPDDoc(av_active_document);
    dictionary.AddPagesFrom(pd_active_document);
    AVAlertNote(dictionary.Stringify().c_str());
  } else {
    // Get root folder
    ASFileSys file_sys = nullptr;
    ASPathName root = nullptr;
    if (!repromatic::acrobat_utils::GetFolderByDialog(file_sys, root)) {
      ASFileSysReleasePath(file_sys, root);
      return;
    }

    // Get files
    std::vector<std::string> files;
    files = repromatic::QueryFolder(file_sys, kASFileSysFile, root, true);

    // Status monitor
    repromatic::acrobat_utils::StatusMonitorUtil status_monitor;
    status_monitor.SetDuration(static_cast<int>(files.size()));

    // Iterate files
    int iteration = 0;
    for (const std::string& file : files) {
      ASPathName as_file_path = ASFileSysPathFromDIPath(file_sys, file.c_str(), 0);
      ASPathName as_file_path_parent = ASFileSysAcquireParent(file_sys, as_file_path);
      char* file_path_cstr = ASFileSysDIPathFromPath(file_sys, as_file_path_parent, root);
      char* file_name_cstr = ASFileSysDIPathFromPath(file_sys, as_file_path, as_file_path_parent);
      ASFileSysReleasePath(file_sys, as_file_path_parent);
      ASFileSysReleasePath(file_sys, as_file_path);
      std::string file_path(file_path_cstr);
      std::string file_name(file_name_cstr);
      ASfree(file_path_cstr);
      ASfree(file_name_cstr);

      status_monitor.SetValue(iteration++);
      status_monitor.SetText(std::string("[[") + file_name + "]] @ " +
                             std::to_string(iteration) + "/" +
                             std::to_string(files.size()));
    }

    // Close status monitor
    status_monitor.EndOperation();
  }
}

// Din
ACCB1 void ACCB2 OverviewDinCallback(void *clientData) {
  Overview<repromatic::DinDictionary>();
}

ACCB1 ASBool ACCB2 OverviewDinCallbackIsEnabled(void *clientData) {
  return true;
}

// Scan
ACCB1 void ACCB2 OverviewScanCallback(void *clientData) {
  Overview<repromatic::ScanDictionary>();
}

ACCB1 ASBool ACCB2 OverviewScanCallbackIsEnabled(void *clientData) {
  return true;
}
