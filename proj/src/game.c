#include <minix/syslib.h>
#include <minix/drivers.h>
#include <machine/int86.h>
#include <sys/mman.h>
#include <sys/types.h>
#include "game.h"
#include "video_gr.h"
#include "otherlabs.h"
#include "tron_arcade_lightcycles.xpm"
#include "read_xpm.h"
#include "i8042.h"
#include "i8254.h"


#define HRES_1024	1280
#define VRES_768	1024
#define GR_MODE		0x107

int test_xpm(unsigned short xi, unsigned short yi, char *xpm[]) {

	vg_init(GR_MODE);
	vg_test_xpm(xi, yi, xpm);
	kbd_test_scan();
	vg_exit();
	printf("\n");
	return 0;

}
