#ifndef __TIMER_H
#define __TIMER_H

int kbd_subscribe_int(game_t *game1);
int kbd_unsubscribe_int(game_t *game1);
unsigned long kbd_handler();
//int kbd_test_scan();


int sub_game(game_t *game1);
int unsub_game(game_t *game1);


/** @defgroup timer timer
 * @{
 *
 * Functions for using the i8254 timers
 */



/**
 * @brief Subscribes and enables Timer 0 interrupts
 *
 * @return Returns bit order in interrupt mask; negative value on failure
 */
int timer_subscribe_int(game_t *game1);

/**
 * @brief Unsubscribes Timer 0 interrupts
 *
 * @return Return 0 upon success and non-zero otherwise
 */
int timer_unsubscribe_int(game_t *game1);



/**
 * @brief Tests Timer 0 interrupt handling
 *
 * Subscribes Timer 0 interrupts and prints a message once
 *  per second for the specified time interval
 *
 * @param time Length of time interval while interrupts are subscribed
 * @return Return 0 upon success and non-zero otherwise
 */
//int timer_test_int(unsigned long time);


int mouse_subscribe_int(game_t *game1);

int mouse_unsubscribe_int(game_t *game1);


int unsub_game(game_t *game1);


int kbd_send_command(unsigned long cmd);

int kbd_send(unsigned long cmd);

int mouse_send(unsigned long cmd);

#endif /* __TIMER_H */
