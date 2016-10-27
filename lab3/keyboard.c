#include <minix/syslib.h>
#include <minix/drivers.h>
#include <minix/com.h>
#include <minix/sysutil.h>
#include "i8254.h"
#include "keyboard.h"
#include "i8042.h"

#define DELAY_US    20000

int hook_id_kbd;

int kbd_subscribe_int(void) {
	hook_id_kbd = KB_IRQ;
	if (sys_irqsetpolicy(KB_IRQ, IRQ_REENABLE | IRQ_EXCLUSIVE, &hook_id_kbd)
			!= OK) {
		printf("Error in sys_irqsetpolicy()\n");
		return -1;
	}
	if (sys_irqenable(&hook_id_kbd) != OK) {
		printf("Error in sys_irqenable()\n");
		return -1;
	}

	return BIT(KB_IRQ);

}

int kbd_unsubscribe_int() {
	if (sys_irqdisable(&hook_id_kbd) != OK) {
		printf("Error in sys_irqdisable()\n");
		return 1;
	}
	if (sys_irqrmpolicy(&hook_id_kbd) != OK) {
		printf("Error in sys_irqrmpolicy()\n");
		return 1;
	}

	return 0;
}

int kbd_make_or_break(unsigned long data) {
	if ((data & BIT(7)) == BIT(7))
		return 1;
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
			else
				return -1;
		}
		tickdelay(micros_to_ticks(DELAY_US));
		var--;
	}
	return -1;
}

int kbd_ACK(unsigned long cmd) {
	unsigned long data;
	kbd_send_command(cmd);
	data = kbd_handler();
	if (data == -1){
		printf("-1\n");
		return -1;
	}
	if ((data & KB_ACK) == KB_ACK){
		printf("0\n");
		return 0;
	}
	if ((data & KB_RESEND) == KB_RESEND){
		printf("1\n");
		return 1;
	}
	if ((data & KB_ERROR) == KB_ERROR){
		printf("2\n");
		return 2;
	}
	else
		return -1;

}

int kbd_led_handler(unsigned long cmd1, unsigned long cmd2) {
	int breakwhile = 0;
	while (1) {
		while (breakwhile < 5) {
			switch (kbd_ACK(cmd1)) {
			case 0:
				printf("ACK\n");
				breakwhile = 10;
				break;
			case 1:
				break;
			case 2:
				break;
			//default:
			//	return -1;

			}
			breakwhile ++;
		}
		if(breakwhile != 11)
			return -1;
		breakwhile = 0;
		while (breakwhile < 5) {
			switch (kbd_ACK(cmd2)) {
			case 0:
				printf("Led %X\n", cmd2);
				return 0;
			case 1:
				break;
			case 2:
				breakwhile = 5;
				break;
			//default:
				//return -1;
			}
		}
	}
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

	}
	return -1;
}

