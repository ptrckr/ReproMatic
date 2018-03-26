//	
//  repromatic.cc
//  Created by ptrckr on 24.03.18.	
//	

#include "repromatic.h"
#include <string>
#include <functional>
#include <regex>

#ifndef MAC_PLATFORM
#include "PIHeaders.h"
#endif

namespace repromatic {

bool IsRectWithinBounds(float src_width, float src_height, float ref_width, float ref_height, float tolerance) {
  if (src_width - tolerance < ref_width && src_height - tolerance < ref_height ||
      src_width - tolerance < ref_height && src_height - tolerance < ref_width) {
    return true;
  }

  return false;
}

void IterateFolder(
    ASFileSys file_system,
    ASPathName root,
    ASPathName folder,
    std::function<void(
      ASFileSys file_system,
      ASPathName root,
      ASPathName item_path,
      std::string item_type
    )> callback,
    std::function<bool()> continue_iteration) {
  ASFileSysItemPropsRec props;
  memset(&props, NULL, sizeof(ASFileSysItemPropsRec));
  props.size = sizeof(ASFileSysItemPropsRec);

  ASPathName as_item_path;
  ASFolderIterator folder_iterator = ASFileSysFirstFolderItem(file_system, folder, &props, &as_item_path);

  if (folder_iterator == NULL) return;

  std::regex is_pdf(R"(.*\.pdf$)", std::regex::icase);
  do {
    if (props.type == kASFileSysFile) {
      char *item_path = ASFileSysDIPathFromPath(file_system, as_item_path, NULL);

      if (std::regex_match(item_path, is_pdf)) {
        callback(file_system, root, as_item_path, "file_pdf");
      } else {
        callback(file_system, root, as_item_path, "file_non_pdf");
      }

      ASfree(item_path);
    } else if (props.type == kASFileSysFolder) {
      ASText folder_name = ASTextNew();

      if (ASFileSysGetNameFromPathAsASText(file_system, as_item_path, folder_name) != 0) {
        AVAlert(ALERT_STOP, "Folder name could not be retrieved.", "OK", NULL, NULL, true);
      } else if (ASTextCaseSensitiveCmp(folder_name, ASTextFromEncoded("Sorted", AVAppGetLanguageEncoding())) != 0) {
        callback(file_system, root, as_item_path, "folder");
        IterateFolder(file_system, root, as_item_path, callback, continue_iteration );
      }
    }

    ASFileSysReleasePath(file_system, as_item_path);
  } while (continue_iteration() && ASFileSysNextFolderItem(file_system, folder_iterator, &props, &as_item_path));

  ASFileSysDestroyFolderIterator(file_system, folder_iterator);
}

int GetItemCount(ASPathName root, ASFileSys file_system, std::string filter) {
  int file_count = 0;

  IterateFolder(file_system, root, root, [&file_count, filter](ASFileSys file_system, ASPathName root, ASPathName item_path, std::string item_type) -> void {
    if (filter == "") {
      file_count += 1;
    } else {
      if (filter == item_type) {
        file_count += 1;
      }
    }
  });

  return file_count;
}

ASPathName GetFirstPdfFileInFolder(ASFileSys file_system, ASPathName folder) {
  ASPathName first_file = nullptr;
  bool found_first_file = false;

  std::function<bool()> continue_iteration = [&found_first_file]() -> bool {
    return !found_first_file ? true : false;
  };

  IterateFolder(file_system, folder, folder, [&found_first_file, &first_file](ASFileSys file_system, ASPathName root, ASPathName item_path, std::string item_type) -> void {
    if (item_type == "file_pdf") {
      found_first_file = true;
      first_file = ASFileSysCopyPath(file_system, item_path);
    }
  }, continue_iteration);

  return first_file;
}

}  // repromatic
