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

//#include <sprintf.h>
#include "plugin.h"
#include "lib/pluginlib_actions.h"
//#include "../lib/oldmenuapi.h"

#include "rattax.h"
#include "board.h"
#include "cplayer.h"
#include "draw.h"
#include "util.h"
#include "state.h"

/*PLUGIN_HEADER
*
*const struct plugin_api* rb;
*
*const struct button_mapping* plugin_contexts[]={
*    generic_directions,
*    generic_actions
*};*/
//const int plugin_contexts_count = alength(plugin_contexts);
 const struct button_mapping *plugin_contexts[] = { pla_main_ctx };

int done = 0;

void cleanup(void* p)
{
        (void)p;
}

static void reset_cursor(void)
{
        board_place_cursor_for(other(cplayer_color));
}

static bool game_menu_cplayer_level(void)
{
        static const struct opt_items items[] = {
                { "Completely random", -1 },
                { "Greedy", -1 },
                { "1 turn", -1 },
                { "1.5 turns", -1 },
                { "2 turns", -1 },
                { "2.5 turns", -1 },
                { "3 turns", -1 },
                { "3.5 turns", -1 },
                { "4 turns", -1 }
        };

        return rb->set_option("Bot strategy", &cplayer_level, INT,
                        items, alength(items), NULL);
}

static bool game_menu_cplayer_color(void)
{
        static const struct opt_items items[] = {
                { "Black", -1 },
                { "White", -1 }
        };

        int value = cplayer_color == C_BLACK ? 0 : 1;

        bool ret = rb->set_option("Bot color", &value, INT,
                        items, alength(items), NULL);

        cplayer_color = value ? C_WHITE : C_BLACK;
        return ret;
}

/* Returns true iff USB ws connected while in the menu */
static bool game_menu(void)
{
        int selection=0;
		int menu_quit = 0;

        MENUITEM_STRINGLIST(menu, "Rattax", NULL, "Start new game", "Restart game", "Pass the move", "Bot level", "Bot color", "Back", "Quit");

        while(!menu_quit)
{
        switch (rb->do_menu(&menu, &selection, NULL, false)) {
                case 0:
                        board_rand();
                        draw_board_init();
                        reset_cursor();
                        menu_quit = 1;
						break;
                case 1:
                        board_clean();
                        reset_cursor();
                        menu_quit = 1;
						break;
                case 2:
                        cur_player = other(cur_player);
                        menu_quit = 1;
						break;
                case 3:
                        game_menu_cplayer_level();
                        break;
                case 4:
                        game_menu_cplayer_color();
                        break;
                case 5:
                        menu_quit = 1;
                        break;
				case 6:
                        menu_quit = 1;
						return (done = 1);
                        break;
				case MENU_ATTACHED_USB:
            return MENU_ATTACHED_USB;
            break;
    //    case GO_TO_PREVIOUS:
    //        return RET_VAL_OK;
    //        break;
        }
    }
 //   return RET_VAL_OK;
 return PLUGIN_OK;
}

enum plugin_status plugin_start(const void* parameter)
{
        int button;
        char message[50];
        (void)parameter;
		bool done = false;

        //rb = api;
        rb->srand(*rb->current_tick);
        if(board_load())
                board_rand();
        else
                cur_player = other(cplayer_color);
        draw_board_init();
        reset_cursor();

        while(!done) {
                draw_whole_board();
                if(cur_player != P_GEND && board_game_end()) {
                        int cw = board_count(C_WHITE);
                        int cb = board_count(C_BLACK);
                        if(cw == cb)
                                rb->snprintf(message, sizeof(message),
                                                "%d:%d, draw", cw, cb
                                );
                        else
                                rb->snprintf(message, sizeof(message),
                                                "%d:%d %s won", cw, cb,
                                                (cw > cb ? "white" : "black")
                                );
                        rb->splash(2*HZ, message);
                        cur_player = P_GEND;
                        continue;
                }
                if(cur_player != P_GEND && !has_move(cur_player)) {
                        cur_player = other(cur_player);
                        continue;
                }
                if(cur_player != P_GEND && cur_player == cplayer_color) {
                        cplayer_make_move();
                        continue;
                }
               // button = pluginlib_getaction(rb, TIMEOUT_BLOCK,
               //                 plugin_contexts, plugin_contexts_count);
			    button=pluginlib_getaction(-1, plugin_contexts, ARRAYLEN(plugin_contexts));
                switch(button) {
                        case PLA_CANCEL:
                                /* a bit illogical, but PLA_MENU shares
                                 * a single button with PLA_DOWN on some
                                 * targets (??!) */
                                if(game_menu())
                                        return PLUGIN_USB_CONNECTED;
                                break;
                        case PLA_LEFT:
                                board_move_cursor( 0, -1); break;
                        case PLA_RIGHT:
                                board_move_cursor( 0,  1); break;
                        case PLA_UP:
                                board_move_cursor(-1,  0); break;
                        case PLA_DOWN:
                                board_move_cursor( 1,  0); break;
                        case PLA_SELECT:
                                board_act(); break;
                        default:
                                /* Don't know whether it's correct for PLA_*,
                                 * but some other plugins do exactly this */
                                if(rb->default_event_handler_ex(button, cleanup, NULL) == SYS_USB_CONNECTED)
                                        return PLUGIN_USB_CONNECTED;
                }
        }
        if(!cur_player != P_GEND)
                board_save();
        else
                board_remove_file();

        return PLUGIN_OK;
}
