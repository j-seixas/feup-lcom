#include <minix/syslib.h>
#include <minix/drivers.h>
#include <minix/com.h>
#include "i8254.h"
#include "timer.h"

static unsigned int counter = 0;
int hook_id;

int timer_set_square(unsigned long timer, unsigned long freq) {
	if (timer < 0 || timer > 2 || freq == 0)
		return 1;
	else {
		int div = TIMER_FREQ / freq;
		unsigned char timer_lsb = (char) div;
		unsigned char timer_msb = (char) div >> 8;

		if (timer == 0) {
			sys_outb(TIMER_CTRL,
					TIMER_LSB_MSB | TIMER_SEL0 | TIMER_BIN | TIMER_SQR_WAVE);
			sys_outb(TIMER_0, timer_lsb);
			sys_outb(TIMER_0, timer_msb);

		} else if (timer == 1) {
			sys_outb(TIMER_CTRL,
					TIMER_LSB_MSB | TIMER_SEL1 | TIMER_BIN | TIMER_SQR_WAVE);
			sys_outb(TIMER_1, timer_lsb);
			sys_outb(TIMER_1, timer_msb);

		} else if (timer == 2) {
			sys_outb(TIMER_CTRL,
					TIMER_LSB_MSB | TIMER_SEL2 | TIMER_BIN | TIMER_SQR_WAVE);
			sys_outb(TIMER_2, timer_lsb);
			sys_outb(TIMER_2, timer_msb);

		} else
			return 1;

	}
	return 0;
}

int timer_subscribe_int(void) {
	hook_id = 0;
	sys_irqsetpolicy(TIMER0_IRQ, IRQ_REENABLE, &hook_id);
	sys_irqenable(&hook_id);

	return 0;

}

int timer_unsubscribe_int() {
	sys_irqdisable(&hook_id);
	sys_irqrmpolicy(&hook_id);

	return 1;
}

void timer_int_handler() {
	counter++;

}

int timer_get_conf(unsigned long timer, unsigned char *st) {
	switch (timer) {
	case 0:
		sys_outb(TIMER_CTRL,
				TIMER_RB_CMD | TIMER_RB_COUNT_ | TIMER_RB_SEL(timer));
		sys_inb(TIMER_0, (unsigned long *)st);
		return 0;
	case 1:
		sys_outb(TIMER_CTRL,
				TIMER_RB_CMD | TIMER_RB_COUNT_ | TIMER_RB_SEL(timer));
		sys_inb(TIMER_1, (unsigned long *)st);
		return 0;
	case 2:
		sys_outb(TIMER_CTRL,
				TIMER_RB_CMD | TIMER_RB_COUNT_ | TIMER_RB_SEL(timer));
		sys_inb(TIMER_2, (unsigned long *)st);
		return 0;
	default:
		return 1;
	}

	return 1;
}

int timer_display_conf(unsigned char conf) {
	if (BIT(7) & conf) {
		printf("Output Value: 1\n");
	} else {
		printf("Output Value: 0\n");
	}
	if (BIT(6) & conf) {
		printf("Null Count: 1\n");
	} else {
		printf("Null Count: 0\n");
	}
	if ((BIT(5) | BIT(4)) == BIT(4)) {
		printf("LSB\n");

	} else if (((BIT(5) | BIT(4)) & conf) == BIT(5)) {
		printf("MSB\n");

	} else if (((BIT(5) | BIT(4)) & conf) == (BIT(5) | BIT(4))) {
		printf("LSB followed by MSB\n");

	}
	if (((BIT(3) | BIT(2) | BIT(1)) & conf) == 0) {
		printf("MODE 0: INTERRUPT ON TERMINAL COUNT\n");

	} else if (((BIT(3) | BIT(2) | BIT(1)) & conf) == BIT(1)) {
		printf("MODE 1: HARDWARE RETRIGGERABLE ONE-SHOT\n");

	} else if ((((BIT(3) | BIT(2) | BIT(1)) & conf) == BIT(2))
			|| (((BIT(3) | BIT(2) | BIT(1)) & conf) == (BIT(3) | BIT(2)))) {
		printf("MODE 2: RATE GENERATOR\n");

	} else if ((((BIT(3) | BIT(2) | BIT(1)) & conf) == (BIT(2) | BIT(1)))
			|| (((BIT(3) | BIT(2) | BIT(1)) & conf)
					== (BIT(3) | BIT(2) | BIT(1)))) {
		printf("MODE 3: SQUARE WAVE MODE\n");

	} else if (((BIT(3) | BIT(2) | BIT(1)) & conf) == BIT(3)) {
		printf("MODE 4: SOFTWARE TRIGGERED STROBE\n");

	} else if (((BIT(3) | BIT(2) | BIT(1)) & conf) == (BIT(3) | BIT(1))) {
		printf("MODE 5: HARDWARE TRIGGERED STROBE (RETRIGGERABLE)\n");

	}
	if ((BIT(0) & conf) == 0)
		printf("BCD = 0\n");
	else if ((BIT(0) & conf) == BIT(0))
		printf("BCD = 1\n");
	else
		return 1;

	return 0;
}

int timer_test_square(unsigned long freq) {
	timer_set_square(0, freq);
	return 1;
}

int timer_test_int(unsigned long time) {
	counter = 0;
	int r, ipc_status, irq_set;
	message msg;
	irq_set = timer_subscribe_int();
	irq_set = BIT(irq_set);

	while (counter/60 < time) { /*You may want to use a different condition*/
		/*Get a request message.*/
		if ((r = driver_receive(ANY, &msg, &ipc_status)) != 0) {
			printf("driver_receive failed with: %d", r);
			continue;
		}
		if (is_ipc_notify(ipc_status)) { 				/*received notification*/
			switch (_ENDPOINT_P(msg.m_source)) {
			case HARDWARE:								 /*hardware interrupt notification*/
				if (msg.NOTIFY_ARG & irq_set) { 		/*subscribed interrupt*/
					timer_int_handler();				/*process it*/
					if (counter % 60 == 0)
						printf("\n%d seconds", counter/60);
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
	timer_unsubscribe_int();
	return 1;
}

int timer_test_config(unsigned long timer) {
	unsigned char conf;
	timer_get_conf(timer, &conf);
	timer_display_conf(conf);

	return 1;
}

