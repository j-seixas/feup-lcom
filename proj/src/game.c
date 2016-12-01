#include <minix/syslib.h>
#include <minix/drivers.h>
#include <machine/int86.h>
#include <sys/mman.h>
#include <sys/types.h>
#include "game.h"
#include "video_gr.h"
#include "otherlabs.h"
#include "read_bitmap.h"
#include "tools.h"

#define HRES_1024	1280
#define VRES_768	1024
#define GR_MODE		0x11A

static player player1, player2;

int draw_board() {
	Bitmap* tron = loadBitmap(getImagePath("Tronboard"));
	drawBitmap(tron, 0, 0);

	test_square(400, 0, 200, rgb(255,255,255));
	//test_square(400, 0, 200, rgb(12,20,31));

	return 0;
}

int start_multigame() {
	draw_board();
	player1.x = XLLIMIT + 100;
	player1.y = YULIMIT + 400;
	player2.x = XRLIMIT - 100;
	player2.y = YULIMIT + 400;
}

int test_bmp(unsigned short xi, unsigned short yi, char *bmp) {
	return 0;
}

int test_square(unsigned short x, unsigned short y, unsigned short size,
		unsigned long color) {

	unsigned short xline, yline;
	for (xline = x; xline < size + x; xline++) {
		for (yline = y; yline < size + y; yline++) {
			paint_pixel(xline, yline, color);
		}

	}
	return 0;

}
