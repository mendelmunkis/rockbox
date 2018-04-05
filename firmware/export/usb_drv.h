/***************************************************************************
 *             __________               __   ___.
 *   Open      \______   \ ____   ____ |  | _\_ |__   _______  ___
 *   Source     |       _//  _ \_/ ___\|  |/ /| __ \ /  _ \  \/  /
 *   Jukebox    |    |   (  <_> )  \___|    < | \_\ (  <_> > <  <
 *   Firmware   |____|_  /\____/ \___  >__|_ \|___  /\____/__/\_ \
 *                     \/            \/     \/    \/            \/
 * $Id$
 *
 * Copyright (C) 2007 by Björn Stenberg
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
#ifndef _USB_DRV_H
#define _USB_DRV_H
#include "usb_ch9.h"
#include "kernel.h"

void usb_drv_startup(void);
void usb_drv_int_enable(bool enable); /* Target implemented */
void usb_drv_init(void);
void usb_drv_exit(void);
void usb_drv_int(void); /* Call from target INT handler */
void usb_drv_stall(int endpoint, bool stall,bool in);
bool usb_drv_stalled(int endpoint,bool in);
int usb_drv_send(int endpoint, void* ptr, int length);
int usb_drv_send_nonblocking(int endpoint, void* ptr, int length);
int usb_drv_recv(int endpoint, void* ptr, int length);
int usb_drv_recv_blocking(int endpoint, void* ptr, int length);
void usb_drv_ack(struct usb_ctrlrequest* req);
void usb_drv_set_address(int address);
void usb_drv_reset_endpoint(int endpoint, bool send);
bool usb_drv_powered(void);
int usb_drv_port_speed(void);
void usb_drv_cancel_all_transfers(void);
void usb_drv_set_test_mode(int mode);
bool usb_drv_connected(void);
int usb_drv_request_endpoint(int type, int dir);
void usb_drv_release_endpoint(int ep);
#ifdef USB_HAS_ISOCHRONOUS
/* returns the last received frame number (the 11-bit number contained in the last SOF):
 * - full-speed: the host sends one SOF every 1ms (so 1000 SOF/s)
 * - high-speed: the hosts sends one SOF every 125us but each consecutive 8 SOF have the same frame
 *   number
 * thus in all mode, the frame number can be interpreted as the current millisecond *in USB time*. */
int usb_drv_get_frame_number(void);
#endif

#endif /* _USB_DRV_H */
