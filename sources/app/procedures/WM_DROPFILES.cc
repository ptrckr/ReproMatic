#include "WM_DROPFILES.h"

#include "app/state.h"
#include "app/classes/file_storage.h"

#include <string>

#ifndef MAC_PLATFORM
#include "PIHeaders.h"
#include <shellapi.h>  // DragQueryFileW()
#endif

int WM_DROPFILES_FUNC(app_state *state, HWND window_handle, HDROP hdrop) {
	int file_count = DragQueryFileW(hdrop, 0xFFFFFFFF, NULL, NULL);
	if (file_count == 0) {
		MessageBoxW(window_handle, L"Could not query filecount.",
			L"WM_DROPFILES::DragQueryFileW", MB_OK);
		return 0;
	}

	for (int i = 0; i < file_count; ++i) {
		int buffer_size = DragQueryFileW(hdrop, i, NULL, NULL);
		if (buffer_size == 0) {
			MessageBoxW(window_handle, L"Could not query string buffer size.",
				L"WM_DROPFILES::DragQueryFileW", MB_OK);
			return 0;
		}
		buffer_size += 1;

		LPWSTR buffer = new WCHAR[buffer_size];
		if (DragQueryFileW(hdrop, i, buffer, buffer_size) != std::char_traits<wchar_t>::length(buffer)) {
			delete[] buffer;
			MessageBoxW(window_handle, L"Could not copy string into buffer.",
				L"WM_DROPFILES::DragQueryFileW", MB_OK);
			return 0;
		}

		state->files.add_file(std::wstring(buffer));
		delete[] buffer;
	}

	return 0;
}