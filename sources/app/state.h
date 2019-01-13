#ifndef STATE_H
#define STATE_H

#include "app/classes/file_storage.h"

struct app_state {
        // Window
        RECT window_client_rect;
        float client_width;
        float client_height;

        // FileStorage
        file_tree files;
};

#endif