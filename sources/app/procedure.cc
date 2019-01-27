#include "procedure.h"

#include "app/state.h"
#include "resources.h"  // IDM_FORMAT_ITEMS
#include "utils/convert.h"  // wide_to_narrow_str()
#include "format.h"  // .get_formats()
#include "procedures/WM_DROPFILES.h"  // WM_DROPFILES_FUNC()
#include "procedures/WM_COMMAND.h"  // WM_COMMAND_FUNC()
#include "utils/win.h"  // assert_rv()

#include <string>
#include "PIHeaders.h"

LRESULT CALLBACK RepromaticWndProc(HWND window_handle, UINT msg, WPARAM wParam, LPARAM lParam)
{
        app_state *state;
        if (msg != WM_CREATE && msg != WM_NCCREATE) {
                LONG_PTR userdata = GetWindowLongPtr(window_handle, GWLP_USERDATA);

                if (userdata == 0)
                        return 0;
                     
                state = reinterpret_cast<app_state*>(userdata);  
        }

        switch(msg) {
        case WM_CREATE: {
                try {
                        BOOL rv;                

                        // Set state pointer
                        CREATESTRUCT *create_struct = reinterpret_cast<CREATESTRUCT*>(lParam);
                        state = reinterpret_cast<app_state*>(create_struct->lpCreateParams);
                        SetWindowLongPtr(window_handle, GWLP_USERDATA, (LONG_PTR)state);

                        // Set the client area
                        rv = GetClientRect(window_handle, &state->window_client_rect);                
                        assert_rv(L"GetClientRect", rv);
                        state->client_width = static_cast<float>(state->window_client_rect.right - state->window_client_rect.left);
                        state->client_height = static_cast<float>(state->window_client_rect.bottom - state->window_client_rect.top);

                        // Create format menu
                        HMENU format_menu = CreatePopupMenu();
                        assert_rv(L"CreatePopupMenu", format_menu == NULL ? false : true);
                        state->format_menu = format_menu;

                        std::vector<std::wstring> formats = state->formats.get_formats();
                        for (size_t i = 0; i < formats.size(); ++i) {
                                std::wstring format = formats[i];
                                rv = AppendMenu(state->format_menu, MF_STRING,
                                        IDM_FORMAT_ITEMS + i, format.c_str());
                                assert_rv(L"AppendMenu", rv);
                        }

                        rv = CheckMenuRadioItem(state->format_menu, IDM_FORMAT_ITEMS,
                                IDM_FORMAT_ITEMS + formats.size() - 1, IDM_FORMAT_ITEMS, MF_BYCOMMAND);
                        assert_rv(L"CheckMenuRadioItem", rv);

                        rv = AppendMenuW(GetMenu(window_handle), MF_STRING | MF_POPUP,
                                reinterpret_cast<UINT>(state->format_menu), L"Formats");
                        assert_rv(L"AppendMenuW", rv);
                } catch (const std::exception &e) {
                        MessageBoxW(window_handle, narrow_to_wide_str(e.what()).c_str(),
                                L"Failed to create Window", MB_OK);
                        return -1;
                }
        } break;

        case WM_COMMAND:
                try {
                        WM_COMMAND_FUNC(state, window_handle, LPARAM(wParam));
                } catch(const std::exception &e) {
                        MessageBoxW(window_handle, narrow_to_wide_str(e.what()).c_str(),
                                L"Failed to execute menu item", MB_OK);
                }

                return 0;
    
        case WM_DROPFILES:
                try {
                        WM_DROPFILES_FUNC(state, window_handle, reinterpret_cast<HDROP>(wParam));
                } catch(const std::exception &e) {
                        MessageBoxW(window_handle, narrow_to_wide_str(e.what()).c_str(),
                                L"Failed to drop files", MB_OK);
                }

                return 0;
                
        case WM_CLOSE:
                DestroyWindow(window_handle);
                break;

        case WM_DESTROY:
                PostQuitMessage(0);
                break;

        default:
                return DefWindowProc(window_handle, msg, wParam, lParam);
        }

        return 0;
}