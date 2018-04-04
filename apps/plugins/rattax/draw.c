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
#include "board.h"
#include "draw.h"
#include "util.h"

#include "pluginbitmaps/rattax_cells.h"
//#define BMPWIDTH_rattax_cells 20
#define TILE_W BMPWIDTH_rattax_cells
#define TILE_H TILE_W

int xofs = 0;
int yofs = 0;
int screen_w = -1;
int screen_h = -1;
int xend = -1;
int yend = -1;

#define BMP_EMPTY 0
#define BMP_OBST 2
#define BMP_BLACK 4
#define BMP_WHITE 7

void draw_board_init(void)
{
        struct screen* display = rb->screens[0];

        if(screen_w < 0)
                screen_w = display->getwidth();
        if(screen_h < 0)
                screen_h = display->getheight();

        xofs = (screen_w/2 - board_w*TILE_W/2);
        yofs = (screen_h/2 - board_h*TILE_H/2);
        xend = xofs + board_w*TILE_W;
        yend = yofs + board_h*TILE_H;

#if LCD_DEPTH > 1
        rb->lcd_set_backdrop(NULL);
        rb->lcd_set_foreground(LCD_WHITE);
        rb->lcd_set_background(LCD_BLACK);
#endif
}

static int cell_bitmap_id(int r, int c)
{
        int val = board_get(r, c);
        int cur;
        int tag;

        cur = (r == cur_r && c == cur_c) ? 1 : 0;
        tag = (r == tag_r && c == tag_c) ? 2 : 0;

        switch(val) {
                case C_EMPTY:
                        return BMP_EMPTY + cur;
                case C_OBST:
                        return BMP_OBST + cur;
                case C_BLACK:
                        return tag ? BMP_BLACK + tag : BMP_BLACK + cur;
                case C_WHITE:
                        return tag ? BMP_WHITE + tag : BMP_WHITE + cur;
                default:
                        return BMP_EMPTY;
        }
}

#define CNT_BUF_SIZE 5

void draw_counters(void)
{
        int twb, thb;
        int tww, thw;
        char bufb[CNT_BUF_SIZE];
        char bufw[CNT_BUF_SIZE];
        int xw, yw, xb, yb;

        rb->snprintf(bufw, sizeof(bufw), "%d", board_count(C_WHITE));
        rb->lcd_getstringsize(bufw, &tww, &thw);
        rb->snprintf(bufb, sizeof(bufb), "%d", board_count(C_BLACK));
        rb->lcd_getstringsize(bufb, &twb, &thb);

        if(yofs > thb + TILE_H/2) {
                xw = xofs;              yw = yofs - 1 - thw - TILE_H/2;
                xb = xend - twb;        yb = yofs - 1 - thb - TILE_H/2;
        } else if(xofs > tww + TILE_W/2) {
                xw = xofs - 1 - TILE_W/2 - tww;         yw = yofs + TILE_H;
                xb = xend + 1 + TILE_W/2;               yb = yofs + TILE_H;
        } else {
                return;
        }

        rb->lcd_putsxy(xw, yw, bufw);
        rb->lcd_putsxy(xb, yb, bufb);

        if(cur_player == C_BLACK)
                rb->lcd_hline(xb, xb + twb, yb + thb);
        else if(cur_player == C_WHITE)
                rb->lcd_hline(xw, xw + tww, yw + thw);
}

void draw_whole_board(void)
{
        int r, c;

        rb->lcd_clear_display();
        rb->lcd_set_drawmode(DRMODE_SOLID | DRMODE_INVERSEVID);
        rb->lcd_fillrect(0, 0, LCD_WIDTH, LCD_HEIGHT);
        rb->lcd_set_drawmode(DRMODE_SOLID);
        rb->lcd_drawrect(xofs-1, yofs-1, board_w*TILE_W+2, board_h*TILE_H+2);
        foreach_cell(r, c)
                rb->lcd_bitmap_part(rattax_cells,
                                0, TILE_H*cell_bitmap_id(r, c),
                                TILE_W,
                                xofs + c*TILE_W,
                                yofs + r*TILE_H,
                                TILE_W,
                                TILE_H
                );

        draw_counters();
        rb->lcd_update();
}
