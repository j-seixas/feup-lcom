#include "lmlib.h"
#include "vbe.h"
#include "video_gr.h"
#include "game.h"
#include "otherlabs.h"
#include "tools.h"
#include "read_bitmap.h"
#include <limits.h>
#include <string.h>
#include <errno.h>


int main(int argc, char *argv[]) {

	sef_startup();
	/*!
	 * Video mode 0x11A -> 1280x1024 16bit 5:6:5
	 */
	vg_init(GR_MODE);
	game_t game1;
	sub_game(&game1);

	playgame(&game1);

	//kbd_test_scan();

	unsub_game(&game1);
	vg_exit();
	printf("\n");

	//test_bmp(0, 0, tron);
	return 0;
}

