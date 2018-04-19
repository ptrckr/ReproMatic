//	
//  repromatic_main.cc
//  Created by ptrckr on 28.02.18.	
//	

#include <string>
#include <sstream>
#include <cstdio>
#include <vector>

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
    ASFileSysReleasePath(file_system, folder);
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

      bool doc_has_extract_format = false;
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
          doc_has_extract_format = true;
        }
      } else {        
        if (page_dict.ContainsDinKey(extract_format)) {
          folder_has_locked_docs = true;
          doc_has_extract_format = true;
        }
      }

      if (doc_has_extract_format) {
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

        PDDocSave(doc, PDSaveFull | PDSaveCopy | PDSaveCollectGarbage, complete_path, file_system, NULL, NULL);
      
        ASFileSysReleasePath(file_system, folder_root);
        ASFileSysReleasePath(file_system, extracted_path);
        ASFileSysReleasePath(file_system, extracted_format_path);
        ASFileSysReleasePath(file_system, complete_path);
      }
      
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

// Extract Callbacks
ACCB1 void ACCB2 ExtractA4(void *clientData) { Extract("A4"); }
ACCB1 void ACCB2 ExtractA3(void *clientData) { Extract("A3"); }
ACCB1 void ACCB2 ExtractPlans(void *clientData) { Extract("Plan"); }
ACCB1 ASBool ACCB2 ExtractIsEnabled(void *clientData) { return true; }


// Divider
// =======

ACCB1 ASBool ACCB2 find_font(PDSysFont sys_font, void *font_attrs) {
  std::vector<ASAtom> fonts;
  fonts.push_back(ASAtomFromString("Helvetica"));
  fonts.push_back(ASAtomFromString("Arial"));
  fonts.push_back(ASAtomFromString("Courier"));

  for (const auto &font : fonts) {
    if (font == PDSysFontGetName(sys_font)) {
      ((PDEFontAttrsP)font_attrs)->name = font;
      return false;
    }
  }

  return true;
};

void CreateDivider() {
  // Get root folder
  ASFileSys file_sys = nullptr;
  ASPathName root = nullptr;
  if (!repromatic::acrobat_utils::GetFolderByDialog(file_sys, root)) {
    ASFileSysReleasePath(file_sys, root);
    return;
  }

  // Get sub folders
  std::vector<std::string> folders;
  folders = repromatic::QueryFolder(file_sys, kASFileSysFolder, root, true);

  repromatic::acrobat_utils::StatusMonitorUtil status_monitor;
  status_monitor.SetDuration(int(folders.size()));

  // Create doc
  float page_width = 105.0f;
  float page_height = 240.0f;
  PDDoc pddoc = PDDocCreate();

  int iteration = 0;

  // Fill doc with pages
  for (const std::string& folder : folders) {
    ASPathName as_folder_path = ASFileSysPathFromDIPath(file_sys, folder.c_str(), 0);
    ASPathName as_folder_path_parent = ASFileSysAcquireParent(file_sys, as_folder_path);
    char* folder_path_cstr = ASFileSysDIPathFromPath(file_sys, as_folder_path_parent, root);
    char* folder_name_cstr = ASFileSysDIPathFromPath(file_sys, as_folder_path, as_folder_path_parent);
    ASFileSysReleasePath(file_sys, as_folder_path_parent);
    ASFileSysReleasePath(file_sys, as_folder_path);
    std::string folder_path(folder_path_cstr);
    std::string folder_name(folder_name_cstr);
    ASfree(folder_path_cstr);
    ASfree(folder_name_cstr);

    status_monitor.SetValue(iteration++);
    status_monitor.SetText(std::string("[[") + folder_name + "]] @ " +
                           std::to_string(iteration) + "/" +
                           std::to_string(folders.size()));

    PDPage page = PDDocCreatePage(pddoc, PDPageNumber(PDDocGetNumPages(pddoc) - 1), ASFixedRect{
      fixedZero, fixedZero,
      ASInt32ToFixed(repromatic::ConvertPrintUnits::MmToPoint(page_width)),
      ASInt32ToFixed(repromatic::ConvertPrintUnits::MmToPoint(page_height))
    });

    PDEText text = PDETextCreate(); 

    // Font
    PDEFontAttrs font_attrs = {0};
    PDSysFont sys_font;
    PDEFont font;
    PDEnumSysFonts(find_font, &font_attrs);

    if (font_attrs.name != NULL) {
      sys_font = PDFindSysFont(&font_attrs, sizeof(font_attrs), 0);
      font = PDEFontCreateFromSysFont(sys_font, kPDEFontCreateEmbedded);
    } else {
      AVAlertNote("No font could be found.");
      return;
    }

    // Graphic State
    PDEGraphicState graphic_state = {0};
    PDEDefaultGState(&graphic_state, sizeof(graphic_state)); 

    PDEColorSpec fill_clr_spec = {0};
    fill_clr_spec.space = PDEColorSpaceCreateFromName(
      ASAtomFromString("DeviceCMYK")
    ); 

    PDEColorValue fill_clr_value = {0};
    fill_clr_value.color[3] = ASFloatToFixed(1.0f);
    fill_clr_spec.value = fill_clr_value;

    graphic_state.fillColorSpec = fill_clr_spec;
    graphic_state.wasSetFlags = kPDEFillCSpaceWasSet | kPDEFillCValueWasSet;

    // Text State
    PDETextState text_state = {0};
    text_state.charSpacing = ASFloatToFixed(0.0);
    text_state.wordSpacing = ASFloatToFixed(0.0);
    text_state.renderMode = (ASInt32)0;  // Fill text.
    text_state.fontSize = ASInt32ToFixed(1);
    text_state.hScale = ASFloatToFixed(100.0);
    text_state.textRise= ASFloatToFixed(0.0);

    // Text Matrix  
    ASFixedMatrix text_matrix = {0};
    text_matrix.a = ASInt32ToFixed(11);
    text_matrix.d = ASInt32ToFixed(11);
    text_matrix.h = FloatToASFixed(0.0);
    text_matrix.v = FloatToASFixed(repromatic::ConvertPrintUnits::MmToPoint(10.0));

    PDETextAdd( 
      text, kPDETextRun, 0, 
      (ASUns8P)folder_name.c_str(), folder_name.length(), 
      font, 
      &graphic_state, sizeof(graphic_state), 
      &text_state, sizeof(text_state), 
      &text_matrix, NULL 
    );

    ASFixedQuad quad = {0};
    PDETextGetQuad(text, kPDETextRun, 0, &quad);
    ASFixedMatrix new_matrix;
    PDETextGetTextMatrix(text, kPDETextRun, 0, &new_matrix);
    new_matrix.h = ASFixedDiv(
      ASFloatToFixed(repromatic::ConvertPrintUnits::MmToPoint(page_width)) - quad.tr.h,
      ASInt32ToFixed(2)
    );
    PDETextRunSetTextMatrix(text, 0, &new_matrix);

    graphic_state.fillColorSpec.value.color[3] = ASFloatToFixed(0.5f);
    text_matrix.a = ASInt32ToFixed(7);
    text_matrix.d = ASInt32ToFixed(7);

    PDETextAdd( 
      text, kPDETextRun, 0, 
      (ASUns8P)folder_path.c_str(), folder_path.length(), 
      font, 
      &graphic_state, sizeof(graphic_state), 
      &text_state, sizeof(text_state), 
      &text_matrix, NULL 
    );

    PDETextGetQuad(text, kPDETextRun, 0, &quad);
    PDETextGetTextMatrix(text, kPDETextRun, 0, &new_matrix);
    new_matrix.h = ASFixedDiv(
      ASFloatToFixed(repromatic::ConvertPrintUnits::MmToPoint(page_width)) - quad.tr.h,
      ASInt32ToFixed(2)
    );
    new_matrix.v = new_matrix.v + (quad.tl.v - quad.bl.v);
    PDETextRunSetTextMatrix(text, 0, &new_matrix);

    PDEContent content = PDPageAcquirePDEContent(page, gExtensionID); 
    PDEContentAddElem(content, kPDEAfterLast, PDEElement(text));
    PDPageSetPDEContent(page, gExtensionID);
    PDPageNotifyContentsDidChange(page);

    PDERelease(PDEObject(font));
    PDPageReleasePDEContent(page, gExtensionID);
  }

  status_monitor.EndOperation();

  // Free resources
  ASFileSysReleasePath(file_sys, root);

  // Display doc
  if (PDDocGetNumPages(pddoc) > 0) {
    /*AVDocOpenParams params{0};
    params->size = sizeof(AVDocOpenParamsRec);
    params->useViewDef = true;

    AVDocViewDef view_def{0};
    view_def->size = sizeof(AVDocViewDef);
    view_def->pageViewZoomType = AVZoomFitPage;

    params->viewDef = view_def;*/

    AVDoc avdoc = AVDocOpenFromPDDocWithParamString(
      pddoc,
      ASTextFromEncoded("Divider", AVAppGetLanguageEncoding()),
      NULL,
      "page=2&view=fit,100");
  } else {
    PDDocClose(pddoc);
    AVAlertNote("No folders found.");
  }
}

// Divider Callbacks
ACCB1 ASBool ACCB2 DividerIsEnabled(void *clientData) { return true; }
ACCB1 void ACCB2 Divider(void *clientData) { CreateDivider(); }
