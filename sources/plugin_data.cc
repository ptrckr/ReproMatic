#include "plugin_data.h"

#include <string>

const std::wstring PluginData::CURRENT_VERSION(L"v0.1");
const std::wstring PluginData::DEVELOPER_PREFIX(L"PTRC:");
const std::wstring PluginData::PLUGIN_NAME(L"Repromatic");
const std::wstring PluginData::FULL_PLUGIN_NAME(PluginData::PLUGIN_NAME + L" " + PluginData::CURRENT_VERSION);
const std::wstring PluginData::FULL_DEV_PLUGIN_NAME(PluginData::DEVELOPER_PREFIX + PluginData::PLUGIN_NAME);
const std::wstring PluginData::EXTENSION_NAME(PluginData::DEVELOPER_PREFIX + PluginData::PLUGIN_NAME + PluginData::CURRENT_VERSION);

const int PluginData::WINDOW_WIDTH = 800;
const int PluginData::WINDOW_HEIGHT = 600;