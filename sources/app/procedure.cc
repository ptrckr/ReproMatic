#include "procedure.h"

#include "app/state.h"
#include "resources.h"

#include <string>

#ifndef MAC_PLATFORM
  #include "PIHeaders.h"
#endif

#include <string>

LRESULT CALLBACK RepromaticWndProc(HWND window_handle, UINT msg, WPARAM wParam, LPARAM lParam) {
  State* state;
  if (msg == WM_CREATE) {
    CREATESTRUCT* pCreate = reinterpret_cast<CREATESTRUCT*>(lParam);
    state = reinterpret_cast<State*>(pCreate->lpCreateParams);
    SetWindowLongPtr(window_handle, GWLP_USERDATA, (LONG_PTR)state);
  } else {
    LONG_PTR ptr = GetWindowLongPtr(window_handle, GWLP_USERDATA);
    state = reinterpret_cast<State*>(ptr);
  }

  switch(msg) {
    case WM_CREATE:
      // Set the client area
      if(GetClientRect(window_handle, &state->window_client_rect)) {
        state->client_width = static_cast<float>(state->window_client_rect.right - state->window_client_rect.left);
        state->client_height = static_cast<float>(state->window_client_rect.bottom - state->window_client_rect.top);
      } else {
        AVAlertNote("Error calling `GetClientRect'.");
        return -1;
      }
      break;

    case WM_COMMAND:
      switch(LPARAM(wParam)) {
        case IDM_FILE_CLOSE:
          std::string y = "Client area is width: " + std::to_string(state->client_width);
          AVAlertNote(y.c_str());
          PostMessageW(window_handle, WM_CLOSE, 0, 0);
          break;
      }
      break;

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