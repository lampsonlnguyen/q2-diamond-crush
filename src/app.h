/* SPDX-License-Identifier: GPL-3.0-or-later */
#ifndef DIAMOND_APP_H
#define DIAMOND_APP_H
#include "game.h"
#define SCREEN_W 375
#define SCREEN_H 320
#define CELL 36
#define PITCH 37
#define GRID_X ((SCREEN_W - COLS * PITCH) / 2)
#define GRID_Y 87
typedef uint32_t u32;
typedef struct { Game game; int light,wheel_rem; int drag,drag_x,drag_y;
    int animation,swap_a,swap_b; unsigned animation_ms; u32 accent; } App;
void app_init(App *a,unsigned seed,unsigned best);
int app_tick(App *a,unsigned elapsed);
int app_input(App *a,const char *message);
void app_render(const App *a,u32 *pixels,unsigned stride);
#endif
