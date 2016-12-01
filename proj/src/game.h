#ifndef __GAME_H
#define __GAME_H

typedef enum {
	LEFT, RIGHT, UP, DOWN
} state;

typedef enum {
	LPressed, RPressed, UPressed, DPressed, LReleased, RReleased, UReleased, DReleased
} keys;


typedef struct{
	unsigned int x, y;
	state st;
	unsigned int wins;
	keys Lkey, Rkey, Ukey, Dkey;
	unsigned long color1, color2;

} player;

int draw_board();
int start_multigame();
int test_bmp(unsigned short xi, unsigned short yi, char *bmp);
int test_square(unsigned short x, unsigned short y, unsigned short size,
		unsigned long color) ;

#endif /* GAME_H */
