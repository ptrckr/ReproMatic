//	
//  repromatic_main.cc
//  Created by ptrckr on 28.02.18.	
//	

#include <string>
#include <sstream>
#include <cstdio>

#ifndef MAC_PLATFORM
  #include "PIHeaders.h"
#endif

#include "repromatic.h"
#include "acrobat_utils.h"
#include "page_dictionary.h"

// Sort
// ====

void Sort(bool extended = false) {
  AVOpenSaveDialogParamsRec params;
  memset(&params, NULL, sizeof(AVOpenSaveDialogParamsRec));
  params.size = sizeof(AVOpenSaveDialogParamsRec);

  ASFileSys file_system;
  ASPathName folder;

  if (!AVAppChooseFolderDialog(&params, &file_system, &folder)) return;

  // Open a file to be able to get a status monitor.
  ASPathName first_file = repromatic::GetFirstPdfFileInFolder(file_system, folder);
  AVDoc first_avdoc = NULL;
  if (first_file != NULL) {
    first_avdoc = AVDocOpenFromFile(first_file, file_system, NULL);
  }

  ASFileSysReleasePath(file_system, first_file);

  AVStatusMonitorProcsRec status_monitor = repromatic::acrobat_utils::GetStatusMonitor();

  int file_pdf_count = 0;
  int folder_count = 0;

  int total_file_count = repromatic::GetItemCount(folder, file_system, "file_pdf");
  int current_file_iteration = 0;

  repromatic::IterateFolder(file_system, folder, folder,
                [&file_pdf_count, &folder_count, &status_monitor, &current_file_iteration, total_file_count](
                  ASFileSys file_system,
                  ASPathName root,
                  ASPathName as_item_path,
                  std::string item_type) -> void {
    if (item_type == "file_pdf") {
      if (status_monitor.progMon && status_monitor.progMon->setText) {
        ASText pdf_file_name = ASTextNew();
        if (ASFileSysGetNameFromPathAsASText(file_system, as_item_path, pdf_file_name) == 0) {
          char *pdf_file_name_cstr = ASTextGetEncodedCopy(pdf_file_name, AVAppGetLanguageEncoding());

          char status_monitor_message[48];
          std::sprintf(status_monitor_message, "%3d%% @ %-40.40s", (100 / total_file_count * current_file_iteration), pdf_file_name_cstr);

          ASfree(pdf_file_name_cstr);

          status_monitor.progMon->setText(ASTextFromEncoded(status_monitor_message, AVAppGetLanguageEncoding()), status_monitor.progMonClientData);
        }
      }

      if (status_monitor.progMon && status_monitor.progMon->setCurrValue) {
        status_monitor.progMon->setCurrValue(100 / total_file_count * current_file_iteration, status_monitor.progMonClientData);
      }

      current_file_iteration += 1;

      file_pdf_count += 1;

      repromatic::PageDictionary page_dict;
      PDDoc my_doc = PDDocOpen(as_item_path, file_system, NULL, false);
      ASPathName pdfRoot = ASFileSysAcquireParent(file_system, as_item_path);

      page_dict.AddPagesFrom(my_doc);

      ASText pdf_file_name = ASTextNew();
      if (ASFileSysGetNameFromPathAsASText(file_system, as_item_path, pdf_file_name) != 0) {
        AVAlertNote("Could not get filename.");
        return;
      }

      ASPathName sorted_path = ASFileSysCreatePathWithFolderName(file_system, pdfRoot, "Sorted");
      ASPathName sorted_din_format_path = ASFileSysCreatePathWithFolderName(file_system, sorted_path, page_dict.GetDinKey().c_str());
      ASPathName complete_path = ASFileSysCreatePathWithFolderNameWithASText(file_system, sorted_din_format_path, pdf_file_name);

      if (ASFileSysCreateFolder(file_system, sorted_din_format_path, true) != 0) {
        AVAlertNote("Could not create folder.");
        return;
      }

      PDDocSave(my_doc, PDSaveFull | PDSaveCopy | PDSaveCollectGarbage, complete_path, file_system, NULL, NULL);
      PDDocClose(my_doc);
    } else if (item_type == "folder") {
      folder_count += 1;
    }
  });

  AVDocClose(first_avdoc, true);

  std::stringstream alert;
  alert << "Sorted `";
  alert << ASFileSysDIPathFromPath(NULL, folder, NULL) << "'." << std::endl << std::endl;
  alert << "• " << file_pdf_count << " pdf file(s)." << std::endl;;
  alert << "• " << folder_count << " sub folder(s).";

  AVAlert(ALERT_NOTE, alert.str().c_str(), "OK", NULL, NULL, true);
}

// Sort Callbacks
ACCB1 ASBool ACCB2 SortExtendedIsEnabled(void *clientData) { return true; }
ACCB1 void ACCB2 SortExtended(void *clientData) { Sort(true); };


// Overview
// ========

void Overview(bool extended = false) {
  repromatic::PageDictionary page_dict;
  AVDoc avActiveDocument = AVAppGetActiveDoc();

  if (avActiveDocument) {
    PDDoc pdActiveDocument = AVDocGetPDDoc(avActiveDocument);
    page_dict.AddPagesFrom(pdActiveDocument);
    AVAlert(ALERT_NOTE, page_dict.dinWrapper.ToString(true).c_str(), "OK", NULL, NULL, true);
  } else {
    AVOpenSaveDialogParamsRec params;
    memset(&params, NULL, sizeof(AVOpenSaveDialogParamsRec));
    params.size = sizeof(AVOpenSaveDialogParamsRec);

    ASFileSys file_system;
    ASPathName folder;
    if (!AVAppChooseFolderDialog(&params, &file_system, &folder)) return;

    // Open a file to be able to get a status monitor.
    ASPathName first_file = repromatic::GetFirstPdfFileInFolder(file_system, folder);

    AVDoc first_avdoc = NULL;
    if (first_file != NULL) {
      first_avdoc = AVDocOpenFromFile(first_file, file_system, NULL);
    }

    ASFileSysReleasePath(file_system, first_file);

    AVStatusMonitorProcsRec status_monitor = repromatic::acrobat_utils::GetStatusMonitor();

    int file_pdf_count = 0;
    int file_non_pdf_count = 0;
    int folder_count = 0;

    int total_file_count = repromatic::GetItemCount(folder, file_system);
    int current_file_iteration = 0;

    repromatic::IterateFolder(file_system, folder, folder,
      [&page_dict, &file_pdf_count, &file_non_pdf_count, &folder_count, &status_monitor, &current_file_iteration, total_file_count](
                    ASFileSys file_system,
                    ASPathName root,
                    ASPathName as_item_path,
                    std::string item_type) -> void {
      if (item_type == "file_pdf") {
        if (status_monitor.progMon && status_monitor.progMon->setText) {
          ASText pdf_file_name = ASTextNew();
          if (ASFileSysGetNameFromPathAsASText(file_system, as_item_path, pdf_file_name) == 0) {
            char *pdf_file_name_cstr = ASTextGetEncodedCopy(pdf_file_name, AVAppGetLanguageEncoding());

            char status_monitor_message[48];
            std::sprintf(status_monitor_message, "%3d%% @ %-40.40s", (100 / total_file_count * current_file_iteration), pdf_file_name_cstr);

            ASfree(pdf_file_name_cstr);

            status_monitor.progMon->setText(ASTextFromEncoded(status_monitor_message, AVAppGetLanguageEncoding()), status_monitor.progMonClientData);
          }
        }

        if (status_monitor.progMon && status_monitor.progMon->setCurrValue) {
          status_monitor.progMon->setCurrValue(100 / total_file_count * current_file_iteration, status_monitor.progMonClientData);
        }

        current_file_iteration += 1;

        file_pdf_count += 1;

        PDDoc my_doc = PDDocOpen(as_item_path, file_system, NULL, false);
        page_dict.AddPagesFrom(my_doc);
        PDDocClose(my_doc);
      } else if (item_type == "file_non_pdf") {
        if (status_monitor.progMon && status_monitor.progMon->setCurrValue) {
          status_monitor.progMon->setCurrValue(100 / total_file_count * current_file_iteration, status_monitor.progMonClientData);
        }

        current_file_iteration += 1;

        file_non_pdf_count += 1;
      } else if (item_type == "folder") {
        folder_count += 1;
      }
    });

    if (status_monitor.progMon && status_monitor.progMon->endOperation) {
      status_monitor.progMon->endOperation(status_monitor.progMonClientData);
    }

    AVDocClose(first_avdoc, true);

    std::stringstream alert;
    alert << "Searched `" << ASFileSysDIPathFromPath(NULL, folder, NULL) << "'." << std::endl;
    alert << "• " << file_pdf_count << " pdf file(s)." << std::endl;
    alert << "• " << folder_count << " folder(s)." << std::endl << std::endl;
    alert << page_dict.dinWrapper.ToString(false);

    AVAlert(ALERT_NOTE, alert.str().c_str(), "OK", NULL, NULL, true);
  }
}

// Overview Callbacks
ACCB1 ASBool ACCB2 OverviewExtendedIsEnabled(void *clientData) { return true; }
ACCB1 void ACCB2 OverviewExtended(void *clientData) { Overview(true); }


// Divider
// =======

void CreateDivider() {
  AVOpenSaveDialogParamsRec params;
  memset(&params, NULL, sizeof(AVOpenSaveDialogParamsRec));
  params.size = sizeof(AVOpenSaveDialogParamsRec);

  ASFileSys file_system;

  ASPathName folder;
  if (!AVAppChooseFolderDialog(&params, &file_system, &folder)) return;

  PDDoc divider_doc = PDDocCreate();
  ASFixedRect mediabox = {
    fixedZero,
    ASInt32ToFixed(repromatic::ConvertPrintUnits::MmToPoint(240)),
    ASInt32ToFixed(repromatic::ConvertPrintUnits::MmToPoint(105)),
    fixedZero
  };

  std::stringstream f;

  repromatic::IterateFolder(file_system, folder, folder,
                [&f, &divider_doc, mediabox](ASFileSys file_system, ASPathName root, ASPathName as_item_path, std::string item_type) -> void {
    if (item_type == "folder") {
      PDPage divider_page = PDDocCreatePage(divider_doc, PDBeforeFirstPage, mediabox);

      ASPathName root = ASFileSysAcquireParent(file_system, as_item_path);
      ASText filename = ASTextNew();

      if (ASFileSysGetNameFromPathAsASText(file_system, as_item_path, filename) != 0) {
        AVAlertNote("Could not get filename.");
        return;
      }

      f << ASFileSysDIPathFromPath(NULL, root, NULL) << " <=> " << ASTextGetEncoded(filename, (ASHostEncoding)PDGetHostEncoding()) << std::endl;

      PDPageRelease(divider_page);
    }
  });

  AVDoc avDoc = AVDocOpenFromPDDoc(divider_doc, NULL);

  //AVAlert(ALERT_NOTE, f.str().c_str(), "OK", NULL, NULL, true);
}

// Divider Callbacks
ACCB1 ASBool ACCB2 DividerIsEnabled(void *clientData) { return true; }
ACCB1 void ACCB2 Divider(void *clientData) { CreateDivider(); }
