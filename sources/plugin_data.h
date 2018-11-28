#ifndef VERSIONING_H
#define VERSIONING_H

#include <string>

struct PluginData {
  static const std::wstring CURRENT_VERSION;
  static const std::wstring DEVELOPER_PREFIX;
  static const std::wstring PLUGIN_NAME;
  static const std::wstring FULL_PLUGIN_NAME;
  static const std::wstring FULL_DEV_PLUGIN_NAME;
  static const std::wstring EXTENSION_NAME;

  static const int WINDOW_WIDTH;
  static const int WINDOW_HEIGHT;
};

#endif