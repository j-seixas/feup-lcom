#include "keyboard.h"
#define WAITTIME = 5;

int kbd_test_scan(unsigned short ass) {
	if (asm != 0 || asm != 1) {
		return 1;
	}
	if (asm == 0) {
		int r, ipc_status, irq_set, counter = 0;
		message msg;
		irq_set = kbd_subscribe_int();
		if (irq_set == -1) {
			printf("Error in kbd_subscribe_inta()\n");
			return 1;
		}
		/*if (time < 0) {
			printf("Error: time can't be negative\n");
			return 1;
		}*/

		while (counter < WAITTIME) {

			if ((r = driver_receive(ANY, &msg, &ipc_status)) != 0) { /*Get a request message.*/
				printf("driver_receive failed with: %d", r);
				continue;
			}
			if (is_ipc_notify(ipc_status)) { /*received notification*/
				switch (_ENDPOINT_P(msg.m_source)) {
				case HARDWARE: /*hardware interrupt notification*/
					if (msg.NOTIFY_ARG & irq_set) { /*subscribed interrupt*/
						int data = kbd_handler(); /*process it*/
						if (data == -1)
							break;


					}
					break;
				default:
					break; /*no other notifications expected: do nothing*/
				}
			} else { /*received a standard message, not a notification*/
				/*
				 no standard messages expected: do nothing
				 */
			}
			counter++;

		}
		if (kbd_unsubscribe_int() != OK) {
			printf("Error in kbd_unsubscribe_int()\n");
			return 1;
		}
		return 0;
	}
	/* To be completed */
}
int kbd_test_leds(unsigned short n, unsigned short *leds) {
	/* To be completed */
}
int kbd_test_timed_scan(unsigned short n) {
	/* To be completed */
}
