#include "tools.h"

int rgb(unsigned char r, unsigned char g, unsigned char b) {
	if (r < 0 || r > 255 || g < 0 || g > 255 || b < 0 || b > 255)
		return -1;

	unsigned char red = r >> 3;
	unsigned char green = g >> 2;
	unsigned char blue = b >> 3;

	return (red << (5 + 6)) | (green << 5) | blue;
}

void init_mouse(game_t *game1) {
	game1->mouse1.x = 640;
	game1->mouse1.y = 512;
	game1->mouse1.left = KUP;
	game1->mouse1.right = KUP;
	drawBitmap(game1->menu, 0, 0, 0);
	drawBitmap(game1->mouse, game1->mouse1.x, game1->mouse1.y, 0);
}

int init_players(unsigned int num_players, game_t *game1) {
	switch (num_players) {
	case 4: {
		game1->player4.x = 650;
		game1->player4.y = YULIMIT + 100;
		game1->player4.st = DOWN;
		game1->player4.color1 = PINK1;
		game1->player4.color2 = PINK1;
		game1->player4.color3 = WHITE;
		game1->player4.lost = 0;
	}
	case 3: {
		game1->player3.x = 650;
		game1->player3.y = YDLIMIT - 100;
		game1->player3.st = UP;
		game1->player3.color1 = GREEN1;
		game1->player3.color2 = GREEN1;
		game1->player3.color3 = WHITE;
		game1->player3.left = B_MAKE;
		game1->player3.right = M_MAKE;
		game1->player3.lost = 0;
	}
	case 2: {
		game1->player2.x = XRLIMIT - 100;
		game1->player2.y = YULIMIT + 400;
		game1->player2.st = LEFT;
		game1->player2.color1 = ORANGE1;
		game1->player2.color2 = ORANGE2;
		game1->player2.color3 = WHITE;
		game1->player2.left = LARROW_MAKE;
		game1->player2.right = RARROW_MAKE;
		game1->player2.lost = 0;
	}
	case 1: {
		game1->player1.x = XLLIMIT + 100;
		game1->player1.y = YULIMIT + 400;
		game1->player1.st = RIGHT;
		game1->player1.color1 = BLUE1;
		game1->player1.color2 = BLUE2;
		game1->player1.color3 = WHITE;
		game1->player1.left = A_MAKE;
		game1->player1.right = D_MAKE;
		game1->player1.lost = 0;
		return 0;
	}
	default:
		break;
	}

	return 1;
}

void draw_board(unsigned int num_players, game_t *game1) {
	if (game1->gamest == PAUSED) {
		switch (num_players) {
		case 4:
			drawBitmap(game1->board4.boardp, 0, 0, 0);
			break;
		case 3:
			drawBitmap(game1->board3.boardp, 0, 0, 0);
			break;
		case 2:
			drawBitmap(game1->board2.boardp, 0, 0, 0);
			break;
		default:
			break;
		}
	} else {
		switch (num_players) {
		case 4:
			drawBitmap(game1->board4.board, 0, 0, 0);
			break;
		case 3:
			drawBitmap(game1->board3.board, 0, 0, 0);
			break;
		case 2:
			drawBitmap(game1->board2.board, 0, 0, 0);
			break;
		default:
			break;
		}
	}
}

