#ifndef WM_DROPFILES_H
#define WM_DROPFILES_H

#include <string>
#include <stdexcept>

#include "app/state.h"
#include "app/file_storage.h"
#include "utils/win.h"

#include "shellapi.h"  // DragQueryFileW()
#include "PIHeaders.h"

void WM_DROPFILES_FUNC(app_state *state, HWND window_handle, HDROP hdrop)
{
	int file_count = DragQueryFileW(hdrop, 0xFFFFFFFF, NULL, NULL);
        assert_rv(std::wstring(L"DragQueryFileW (file_count: ") +
                std::to_wstring(file_count) + L")", file_count);

	for (int i = 0; i < file_count; ++i) {
		int buffer_size = DragQueryFileW(hdrop, i, NULL, NULL);
                assert_rv(std::wstring(L"DragQueryFileW (buffer_size: ") +
                        std::to_wstring(buffer_size) + L")", buffer_size);

		buffer_size += 1;
		LPWSTR buffer = new WCHAR[buffer_size];
                BOOL rv = DragQueryFileW(hdrop, i, buffer, buffer_size) == std::char_traits<wchar_t>::length(buffer);
                assert_rv_cb(std::wstring(L"DragQueryFileW (") + buffer + L")", rv, [&buffer]() {
                        delete[] buffer;
                });

		state->files.add_file(std::wstring(buffer));
		delete[] buffer;
	}
}

#endif