/***************************************************************************
 *             __________               __   ___.
 *   Open      \______   \ ____   ____ |  | _\_ |__   _______  ___
 *   Source     |       _//  _ \_/ ___\|  |/ /| __ \ /  _ \  \/  /
 *   Jukebox    |    |   (  <_> )  \___|    < | \_\ (  <_> > <  <
 *   Firmware   |____|_  /\____/ \___  >__|_ \|___  /\____/__/\_ \
 *                     \/            \/     \/    \/            \/
 * $Id: freecell.c ??? $
 *
 * Original solitaire
 * Copyright (C) 2004-2007 Antoine Cellerier <dionoea @t videolan d.t org>
 * 
 * Frecell adaptation
 * Copyright (C) 2011 Jason Stark <rockbox @t jasonstark d.t com>
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

#include "plugin.h"
#include "lib/playback_control.h"
#include "lib/configfile.h"
#include "lib/display_text.h"
#include "button.h"
#include "lcd.h"

#define min(a,b) (a<b?a:b)

/**
 * Key definitions *** NOTE: Only Sansa e200 keys are configured.
 */

#if CONFIG_KEYPAD == RECORDER_PAD
#   define FC_QUIT         BUTTON_OFF
#   define FC_UP           BUTTON_UP
#   define FC_DOWN         BUTTON_DOWN
#   define FC_LEFT         BUTTON_LEFT
#   define FC_RIGHT        BUTTON_RIGHT
#   define FC_MOVE         BUTTON_ON
#   define FC_CUR2FC         BUTTON_F1
#   define FC_REM2CUR      BUTTON_PLAY
#   define FC_CUR2STACK    BUTTON_F2
#   define FC_REM2STACK    BUTTON_F3
#   define HK_MOVE         "ON"
#   define HK_DRAW         "F1"
#   define HK_REM2CUR      "PLAY"
#   define HK_CUR2STACK    "F2"
#   define HK_REM2STACK    "F3"

#elif CONFIG_KEYPAD == ARCHOS_AV300_PAD
#   define FC_QUIT         BUTTON_OFF
#   define FC_UP           BUTTON_UP
#   define FC_DOWN         BUTTON_DOWN
#   define FC_LEFT         BUTTON_LEFT
#   define FC_RIGHT        BUTTON_RIGHT
#   define FC_MOVE         BUTTON_ON
#   define FC_CUR2FC         BUTTON_F1
#   define FC_REM2CUR      BUTTON_SELECT
#   define FC_CUR2STACK    BUTTON_F2
#   define FC_REM2STACK    BUTTON_F3
#   define HK_MOVE         "ON"
#   define HK_DRAW         "F1"
#   define HK_REM2CUR      "SELECT"
#   define HK_CUR2STACK    "F2"
#   define HK_REM2STACK    "F3"

#elif CONFIG_KEYPAD == ONDIO_PAD
#   define FC_QUIT         BUTTON_OFF
#   define FC_UP_PRE       BUTTON_UP
#   define FC_UP           (BUTTON_UP | BUTTON_REL)
#   define FC_DOWN_PRE     BUTTON_DOWN
#   define FC_DOWN         (BUTTON_DOWN | BUTTON_REL)
#   define FC_LEFT_PRE     BUTTON_LEFT
#   define FC_LEFT         (BUTTON_LEFT | BUTTON_REL)
#   define FC_RIGHT_PRE    BUTTON_RIGHT
#   define FC_RIGHT        (BUTTON_RIGHT | BUTTON_REL)
#   define FC_MOVE_PRE     BUTTON_MENU
#   define FC_MOVE         (BUTTON_MENU | BUTTON_REL)
#   define FC_CUR2FC_PRE     BUTTON_MENU
#   define FC_CUR2FC         (BUTTON_MENU | BUTTON_REPEAT)
#   define FC_REM2CUR_PRE  BUTTON_DOWN
#   define FC_REM2CUR      (BUTTON_DOWN | BUTTON_REPEAT)
#   define FC_CUR2STACK_PRE BUTTON_UP
#   define FC_CUR2STACK    (BUTTON_UP | BUTTON_REPEAT)
#   define FC_REM2STACK_PRE BUTTON_RIGHT
#   define FC_REM2STACK    (BUTTON_RIGHT | BUTTON_REPEAT)
#   define HK_MOVE         "MODE"
#   define HK_DRAW         "MODE.."
#   define HK_REM2CUR      "DOWN.."
#   define HK_CUR2STACK    "UP.."
#   define HK_REM2STACK    "RIGHT.."

#elif (CONFIG_KEYPAD == IRIVER_H100_PAD) || \
      (CONFIG_KEYPAD == IRIVER_H300_PAD)
#   define FC_QUIT         BUTTON_OFF
#   define FC_UP           BUTTON_UP
#   define FC_DOWN         BUTTON_DOWN
#   define FC_LEFT         BUTTON_LEFT
#   define FC_RIGHT        BUTTON_RIGHT
#   define FC_MOVE_PRE     BUTTON_SELECT
#   define FC_MOVE         (BUTTON_SELECT | BUTTON_REL)
#   define FC_CUR2FC         BUTTON_MODE
#   define FC_REM2CUR      (BUTTON_LEFT | BUTTON_ON)
#   define FC_CUR2STACK_PRE BUTTON_SELECT
#   define FC_CUR2STACK    (BUTTON_SELECT | BUTTON_REPEAT)
#   define FC_REM2STACK    (BUTTON_RIGHT | BUTTON_ON)
#   define FC_REM          BUTTON_REC
#   define FC_RC_QUIT      BUTTON_RC_STOP
#   define HK_MOVE         "NAVI"
#   define HK_DRAW         "A-B"
#   define HK_REM2CUR      "PLAY+LEFT"
#   define HK_CUR2STACK    "NAVI.."
#   define HK_REM2STACK    "PLAY+RIGHT"

#elif (CONFIG_KEYPAD == IPOD_4G_PAD) || (CONFIG_KEYPAD == IPOD_3G_PAD) || \
      (CONFIG_KEYPAD == IPOD_1G2G_PAD)
#   define FC_QUIT         (BUTTON_SELECT | BUTTON_MENU)
#   define FC_UP           BUTTON_SCROLL_BACK
#   define FC_DOWN         BUTTON_SCROLL_FWD
#   define FC_LEFT_PRE     BUTTON_LEFT
#   define FC_LEFT         (BUTTON_LEFT | BUTTON_REL)
#   define FC_RIGHT_PRE    BUTTON_RIGHT
#   define FC_RIGHT        (BUTTON_RIGHT | BUTTON_REL)
#   define FC_MOVE_PRE     BUTTON_SELECT
#   define FC_MOVE         (BUTTON_SELECT | BUTTON_REL)
#   define FC_CUR2FC_PRE     BUTTON_MENU
#   define FC_CUR2FC         (BUTTON_MENU | BUTTON_REL)
#   define FC_REM2CUR      BUTTON_PLAY
#   define FC_CUR2STACK_PRE BUTTON_MENU
#   define FC_CUR2STACK    (BUTTON_MENU | BUTTON_REPEAT)
#   define FC_REM2STACK_PRE BUTTON_RIGHT
#   define FC_REM2STACK    (BUTTON_RIGHT | BUTTON_REPEAT)
#   define HK_UD           "SCROLL U/D"
#   define HK_MOVE         "SELECT"
#   define HK_DRAW         "MENU"
#   define HK_REM2CUR      "PLAY"
#   define HK_CUR2STACK    "MENU.."
#   define HK_REM2STACK    "RIGHT.."

#elif (CONFIG_KEYPAD == IAUDIO_X5M5_PAD)
#   define FC_QUIT          BUTTON_POWER
#   define FC_UP            BUTTON_UP
#   define FC_DOWN          BUTTON_DOWN
#   define FC_LEFT          BUTTON_LEFT
#   define FC_RIGHT         BUTTON_RIGHT
#   define FC_MOVE_PRE      BUTTON_SELECT
#   define FC_MOVE          (BUTTON_SELECT | BUTTON_REL)
#   define FC_CUR2FC_PRE      BUTTON_PLAY
#   define FC_CUR2FC          (BUTTON_PLAY | BUTTON_REL)
#   define FC_REM2CUR_PRE   BUTTON_PLAY
#   define FC_REM2CUR       (BUTTON_PLAY | BUTTON_REPEAT)
#   define FC_CUR2STACK_PRE BUTTON_SELECT
#   define FC_CUR2STACK     (BUTTON_SELECT | BUTTON_REPEAT)
#   define FC_REM2STACK     BUTTON_REC
#   define HK_MOVE           "SELECT"
#   define HK_DRAW           "PLAY"
#   define HK_REM2CUR        "PLAY.."
#   define HK_CUR2STACK      "SELECT.."
#   define HK_REM2STACK      "REC"

#elif (CONFIG_KEYPAD == GIGABEAT_PAD)
#   define FC_QUIT         BUTTON_POWER
#   define FC_UP           BUTTON_UP
#   define FC_DOWN         BUTTON_DOWN
#   define FC_LEFT         BUTTON_LEFT
#   define FC_RIGHT        BUTTON_RIGHT
#   define FC_MOVE_PRE     BUTTON_SELECT
#   define FC_MOVE         (BUTTON_SELECT | BUTTON_REL)
#   define FC_CUR2FC         BUTTON_MENU
#   define FC_REM2CUR      (BUTTON_LEFT | BUTTON_A)
#   define FC_CUR2STACK    (BUTTON_SELECT | BUTTON_REPEAT)
#   define FC_REM2STACK    (BUTTON_RIGHT | BUTTON_A)
#   define HK_MOVE         "SELECT"
#   define HK_DRAW         "MENU"
#   define HK_REM2CUR      "A+LEFT"
#   define HK_CUR2STACK    "SELECT.."
#   define HK_REM2STACK    "A+RIGHT"

/* JStark TODO the rest of the key configurations... */
#elif (CONFIG_KEYPAD == SANSA_E200_PAD)
#   define FC_QUIT          BUTTON_POWER
#   define FC_UP            BUTTON_UP
#   define FC_DOWN          BUTTON_DOWN
#   define FC_LEFT          BUTTON_SCROLL_BACK
#   define FC_RIGHT         BUTTON_SCROLL_FWD
#   define FC_MOVE          BUTTON_SELECT
#   define FC_CUR2FC        BUTTON_LEFT
#   define FC_CUR2STACK     BUTTON_RIGHT
#   define HK_MOVE           "SELECT"
#   define HK_CUR2FC         "LEFT"
#   define HK_CUR2STACK      "RIGHT"

#elif (CONFIG_KEYPAD == SANSA_FUZE_PAD)
#   define FC_QUIT          (BUTTON_HOME|BUTTON_REPEAT)
#   define FC_UP            BUTTON_UP
#   define FC_DOWN          BUTTON_DOWN
#   define FC_LEFT          BUTTON_SCROLL_BACK
#   define FC_RIGHT         BUTTON_SCROLL_FWD
#   define FC_MOVE          (BUTTON_SELECT|BUTTON_REL)
#   define FC_CUR2FC          (BUTTON_HOME|BUTTON_REL)
#   define FC_REM2CUR       BUTTON_LEFT
#   define FC_CUR2STACK_PRE (BUTTON_RIGHT | BUTTON_REPEAT)
#   define FC_CUR2STACK     BUTTON_RIGHT
#   define FC_REM2STACK     (BUTTON_LEFT|BUTTON_REPEAT)
#   define FC_REM2STACK_PRE BUTTON_LEFT
#   define HK_MOVE           "SHORT SELECT"
#   define HK_DRAW           "SHORT HOME"
#   define HK_REM2CUR        "LEFT"
#   define HK_CUR2STACK      "DOUBLE SELECT"
#   define HK_REM2STACK      "LEFT"

#elif (CONFIG_KEYPAD == SANSA_C200_PAD)
#   define FC_QUIT          BUTTON_POWER
#   define FC_UP            BUTTON_UP
#   define FC_DOWN          BUTTON_DOWN
#   define FC_LEFT          BUTTON_LEFT
#   define FC_RIGHT         BUTTON_RIGHT
#   define FC_MOVE_PRE      BUTTON_SELECT
#   define FC_MOVE          (BUTTON_SELECT | BUTTON_REL)
#   define FC_CUR2FC          BUTTON_VOL_DOWN
#   define FC_REM2CUR       BUTTON_REC
#   define FC_CUR2STACK_PRE BUTTON_SELECT
#   define FC_CUR2STACK     (BUTTON_SELECT | BUTTON_REPEAT)
#   define FC_REM2STACK     BUTTON_VOL_UP
#   define HK_MOVE           "SELECT"
#   define HK_DRAW           "REC"
#   define HK_REM2CUR        "LEFT"
#   define HK_CUR2STACK      "DOUBLE SELECT"
#   define HK_REM2STACK      "RIGHT"

#elif CONFIG_KEYPAD == SANSA_CLIP_PAD
#   define FC_QUIT          BUTTON_POWER
#   define FC_UP            BUTTON_UP
#   define FC_DOWN          BUTTON_DOWN
#   define FC_LEFT          BUTTON_LEFT
#   define FC_RIGHT         BUTTON_RIGHT
#   define FC_MOVE_PRE      BUTTON_SELECT
#   define FC_MOVE          (BUTTON_SELECT | BUTTON_REL)
#   define FC_CUR2FC          BUTTON_HOME
#   define FC_REM2CUR       BUTTON_VOL_DOWN
#   define FC_CUR2STACK_PRE BUTTON_SELECT
#   define FC_CUR2STACK     (BUTTON_SELECT | BUTTON_REPEAT)
#   define FC_REM2STACK     BUTTON_VOL_UP
#   define HK_MOVE           "SELECT"
#   define HK_DRAW           "HOME"
#   define HK_REM2CUR        "LEFT"
#   define HK_CUR2STACK      "DOUBLE SELECT"
#   define HK_REM2STACK      "RIGHT"

#elif CONFIG_KEYPAD == SANSA_M200_PAD
#   define FC_QUIT          BUTTON_POWER
#   define FC_UP            BUTTON_UP
#   define FC_DOWN          BUTTON_DOWN
#   define FC_LEFT          BUTTON_LEFT
#   define FC_RIGHT         BUTTON_RIGHT
#   define FC_MOVE_PRE      BUTTON_SELECT
#   define FC_MOVE          (BUTTON_SELECT | BUTTON_REL)
#   define FC_CUR2FC          (BUTTON_SELECT | BUTTON_UP)
#   define FC_REM2CUR       BUTTON_VOL_DOWN
#   define FC_CUR2STACK_PRE BUTTON_SELECT
#   define FC_CUR2STACK     (BUTTON_SELECT | BUTTON_REPEAT)
#   define FC_REM2STACK     BUTTON_VOL_UP
#   define HK_MOVE           "SELECT"
#   define HK_DRAW           "SELECT + UP"
#   define HK_REM2CUR        "LEFT"
#   define HK_CUR2STACK      "DOUBLE SELECT"
#   define HK_REM2STACK      "RIGHT"

#elif (CONFIG_KEYPAD == IRIVER_H10_PAD)
#   define FC_QUIT          BUTTON_POWER
#   define FC_UP            BUTTON_SCROLL_UP
#   define FC_DOWN          BUTTON_SCROLL_DOWN
#   define FC_LEFT_PRE      BUTTON_LEFT
#   define FC_LEFT          (BUTTON_LEFT | BUTTON_REL)
#   define FC_RIGHT_PRE     BUTTON_RIGHT
#   define FC_RIGHT         (BUTTON_RIGHT | BUTTON_REL)
#   define FC_MOVE          BUTTON_PLAY
#   define FC_CUR2FC_PRE      BUTTON_LEFT
#   define FC_CUR2FC          (BUTTON_LEFT | BUTTON_REPEAT)
#   define FC_REM2CUR       BUTTON_FF
#   define FC_CUR2STACK     BUTTON_REW
#   define FC_REM2STACK_PRE BUTTON_RIGHT
#   define FC_REM2STACK     (BUTTON_RIGHT | BUTTON_REPEAT)
#   define HK_MOVE           "PLAY"
#   define HK_DRAW           "LEFT.."
#   define HK_REM2CUR        "FF"
#   define HK_CUR2STACK      "REW"
#   define HK_REM2STACK      "RIGHT.."

#elif (CONFIG_KEYPAD == GIGABEAT_S_PAD)
#   define FC_QUIT         BUTTON_BACK
#   define FC_UP           BUTTON_UP
#   define FC_DOWN         BUTTON_DOWN
#   define FC_LEFT         BUTTON_LEFT
#   define FC_RIGHT        BUTTON_RIGHT
#   define FC_MOVE_PRE     BUTTON_SELECT
#   define FC_MOVE         (BUTTON_SELECT | BUTTON_REL)
#   define FC_CUR2FC         BUTTON_MENU
#   define FC_REM2CUR      (BUTTON_LEFT | BUTTON_SELECT)
#   define FC_CUR2STACK    (BUTTON_SELECT | BUTTON_REPEAT)
#   define FC_REM2STACK    (BUTTON_RIGHT | BUTTON_SELECT)
#   define HK_MOVE         "SELECT"
#   define HK_DRAW         "MENU"
#   define HK_REM2CUR      "SELECT+LEFT"
#   define HK_CUR2STACK    "SELECT.."
#   define HK_REM2STACK    "SELECT+RIGHT"

#elif (CONFIG_KEYPAD == MROBE100_PAD)
#   define FC_QUIT         BUTTON_POWER
#   define FC_UP           BUTTON_UP
#   define FC_DOWN         BUTTON_DOWN
#   define FC_LEFT         BUTTON_LEFT
#   define FC_RIGHT        BUTTON_RIGHT
#   define FC_MOVE_PRE     BUTTON_SELECT
#   define FC_MOVE         (BUTTON_SELECT | BUTTON_REL)
#   define FC_CUR2FC         BUTTON_MENU
#   define FC_REM2CUR      (BUTTON_LEFT | BUTTON_DISPLAY)
#   define FC_CUR2STACK    (BUTTON_SELECT | BUTTON_REPEAT)
#   define FC_REM2STACK    (BUTTON_RIGHT | BUTTON_DISPLAY)
#   define HK_MOVE         "SELECT"
#   define HK_DRAW         "MENU"
#   define HK_REM2CUR      "DISPLAY+LEFT"
#   define HK_CUR2STACK    "SELECT.."
#   define HK_REM2STACK    "DISPLAY+RIGHT"

#elif CONFIG_KEYPAD == IAUDIO_M3_PAD
#   define FC_QUIT         BUTTON_RC_REC
#   define FC_UP           BUTTON_RC_VOL_UP
#   define FC_DOWN         BUTTON_RC_VOL_DOWN
#   define FC_LEFT         BUTTON_RC_REW
#   define FC_RIGHT        BUTTON_RC_FF
#   define FC_MOVE         BUTTON_RC_PLAY
#   define FC_CUR2FC_PRE     BUTTON_RC_MENU
#   define FC_CUR2FC         (BUTTON_RC_MENU | BUTTON_REL)
#   define FC_REM2CUR_PRE  BUTTON_RC_MENU
#   define FC_REM2CUR      (BUTTON_RC_MENU | BUTTON_REPEAT)
#   define FC_CUR2STACK_PRE BUTTON_RC_MODE
#   define FC_CUR2STACK    (BUTTON_RC_MODE | BUTTON_REL)
#   define FC_REM2STACK_PRE BUTTON_RC_MODE
#   define FC_REM2STACK    (BUTTON_RC_MODE | BUTTON_REPEAT)
#   define HK_LR           "REW/FF"
#   define HK_UD           "VOL UP/DOWN"
#   define HK_MOVE         "PLAY"
#   define HK_DRAW         "MENU"
#   define HK_REM2CUR      "MENU.."
#   define HK_CUR2STACK    "MODE"
#   define HK_REM2STACK    "MODE.."

#elif (CONFIG_KEYPAD == COWON_D2_PAD)
#   define FC_QUIT          BUTTON_POWER

#elif CONFIG_KEYPAD == CREATIVEZVM_PAD
#   define FC_QUIT         BUTTON_BACK
#   define FC_UP           BUTTON_UP
#   define FC_DOWN         BUTTON_DOWN
#   define FC_LEFT         BUTTON_LEFT
#   define FC_RIGHT        BUTTON_RIGHT
#   define FC_MOVE_PRE     BUTTON_SELECT
#   define FC_MOVE         (BUTTON_SELECT | BUTTON_REL)
#   define FC_CUR2FC         BUTTON_MENU
#   define FC_REM2CUR      (BUTTON_LEFT | BUTTON_SELECT)
#   define FC_CUR2STACK    (BUTTON_SELECT | BUTTON_REPEAT)
#   define FC_REM2STACK    (BUTTON_RIGHT | BUTTON_SELECT)
#   define HK_MOVE         "MIDDLE"
#   define HK_DRAW         "MENU"
#   define HK_REM2CUR      "PLAY+LEFT"
#   define HK_CUR2STACK    "PLAY.."
#   define HK_REM2STACK    "PLAY+RIGHT"

#elif CONFIG_KEYPAD == PHILIPS_HDD1630_PAD
#   define FC_QUIT         BUTTON_POWER
#   define FC_UP           BUTTON_UP
#   define FC_DOWN         BUTTON_DOWN
#   define FC_LEFT         BUTTON_LEFT
#   define FC_RIGHT        BUTTON_RIGHT
#   define FC_MOVE_PRE     BUTTON_SELECT
#   define FC_MOVE         (BUTTON_SELECT | BUTTON_REL)
#   define FC_CUR2FC         BUTTON_MENU
#   define FC_REM2CUR      (BUTTON_LEFT | BUTTON_VIEW)
#   define FC_CUR2STACK    (BUTTON_SELECT | BUTTON_REPEAT)
#   define FC_REM2STACK    (BUTTON_RIGHT | BUTTON_VIEW)
#   define HK_MOVE          "SELECT"
#   define HK_DRAW          "MENU"
#   define HK_REM2CUR       "VIEW+LEFT"
#   define HK_CUR2STACK     "SELECT.."
#   define HK_REM2STACK     "VIEW+RIGHT"

#elif CONFIG_KEYPAD == PHILIPS_HDD6330_PAD
#   define FC_QUIT          BUTTON_POWER
#   define FC_UP            BUTTON_UP
#   define FC_DOWN          BUTTON_DOWN
#   define FC_LEFT          BUTTON_PREV
#   define FC_RIGHT         BUTTON_NEXT
#   define FC_MOVE_PRE      BUTTON_PLAY
#   define FC_MOVE          (BUTTON_PLAY | BUTTON_REL)
#   define FC_CUR2FC          BUTTON_MENU
#   define FC_REM2CUR       BUTTON_LEFT
#   define FC_CUR2STACK     (BUTTON_PLAY | BUTTON_REPEAT)
#   define FC_REM2STACK     BUTTON_RIGHT
#   define HK_MOVE           "PLAY"
#   define HK_DRAW           "MENU"
#   define HK_REM2CUR        "LEFT"
#   define HK_CUR2STACK      "PLAY.."
#   define HK_REM2STACK      "RIGHT"

#elif CONFIG_KEYPAD == PHILIPS_SA9200_PAD
#   define FC_QUIT          BUTTON_POWER
#   define FC_UP            BUTTON_UP
#   define FC_DOWN          BUTTON_DOWN
#   define FC_LEFT          BUTTON_PREV
#   define FC_RIGHT         BUTTON_NEXT
#   define FC_MOVE_PRE      BUTTON_PLAY
#   define FC_MOVE          (BUTTON_PLAY | BUTTON_REL)
#   define FC_CUR2FC          BUTTON_MENU
#   define FC_REM2CUR       BUTTON_LEFT
#   define FC_CUR2STACK     (BUTTON_PLAY | BUTTON_REPEAT)
#   define FC_REM2STACK     BUTTON_RIGHT
#   define HK_MOVE           "PLAY"
#   define HK_DRAW           "MENU"
#   define HK_REM2CUR        "LEFT"
#   define HK_CUR2STACK      "PLAY..."
#   define HK_REM2STACK      "RIGHT"

#elif (CONFIG_KEYPAD == ONDAVX747_PAD) || \
(CONFIG_KEYPAD == ONDAVX777_PAD) || \
CONFIG_KEYPAD == MROBE500_PAD
#   define FC_QUIT          BUTTON_POWER

#elif (CONFIG_KEYPAD == SAMSUNG_YH920_PAD)
#   define FC_QUIT          BUTTON_REC
#   define FC_UP            BUTTON_UP
#   define FC_DOWN          BUTTON_DOWN
#   define FC_LEFT_PRE      BUTTON_LEFT
#   define FC_LEFT          (BUTTON_LEFT | BUTTON_REW)
#   define FC_RIGHT_PRE     BUTTON_RIGHT
#   define FC_RIGHT         (BUTTON_RIGHT | BUTTON_REW)
#   define FC_MOVE          BUTTON_PLAY
#   define FC_CUR2FC_PRE      (BUTTON_PLAY | BUTTON_RIGHT)
#   define FC_CUR2FC          (BUTTON_PLAY | BUTTON_LEFT)
#   define FC_REM2CUR       BUTTON_FFWD
#   define FC_CUR2STACK     BUTTON_REW
#   define FC_REM2STACK_PRE (BUTTON_FFWD | BUTTON_RIGHT)
#   define FC_REM2STACK     (BUTTON_FFWD | BUTTON_LEFT)
#   define HK_MOVE           "PLAY"
#   define HK_DRAW           "REW.."
#   define HK_REM2CUR        "FFWD"
#   define HK_CUR2STACK      "REW"
#   define HK_REM2STACK      "FFWD.."

#elif CONFIG_KEYPAD == PBELL_VIBE500_PAD
#   define FC_QUIT          BUTTON_REC
#   define FC_UP            BUTTON_UP
#   define FC_DOWN          BUTTON_DOWN
#   define FC_LEFT          BUTTON_PREV
#   define FC_RIGHT         BUTTON_NEXT
#   define FC_MOVE_PRE      BUTTON_OK
#   define FC_MOVE          (BUTTON_OK | BUTTON_REL)
#   define FC_CUR2FC          BUTTON_MENU
#   define FC_REM2CUR       BUTTON_CANCEL
#   define FC_CUR2STACK     BUTTON_PLAY
#   define FC_REM2STACK     (BUTTON_PLAY | BUTTON_REPEAT)
#   define HK_MOVE           "OK"
#   define HK_DRAW           "MENU"
#   define HK_REM2CUR        "CANCEL"
#   define HK_CUR2STACK      "PLAY"
#   define HK_REM2STACK      "PLAY..."

#elif CONFIG_KEYPAD == MPIO_HD200_PAD
#   define FC_QUIT         (BUTTON_REC | BUTTON_PLAY)
#   define FC_UP           BUTTON_REW
#   define FC_DOWN         BUTTON_FF
#   define FC_LEFT         BUTTON_VOL_DOWN
#   define FC_RIGHT        BUTTON_VOL_UP
#   define FC_MOVE_PRE     BUTTON_FUNC
#   define FC_MOVE         (BUTTON_FUNC | BUTTON_REL)
#   define FC_CUR2FC         BUTTON_REC
#   define FC_REM2CUR      (BUTTON_REC | BUTTON_REPEAT)
#   define FC_CUR2STACK    BUTTON_PLAY
#   define FC_REM2STACK    (BUTTON_PLAY | BUTTON_REPEAT)
#   define HK_MOVE         "FUNC"
#   define HK_DRAW         "REC"
#   define HK_REM2CUR      "REC.."
#   define HK_CUR2STACK    "PLAY"
#   define HK_REM2STACK    "PLAY...."

#elif CONFIG_KEYPAD == SANSA_FUZEPLUS_PAD
#   define FC_QUIT         BUTTON_BACK
#   define FC_UP           BUTTON_UP
#   define FC_DOWN         BUTTON_DOWN
#   define FC_LEFT         BUTTON_LEFT
#   define FC_RIGHT        BUTTON_RIGHT
#   define FC_CUR2FC       BUTTON_PLAYPAUSE
#   define FC_MOVE         BUTTON_SELECT
#   define FC_REM2CUR      BUTTON_VOL_DOWN
#   define FC_CUR2STACK    BUTTON_BOTTOMRIGHT
#   define FC_REM2STACK    BUTTON_VOL_UP
#   define HK_MOVE           "SELECT"
#   define HK_DRAW           "PLAY"
#   define HK_REM2CUR        "LEFT"
#   define HK_CUR2STACK      "BOTTOM LEFT"
#   define HK_REM2STACK      "RIGHT"

#elif CONFIG_KEYPAD == MPIO_HD300_PAD
#   define FC_QUIT         BUTTON_REC
#   define FC_UP           BUTTON_UP
#   define FC_DOWN         BUTTON_DOWN
#   define FC_LEFT         BUTTON_MENU
#   define FC_RIGHT        BUTTON_ENTER
#   define FC_MOVE_PRE     BUTTON_REW
#   define FC_MOVE         BUTTON_FF
#   define FC_CUR2FC         (BUTTON_ENTER | BUTTON_PLAY)
#   define FC_REM2CUR      (BUTTON_MENU | BUTTON_REPEAT)
#   define FC_CUR2STACK    BUTTON_PLAY
#   define FC_REM2STACK    (BUTTON_PLAY | BUTTON_REPEAT)
#   define HK_MOVE         "FF"
#   define HK_DRAW         "ENTER + PLAY"
#   define HK_REM2CUR      "EMNU.."
#   define HK_CUR2STACK    "PLAY"
#   define HK_REM2STACK    "PLAY...."

#else
#error No keymap defined!
#endif

#ifdef HAVE_TOUCHSCREEN
//#ifndef FC_QUIT
//#   define FC_QUIT          BUTTON_TOPLEFT
//endif
#ifndef FC_UP
#   define FC_UP            BUTTON_TOPMIDDLE
#endif
#ifndef FC_DOWN
#   define FC_DOWN          BUTTON_BOTTOMMIDDLE
#endif
#ifndef FC_LEFT
#   define FC_LEFT          BUTTON_MIDLEFT
#endif
#ifndef FC_RIGHT
#   define FC_RIGHT         BUTTON_MIDRIGHT
#endif
#ifndef FC_MOVE
#   define FC_MOVE          BUTTON_CENTER
#   define HK_MOVE           "CENTRE"
#endif
#ifndef FC_CUR2FC
#   define FC_CUR2FC       BUTTON_TOPRIGHT
#   define HK_CUR2FC        "TOPRIGHT"
#endif
#ifndef FC_CUR2STACK
#   define FC_CUR2STACK     BUTTON_BOTTOMLEFT
#   define HK_CUR2STACK      "BOTTOMLEFT"
#endif

#endif

#ifndef HK_LR
#   define HK_LR "LEFT/RIGHT"
#endif
#ifndef HK_UD
#   define HK_UD "UP/DOWN"
#endif

/**
 * Misc constants, graphics and other defines
 */

#include "pluginbitmaps/card_back.h"
#include "pluginbitmaps/card_deck.h"
#include "pluginbitmaps/solitaire_suitsi.h"

#define CARD_GFX_WIDTH  BMPWIDTH_card_back
#define CARD_GFX_HEIGHT BMPHEIGHT_card_back
#define CARD_WIDTH      (BMPWIDTH_card_back+2)
#define CARD_HEIGHT     (BMPHEIGHT_card_back+2)

#if LCD_WIDTH >= 640
#   define MARGIN 4
#   define LARGE_CARD
#   define SYMBOL_HEIGHT 24

#elif LCD_WIDTH >= 320
#   define MARGIN 4
#   define LARGE_CARD
#   define SYMBOL_HEIGHT 12
#elif LCD_WIDTH >= 220
#   define MARGIN 3
#   define LARGE_CARD
#   define SYMBOL_HEIGHT 12
#elif LCD_WIDTH >= 160
#   define MARGIN 2
#   define SYMBOL_HEIGHT 11
#elif LCD_WIDTH >= 128
#   define MARGIN 1
#   define SYMBOL_HEIGHT 10
#else
#   define MARGIN 0
#   define SYMBOL_HEIGHT 8
#endif

#define CARD_START (CARD_HEIGHT+2*MARGIN+1)

/* background color */
#ifdef HAVE_LCD_COLOR
#   define BACKGROUND_COLOR LCD_RGBPACK(0,157,0)
#   define FRAME_COLOR      LCD_RGBPACK(23,119,218)
#elif LCD_DEPTH > 1
#   define BACKGROUND_COLOR LCD_WHITE
#   define FRAME_COLOR      LCD_BLACK
#endif


#define CONFIG_FILENAME "freecell.cfg"

#define NOT_A_CARD -1

/* number of cards per suit */
#define CARDS_PER_SUIT 13

/* number of suits */
#define NUM_SUITS 4

#define NUM_CARDS ( CARDS_PER_SUIT * NUM_SUITS )

/* number of columns */
#define NUM_COL 8

/* number of cards to deal per column */
#define CARDS_PER_COL_A  7
#define CARDS_PER_COL_B  6


/* the four free slots */
#define NUM_FREECELLS 4

/* pseudo column numbers to be used for cursor coordinates */
/* columns 0 to NUM_COL-1 correspond to the eight columns,  */
/* columns NUM_COL to NUM_COL+NUM_FREECELLS-1 correspond to free cells, */
/* columns NUM_COL+NUM_FREECELLS to NUM_COL+NUM_FREECELLS+NUM_SUITS-1 correspond to the final stacks */
#define FREECELL_COL	NUM_COL
#define STACKS_COL		( NUM_COL + NUM_FREECELLS )

#define NOT_A_COL -1

#if defined(FC_LEFT_PRE)      || defined(FC_RIGHT_PRE)     || \
    defined(FC_DOWN_PRE)      || defined(FC_UP_PRE)        || \
    defined(FC_CUR2STACK_PRE) || defined(FC_MOVE_PRE)      || \
    defined(FC_REM2CUR_PRE)   || defined(FC_REM2STACK_PRE) || \
    defined(FC_CUR2FC_PRE)
#   define NEED_LASTBUTTON_VAR
#endif

typedef struct
{
    signed char suit;
    signed char num;
    bool used : 1; /* this is what is used when dealing cards */
    signed char next;
} card_t;


/**
 * LCD card drawing routines
 */

static void draw_cursor( int x, int y )
{
    rb->lcd_set_drawmode( DRMODE_COMPLEMENT );
    rb->lcd_fillrect( x+1, y+1, CARD_GFX_WIDTH, CARD_GFX_HEIGHT );
#ifdef LARGE_CARD
    rb->lcd_drawpixel( x+1, y+1 );
    rb->lcd_drawpixel( x+1, y+CARD_HEIGHT-2 );
    rb->lcd_drawpixel( x+CARD_WIDTH-2, y+1 );
    rb->lcd_drawpixel( x+CARD_WIDTH-2, y+CARD_HEIGHT-2 );
#endif
    rb->lcd_set_drawmode( DRMODE_SOLID );
}

/* Draw a card's border, select it if it's selected and draw the cursor
 * if the cursor is currently over the card */
static void draw_card_ext( int x, int y, bool selected, bool cursor )
{
#if LCD_DEPTH > 1
    int oldfg = rb->lcd_get_foreground();

    rb->lcd_set_foreground( LCD_BLACK );
#endif
#ifdef LARGE_CARD
    rb->lcd_hline( x+2, x+CARD_WIDTH-3, y );
    rb->lcd_hline( x+2, x+CARD_WIDTH-3, y+CARD_HEIGHT-1 );
    rb->lcd_vline( x, y+2, y+CARD_HEIGHT-3 );
    rb->lcd_vline( x+CARD_WIDTH-1, y+2, y+CARD_HEIGHT-3 );
    rb->lcd_drawpixel( x+1, y+1 );
    rb->lcd_drawpixel( x+1, y+CARD_HEIGHT-2 );
    rb->lcd_drawpixel( x+CARD_WIDTH-2, y+1 );
    rb->lcd_drawpixel( x+CARD_WIDTH-2, y+CARD_HEIGHT-2 );
#else
    rb->lcd_hline( x+1, x+CARD_WIDTH-2, y );
    rb->lcd_hline( x+1, x+CARD_WIDTH-2, y+CARD_HEIGHT-1 );
    rb->lcd_vline( x, y+1, y+CARD_HEIGHT-2 );
    rb->lcd_vline( x+CARD_WIDTH-1, y+1, y+CARD_HEIGHT-2 );
#endif

    if( selected )
    {
#if LCD_DEPTH > 1
        rb->lcd_set_foreground( FRAME_COLOR );
#endif
        rb->lcd_drawrect( x+1, y+1, CARD_WIDTH-2, CARD_HEIGHT-2 );
#ifdef LARGE_CARD
        rb->lcd_drawrect( x+2, y+2, CARD_WIDTH-4, CARD_HEIGHT-4 );
#endif
    }
#if LCD_DEPTH > 1
    rb->lcd_set_foreground( oldfg );
#endif

    if( cursor )
    {
        draw_cursor( x, y );
    }
}

/* Draw a card's inner graphics */
static void draw_card( card_t *card, int x, int y,
                       bool selected, bool cursor )
{
        rb->lcd_bitmap_part( card_deck, CARD_GFX_WIDTH * card->num,
                             CARD_GFX_HEIGHT * card->suit, 
                             STRIDE(SCREEN_MAIN, 
                                    BMPWIDTH_card_deck, BMPHEIGHT_card_deck),
                             x+1, y+1, CARD_GFX_WIDTH, CARD_GFX_HEIGHT );
    draw_card_ext( x, y, selected, cursor );
}

/* Draw an empty stack */
static void draw_empty_stack( int s, int x, int y, bool cursor )
{
    rb->lcd_bitmap_part( solitaire_suitsi, 0,
                 CARD_GFX_HEIGHT * s, 
                 STRIDE( SCREEN_MAIN,
                         BMPWIDTH_solitaire_suitsi, BMPHEIGHT_solitaire_suitsi),
                 x+1, y+1, CARD_GFX_WIDTH, CARD_GFX_HEIGHT );

    draw_card_ext( x, y, false, cursor );
}

/* Draw an empty free cell */
static void draw_free_cell( int x, int y, bool cursor )
{
    draw_card_ext( x, y, false, cursor );
}

/* Help */
static bool freecell_help( void )
{
    static char* help_text[] = {
        "FreeCell", "", "Controls", "",
        HK_LR   ":", "Move", "the", "cursor", "to", "the",
            "previous/", "next", "column.", "",
        HK_UD   ":", "Move", "the", "cursor", "up/", "down", "in", "the",
            "column.", "",
        HK_MOVE ":", "Select", "cards,", "move", "cards...", "", "",
        "Shortcuts", "",
        "Play"   ":", "Put", "the", "card", "under", "the", "cursor", 
            "in", "an", "open", "free", "cell.", "",
        HK_CUR2STACK ":", "Put", "the", "card", "under", "the", "cursor",
            "on", "one", "of", "the", "4", "final", "stacks."
    };
    static struct style_text formation[]={
        { 0, TEXT_CENTER|TEXT_UNDERLINE },
        { 2, C_RED },
        { 30, C_RED },
        LAST_STYLE_ITEM
    };
    if (display_text(ARRAYLEN(help_text), help_text, formation, NULL, true))
        return true;
    return false;
}

/**
 * Custom menu / options
 */

void freecell_init(void);

int find_last_card( int col );

/* menu return codes */
enum { MENU_RESUME, MENU_SAVE_AND_QUIT, MENU_QUIT, MENU_USB };

static bool _ingame;

int freecell_menu_cb(int action, const struct menu_item_ex *this_item)
{
    int i = (intptr_t)this_item;
    if( action == ACTION_REQUEST_MENUITEM )
    {
        if( !_ingame && (i==0 || i==4))
            return ACTION_EXIT_MENUITEM;
    }
    return action;
}

int freecell_menu(bool in_game)
{
    int selected = 0;
    int result = -1;

    MENUITEM_STRINGLIST(menu, "FreeCell Menu", freecell_menu_cb,
                        "Resume Game", "Start New Game",
                        "Help", "Playback Control",
                        "Quit Without Saving", "Quit");
    _ingame = in_game;

    while (result < 0)
    {
        switch (rb->do_menu(&menu, &selected, NULL, false))
        {
            default:
                result = MENU_RESUME;
                break;

            case MENU_ATTACHED_USB:
                result = MENU_USB;
                break;

            case 0:
                result = MENU_RESUME;
                break;

            case 1:
                freecell_init();
                result = MENU_RESUME;
                break;

            case 2:
                if (freecell_help())
                    result = MENU_USB;
                break;

            case 3:
                 playback_control(NULL);
                 break;
				 
		    case 4:
                result = MENU_QUIT;
                break;

            case 5:
                result = MENU_SAVE_AND_QUIT;
                break;


        }
    }
    return result;
}

/**
 * Global variables
 */

/* player's cursor */
int cur_card;
/* player's cursor column num */
int cur_col;

/* selected card */
int sel_card;

/* the deck */
card_t deck[ NUM_CARDS ];

/* the 8 game columns */
int cols[NUM_COL];
/* the 4 free slots */
int freecells[NUM_FREECELLS];
/* the 4 final stacks */
int stacks[NUM_SUITS];

/**
 * Card handling routines
 */

int next_random_card( card_t *deck )
{
    int i,r;

    r = rb->rand()%(NUM_CARDS)+1;
    i = 0;

    while( r>0 )
    {
        i = (i + 1)%(NUM_CARDS);
        if( !deck[i].used ) r--;
    }

    deck[i].used = true;

    return i;
}


/* initialize the game */
void freecell_init( void )
{
    int c;
    int i, j;

    /* init deck */
    for( i=0; i<NUM_SUITS; i++ )
    {
        for( j=0; j<CARDS_PER_SUIT; j++ )
        {
#define card deck[i*CARDS_PER_SUIT+j]
            card.suit = i;
            card.num = j;
            /* card.known = true; */
            card.used = false;
            card.next = NOT_A_CARD;
#undef card
        }
    }

    /* deal the cards ... */
    /* ... in the columns */
    for( i=0; i<NUM_COL; i++ )
    {
        c = NOT_A_CARD;
        for( j=0; j<(i<4?CARDS_PER_COL_A:CARDS_PER_COL_B); j++ )
        {
            if( c == NOT_A_CARD )
            {
                cols[i] = next_random_card( deck );
                c = cols[i];
            }
            else
            {
                deck[c].next = next_random_card( deck );
                c = deck[c].next;
            }
        }
    }

    /* we now finished dealing the cards. The game can start ! (at last) */

    /* init the stack */
    for( i = 0; i<NUM_SUITS; i++ )
    {
        stacks[i] = NOT_A_CARD;
    }

	/* init the free cells */
	for( i=0; i<NUM_FREECELLS; i++ )
	{
		freecells[i] = NOT_A_CARD;
	}

    /* the cursor starts on lower left card */
    cur_card = find_last_card( 0 );
    cur_col = 0;

    /* no card is selected */
    sel_card = NOT_A_CARD;
    
}

/* find the column number in which 'card' can be found */
int find_card_col( int card )
{
    int i;
    int c;

    if( card == NOT_A_CARD ) return NOT_A_COL;

    for( i=0; i<NUM_COL; i++ )
    {
        c = cols[i];
        while( c != NOT_A_CARD )
        {
            if( c == card ) return i;
            c = deck[c].next;
        }
    }

	for( i=0; i<NUM_FREECELLS; i++ )
    {
        if( freecells[i] == card ) return (FREECELL_COL + i);
    }

	
    for( i=0; i<NUM_SUITS; i++ )
    {
        c = stacks[i];
        while( c != NOT_A_CARD )
        {
            if( c == card ) return STACKS_COL + i;
            c = deck[c].next;
        }
    }

    return NOT_A_COL;
}

/* determines if the cards are alternating colors & decreasing number*/
bool is_stackable( int top_card, int bottom_card ) {
    bool stackable = false;
    if (top_card == NOT_A_CARD || bottom_card == NOT_A_CARD) 
        stackable = false;
    if (( deck[bottom_card].suit + deck[top_card].suit)%2==1
                  && deck[top_card].num == (deck[bottom_card].num-1))
        stackable = true;
	return stackable;
}

/* find the card preceding 'card' */
/* if it doesn't exist, return NOT_A_CARD */
int find_prev_card( int card ){
    int i;

    for( i=0; i < NUM_CARDS; i++ )
    {
        if( deck[i].next == card ) return i;
    }

    return NOT_A_CARD;
}

/* find the last card of a given column */
int find_last_card( int col )
{
    int c;

    if( col < NUM_COL )
    {
        c = cols[col];
    }
    else if( col < STACKS_COL )
    {
        c = freecells[col - FREECELL_COL];
    }
    else if( col < NUM_COL+NUM_FREECELLS+NUM_SUITS )
    {
        c = stacks[col - STACKS_COL];
    }
    else
    {
        /* there is a problem... */
        rb->splashf(HZ, "Error: Attempting to find_last_card for column %i", col);
        return NOT_A_CARD;
    }

    if(c == NOT_A_CARD)
        return c;
    else
    {
        while(deck[c].next != NOT_A_CARD)
            c = deck[c].next;
        return c;
    }
}

enum move { MOVE_OK, MOVE_NOT_OK };

enum move move_card( int dest_col, int src_card )
{
    /* the column from which to take src_card */
    int src_col;

    /* the last card of dest_col */
    int dest_card;

    /* the card under src_card */
    int src_card_prev;

    /* you can't move no card (at least, it doesn't have any consequence) */
    if( src_card == NOT_A_CARD ) return MOVE_NOT_OK;

    src_col = find_card_col( src_card );
    dest_card = find_last_card( dest_col );
    src_card_prev = find_prev_card( src_card );

	/* you can't move a card from the final stacks in free cell */
	if( src_col >= STACKS_COL ) return MOVE_NOT_OK;
	
    /* if we (that means dest) are on one of the 8 columns ... */
    if( dest_col < NUM_COL )
    {
        /* ... check if we are on an empty stack, if so, it's ok */
        if( dest_card == NOT_A_CARD )
        {
            cols[dest_col] = src_card;
        }
        /* ... or check if the cards follow one another and have
         * different colorsuit */
        else if( is_stackable( src_card, dest_card ) )
        {
            /* this is a winning combination */
            deck[dest_card].next = src_card;
			/* JStark TODO: when the top card of an alternating stack is moved, */
			/* check to see if a larger stack could be moved.... this is more complex. */
        }
        /* ... or, humpf, well that's not good news */
        else
        {
            /* this is not a winning combination */
            return MOVE_NOT_OK;
        }
    }
    /* if we are on one of the free cells ... */
    else if ( dest_col >= FREECELL_COL && dest_col < STACKS_COL )
    {
        /* ... check if we are on an empty stack and only moving top card... */
        if( dest_card == NOT_A_CARD && deck[src_card].next == NOT_A_CARD )
        {
            freecells[dest_col - FREECELL_COL] = src_card;
        }
        else /* non-empty stack */
        {
			return MOVE_NOT_OK;
        }
    }
    /* if dest_col is the final suit stacks */
    else
    {
		/* card must go with correct suit */
		if( dest_col != STACKS_COL+deck[src_card].suit ) return MOVE_NOT_OK;

		/* is the stack started?  */
		if( dest_card == NOT_A_CARD )
		{
			/* can only move an ace to start a stack */	
			if( deck[src_card].num == 0 )
			{
				stacks[dest_col - STACKS_COL] = src_card;
			}
			else 
			{
				return MOVE_NOT_OK;
			}
		} else {
			if( deck[src_card].num == deck[dest_card].num + 1 )
			{
				deck[dest_card].next = src_card;
			}
			else 
			{
				return MOVE_NOT_OK;
			}
		}
		
    }

    /* if the src card is from somewhere else, just take everything */
    if( src_card_prev == NOT_A_CARD )
	{
		if( src_col < NUM_COL )
		{
			cols[src_col] = NOT_A_CARD;
		}
		else if( src_col >= FREECELL_COL && src_col < STACKS_COL )
		{
			freecells[src_col - FREECELL_COL] = NOT_A_CARD;
		}
		else if( src_col >= STACKS_COL )
		{
			/* this should never happen */
			return MOVE_NOT_OK;
		}
	}
	else
	{
		deck[src_card_prev].next = NOT_A_CARD;
	}
    sel_card = NOT_A_CARD;
    /* tada ! */
    return MOVE_OK;
}

enum { FREECELL_WIN, FREECELL_SAVE_AND_QUIT, FREECELL_QUIT, FREECELL_USB };

/**
 * Bouncing cards at the end of the game
 */

#define BC_ACCEL   ((1<<16)*LCD_HEIGHT/128)
#define BC_MYSPEED (6*BC_ACCEL)
#define BC_MXSPEED (6*LCD_HEIGHT/128)

int bouncing_cards( void )
{
    int i, j, x, vx, y, fp_y, fp_vy, button;

    /* flush the button queue */
    while( ( button = rb->button_get( false ) ) != BUTTON_NONE )
    {
        if( rb->default_event_handler( button ) == SYS_USB_CONNECTED )
            return FREECELL_USB;
    }

    /* fun stuff :) */
    for( i = CARDS_PER_SUIT-1; i>=0; i-- )
    {
        for( j = 0; j < NUM_SUITS; j++ )
        {
            x = LCD_WIDTH-(CARD_WIDTH*4+4+MARGIN)+CARD_WIDTH*j+j+1;
            fp_y = MARGIN<<16;

#if LCD_WIDTH > 200
            vx = rb->rand() % (4*BC_MXSPEED/3-2) - BC_MXSPEED;
            if( vx >= -1 )
                vx += 3;
#else
            vx = rb->rand() % (4*BC_MXSPEED/3) - BC_MXSPEED;
            if( vx >= 0 )
                vx++;
#endif

            fp_vy = -rb->rand() % BC_MYSPEED;

            while( x < LCD_WIDTH && x + CARD_WIDTH > 0 )
            {
                fp_vy += BC_ACCEL;
                x += vx;
                fp_y += fp_vy;
                if( fp_y >= (LCD_HEIGHT-CARD_HEIGHT) << 16 )
                {
                    fp_vy = -fp_vy*4/5;
                    fp_y = (LCD_HEIGHT-CARD_HEIGHT) << 16;
                }
                y = fp_y >> 16;
                draw_card( &deck[j*CARDS_PER_SUIT+i], x, y,
                           false, false );
                rb->lcd_update_rect( x<0?0:x, y<0?0:y,
                                     CARD_WIDTH, CARD_HEIGHT );

                button = rb->button_get_w_tmo( 2 );
                if( rb->default_event_handler( button ) == SYS_USB_CONNECTED )
                    return FREECELL_USB;
                if( button == FC_QUIT || button == FC_MOVE )
                    return FREECELL_WIN;
            }
        }
    }
    return FREECELL_WIN;
}

/**
 * Game save/load routines
 */
void get_save_filename( char *buf )
{
#ifdef APPLICATION
    rb->snprintf(buf, sizeof(buf), PLUGIN_DATA_DIR "/freecell.save");
#else
    char *s;
    rb->strcpy( buf, rb->plugin_get_current_filename() );
    s = rb->strrchr( buf, '/' ) + 1;
    *s = '\0';
    rb->strcat( s, "freecell.save" );
#endif
}

int open_save_file( int flags )
{
    char buf[MAX_PATH];
    get_save_filename( buf );
    return rb->open( buf, flags, 0666);
}

void delete_save_file( void )
{
    char buf[MAX_PATH];
    get_save_filename( buf );
    rb->remove( buf );
}

#ifdef write
#   undef write
#endif
int save_write( int fd, const void *buf, size_t count, int *checksum )
{
    size_t i;
    if( rb->write( fd, buf, count ) < (ssize_t)count )
        return 1;
    for( i = 0; i < count; i++ )
        *checksum += (int)(((const char *)buf)[i]);
    return 0;
}

#ifdef read
#   undef read
#endif
int save_read( int fd, void *buf, size_t count, int *checksum )
{
    size_t i;
    if( rb->read( fd, buf, count ) < (ssize_t)count )
        return 1;
    for( i = 0; i < count; i++ )
        *checksum -= (int)(((const char *)buf)[i]);
    return 0;
}

int save_game( void )
{
    int fd = open_save_file( O_CREAT|O_WRONLY|O_TRUNC );
    int checksum = 42;
    if( fd < 0 )
        return -1;
    if(    save_write( fd, &cur_card, sizeof( int ), &checksum )
        || save_write( fd, &cur_col, sizeof( int ), &checksum )
        || save_write( fd, &sel_card, sizeof( int ), &checksum )
        || save_write( fd, deck, NUM_CARDS * sizeof( card_t ), &checksum )
        || save_write( fd, cols, NUM_COL * sizeof( int ), &checksum )
		|| save_write( fd, freecells, NUM_FREECELLS * sizeof( int ), &checksum )
        || save_write( fd, stacks, NUM_SUITS * sizeof( int ), &checksum )
        || ( rb->write( fd, &checksum, sizeof( int ) ) < (ssize_t)(sizeof( int ) ) ) )
    {
        rb->close( fd );
        rb->splash( 2*HZ, "Error while saving game. Aborting." );
        return -2;
    }
    rb->close( fd );
    return 0;
}

int load_game( void )
{
    int checksum, retval;
    
    int fd = open_save_file( O_RDONLY );
    if( fd < 0 )
        return -1;
    
    retval = 0; /* Assume good case */
    if(    ( rb->lseek( fd, -sizeof( int ), SEEK_END ) == -((ssize_t)sizeof( int ))-1 )
        || ( rb->read( fd, &checksum, sizeof( int ) ) < ((ssize_t)sizeof( int )) )
        || ( rb->lseek( fd, 0, SEEK_SET ) == -1 )
        || save_read( fd, &cur_card, sizeof( int ), &checksum )
        || save_read( fd, &cur_col, sizeof( int ), &checksum )
        || save_read( fd, &sel_card, sizeof( int ), &checksum )
        || save_read( fd, deck, NUM_CARDS * sizeof( card_t ), &checksum )
        || save_read( fd, cols, NUM_COL * sizeof( int ), &checksum )
		|| save_read( fd, freecells, NUM_FREECELLS * sizeof( int ), &checksum )
        || save_read( fd, stacks, NUM_SUITS * sizeof( int ), &checksum ) )
    {
        rb->splash( 2*HZ, "Error while loading saved game. Aborting." );
        retval = -2;
    }
    else if( checksum != 42 )
    {
        rb->splash( 2*HZ, "Save file was corrupted. Aborting." );
        retval = -3;
    }
    
    rb->close( fd );
    delete_save_file();
    return retval;
}

/**
 * The main game loop
 *
 * If skipmenu is defined to FREECELL_QUIT, the menu will be skipped and
 * game will resume.
 */

int freecell( int skipmenu )
{
    int i,j;
    int button;
#ifdef NEED_LASTBUTTON_VAR
    int lastbutton = 0;
#endif
    int c,h;
    int biggest_col_length;

    rb->srand( *rb->current_tick );
    if( skipmenu != FREECELL_QUIT )
    {
        switch( freecell_menu(false) )
        {
            case MENU_QUIT:
                return FREECELL_QUIT;

            case MENU_USB:
                return FREECELL_USB;
        }
        freecell_init();
    }

    while( true )
    {
        rb->lcd_clear_display();

        /* get the biggest column length so that display can be "optimized" */
        biggest_col_length = 0;

        for(i=0;i<NUM_COL;i++)
        {
            j = 0;
            c = cols[i];

            if( c != NOT_A_CARD )
            {
                while( true )
                {
                    /* don't include the last card in the column length. */
                    if( deck[c].next == NOT_A_CARD )
                    {
                        break;  /* no successor: get outta here. */
                    }
                    else
                    {
                    	j++;
                    }
                    c = deck[c].next;
                }
                /* make column distinguishable from an empty column,
                 * and avoid division by zero while displaying */
                if( j == 0 )
                    j = 1;
            }
            if( j > biggest_col_length )
                biggest_col_length = j;
        }

        /* check if there are cards remaining in the game. */
        /* if there aren't any, that means you won :) */
        if( biggest_col_length == 0 )
        {
            rb->lcd_update();
            rb->splash( HZ, "You Won :)" );
            return FREECELL_WIN;
        }

        /* draw the columns */
        for( i = 0; i < NUM_COL; i++ )
        {
            c = cols[i];
            j = CARD_START;
            while( true )
            {
                if( c == NOT_A_CARD )
                {
                    /* draw the cursor on empty columns */
                    if( cur_col == i )
                    {
                        draw_cursor( MARGIN + i * (CARD_WIDTH
                                     +(LCD_WIDTH-NUM_COL*CARD_WIDTH-2*MARGIN)/(NUM_COL-1)),
                                     j );
                    }
                    break;
                }

                draw_card( &deck[c], MARGIN + i * (CARD_WIDTH
                           +(LCD_WIDTH-NUM_COL*CARD_WIDTH-2*MARGIN)/(NUM_COL-1)),
                           j, c == sel_card, c == cur_card );

                h = c;
                c = deck[c].next;
                if( c == NOT_A_CARD ) break;

                /* This is where we change the spacing between cards so that
                 * they don't overflow out of the LCD */
                if( h == cur_card )
                    j += SYMBOL_HEIGHT;
                else 
                    j += min( SYMBOL_HEIGHT,
                              2 * (LCD_HEIGHT - CARD_START - CARD_HEIGHT - MARGIN)
                               / biggest_col_length );
                /* else
                    j += min( SYMBOL_HEIGHT / 2,
                              (LCD_HEIGHT - CARD_START - CARD_HEIGHT - MARGIN)
                               / biggest_col_length ); */
            }
        }

		/* draw the freecells */
		for( i=0; i<NUM_FREECELLS; i++ )
		{
			c = find_last_card( FREECELL_COL + i );

			if ( c != NOT_A_CARD )
			{
				draw_card( &deck[c], (CARD_WIDTH+MARGIN)*i, MARGIN, 
				          c == sel_card, cur_col == FREECELL_COL + i);
			}
			else 
			{
				draw_free_cell ( (CARD_WIDTH+MARGIN)*i, MARGIN, 
				          cur_col == FREECELL_COL + i);
			}
		}
		
        /* draw the stacks */
        for( i=0; i<NUM_SUITS; i++ )
        {
            c = find_last_card( STACKS_COL + i );

            if( c != NOT_A_CARD )
            {
                draw_card( &deck[c],
                           LCD_WIDTH-(CARD_WIDTH*4+4+MARGIN)+CARD_WIDTH*i+i+1,
                           MARGIN,
                           c == sel_card, cur_col == STACKS_COL + i );
            }
            else
            {
                draw_empty_stack( i,
                           LCD_WIDTH-(CARD_WIDTH*4+4+MARGIN)+CARD_WIDTH*i+i+1,
                           MARGIN, cur_col == STACKS_COL + i );
            }
        }

        rb->lcd_update();

        /* what to do when a key is pressed ... */
        button = rb->button_get( true );
#if (CONFIG_KEYPAD == SANSA_E200_PAD) || (CONFIG_KEYPAD == SANSA_FUZE_PAD)
        if (button&(BUTTON_SCROLL_BACK|BUTTON_SCROLL_FWD))
            button = button & (~BUTTON_REPEAT);
#endif
        switch( button )
        {
            /* move cursor to the next column or free cell */
            case FC_RIGHT:
#ifdef FC_RIGHT_PRE
                if( lastbutton != FC_RIGHT_PRE )
                    break;
#endif
                if( cur_col >= NUM_COL + NUM_FREECELLS )
                {
                    cur_col = 0;
                }
                else
                {
                    cur_col = (cur_col+1)%(NUM_COL + NUM_FREECELLS);
                }
                cur_card  = find_last_card( cur_col );
                break;

            /* move cursor to the previous column
             * or to the previous free cell */
            case FC_LEFT:
#ifdef FC_LEFT_PRE
                if( lastbutton != FC_LEFT_PRE )
                    break;
#endif
                if( cur_col == 0 )
                {
                    cur_col = NUM_COL + NUM_FREECELLS - 1;
                }
                else
                {
                    cur_col = (cur_col-1)%(NUM_COL + NUM_FREECELLS - 1);
                }
                cur_card = find_last_card( cur_col );
                break;

            /* move cursor to card that's bellow */
            case FC_DOWN:
#ifdef FC_DOWN_PRE
                if( lastbutton != FC_DOWN_PRE )
                    break;
#else
            case FC_DOWN|BUTTON_REPEAT:
#endif
                /* Don't do this... */
				/* cur_card = find_last_card( cur_col ); */
				if( cur_card == NOT_A_CARD ) break;
                if( deck[cur_card].next != NOT_A_CARD )
                {
                    cur_card = deck[cur_card].next;
                }
                else
                {
					/* do nothing */
				}
                break;

            /* move cursor to card that's above */
            case FC_UP:
#ifdef FC_UP_PRE
                if( lastbutton != FC_UP_PRE )
                    break;
#else
            case FC_UP|BUTTON_REPEAT:
#endif
                /* Don't do this - the current card might not be the top card. */
                /* cur_card = find_last_card( cur_col ); */
                if( cur_card == NOT_A_CARD ) break;
				int new_cur_card = find_prev_card( cur_card );
                if( new_cur_card != NOT_A_CARD )
                {
                    cur_card = new_cur_card;
                }
                break;

            /* Try to put card under cursor on one of the stacks */
            case FC_CUR2STACK:
#ifdef FC_CUR2STACK_PRE
                if( lastbutton != FC_CUR2STACK_PRE )
                    break;
#endif
                move_card( deck[cur_card].suit + STACKS_COL, cur_card );
                break;

            /* (select button) Move cards arround, Uncover cards, ... */
            case FC_MOVE:
#ifdef FC_MOVE_PRE
                if( lastbutton != FC_MOVE_PRE )
                    break;
#endif

                if( sel_card == NOT_A_CARD )
                {
                    if( cur_card != NOT_A_CARD )
                    {
                        /* select a card */
                        sel_card = cur_card;
                    }
                }
                else if( sel_card == cur_card )
                {
                    /* unselect card or try putting card on
                     * one of the 4 stacks */
					int prev_card = find_prev_card( cur_card );
                    if( move_card( deck[sel_card].suit + STACKS_COL, sel_card )
                        == MOVE_OK )
                    {
                        cur_card = prev_card;
                    }
                    sel_card = NOT_A_CARD;
                }
                else
                {
			        int i;
			        int fcols = 0;
			        int ncards = 1;
			        /* calculate the max. number of cards that can be moved */
			        for( i=0; i<NUM_FREECELLS; i++ )
			        {
				        if( freecells[i] == NOT_A_CARD ) ncards++;
			        }
			        for( i=0; i<NUM_COL; i++ ) 
			        {
				        if( cols[i] == NOT_A_CARD ) fcols++;
			        }
			        ncards = (ncards)*(1 + fcols);

                    /* Count number of cards to be moved, be sure enough */		            
                    /* free cell space is available */
                    int ccard = sel_card;
                    while ((ccard = deck[ccard].next) != NOT_A_CARD)
                    {
                        ncards--;
                    }

			        /* try moving cards, moving up */
			        while (sel_card != NOT_A_CARD && ncards > 0) 
                    {
                        if (move_card( cur_col, sel_card ) == MOVE_OK) break;
                        int prev_card = find_prev_card (sel_card);
                        if (!is_stackable(prev_card, sel_card)) break;
                        sel_card = prev_card;
                        ncards--;
                    }
                    
                }
                break;

            /* move current card to the first open free cell */
            case FC_CUR2FC:
#ifdef FC_CUR2FC_PRE
                if( lastbutton != FC_CUR2FC_PRE )
                    break;
#endif
                if( cur_card == NOT_A_CARD )
                {
                    break;
                }
                /* can only move top card from stack onto free cell */
                if( deck[cur_card].next != NOT_A_CARD ) {
                    break;
                }

                int i;
                for( i=0; i<NUM_FREECELLS; i++ )
                {
                    if( freecells[i] == NOT_A_CARD )
                    {
                        move_card( FREECELL_COL + i, cur_card );
                    }
                }
                break;

            /* Show the menu */
#ifdef FC_RC_QUIT
            case FC_RC_QUIT:
#endif
            case FC_QUIT:
                switch( freecell_menu(true) )
                {
                    case MENU_SAVE_AND_QUIT:
                        return FREECELL_SAVE_AND_QUIT;

                    case MENU_QUIT:
                        return FREECELL_QUIT;

                    case MENU_USB:
                        return FREECELL_USB;
                }
                break;

            case SYS_POWEROFF:
                return FREECELL_SAVE_AND_QUIT;

            default:
                if( rb->default_event_handler( button ) == SYS_USB_CONNECTED )
                    return FREECELL_USB;
                break;
        }

#ifdef NEED_LASTBUTTON_VAR
        if( button != BUTTON_NONE )
            lastbutton = button;
#endif

        /* fix incoherences concerning cur_col and cur_card */
        c = find_card_col( cur_card );
        if( c != NOT_A_COL && c != cur_col )
            cur_card = find_last_card( cur_col );

        if(    cur_card == NOT_A_CARD
            && find_last_card( cur_col ) != NOT_A_CARD )
            cur_card = find_last_card( cur_col );

        rb->yield();
    }
}

/**
 * Plugin entry point
 */

enum plugin_status plugin_start(const void* parameter )
{
    int result;

    /* plugin init */
    (void)parameter;

    if( load_game() == 0 )
    {
        rb->splash( HZ, "Resuming saved game." );
        result = FREECELL_QUIT;
    }
    else
        result = FREECELL_WIN;

    /* play the game :)
     * Keep playing if a game was won (that means display the menu after
     * winning instead of quiting) */
    while( ( result = freecell( result ) ) == FREECELL_WIN );

    if( result != FREECELL_QUIT )
        /* result == FREECELL_USB || result == FREECELL_SAVE_AND_QUIT */
        save_game();

    /* Exit the plugin */
    return ( result == FREECELL_USB ) ? PLUGIN_USB_CONNECTED : PLUGIN_OK;
}
