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
	Bitmap* tron = loadBitmap(getImagePath("Tronboard"));
	drawBitmap(tron, 0, 0);
	game1.irq_set_kbd = kbd_subscribe_int();
	if (game1.irq_set_kbd == -1) {
		printf("Error in kbd_subscribe_int()\n");
		return 1;
	}
	game1.irq_set_timer = timer_subscribe_int();
	if (game1.irq_set_timer == -1) {
		printf("Error in timer_subscribe_int()\n");
		return 1;
	}
	return 0;
}

int init_players(unsigned int num_players) {
	switch (num_players) {
	case 3:
	case 2: {
		game1.player2.x = XRLIMIT - 100;
		game1.player2.y = YULIMIT + 400;
		game1.player2.st = LEFT;
		game1.player2.color1 = ORANGE1;
		game1.player2.color2 = ORANGE2;
		game1.player2.color3 = WHITE;
	}
	case 1: {
		game1.player1.x = XLLIMIT + 100;
		game1.player1.y = YULIMIT + 400;
		game1.player1.st = RIGHT;
		game1.player1.color1 = BLUE1;
		game1.player1.color2 = BLUE2;
		game1.player1.color3 = WHITE;
		game1.player1.Rkey = PRESSED;
		game1.player1.begin = 1;
		return 0;
	}
	default:
		break;
	}

	return 1;
}

void draw_player(unsigned int n, state st) {
	if (n == 1) {
		if (st == UP || st == DOWN) {
			paint_pixel(game1.player1.x, game1.player1.y, game1.player1.color1);
			paint_pixel(game1.player1.x + 1, game1.player1.y,
					game1.player1.color1);
			paint_pixel(game1.player1.x - 1, game1.player1.y,
					game1.player1.color2);
			paint_pixel(game1.player1.x - 2, game1.player1.y,
					game1.player1.color3);
			paint_pixel(game1.player1.x + 2, game1.player1.y,
					game1.player1.color3);
		} else if (st == RIGHT || st == LEFT) {
			paint_pixel(game1.player1.x, game1.player1.y, game1.player1.color1);
			paint_pixel(game1.player1.x, game1.player1.y + 1,
					game1.player1.color1);
			paint_pixel(game1.player1.x, game1.player1.y - 1,
					game1.player1.color2);
			paint_pixel(game1.player1.x, game1.player1.y - 2,
					game1.player1.color3);
			paint_pixel(game1.player1.x, game1.player1.y + 2,
					game1.player1.color3);
		}
	} else if (n == 2) {
		if (st == UP || st == DOWN) {
			paint_pixel(game1.player2.x, game1.player2.y, game1.player2.color1);
			paint_pixel(game1.player2.x + 1, game1.player2.y,
					game1.player2.color1);
			paint_pixel(game1.player2.x - 1, game1.player2.y,
					game1.player2.color2);
			paint_pixel(game1.player2.x - 2, game1.player2.y,
					game1.player2.color3);
			paint_pixel(game1.player2.x + 2, game1.player2.y,
					game1.player2.color3);
		} else if (st == RIGHT || st == LEFT) {
			paint_pixel(game1.player2.x, game1.player2.y, game1.player2.color1);
			paint_pixel(game1.player2.x, game1.player2.y + 1,
					game1.player2.color1);
			paint_pixel(game1.player2.x, game1.player2.y - 1,
					game1.player2.color2);
			paint_pixel(game1.player2.x, game1.player2.y - 2,
					game1.player2.color3);
			paint_pixel(game1.player2.x, game1.player2.y + 2,
					game1.player2.color3);
		}
	}
}

void change_player_state(unsigned int i, unsigned long data) {
	if (i == 0) {
		if(game1.player1.begin){
			game1.player1.begin = 0;
			game1.player1.Rkey = RELEASED;
		}
		if (data == W_MAKE) {
			if (game1.player1.st != DOWN) {
				game1.player1.st = UP;
				game1.player1.Ukey = PRESSED;
			}
		} else if (data == A_MAKE) {
			if (game1.player1.st != RIGHT) {
				game1.player1.st = LEFT;
				game1.player1.Lkey = PRESSED;
			}
		} else if (data == S_MAKE) {
			if (game1.player1.st != UP) {
				game1.player1.st = DOWN;
				game1.player1.Dkey = PRESSED;
			}
		} else if (data == D_MAKE) {
			if (game1.player1.st != LEFT) {
				game1.player1.st = RIGHT;
				game1.player1.Rkey = PRESSED;
			}
		} else if (data == A_BREAK) {
			if (game1.player1.Ukey == PRESSED)
				game1.player1.st = UP;
			else if (game1.player1.Dkey == PRESSED)
				game1.player1.st = DOWN;
			else
				game1.player1.st = LEFT;
			game1.player1.Lkey = RELEASED;

		} else if (data == S_BREAK) {
			if (game1.player1.Rkey == PRESSED)
				game1.player1.st = RIGHT;
			else if (game1.player1.Lkey == PRESSED)
				game1.player1.st = LEFT;
			else
				game1.player1.st = DOWN;

			game1.player1.Dkey = RELEASED;

		} else if (data == D_BREAK) {
			if (game1.player1.Ukey == PRESSED)
				game1.player1.st = UP;
			else if (game1.player1.Dkey == PRESSED)
				game1.player1.st = DOWN;
			else
				game1.player1.st = RIGHT;
			game1.player1.Rkey = RELEASED;

		} else if (data == W_BREAK) {
			if (game1.player1.Rkey == PRESSED)
				game1.player1.st = RIGHT;
			else if (game1.player1.Lkey == PRESSED)
				game1.player1.st = LEFT;
			else
				game1.player1.st = UP;

			game1.player1.Ukey = RELEASED;

		}

	} else if (i == 1) {
		if (data == UARROW_MAKE) {
			if (game1.player2.st != DOWN) {
				game1.player2.st = UP;
				game1.player2.Ukey = PRESSED;
			}
		} else if (data == LARROW_MAKE) {
			if (game1.player2.st != RIGHT) {
				game1.player2.st = LEFT;
				game1.player2.Lkey = PRESSED;
			}
		} else if (data == DARROW_MAKE) {
			if (game1.player2.st != UP) {
				game1.player2.st = DOWN;
				game1.player2.Dkey = PRESSED;
			}
		} else if (data == RARROW_MAKE) {
			if (game1.player2.st != LEFT) {
				game1.player2.st = RIGHT;
				game1.player2.Rkey = PRESSED;
			}
		} else if (data == LARROW_BREAK) {
			if (game1.player2.Ukey == PRESSED)
				game1.player2.st = UP;
			else if (game1.player2.Dkey == PRESSED)
				game1.player2.st = DOWN;
			game1.player2.Lkey = RELEASED;
			game1.player2.st = LEFT;
		} else if (data == DARROW_BREAK) {
			if (game1.player2.Rkey == PRESSED)
				game1.player2.st = RIGHT;
			else if (game1.player2.Lkey == PRESSED)
				game1.player2.st = LEFT;
			game1.player2.Dkey = RELEASED;
			game1.player2.st = DOWN;
		} else if (data == RARROW_BREAK) {
			if (game1.player2.Ukey == PRESSED)
				game1.player2.st = UP;
			else if (game1.player2.Dkey == PRESSED)
				game1.player2.st = DOWN;
			game1.player2.Rkey = RELEASED;
			game1.player2.st = RIGHT;
		} else if (data == UARROW_BREAK) {
			if (game1.player2.Rkey == PRESSED)
				game1.player2.st = RIGHT;
			else if (game1.player2.Lkey == PRESSED)
				game1.player2.st = LEFT;
			game1.player2.Ukey = RELEASED;
			game1.player2.st = UP;
		}
	}
}

void update_player() {
	if (game1.player1.st == LEFT)
		game1.player1.x--;
	else if (game1.player1.st == RIGHT)
		game1.player1.x++;
	else if (game1.player1.st == UP)
		game1.player1.y--;
	else if (game1.player1.st == DOWN)
		game1.player1.y++;

	if (game1.player2.st == LEFT)
		game1.player2.x--;
	else if (game1.player2.st == RIGHT)
		game1.player2.x++;
	else if (game1.player2.st == UP)
		game1.player2.y--;
	else if (game1.player2.st == DOWN)
		game1.player2.y++;
}

int playgame(unsigned int num_players) {
	int r, ipc_status;
	message msg;
	unsigned int twobyteslong = 0;
	unsigned long data;
	while (data != ESC_BREAK) {

		if (twobyteslong == 0) {
			data = 0;
		}
		if ((r = driver_receive(ANY, &msg, &ipc_status)) != 0) { /*Get a request message.*/
			printf("driver_receive failed with: %d", r);
			continue;
		}
		if (is_ipc_notify(ipc_status)) { /*received notification*/
			switch (_ENDPOINT_P(msg.m_source)) {
			case HARDWARE: /*hardware interrupt notification*/
				if (msg.NOTIFY_ARG & game1.irq_set_timer) {
					update_player();
					draw_player(1, game1.player1.st);
					draw_player(2, game1.player2.st);
					update_player();
					draw_player(1, game1.player1.st);
					draw_player(2, game1.player2.st);
				}
				if (msg.NOTIFY_ARG & game1.irq_set_kbd) { /*subscribed interrupt*/
					data |= kbd_handler();
					if (data == TWO_BYTES) {
						data = data << 8;
						twobyteslong = 1;
						continue;
					} else {
						twobyteslong = 0;
						int i;
						for (i = 0; i < num_players; i++)
							change_player_state(i, data);
					}
					break;
				}
			default:
				break;
			}

		} else {

		}

	}
	return 0;
}

int start_multigame(unsigned int num_players) {
	draw_board();
	if (init_players(num_players) != OK) {
		printf("Error in num of players\n");
		return 1;
	}
	playgame(num_players);

}

int test_bmp(unsigned short xi, unsigned short yi, char *bmp) {
	return 0;
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
