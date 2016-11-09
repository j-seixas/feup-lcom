#include "test4.h"

#define DELAY_US    20000
int hook_id_mouse;

int mouse_subscribe_int(void) {
	hook_id_mouse = MOUSE_IRQ;
	if (sys_irqsetpolicy(MOUSE_IRQ, IRQ_REENABLE | IRQ_EXCLUSIVE,
			&hook_id_mouse) != OK) {
		printf("Error in sys_irqsetpolicy()\n");
		return -1;
	}
	if (sys_irqenable(&hook_id_mouse) != OK) {
		printf("Error in sys_irqenable()\n");
		return -1;
	}
	return BIT(MOUSE_IRQ);

}

int mouse_unsubscribe_int() {
	if (sys_irqdisable(&hook_id_mouse) != OK) {
		printf("Error in sys_irqdisable()\n");
		return 1;
	}
	if (sys_irqrmpolicy(&hook_id_mouse) != OK) {
		printf("Error in sys_irqrmpolicy()\n");
		return 1;
	}

	return 0;

}

unsigned long kbd_read() {
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
			//no args command
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
			//no args command
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
			printf("Entrou na 1\n");
			data1 = kbd_read();
			printf("data1: %X\n", data1);

			printf("Entrou na 2\n");
			if (kbd_send(cmd) == 0) {
				printf("Entrou na 3\n");
				data2 = kbd_read();
				if (data2 == KB_ACK)
					return 0;
				//else
				//return 1;
			}
			//return -1;

			//return 1;
		}
		//return -1;
		count--;
	}

	return 1;

}

int test_packet(unsigned short cnt) {

	int r, ipc_status, irq_set, counter = 0;
	message msg;
	unsigned long packet[3];

	irq_set = mouse_subscribe_int();
	if (irq_set == -1) {
		printf("Error in mouse_subscribe_int()\n");
		return 1;
	}
	kbd_read();
	if (mouse_send(MOUSE_ENB) != 0) {
		printf("Error in mouse sending command\n");
		return 1;
	}

	printf("Vai entrar no loop\n");
	unsigned long data;
	unsigned int loop = 0;

	while (cnt > 0) {
		loop = 0;
		while (loop < 3) {
			if ((r = driver_receive(ANY, &msg, &ipc_status)) != 0) { /*Get a request message.*/
				printf("driver_receive failed with: %d", r);
				continue;
			}
			if (is_ipc_notify(ipc_status)) { /*received notification*/
				switch (_ENDPOINT_P(msg.m_source)) {
				case HARDWARE: /*hardware interrupt notification*/
					if (msg.NOTIFY_ARG & irq_set) { /*subscribed interrupt*/
						//printf("1 interrupt\n");
						data = kbd_read(); /*process it*/
						if (data == -1) {
							//printf("erro\n");
							break;
						}
						if (loop == 0) {
							if ((data & BIT(3)) == OK) {
								loop = 5;
								break;
							}

						}
						packet[loop] = data;
						loop++;
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
		if (loop != 6) {
			printf("B1=0x%X B2=0x%X B3=0x%X ", packet[0], packet[1], packet[2]);
			if ((packet[0] & BIT(0)) != 0)
				printf("LB=1 ");
			else
				printf("LB=0 ");
			if ((packet[0] & BIT(1)) != 0)
				printf("MB=1 ");
			else
				printf("MB=0 ");
			if ((packet[0] & BIT(2)) != 0)
				printf("RB=1 ");
			else
				printf("RB=0 ");
			if ((packet[0] & BIT(6)) != 0)
				printf("XOV=1 ");
			else
				printf("XOV=0 ");
			if ((packet[0] & BIT(7)) != 0)
				printf("YOV=1 ");
			else
				printf("YOV=0 ");
			if ((packet[0] & BIT(4)) != 0) {
				packet[1] ^= 0xFF;
				packet[1]++;
				printf("X=%d ", packet[1]);
			} else
				printf("X=%d ", packet[1]);
			if ((packet[0] & BIT(5)) != 0) {
				packet[2] ^= 0xFF;
				packet[2]++;
				printf("Y=%d \n", packet[2]);
			} else
				printf("Y=%d \n", packet[2]);
			cnt--;
		}
	}
	if (mouse_unsubscribe_int() != OK) {
		printf("Error in mouse_unsubscribe_int()\n");
		return 1;
	}

	return 0;

}

int test_async(unsigned short idle_time) {
	/* To be completed ... */
}

int test_config(void) {
	/* To be completed ... */
}

int test_gesture(short length) {
	/* To be completed ... */
}
