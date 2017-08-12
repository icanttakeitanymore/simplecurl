#include <stdio.h>
#include <errno.h>
#include <stdlib.h>
#include <time.h>
#include <limits.h>
#include <string.h>

#define TIME_FORMAT "[%Y/%m/%d %H:%M:%S]"
/* system actions */
#define ERR  0x1
#define INFO 0x2

/* logger and error excepting catcher */
#define SYSTEM(message, action)	         	                             \
	fprintf(stderr, "[%s] ", (action == ERR ? "ERR" : "INFO"));          \
	SYSTEM_timestamp(TIME_FORMAT);                                       \
	fprintf(stderr, " %s:%d %s() -> %s\n", __FILE__, __LINE__, __func__, message);    \
        if (action == ERR) { exit(EXIT_FAILURE); } else { memset(message, 0, strlen(message) * sizeof(char)); }

char *SYSTEM_timestamp(char *timeformat);
