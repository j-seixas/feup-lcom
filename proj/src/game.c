#include <minix/syslib.h>
#include <minix/drivers.h>
#include <machine/int86.h>
#include <sys/mman.h>
#include <sys/types.h>
#include "game.h"
#include "video_gr.h"
#include "otherlabs.h"
#include "read_bitmap.h"
#include "i8042.h"
#include "i8254.h"
//#include "seixas.bmp"


#define HRES_1024	1280
#define VRES_768	1024
#define GR_MODE		0x11A

int test_bmp(unsigned short xi, unsigned short yi, char *bmp[]) {

	vg_init(GR_MODE);

	drawBitmap(bmp, xi, yi, ALIGN_LEFT);
	kbd_test_scan();
	vg_exit();
	printf("\n");
	return 0;

}
