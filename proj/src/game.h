#ifndef __GAME_H
#define __GAME_H

#include "tools.h"

void load_Bmps(game_t* game1);

void change_plst_handler(unsigned int num_players, unsigned long data, game_t *game1);

void change_player_state(player_t *p, unsigned long data);

void update_player(unsigned int num_players, game_t *game1);

int draw_handler(unsigned int num_players, game_t *game1);

void state_handler(unsigned int num_players, unsigned long data, game_t *game1);

int check_mouse(game_t *game1);

int mouse_mov_handler(unsigned long mouse_packet[3], game_t *game1);

void mouse_st_handler(player_t *p, unsigned long mouse_packet[3], game_t *game1);

void check_winner(game_t *game1);

void timer_intrhandler(game_t *game1);

void kbd_intrhandler(unsigned long datakbd, game_t *game1);

void mouse_intrhandler(unsigned long mouse_packet[3], game_t *game1);

int playgame(game_t *game1);

int start_multigame(unsigned int num_players, game_t *game1);

#endif /* GAME_H */
