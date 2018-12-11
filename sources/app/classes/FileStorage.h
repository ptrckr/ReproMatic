#ifndef FILE_STORAGE_H
#define FILE_STORAGE_H

#include <vector>
#include <string>

#ifndef MAC_PLATFORM
  #include "PIHeaders.h"
#endif

struct FileStorage {
  std::vector<std::wstring> files;

  void AddFile(std::wstring path);
  std::wstring GetAddedFiles();
};

#endif