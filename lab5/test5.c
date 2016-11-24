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
#include <math.h>

#define absol(a) ((a) >= 0 ? (a) : -(a))

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
	if (xi < 0 || xf < 0 || yi < 0 || yf < 0) {
		printf("Invalid parameters\n");
		return 1;
	}

	vg_init(0x105);
	int steps, v = 0, x, y;
	double dx, dy;
	int bool;
	if (xf >= xi && yf >= yi) {
		bool = 1;
	} else if (xi >= xf && yi >= yf) {
		bool = 0;
	} else if (xi >= xf && yi <= yf) {
		bool = 2;
	} else
		bool = 3;

	dx = absol(xf - xi);
	dy = absol(yf - yi);
	if (dx > dy)
		steps = dx;
	else
		steps = dy;

	double Xincrement = dx / (double) steps;
	double Yincrement = dy / (double) steps;

	for (; v < steps; v++) {
		if (bool == 1) {
			xi = xi + Xincrement;
			yi = yi + Yincrement;
		} else if (bool == 0) {
			xi = xi - Xincrement;
			yi = yi - Yincrement;
		} else if (bool == 2) {
			xi = xi - Xincrement;
			yi = yi + Yincrement;
		} else {
			xi = xi + Xincrement;
			yi = yi - Yincrement;
		}
		paint_pixel((int) (xi + 0.5), (int) (yi + 0.5), color);
	}

	kbd_test_scan();
	vg_exit();
	printf("\n");
	return 0;
}

int test_xpm(unsigned short xi, unsigned short yi, char *xpm[]) {

	int width, height;

	vg_init(0x105);
	char * xpm_sprt = read_xpm(xpm, &width, &height);

	unsigned short xline, yline;
	for (xline = xi; xline < width + xi; xline++) {
		for (yline = yi; yline < height + yi; yline++) {
			//color = xpm_sprt + width*yline + xline;
			paint_pixel(xline, yline, *(xpm_sprt + width*yline + xline));
		}

	}

	kbd_test_scan();
	vg_exit();
	printf("\n");
	return 0;

}

int test_move(unsigned short xi, unsigned short yi, char *xpm[],
		unsigned short hor, short delta, unsigned short time) {

	/* To be completed */

}

int test_controller() {

	/* To be completed */

}

