#include "system_utils.h"

char *SYSTEM_timestamp(char *timeformat) {
        time_t ep_now;
        struct tm *local;
        time(&ep_now);
        local = localtime(&ep_now);
        char *buff = (char *) calloc(LINE_MAX, sizeof(char));
	strftime(buff, LINE_MAX, timeformat, local);
	fprintf(stderr,"%s ", buff);
        free(buff);
}

