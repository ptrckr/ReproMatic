#ifndef WM_COMMAND_H
#define WM_COMMAND_H

#include "app/state.h"
#include "utils/win.h"

#include <map>

#include "PIHeaders.h"

void WM_COMMAND_FUNC(app_state *state, HWND window_handle, LPARAM command)
{
        LONG_PTR cmd = LPARAM(command);

        switch(cmd) {
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
                if (cmd >= IDM_FORMAT_ITEMS_BEGIN && cmd <= IDM_FORMAT_ITEMS_END) {
                        int max = IDM_FORMAT_ITEMS_BEGIN + state->formats.get_formats().size() - 1;
                        BOOL rv = CheckMenuRadioItem(state->format_menu,
                                IDM_FORMAT_ITEMS_BEGIN, max, cmd, MF_BYCOMMAND);
                        assert_rv(L"CheckMenuRadioItem", rv);

                        std::wstring selected_format = state->format_menu_id_name_lookup.at(cmd);
                        state->formats.set_active_format(selected_format);
                        state->files.apply_format_to_pages(state->formats.get_active_format());
                }
                break;
        }
}

#endif