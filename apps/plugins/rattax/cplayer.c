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
#include "util.h"
#include "cplayer.h"

#define MAX_MOVES 50                            /* XXX: get exact number */
#define MAX_DEPTH CPLAYER_MAX_DEPTH

int scratch[MAX_DEPTH*BOARD_MAX_SIZE];
move_t mlist[MAX_DEPTH*MAX_MOVES];
int mvals[MAX_MOVES];

int cplayer_color = C_BLACK;
int cplayer_level = 4;

#define NULL (void*)0

static int bvalue(int player);
static int list_moves(int cp, move_t* lst, int lst_length);
static int estimate(move_t* mlist, int ml, int cp, int depth, int ival);
static int is_glide(move_t* m);

int* scratch_board(int i)
{
        if(i < 0 || (i + 1)*board_w*board_h > (int)(alength(scratch)))
                return NULL;

        return scratch + i*board_w*board_h;
}

void cplayer_make_move(void)
{
        move_t m;
        if(cplayer(cur_player, &m))
                board_make_move(m.cr, m.cc, m.nr, m.nc, cur_player);
        cur_player = other(cur_player);
}

void cplayer_set_level(int l)
{
        cplayer_level = l;
}

int cplayer(int cp, move_t* move)
{
        move_t* cm;
        int i;
        int nm;
        int vl;
        int best = -10000;
        int bcnt = 0;
        int gcnt = 0;
        int sm;

        if(!(nm = list_moves(cp, mlist, alength(mlist))))
                return 0;

        if(!cplayer_level) {
                *move = mlist[rbrand() % nm];
                return 1;
        }

        for(i = 0; i < nm; i++) {
                cm = &mlist[i];
                board_ptr(scratch_board(0));
                board_copy(board);
                board_make_move(cm->cr, cm->cc, cm->nr, cm->nc, cp);
                vl = -estimate(mlist + nm, alength(mlist) - nm,
                                other(cp), 1, 10000);
                if(vl > best) {
                        best = vl;
                        bcnt = 1;
                } else if(vl == best) {
                        bcnt++;
                }
                mvals[i] = vl;
        }
        board_ptr(board);

        if(!bcnt)
                return 0;

        for(i = 0; i < nm; i++)
                if(mvals[i] == best && is_glide(&mlist[i]))
                        gcnt++;
        if(gcnt) {
                bcnt = gcnt;
                for(i = 0; i < nm; i++)
                        if(mvals[i] == best && !is_glide(&mlist[i]))
                                mvals[i] = -10000;
        }

        sm = rbrand() % bcnt;
        for(i = 0; i < nm; i++)
                if(mvals[i] == best)
                        if(sm-- == 0) {
                                *move = mlist[i];
                                return 1;
                        }

        return 0;
}

static int is_glide_delta(int dx, int dy)
{
        return (rbabs(dx) < 2 && rbabs(dy) < 2);
}

static int is_glide(move_t* m)
{
        return is_glide_delta(m->cr - m->nr, m->cc - m->nc);
}

int estimate(move_t* mlist, int ml, int cp, int depth, int ival)
{
        move_t* cm;                             // current move
        int nm;                                 // num of moves
        int i;
        int vl;                                 // disposition value
        int best = -10000;
        int* initial = board_ptr_get();

        if(depth >= cplayer_level)
                return bvalue(cp);
        
        if(!(nm = list_moves(cp, mlist, ml)))
                return -10000;

        for(i = 0; i < nm; i++) {
                cm = &mlist[i];
                board_ptr(scratch_board(depth));
                board_copy(initial);
                board_make_move(cm->cr, cm->cc, cm->nr, cm->nc, cp);
                vl = -estimate(mlist + nm, ml - ml, other(cp), depth + 1, -best);
                if(vl > best)
                        best = vl;
                if(best > ival)
                        break;
        }

        return best;
}

int bvalue(int player)
{
        int r, c;
        int good = 0, bad = 0, empty = 0;
        int sq;
        int op = other(player);

        foreach_cell(r, c) {
                sq = board_get(r, c);
                if(sq == player)
                        good++;
                else if(sq == op)
                        bad++;
                else if(sq == C_EMPTY)
                        empty++;
        }

        if(empty == 0) {
                if(good == bad)
                        return 0;
                else if(good > bad)
                        return  1000;
                else
                        return -1000;
        } else if(good == 0) {
                return -1000;
        } else if(bad == 0) {
                return  1000;
        } else {
                return (good - bad);
        }
}

static int list_moves(int cp, move_t* lst, int lst_length)
{
        int r, c;
        int* m;
        move_t* p = lst;

        // huge moronic optimization:
        // * don't list splits from different cells to the same cell
        // * don't list jump if this cell is reachable by split -- REMOVED
        // * do this all using primary board (no additional memory allocated)

        foreach_cell(r, c) {
                if(board_get(r, c) != cp)
                        continue;
                foreach_move(m) {
                        switch(board_get(r + m[0], c + m[1])) {
                                case C_MARK:
                                        if(is_glide_delta(m[0], m[1]))
                                                continue;
                                case C_EMPTY:
                                        p->cr = r;
                                        p->cc = c;
                                        p->nr = r + m[0];
                                        p->nc = c + m[1];
                                        p++;
                                        if(is_glide_delta(m[0], m[1]))
                                                board_set(r + m[0], c + m[1], C_MARK);
                                        if(p - lst >= lst_length)
                                                goto exit;
                        }
                }
        }

exit:
        // remove marks, if any
        foreach_cell(r, c)
                if(board_get(r, c) == C_MARK)
                        board_set(r, c, C_EMPTY);

        return p - lst;
}
