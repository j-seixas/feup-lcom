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
			if (paint_pixel(game1.player1.x, game1.player1.y,
					game1.player1.color1) == 1)
				return 1;
			if (paint_pixel(game1.player1.x + 1, game1.player1.y,
					game1.player1.color1) == 1)
				return 1;
			if (paint_pixel(game1.player1.x - 1, game1.player1.y,
					game1.player1.color2) == 1)
				return 1;
			if (paint_pixel(game1.player1.x - 2, game1.player1.y,
					game1.player1.color3) == 1)
				return 1;
			if (paint_pixel(game1.player1.x + 2, game1.player1.y,
					game1.player1.color3) == 1)
				return 1;
		} else if (st == RIGHT || st == LEFT) {
			if (paint_pixel(game1.player1.x, game1.player1.y,
					game1.player1.color1) == 1)
				return 1;
			if (paint_pixel(game1.player1.x, game1.player1.y + 1,
					game1.player1.color1) == 1)
				return 1;
			if (paint_pixel(game1.player1.x, game1.player1.y - 1,
					game1.player1.color2) == 1)
				return 1;
			if (paint_pixel(game1.player1.x, game1.player1.y - 2,
					game1.player1.color3) == 1)
				return 1;
			if (paint_pixel(game1.player1.x, game1.player1.y + 2,
					game1.player1.color3) == 1)
				return 1;
		}
	} else if (n == 2) {
		if (st == UP || st == DOWN) {
			if (paint_pixel(game1.player2.x, game1.player2.y,
					game1.player2.color1) == 1)
				return 2;
			if (paint_pixel(game1.player2.x + 1, game1.player2.y,
					game1.player2.color1) == 1)
				return 2;
			if (paint_pixel(game1.player2.x - 1, game1.player2.y,
					game1.player2.color2) == 1)
				return 2;
			if (paint_pixel(game1.player2.x - 2, game1.player2.y,
					game1.player2.color3) == 1)
				return 2;
			if (paint_pixel(game1.player2.x + 2, game1.player2.y,
					game1.player2.color3) == 1)
				return 2;
		} else if (st == RIGHT || st == LEFT) {
			if (paint_pixel(game1.player2.x, game1.player2.y,
					game1.player2.color1) == 1)
				return 2;
			if (paint_pixel(game1.player2.x, game1.player2.y + 1,
					game1.player2.color1) == 1)
				return 2;
			if (paint_pixel(game1.player2.x, game1.player2.y - 1,
					game1.player2.color2) == 1)
				return 2;
			if (paint_pixel(game1.player2.x, game1.player2.y - 2,
					game1.player2.color3) == 1)
				return 2;
			if (paint_pixel(game1.player2.x, game1.player2.y + 2,
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
				break;
			case RIGHT:
				game1.player1.st = DOWN;
				break;
			case DOWN:
				game1.player1.st = LEFT;
				break;
			case LEFT:
				game1.player1.st = UP;
				break;
			default:
				break;
			}
		} else if (data == A_MAKE) {
			switch (game1.player1.st) {
			case UP:
				game1.player1.st = LEFT;
				break;
			case RIGHT:
				game1.player1.st = UP;
				break;
			case DOWN:
				game1.player1.st = RIGHT;
				break;
			case LEFT:
				game1.player1.st = DOWN;
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
				break;
			case RIGHT:
				game1.player2.st = DOWN;
				break;
			case DOWN:
				game1.player2.st = LEFT;
				break;
			case LEFT:
				game1.player2.st = UP;
				break;
			default:
				break;
			}
		} else if (data == LARROW_MAKE) {
			switch (game1.player2.st) {
			case UP:
				game1.player2.st = LEFT;
				break;
			case RIGHT:
				game1.player2.st = UP;
				break;
			case DOWN:
				game1.player2.st = RIGHT;
				break;
			case LEFT:
				game1.player2.st = DOWN;
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
			if(draw_player(2, game1.player2.st) == 2)
				return 2;
		case 1:
			if(draw_player(1, game1.player1.st) == 1)
				return 1;
		default:
			break;
		}
		loop--;
	}
	return 0;
}

int playgame(unsigned int num_players) {
	int r, ipc_status;
	message msg;
	unsigned int win, twobyteslong = 0;
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
					win = draw_handler(num_players);
					if(win == 1 || win == 2){
						data = ESC_BREAK;
						continue;
					}
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
