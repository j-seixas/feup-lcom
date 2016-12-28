#ifndef __GAME_H
#define __GAME_H

#include "tools.h"

int draw_board(unsigned int num_players, game_t *game1);
//int unsub_game();
int init_players(unsigned int num_players, game_t *game1);
//int draw_player(unsigned int n, state st);
int draw_player(player_t *p, unsigned int num_players);
//void change_player_state(unsigned int i, unsigned long data);
void change_plst_handler(unsigned int num_players, unsigned long data, game_t *game1);
void change_player_state(player_t *p, unsigned long data);
void update_player(unsigned int num_players, game_t *game1);
int draw_handler(unsigned int num_players, game_t *game1);
void state_handler(unsigned int num_players, unsigned long data, game_t *game1);
int playgame(game_t *game1);
int start_multigame(unsigned int num_players, game_t *game1);
int test_square(unsigned short x, unsigned short y, unsigned short size,
		unsigned long color) ;

#endif /* GAME_H */
