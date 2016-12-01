#ifndef __TOOLS_H
#define __TOOLS_H

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

#define ESC_BREAK			0x81
#define ESC_MAKE			0x01
#define TWO_BYTES			0xE0


#define PAR_ERR				BIT(7)
#define TO_ERR				BIT(6)


//MOUSE COMMANDS
#define MOUSE_SEND			0xD4
#define MOUSE_ENB			0xF4
#define MOUSE_CONF			0xE9


#define KB_IRQ 			 	1
#define MOUSE_IRQ 		  	12

#define GR_MODE		0x11A

#define HRES 1280
#define VRES 1024

#define XLLIMIT		50
#define XRLIMIT		1250
#define YULIMIT		200
#define YDLIMIT		1000


int rgb(unsigned char r, unsigned char g, unsigned char b);

#endif /* TOOLS_H */
