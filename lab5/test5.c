#include <minix/syslib.h>
#include <minix/drivers.h>
#include <machine/int86.h>
#include <sys/mman.h>
#include <sys/types.h>
#include "test5.h"
#include "video_gr.h"
#include "otherlabs.h"
#include "pixmap.h"
#include "read_xpm.h"
#include "i8042.h"
#include "i8254.h"

#define absol(a) ( (a >= 0) ? (a) : -(a))

void *test_init(unsigned short mode, unsigned short delay) {

	void *ptr_res = vg_init(mode);

	if (timer_test_int(delay) != 0) {
		vg_exit();
		return NULL;
	}
	vg_exit();
	printf("\n");
	return ptr_res;

}

int test_square(unsigned short x, unsigned short y, unsigned short size,
		unsigned long color) {
	if (x < 0 || y < 0 || size <= 0) {
		printf("Invalid parameters\n");
		return 1;
	}
	vg_init(0x105);

	unsigned short xline, yline;
	for (xline = x; xline < size + x; xline++) {
		for (yline = y; yline < size + y; yline++) {
			paint_pixel(xline, yline, color);
		}

	}

	kbd_test_scan();
	vg_exit();
	printf("\n");
	return 0;

}

int test_line(unsigned short xi, unsigned short yi, unsigned short xf,
		unsigned short yf, unsigned long color) {
	vg_init(0x105);
	if (xi < 0 || xf < 0 || yi < 0 || yf < 0) {
		printf("Invalid parameters\n");
		vg_exit();
		return 1;
	}
	if ((int) xi >= 1024 || (int) xf >= 1024 || (int) yi >= 768
			|| (int) yf >= 768) {
		printf("Invalid parameters\n");
		vg_exit();
		return 1;
	}

	int steps, v, x, y, dx, dy;
	int bool;
	/*if (xf >= xi && yf >= yi) {
	 bool = 1;
	 } else if (xi > xf && yi > yf) {
	 bool = 0;
	 } else if (xi > xf && yi < yf) {
	 bool = 2;
	 } else if (xf > xi && yi > yf)
	 bool = 3;*/

	dx = xf - xi;
	dy = yf - yi;
	if (absol(dx) > absol(dy))
		steps = dx;
	else
		steps = dy;

	double Xincrement = (double) dx / (double) steps;
	double Yincrement = (double) dy / (double) steps;
	steps = absol(steps);
	for (v = 0; v < steps; v++) {
		/*if (bool == 1) {
		 xi = xi + Xincrement;
		 yi = yi + Yincrement;
		 } else if (bool == 0) {
		 xi = xi - Xincrement;
		 yi = yi - Yincrement;
		 /*} else if (bool == 2) {
		 xi = xi - Xincrement;
		 yi = yi + Yincrement;
		 } else if (bool == 3) {
		 xi = xi + Xincrement;
		 yi = yi - Yincrement;*/
		//}
		xi = xi + Xincrement;
		yi = yi + Yincrement;
		paint_pixel((int) (xi + 0.5), (int) (yi + 0.5), color);
	}

	kbd_test_scan();
	vg_exit();
	printf("\n");
	return 0;
}

int test_xpm(unsigned short xi, unsigned short yi, char *xpm[]) {

	vg_init(0x105);
	vg_test_xpm(xi, yi, xpm);
	kbd_test_scan();
	vg_exit();
	printf("\n");
	return 0;

}

int test_move(unsigned short xi, unsigned short yi, char *xpm[],
		unsigned short hor, short delta, unsigned short time) {

	if (xi < 0 || yi < 0 || xi >= 1024 || yi >= 768) {
		printf("Invalid parameters\n");
		return 1;
	}
	if ((delta < 0 && hor == 1 && xi + delta < 0)
			|| (delta >= 0 && hor == 1 && xi + delta >= 1024)
			|| (delta < 0 && hor != 1 && yi + delta < 0)
			|| (delta >= 0 && hor != 1 && yi + delta >= 768)) {
		printf("Invalid parameters\n");
		return 1;
	}
	if (time == 0) {
		printf("Invalid parameters\n");
		return 1;
	}

	vg_init(0x105);
	printf("HERE, %d, %d\n", delta, (int) time);
	double timedelta = time/60 / delta;
	printf("DELTAT = %d", (int) timedelta);
	unsigned int counter = 0;
	unsigned short x = xi, y = yi;
	int r, ipc_status, irq_set, irq_set_kbd;
	message msg;
	irq_set = timer_subscribe_int();
	if (irq_set == -1) {
		printf("Error in timer_subscribe_int()\n");
		return 1;
	}

	irq_set_kbd = kbd_subscribe_int();
	if (irq_set_kbd == -1) {
		printf("Error in kbd_subscribe_int()\n");
		return 1;
	}
	unsigned long data;
	printf("RIGHT BEFORE\n");
	while (counter / 60 < time && data != ESC_BREAK) {
		printf("NOT HERE\n");
		if ((r = driver_receive(ANY, &msg, &ipc_status)) != 0) { /*Get a request message.*/
			printf("driver_receive failed with: %d", r);
			continue;
		}
		if (is_ipc_notify(ipc_status)) { /*received notification*/
			switch (_ENDPOINT_P(msg.m_source)) {
			case HARDWARE: /*hardware interrupt notification*/
				if (msg.NOTIFY_ARG & irq_set) { /*subscribed interrupt*/
					if (hor == 1) {
						if (delta < 0) {
							vg_test_xpm(xi, yi, xpm);
							x = x + (int) (timedelta + 0.5);
							if (x < xi + delta) {
								counter = time * 60;
							}
						} else {
							vg_test_xpm(x, y, xpm);
							x = x + (int) (timedelta + 0.5);
							if (x > xi + delta) {
								counter = time * 60;
							}
						}
					} else {
						if (delta < 0) {
							vg_test_xpm(x, y, xpm);
							y = y + (int) (timedelta + 0.5);
							if (y < yi + delta) {
								counter = time * 60;
							}
						} else {
							vg_test_xpm(x, y, xpm);
							y = y + (int) (timedelta + 0.5);
							if (y > yi + delta) {
								counter = time * 60;
							}
						}

					}
					counter++;
					printf("x=%d,y=%d", xi, yi);
					break;
				}
				if (msg.NOTIFY_ARG & irq_set_kbd) { /*subscribed interrupt*/
					data = kbd_handler(); /*process it*/
					break;
				}
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
	if (kbd_unsubscribe_int() != OK) {
		printf("Error in kbd_unsubscribe_int()\n");
		return 1;
	}
	//
	printf("EXIT\n");
	vg_exit();
	printf("\n");
	return 0;

}

int test_controller() {

	/* To be completed */

}

