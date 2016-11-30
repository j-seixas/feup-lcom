#include "lmlib.h"
#include "vbe.h"
#include "video_gr.h"
#include "game.h"
#include "otherlabs.h"
#include "i8254.h"
#include "i8042.h"
#include "read_bitmap.h"
//#include "seixas.bmp"
#include <limits.h>
#include <string.h>
#include <errno.h>

#define GR_MODE 0x11A

int main(int argc, char *argv[]) {

	sef_startup();
	/*!
	 * Video mode 0x107 -> 1280x1024
	 */
	//vg_init(0x107);
	//vg_(exit);
	vg_init(GR_MODE);
	Bitmap* tron = loadBitmap("home/lcom/lcom1617-t4g12/proj/src/Tronboard.bmp");
	drawBitmap(tron, 0, 0, ALIGN_LEFT);
	kbd_test_scan();
	vg_exit();
	printf("\n");

	//test_bmp(0, 0, tron);
	return 0;
}

