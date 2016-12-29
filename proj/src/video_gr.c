#include <minix/syslib.h>
#include <minix/drivers.h>
#include <machine/int86.h>
#include <sys/mman.h>
#include <sys/types.h>
#include "video_gr.h"
#include "vbe.h"
#include "read_bitmap.h"
#include "tools.h"

/* Private global variables */

static char *video_mem; /* Process address to which VRAM is mapped */
static char *video_dbuff; /* Process address to double buffer */

static unsigned h_res; /* Horizontal screen resolution in pixels */
static unsigned v_res; /* Vertical screen resolution in pixels */
static unsigned bits_per_pixel; /* Number of VRAM bits per pixel */

void *vg_init(unsigned short mode) {
	struct reg86u r;
	r.u.w.ax = 0x4F02; // VBE call, function 02 -- set VBE mode
	r.u.w.bx = 1 << 14 | mode; // set bit 14: linear framebuffer
	r.u.b.intno = 0x10;
	if (sys_int86(&r) != 0) {
		printf("set_vbe_mode: sys_int86() failed \n");
		return NULL;

	}
	vbe_mode_info_t vmi_p;
	if (vbe_get_mode_info(mode, &vmi_p) == 1) {
		printf("set_vbe_get_mode_info: set_vbe_mode: sys_int86() failed \n");
		return NULL;
	}
	h_res = vmi_p.XResolution;
	v_res = vmi_p.YResolution;
	bits_per_pixel = vmi_p.BitsPerPixel;

	int ans;
	struct mem_range mr;

	unsigned int vram_base = vmi_p.PhysBasePtr; /*VRAMs physical addresss*/
	unsigned int vram_size = v_res * h_res * bits_per_pixel; /*VRAMs size, but you can use the frame-buffer size, instead*/

	/*Allow memory mapping*/
	mr.mr_base = (phys_bytes) vram_base;
	mr.mr_limit = mr.mr_base + vram_size;
	if (OK != (ans = sys_privctl(SELF, SYS_PRIV_ADD_MEM, &mr)))
		panic("sys_privctl (ADD_MEM) failed: %d\n", ans);

	/*Map memory*/

	video_mem = vm_map_phys(SELF, (void *) mr.mr_base, vram_size);
	if (video_mem == MAP_FAILED)
		panic("couldn’t map video memory");

	video_dbuff = (unsigned char *) malloc(vram_size); /*initiates double buffer*/
	return video_mem;
}


void* vg_vd_get_vmem() {
	return video_mem;
}

void* vg_vd_get_vbuff() {
	return video_dbuff;
}

void paint_buff(){
	 memcpy(vg_vd_get_vmem(), vg_vd_get_vbuff(), v_res * h_res * bits_per_pixel);
}

int vg_exit() {
	struct reg86u reg86;

	reg86.u.b.intno = 0x10; /* BIOS video services */

	reg86.u.b.ah = 0x00; /* Set Video Mode function */
	reg86.u.b.al = 0x03; /* 80x25 text mode*/

	if (sys_int86(&reg86) != OK) {
		printf("\tvg_exit(): sys_int86() failed \n");
		return 1;
	} else
		return 0;
}

int paint_pixel(unsigned short x, unsigned short y, unsigned long color) {
	char * add_it = video_mem;
	add_it += (x + h_res * y) * bits_per_pixel / 8;
	if (x < h_res && y < v_res && x >= 0 && y >= 0) {
		*add_it = color;
		add_it++;
		*add_it = color >> 8;
		return 0;
	}
	return -1;
}

int paint_pixelver(unsigned short x, unsigned short y, unsigned long color) {
	char white = WHITE;
	char * add_it = video_mem;
	add_it += (x + h_res * y) * bits_per_pixel / 8;
	if (x < h_res && y < v_res && x >= 0 && y >= 0) {
		if (white == *add_it) {
			return 1;
		}
		*add_it = color;
		add_it++;
		*add_it = color >> 8;
		return 0;
	}
	return -1;
}

void draw_borders() {
	unsigned long color = WHITE;
	unsigned int x, y = YULIMIT;
	for (x = XLLIMIT; x < XRLIMIT; x++) {
		paint_pixel(x, y, color);
	}
	y = YDLIMIT;
	for (x = XLLIMIT; x < XRLIMIT; x++) {
		paint_pixel(x, y, color);
	}
	x = XLLIMIT;
	for (y = YULIMIT; y < YDLIMIT; y++) {
		paint_pixel(x, y, color);
	}
	x = XRLIMIT;
	for (y = YULIMIT; y < YDLIMIT; y++) {
		paint_pixel(x, y, color);
	}
}

int draw_player(player_t *p, state_t st) {
	int lost = 0;
	if (st == UP || st == DOWN) {
		if (paint_pixelver(p->x, p->y, p->color1) == 1)
			lost = 1;
		if (paint_pixelver(p->x + 1, p->y, p->color1) == 1)
			lost = 1;
		if (paint_pixelver(p->x - 1, p->y, p->color2) == 1)
			lost = 1;
		if (paint_pixelver(p->x - 2, p->y, p->color3) == 1)
			lost = 1;
		if (paint_pixelver(p->x + 2, p->y, p->color3) == 1)
			lost = 1;
	} else if (st == RIGHT || st == LEFT) {
		if (paint_pixelver(p->x, p->y, p->color1) == 1)
			lost = 1;
		if (paint_pixelver(p->x, p->y + 1, p->color1) == 1)
			lost = 1;
		if (paint_pixelver(p->x, p->y - 1, p->color2) == 1)
			lost = 1;
		if (paint_pixelver(p->x, p->y - 2, p->color3) == 1)
			lost = 1;
		if (paint_pixelver(p->x, p->y + 2, p->color3) == 1)
			lost = 1;
	}
	if(lost)
		p->st=STOP;
	return lost;
}
