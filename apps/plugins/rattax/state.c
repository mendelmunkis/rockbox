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

#include "rattax.h"
#include "cplayer.h"
#include "board.h"
#include "state.h"
#include "util.h"

#define FILE_BOARD ROCKBOX_DIR "/rattax.board"
#define FILE_SETUP ROCKBOX_DIR "/rattax.setting"

static int board_position_load(void);
static int board_position_save(void);
static void board_settings_load(void);
static void board_settings_save(void);

int board_save(void)
{
        board_settings_save();
        return board_position_save();
}

int board_load(void)
{
        board_settings_load();
        return board_position_load();
}

char saved_cell_mark(int v)
{
        switch(v) {
                case C_WHITE: return 'W';
                case C_BLACK: return 'B';
                case C_OBST: return '*';
                case C_EMPTY: return '-';
                default: return '?';
        }
}

int board_position_save(void)
{
        char buf[2*board_w + 2];
        int r, c;
        int fd;

        if((fd = rb->open(FILE_BOARD, O_WRONLY | O_CREAT)) <= 0)
                return -1;

        for(r = 0; r < board_h; r++) {
                for(c = 0; c < board_w; c++) {
                        buf[2*c] = saved_cell_mark(board_get(r, c));
                        buf[2*c + 1] = ' ';
                }
                buf[2*c - 1] = '\n';
                buf[2*c] = '\0';
                rb->write(fd, buf, 2*c);
        }

        rb->close(fd);
        return 0;
}

int board_position_load(void)
{
        char buf[BOARD_MAX_SIZE];
        int len;
        char* p;
        int* b;
        int* c;
        int fd;
        int ret = -2;

        board_settings_load();

        if((fd = rb->open(FILE_BOARD, O_RDONLY)) <= 0)
                return -1;

        board_clean();
        board_p = board;
        board_w = board_h = 0;
        b = board; c = b;
        while((len = rb->read(fd, buf, sizeof(buf))) > 0) {
                for(p = buf; p - buf < len; p++) {
                        switch(*p) {
                                case 'W': *(b++) = C_WHITE; break;
                                case 'B': *(b++) = C_BLACK; break;
                                case '-': *(b++) = C_EMPTY; break;
                                case '*': *(b++) = C_OBST; break;
                                case '\n':
                                        if(board_w <= 0)
                                                board_w = b - c;
                                        else if(b - c != board_w)
                                                goto exit;
                                        c = b;
                        }
                        if(b - board > BOARD_MAX_SIZE)
                                goto exit;
                }
        }

        if(board_w <= 0)
                goto exit;

        board_h = (b - board) / board_w;
        if(board_h <= 0)
                goto exit;

        if(board_game_end())
                ret = 1;
        else
                ret = 0;

exit:
        rb->close(fd);
        return ret;
}

void board_remove_file(void)
{
        rb->remove(FILE_BOARD);
}

void board_settings_save(void)
{
        int fd;

        if((fd = rb->open(FILE_SETUP, O_WRONLY | O_CREAT)) <= 0)
                return;

        rb->fdprintf(fd, "cplayer-level: %d\n", cplayer_level);
        rb->fdprintf(fd, "cplayer-color: %s",
                        cplayer_color == C_BLACK ? "black" : "white");

        rb->close(fd);
}

#define LEN 200

int atoi_in_range(const char* val, int from, int to)
{
        int v = rb->atoi(val);
        if(v < from)
                return from;
        if(v > to)
                return to;
        return v;
}

void board_settings_load(void)
{
        int fd;
        char line[LEN];
        char* name;
        char* val;

        if((fd = rb->open(FILE_SETUP, O_RDONLY)) <= 0)
                return;

        while(rb->read_line(fd, line, LEN) > 0) {
                if(!rb->settings_parseline(line, &name, &val))
                        continue;
                if(!rb->strcmp(name, "cplayer-level"))
                        cplayer_level = atoi_in_range(val, 0, 7);
                if(!rb->strcmp(name, "cplayer-color"))
                        cplayer_color = rb->strcmp(val, "white") ? C_BLACK : C_WHITE;
        }

        rb->close(fd);
}
