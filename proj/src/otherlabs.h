#ifndef __TIMER_H
#define __TIMER_H


/**
 * @brief Subscribes and enables Keyboard interrupts
 *
 * @return Returns bit order in interrupt mask; negative value on failure
 */
int kbd_subscribe_int(game_t *game1);

/**
 * @brief Unsubscribes Keyboard interrupts
 *
 * @return Return 0 upon success and non-zero otherwise
 */
int kbd_unsubscribe_int(game_t *game1);

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
 * @brief Subscribes and enables Mouse interrupts
 *
 * @return Returns bit order in interrupt mask; negative value on failure
 */
int mouse_subscribe_int(game_t *game1);

/**
 * @brief Unsubscribes Mouse interrupts
 *
 * @return Return 0 upon success and non-zero otherwise
 */
int mouse_unsubscribe_int(game_t *game1);

/**
 * @brief Subscribe Handler
 *
 * Calls the Timer 0, Keyboard and Mouse interrupts subscription functions
 *
 * @return Return 0 upon success and non-zero otherwise
 */
int sub_game(game_t *game1);

/**
 * @brief Unsubscribe Handler
 *
 * Calls the Timer 0, Keyboard and Mouse interrupts unsubscription functions
 *
 * @return Return 0 upon success and non-zero otherwise
 */
int unsub_game(game_t *game1);

/**
 * @brief Keyboard Interrupt Reader
 *
 * Reads from the Keyboard Output Buffer
 *
 * @return Return -1 upon failure and the data read otherwise
 */
unsigned long kbd_handler();

/**
 * @brief Send Command to Keyboard Command Register(port 0x64)
 *
 * @param cmd Command to send
 * @return Return -1 upon failure and 0 otherwise
 */
int kbd_send_command(unsigned long cmd);

/**
 * @brief Send Command to Keyboard Data Buffer(port 0x60)
 *
 * @param cmd Command to send
 * @return Return -1 upon failure and 0 otherwise
 */
int kbd_send(unsigned long cmd);


/**
 * @brief Handler to send commands to keyboard register
 *
 * @param cmd Command to send
 * @return Return 1 upon failure and 0 otherwise
 */
int mouse_send(unsigned long cmd);


#endif /* __TIMER_H */
