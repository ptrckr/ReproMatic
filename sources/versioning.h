#ifndef VERSIONING_H
#define VERSIONING_H

#include <string>

struct PluginData {
  static const std::string CURRENT_VERSION;
  static const std::string DEVELOPER_PREFIX;
  static const std::string PLUGIN_NAME;
  static const std::string FULL_PLUGIN_NAME;
  static const std::string FULL_DEV_PLUGIN_NAME;
};

#endif