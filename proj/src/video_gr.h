#ifndef __VIDEO_GR_H
#define __VIDEO_GR_H

#include "tools.h"
/** @defgroup video_gr video_gr
 * @{
 *
 * Functions for outputing data to screen in graphics mode
 */

/**
 * @brief Initializes the video module in graphics mode
 * 
 * Uses the VBE INT 0x10 interface to set the desired
 *  graphics mode, maps VRAM to the process' address space and
 *  initializes static global variables with the resolution of the screen, 
 *  and the number of colors
 * 
 * @param mode 16-bit VBE mode to set
 * @return Virtual address VRAM was mapped to. NULL, upon failure.
 */
void *vg_init(unsigned short mode);

/**
 * @brief Returns the Address of video_mem
 *
 *
 * @return Returns the Address of video_mem
 */
void* vg_vd_get_vmem();

/**
 * @brief Returns the Address of the double buffer
 *
 *
 * @return Returns the Address of the double buffer
 */
void* vg_vd_get_vbuff();

/**
 * @brief Prints the double buffer into the video_mem
 *
 */
void paint_buff();

 /**
 * @brief Returns to default Minix 3 text mode (0x03: 25 x 80, 16 colors)
 * 
 * @return 0 upon success, non-zero upon failure
 */
int vg_exit(void);

/**
 * @brief Paints a pixel
 *
 * @param x coordinate x
 * @param y coordinate y
 * @param color color to paint the pixel
 * @return 0 upon success, -1 if its outside of the range of video_mem
 */
int paint_pixel(unsigned short x, unsigned short y, unsigned long color);

/**
 * @brief Paints a pixel
 *
 * Verifies before painting if its White (checking collisions)
 *
 * @param x coordinate x
 * @param y coordinate y
 * @param color color to paint the pixel
 * @return 0 upon success, -1 if its outside of the range of video_mem, 1 if it was white
 */
int paint_pixelver(unsigned short x, unsigned short y, unsigned long color);

/**
 * @brief Paints borders for the board
 *
 */
void draw_borders();

/**
 * @brief Draws the current position of a Player
 *
 *
 * @param *p Player
 * @param st State of the Player
 * @return 1 if the player lost and 0 if not
 */
int draw_player(player_t *p, state_t st);

/** @} end of video_gr */


#endif /* __VIDEO_GR_H */
