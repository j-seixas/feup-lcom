#include "lmlib.h"
#include "vbe.h"
#include "video_gr.h"
#include "test5.h"
#include "timer.h"
#include "i8254.h"
#include <limits.h>
#include <string.h>
#include <errno.h>

static int proc_args(int argc, char **argv);
static unsigned long parse_ulong(char *str, int base);
static void print_usage(char **argv);

int main(int argc, char *argv[]) {

	sef_startup();

	/* Enable IO-sensitive operations for ourselves */
	//sys_enable_iop(SELF);
	if (argc == 1) { /* Prints usage of the program if no arguments are passed */
		print_usage(argv);
		return 0;
	} else
		return proc_args(argc, argv);
}

static void print_usage(char **argv) {
	printf(
			"Usage: one of the following:\n"
					"\t service run %s -args \"tinit <unsigned no.- mode - delay>\"\n"
					"\t service run %s -args \"tsquare <unsigned no. - x - y - size - color>\"\n"
					"\t service run %s -args \"tline <unsigned no. - xi - yi - xf -yf - color>\"\n"
					"\t service run %s -args \"txpm <unsigned no. - xi - yi> <char - *xpm[]>\"\n"
					"\t service run %s -args \"tmove <unsigned no. - xi - yi> <char - *xpm[]> <no. - hor - delta - time>\"\n"
					"\t service run %s -args \"tcontroller \"\n", argv[0],
			argv[0], argv[0], argv[0], argv[0], argv[0]);
}

static int proc_args(int argc, char **argv) {
	unsigned short mode, delay, x, y, size, yf, xf, hor, time;
	unsigned long color;
	char *xpm;
	short delta;

	if (strncmp(argv[1], "tinit", strlen("tinit")) == 0) {
		if (argc != 4) {
			printf("graphics: wrong no. of arguments for test_init()\n");
			return 1;
		}
		mode = parse_ulong(argv[2], 10); /* Parses string to unsigned long */
		if (mode == ULONG_MAX)
			return 1;
		delay = parse_ulong(argv[3], 10); /* Parses string to unsigned long */
		if (delay == ULONG_MAX)
			return 1;
		printf("test5::test_init(%lu, %lu)\n", mode, delay);
		return test_init(mode, delay);

	} else if (strncmp(argv[1], "tsquare", strlen("tsquare")) == 0) {
		if (argc != 6) {
			printf("graphics: wrong no. of arguments for test_square()\n");
			return 1;
		}
		x = parse_ulong(argv[2], 10); /* Parses string to unsigned long */
		if (x == ULONG_MAX)
			return 1;
		y = parse_ulong(argv[3], 10); /* Parses string to unsigned long */
		if (y == ULONG_MAX)
			return 1;
		size = parse_ulong(argv[4], 10); /* Parses string to unsigned long */
		if (size == ULONG_MAX)
			return 1;
		color = parse_ulong(argv[5], 10); /* Parses string to unsigned long */
		if (color == ULONG_MAX)
			return 1;
		printf("test5::test_square(%lu, %lu, %lu, %lu)\n", x, y, size, color);
		return test_square(x, y, size, color);

	} else if (strncmp(argv[1], "tline", strlen("tline")) == 0) {
		if (argc != 7) {
			printf("graphics: wrong no of arguments for test_line()\n");
			return 1;
		}
		x = parse_ulong(argv[2], 10); /* Parses string to unsigned long */
		if (x == ULONG_MAX)
			return 1;
		y = parse_ulong(argv[3], 10); /* Parses string to unsigned long */
		if (y == ULONG_MAX)
			return 1;
		xf = parse_ulong(argv[4], 10); /* Parses string to unsigned long */
		if (xf == ULONG_MAX)
			return 1;
		yf = parse_ulong(argv[5], 10); /* Parses string to unsigned long */
		if (yf == ULONG_MAX)
			return 1;
		color = parse_ulong(argv[6], 10); /* Parses string to unsigned long */
		if (color == ULONG_MAX)
			return 1;
		printf("test5::test_line()\n");
		return test_line(x, y, xf, yf, color);

	} else if (strncmp(argv[1], "txpm", strlen("txpm")) == 0) {
		if (argc < 5) {
			printf("graphics: wrong no. of arguments for test_xpm()\n");
			return 1;
		}
		x = parse_ulong(argv[2], 10); /* Parses string to unsigned long */
		if (x == ULONG_MAX)
			return 1;
		y = parse_ulong(argv[3], 10); /* Parses string to unsigned long */
		if (y == ULONG_MAX)
			return 1;
		xpm = malloc(sizeof(char) * (argc - 2));
		unsigned int i = 0;
		/*while (i < argc - 2) {
		 xpm[i] = parse_ulong(argv[2 + i], 10);
		 if (xpm[i] == ULONG_MAX)
		 return 1;
		 i++;
		 }
		 printf("test5::test_xpm()\n");
		 return test_xpm(x, y, xpm);*/
		while (i < argc - 2) {
			xpm[i] = argv[2 + i];
			i++;
		}
		printf("test5::test_xpm()\n");
		return test_xpm(x, y, xpm);

	} else if (strncmp(argv[1], "tmove", strlen("tmove")) == 0) {
		if (argc < 5) {
			printf("graphics: wrong no. of arguments for test_tmove()\n");
			return 1;
		}
		x = parse_ulong(argv[2], 10); /* Parses string to unsigned long */
		if (x == ULONG_MAX)
			return 1;
		y = parse_ulong(argv[3], 10); /* Parses string to unsigned long */
		if (y == ULONG_MAX)
			return 1;
		xpm = malloc(sizeof(char) * (argc - 4));
		unsigned int i = 0;
		while (i < argc - 4) {
			xpm[i] = parse_ulong(argv[2 + i], 10);
			if (xpm[i] == ULONG_MAX)
				return 1;
			i++;
		}
		hor = parse_ulong(argv[2], 10); /* Parses string to unsigned long */
		if (x == ULONG_MAX)
			return 1;
		time = parse_ulong(argv[3], 10); /* Parses string to unsigned long */
		if (y == ULONG_MAX)
			return 1;
		printf("test5::test_move()\n");
		return test_move(x, y, xpm, hor, delta, time);

	} else if (strncmp(argv[1], "tcontroller", strlen("tcontroller")) == 0) {
		if (argc != 2) {
			printf("graphics: wrong no. of arguments for test_square()\n");
			return 1;
		}
		printf("test5::test_controller()\n");
		return test_controller();
	} else {
		printf("mouse: %s - no valid function!\n", argv[1]);
		return 1;
	}
}

static unsigned long parse_ulong(char *str, int base) {
	char *endptr;
	unsigned long val;

	/* Convert string to unsigned long */
	val = strtoul(str, &endptr, base);

	/* Check for conversion errors */
	if ((errno == ERANGE && val == ULONG_MAX) || (errno != 0 && val == 0)) {
		perror("strtoul");
		return ULONG_MAX;
	}

	if (endptr == str) {
		printf("mouse: parse_ulong: no digits were found in %s\n", str);
		return ULONG_MAX;
	}

	/* Successful conversion */
	return val;
}

