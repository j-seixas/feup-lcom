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

	unsigned int vram_base = vmi_p.PhysBasePtr; /*VRAM’s physical addresss*/
	unsigned int vram_size = v_res * h_res * bits_per_pixel; /*VRAM’s size, but you can use the frame-buffer size, instead*/

	/*Allow memory mapping*/
	mr.mr_base = (phys_bytes) vram_base;
	mr.mr_limit = mr.mr_base + vram_size;
	if (OK != (ans = sys_privctl(SELF, SYS_PRIV_ADD_MEM, &mr)))
		panic("sys_privctl (ADD_MEM) failed: %d\n", ans);

	/*Map memory*/

	video_mem = vm_map_phys(SELF, (void *) mr.mr_base, vram_size);
	if (video_mem == MAP_FAILED)
		panic("couldn’t map video memory");
	return video_mem;
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
	char white = WHITE;
	char * add_it = video_mem;
	add_it += (x + h_res * y) * bits_per_pixel / 8;
	if (x < h_res && y < v_res && x >= 0 && y >= 0) {
		if (white == *add_it) {
			return 1;
		}
		*add_it = color;
		add_it++;
		white = white >> 8;
		if (white == *add_it) {
			return 1;
		}
		*add_it = color >> 8;
		return 0;
	}
	return -1;
}

void* vg_vd_get_vmem() {
	return video_mem;
}
