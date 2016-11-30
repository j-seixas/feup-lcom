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


int main(int argc, char *argv[]) {

	sef_startup();
	/*!
	 * Video mode 0x107 -> 1280x1024
	 */
	//vg_init(0x107);
	//vg_(exit);
	Bitmap* tron = loadBitmap("seixas.bmp");
	test_bmp(0, 0, &tron);
	return 0;
}

