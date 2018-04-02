/***************************************************************************
 *             __________               __   ___.
 *   Open      \______   \ ____   ____ |  | _\_ |__   _______  ___
 *   Source     |       _//  _ \_/ ___\|  |/ /| __ \ /  _ \  \/  /
 *   Jukebox    |    |   (  <_> )  \___|    < | \_\ (  <_> > <  <
 *   Firmware   |____|_  /\____/ \___  >__|_ \|___  /\____/__/\_ \
 *                     \/            \/     \/    \/            \/
 * 
 * $Id: BOFH.c  2007-09-27 GRaTT $
 *
 * Copyright (C) 2007 Gerritt Gonzales
 *
 * All files in this archive are subject to the GNU General Public License.
 * See the file COPYING in the source tree root for full license agreement.
 *
 * This software is distributed on an "AS IS" basis, WITHOUT WARRANTY OF ANY
 * KIND, either express or implied.
 *
 * BOFH see link if you do not know what it is
 * and to get full excuse list. All credit for the idea
 * Email: Jeff Ballard <ballard@NOSPAMcs.wisc.edu>
 * http://pages.cs.wisc.edu/~ballard/bofh/
 * Like "tip of the day", it will offer one random
 * line of the file /.rockbox/rocks/demos/excuses,
 * you must put the file there manually.
 *
 ****************************************************************************/
#include "plugin.h"
#include "lib/pluginlib_actions.h"

/* variable button definitions */

#if CONFIG_KEYPAD == RECORDER_PAD
#define QUIT BUTTON_OFF

#elif CONFIG_KEYPAD == ARCHOS_AV300_PAD
#define QUIT BUTTON_OFF

#elif CONFIG_KEYPAD == ONDIO_PAD
#define QUIT BUTTON_OFF

#elif CONFIG_KEYPAD == PLAYER_PAD
#define QUIT BUTTON_STOP

#elif (CONFIG_KEYPAD == IRIVER_H100_PAD) || \
      (CONFIG_KEYPAD == IRIVER_H300_PAD)
#define QUIT BUTTON_OFF
#define VIEWER_RC_QUIT BUTTON_RC_STOP

#elif (CONFIG_KEYPAD == IPOD_4G_PAD) || \
      (CONFIG_KEYPAD == IPOD_3G_PAD) || \
      (CONFIG_KEYPAD == IPOD_1G2G_PAD)
#define QUIT BUTTON_SELECT

#elif CONFIG_KEYPAD == IRIVER_IFP7XX_PAD
#define QUIT BUTTON_PLAY

#elif CONFIG_KEYPAD == IAUDIO_X5M5_PAD
#define QUIT BUTTON_POWER

#elif CONFIG_KEYPAD == GIGABEAT_PAD
#define QUIT BUTTON_POWER

#elif CONFIG_KEYPAD == SANSA_E200_PAD
#define QUIT BUTTON_POWER

#elif CONFIG_KEYPAD == SANSA_C200_PAD
#define QUIT BUTTON_POWER

#elif CONFIG_KEYPAD == IRIVER_H10_PAD
#define QUIT BUTTON_POWER

#else
#define QUIT PLA_CANCEL
#endif

enum plugin_status plugin_start(const void* parameter)
{
    (void)parameter;

    /* Seed rand */
    rb->srand(*rb->current_tick);

    int f, button;
    int w, h;
    char file[]={"/.rockbox/data/bofh-excuses"};
    char buff[256];
    int count=0, count1=0, counter=0;
    
    rb->lcd_getstringsize("A",&w,&h);

    rb->lcd_putsxy(LCD_WIDTH/2-(w*3),0, "BOFH");  
    rb->lcd_putsxy(LCD_WIDTH/2-(w*9),h, "Says the cause of");  
    rb->lcd_putsxy(LCD_WIDTH/2-(w*9),h*2, "the problem is...");  
    
    f = rb->open(file, O_RDONLY);
    if(f < 0) 
    {
        rb->splashf(HZ*2, "Can't open file %s", file);
    } else {
        while(rb->read_line(f, buff, 256))
        {
            /* line count */
	    counter++;
        }
	count1=rb->rand() % (counter);
        /* rb->splashf(HZ*1, "Excuse # %d", count1); */
    }
    rb->close(f);

    f = rb->open(file, O_RDONLY);
    if(f < 0) 
    {
        rb->splashf(HZ*2, "Can't open file %s", file);
    } else {
        while(rb->read_line(f, buff, 256))
        {
	    count++;

	    if (count1==count){
		rb->lcd_putsxy(0,(LCD_HEIGHT-h-5),":~/Press any key");
                rb->splashf(HZ*5, "%s", buff); 
		/* rb->button_get(true); */
                rb->button_get_w_tmo(HZ*6);
                break;
	    }

	    button = rb->button_get(false);
            switch(button)
            {
            case(QUIT):
                break;
                
            default:
                if (rb->default_event_handler_ex(button, NULL, NULL)
                    == SYS_USB_CONNECTED)
                    return PLUGIN_USB_CONNECTED;
                break;
            }
	} 
        rb->close(f);
    }
    rb->lcd_clear_display();
    rb->lcd_putsxy(LCD_WIDTH/2-(w*6),h, "Bastard");  
    rb->lcd_putsxy(LCD_WIDTH/2-(w*6),h*2, "Operator");  
    rb->lcd_putsxy(LCD_WIDTH/2-(w*6),h*3, "From Hell");  
    rb->lcd_update();
    rb->sleep(HZ*2); 
    return PLUGIN_OK;
}