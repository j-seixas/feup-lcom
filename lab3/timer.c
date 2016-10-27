#include <minix/syslib.h>
#include <minix/drivers.h>
#include <minix/com.h>
#include "i8254.h"
#include "timer.h"


int hook_id_timer;

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


