#ifndef __KEYBOARD_H
#define __KEYBOARD_H

#include <minix/sysutil.h>
#include <minix/syslib.h>
#include <minix/drivers.h>
#include <minix/com.h>



int kbd_subscribe_int(void);
int kbd_unsubscribe_int();
int kbd_make_or_break(unsigned long data);
unsigned long kbd_handler();
int kbd_ACK(unsigned long cmd);
int kbd_led_handler(unsigned long cmd1, unsigned long cmd2) ;
int kbd_send_command(unsigned long cmd);


#endif /* __KEYBOARD_H */
