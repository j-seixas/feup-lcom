#include <minix/syslib.h>
#include <minix/drivers.h>
#include <minix/com.h>
#include <minix/sysutil.h>
#include "i8254.h"
#include "i8042.h"
#include "otherlabs.h"

#define DELAY_US    20000
static int hook_id, hook_id_kbd;

int kbd_subscribe_int(void) {
	hook_id_kbd = KB_IRQ;
	if (sys_irqsetpolicy(KB_IRQ, IRQ_REENABLE | IRQ_EXCLUSIVE, &hook_id_kbd)
			!= OK) {
		printf("Error in sys_irqsetpolicy()\n");
		return -1;
	}
	if (sys_irqenable(&hook_id_kbd) != OK) {
		printf("Error in sys_irqenable()\n");
		return -1;
	}
	return BIT(KB_IRQ);

}

int kbd_unsubscribe_int() {
	if (sys_irqdisable(&hook_id_kbd) != OK) {
		printf("Error in sys_irqdisable()\n");
		return 1;
	}
	if (sys_irqrmpolicy(&hook_id_kbd) != OK) {
		printf("Error in sys_irqrmpolicy()\n");
		return 1;
	}

	return 0;
}

int timer_subscribe_int(void) {
	hook_id = TM0_IRQSET;
	if (sys_irqsetpolicy(TIMER0_IRQ, IRQ_REENABLE, &hook_id) != OK) {
		printf("Error in sys_irqsetpolicy()\n");
		return -1;
	}
	if (sys_irqenable(&hook_id) != OK) {
		printf("Error in sys_irqenable()\n");
		return -1;
	}

	return BIT(TM0_IRQSET);

}

int timer_unsubscribe_int() {
	if (sys_irqdisable(&hook_id) != OK) {
		printf("Error in sys_irqdisable()\n");
		return 1;
	}
	if (sys_irqrmpolicy(&hook_id) != OK) {
		printf("Error in sys_irqrmpolicy()\n");
		return 1;
	}

	return 0;
}

unsigned long kbd_handler() {
	unsigned long stat, data;
	int var = 6;
	while (var > 0) {
		sys_inb(STATUS_PORT, &stat); /*assuming it returns OK*/
		/*loop while 8042 output buffer is empty*/

		if (stat & OBF) {
			sys_inb(KBD_OUT_BUF, &data); /*	assuming it returns OK	*/
			if ((stat & (PAR_ERR | TO_ERR)) == 0)
				return data;
			else {
				return -1;
			}
		}
		tickdelay(micros_to_ticks(DELAY_US));
		var--;
	}
	return -1;
}

int kbd_test_scan() {
	int r, ipc_status, irq_set, counter = 0;
	message msg;

	irq_set = kbd_subscribe_int();
	if (irq_set == -1) {
		printf("Error in kbd_subscribe_int()\n");
		return 1;
	}
	unsigned long data;
	//unsigned int bool = 0;

	while (data != ESC_BREAK /*&& data != ESC_MAKE*/) {

		if ((r = driver_receive(ANY, &msg, &ipc_status)) != 0) { /*Get a request message.*/
			printf("driver_receive failed with: %d", r);
			continue;
		}
		if (is_ipc_notify(ipc_status)) { /*received notification*/
			switch (_ENDPOINT_P(msg.m_source)) {
			case HARDWARE: /*hardware interrupt notification*/
				if (msg.NOTIFY_ARG & irq_set) { /*subscribed interrupt*/
					data = kbd_handler(); /*process it*/
					break;
				}
			default:
				break;
			}

		} else {

		}
	}
	if (kbd_unsubscribe_int() != OK) {
		printf("Error in kbd_unsubscribe_int()\n");
		return 1;
	}
	return 0;
}

int timer_test_int(unsigned long time) {
	unsigned int counter = 0;
	int r, ipc_status, irq_set;
	message msg;
	irq_set = timer_subscribe_int();
	if (irq_set == -1) {
		printf("Error in timer_subscribe_int()\n");
		return 1;
	}
	if (time < 0) {
		printf("Error: time can't be negative\n");
		return 1;
	}

	while (counter / 60 < time) {

		if ((r = driver_receive(ANY, &msg, &ipc_status)) != 0) { /*Get a request message.*/
			printf("driver_receive failed with: %d", r);
			continue;
		}
		if (is_ipc_notify(ipc_status)) { /*received notification*/
			switch (_ENDPOINT_P(msg.m_source)) {
			case HARDWARE: /*hardware interrupt notification*/
				if (msg.NOTIFY_ARG & irq_set) { /*subscribed interrupt*/
					counter++; /*process it*/
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
	return 0;

}
