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

#include "bsets.h"

#define SET(a) { a, sizeof(a)/sizeof(*a) }
#define END { (void*)0, 0 }

static int b1[] = {
        7, 7,

        3, 0,
        3, 1,
        3, 2,
        0, 3,
        1, 3,
        2, 3,
        4, 3,
        5, 3,
        6, 3,
        3, 4,
        3, 5,
        3, 6
};

static int b10[] = {
        7, 7,

        2, 0,
        4, 0,
        2, 2,
        4, 2,
        1, 3,
        5, 3,
        2, 4,
        4, 4,
        2, 6,
        4, 6
};

static int b11[] = {
        7, 7,

        3, 1,
        3, 2,
        1, 3,
        2, 3,
        3, 3,
        4, 3,
        5, 3,
        3, 4,
        3, 5
};

static int b12[] = {
        7, 7,

        1, 1,
        2, 1,
        3, 1,
        4, 1,
        5, 1,
        1, 2,
        5, 2,
        1, 3,
        5, 3,
        1, 4,
        5, 4,
        1, 5,
        2, 5,
        3, 5,
        4, 5,
        5, 5
};

static int b13[] = {
        7, 7,

        2, 1,
        3, 1,
        4, 1,
        1, 2,
        5, 2,
        1, 3,
        5, 3,
        1, 4,
        5, 4,
        2, 5,
        3, 5,
        4, 5
};

static int b14[] = {
        7, 7,

        2, 1,
        4, 1,
        1, 2,
        2, 2,
        4, 2,
        5, 2,
        1, 4,
        2, 4,
        4, 4,
        5, 4,
        2, 5,
        4, 5
};

static int b15[] = {
        7, 7,

        1, 1,
        5, 5,
        1, 5,
        5, 1
};

static int b16[] = {
        7, 7,

        1, 1,
        2, 1,
        4, 1,
        5, 1,
        1, 2,
        5, 2,
        1, 4,
        5, 4,
        1, 5,
        2, 5,
        4, 5,
        5, 5
};

static int b17[] = {
        7, 7,

        3, 2,
        2, 3,
        3, 3,
        4, 3,
        3, 4
};

static int b18[] = {
        7, 7,

        3, 2,
        2, 3,
        4, 3,
        3, 4
};

static int b19[] = {
        7, 7,

        3, 1,
        3, 2,
        1, 3,
        2, 3,
        4, 3,
        5, 3,
        3, 4,
        3, 5
};

static int b2[] = {
        7, 7,

        3, 0,
        1, 2,
        2, 2,
        4, 2,
        5, 2,
        0, 3,
        1, 3,
        5, 3,
        6, 3,
        1, 4,
        2, 4,
        4, 4,
        5, 4,
        3, 6
};

static int b20[] = {
        7, 7,

        2, 0,
        3, 0,
        4, 0,
        3, 1,
        0, 2,
        6, 2,
        0, 3,
        1, 3,
        5, 3,
        6, 3,
        0, 4,
        6, 4,
        3, 5,
        2, 6,
        3, 6,
        4, 6
};

static int b21[] = {
        7, 7,

        2, 0,
        4, 0,
        3, 1,
        0, 2,
        6, 2,
        1, 3,
        5, 3,
        0, 4,
        6, 4,
        3, 5,
        2, 6,
        4, 6
};

static int b22[] = {
        7, 7,

        3, 1,
        1, 3,
        3, 3,
        5, 3,
        3, 5
};

static int b23[] = {
        7, 7,

        1, 1,
        3, 1,
        5, 1,
        1, 3,
        3, 3,
        5, 3,
        1, 5,
        3, 5,
        5, 5
};

static int b24[] = {
        7, 7,

};

static int b25[] = {
        7, 7,

        2, 2,
        4, 2,
        3, 3,
        2, 4,
        4, 4
};

static int b26[] = {
        7, 7,

        1, 1,
        5, 1,
        2, 2,
        4, 2,
        3, 3,
        2, 4,
        4, 4,
        1, 5,
        5, 5
};

static int b27[] = {
        7, 7,

        2, 0,
        3, 0,
        4, 0,
        2, 1,
        4, 1,
        0, 3,
        6, 3,
        2, 5,
        4, 5,
        2, 6,
        3, 6,
        4, 6
};

static int b28[] = {
        7, 7,

        1, 0,
        3, 0,
        5, 0,
        0, 1,
        2, 1,
        4, 1,
        6, 1,
        1, 2,
        3, 2,
        5, 2,
        0, 3,
        2, 3,
        4, 3,
        6, 3,
        1, 4,
        3, 4,
        5, 4,
        0, 5,
        2, 5,
        4, 5,
        6, 5,
        1, 6,
        3, 6,
        5, 6
};

static int b29[] = {
        7, 7,

        1, 1,
        5, 1,
        2, 2,
        4, 2,
        2, 4,
        4, 4,
        1, 5,
        5, 5
};

static int b3[] = {
        7, 7,

        1, 0,
        5, 0,
        0, 1,
        6, 1,
        3, 3,
        0, 5,
        6, 5,
        1, 6,
        5, 6
};

static int b30[] = {
        7, 7,

        3, 0,
        2, 1,
        4, 1,
        0, 3,
        6, 3,
        2, 5,
        4, 5,
        3, 6
};

static int b31[] = {
        7, 7,

        3, 0,
        0, 3,
        6, 3,
        3, 6
};

static int b32[] = {
        7, 7,

        3, 1,
        1, 3,
        5, 3,
        3, 5
};

static int b33[] = {
        7, 7,

        2, 0,
        3, 0,
        4, 0,
        0, 2,
        1, 2,
        3, 2,
        5, 2,
        6, 2,
        0, 3,
        6, 3,
        0, 4,
        1, 4,
        3, 4,
        5, 4,
        6, 4,
        2, 6,
        3, 6,
        4, 6
};

static int b34[] = {
        7, 7,

        2, 1,
        4, 1,
        0, 2,
        1, 2,
        3, 2,
        5, 2,
        6, 2,
        3, 3,
        0, 4,
        1, 4,
        3, 4,
        5, 4,
        6, 4,
        2, 5,
        4, 5
};

static int b35[] = {
        7, 7,

        2, 0,
        4, 0,
        2, 1,
        4, 1,
        0, 2,
        6, 2,
        0, 3,
        3, 3,
        6, 3,
        0, 4,
        6, 4,
        2, 5,
        4, 5,
        2, 6,
        4, 6
};

static int b4[] = {
        7, 7,

        2, 0,
        4, 0,
        2, 1,
        4, 1,
        0, 3,
        6, 3,
        2, 5,
        4, 5,
        2, 6,
        4, 6
};

static int b5[] = {
        7, 7,

        3, 0,
        3, 1,
        0, 3,
        1, 3,
        5, 3,
        6, 3,
        3, 5,
        3, 6
};

static int b6[] = {
        7, 7,

        3, 1,
        2, 2,
        4, 2,
        1, 3,
        5, 3,
        2, 4,
        4, 4,
        3, 5
};

static int b7[] = {
        7, 7,

        2, 0,
        4, 0,
        0, 2,
        6, 2,
        0, 4,
        6, 4,
        2, 6,
        4, 6
};

static int b8[] = {
        7, 7,

        3, 0,
        2, 1,
        4, 1,
        1, 2,
        5, 2,
        0, 3,
        6, 3,
        1, 4,
        5, 4,
        2, 5,
        4, 5,
        3, 6
};

static int b9[] = {
        7, 7,

        2, 1,
        4, 1,
        1, 2,
        5, 2,
        1, 4,
        5, 4,
        2, 5,
        4, 5
};

bset_t bsets[] = {
        SET(b1),
        SET(b10),
        SET(b11),
        SET(b12),
        SET(b13),
        SET(b14),
        SET(b15),
        SET(b16),
        SET(b17),
        SET(b18),
        SET(b19),
        SET(b2),
        SET(b20),
        SET(b21),
        SET(b22),
        SET(b23),
        SET(b24),
        SET(b25),
        SET(b26),
        SET(b27),
        SET(b28),
        SET(b29),
        SET(b3),
        SET(b30),
        SET(b31),
        SET(b32),
        SET(b33),
        SET(b34),
        SET(b35),
        SET(b4),
        SET(b5),
        SET(b6),
        SET(b7),
        SET(b8),
        SET(b9),
        END
};

int bset_count = sizeof(bsets) / sizeof(*bsets) - 1;
