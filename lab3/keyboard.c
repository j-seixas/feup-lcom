#include <minix/syslib.h>
#include <minix/drivers.h>
#include <minix/com.h>
#include "i8254.h"
#include "keyboard.h"
#include <minix/sysutil.h>

#define DELAY_US    20000


int hook_id;

int kbd_subscribe_int(void) {
	hook_id = KB_IRQSET;
	if (sys_irqsetpolicy(KB_IRQ, IRQ_REENABLE | IRQ_EXCLUSIVE, &hook_id)
			!= OK) {
		printf("Error in sys_irqsetpolicy()\n");
		return -1;
	}
	if (sys_irqenable(&hook_id) != OK) {
		printf("Error in sys_irqenable()\n");
		return -1;
	}

	return BIT(KB_IRQSET);

}

int kbd_unsubscribe_int() {
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

int kbd_handler() {

	while (1) {
		sys_inb(STAT_REG, &stat); /*assuming it returns OK*/
		/*loop while 8042 output buffer is empty*/
		if (stat & OBF) {
			sys_inb(OUT_BUF, &data); /*	assuming it returns OK	*/
			if ((stat & (PAR_ERR | TO_ERR)) == 0)
				return data;
			else
				return -1;
		}
		tickdelay(micros_to_ticks(DELAY_US);
	}
}
