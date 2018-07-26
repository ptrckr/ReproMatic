//
//  menu_methods/overview.cc
//  Created by Patrick Rügheimer on 25.06.18.
//

#include "overview.h"
#include <sstream>
#include <string>
#include <vector>
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
  AVDoc active_document;

  if (active_document = AVAppGetActiveDoc()) {
    dictionary.AddPagesFrom(AVDocGetPDDoc(active_document));

    // Alert stringified dictionary
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
    std::vector<std::string> files = repromatic::QueryFolder(file_sys, kASFileSysFile, root, true);
    std::vector<std::string> irreparable_files;

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
      std::string file_path(file_path_cstr);
      std::string file_name(file_name_cstr);
      ASfree(file_path_cstr);
      ASfree(file_name_cstr);

      status_monitor.SetValue(iteration++);
      status_monitor.SetText(std::to_string(iteration) + " of " + std::to_string(files.size()));

      DURING
        PDDoc pdf_file = PDDocOpen(as_file_path, file_sys, NULL, true);
        dictionary.AddPagesFrom(pdf_file);
        PDDocClose(pdf_file);
      HANDLER
        irreparable_files.push_back(file_name + (file_path.length() == 0 ? "" : " (" + file_path + ")"));
      END_HANDLER

      ASFileSysReleasePath(file_sys, as_file_path);
    }

    // Close status monitor
    status_monitor.EndOperation();

    // Alert files that are damaged and could not be repaired
    if (!irreparable_files.empty()) {
      std::stringstream error_msg;

      error_msg << "Following files are damaged and could not be repaired." << std::endl;
      error_msg << "They have NOT been processed and will not be included in the overview." << std::endl << std::endl;

      for (auto i = irreparable_files.begin(); i != irreparable_files.end(); ++i) {
        error_msg << "• " <<  *i;

        if (i != --irreparable_files.end()) {
          error_msg << std::endl;
        }
      }

      AVAlertNote(error_msg.str().c_str());
    }

    // Alert stringified dictionary
    AVAlertNote(dictionary.Stringify(false, true).c_str());
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
