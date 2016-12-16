#include <minix/syslib.h>
#include <minix/drivers.h>
#include <machine/int86.h>
#include <sys/mman.h>
#include <sys/types.h>
#include "game.h"
#include "video_gr.h"
#include "otherlabs.h"
#include "read_bitmap.h"
#include "tools.h"

#define HRES_1024	1280
#define VRES_768	1024
#define GR_MODE		0x11A

int draw_board() {

	game1.board = loadBitmap(getImagePath("Game4p"));
	drawBitmap(game1.board, 0, 0);

	game1.start = loadBitmap(getImagePath("Start"));
	drawBitmap(game1.start, 255, 460);
	//paint_buff();
	//kbd_test_scan();
	//unsub_game();
	//sub_game();
	printf(".........\n");
	//drawBitmap(tron2p, 0, 0);
	return 0;

}

void draw_borders() {
	unsigned long color = WHITE;
	unsigned int x, y = YULIMIT;
	for (x = XLLIMIT; x < XRLIMIT; x++) {
		paint_pixel(x, y, color);
	}
	y = YDLIMIT;
	for (x = XLLIMIT; x < XRLIMIT; x++) {
		paint_pixel(x, y, color);
	}
	x = XLLIMIT;
	for (y = YULIMIT; y < YDLIMIT; y++) {
		paint_pixel(x, y, color);
	}
	x = XRLIMIT;
	for (y = YULIMIT; y < YDLIMIT; y++) {
		paint_pixel(x, y, color);
	}
}

int init_players(unsigned int num_players) {
	switch (num_players) {
	case 4: {
		game1.player4.x = (XRLIMIT - XLLIMIT)/2;
		game1.player4.y = YULIMIT - 100;
		game1.player4.st = UP;
		game1.player4.color1 = PINK1;
		game1.player4.color2 = PINK1;
		game1.player4.color3 = WHITE;
		game1.player4.left = I_MAKE;
		game1.player4.right = P_MAKE;
	}
	case 3: {
		game1.player3.x = (XRLIMIT - XLLIMIT)/2;
		game1.player3.y = YDLIMIT + 100;
		game1.player3.st = DOWN;
		game1.player3.color1 = GREEN1;
		game1.player3.color2 = GREEN1;
		game1.player3.color3 = WHITE;
		game1.player3.left = V_MAKE;
		game1.player3.right = N_MAKE;
	}
	case 2: {
		game1.player2.x = XRLIMIT - 100;
		game1.player2.y = YULIMIT + 400;
		game1.player2.st = LEFT;
		game1.player2.color1 = ORANGE1;
		game1.player2.color2 = ORANGE2;
		game1.player2.color3 = WHITE;
		game1.player2.left = LARROW_MAKE;
		game1.player2.right = RARROW_MAKE;
	}
	case 1: {
		game1.player1.x = XLLIMIT + 100;
		game1.player1.y = YULIMIT + 400;
		game1.player1.st = RIGHT;
		game1.player1.color1 = BLUE1;
		game1.player1.color2 = BLUE2;
		game1.player1.color3 = WHITE;
		game1.player1.left = A_MAKE;
		game1.player1.right = D_MAKE;
		return 0;
	}
	default:
		break;
	}

	return 1;
}

int draw_player(unsigned int n, state st) {
	if (n == 1) {
		if (st == UP || st == DOWN) {
			if (paint_pixelver(game1.player1.x, game1.player1.y,
					game1.player1.color1) == 1)
				return 1;
			if (paint_pixelver(game1.player1.x + 1, game1.player1.y,
					game1.player1.color1) == 1)
				return 1;
			if (paint_pixelver(game1.player1.x - 1, game1.player1.y,
					game1.player1.color2) == 1)
				return 1;
			if (paint_pixelver(game1.player1.x - 2, game1.player1.y,
					game1.player1.color3) == 1)
				return 1;
			if (paint_pixelver(game1.player1.x + 2, game1.player1.y,
					game1.player1.color3) == 1)
				return 1;
		} else if (st == RIGHT || st == LEFT) {
			if (paint_pixelver(game1.player1.x, game1.player1.y,
					game1.player1.color1) == 1)
				return 1;
			if (paint_pixelver(game1.player1.x, game1.player1.y + 1,
					game1.player1.color1) == 1)
				return 1;
			if (paint_pixelver(game1.player1.x, game1.player1.y - 1,
					game1.player1.color2) == 1)
				return 1;
			if (paint_pixelver(game1.player1.x, game1.player1.y - 2,
					game1.player1.color3) == 1)
				return 1;
			if (paint_pixelver(game1.player1.x, game1.player1.y + 2,
					game1.player1.color3) == 1)
				return 1;
		}
	} else if (n == 2) {
		if (st == UP || st == DOWN) {
			if (paint_pixelver(game1.player2.x, game1.player2.y,
					game1.player2.color1) == 1)
				return 2;
			if (paint_pixelver(game1.player2.x + 1, game1.player2.y,
					game1.player2.color1) == 1)
				return 2;
			if (paint_pixelver(game1.player2.x - 1, game1.player2.y,
					game1.player2.color2) == 1)
				return 2;
			if (paint_pixelver(game1.player2.x - 2, game1.player2.y,
					game1.player2.color3) == 1)
				return 2;
			if (paint_pixelver(game1.player2.x + 2, game1.player2.y,
					game1.player2.color3) == 1)
				return 2;
		} else if (st == RIGHT || st == LEFT) {
			if (paint_pixelver(game1.player2.x, game1.player2.y,
					game1.player2.color1) == 1)
				return 2;
			if (paint_pixelver(game1.player2.x, game1.player2.y + 1,
					game1.player2.color1) == 1)
				return 2;
			if (paint_pixelver(game1.player2.x, game1.player2.y - 1,
					game1.player2.color2) == 1)
				return 2;
			if (paint_pixelver(game1.player2.x, game1.player2.y - 2,
					game1.player2.color3) == 1)
				return 2;
			if (paint_pixelver(game1.player2.x, game1.player2.y + 2,
					game1.player2.color3) == 1)
				return 2;
		}
	}
}

void change_player_state(unsigned int i, unsigned long data) {
	if (i == 0) {
		if (data == D_MAKE) {
			switch (game1.player1.st) {
			case UP:
				game1.player1.st = RIGHT;
				game1.player1.x += 2;
				game1.player1.y += 2;
				break;
			case RIGHT:
				game1.player1.st = DOWN;
				game1.player1.y += 2;
				game1.player1.x -= 2;
				break;
			case DOWN:
				game1.player1.st = LEFT;
				game1.player1.x -= 2;
				game1.player1.y -= 2;
				break;
			case LEFT:
				game1.player1.st = UP;
				game1.player1.y -= 2;
				game1.player1.x += 2;
				break;
			default:
				break;
			}
		} else if (data == A_MAKE) {
			switch (game1.player1.st) {
			case UP:
				game1.player1.st = LEFT;
				game1.player1.x -= 2;
				game1.player1.y += 2;
				break;
			case RIGHT:
				game1.player1.st = UP;
				game1.player1.y -= 2;
				game1.player1.x -= 2;
				break;
			case DOWN:
				game1.player1.st = RIGHT;
				game1.player1.x += 2;
				game1.player1.y -= 2;
				break;
			case LEFT:
				game1.player1.st = DOWN;
				game1.player1.y += 2;
				game1.player1.x += 2;
				break;
			default:
				break;
			}
		}
	} else if (i == 1) {
		if (data == RARROW_MAKE) {
			switch (game1.player2.st) {
			case UP:
				game1.player2.st = RIGHT;
				game1.player2.x += 2;
				game1.player2.y += 2;
				break;
			case RIGHT:
				game1.player2.st = DOWN;
				game1.player2.y += 2;
				game1.player2.x -= 2;
				break;
			case DOWN:
				game1.player2.st = LEFT;
				game1.player2.x -= 2;
				game1.player2.y -= 2;
				break;
			case LEFT:
				game1.player2.st = UP;
				game1.player2.y -= 2;
				game1.player2.x += 2;
				break;
			default:
				break;
			}
		} else if (data == LARROW_MAKE) {
			switch (game1.player2.st) {
			case UP:
				game1.player2.st = LEFT;
				game1.player2.x -= 2;
				game1.player2.y += 2;
				break;
			case RIGHT:
				game1.player2.st = UP;
				game1.player2.y -= 2;
				game1.player2.x -= 2;
				break;
			case DOWN:
				game1.player2.st = RIGHT;
				game1.player2.x += 2;
				game1.player2.y -= 2;
				break;
			case LEFT:
				game1.player2.st = DOWN;
				game1.player2.y += 2;
				game1.player2.x += 2;
				break;
			default:
				break;
			}
		}
	}
}

void update_player(unsigned int num_players) {
	switch (num_players) {
	case 4:
	case 3:
	case 2:
		if (game1.player2.st == LEFT)
			game1.player2.x--;
		else if (game1.player2.st == RIGHT)
			game1.player2.x++;
		else if (game1.player2.st == UP)
			game1.player2.y--;
		else if (game1.player2.st == DOWN)
			game1.player2.y++;
	case 1:
		if (game1.player1.st == LEFT)
			game1.player1.x--;
		else if (game1.player1.st == RIGHT)
			game1.player1.x++;
		else if (game1.player1.st == UP)
			game1.player1.y--;
		else if (game1.player1.st == DOWN)
			game1.player1.y++;
	default:
		break;
	}

}

int draw_handler(unsigned int num_players) {
	unsigned int loop = 3;
	while (loop) {
		update_player(num_players);
		switch (num_players) {
		case 4:
		case 3:
		case 2:
			if (draw_player(2, game1.player2.st) == 2)
				return 2;
		case 1:
			if (draw_player(1, game1.player1.st) == 1)
				return 1;
		default:
			break;
		}
		loop--;
	}
	return 0;
}

void state_handler(unsigned long data) {
	if (data == ESC_BREAK) {
		//printf(",,,,,,,,,,,\n");
		switch (game1.gamest) {
		case PLAYING:
			//printf("Playing -> PAUSE\n");
			game1.gamest = PAUSE;
			break;
		case PAUSE:
		case FINISHED:
			//printf("QUIT\n");
			game1.gamest = QUIT;
			break;
		case INIT:
			//printf("Init -> Playing\n");
			drawBitmap(game1.board, 0, 0);
			game1.gamest = PLAYING;
			break;
		default:
			break;
		}
	} else if (data == SPACE_BREAK) {
		//printf("-----\n");
		switch (game1.gamest) {
		case PLAYING:
			//printf("Playing -> Pause\n");
			game1.gamest = PAUSE;
			break;
		case INIT:
			//printf("Init -> Playing\n");
			drawBitmap(game1.board, 0, 0);
			//paint_buff();
			game1.gamest = PLAYING;
			break;
		case PAUSE:
			//printf("Pause -> Playing\n");
			game1.gamest = PLAYING;
			break;
		default:
			break;
		}
	}
}

int playgame(unsigned int num_players) {
	int r, ipc_status;
	message msg;
	unsigned int win, twobyteslong = 0;
	unsigned long data;
	//printf("THERE\n");
	game1.irq_set_timer = timer_subscribe_int();
	if (game1.irq_set_timer == -1) {
		printf("Error in timer_subscribe_int()\n");
		return 1;
	}
	game1.irq_set_kbd = kbd_subscribe_int();
	if (game1.irq_set_kbd == -1) {
		printf("Error in kbd_subscribe_int()\n");
		return 1;
	}
	//sub_game();
	kbd_handler();
	while (game1.gamest != QUIT) {
		//printf("22222222\n");
		if (twobyteslong == 0) {
			data = 0;
		}
		if ((r = driver_receive(ANY, &msg, &ipc_status)) != 0) { /*Get a request message.*/
			printf("driver_receive failed with: %d", r);
			continue;
		}
		if (is_ipc_notify(ipc_status)) { /*received notification*/
			//printf("AAAAAAAAAAAAA\n");
			switch (_ENDPOINT_P(msg.m_source)) {
			case HARDWARE: /*hardware interrupt notification*/
				if (msg.NOTIFY_ARG & game1.irq_set_timer) {
					//printf("9999999999\n");
					if (game1.gamest == PLAYING) {
						win = draw_handler(num_players);
						if (win == 1 || win == 2) {
							game1.gamest = FINISHED;
							continue;
						}
					}
				}
				if (msg.NOTIFY_ARG & game1.irq_set_kbd) {
					//printf("11111111\n");
					data |= kbd_handler();
					if (data == TWO_BYTES) {
						data = data << 8;
						twobyteslong = 1;
						continue;
					} else {
						twobyteslong = 0;
						int i;

						if (game1.gamest == PLAYING) {
							for (i = 0; i < num_players; i++)
								change_player_state(i, data);
						}
						state_handler(data);
					}

				}
				break;
			default:
				break;
			}

		} else {

		}

	}
	//printf("33333333333333\n");
	return 0;
}

int start_multigame(unsigned int num_players) {
	game1.gamest = INIT;
	draw_board();
	if (init_players(num_players) != OK) {
		printf("Error in num of players\n");
		return 1;
	}
	draw_borders();

	playgame(num_players);

}

int test_square(unsigned short x, unsigned short y, unsigned short size,
		unsigned long color) {

	unsigned short xline, yline;
	for (xline = x; xline < size + x; xline++) {
		for (yline = y; yline < size + y; yline++) {
			paint_pixel(xline, yline, color);
		}

	}
	return 0;

}
