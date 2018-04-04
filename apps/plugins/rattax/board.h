/***************************************************************************
 *             __________               __   ___.
 *   Open      \______   \ ____   ____ |  | _\_ |__   _______  ___
 *   Source     |       _//  _ \_/ ___\|  |/ /| __ \ /  _ \  \/  /
 *   Jukebox    |    |   (  <_> )  \___|    < | \_\ (  <_> > <  <
 *   Firmware   |____|_  /\____/ \___  >__|_ \|___  /\____/__/\_ \
 *                     \/            \/     \/    \/            \/
 * $Id$
 *
 * RAttax - Attax arcade game implementation for Rockbox
 *
 * Copyright (c) 2008 Alex Suykov <axs@ukr.net>
 * Inspired by XAttax originally written by
 *      Steven Gifford <sg10+@andrew.cmu.edu>
 *      Jim Zelenka <jz1j+@andrew.cmu.edu>
 *      Andrew Plotkin <ap1i+@andrew.cmu.edu>
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; either version 2
 * of the License, or (at your option) any later version.
 *
 * This software is distributed on an "AS IS" basis, WITHOUT WARRANTY OF ANY
 * KIND, either express or implied.
 *
 ****************************************************************************/

#ifndef __BOARD_H__
#define __BOARD_H__

#define C_EMPTY 0
#define C_BLACK 1
#define C_WHITE 2
#define C_OBST 3
#define C_MARK 4                // marks are used internally by bot

#define P_BLACK C_BLACK
#define P_WHITE C_WHITE
#define P_GEND 5                // game end, no current player

#define BOARD_MAX_SIZE 7*7

/* game UI state */
extern int cur_r, cur_c;
extern int tag_r, tag_c;
extern int cur_player;

/* internal "current board" */
extern int* board_p;
extern int board_w, board_h;

/* buffer for primary game board */
extern int board[];

/* static lists of possible moves */
extern int moves[];
extern int ranges[];
extern const int moves_size;
extern const int ranges_size;

// end of array containing flat int pairs
#define eo2(array) (array + array##_size - 2)
// foreach pair in such array
#define foreach2(m,array) for(m = eo2(array); m >= array; m -= 2)

#define foreach_cell(r, c) for(r = 0; r < board_h; r++) for(c = 0; c < board_w; c++)
#define foreach_range(m) foreach2(m, ranges)
#define foreach_move(m) foreach2(m, moves)

int other(int player);

/* get/set "current board" (the on board_{get,set,count,...} work with */
void board_ptr(int* ptr);
int* board_ptr_get(void);

void board_set(int r, int c, int obj);
int board_get(int r, int c);

int board_game_end(void);

void board_clean(void);
void board_reset(void);
int board_count(int player);
void board_copy(int* data);

/* make a move from tag(r,c) to cur(r,c) as cur_player, return !0 if move is forbidden */
int board_make_move(int tr, int tc, int cr, int cc, int cp);
/* change color of pieces affected after move */
void board_eat_pieces(int r, int c, int player);
/* check whether player has valid moves */
int has_move(int player);

void board_rand(void);
void board_init(int w, int h);

void board_move_cursor(int dr, int dc);
void board_place_cursor_for(int player);

/* user pressed a button over cur_r, cur_c */
int board_act(void);
/* make a move from tag(r,c) to cur(r,c) as cur_player, return !0 if move is forbidden */
int board_make_move(int tr, int tc, int cr, int cc, int cp);
void board_eat_pieces(int r, int c, int player);
/* check whether player has valid moves */
int has_move(int player);

#endif
