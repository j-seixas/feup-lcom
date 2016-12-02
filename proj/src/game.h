#ifndef __GAME_H
#define __GAME_H



int draw_board();
int init_players(unsigned int num_players);
int playgame(unsigned int num_players);
int start_multigame(unsigned int num_players);
int test_square(unsigned short x, unsigned short y, unsigned short size,
		unsigned long color) ;

#endif /* GAME_H */
