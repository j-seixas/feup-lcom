#include <minix/syslib.h>
#include <minix/drivers.h>
#include <minix/com.h>
#include "i8254.h"
#include "timer.h"

static unsigned int counter = 0;
int hook_id;

int timer_set_square(unsigned long timer, unsigned long freq) {
	if (timer < 0 || timer > 2 || freq == 0) {
		printf("Error: timer as to be 0, 1 or 2 OR freq can't be 0\n");
		return 1;
	}

	int div = TIMER_FREQ / freq;
	unsigned char timer_lsb = div;
	unsigned char timer_msb = div >> 8;

	if (timer == 0) {
		if (sys_outb(TIMER_CTRL,
				TIMER_LSB_MSB | TIMER_SEL0 | TIMER_BIN | TIMER_SQR_WAVE)
				!= OK) {
			printf("Error in sys_outb()\n");
			return 1;
		}
		if (sys_outb(TIMER_0, timer_lsb) != OK) {
			printf("Error in sys_outb()\n");
			return 1;
		}

		if (sys_outb(TIMER_0, timer_msb) != OK) {
			printf("Error in sys_outb()\n");
			return 1;
		}

		return 0;

	} else if (timer == 1) {
		if (sys_outb(TIMER_CTRL,
				TIMER_LSB_MSB | TIMER_SEL1 | TIMER_BIN | TIMER_SQR_WAVE)
				!= OK) {
			printf("Error in sys_outb()\n");
			return 1;
		}
		if (sys_outb(TIMER_1, timer_lsb) != OK) {
			printf("Error in sys_outb()\n");
			return 1;
		}
		if (sys_outb(TIMER_1, timer_msb) != OK) {
			printf("Error in sys_outb()\n");
			return 1;
		}

		return 0;

	} else if (timer == 2) {
		if (sys_outb(TIMER_CTRL,
				TIMER_LSB_MSB | TIMER_SEL2 | TIMER_BIN | TIMER_SQR_WAVE)
				!= OK) {
			printf("Error in sys_outb()\n");
			return 1;
		}
		if (sys_outb(TIMER_2, timer_lsb) != OK) {
			printf("Error in sys_outb()\n");
			return 1;
		}
		if (sys_outb(TIMER_2, timer_msb) != OK) {
			printf("Error in sys_outb()\n");
			return 1;
		}
		return 0;

	} else
		return 1;

	return 1;
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

void timer_int_handler() {
	counter++;

}

int timer_get_conf(unsigned long timer, unsigned char *st) {

	if (timer > 2 || timer < 0)
		return 1;
	else {
		switch (timer) {
		case 0:
			if (sys_outb(TIMER_CTRL,
					TIMER_RB_CMD | TIMER_RB_COUNT_ | TIMER_RB_SEL(timer))
					!= OK) {
				printf("Error in sys_outb()\n");
				return 1;
			}
			if (sys_inb(TIMER_0, (unsigned long *) st) != OK) {
				printf("Error in sys_inb()\n");
				return 1;
			}

			return 0;
		case 1:
			if (sys_outb(TIMER_CTRL,
					TIMER_RB_CMD | TIMER_RB_COUNT_ | TIMER_RB_SEL(timer))
					!= OK) {
				printf("Error in sys_outb()\n");
				return 1;
			}
			if (sys_inb(TIMER_1, (unsigned long *) st) != OK) {
				printf("Error in sys_inb()\n");
				return 1;
			}

			return 0;
		case 2:
			if (sys_outb(TIMER_CTRL,
					TIMER_RB_CMD | TIMER_RB_COUNT_ | TIMER_RB_SEL(timer))
					!= OK) {
				printf("Error in sys_outb()\n");
				return 1;
			}
			if (sys_inb(TIMER_2, (unsigned long *) st) != OK) {
				printf("Error in sys_inb()\n");
				return 1;
			}

			return 0;
		default:
			return 1;
		}
	}
	return 1;
}

int timer_display_conf(unsigned char conf) {
	if (BIT(7) & conf)
		printf("Output Value: 1\n");
	else
		printf("Output Value: 0\n");

	if (BIT(6) & conf)
		printf("Null Count: 1\n");
	else
		printf("Null Count: 0\n");

	if (((BIT(5) | BIT(4)) & conf) == BIT(4))
		printf("Type of Access: LSB\n");

	else if (((BIT(5) | BIT(4)) & conf) == BIT(5))
		printf("Type of Access: MSB\n");

	else if (((BIT(5) | BIT(4)) & conf) == (BIT(5) | BIT(4)))
		printf("Type of Access: LSB followed by MSB\n");

	else
		printf("Type of Access: WARNING! Not recognised\n");

	if (((BIT(3) | BIT(2) | BIT(1)) & conf) == 0)
		printf("Operating Mode: MODE 0: INTERRUPT ON TERMINAL COUNT\n");

	else if (((BIT(3) | BIT(2) | BIT(1)) & conf) == BIT(1))
		printf("Operating Mode: MODE 1: HARDWARE RETRIGGERABLE ONE-SHOT\n");

	else if ((((BIT(3) | BIT(2) | BIT(1)) & conf) == BIT(2))
			|| (((BIT(3) | BIT(2) | BIT(1)) & conf) == (BIT(3) | BIT(2))))
		printf("Operating Mode: MODE 2: RATE GENERATOR\n");

	else if ((((BIT(3) | BIT(2) | BIT(1)) & conf) == (BIT(2) | BIT(1)))
			|| (((BIT(3) | BIT(2) | BIT(1)) & conf)
					== (BIT(3) | BIT(2) | BIT(1))))
		printf("Operating Mode: MODE 3: SQUARE WAVE MODE\n");

	else if (((BIT(3) | BIT(2) | BIT(1)) & conf) == BIT(3))
		printf("Operating Mode: MODE 4: SOFTWARE TRIGGERED STROBE\n");

	else if (((BIT(3) | BIT(2) | BIT(1)) & conf) == (BIT(3) | BIT(1)))
		printf(
				"Operating Mode: MODE 5: HARDWARE TRIGGERED STROBE (RETRIGGERABLE)\n");
	else
		printf("Operating Mode: WARNING! Not recognised\n");

	if ((BIT(0) & conf) == 0)
		printf("Counting Mode: Binary\n");
	else if ((BIT(0) & conf) == BIT(0))
		printf("Counting Mode: BCD\n");
	else
		return 1;

	return 0;
}

int timer_test_square(unsigned long freq) {
	if (freq <= 0) {
		printf("Error: freq can't be negative\n");
		return 1;
	}

	if (timer_set_square(0, freq) != OK) {
		printf("Error in timer_set_square()\n");
		return 1;
	}

	return 0;

}

int timer_test_int(unsigned long time) {
	counter = 0;
	int r, ipc_status, irq_set;
	message msg;
	irq_set = timer_subscribe_int();
	if (irq_set == -1) {
		printf("Error in timer_subscribe_inta()\n");
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
					timer_int_handler(); /*process it*/
					if (counter % 60 == 0)
						printf("\n%d seconds", counter / 60);
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

int timer_test_config(unsigned long timer) {
	if (timer >= 0 && timer <= 2) {

		unsigned char conf;
		if (timer_get_conf(timer, &conf) != OK) {
			printf("Error in timer_get_conf()\n");
			return 1;
		}
		if (timer_display_conf(conf) != OK) {
			printf("Error in timer_display_conf()\n");
			return 1;
		}

		return 0;

	} else
		return 1;
}

