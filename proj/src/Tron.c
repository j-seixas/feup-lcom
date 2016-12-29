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

	if (sub_game(&game1) == 1) {
		vg_exit();
		printf("\n");
		return 1;
	}
	playgame(&game1);

	if (unsub_game(&game1) == 1) {
		vg_exit();
		printf("\n");
		return 1;
	}
	vg_exit();
	printf("\n");

	return 0;
}

