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

void Sort(bool detailed = true) {
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
  int current_pdf_file_iteration = 0;

  repromatic::PageDictionary all_pages;

  repromatic::IterateFolder(file_system, folder, folder,
                [&file_pdf_count, &folder_count, &status_monitor, &current_pdf_file_iteration, total_file_count, &all_pages](
                  ASFileSys file_system,
                  ASPathName root,
                  ASPathName as_item_path,
                  std::string item_type) -> void {
    if (item_type == "file_pdf") {
      current_pdf_file_iteration += 1;
      file_pdf_count += 1;

      if (status_monitor.progMon) {
        if (status_monitor.progMon->setText) {
          char *filename = ASFileSysDIPathFromPath(
            file_system, as_item_path, ASFileSysAcquireParent(file_system, as_item_path)
          );

          std::string status_message(filename != nullptr ? filename : "Unknown filename");
          status_message += " - " + std::to_string(current_pdf_file_iteration) + "/" + std::to_string(total_file_count);

          ASfree(filename);

          status_monitor.progMon->setText(
            ASTextFromEncoded(status_message.c_str(), AVAppGetLanguageEncoding()),
            status_monitor.progMonClientData
          );
        }

        if (status_monitor.progMon->setCurrValue) {
          status_monitor.progMon->setCurrValue(
            100 / total_file_count * current_pdf_file_iteration,
            status_monitor.progMonClientData
          );
        }
      }

      repromatic::PageDictionary file_pages;
      PDDoc my_doc = PDDocOpen(as_item_path, file_system, NULL, false);
      ASPathName pdfRoot = ASFileSysAcquireParent(file_system, as_item_path);

      file_pages.AddPagesFrom(my_doc);
      all_pages.AddPagesFrom(my_doc);

      ASText pdf_file_name = ASTextNew();
      if (ASFileSysGetNameFromPathAsASText(file_system, as_item_path, pdf_file_name) != 0) {
        AVAlertNote("Could not get filename.");
        return;
      }

      ASPathName sorted_path = ASFileSysCreatePathWithFolderName(file_system, pdfRoot, "Sorted");
      ASPathName sorted_din_format_path = ASFileSysCreatePathWithFolderName(file_system, sorted_path, file_pages.GetDinKey().c_str());
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
  alert << "Sorted `" << ASFileSysDIPathFromPath(NULL, folder, NULL) << "'." << std::endl;
  alert << "• " << file_pdf_count << " PDF file" << (file_pdf_count > 1 ? "s" : "");
  alert << " in " << folder_count << " sub folder" << (folder_count > 1 ? "s" : "") << "." << std::endl << std::endl;
  alert << all_pages.ToString(false);

  AVAlertNote(alert.str().c_str());
}

// Sort Callbacks
ACCB1 void ACCB2 SortRecursive(void *clientData) { Sort(true); };
ACCB1 ASBool ACCB2 SortRecursiveIsEnabled(void *clientData) { return true; }


// Summary
// =======

void Summary(bool detailed = true) {
  repromatic::PageDictionary page_dict;
  AVDoc avActiveDocument = AVAppGetActiveDoc();

  if (avActiveDocument) {
    PDDoc pdActiveDocument = AVDocGetPDDoc(avActiveDocument);
    page_dict.AddPagesFrom(pdActiveDocument);
    AVAlertNote(page_dict.ToString(detailed).c_str());
  } else {
    AVOpenSaveDialogParamsRec params;
    memset(&params, NULL, sizeof(AVOpenSaveDialogParamsRec));
    params.size = sizeof(AVOpenSaveDialogParamsRec);
    ASFileSys file_system;
    ASPathName folder;
    if (!AVAppChooseFolderDialog(&params, &file_system, &folder)) return;

    // Open a file to be able to get a status monitor.
    AVDoc first_avdoc = NULL;
    ASPathName first_file = repromatic::GetFirstPdfFileInFolder(file_system, folder);
    if (first_file != nullptr) {
      first_avdoc = AVDocOpenFromFile(first_file, file_system, NULL);
    }
    ASFileSysReleasePath(file_system, first_file);
    AVStatusMonitorProcsRec status_monitor = repromatic::acrobat_utils::GetStatusMonitor();

    int file_pdf_count = 0;
    int file_non_pdf_count = 0;
    int folder_count = 0;

    int total_file_count = repromatic::GetItemCount(folder, file_system, "file_pdf");
    int current_pdf_file_iteration = 0;

    repromatic::IterateFolder(file_system, folder, folder,
      [&page_dict, &file_pdf_count, &file_non_pdf_count, &folder_count, &status_monitor, &current_pdf_file_iteration, total_file_count](
                    ASFileSys file_system,
                    ASPathName root,
                    ASPathName as_item_path,
                    std::string item_type) -> void {
      if (item_type == "file_pdf") {
        current_pdf_file_iteration += 1;
        file_pdf_count += 1;

        if (status_monitor.progMon) {
          if (status_monitor.progMon->setText) {
            char *filename = ASFileSysDIPathFromPath(
              file_system, as_item_path, ASFileSysAcquireParent(file_system, as_item_path)
            );

            std::string status_message(filename != nullptr ? filename : "Unknown filename");
            status_message += " - " + std::to_string(current_pdf_file_iteration) + "/" + std::to_string(total_file_count);

            ASfree(filename);

            status_monitor.progMon->setText(
              ASTextFromEncoded(status_message.c_str(), AVAppGetLanguageEncoding()),
              status_monitor.progMonClientData
            );
          }

          if (status_monitor.progMon->setCurrValue) {
            status_monitor.progMon->setCurrValue(
              100 / total_file_count * current_pdf_file_iteration,
              status_monitor.progMonClientData
            );
          }
        }

        PDDoc my_doc = PDDocOpen(as_item_path, file_system, NULL, false);
        page_dict.AddPagesFrom(my_doc);
        PDDocClose(my_doc);
      } else if (item_type == "file_non_pdf") {
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
    alert << "• " << file_pdf_count << " PDF file" << (file_pdf_count > 1 ? "s" : "");
    alert << " in " << folder_count << " sub folder" << (folder_count > 1 ? "s" : "") << "." << std::endl << std::endl;
    alert << page_dict.ToString(detailed, false);

    AVAlertNote(alert.str().c_str());
  }
}

// Summary Callbacks
ACCB1 void ACCB2 PdfFormatSummarySimple(void *clientData) { Summary(false); }
ACCB1 void ACCB2 PdfFormatSummaryDetailed(void *clientData) { Summary(true); }
ACCB1 ASBool ACCB2 PdfFormatSummaryIsEnabled(void *clientData) { return true; }


// Extract
// =======

void Extract(std::string extract_format) {
  ASFileSys file_system = nullptr;
  ASPathName folder = nullptr;
  if (!repromatic::acrobat_utils::GetFolderByDialog(file_system, folder)) {
    return;
  }

  int pdf_file_count = 0;
  int folder_count = 0;
  int extracted_pages_count = 0;
  int current_pdf_file_iteration = 0;
  int total_file_count = repromatic::GetItemCount(
    folder, file_system, "file_pdf"
  );

  bool folder_has_locked_docs = false;

  repromatic::acrobat_utils::StatusMonitorUtil status_monitor;
  status_monitor.SetDuration(total_file_count);

  repromatic::IterateFolder(file_system, folder, folder,
    [&status_monitor, extract_format,
     &pdf_file_count, &folder_count, &extracted_pages_count,
     &current_pdf_file_iteration, &folder_has_locked_docs,
     total_file_count](
       ASFileSys file_system,
       ASPathName root,
       ASPathName as_item_path,
       std::string item_type) -> void {
    if (item_type == "file_pdf") {
      current_pdf_file_iteration += 1;
      pdf_file_count += 1;

      status_monitor.SetValue(current_pdf_file_iteration);

      char *filename = ASFileSysDIPathFromPath(
        file_system, as_item_path, ASFileSysAcquireParent(file_system, as_item_path)
      );
      status_monitor.SetText(
        "[[" + std::string(filename != nullptr ? filename : "Unknown filename") + "]] " +
        std::to_string(current_pdf_file_iteration) + "/" + std::to_string(total_file_count)
      );
      ASfree(filename);

      ASPathName folder_root = ASFileSysAcquireParent(file_system, as_item_path);
      ASPathName extracted_path = ASFileSysCreatePathWithFolderName(file_system, folder_root, "Extracted");
      ASPathName extracted_format_path = ASFileSysCreatePathWithFolderName(file_system, extracted_path, extract_format.c_str());
      if (ASFileSysCreateFolder(file_system, extracted_format_path, true) != 0) {
        AVAlertNote("Could not create `Extracted' folder.");
        return;
      }

      ASText pdf_file_name = ASTextNew();
      if (ASFileSysGetNameFromPathAsASText(file_system, as_item_path, pdf_file_name) != 0) {
        AVAlertNote("Could not get filename.");
        return;
      }

      ASPathName complete_path = ASFileSysCreatePathWithFolderNameWithASText(
        file_system, extracted_format_path, pdf_file_name
      );

      PDDoc doc = PDDocOpen(as_item_path, file_system, NULL, false);
      repromatic::PageDictionary page_dict;
      page_dict.AddPagesFrom(doc);

      bool can_delete_pages = false;
      DURING
        if (PDDocPermRequest(doc, PDPermReqObjPage, PDPermReqOprDelete, nullptr) == 0) {
          can_delete_pages = true;
        } else {
          can_delete_pages = false;
        }
      HANDLER
      END_HANDLER

      if (can_delete_pages) {
        auto pages = page_dict.GetPages();
        for (auto page = pages.rbegin(); page != pages.rend(); ++page) {
          if (page->GetDinKey() != extract_format) {
            PDDocDeletePages(
              doc,
              page->number, page->number,
              NULL, NULL
            );
          }
        }

        if (PDDocGetNumPages(doc) > 0) {
          extracted_pages_count += PDDocGetNumPages(doc);

          PDDocSave(doc, PDSaveFull | PDSaveCopy | PDSaveCollectGarbage, complete_path, file_system, NULL, NULL);
        }
      } else {        
        if (page_dict.ContainsDinKey(extract_format)) {
          folder_has_locked_docs = true;
          PDDocSave(doc, PDSaveFull | PDSaveCopy | PDSaveCollectGarbage, complete_path, file_system, NULL, NULL);
        }
      }
      
      ASFileSysReleasePath(file_system, folder_root);
      ASFileSysReleasePath(file_system, extracted_path);
      ASFileSysReleasePath(file_system, extracted_format_path);
      ASFileSysReleasePath(file_system, complete_path);
      PDDocClose(doc);
    } else if (item_type == "folder") {
      folder_count += 1;
    }
  });

  status_monitor.EndOperation();

  std::stringstream alert;

  char *root_folder_path = ASFileSysDIPathFromPath(NULL, folder, NULL);
  ASFileSysReleasePath(file_system, folder);
  alert << "[[" << root_folder_path << "]]";
  ASfree(root_folder_path);

  alert << std::endl << std::endl;
  alert << "• Extracted " << extracted_pages_count << " `" << extract_format << "' page";
  alert << (extracted_pages_count != 1 ? "s" : "") << " from " << pdf_file_count << " PDF file";
  alert << (pdf_file_count != 1 ? "s" : "") << "." << std::endl;

  if (folder_has_locked_docs) {
    alert << "• Could NOT extract pages from some PDF files because of ";
    alert << "restricted permissions, the full document was extracted instead.";
    alert << std::endl;
  }

  AVAlert(
    !folder_has_locked_docs ? ALERT_NOTE : ALERT_CAUTION,
    alert.str().c_str(), "OK", NULL, NULL, false
  );
}

// Summary Callbacks
ACCB1 void ACCB2 ExtractA4(void *clientData) { Extract("A4"); }
ACCB1 void ACCB2 ExtractA3(void *clientData) { Extract("A3"); }
ACCB1 void ACCB2 ExtractPlans(void *clientData) { Extract("Plan"); }
ACCB1 ASBool ACCB2 ExtractIsEnabled(void *clientData) { return true; }


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
