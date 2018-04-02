
/*****************************************************************************
 *             __________               __   ___.
 *   Open      \______   \ ____   ____ |  | _\_ |__   _______  ___
 *   Source     |       _//  _ \_/ ___\|  |/ /| __ \ /  _ \  \/  /
 *   Jukebox    |    |   (  <_> )  \___|    < | \_\ (  <_> > <  <
 *   Firmware   |____|_  /\____/ \___  >__|_ \|___  /\____/__/\_ \
 *                     \/            \/     \/    \/            \/
 * $Id: HTML_Parser $ 
 *
 * Copyright (C) 2007 Gerritt Gonzales
 *
 * All files in this archive are subject to the GNU General Public License.
 * See the file COPYING in the source tree root for full license agreement.
 *
 * This software is distributed on an "AS IS" basis, WITHOUT WARRANTY OF ANY
 * KIND, either express or implied.
 *
 * This program will parse html and htm files, suitable for reading
 * in the text viewer.
 * More formatting to come.
 * Usage: Select a html or htm file, then select the txt file created 
 *        for viewing.
 ****************************************************************************/

#include "plugin.h"

/* This macros must always be included. Should be placed at the top by
   convention, although the actual position doesn't matter */
//PLUGIN_HEADER

/* here is a global api struct pointer. while not strictly necessary,
   it's nice not to have to pass the api pointer in all function calls
   in the plugin */
//static const struct plugin_api* rb;
//MEM_FUNCTION_WRAPPERS(rb); /* gcc gives a weird error if omitted */
#define plug_QUIT BUTTON_POWER
#define EXT_UNKNOWN -1
#define EXT_l 0
#define EXT_m 1

void cleanup(void *parameter){
    (void)parameter;
    rb->backlight_set_timeout(rb->global_settings->backlight_timeout);
}

static int getExt(const char *filename){   
    /* requires additional extention checking here */
    switch (filename[rb->strlen(filename)-1]){
        /*html*/
        case 'l': return EXT_l;
        /*htm*/
        case 'm': return EXT_m;
        /*anything else*/
        default: return EXT_UNKNOWN;
    }
    return EXT_UNKNOWN; /*error*/
}

int tag(int IN, int OUT){
    int inc=0;
    int dec=0;
    int size_of=1;
    char buff[1];

    dec=rb->read(IN, &buff[0], size_of);
    inc=inc+dec;
    if(dec==0){
        return inc;
    }

    /* tag formatting here */
    /* looking for <br> and <p> tags also get <b> by default*/
    if((buff[0]=='b') || (buff[0]=='B')){
        //check for r
        dec=rb->read(IN, &buff[0], size_of);
        inc=inc+dec;
        if(dec==0){
            return inc;
        }
        if((buff[0]=='r') || (buff[0]=='R')){
            rb->write(OUT, "\n", 1);
        }
        if((buff[0]=='>')){
            /* Some possible formatting for the <B> Bold tag */
            /* to much effort to look for the end tag </B> */
            /*
            rb->write(OUT, "\n", 1);
            rb->write(OUT, "*", 1);
            rb->write(OUT, "\n", 1);
            */
        }
    }else{
    /* some formatting for the <P> paragragh tag */
    if((buff[0]=='P') || (buff[0]=='p')){
            rb->write(OUT, "\n", 1);
            rb->write(OUT, "\n", 1);
            /* rb->write(OUT, " ", 1); */
        }
    }
    while (buff[0]!='>'){
        dec=rb->read(IN, &buff[0], size_of);
        inc=inc+dec;
        if(dec==0){
            return inc;
        }
        /* to deal with nested tags */
        /* this can have adverse affects on poorly tagged pages */
        /* a missed closing tag will leave the document missing characters */ 
        if(buff[0]=='<'){
            inc=inc+tag(IN, OUT);
        } 
    }
    return inc;
} 

int html_parse(const char *param, char *newparam){
    int f=0;
    int fd1=0;
    int dec=0;
    int inc=0;
    int skipit=0;
    int button;
    /* Make sure the \0 of the string doesn't go into the textfile
     * all text editors I tried had problems (except the rockbox text viewer) */
    const char target_header[0] = "";
    const char target_footer[0] = "";
    int size_of=1; 
    int size_of_file=0; 
    char buffheader[26];
    char buff[1];

    f = rb->open(param, O_RDONLY);
    if(f < 0){
        rb->splash(HZ*2, "Can't open file");
    }else{
        size_of_file=rb->filesize(f);
        button = rb->button_get(false);
        switch(button){
            case(plug_QUIT):
                cleanup(NULL);
                rb->close(f);
                rb->lcd_clear_display();
                return PLUGIN_OK;
                break;
            default:
                if (rb->default_event_handler_ex(button, cleanup, NULL)
                    == SYS_USB_CONNECTED)
                    return PLUGIN_USB_CONNECTED;
                break;
        }
        fd1 = rb->open(newparam, O_CREAT|O_WRONLY|O_TRUNC); 
        rb->lcd_puts(0,6," * FILE Name:");
        rb->lcd_puts_scroll(0,7, newparam);
        rb->lcd_update();
        /* build header */
        rb->strcpy(buffheader, target_header);
        rb->write(fd1, &buffheader[0], sizeof(target_header));
        while(inc<size_of_file){/* keep reading file */
            dec=rb->read(f, &buff[0], size_of);
            if(dec>0){
                inc=inc+dec;
                /* need to look for all the instances of &... (&nbsp, &amp, etc..) */
                /* another function for those. I need to look up HTML tagging info */

                /* if a tag is found */
                if(buff[0]=='<'){
                /* formatting is in function tag() */
                inc=inc+tag(f, fd1);
                }else{ 
                    /* skip extra blank spaces */
                    if(skipit>0 &&  buff[0]==' '){
                        /* skipit */
                    }else{
                        /* Remove all newlines */
                        /* Formatting will be done by HTML tag */
                        /* if(skipit>0 &&  buff[0]=='\n'){*/
                        if(buff[0]=='\n'){
                            rb->write(fd1, " ", 1);
                            /* skipit */
                        }else{
                            /* Remove all carriage returns */
                            if(buff[0]=='\r'){
                            /* skipit */
                            }else{
                                rb->write(fd1, &buff[0], 1);
                                skipit=0;
                            }
                        }
                        /* if(buff[0]==' ' || buff[0]=='\n'){ */
                        if(buff[0]==' '){
                        skipit++; 
                        }
                    }
                }
            }
        }
    }
    /* now the footer */
    rb->strcpy(buffheader, target_footer);
    rb->write(fd1, &buffheader[0],  sizeof(target_footer));
    rb->close(f);
    rb->close(fd1);
    if(fd1<0){
        rb->splash(HZ*2, "Unable to write file!");
    }

    rb->button_get_w_tmo(250);
    return 1;
}

/* this is the plugin entry point */
enum plugin_status plugin_start(const void* parameter)
{
    //rb = api;
    char newparameter[256];
    int button, h;
    bool prev_show_statusbar;

    prev_show_statusbar = rb->global_settings->statusbar;
    rb->global_settings->statusbar = false;
    if (!parameter){  
        rb->splash(HZ*2, "Select a file to convert and use Open With..."); 
    }else{
        rb->lcd_getstringsize("A",NULL,&h);
        rb->strcpy(newparameter, (char*)parameter);
        rb->lcd_puts(4,0," * HTML Parser *");
        rb->lcd_drawline(0,h+5,LCD_WIDTH-1,h+5);
        rb->lcd_drawline(0,LCD_HEIGHT-h-7,LCD_WIDTH-1,LCD_HEIGHT-h-7);
        rb->lcd_update();

        button = rb->button_get(false);
        switch(button){
            case(plug_QUIT):
                cleanup(NULL);
                rb->lcd_clear_display();
                return PLUGIN_OK;
                break;
            default:
                if (rb->default_event_handler_ex(button, cleanup, NULL)
                    == SYS_USB_CONNECTED)
                    return PLUGIN_USB_CONNECTED;
                break;
        }
        switch (getExt(parameter)){
            case EXT_l: /* html */
                rb->lcd_putsxy(0,(LCD_HEIGHT-h-5), "* Converting html to txt *");
                /* change ext to .txt */
                newparameter[rb->strlen(newparameter)-4]='t';
                newparameter[rb->strlen(newparameter)-3]='x';
                newparameter[rb->strlen(newparameter)-2]='t';
                newparameter[rb->strlen(newparameter)-1]='\0';
                /* function call here for conversion */
                html_parse(parameter, newparameter);
                break;
            case EXT_m: /* htm */
                rb->lcd_putsxy(0,(LCD_HEIGHT-h-5), "* Converting htm to txt *");
                /* change ext to .txt */
                newparameter[rb->strlen(newparameter)-3]='t';
                newparameter[rb->strlen(newparameter)-2]='x';
                newparameter[rb->strlen(newparameter)-1]='t';
                newparameter[rb->strlen(newparameter)-0]='\0';
                /* function call here for conversion */
                html_parse(parameter, newparameter);
                break;
            case EXT_UNKNOWN:
                rb->splash(HZ*2, "Select a (.htm) or (.html) file"); 
                return PLUGIN_OK;
        }
        /* I would like to start the viewer here with the new file name */
        rb->set_current_file(newparameter);
    }
    rb->global_settings->statusbar = prev_show_statusbar;
    return PLUGIN_OK;
}