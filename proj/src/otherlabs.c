#include <minix/syslib.h>
#include <minix/drivers.h>
#include <minix/com.h>
#include <minix/sysutil.h>
#include "tools.h"
#include "otherlabs.h"

#define DELAY_US    20000
//static int hook_id, hook_id_kbd;

int kbd_subscribe_int(void) {
	game1.hook_id_kbd = KB_IRQ;
	if (sys_irqsetpolicy(KEYBOARD_IRQ, IRQ_REENABLE | IRQ_EXCLUSIVE,
			&game1.hook_id_kbd) != OK) {
		printf("Error in sys_irqsetpolicy()\n");
		return -1;
	}
	if (sys_irqenable(&game1.hook_id_kbd) != OK) {
		printf("Error in sys_irqenable()\n");
		return -1;
	}
	return BIT(KB_IRQ);

}

int kbd_unsubscribe_int() {
	if (sys_irqdisable(&game1.hook_id_kbd) != OK) {
		printf("Error in sys_irqdisable()\n");
		return 1;
	}
	if (sys_irqrmpolicy(&game1.hook_id_kbd) != OK) {
		printf("Error in sys_irqrmpolicy()\n");
		return 1;
	}

	return 0;
}

int timer_subscribe_int(void) {
	game1.hook_id_timer = TM0_IRQSET;
	if (sys_irqsetpolicy(TIMER0_IRQ, IRQ_REENABLE, &game1.hook_id_timer)
			!= OK) {
		printf("Error in sys_irqsetpolicy()\n");
		return -1;
	}
	if (sys_irqenable(&game1.hook_id_timer) != OK) {
		printf("Error in sys_irqenable()\n");
		return -1;
	}

	return BIT(TM0_IRQSET);

}

int timer_unsubscribe_int() {
	if (sys_irqdisable(&game1.hook_id_timer) != OK) {
		printf("Error in sys_irqdisable()\n");
		return 1;
	}
	if (sys_irqrmpolicy(&game1.hook_id_timer) != OK) {
		printf("Error in sys_irqrmpolicy()\n");
		return 1;
	}

	return 0;
}

int mouse_subscribe_int(void) {
	game1.hook_id_mouse = MOUSE_IRQ;
	if (sys_irqsetpolicy(MOUSE_IRQ, IRQ_REENABLE | IRQ_EXCLUSIVE,
			&game1.hook_id_mouse) != OK) {
		printf("Error in sys_irqsetpolicy()\n");
		return -1;
	}
	if (sys_irqenable(&game1.hook_id_mouse) != OK) {
		printf("Error in sys_irqenable()\n");
		return -1;
	}
	return BIT(MOUSE_IRQ);

}

int mouse_unsubscribe_int() {
	if (sys_irqdisable(&game1.hook_id_mouse) != OK) {
		printf("Error in sys_irqdisable()\n");
		return 1;
	}
	if (sys_irqrmpolicy(&game1.hook_id_mouse) != OK) {
		printf("Error in sys_irqrmpolicy()\n");
		return 1;
	}

	return 0;

}


int unsub_game() {
	game1.irq_set_kbd = kbd_unsubscribe_int();
	if (game1.irq_set_kbd == -1) {
		printf("Error in kbd_unsubscribe_int()\n");
		return 1;
	}
	game1.irq_set_timer = timer_unsubscribe_int();
	if (game1.irq_set_timer == -1) {
		printf("Error in timer_unsubscribe_int()\n");
		return 1;
	}
	game1.irq_set_mouse = mouse_unsubscribe_int();
	if (game1.irq_set_mouse == -1) {
		printf("Error in timer_unsubscribe_int()\n");
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

int kbd_send_command(unsigned long cmd) {
	unsigned long stat, data;
	int var = 6;

	while (var > 0) {
		sys_inb(STATUS_PORT, &stat);
		//assuming it returns OK
		//loop while 8042 input buffer is not empty
		if ((stat & IBF) == 0) {
			sys_outb(KBC_CMD_REG, cmd);
			return 0;
		}
		tickdelay(micros_to_ticks(DELAY_US));
		var--;
	}
	return -1;
}

int kbd_send(unsigned long cmd) {
	unsigned long stat, data;
	int var = 6;

	while (var > 0) {
		sys_inb(STATUS_PORT, &stat);
		//assuming it returns OK
		//loop while 8042 input buffer is not empty
		if ((stat & IBF) == 0) {
			sys_outb(KBD_DATA_BUF, cmd);
			return 0;
		}
		tickdelay(micros_to_ticks(DELAY_US));
		var--;
	}
	return -1;
}

int mouse_send(unsigned long cmd) {
	int count = 5;
	while (count > 0) {
		unsigned long data1, data2;
		if (kbd_send_command(MOUSE_SEND) == 0) {
			data1 = kbd_handler();
			if (kbd_send(cmd) == 0) {
				data2 = kbd_handler();
				if (data2 == KB_ACK)
					return 0;
			}
		}
		count--;
	}

	return 1;

}
//
//int kbd_test_scan() {
//	int r, ipc_status, counter = 0;
//	message msg;
//
//	int irq_set = kbd_subscribe_int();
//	if (irq_set == -1) {
//		printf("Error in kbd_subscribe_int()\n");
//		return 1;
//	}
//	unsigned long data;
//
//	while (data != ESC_BREAK) {
//		printf("Over here\n");
//		if ((r = driver_receive(ANY, &msg, &ipc_status)) != 0) { /*Get a request message.*/
//			printf("driver_receive failed with: %d", r);
//			continue;
//		}
//		if (is_ipc_notify(ipc_status)) { /*received notification*/
//			switch (_ENDPOINT_P(msg.m_source)) {
//			case HARDWARE: /*hardware interrupt notification*/
//				if (msg.NOTIFY_ARG & game1.irq_set_kbd) { /*subscribed interrupt*/
//					data = kbd_handler(); /*process it*/
//					break;
//				}
//			default:
//				break;
//			}
//
//		} else {
//
//		}
//	}
//
//	if (kbd_unsubscribe_int() != OK) {
//		printf("Error in kbd_unsubscribe_int()\n");
//		return 1;
//	}
//
//	printf("--------\n");
//	return 0;
//}
//
//int timer_test_int(unsigned long time) {
//	unsigned int counter = 0;
//	int r, ipc_status;
//	message msg;
//	int irq_set = timer_subscribe_int();
//	if (irq_set == -1) {
//		printf("Error in timer_subscribe_int()\n");
//		return 1;
//	}
//	if (time < 0) {
//		printf("Error: time can't be negative\n");
//		return 1;
//	}
//
//	while (counter / 60 < time) {
//
//		if ((r = driver_receive(ANY, &msg, &ipc_status)) != 0) { /*Get a request message.*/
//			printf("driver_receive failed with: %d", r);
//			continue;
//		}
//		if (is_ipc_notify(ipc_status)) { /*received notification*/
//			switch (_ENDPOINT_P(msg.m_source)) {
//			case HARDWARE: /*hardware interrupt notification*/
//				if (msg.NOTIFY_ARG & game1.irq_set_timer) { /*subscribed interrupt*/
//					counter++; /*process it*/
//				}
//				break;
//			default:
//				break; /*no other notifications expected: do nothing*/
//			}
//		} else { /*received a standard message, not a notification*/
//			/*
//			 no standard messages expected: do nothing
//			 */
//		}
//
//	}
//
//	if (timer_unsubscribe_int() != OK) {
//		printf("Error in timer_unsubscribe_int()\n");
//		return 1;
//	}
//	return 0;
//
//}
