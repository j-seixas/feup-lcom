#include "test4.h"

#define DELAY_US    20000
int hook_id_mouse, hook_id_timer;
static unsigned int three = 0;

int timer_subscribe_int(void) {
	hook_id_timer = TM0_IRQSET;
	if (sys_irqsetpolicy(TIMER0_IRQ, IRQ_REENABLE, &hook_id_timer) != OK) {
		printf("Error in sys_irqsetpolicy()\n");
		return -1;
	}
	if (sys_irqenable(&hook_id_timer) != OK) {
		printf("Error in sys_irqenable()\n");
		return -1;
	}

	return BIT(TM0_IRQSET);

}

int timer_unsubscribe_int() {
	if (sys_irqdisable(&hook_id_timer) != OK) {
		printf("Error in sys_irqdisable()\n");
		return 1;
	}
	if (sys_irqrmpolicy(&hook_id_timer) != OK) {
		printf("Error in sys_irqrmpolicy()\n");
		return 1;
	}

	return 0;
}

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
			data1 = kbd_read();
			if (kbd_send(cmd) == 0) {
				data2 = kbd_read();
				if (data2 == KB_ACK)
					return 0;
			}
		}
		count--;
	}

	return 1;

}

int test_packet(unsigned short cnt) {

	int r, ipc_status, irq_set;
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

	unsigned long data;

	while (cnt > 0) {
		if ((r = driver_receive(ANY, &msg, &ipc_status)) != 0) { /*Get a request message.*/
			printf("driver_receive failed with: %d", r);
			continue;
		}
		if (is_ipc_notify(ipc_status)) { /*received notification*/
			switch (_ENDPOINT_P(msg.m_source)) {
			case HARDWARE: /*hardware interrupt notification*/
				if (msg.NOTIFY_ARG & irq_set) { /*subscribed interrupt*/
					data = kbd_read(); /*process it*/
					if (data == -1) {
						break;
					}
					if (three == 0) {
						if ((data & BIT(3)) == OK) {
							break;
						}
					}
					packet[three] = data;
					three++;
					break;
				}
			default:
				break;
			}/*no other notifications expected: do nothing*/

		} else { /*received a    standard message, not a notification*/
			/*
			 no standard messages expected: do nothing
			 */}

		if (three == 3) {
			printf("B1=0x%02X B2=0x%02X B3=0x%02X ", packet[0], packet[1],
					packet[2]);
			if ((packet[0] & BIT(0)) != 0)
				printf("LB=1 ");
			else
				printf("LB=0 ");
			if ((packet[0] & BIT(2)) != 0)
				printf("MB=1 ");
			else
				printf("MB=0 ");
			if ((packet[0] & BIT(1)) != 0)
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
				printf("X=-%d ", packet[1]);
			} else
				printf("X=%d ", packet[1]);
			if ((packet[0] & BIT(5)) != 0) {
				packet[2] ^= 0xFF;
				packet[2]++;
				printf("Y=-%d \n", packet[2]);
			} else
				printf("Y=%d \n", packet[2]);

			three = 0;
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
	int r, ipc_status, irq_set, irq_set_timer;
	message msg;
	unsigned long packet[3];
	unsigned int counter = 0;
	irq_set = mouse_subscribe_int();
	irq_set_timer = timer_subscribe_int();

	if (irq_set == -1) {
		printf("Error in mouse_subscribe_int()\n");
		return 1;
	}
	if (irq_set_timer == -1) {
		printf("Error in timer_subscribe_int()\n");
		return 1;
	}
	kbd_read();
	if (mouse_send(MOUSE_ENB) != 0) {
		printf("Error in mouse sending command\n");
		return 1;
	}

	unsigned long data;

	while (counter / 60 < idle_time) {
		if ((r = driver_receive(ANY, &msg, &ipc_status)) != 0) /*Get a request message.*/
		{
			printf("driver_receive failed with: %d", r);
			continue;
		}

		if (is_ipc_notify(ipc_status)) { /*received notification*/
			switch (_ENDPOINT_P(msg.m_source)) {
			case HARDWARE: /*hardware interrupt notification*/
				if (msg.NOTIFY_ARG & irq_set_timer) /*subscribed interrupt*/
					counter++;

				if (is_ipc_notify(ipc_status)) { /*received notification*/
					switch (_ENDPOINT_P(msg.m_source)) {
					case HARDWARE: /*hardware interrupt notification*/
						if (msg.NOTIFY_ARG & irq_set) { /*subscribed interrupt*/
							data = kbd_read(); /*process it*/
							if (data == -1) {
								break;
							}
							counter = 0;
							if (three == 0) {
								if ((data & BIT(3)) == OK) {
									break;
								}

							}
							packet[three] = data;
							three++;
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
		if (three == 3) {
			printf("B1=0x%02X B2=0x%02X B3=0x%02X ", packet[0], packet[1],
					packet[2]);
			if ((packet[0] & BIT(0)) != 0)
				printf("LB=1 ");
			else
				printf("LB=0 ");
			if ((packet[0] & BIT(2)) != 0)
				printf("MB=1 ");
			else
				printf("MB=0 ");
			if ((packet[0] & BIT(1)) != 0)
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
				printf("X=-%d ", packet[1]);
			} else
				printf("X=%d ", packet[1]);
			if ((packet[0] & BIT(5)) != 0) {
				packet[2] ^= 0xFF;
				packet[2]++;
				printf("Y=-%d \n", packet[2]);
			} else
				printf("Y=%d \n", packet[2]);
			three = 0;
		}
	}
	if (timer_unsubscribe_int() != OK) {
		printf("Error in mouse_unsubscribe_int()\n");
		return 1;
	}
	if (mouse_unsubscribe_int() != OK) {
		printf("Error in mouse_unsubscribe_int()\n");
		return 1;
	}
	return 0;

}

int test_config(void) {
	int irq_set;
	unsigned long packet[3], data;
	irq_set = mouse_subscribe_int();
	if (irq_set == -1) {
		printf("Error in mouse_subscribe_int()\n");
		return 1;
	}
	kbd_read();
	if (mouse_send(MOUSE_CONF) != 0) {
		printf("Error in mouse sending command\n");
		return 1;
	}
	three = 0;
	while (three != 3) {
		data = kbd_read(); /*process it*/
		if (data == -1) {
			printf("Error in packets received in test_config()\n");
			return 1;
		}
		if (three == 0) {
			if (((data & BIT(3)) != OK) || ((data & BIT(7)) != OK)) {
				printf("Error in packet 1 received in test_config()\n");
				return 1;
			}

		} else if (three == 1) {
			if (((data & BIT(2)) != OK) || ((data & BIT(3)) != OK)
					|| ((data & BIT(4)) != OK) || ((data & BIT(5)) != OK)
					|| ((data & BIT(6)) != OK) || ((data & BIT(7)) != OK)) {
				printf("Error in packet 2 received in test_config()\n");
				return 1;
			}
		}
		packet[three] = data;
		three++;
	}
	if (three == 3) {
		if ((packet[0] & BIT(6)) != 0)
			printf("Remote (polled) Mode\n");
		else
			printf("Stream Mode\n");
		if ((packet[0] & BIT(5)) != 0)
			printf("Data Reporting: Enabled\n");
		else
			printf("Data Reporting: Disabled\n");
		if ((packet[0] & BIT(4)) != 0)
			printf("Scaling: 2:1\n");
		else
			printf("Scaling: 1:1\n");
		if ((packet[0] & BIT(0)) != 0)
			printf("Left button: Currently Pressed\n");
		else
			printf("Left button: Currently Released\n");
		if ((packet[0] & BIT(2)) != 0)
			printf("Middle button: Currently Pressed\n");
		else
			printf("Middle button: Currently Released\n");
		if ((packet[0] & BIT(1)) != 0)
			printf("Right button: Currently Pressed\n");
		else
			printf("Right button: Currently Released\n");

		if ((packet[1] & BIT(0)) != 0)
			printf("Resolution: 2 counts per mm\n");
		else if ((packet[1] & BIT(1)) != 0)
			printf("Resolution: 4 counts per mm\n");
		else if ((packet[1] & (BIT(0) | BIT(1))) == 0)
			printf("Resolution: 1 counts per mm\n");
		else
			printf("Resolution: 8 counts per mm\n");
		printf("Sample Rate: %d\n", packet[2]);
		three = 0;
		if (mouse_unsubscribe_int() != OK) {
			printf("Error in mouse_unsubscribe_int()\n");
			return 1;
		}
		return 0;
	}
	if (mouse_unsubscribe_int() != OK)
		printf("Error in mouse_unsubscribe_int()\n");
	return 1;

}



int test_gesture(short length) {
	event_t evt;
	evt.type = RUP;
	evt.state = INIT;
	unsigned int negative;
	if (length < 0)
		negative = 1;
	else
		negative = 0;

	int r, ipc_status, irq_set;
	message msg;
	unsigned long packet[3];
	long test, testx;

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

	unsigned long data;

	while (evt.state != COMP) {
		if ((r = driver_receive(ANY, &msg, &ipc_status)) != 0) { /*Get a request message.*/
			printf("driver_receive failed with: %d", r);
			continue;
		}
		if (is_ipc_notify(ipc_status)) { /*received notification*/
			switch (_ENDPOINT_P(msg.m_source)) {
			case HARDWARE: /*hardware interrupt notification*/
				if (msg.NOTIFY_ARG & irq_set) { /*subscribed interrupt*/
					data = kbd_read(); /*process it*/
					if (data == -1) {
						break;
					}
					if (three == 0) {
						if ((data & BIT(3)) == OK) {
							break;
						}
					}
					packet[three] = data;
					three++;
					break;
				}
			default:
				break;
			}/*no other notifications expected: do nothing*/

		} else { /*received a    standard message, not a notification*/
			/*
			 no standard messages expected: do nothing
			 */}

		if (three == 3) {
			printf("B1=0x%02X B2=0x%02X B3=0x%02X ", packet[0], packet[1],
					packet[2]);
			if ((packet[0] & BIT(0)) != 0)
				printf("LB=1 ");
			else
				printf("LB=0 ");
			if ((packet[0] & BIT(2)) != 0)
				printf("MB=1 ");
			else
				printf("MB=0 ");
			if ((packet[0] & BIT(1)) != 0) {
				printf("RB=1 ");
				if (evt.state == INIT)
					evt.type = RDOWN;
				else
					evt.type = MOVE;
			} else {
				printf("RB=0 ");
				evt.state = INIT;
				evt.type = RUP;
			}
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
				printf("X=-%d ", packet[1]);
				testx = -packet[1];
			} else {
				printf("X=%d ", packet[1]);
				testx = packet[1];

			}
			if ((packet[0] & BIT(5)) != 0) {
				packet[2] ^= 0xFF;
				packet[2]++;
				test = -packet[2];
				printf("Y=-%d\n", packet[2]);

			} else {
				printf("Y=%d \n", packet[2]);
				test = packet[2];

			}
			check_state(&evt, negative, testx, test, length);
			three = 0;
		}

	}
	if (mouse_unsubscribe_int() != OK) {
		printf("Error in mouse_unsubscribe_int()\n");
		return 1;
	}
	return 0;
}

void check_state(event_t *evt, unsigned int negative, long deltax, long deltay, short length) {
	switch (evt->state) {
	case INIT:
		if (evt->type == RDOWN)
			evt->state = DRAW;
		break;
	case DRAW:
		if (evt->type == MOVE) {
			if (negative == 1 && deltay <= length && deltax < 0)
				evt->state = COMP;
			else if (negative == 0 && deltay >= length && deltax > 0)
				evt->state = COMP;

		} else if (evt->type == RUP)
			evt->state = INIT;
		break;
	default:
		break;
	}

}

