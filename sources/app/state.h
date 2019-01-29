#ifndef STATE_H
#define STATE_H

#include "app/format.h"
#include "app/file_storage.h"

#include <map>

struct app_state {
        // Window
        RECT window_client_rect;
        float client_width;
        float client_height;

        // Menu
        std::map<int, std::wstring> format_menu_id_name_lookup; 
        HMENU format_menu;

        // Format
        formats formats;

        // FileStorage
        file_tree files;
};

#endif