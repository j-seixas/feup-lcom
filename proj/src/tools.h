#ifndef __TOOLS_H
#define __TOOLS_H

#include "read_bitmap.h"

#define BIT(n) (0x01<<(n))


/** @defgroup i8254 i8254
 * @{
 *
 * Constants for programming the i8254 Timer.
 */

#define TIMER0_IRQ	        0    /**< @brief Timer 0 IRQ line */
/* I/O port addresses */
#define TIMER_0			0x40 /**< @brief Timer 0 count register */
#define TIMER_CTRL		0x43 /**< @brief Control register */

/* Timer control */

/* Timer selection: bits 7 and 6 */

#define TIMER_SEL0		0x00     /**< @brief Control Word for Timer 0 */
#define TIMER_RB_CMD		(BIT(7)|BIT(6))  /**< @brief Read Back Command */

#define TM0_IRQSET		0 // Timer 0 irq_set for bitmask

/** @defgroup i8042 i8042
 * @{
 *
 * Constants for programming the i8042 KBC.
 */

#define STATUS_PORT         0x64
#define KBD_OUT_BUF         0x60
#define KBD_INPT_BUF        0x64
#define OBF   				BIT(0)
#define IBF					BIT(1)

#define TWO_BYTES			0xE0


#define PAR_ERR				BIT(7)
#define TO_ERR				BIT(6)


//MOUSE COMMANDS
#define MOUSE_SEND			0xD4
#define MOUSE_ENB			0xF4
#define MOUSE_CONF			0xE9
#define KB_ACK              0xFA
#define KBC_CMD_REG			0x64
#define KBD_DATA_BUF        0x60


#define KB_IRQ 			 	1
#define KEYBOARD_IRQ 	 	1
#define MOUSE_IRQ 		  	12

#define GR_MODE		0x11A

#define HRES 1280
#define VRES 1024

#define XLLIMIT		50
#define XRLIMIT		1250
#define YULIMIT		200
#define YDLIMIT		1000
#define MENU1U		347
#define MENU1D		435
#define MENU1L		221
#define MENU1R		578
#define MENU2U		347
#define MENU2D		435
#define MENU2L		715
#define MENU2R		1071
#define MENU3U		526
#define MENU3D		614
#define MENU3L		221
#define MENU3R		578
#define MENU4U		526
#define MENU4D		614
#define MENU4L		715
#define MENU4R		1071


/*!
 * Colors
 */
int rgb(unsigned char r, unsigned char g, unsigned char b);
#define ORANGE1		rgb(223,116,12);
#define ORANGE2		rgb(255,230,77);
#define BLUE1		rgb(111,195,223);
#define BLUE2		rgb(230,255,255);
#define GREEN1		rgb(8,254,20);
#define PINK1		rgb(255,54,155);
#define WHITE		rgb(255,255,255);
#define BLACK		rgb(0,0,0);



/*!
 *  SCANCODES
 */
#define ESC_BREAK			0x81
#define ESC_MAKE			0x01
#define SPACE_BREAK			0xB9
#define A_MAKE				0x1E
#define A_BREAK				0x9E
#define W_MAKE				0x11
#define W_BREAK				0x91
#define S_MAKE				0x1F
#define S_BREAK				0x9F
#define D_MAKE				0x20
#define D_BREAK				0xA0
#define UARROW_MAKE			0xE048
#define UARROW_BREAK		0xE0C8
#define LARROW_MAKE			0xE04B
#define LARROW_BREAK		0xE0CB
#define DARROW_MAKE			0xE050
#define DARROW_BREAK		0xE0D0
#define RARROW_MAKE			0xE04D
#define RARROW_BREAK		0xE0CD
#define V_MAKE				0x2F
#define N_MAKE				0x31
#define I_MAKE				0x17
#define P_MAKE				0x19



typedef enum {
	LEFT, RIGHT, UP, DOWN, STOP
} state;

typedef enum {
	MENU, INIT, PLAYING, PAUSED, FINISHED, QUIT
} game_state;


typedef enum {
	LDOWN, LUP
} ev_type_t;

typedef struct{
	unsigned int x, y;
	state st;
	unsigned int wins, lost;
	unsigned long color1, color2, color3, left, right;

} player;

typedef struct{
	unsigned int x, y;
	ev_type_t type;
} mouse_t;


typedef struct {
	game_state gamest;
	player player1, player2, player3, player4;
	mouse_t mouse1;
	unsigned int num_players, lost;
	int hook_id_timer, irq_set_timer, hook_id_kbd, irq_set_kbd, hook_id_mouse, irq_set_mouse;
	Bitmap* start, * board, *mouse, *menu;

}game_t;

static game_t game1;



#endif /* TOOLS_H */
