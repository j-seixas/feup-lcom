#ifndef __GAME_H
#define __GAME_H

#include "tools.h"

/**
 * @brief Loads all Bitmaps needed in the Game
 *
 */
void load_Bmps(game_t* game1);

/**
 * @brief Handler for Player States
 *
 * Calls the other function accordingly to the Number of Players
 *
 * @param num_players Number of Players in the current Game Mode
 * @param data Keyboard data
 * @param *game1 The game itself
 *
 */
void change_plst_handler(unsigned int num_players, unsigned long data, game_t *game1);

/**
 * @brief Changes the movement state accordingly with the Data
 *
 *
 * @param *p Player
 * @param data Keyboard data
 */
void change_player_state(player_t *p, unsigned long data);

/**
 * @brief Updates the position of a Player based on its movement
 *
 *
 * @param num_players Number of Players in the current Game Mode
 * @param *game1 The game itself
 */
void update_player(unsigned int num_players, game_t *game1);

/**
 * @brief Handler for Painting the Players
 *
 * Calls the other function accordingly to the Number of Players and if they didn't already lost
 *
 * @param num_players Number of Players in the current Game Mode
 * @param *game1 The game itself
 */
void draw_handler(unsigned int num_players, game_t *game1);

/**
 * @brief Handler for Game States
 *
 *
 * @param num_players Number of Players in the current Game Mode
 * @param data Keyboard data
 * @param *game1 The game itself
 */
void state_handler(unsigned int num_players, unsigned long data, game_t *game1);

/**
 * @brief Checks if Mouse is inside a Button in Menu
 *
 *
 * @param *game1 The game itself
 * @return Return 0 if not in a buttton and non-zero otherwise
 */
int check_mouse(game_t *game1);

/**
 * @brief Mouse Handler for Menu
 *
 * Handles the buttons and movement of the mouse in the Menu
 *
 * @param mouse_packet[3] Data packets of the Mouse
 * @param *game1 The game itself
 * @return Return 1 on acceptable data and 0 if overflow
 */
int mouse_mov_handler(unsigned long mouse_packet[3], game_t *game1);

/**
 * @brief Mouse State Handler for Player 4
 *
 *
 * @param *p Player
 * @param mouse_packet[3] Data packets of the Mouse
 * @param *game1 The game itself
 */
void mouse_st_handler(player_t *p, unsigned long mouse_packet[3], game_t *game1);

/**
 * @brief Checks who wins and displays the winner bitmap
 *
 * @param *game1 The game itself
 */
void check_winner(game_t *game1);

/**
 * @brief Handler for Timer Interrupts
 *
 *
 * @param *game1 The game itself
 */
void timer_intrhandler(game_t *game1);

/**
 * @brief Handler for Keyboard Interrupts
 *
 *
 * @param datakbd Keyboard Data
 * @param *game1 The game itself
 */
void kbd_intrhandler(unsigned long datakbd, game_t *game1);

/**
 * @brief Handler for Mouse Interrupts
 *
 *
 * @param mouse_packet[3] Data packets of the Mouse
 * @param *game1 The game itself
 */
void mouse_intrhandler(unsigned long mouse_packet[3], game_t *game1);

/**
 * @brief The Game Function
 *
 * Interrupts cycle
 *
 *
 * @param *game1 The game itself
 */
int playgame(game_t *game1);

/**
 * @brief Starts a Multiplayer Game accorgingly to the Number of Players
 *
 *
 * @param num_players Number of Players on the Game Mode
 * @param *game1 The game itself
 * @return Return 0 on success and 1 otherwise
 */
int start_multigame(unsigned int num_players, game_t *game1);

#endif /* GAME_H */
