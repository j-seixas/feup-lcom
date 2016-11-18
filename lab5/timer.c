#include <minix/syslib.h>
#include <minix/drivers.h>
#include <minix/com.h>
#include "i8254.h"
#include "timer.h"

int timer_subscribe_int(int *hook_id) {
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

int timer_unsubscribe_int(int *hook_id) {
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

int timer_test_int(unsigned long time) {
	unsigned int counter = 0;
	int r, ipc_status, irq_set, *hook_id;
	message msg;
	irq_set = timer_subscribe_int(hook_id);
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
	if (timer_unsubscribe_int(hook_id) != OK) {
		printf("Error in timer_unsubscribe_int()\n");
		return 1;
	}
	return 0;

}
