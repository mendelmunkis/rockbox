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

#include "board.h"
#include "bsets.h"
#include "util.h"

int* board_p;
int board_w, board_h;
int cur_player;
int cur_r, cur_c;
int tag_r, tag_c;

int board[BOARD_MAX_SIZE];

#define array2(name, ...) \
        int name[] = { __VA_ARGS__ };\
        const int name##_size = sizeof(name)/sizeof(*name)

/* Note: these are iterated backwards */
array2(moves,
        /* jump */
         2,  0,
         2,  1,         /*  j j j j j  */ 
         2,  2,         /*  j       j  */
         1,  2,         /*  j   x   j  */
         0,  2,         /*  j       j  */
        -1,  2,         /*  j j j j j  */
        -2,  2,
        -2,  1,
        -2,  0,
        -2, -1,
        -2, -2,
        -1, -2,
         0, -2,
         1, -2,
         2, -2,
         2, -1,
        /* split */
         1,  0,         /*             */ 
         1,  1,         /*    s s s    */
         0,  1,         /*    s x s    */
        -1,  1,         /*    s s s    */
        -1,  0,         /*             */
        -1, -1,
         0, -1,
         1, -1
);

array2(ranges,
         1,  0,
         1,  1,
         0,  1,
        -1,  1,
        -1,  0,
        -1, -1,
         0, -1,
         1, -1
);

int other(int player)
{
        if(player == P_BLACK) return P_WHITE;
        if(player == P_WHITE) return P_BLACK;
        return player;
}

void board_ptr(int* p)
{
        board_p = p;
}

int* board_ptr_get(void)
{
        return board_p;
}

void board_set(int r, int c, int obj)
{
        if(r < 0 || r >= board_h)
                return;
        if(c < 0 || c >= board_w)
                return;

        board_p[r*board_w + c] = obj;
}

int board_get(int r, int c)
{
        if(r < 0 || r >= board_h)
                return C_OBST;
        if(c < 0 || c >= board_w)
                return C_OBST;

        return board_p[r*board_w + c];
}

// tag{row,col} -> cur{row,col} by current player
int board_make_move(int tr, int tc, int cr, int cc, int cp)
{
        int dc, dr;

        if(board_get(tr, tc) != cp)
                return 0;

        if(board_get(cr, cc) != C_EMPTY)
                return 0;

        dc = rbabs(cc - tc);
        dr = rbabs(cr - tr);

        if(dc <= 1 && dr <= 1) {
                board_set(cr, cc, cp);
                board_eat_pieces(cr, cc, cp);
                return 1;
        }

        if(dc <= 2 && dr <= 2) {
                board_set(cr, cc, cp);
                board_set(tr, tc, C_EMPTY);
                board_eat_pieces(cr, cc, cp);
                return 1;
        }

        return 0;
}

void board_eat_pieces(int r, int c, int player)
{
        int* p;
        int oth = other(player);

        foreach_range(p)
                if(board_get(r + p[0], c + p[1]) == oth)
                        board_set(r + p[0], c + p[1], player);
}

int board_game_end(void)
{
        int r, c;

        foreach_cell(r, c)
                if(board_get(r, c) == C_EMPTY)
                        return 0;
        
        return 1;
}

int board_count(int player)
{
        int r, c;
        int count = 0;

        foreach_cell(r, c)
                if(board_get(r, c) == player)
                        count++;

        return count;
}

int has_move(int player)
{
        int r, c, *p;

        foreach_cell(r, c)
                if(board_get(r, c) == player)
                        foreach_move(p)
                                if(board_get(r + p[0], c + p[1]) == C_EMPTY)
                                        return 1;

        return 0;
}

void board_copy(int* data)
{
        rbmemcpy(board_p, data, board_w*board_h*sizeof(int));
}

void board_rand(void)
{
        int i;
        int r = rbrand();
        bset_t set = bsets[r % bset_count];
        board_init(set.data[0], set.data[1]);
        for(i = 2; i < set.len; i += 2)
                board_set(set.data[i], set.data[i+1], C_OBST);
}

void board_init(int w, int h)
{
        int r, c;

        if(w > 0 && h > 0 && w*h <= BOARD_MAX_SIZE) {
                board_p = board;
                board_w = w;
                board_h = h;
                foreach_cell(r, c)
                        board_set(r, c, C_EMPTY);
                board_reset();
        }
}

void board_reset(void)
{
        tag_c = -1;
        tag_r = -1;
        cur_player = P_WHITE;

        board_set(        0,         0, C_WHITE);
        board_set(board_w-1, board_h-1, C_WHITE);
        board_set(board_w-1,         0, C_BLACK);
        board_set(        0, board_h-1, C_BLACK);
}

/* this is "button press" over cur_r, cur_c */
int board_act(void)
{
        if(tag_r < 0) {
                if(board_get(cur_r, cur_c) == cur_player) {
                        tag_r = cur_r;
                        tag_c = cur_c;
                };
        } else {
                if(board_get(cur_r, cur_c) == cur_player) {
                        // tag another piece
                        tag_r = cur_r;
                        tag_c = cur_c;
                } else if(!board_make_move(tag_r, tag_c, cur_r, cur_c, cur_player)) {
                        // incorrect move - do nothing
                        return 0;
                } else {
                        // move done, reset tag
                        tag_r = -1;
                        tag_c = -1;
                        cur_player = other(cur_player);
                        return 1;
                }
        }
        return 0;
}

void board_move_cursor(int dr, int dc)
{
        cur_r = (board_h + cur_r + dr) % board_h;
        cur_c = (board_w + cur_c + dc) % board_w;
}

void board_clean(void)
{
        int r, c;

        foreach_cell(r, c)
                if(board_get(r, c) != C_OBST)
                        board_set(r, c, C_EMPTY);

        board_reset();
}

void board_place_cursor_for(int player)
{
        if(player == P_BLACK) {
                cur_r = board_w - 1;
                cur_c = 0;
        } else {
                cur_r = 0;
                cur_c = 0;
        }
}
