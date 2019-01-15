#include "procedure.h"

#include "app/state.h"
#include "resources.h"
#include "utils/convert.h"  // wide_to_narrow_str()
#include "procedures/WM_DROPFILES.h"  // WM_DROPFILES_FUNC()

#include <string>

#ifndef MAC_PLATFORM
#include "PIHeaders.h"
#endif

LRESULT CALLBACK RepromaticWndProc(HWND window_handle, UINT msg, WPARAM wParam, LPARAM lParam) {
        app_state* state;
        if (msg != WM_CREATE && msg != WM_NCCREATE) {
                LONG_PTR ptr = GetWindowLongPtr(window_handle, GWLP_USERDATA);

                if (ptr == 0)
                        return 0;
                     
                state = reinterpret_cast<app_state*>(ptr);  
        }

        switch(msg) {
        case WM_CREATE: {
                // Set state pointer
                CREATESTRUCT* create_struct = reinterpret_cast<CREATESTRUCT*>(lParam);
                state = reinterpret_cast<app_state*>(create_struct->lpCreateParams);
                SetWindowLongPtr(window_handle, GWLP_USERDATA, (LONG_PTR)state);

                // Set the client area
                if(GetClientRect(window_handle, &state->window_client_rect)) {
                        state->client_width = static_cast<float>(state->window_client_rect.right - state->window_client_rect.left);
                        state->client_height = static_cast<float>(state->window_client_rect.bottom - state->window_client_rect.top);
                } else {
                        AVAlertNote("Error calling `GetClientRect()'.");
                        return -1;
                }
        } break;

        case WM_COMMAND:
                switch(LPARAM(wParam)) {
                case IDM_FILE_CLOSE:
                        PostMessageW(window_handle, WM_CLOSE, 0, 0);
                        break;
                case IDM_FILE_EXPORT: {
                        std::wstring tmp;

                        for (auto it = state->files.drives.cbegin(); it != state->files.drives.cend(); ++it) {
                                tmp += it->first + L"\n";
                                tmp += it->second.to_string() + L"\n";
                        }

                        MessageBoxW(window_handle, tmp.c_str(), L"Export View", MB_OK);
                        } break;
                }
        break;
    
        case WM_DROPFILES:
                return WM_DROPFILES_FUNC(state, window_handle, reinterpret_cast<HDROP>(wParam));

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