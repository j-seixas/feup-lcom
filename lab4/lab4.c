#include "i8042.h"
//#include "i8254.h"
#include "test4.h"
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
	printf("Usage: one of the following:\n"
			"\t service run %s -args \"packet <decimal no.- cnt>\"\n"
			"\t service run %s -args \"async <decimal no. - idle_time>\"\n"
			"\t service run %s -args \"config \"\n"
			"\t service run %s -args \"gesture <decimal no. - length>\"\n",
			argv[0], argv[0], argv[0], argv[0]);
}

static int proc_args(int argc, char **argv) {
	unsigned short cnt, idle_time;
	short length;

	if (strncmp(argv[1], "packet", strlen("packet")) == 0) {
		if (argc != 3) {
			printf("mouse: wrong no. of arguments for test_packet()\n");
			return 1;
		}
		cnt = parse_ulong(argv[2], 10); /* Parses string to unsigned long */
		if (cnt == ULONG_MAX)
			return 1;
		printf("mouse::test_packet(%lu)\n", cnt);
		return test_packet(cnt);
	} else if (strncmp(argv[1], "async", strlen("async")) == 0) {
		if (argc != 3) {
			printf("mouse: wrong no. of arguments for test_async()\n");
			return 1;
		}
		idle_time = parse_ulong(argv[2], 10); /* Parses string to unsigned long */
		if (idle_time == ULONG_MAX)
			return 1;
		printf("mouse::test_async()\n");
		return test_async(idle_time);
	} else if (strncmp(argv[1], "config", strlen("config")) == 0) {
		if (argc != 2) {
			printf("mouse: wrong no of arguments for test_config()\n");
			return 1;
		}
		printf("test3::test_config()\n");
		return test_config();
	} else if (strncmp(argv[1], "gesture", strlen("gesture")) == 0) {
			if (argc != 3) {
				printf("mouse: wrong no. of arguments for test_gesture()\n");
				return 1;
			}
			length = parse_ulong(argv[2], 10); /* Parses string to unsigned long */
			if (length == ULONG_MAX)
				return 1;
			printf("mouse::test_gesture(%lu)\n", length);
			return test_gesture(length);
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

