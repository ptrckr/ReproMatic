#ifndef WM_DROPFILES_H
#define WM_DROPFILES_H

#include "app/state.h"

#ifndef MAC_PLATFORM
  #include "PIHeaders.h"
#endif

int WM_DROPFILES_FUNC(app_state *state, HWND window_handle, HDROP hdrop);

#endif