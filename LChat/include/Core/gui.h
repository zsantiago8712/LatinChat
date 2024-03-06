#pragma once

#include "Utils/types.h"

struct _win_st;

typedef struct {
    struct _win_st* main_window;
    struct _win_st* secondary_window;
    struct _win_st* input_window;

    i32 main_window_width;
    i32 main_window_height;

    i32 main_window_x;
    i32 main_window_y;

    i32 secondary_window_width;
    i32 secondary_window_height;

} Gui;

#define NewGui                             \
    (Gui) {                                \
        NULL, NULL, NULL, 0, 0, 0, 0, 0, 0 \
    }
