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

#define HRES_1024	1024
#define VRES_768	768
#define GR_MODE		0x105

#define absol(a) ( (a >= 0) ? (a) : -(a))

void *test_init(unsigned short mode, unsigned short delay) {

	void *ptr_res = vg_init(mode);

	if (timer_test_int(delay) != 0) {
		vg_exit();
		return NULL;
	}
	vg_exit();
	printf("Physical Address: %X", ptr_res);
	printf("\n");
	return ptr_res;

}

int test_square(unsigned short x, unsigned short y, unsigned short size,
		unsigned long color) {
	if (x < 0 || y < 0 || size <= 0 || x >= HRES_1024 || y >= VRES_768) {
		printf("Invalid parameters\n");
		return 1;
	}
	vg_init(GR_MODE);

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
	vg_init(GR_MODE);
	if (xi < 0 || xf < 0 || yi < 0 || yf < 0) {
		printf("Invalid parameters\n");
		vg_exit();
		return 1;
	}
	if (xi >= HRES_1024 || xf >= HRES_1024 || yi >= VRES_768 || yf >= VRES_768) {
		printf("Invalid parameters\n");
		vg_exit();
		return 1;
	}

	/*
	 *  Bresenham's line algorithm - https://rosettacode.org/wiki/Bitmap/Bresenham%27s_line_algorithm
	 *
	 *  I used the algorithm above. At first I tried to use another and tried to make one,
	 *   but both times there were cases that weren't working correctly
	 */

	int dx = absol(xf - xi), sx = xi < xf ? 1 : -1; //Checks the variance of x ; Checks if variance is negative or positive
	int dy = absol(yf - yi), sy = yi < yf ? 1 : -1; //Checks the variance of y ; Checks if variance is negative or positive
	int err = (dx > dy ? dx : -dy) / 2, e2; //Checks the higher variance , if x or y

	for (;;) {
		paint_pixel(xi, yi, color);
		if (xi == xf && yi == yf) // If its already on the final point breaks
			break;
		e2 = err;
		if (e2 > -dx) {
			err -= dy;
			xi += sx;
		}
		if (e2 < dy) {
			err += dx;
			yi += sy;
		}
	}

	kbd_test_scan();
	vg_exit();
	printf("\n");
	return 0;
}

int test_xpm(unsigned short xi, unsigned short yi, char *xpm[]) {

	vg_init(GR_MODE);
	vg_test_xpm(xi, yi, xpm);
	kbd_test_scan();
	vg_exit();
	printf("\n");
	return 0;

}

int test_move(unsigned short xi, unsigned short yi, char *xpm[],
		unsigned short hor, short delta, unsigned short time) {

	if (xi < 0 || yi < 0 || xi >= HRES_1024 || yi >= VRES_768) {
		printf("Invalid parameters\n");
		return 1;
	}
	if ((delta < 0 && hor == 1 && xi + delta < 0)
			|| (delta >= 0 && hor == 1 && xi + delta >= HRES_1024)
			|| (delta < 0 && hor != 1 && yi + delta < 0)
			|| (delta >= 0 && hor != 1 && yi + delta >= VRES_768)) {
		printf("Invalid parameters\n");
		return 1;
	}
	if (time == 0) {
		printf("Invalid parameters\n");
		return 1;
	}

	vg_init(GR_MODE);
	double timedelta = delta / (time * 60);
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
	vg_test_xpm(x, y, xpm);
	while (data != ESC_BREAK) {
		if ((r = driver_receive(ANY, &msg, &ipc_status)) != 0) {
			printf("driver_receive failed with: %d", r);
			continue;
		}
		if (is_ipc_notify(ipc_status)) {
			switch (_ENDPOINT_P(msg.m_source)) {
			case HARDWARE:
				if (msg.NOTIFY_ARG & irq_set) {
					if (counter / 60 < time) {
						if (hor == 1) {
							if (delta < 0) {
								vg_clean_xpm(x, y, xpm);
								x = x + (int) (timedelta - 0.5);
								vg_test_xpm(x, y, xpm);
								if (x < xi + delta) {
									counter = time * 60;
								}
							} else {
								vg_clean_xpm(x, y, xpm);
								x = x + (int) (timedelta + 0.5);
								vg_test_xpm(x, y, xpm);
								if (x > xi + delta) {
									counter = time * 60;
								}
							}
						} else {
							if (delta < 0) {
								vg_clean_xpm(x, y, xpm);
								y = y + (int) (timedelta - 0.5);
								vg_test_xpm(x, y, xpm);
								if (y < yi + delta) {
									counter = time * 60;
								}
							} else {
								vg_clean_xpm(x, y, xpm);
								y = y + (int) (timedelta + 0.5);
								vg_test_xpm(x, y, xpm);
								if (y > yi + delta) {
									counter = time * 60;
								}
							}

						}

						counter++;
						break;
					}
				}
				if (msg.NOTIFY_ARG & irq_set_kbd) {
					data = kbd_handler();
					break;
				}
			default:
				break;
			}
		} else {

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

	printf("Ended with ESC key\n");
	vg_exit();
	printf("\n");
	return 0;

}

int test_controller() {

	if (vg_controller_handler() == 1)
		return 1;
	return 0;

}

