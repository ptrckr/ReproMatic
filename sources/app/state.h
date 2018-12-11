#ifndef STATE_H
#define STATE_H

#include "app/classes/FileStorage.h"

struct AppState {
  // Window
  RECT window_client_rect;
  float client_width;
  float client_height;

  // FileStorage
  FileStorage files;
};

#endif