#include "i8042.h"
#include "i8254.h"
#include "test3.h"
#include <limits.h>
#include <string.h>
#include <errno.h>

static int proc_args(int argc, char **argv);
static unsigned long parse_ulong(char *str, int base);
static void print_usage(char **argv);

int main(int argc, char **argv)
{

	sef_startup();

	/* Enable IO-sensitive operations for ourselves */
	    sys_enable_iop(SELF);

	if (argc == 1) {					/* Prints usage of the program if no arguments are passed */
		print_usage(argv);
		return 0;
	}
	else return proc_args(argc, argv);
}

static void print_usage(char **argv)
{
	printf("Usage: one of the following:\n"
			"\t service run %s -args \"scan <decimal no.- ass>\"\n"
			"\t service run %s -args \"leds <decimal no. - leds>\"\n"
			"\t service run %s -args \"timedscan <decimal no. - n>\"\n",
			argv[0], argv[0], argv[0]);
}

static int proc_args(int argc, char **argv)
{
	unsigned long ass, n, leds[];

	if (strncmp(argv[1], "scan", strlen("scan")) == 0) {
		if (argc != 3) {
			printf("keyboard: wrong no. of arguments for kbd_test_scan()\n");
			return 1;
		}
		ass = parse_ulong(argv[2], 10);						/* Parses string to unsigned long */
		if (ass == ULONG_MAX)
			return 1;
		printf("test3::kbd_test_scan(%lu)\n", ass);
		return kbd_test_scan(ass);
	}
	else if (strncmp(argv[1], "leds", strlen("leds")) == 0) {
		if (argc < 3) {
			printf("keyboard: wrong no. of arguments for kbd_test_leds()\n");
			return 1;
		}
		//leds = parse_ulong(argv[2], 10);						/* Parses string to unsigned long */
		//if (leds == ULONG_MAX)
		//	return 1;
		for (unisgned int i = 2; i < argv.size() < i++)
			leds[i-2] = argv[i];

		printf("test3::kbd_test_leds()\n");
		return kbd_test_leds(argc-2, leds);
	}
	else if (strncmp(argv[1], "timedscan", strlen("timedscan")) == 0) {
		if (argc != 3) {
			printf("keyboard: wrong no of arguments for kbd_test_timed_scan()\n");
			return 1;
		}
		n = parse_ulong(argv[2], 10);						/* Parses string to unsigned long */
		if (n == ULONG_MAX)
			return 1;
		printf("test3::kbd_test_timed_scan(%lu)\n", n);
		return kbd_test_timed_scan(n);
	}
	else {
		printf("timer: %s - no valid function!\n", argv[1]);
		return 1;
	}
}

static unsigned long parse_ulong(char *str, int base)
{
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
		printf("keyboard: parse_ulong: no digits were found in %s\n", str);
		return ULONG_MAX;
	}

	/* Successful conversion */
	return val;
}
