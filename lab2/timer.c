#include <minix/syslib.h>
#include <minix/drivers.h>
#include "i8254.h"
#include "timer.h"

int timer_set_square(unsigned long timer, unsigned long freq) {

	return 1;
}

int timer_subscribe_int(void) {

	return 1;
}

int timer_unsubscribe_int() {

	return 1;
}

void timer_int_handler() {

}

int timer_get_conf(unsigned long timer, unsigned char *st) {
	switch (timer) {
	case 0:
		sys_outb(TIMER_CTRL,
				TIMER_RB_CMD | TIMER_RB_COUNT_ | TIMER_RB_SEL(timer));
		sys_inb(TIMER_0, st);
		return 0;
	case 1:
		sys_outb(TIMER_CTRL,
				TIMER_RB_CMD | TIMER_RB_COUNT_ | TIMER_RB_SEL(timer));
		sys_inb(TIMER_1, st);
		return 0;
	case 2:
		sys_outb(TIMER_CTRL,
				TIMER_RB_CMD | TIMER_RB_COUNT_ | TIMER_RB_SEL(timer));
		sys_inb(TIMER_2, st);
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


	return 1;
}

int timer_test_int(unsigned long time) {

	return 1;
}

int timer_test_config(unsigned long timer) {
	unsigned char conf;
	timer_get_conf(timer, &conf);
	timer_display_conf(conf);

	return 1;
}

