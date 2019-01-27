#ifndef STATE_H
#define STATE_H

#include "app/format.h"
#include "app/file_storage.h"

struct app_state {
        // Window
        RECT window_client_rect;
        float client_width;
        float client_height;

        // Menu
        HMENU format_menu;

        // Format
        formats formats;

        // FileStorage
        file_tree files;
};

#endif