 /***************************************************************************
 *             __________               __   ___.
 *   Open      \______   \ ____   ____ |  | _\_ |__   _______  ___
 *   Source     |       _//  _ \_/ ___\|  |/ /| __ \ /  _ \  \/  /
 *   Jukebox    |    |   (  <_> )  \___|    < | \_\ (  <_> > <  <
 *   Firmware   |____|_  /\____/ \___  >__|_ \|___  /\____/__/\_ \
 *                     \/            \/     \/    \/            \/
 * $Id$
 *
 * Port of xrick, a Rick Dangerous clone, to Rockbox.
 * See http://www.bigorno.net/xrick/
 *
 * Copyright (C) 2008-2014 Pierluigi Vicinanza
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

#include "xrick/system/system.h"
#include "xrick/game.h"

#include "plugin.h"
static const unsigned int sentinel = 0xBA5EFAC7;
 
#define SENTINEL(n) (sentinel ^ (n))

static size_t check_sentinel(void* buf, size_t size)
 
{
 
    const size_t sz = size / sizeof(sentinel);
 
    size_t unused = 0;
 
    size_t i;
 
    unsigned int *b = (int*) buf;
 
    for(i = 0; i < sz; i++)
 
        if (b[i] == SENTINEL(i))
 
        {
 
            unused++;
 
            while(++i < sz && b[i] == SENTINEL(i) && ++unused)
 
                ;;
 
        }
 
        DEBUGF("Unused: %ld\n", unused*sizeof(sentinel));
        return 1;
 
}


static void set_sentinel(void* buf, size_t size)
 
{
 
    size_t i;
 
    unsigned int *b = (int*) buf;
 
    for(i = 0; i < size / sizeof(sentinel); i++)
 
        *b++ = SENTINEL(i);
 
}

/* Plug-in entry point */
enum plugin_status plugin_start(const void* parameter)
{
    size_t size, size2;
    char *buf = rb->plugin_get_buffer(&size);
    char *buf2 = rb->plugin_get_audio_buffer(&size2);
    DEBUGF("Size: %ld\nSize2: %ld\n", size, size2);
    set_sentinel(buf, size);
    set_sentinel(buf2, size2);
    char *filename = (char*)parameter;
    bool success = sys_init(1, &filename);
    if (success)
    {
        game_run();
    }
    check_sentinel(buf, size);
    check_sentinel(buf2, size2);
    sys_shutdown();
    return (success? PLUGIN_OK : PLUGIN_ERROR);
}

/* eof */
