#include "procedure.h"

#include "resources.h"

#ifndef MAC_PLATFORM
  #include "PIHeaders.h"
#endif

#include <string>

LRESULT CALLBACK RepromaticWndProc(HWND window_handle, UINT msg, WPARAM wParam, LPARAM lParam) {
  switch(msg) {
    case WM_COMMAND:
      switch(wParam) {
        case ID_FILE_CLOSE:
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








// 49309
// 127
