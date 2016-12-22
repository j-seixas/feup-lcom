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
	game1.board = loadBitmap(getImagePath("Game4p"));
	game1.menu = loadBitmap(getImagePath("Menu"));
	game1.mouse = loadBitmap(getImagePath("mouse"));
	game1.start = loadBitmap(getImagePath("Start"));
}
int draw_board() {

	drawBitmap(game1.board, 0, 0, 0);

	drawBitmap(game1.start, 255, 460, 0);
	//paint_buff();
	//kbd_test_scan();
	//unsub_game();
	//sub_game();
	printf(".........\n");
	//drawBitmap(tron2p, 0, 0);
	return 0;

}

void init_mouse() {
	game1.mouse1.x = 640;
	game1.mouse1.y = 512;
	game1.mouse1.type = LUP;
	game1.mouse1.st = DRAW;
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
		game1.player4.x = 650; //(XRLIMIT - XLLIMIT) / 2;
		//printf("%d\n", game1.player4.x);
		game1.player4.y = YULIMIT + 100;
		game1.player4.st = DOWN;
		game1.player4.color1 = PINK1;
		game1.player4.color2 = PINK1;
		game1.player4.color3 = WHITE;
		game1.player4.left = I_MAKE;
		game1.player4.right = P_MAKE;
	}
	case 3: {
		game1.player3.x = 650; // (XRLIMIT - XLLIMIT) / 2;
		//printf("%d\n", game1.player3.x);
		game1.player3.y = YDLIMIT - 100;
		game1.player3.st = UP;
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

int draw_player(player *p, state st) {
	if (st == UP || st == DOWN) {
		if (paint_pixelver(p->x, p->y, p->color1) == 1)
			return 1;
		if (paint_pixelver(p->x + 1, p->y, p->color1) == 1)
			return 1;
		if (paint_pixelver(p->x - 1, p->y, p->color2) == 1)
			return 1;
		if (paint_pixelver(p->x - 2, p->y, p->color3) == 1)
			return 1;
		if (paint_pixelver(p->x + 2, p->y, p->color3) == 1)
			return 1;
	} else if (st == RIGHT || st == LEFT) {
		if (paint_pixelver(p->x, p->y, p->color1) == 1)
			return 1;
		if (paint_pixelver(p->x, p->y + 1, p->color1) == 1)
			return 1;
		if (paint_pixelver(p->x, p->y - 1, p->color2) == 1)
			return 1;
		if (paint_pixelver(p->x, p->y - 2, p->color3) == 1)
			return 1;
		if (paint_pixelver(p->x, p->y + 2, p->color3) == 1)
			return 1;
	}
}

void change_plst_handler(unsigned int num_players, unsigned long data) {
	switch (num_players) {
	case 4:
		change_player_state(&game1.player4, data);
	case 3:
		change_player_state(&game1.player3, data);
	case 2:
		change_player_state(&game1.player2, data);
	case 1:
		change_player_state(&game1.player1, data);
	default:
		break;
	}
}

void change_player_state(player *p, unsigned long data) {
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
			if (draw_player(&game1.player4, game1.player4.st) == 1)
				return 4;
		case 3:
			if (draw_player(&game1.player3, game1.player3.st) == 1)
				return 3;
		case 2:
			if (draw_player(&game1.player2, game1.player2.st) == 1)
				return 2;
		case 1:
			if (draw_player(&game1.player1, game1.player1.st) == 1)
				return 1;
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
		case PLAYING:
			game1.gamest = PAUSE;
			break;
		case PAUSE:
		case FINISHED:
			game1.gamest = MENU;
			init_mouse();
			drawBitmap(game1.menu, 0, 0, 0);
			drawBitmap(game1.mouse, game1.mouse1.x, game1.mouse1.y, 0);
			break;
		case INIT:
			drawBitmap(game1.board, 0, 0, 0);
			draw_borders();
			game1.gamest = PLAYING;
			break;
		case MENU:
			game1.gamest = QUIT;
		default:
			break;
		}
	} else if (data == SPACE_BREAK) {
		switch (game1.gamest) {
		case PLAYING:
			game1.gamest = PAUSE;
			break;
		case MENU:
			game1.num_players = 4;
			start_multigame(4);
			break;
		case INIT:
			drawBitmap(game1.board, 0, 0, 0);
			draw_borders();
			//paint_buff();
			game1.gamest = PLAYING;
			break;
		case PAUSE:
			game1.gamest = PLAYING;
			break;
		case FINISHED:
			init_players(num_players);
			game1.gamest = INIT;
			drawBitmap(game1.board, 0, 0, 0);
			drawBitmap(game1.start, 255, 460, 0);
			break;
		default:
			break;
		}
	}
}

int check_mouse() {
	if (game1.mouse1.x >= MENU1L && game1.mouse1.x <= MENU1R
			&& game1.mouse1.y >= MENU1U && game1.mouse1.y <= MENU1D) {
		game1.mouse1.st = COMP;
		return 1;
	} else if (game1.mouse1.x >= MENU2L && game1.mouse1.x <= MENU2R
			&& game1.mouse1.y >= MENU2U && game1.mouse1.y <= MENU2D) {
		game1.mouse1.st = COMP;
		return 2;
	} else if (game1.mouse1.x >= MENU3L && game1.mouse1.x <= MENU3R
			&& game1.mouse1.y >= MENU3U && game1.mouse1.y <= MENU3D) {
		game1.mouse1.st = COMP;
		return 3;
	} else if (game1.mouse1.x >= MENU4L && game1.mouse1.x <= MENU4R
			&& game1.mouse1.y >= MENU4U && game1.mouse1.y <= MENU4D) {
		game1.mouse1.st = COMP;
		return 4;
	}
	return 0;
}

int mouse_mov_handler(unsigned long mouse_packet[3]) {

	unsigned int check = 0;
	int menu, testx, testy;
	if ((mouse_packet[0] & BIT(6)) == 0 && (mouse_packet[0] & BIT(7)) == 0) {
		if ((mouse_packet[0] & BIT(0)) != 0)
			game1.mouse1.type = LDOWN;
		//printf("LB=1 ");
		else {
			if (game1.mouse1.type == LDOWN) {
				game1.mouse1.type = LUP;
				check = 1;
			}
		}
		/*printf("LB=0 ");
		 if ((packet[0] & BIT(2)) != 0)
		 printf("MB=1 ");
		 else
		 printf("MB=0 ");
		 if ((packet[0] & BIT(1)) != 0) {
		 printf("RB=1 ");
		 if (evt.state == INIT)
		 evt.type = RDOWN;
		 else
		 evt.type = MOVE;
		 } else {
		 printf("RB=0 ");
		 evt.state = INIT;
		 evt.type = RUP;
		 }*/
		/*if ((packet[0] & BIT(6)) != 0)
		 printf("XOV=1 ");
		 else
		 printf("XOV=0 ");
		 if ((packet[0] & BIT(7)) != 0)
		 printf("YOV=1 ");
		 else
		 printf("YOV=0 ");*/
		if ((mouse_packet[0] & BIT(4)) != 0) {
			mouse_packet[1] ^= 0xFF;
			mouse_packet[1]++;
			printf("X=-%d ", mouse_packet[1]);
			testx = -mouse_packet[1];
		} else {
			printf("X=%d ", mouse_packet[1]);
			testx = mouse_packet[1];

		}
		game1.mouse1.x += testx;
		if ((mouse_packet[0] & BIT(5)) != 0) {
			mouse_packet[2] ^= 0xFF;
			mouse_packet[2]++;
			testy = mouse_packet[2];
			printf("Y=-%d\n", mouse_packet[2]);

		} else {
			printf("Y=%d \n", mouse_packet[2]);
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
	printf("OVERFLOW");
	return 0;

}

int playgame() {
	loadBit();
	game1.gamest = MENU;
	int r, ipc_status, menu;
	message msg;
	unsigned int win, twobyteslong = 0, packet = 0, counter = 0, boolpaintmouse = 0;
	unsigned long datakbd, datamouse, mouse_packet[3];
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

	game1.irq_set_mouse = mouse_subscribe_int();
	if (game1.irq_set_mouse == -1) {
		printf("Error in mouse_subscribe_int()\n");
		return 1;
	}

	//kbd_handler();
	if (mouse_send(MOUSE_ENB) != 0) {
		printf("Error in mouse sending command\n");
		return 1;
	}

	init_mouse();
//	drawBitmap(game1.board, 0, 0, 0);
	drawBitmap(game1.menu, 0, 0, 0);
	drawBitmap(game1.mouse, game1.mouse1.x, game1.mouse1.y, 0);
	//paint_buff();
//	drawBitmap(game1.board, 0, 0, 0);
	printf("IN\n");
	while (game1.gamest != QUIT) {
		if (twobyteslong == 0) {
			datakbd = 0;
		}
		if ((r = driver_receive(ANY, &msg, &ipc_status)) != 0) { /*Get a request message.*/
			printf("driver_receive failed with: %d", r);
			continue;
		}
		if (is_ipc_notify(ipc_status)) { /*received notification*/
			switch (_ENDPOINT_P(msg.m_source)) {
			case HARDWARE: /*hardware interrupt notification*/
				if (msg.NOTIFY_ARG & game1.irq_set_timer) {
					printf("...TIMER----------\n");
					if (game1.gamest == PLAYING) {
						win = draw_handler(game1.num_players);
						if (win == 1 || win == 2 || win == 3 || win == 4) {
							game1.gamest = FINISHED;
							continue;
						}
					} else if (game1.gamest == MENU) {
						if(boolpaintmouse){
						drawBitmap(game1.menu, 0, 0, 0);
						drawBitmap(game1.mouse, game1.mouse1.x, game1.mouse1.y,
								0);
						boolpaintmouse = 0;
						}
					}
				}
				if (msg.NOTIFY_ARG & game1.irq_set_kbd) {
					printf("---- KBD ----\n");
					datakbd |= kbd_handler();
					if (datakbd == TWO_BYTES) {
						datakbd = datakbd << 8;
						twobyteslong = 1;
						continue;
					} else {
						twobyteslong = 0;
						int i;

						if (game1.gamest == PLAYING) {
							change_plst_handler(game1.num_players, datakbd);
						}
						state_handler(game1.num_players, datakbd);
					}

				}
				if (msg.NOTIFY_ARG & game1.irq_set_mouse) {
					printf("----->> MOUSE\n");
					datamouse = kbd_handler();
					if (datamouse == -1) {
						break;
					}
					if (packet == 0) {
						if ((datamouse & BIT(3)) == OK) {
							break;
						}
					}
					mouse_packet[packet] = datamouse;
					packet++;
					if (packet == 3) {
						packet = 0;
						if (game1.gamest == MENU) {
							boolpaintmouse = mouse_mov_handler(mouse_packet);
						}
					}
				}
				break;
			default:
				break;
			}

		} else {

		}

	}
	printf("OUT\n");
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

	//playgame(num_players);
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
