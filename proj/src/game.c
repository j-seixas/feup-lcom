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

void loadBit() {
	game1.board2.board = loadBitmap(getImagePath("Game2p"));
	game1.board2.boardp = loadBitmap(getImagePath("Game2p_1"));
	game1.board3.board = loadBitmap(getImagePath("Game3p"));
	game1.board3.boardp = loadBitmap(getImagePath("Game3p_1"));
	game1.board4.board = loadBitmap(getImagePath("Game4p"));
	game1.board4.boardp = loadBitmap(getImagePath("Game4p_1"));
	game1.menu = loadBitmap(getImagePath("Menu"));
	game1.mouse = loadBitmap(getImagePath("mouse"));
	game1.start = loadBitmap(getImagePath("Start"));
	game1.player1.win = loadBitmap(getImagePath("win1"));
	game1.player2.win = loadBitmap(getImagePath("win2"));
	game1.player3.win = loadBitmap(getImagePath("win3"));
	game1.player4.win = loadBitmap(getImagePath("win4"));
	game1.pause = loadBitmap(getImagePath("Pause1"));
	game1.draw = loadBitmap(getImagePath("draw"));
}

int draw_board(unsigned int num_players) {
	if (game1.gamest == PAUSE) {
		switch (num_players) {
		case 4:
			drawBitmap(game1.board4.boardp, 0, 0, 0);
			break;
		case 3:
			drawBitmap(game1.board3.boardp, 0, 0, 0);
			break;
		case 2:
			drawBitmap(game1.board2.boardp, 0, 0, 0);
			break;
		default:
			break;
		}
	} else {
		switch (num_players) {
		case 4:
			drawBitmap(game1.board4.board, 0, 0, 0);
			break;
		case 3:
			drawBitmap(game1.board3.board, 0, 0, 0);
			break;
		case 2:
			drawBitmap(game1.board2.board, 0, 0, 0);
			break;
		default:
			break;
		}
	}
	return 0;
}

void init_mouse() {
	game1.mouse1.x = 640;
	game1.mouse1.y = 512;
	game1.mouse1.left = KUP;
	game1.mouse1.right = KUP;
	drawBitmap(game1.menu, 0, 0, 0);
	drawBitmap(game1.mouse, game1.mouse1.x, game1.mouse1.y, 0);
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
		game1.player4.x = 650;
		game1.player4.y = YULIMIT + 100;
		game1.player4.st = DOWN;
		game1.player4.color1 = PINK1;
		game1.player4.color2 = PINK1;
		game1.player4.color3 = WHITE;
		game1.player4.lost = 0;
	}
	case 3: {
		game1.player3.x = 650;
		game1.player3.y = YDLIMIT - 100;
		game1.player3.st = UP;
		game1.player3.color1 = GREEN1;
		game1.player3.color2 = GREEN1;
		game1.player3.color3 = WHITE;
		game1.player3.left = B_MAKE;
		game1.player3.right = M_MAKE;
		game1.player3.lost = 0;
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
		game1.player2.lost = 0;
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
		game1.player1.lost = 0;
		return 0;
	}
	default:
		break;
	}

	return 1;
}

int draw_player(player_t *p, state_t st) {
	int lost = 0;
	if (st == UP || st == DOWN) {
		if (paint_pixelver(p->x, p->y, p->color1) == 1)
			lost = 1;
		if (paint_pixelver(p->x + 1, p->y, p->color1) == 1)
			lost = 1;
		if (paint_pixelver(p->x - 1, p->y, p->color2) == 1)
			lost = 1;
		if (paint_pixelver(p->x - 2, p->y, p->color3) == 1)
			lost = 1;
		if (paint_pixelver(p->x + 2, p->y, p->color3) == 1)
			lost = 1;
	} else if (st == RIGHT || st == LEFT) {
		if (paint_pixelver(p->x, p->y, p->color1) == 1)
			lost = 1;
		if (paint_pixelver(p->x, p->y + 1, p->color1) == 1)
			lost = 1;
		if (paint_pixelver(p->x, p->y - 1, p->color2) == 1)
			lost = 1;
		if (paint_pixelver(p->x, p->y - 2, p->color3) == 1)
			lost = 1;
		if (paint_pixelver(p->x, p->y + 2, p->color3) == 1)
			lost = 1;
	}
	return lost;
}

void change_plst_handler(unsigned int num_players, unsigned long data) {
	switch (num_players) {
	case 4:
	case 3:
		if (game1.player3.lost == 0)
			change_player_state(&game1.player3, data);
	case 2:
		if (game1.player2.lost == 0)
			change_player_state(&game1.player2, data);
	case 1:
		if (game1.player1.lost == 0)
			change_player_state(&game1.player1, data);
	default:
		break;
	}
}

void change_player_state(player_t *p, unsigned long data) {
	if (data == p->right) {
		switch (p->st) {
		case UP:
			p->st = RIGHT;
			p->x += 2;
			p->y += 2;
			break;
		case RIGHT:
			p->st = DOWN;
			p->y += 2;
			p->x -= 2;
			break;
		case DOWN:
			p->st = LEFT;
			p->x -= 2;
			p->y -= 2;
			break;
		case LEFT:
			p->st = UP;
			p->y -= 2;
			p->x += 2;
			break;
		default:
			break;
		}
	} else if (data == p->left) {
		switch (p->st) {
		case UP:
			p->st = LEFT;
			p->x -= 2;
			p->y += 2;
			break;
		case RIGHT:
			p->st = UP;
			p->y -= 2;
			p->x -= 2;
			break;
		case DOWN:
			p->st = RIGHT;
			p->x += 2;
			p->y -= 2;
			break;
		case LEFT:
			p->st = DOWN;
			p->y += 2;
			p->x += 2;
			break;
		default:
			break;
		}
	}

}

void update_player(unsigned int num_players) {
	switch (num_players) {
	case 4:
		if (game1.player4.st == LEFT)
			game1.player4.x--;
		else if (game1.player4.st == RIGHT)
			game1.player4.x++;
		else if (game1.player4.st == UP)
			game1.player4.y--;
		else if (game1.player4.st == DOWN)
			game1.player4.y++;
	case 3:
		if (game1.player3.st == LEFT)
			game1.player3.x--;
		else if (game1.player3.st == RIGHT)
			game1.player3.x++;
		else if (game1.player3.st == UP)
			game1.player3.y--;
		else if (game1.player3.st == DOWN)
			game1.player3.y++;
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
			if (game1.player4.lost == 0) {
				if (draw_player(&game1.player4, game1.player4.st) == 1) {
					game1.player4.lost = 1;
					game1.lost++;
				}
			}
		case 3:
			if (game1.player3.lost == 0) {
				if (draw_player(&game1.player3, game1.player3.st) == 1) {
					game1.player3.lost = 1;
					game1.lost++;
				}
			}
		case 2:
			if (game1.player2.lost == 0) {
				if (draw_player(&game1.player2, game1.player2.st) == 1) {
					game1.player2.lost = 1;
					game1.lost++;
				}
			}
		case 1:
			if (game1.player1.lost == 0) {
				if (draw_player(&game1.player1, game1.player1.st) == 1) {
					game1.player1.lost = 1;
					game1.lost++;
				}
			}
		default:
			break;
		}
		loop--;
	}
	return 0;
}

void state_handler(unsigned int num_players, unsigned long data) {
	if (data == ESC_BREAK) {
		switch (game1.gamest) {
		case INIT:
		case PLAYING:
		case PAUSE:
		case FINISHED:
			game1.gamest = MENU;
			init_mouse();
			drawBitmap(game1.menu, 0, 0, 0);
			drawBitmap(game1.mouse, game1.mouse1.x, game1.mouse1.y, 0);
			break;
		case MENU:
			game1.gamest = QUIT;
		default:
			break;
		}
	} else if (data == SPACE_BREAK) {
		switch (game1.gamest) {
		case PLAYING:
			drawBitmap(game1.pause, 0, 0, 0);
			game1.gamest = PAUSE;
			break;
		case INIT:
			draw_board(num_players);
			draw_borders();
			game1.gamest = PLAYING;
			break;
		case PAUSE:
			draw_board(num_players);
			game1.gamest = PLAYING;
			break;
		case FINISHED:
			init_players(num_players);
			game1.gamest = PLAYING;
			game1.lost = 0;
			draw_board(num_players);
			break;
		default:
			break;
		}
	} else if (data == NUM2_BREAK) {
		if (game1.gamest == MENU) {
			game1.num_players = 2;
			start_multigame(2);
		}
	} else if (data == NUM3_BREAK) {
		if (game1.gamest == MENU) {
			game1.num_players = 3;
			start_multigame(3);
		}
	} else if (data == NUM4_BREAK) {
		if (game1.gamest == MENU) {
			game1.num_players = 4;
			start_multigame(4);
		}
	}
}

int check_mouse() {
	if (game1.mouse1.x >= MENU1L && game1.mouse1.x <= MENU1R
			&& game1.mouse1.y >= MENU1U && game1.mouse1.y <= MENU1D) {
		return 1;
	} else if (game1.mouse1.x >= MENU2L && game1.mouse1.x <= MENU2R
			&& game1.mouse1.y >= MENU2U && game1.mouse1.y <= MENU2D) {
		return 2;
	} else if (game1.mouse1.x >= MENU3L && game1.mouse1.x <= MENU3R
			&& game1.mouse1.y >= MENU3U && game1.mouse1.y <= MENU3D) {
		return 3;
	} else if (game1.mouse1.x >= MENU4L && game1.mouse1.x <= MENU4R
			&& game1.mouse1.y >= MENU4U && game1.mouse1.y <= MENU4D) {
		return 4;
	}
	return 0;
}

int sub_game() {
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

	kbd_handler();
	game1.irq_set_mouse = mouse_subscribe_int();
	if (game1.irq_set_mouse == -1) {
		printf("Error in mouse_subscribe_int()\n");
		return 1;
	}

	if (mouse_send(MOUSE_ENB) != 0) {
		printf("Error in mouse sending command\n");
		return 1;

	}
	return 0;
}

int mouse_mov_handler(unsigned long mouse_packet[3]) {

	unsigned int check = 0;
	int menu, testx, testy;
	if ((mouse_packet[0] & BIT(6)) == 0 && (mouse_packet[0] & BIT(7)) == 0) {
		if ((mouse_packet[0] & BIT(0)) != 0)
			game1.mouse1.left = KDOWN;
		else {
			if (game1.mouse1.left == KDOWN) {
				game1.mouse1.left = KUP;
				check = 1;
			}
		}

		if ((mouse_packet[0] & BIT(4)) != 0) {
			mouse_packet[1] ^= 0xFF;
			mouse_packet[1]++;
			testx = -mouse_packet[1];
		} else {
			testx = mouse_packet[1];

		}
		game1.mouse1.x += testx;
		if ((mouse_packet[0] & BIT(5)) != 0) {
			mouse_packet[2] ^= 0xFF;
			mouse_packet[2]++;
			testy = mouse_packet[2];

		} else {
			testy = -mouse_packet[2];

		}
		game1.mouse1.y += testy;

		if (check) {
			menu = check_mouse();
			switch (menu) {
			case 4:
				game1.gamest = QUIT;
				break;
			case 3:
				game1.num_players = 4;
				start_multigame(4);
				break;
			case 2:
				game1.num_players = 3;
				start_multigame(3);
				break;
			case 1:
				game1.num_players = 2;
				start_multigame(2);
				break;
			default:
				break;
			}

		}

		return 1;
	}
	return 0;

}

void mouse_st_handler(player_t *p, unsigned long mouse_packet[3]) {
	unsigned int check = 0;
	if ((mouse_packet[0] & BIT(6)) == 0 && (mouse_packet[0] & BIT(7)) == 0) {
		if ((mouse_packet[0] & BIT(0)) != 0) {
			if (game1.mouse1.left != KDOWN)
				check = 1;
			game1.mouse1.left = KDOWN;
		} else
			game1.mouse1.left = KUP;

		if ((mouse_packet[0] & BIT(1)) != 0) {
			if (game1.mouse1.right != KDOWN)
				check = 2;
			game1.mouse1.right = KDOWN;
		} else
			game1.mouse1.right = KUP;

	}

	if (check == 1) {
		switch (p->st) {
		case UP:
			p->st = LEFT;
			p->x -= 2;
			p->y += 2;
			break;
		case RIGHT:
			p->st = UP;
			p->y -= 2;
			p->x -= 2;
			break;
		case DOWN:
			p->st = RIGHT;
			p->x += 2;
			p->y -= 2;
			break;
		case LEFT:
			p->st = DOWN;
			p->y += 2;
			p->x += 2;
			break;
		default:
			break;
		}
	} else if (check == 2) {
		switch (p->st) {
		case UP:
			p->st = RIGHT;
			p->x += 2;
			p->y += 2;
			break;
		case RIGHT:
			p->st = DOWN;
			p->y += 2;
			p->x -= 2;
			break;
		case DOWN:
			p->st = LEFT;
			p->x -= 2;
			p->y -= 2;
			break;
		case LEFT:
			p->st = UP;
			p->y -= 2;
			p->x += 2;
			break;
		default:
			break;
		}
	}

}

void check_winner() {

	switch (game1.num_players) {
	case 4:
		if (game1.player1.lost) {
			if (game1.player2.lost) {
				if (game1.player4.lost) {
					if (game1.player3.lost) {
						drawBitmap(game1.draw, 255, 360, 0);
					} else
						drawBitmap(game1.player3.win, 255, 360, 0);
				} else
					drawBitmap(game1.player4.win, 255, 360, 0);
			} else
				drawBitmap(game1.player2.win, 255, 360, 0);
		} else
			drawBitmap(game1.player1.win, 255, 360, 0);
		break;
	case 3:
		if (game1.player1.lost) {
			if (game1.player2.lost) {
				if (game1.player3.lost) {
					drawBitmap(game1.draw, 255, 360, 0);
				} else
					drawBitmap(game1.player3.win, 255, 360, 0);
			} else
				drawBitmap(game1.player2.win, 255, 360, 0);
		} else
			drawBitmap(game1.player1.win, 255, 360, 0);
		break;
	case 2:
		if (game1.player1.lost) {
			if (game1.player2.lost) {
				drawBitmap(game1.draw, 255, 360, 0);
			} else
				drawBitmap(game1.player2.win, 255, 360, 0);
		} else
			drawBitmap(game1.player1.win, 255, 360, 0);
		break;
	default:
		break;
	}
}

void timer_intrhandler() {
	if (game1.gamest == PLAYING) {
		draw_handler(game1.num_players);
		if (game1.lost + 1 >= game1.num_players) {
			game1.gamest = FINISHED;
			check_winner();
		}
	} else if (game1.gamest == MENU) {
		if (game1.mouse1.paint) {
			drawBitmap(game1.menu, 0, 0, 0);
			drawBitmap(game1.mouse, game1.mouse1.x, game1.mouse1.y, 0);
			game1.mouse1.paint = 0;
		}
	}
}

void kbd_intrhandler(unsigned long datakbd) {
	if (game1.gamest == PLAYING) {
		change_plst_handler(game1.num_players, datakbd);
	}
	state_handler(game1.num_players, datakbd);
}

void mouse_intrhandler(unsigned long mouse_packet[3]) {
	if (game1.gamest == MENU) {
		game1.mouse1.paint = mouse_mov_handler(mouse_packet);
	} else if (game1.gamest == PLAYING && game1.num_players == 4) {
		mouse_st_handler(&game1.player4, mouse_packet);

	}
}

int playgame() {
	loadBit();
	game1.gamest = MENU;
	int r, ipc_status, menu;
	message msg;
	unsigned int win, twobyteslong = 0, packet = 0, counter = 0;
	unsigned long datakbd, datamouse, mouse_packet[3];

	init_mouse();

	while (game1.gamest != QUIT) {
		if (twobyteslong == 0) {
			datakbd = 0;
		}
		if ((r = driver_receive(ANY, &msg, &ipc_status)) != 0) {
			printf("driver_receive failed with: %d", r);
			continue;
		}
		if (is_ipc_notify(ipc_status)) {
			switch (_ENDPOINT_P(msg.m_source)) {
			case HARDWARE:
				if (msg.NOTIFY_ARG & game1.irq_set_timer) {
					timer_intrhandler();
				}
				if (msg.NOTIFY_ARG & game1.irq_set_kbd) {
					datakbd |= kbd_handler();
					if (datakbd == TWO_BYTES) {
						datakbd = datakbd << 8;
						twobyteslong = 1;
					} else {
						twobyteslong = 0;
						kbd_intrhandler(datakbd);
					}

				}
				if (msg.NOTIFY_ARG & game1.irq_set_mouse) {
					datamouse = kbd_handler();
					if (datamouse == -1)
						break;
					if (packet == 0) {
						if ((datamouse & BIT(3)) == OK)
							break;

					}
					mouse_packet[packet] = datamouse;
					packet++;
					if (packet == 3) {
						packet = 0;
						mouse_intrhandler(mouse_packet);
					}
				}
				break;
			default:
				break;
			}

		} else {

		}

	}
	return 0;
}

int start_multigame(unsigned int num_players) {
	game1.gamest = INIT;
	game1.lost = 0;
	draw_board(num_players);
	drawBitmap(game1.start, 255, 460, 0);
	if (init_players(num_players) != OK) {
		printf("Error in num of players\n");
		return 1;
	}
	draw_borders();

	return 0;
}

