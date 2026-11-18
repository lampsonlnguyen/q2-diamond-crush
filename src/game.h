/* SPDX-License-Identifier: GPL-3.0-or-later */
#ifndef DIAMOND_GAME_H
#define DIAMOND_GAME_H
#include <stdint.h>
#define COLS 7
#define ROWS 5
#define CELLS (COLS * ROWS)
#define TYPES 6
typedef struct {
    uint8_t cell[ROWS][COLS];
    uint32_t seed, score, best, swaps;
    int cursor, selected, paused, quit, sound;
} Game;
void game_init(Game *g, uint32_t seed, uint32_t best);
void game_shuffle(Game *g);
int game_swap(Game *g, int a, int b);
int game_has_move(const Game *g);
int game_has_matches(const Game *g);
#endif
