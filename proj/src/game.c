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


void load_Bmps(game_t* game1) {
	game1->board2.board = loadBitmap(getImagePath("Game2p"));
	game1->board2.boardp = loadBitmap(getImagePath("Game2p_1"));
	game1->board3.board = loadBitmap(getImagePath("Game3p"));
	game1->board3.boardp = loadBitmap(getImagePath("Game3p_1"));
	game1->board4.board = loadBitmap(getImagePath("Game4p"));
	game1->board4.boardp = loadBitmap(getImagePath("Game4p_1"));
	game1->menu = loadBitmap(getImagePath("Menu"));
	game1->mouse = loadBitmap(getImagePath("mouse"));
	game1->start = loadBitmap(getImagePath("Start"));
	game1->player1.win = loadBitmap(getImagePath("win1"));
	game1->player2.win = loadBitmap(getImagePath("win2"));
	game1->player3.win = loadBitmap(getImagePath("win3"));
	game1->player4.win = loadBitmap(getImagePath("win4"));
	game1->pause = loadBitmap(getImagePath("Pause1"));
	game1->draw = loadBitmap(getImagePath("draw"));
}

void change_plst_handler(unsigned int num_players, unsigned long data, game_t *game1) {
	switch (num_players) {
	case 4:
	case 3:
		if (game1->player3.lost == 0)
			change_player_state(&game1->player3, data);
	case 2:
		if (game1->player2.lost == 0)
			change_player_state(&game1->player2, data);
	case 1:
		if (game1->player1.lost == 0)
			change_player_state(&game1->player1, data);
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

void update_player(unsigned int num_players, game_t *game1) {
	switch (num_players) {
	case 4:
		if (game1->player4.st == LEFT)
			game1->player4.x--;
		else if (game1->player4.st == RIGHT)
			game1->player4.x++;
		else if (game1->player4.st == UP)
			game1->player4.y--;
		else if (game1->player4.st == DOWN)
			game1->player4.y++;
	case 3:
		if (game1->player3.st == LEFT)
			game1->player3.x--;
		else if (game1->player3.st == RIGHT)
			game1->player3.x++;
		else if (game1->player3.st == UP)
			game1->player3.y--;
		else if (game1->player3.st == DOWN)
			game1->player3.y++;
	case 2:
		if (game1->player2.st == LEFT)
			game1->player2.x--;
		else if (game1->player2.st == RIGHT)
			game1->player2.x++;
		else if (game1->player2.st == UP)
			game1->player2.y--;
		else if (game1->player2.st == DOWN)
			game1->player2.y++;
	case 1:
		if (game1->player1.st == LEFT)
			game1->player1.x--;
		else if (game1->player1.st == RIGHT)
			game1->player1.x++;
		else if (game1->player1.st == UP)
			game1->player1.y--;
		else if (game1->player1.st == DOWN)
			game1->player1.y++;
	default:
		break;
	}

}

void draw_handler(unsigned int num_players, game_t *game1) {
	unsigned int loop = 3;
	while (loop) {
		update_player(num_players, &(*game1));
		switch (num_players) {
		case 4:
			if (game1->player4.lost == 0) {
				if (draw_player(&game1->player4, game1->player4.st) == 1) {
					game1->player4.lost = 1;
					game1->lost++;
				}
			}
		case 3:
			if (game1->player3.lost == 0) {
				if (draw_player(&game1->player3, game1->player3.st) == 1) {
					game1->player3.lost = 1;
					game1->lost++;
				}
			}
		case 2:
			if (game1->player2.lost == 0) {
				if (draw_player(&game1->player2, game1->player2.st) == 1) {
					game1->player2.lost = 1;
					game1->lost++;
				}
			}
		case 1:
			if (game1->player1.lost == 0) {
				if (draw_player(&game1->player1, game1->player1.st) == 1) {
					game1->player1.lost = 1;
					game1->lost++;
				}
			}
		default:
			break;
		}
		loop--;
	}

}

void state_handler(unsigned int num_players, unsigned long data, game_t *game1) {
	if (data == ESC_BREAK) {
		switch (game1->gamest) {
		case INIT:
		case PLAYING:
		case PAUSED:
		case FINISHED:
			game1->gamest = MENU;
			init_mouse(&(*game1));
			drawBitmap(game1->menu, 0, 0, 0);
			drawBitmap(game1->mouse, game1->mouse1.x, game1->mouse1.y, 0);
			break;
		case MENU:
			game1->gamest = QUIT;
		default:
			break;
		}
	} else if (data == SPACE_BREAK) {
		switch (game1->gamest) {
		case PLAYING:
			drawBitmap(game1->pause, 0, 0, 0);
			game1->gamest = PAUSED;
			break;
		case INIT:
			draw_board(num_players, &(*game1));
			draw_borders();
			game1->gamest = PLAYING;
			break;
		case PAUSED:
			draw_board(num_players, &(*game1));
			game1->gamest = PLAYING;
			break;
		case FINISHED:
			init_players(num_players, &(*game1));
			game1->gamest = PLAYING;
			game1->lost = 0;
			draw_board(num_players, &(*game1));
			break;
		default:
			break;
		}
	} else if (data == NUM2_BREAK) {
		if (game1->gamest == MENU) {
			game1->num_players = 2;
			start_multigame(2, &(*game1));
		}
	} else if (data == NUM3_BREAK) {
		if (game1->gamest == MENU) {
			game1->num_players = 3;
			start_multigame(3, &(*game1));
		}
	} else if (data == NUM4_BREAK) {
		if (game1->gamest == MENU) {
			game1->num_players = 4;
			start_multigame(4, &(*game1));
		}
	}
}

int check_mouse(game_t *game1) {
	if (game1->mouse1.x + 8 >= MENU1L && game1->mouse1.x + 8 <= MENU1R
			&& game1->mouse1.y + 2 >= MENU1U && game1->mouse1.y + 2 <= MENU1D) {
		return 1;
	} else if (game1->mouse1.x + 8 >= MENU2L && game1->mouse1.x + 8 <= MENU2R
			&& game1->mouse1.y + 2 >= MENU2U && game1->mouse1.y + 2 <= MENU2D) {
		return 2;
	} else if (game1->mouse1.x + 8 >= MENU3L && game1->mouse1.x + 8 <= MENU3R
			&& game1->mouse1.y + 2 >= MENU3U && game1->mouse1.y + 2 <= MENU3D) {
		return 3;
	} else if (game1->mouse1.x + 8 >= MENU4L && game1->mouse1.x + 8 <= MENU4R
			&& game1->mouse1.y + 2 >= MENU4U && game1->mouse1.y + 2 <= MENU4D) {
		return 4;
	}
	return 0;
}



int mouse_mov_handler(unsigned long mouse_packet[3], game_t *game1) {

	unsigned int check = 0;
	int menu, testx, testy;
	if ((mouse_packet[0] & BIT(6)) == 0 && (mouse_packet[0] & BIT(7)) == 0) {
		if ((mouse_packet[0] & BIT(0)) != 0)
			game1->mouse1.left = KDOWN;
		else {
			if (game1->mouse1.left == KDOWN) {
				game1->mouse1.left = KUP;
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
		game1->mouse1.x += testx;
		if ((mouse_packet[0] & BIT(5)) != 0) {
			mouse_packet[2] ^= 0xFF;
			mouse_packet[2]++;
			testy = mouse_packet[2];

		} else {
			testy = -mouse_packet[2];

		}
		game1->mouse1.y += testy;

		if (check) {
			menu = check_mouse(&(*game1));
			switch (menu) {
			case 4:
				game1->gamest = QUIT;
				break;
			case 3:
				game1->num_players = 4;
				start_multigame(4, &(*game1));
				break;
			case 2:
				game1->num_players = 3;
				start_multigame(3, &(*game1));
				break;
			case 1:
				game1->num_players = 2;
				start_multigame(2, &(*game1));
				break;
			default:
				break;
			}

		}

		return 1;
	}
	return 0;

}

void mouse_st_handler(player_t *p, unsigned long mouse_packet[3], game_t *game1) {
	unsigned int check = 0;
	if ((mouse_packet[0] & BIT(6)) == 0 && (mouse_packet[0] & BIT(7)) == 0) {
		if ((mouse_packet[0] & BIT(0)) != 0) {
			if (game1->mouse1.left != KDOWN)
				check = 1;
			game1->mouse1.left = KDOWN;
		} else
			game1->mouse1.left = KUP;

		if ((mouse_packet[0] & BIT(1)) != 0) {
			if (game1->mouse1.right != KDOWN)
				check = 2;
			game1->mouse1.right = KDOWN;
		} else
			game1->mouse1.right = KUP;

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

void check_winner(game_t *game1) {

	switch (game1->num_players) {
	case 4:
		if (game1->player1.lost) {
			if (game1->player2.lost) {
				if (game1->player4.lost) {
					if (game1->player3.lost) {
						drawBitmap(game1->draw, 255, 360, 0);
					} else
						drawBitmap(game1->player3.win, 255, 360, 0);
				} else
					drawBitmap(game1->player4.win, 255, 360, 0);
			} else
				drawBitmap(game1->player2.win, 255, 360, 0);
		} else
			drawBitmap(game1->player1.win, 255, 360, 0);
		break;
	case 3:
		if (game1->player1.lost) {
			if (game1->player2.lost) {
				if (game1->player3.lost) {
					drawBitmap(game1->draw, 255, 360, 0);
				} else
					drawBitmap(game1->player3.win, 255, 360, 0);
			} else
				drawBitmap(game1->player2.win, 255, 360, 0);
		} else
			drawBitmap(game1->player1.win, 255, 360, 0);
		break;
	case 2:
		if (game1->player1.lost) {
			if (game1->player2.lost) {
				drawBitmap(game1->draw, 255, 360, 0);
			} else
				drawBitmap(game1->player2.win, 255, 360, 0);
		} else
			drawBitmap(game1->player1.win, 255, 360, 0);
		break;
	default:
		break;
	}
}

void timer_intrhandler(game_t *game1) {
	if (game1->gamest == PLAYING) {
		draw_handler(game1->num_players, &(*game1));
		if (game1->lost + 1 >= game1->num_players) {
			game1->gamest = FINISHED;
			check_winner(&(*game1));
		}
	} else if (game1->gamest == MENU) {
		if (game1->mouse1.paint) {
			drawBitmap(game1->menu, 0, 0, 0);
			drawBitmap(game1->mouse, game1->mouse1.x, game1->mouse1.y, 0);
			game1->mouse1.paint = 0;
		}
	}
}

void kbd_intrhandler(unsigned long datakbd, game_t *game1) {
	if (game1->gamest == PLAYING) {
		change_plst_handler(game1->num_players, datakbd, &(*game1));
	}
	state_handler(game1->num_players, datakbd, &(*game1));
}

void mouse_intrhandler(unsigned long mouse_packet[3], game_t *game1) {
	if (game1->gamest == MENU) {
		game1->mouse1.paint = mouse_mov_handler(mouse_packet, &(*game1));
	} else if (game1->gamest == PLAYING && game1->num_players == 4) {
		mouse_st_handler(&game1->player4, mouse_packet, &(*game1));

	}
}

int playgame(game_t *game1) {
	load_Bmps(&(*game1));
	game1->gamest = MENU;
	int r, ipc_status, menu;
	message msg;
	unsigned int win, twobyteslong = 0, packet = 0, counter = 0;
	unsigned long datakbd, datamouse, mouse_packet[3];

	init_mouse(&(*game1));

	while (game1->gamest != QUIT) {
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
				if (msg.NOTIFY_ARG & game1->irq_set_timer) {
					timer_intrhandler(&(*game1));
				}
				if (msg.NOTIFY_ARG & game1->irq_set_kbd) {
					datakbd |= kbd_handler();
					if (datakbd == TWO_BYTES) {
						datakbd = datakbd << 8;
						twobyteslong = 1;
					} else {
						twobyteslong = 0;
						kbd_intrhandler(datakbd, &(*game1));
					}

				}
				if (msg.NOTIFY_ARG & game1->irq_set_mouse) {
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
						mouse_intrhandler(mouse_packet, &(*game1));
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

int start_multigame(unsigned int num_players, game_t *game1) {
	game1->gamest = INIT;
	game1->lost = 0;
	draw_board(num_players, &(*game1));
	drawBitmap(game1->start, 255, 460, 0);
	if (init_players(num_players, &(*game1)) != OK) {
		printf("Error in num of players\n");
		return 1;
	}
	draw_borders();

	return 0;
}

