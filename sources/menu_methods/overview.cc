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
    AVAlertNote(dictionary.Stringify().c_str());
    return;
  }

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

  // Set up status monitor
  repromatic::acrobat_utils::StatusMonitorUtil status_monitor;
  status_monitor.SetDuration(static_cast<int>(files.size()));

  // Iterate files
  for (auto file = files.begin(); file != files.end(); ++file) {
    ASPathName as_file_path = ASFileSysPathFromDIPath(file_sys, (*file).c_str(), 0);
    std::string file_path = repromatic::GetParentPathFromDIPath(*file, file_sys, root);
    std::string filename = repromatic::GetFilenameFromDIPath(*file, file_sys);

    status_monitor.SetValue(file - files.begin());

    DURING
      PDDoc pdf_file = PDDocOpen(as_file_path, file_sys, NULL, true);
      dictionary.AddPagesFrom(pdf_file);
      PDDocClose(pdf_file);
    HANDLER
      irreparable_files.push_back(
        filename + (file_path.length() == 0 ? "" : " (" + file_path + ")")
      );
    END_HANDLER

    ASFileSysReleasePath(file_sys, as_file_path);
  }

  // Clean up
  ASFileSysReleasePath(file_sys, root);
  status_monitor.EndOperation();

  // Alert files that are damaged and could not be repaired
  if (!irreparable_files.empty()) {
    std::stringstream error_msg;

    error_msg << "The following files are damaged and could not be repaired." << std::endl;
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
