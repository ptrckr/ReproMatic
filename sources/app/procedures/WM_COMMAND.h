#ifndef WM_COMMAND_H
#define WM_COMMAND_H

#include "app/state.h"
#include "utils/win.h"

#include "PIHeaders.h"

void WM_COMMAND_FUNC(app_state *state, HWND window_handle, LPARAM command)
{
        switch(LPARAM(command)) {
        case IDM_FILE_EXPORT: {
                std::wstring tmp;
                auto drives = state->files.drives;

                for (auto it = drives.cbegin(); it != drives.cend(); ++it) {
                        tmp += it->first + L"\n";
                        tmp += it->second.to_string() + L"\n";
                }

                MessageBoxW(window_handle, tmp.c_str(), L"Export View", MB_OK);
                } break;
        case IDM_FILE_CLOSE:
                PostMessageW(window_handle, WM_CLOSE, 0, 0);
                break;
        default:
                if (command >= IDM_FORMAT_ITEMS && command <= IDM_FORMAT_ITEMS_MAX) {
                        BOOL rv = CheckMenuRadioItem(state->format_menu, IDM_FORMAT_ITEMS,
                                IDM_FORMAT_ITEMS + state->formats.get_formats().size() - 1, command, MF_BYCOMMAND);
                        assert_rv(L"CheckMenuRadioItem", rv);
                }
                break;
        }
}

#endif