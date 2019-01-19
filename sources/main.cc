#include "plugin_data.h"
#include "app/create_window.h"
#include "acrobat_utils.h"  // ACROBAT_UTILS::MenuManager
#include "utils/convert.h"  // wide_to_narrow_str()

#include <string>

#ifndef MAC_PLATFORM
#include "PIHeaders.h"
#endif

static MenuManager menu_manager;

ASAtom GetExtensionName() {
        return ASAtomFromString(wide_to_narrow_str(PluginData::EXTENSION_NAME).c_str());
}

ACCB1 ASBool ACCB2 PluginExportHFTs(void) {
        return true;
}

ACCB1 ASBool ACCB2 PluginImportReplaceAndRegister(void) {
        return true;
}

ACCB1 ASBool ACCB2 PluginInit(void) {
        menu_manager.Init();

        DURING
                menu_manager.AddMenuItemToMenu(
                        L"", L"Start Repromatic " + PluginData::CURRENT_VERSION,
                        ASCallbackCreateProto(AVExecuteProc, LaunchRepromaticWindow),
                        ASCallbackCreateProto(AVComputeEnabledProc, LaunchRepromaticWindowIsEnabled)
                );

                menu_manager.ReleaseMenus();
        HANDLER
                menu_manager.ReleaseMenus();

                return false;
        END_HANDLER

        return true;
}

ACCB1 ASBool ACCB2 PluginUnload(void) {
        menu_manager.RemoveMenuItems();

        return true;
}

ACCB1 ASBool ACCB2 PIHandshake(Uns32 handshakeVersion, void* handshakeData) {
        if (handshakeVersion == HANDSHAKE_V0200) {
                PIHandshakeData_V0200 *hsData = (PIHandshakeData_V0200*)handshakeData;
                hsData->extensionName = GetExtensionName();
                hsData->exportHFTsCallback = (void*)ASCallbackCreateProto(PIExportHFTsProcType, &PluginExportHFTs);
                hsData->importReplaceAndRegisterCallback = (void*)ASCallbackCreateProto(PIImportReplaceAndRegisterProcType, &PluginImportReplaceAndRegister);
                hsData->initCallback = (void*)ASCallbackCreateProto(PIInitProcType, &PluginInit);
                hsData->unloadCallback = (void*)ASCallbackCreateProto(PIUnloadProcType, &PluginUnload);
                return true;
        }

        return false;
}
