#include <minix/syslib.h>
#include <minix/drivers.h>
#include <machine/int86.h>
#include <sys/mman.h>
#include <sys/types.h>
#include "video_gr.h"
#include "vbe.h"
#include "read_xpm.h"
#include "i8254.h"

#define BLACK 0

/* Constants for VBE 0x105 mode */

/* The physical address may vary from VM to VM.
 * At one time it was 0xD0000000
 *  #define VRAM_PHYS_ADDR    0xD0000000 
 * Currently on lab B107 is 0xF0000000
 * Better run my version of lab5 as follows:
 *     service run `pwd`/lab5 -args "mode 0x105"
 */
#define VRAM_PHYS_ADDR	0xE0000000
#define H_RES             1024
#define V_RES		  768
#define BITS_PER_PIXEL	  8

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
	unsigned int vram_size = v_res * h_res * bits_per_pixel * 8; /*VRAM’s size, but you can use the frame-buffer size, instead*/

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

void paint_pixel(unsigned short x, unsigned short y, unsigned long color) {
	char * add_it = video_mem;
	add_it += x + h_res * y;
	if (x < h_res && y < v_res && x >= 0 && y >= 0)
		*add_it = color;

}
int vg_test_xpm(unsigned short xi, unsigned short yi, char *xpm[]) {

	int width, height, out = 0;

	char * xpm_sprt = read_xpm(xpm, &width, &height), *color;

	unsigned short xline, yline;
	for (yline = 0; yline < height; yline++) {
		for (xline = 0; xline < width; xline++) {
			if (xline + xi > h_res || yline + yi > v_res) {
				out = 1;
			} else {
				color = xpm_sprt + width * yline + xline;
				paint_pixel(xline + xi, yline + yi, *color);
			}
		}

	}

	return out;

}

int vg_clean_xpm(unsigned short xi, unsigned short yi, char *xpm[]) {

	int width, height, out = 0;

	char * xpm_sprt = read_xpm(xpm, &width, &height);

	unsigned short xline, yline;
	for (yline = 0; yline < height; yline++) {
		for (xline = 0; xline < width; xline++) {
			if (xline + xi > h_res || yline + yi > v_res) {
				out = 1;
			} else
				paint_pixel(xline + xi, yline + yi, BLACK);
		}

	}

	return out;

}

int vg_controller_handler() {
	VbeInfoBlock vib_p;
	if (vbe_get_controller_info(&vib_p) == 1) {
		printf("set_vbe_get_mode_info: set_vbe_mode: sys_int86() failed \n");
		return 1;
	}

	printf("\n  VBE INFO:\n\n");
	printf("Capabilities : \n");
	if (*(vib_p.Capabilities) & BIT(0))
		printf("\tDAC width is switchable to 8 bits per primary color\n");
	else
		printf("\tDAC is fixed width, with 6 bits per primary color\n");

	if (*(vib_p.Capabilities) & BIT(1))
		printf("\tController is not VGA compatible\n");
	else
		printf("\tController is VGA compatible\n");
	if (*(vib_p.Capabilities) & BIT(2))
		printf(
				"\tWhen programming large blocks of information to the RAMDAC, use the blank bit in Function 09h\n");
	else
		printf("\tNormal RAMDAC operation\n");
	printf("Total Memory Size : %u KB\n", vib_p.TotalMemory * 64);
	return 0;

}
