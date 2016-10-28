#include "keyboard.h"
#include "timer.h"
#include "i8254.h"
#include "i8042.h"

static unsigned int counter = 0;
unsigned long assembly();

void timer_int_handler() {
	counter++;

}

int kbd_test_scan(unsigned short ass) {

	int r, ipc_status, irq_set, counter = 0;
	message msg;

	irq_set = kbd_subscribe_int();
	if (irq_set == -1) {
		printf("Error in kbd_subscribe_int()\n");
		return 1;
	}
	unsigned long data;
	unsigned int bool = 0;

	while (data != ESC_BREAK) {

		if ((r = driver_receive(ANY, &msg, &ipc_status)) != 0) { /*Get a request message.*/
			printf("driver_receive failed with: %d", r);
			continue;
		}
		if (is_ipc_notify(ipc_status)) { /*received notification*/
			switch (_ENDPOINT_P(msg.m_source)) {
			case HARDWARE: /*hardware interrupt notification*/
				if (msg.NOTIFY_ARG & irq_set) { /*subscribed interrupt*/
					if (ass == 0)
						data = kbd_handler(); /*process it*/
					else
						data = assembly();
					if (data == -1)
						return 1;
					if (data == TWO_BYTES || bool) {
						if (!bool) {
							bool = 1;
						} else {
							if (kbd_make_or_break(data))
								printf("Breakcode: 0xE0%02X\n", data);
							else
								printf("Makecode: 0xE0%02X\n", data);
							bool = 0;
						}

					} else {
						if (kbd_make_or_break(data))
							printf("Breakcode: 0x%02X\n", data);
						else
							printf("Makecode: 0x%02X\n", data);
					}

					break;
				}
			default:
				break;
			}/*no other notifications expected: do nothing*/

		} else { /*received a    standard message, not a notification*/
			/*
			 no standard messages expected: do nothing
			 */}
	}
}

int kbd_test_leds(unsigned short n, unsigned short *leds) {
	counter = 0;
	int r, ipc_status, irq_set;
	message msg;
	kbd_subscribe_int();
	irq_set = timer_subscribe_int();
	if (irq_set == -1) {
		printf("Error in timer_subscribe_int()\n");
		return 1;
	}
	if (n < 0) {
		printf("Error: number of arguments can't be zero!\n");
		return 1;
	}

	while (counter / 60 < n) {

		if ((r = driver_receive(ANY, &msg, &ipc_status)) != 0) { /*Get a request message.*/
			printf("driver_receive failed with: %d", r);
			continue;
		}
		if (is_ipc_notify(ipc_status)) { /*received notification*/
			switch (_ENDPOINT_P(msg.m_source)) {
			case HARDWARE: /*hardware interrupt notification*/
				if (msg.NOTIFY_ARG & irq_set) { /*subscribed interrupt*/
					timer_int_handler(); /*process it*/
					if (counter % 60 == 0) {
						if (kbd_led_handler(KB_LED_CMD, BIT(leds[counter / n]))
								!= OK) {
							printf("Erro ao mudar LEDS!!\n");
							//return -1;

						}
					}
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

	}
	if (timer_unsubscribe_int() != OK || kbd_unsubscribe_int() != OK) {
		printf("Error in unsubscribe_int()\n");
		return 1;
	}

	return 0;
}

int kbd_test_timed_scan(unsigned short n) {
	if (n < 0) {
		printf("Error: time can't be negative\n");
		return 1;
	}

	counter = 0;
	int r, ipc_status, irq_set_kbd, irq_set_timer;
	message msg;
	irq_set_timer = timer_subscribe_int();
	irq_set_kbd = kbd_subscribe_int();
	if (irq_set_kbd == -1 || irq_set_timer == -1) {
		printf("Error in subscribe_int()\n");
		return 1;
	}

	unsigned long data;
	unsigned int bool = 0;

	while ((counter / 60 < n) && (data != ESC_BREAK)) {

		if ((r = driver_receive(ANY, &msg, &ipc_status)) != 0) { /*Get a request message.*/
			printf("driver_receive failed with: %d", r);
			continue;
		}
		if (is_ipc_notify(ipc_status)) { /*received notification*/
			switch (_ENDPOINT_P(msg.m_source)) {
			case HARDWARE: /*hardware interrupt notification*/
				if (msg.NOTIFY_ARG & irq_set_timer) { /*subscribed interrupt of timer*/
					timer_int_handler(); /*process it*/
				}
				if (msg.NOTIFY_ARG & irq_set_kbd) { /*subscribed interrupt of keyboard*/
					data = kbd_handler(); /*process it*/
					if (data == -1)
						return 1;
					if (data == TWO_BYTES || bool) {
						if (!bool) {
							bool = 1;
						} else {
							if (kbd_make_or_break(data))
								printf("Breakcode: 0xE0%02X\n", data);
							else
								printf("Makecode: 0xE0%02X\n", data);
							bool = 0;
						}

					} else {
						if (kbd_make_or_break(data))
							printf("Breakcode: 0x%02X\n", data);
						else
							printf("Makecode: 0x%02X\n", data);
					}
					counter = 0;
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

	}
	if (timer_unsubscribe_int() != OK) {
		printf("Error in timer_unsubscribe_int()\n");
		return 1;
	}
	if (kbd_unsubscribe_int() != OK) {
		printf("Error in kbd_unsubscribe_int()\n");
		return 1;
	}
	return 0;

}
