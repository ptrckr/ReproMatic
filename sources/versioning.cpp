#include "versioning.h"

#include <string>

const std::string PluginData::CURRENT_VERSION("v1");
const std::string PluginData::DEVELOPER_PREFIX("PTRC:");
const std::string PluginData::PLUGIN_NAME("ReproMatic");
const std::string PluginData::FULL_PLUGIN_NAME(PluginData::PLUGIN_NAME + " " + PluginData::CURRENT_VERSION);
const std::string PluginData::FULL_DEV_PLUGIN_NAME(PluginData::DEVELOPER_PREFIX + PluginData::PLUGIN_NAME);