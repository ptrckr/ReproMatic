#include "app/classes/FileStorage.h"

#include <vector>
#include <string>

void FileStorage::AddFile(std::wstring path) {
  this->files.push_back(path);
}

std::wstring FileStorage::GetAddedFiles() {
  std::wstring acc(L"");

  for (const auto& s : this->files) {
    acc += s;
  }

  return acc;
}