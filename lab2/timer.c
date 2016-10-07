#include <minix/syslib.h>
#include <minix/drivers.h>

int timer_set_square(unsigned long timer, unsigned long freq) {

	return 1;
}

int timer_subscribe_int(void ) {

	return 1;
}

int timer_unsubscribe_int() {

	return 1;
}

void timer_int_handler() {

}

int timer_get_conf(unsigned long timer, unsigned char *st) {
	switch(timer){
	case 0:
		sys_outb(TIMER_CTRL, TIMER_RB_CMD|TIMER_RB_STATUS_|TIMER_RB_SEL(timer));
		sys_inb(TIMER_0, st);
		return 0;
	case 1:
		sys_outb(TIMER_CTRL, TIMER_RB_CMD|TIMER_RB_STATUS_|TIMER_RB_SEL(timer));
		sys_inb(TIMER_1, st);
		return 0;
	case 2:
		sys_outb(TIMER_CTRL, TIMER_RB_CMD|TIMER_RB_STATUS_|TIMER_RB_SEL(timer));
		sys_inb(TIMER_2, st);
		return 0;
	default:
		return 1;
	}

	return 1;
}

int timer_display_conf(unsigned char conf) {
	
	return 1;
}

int timer_test_square(unsigned long freq) {
	
	return 1;
}

int timer_test_int(unsigned long time) {
	
	return 1;
}

int timer_test_config(unsigned long timer) {
	time_get_conf(timer, );
	time_display_conf();
	
	return 1;
}


