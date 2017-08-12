#include <arpa/inet.h>
#include <stdio.h>
#include <netinet/in.h>
#include <netdb.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>

#define foreach_addrinfo(resolve_info, resolve_temp_info)   \
        for(resolve_temp_info = resolve_info;               \
            resolve_temp_info->ai_next != NULL;             \
            resolve_temp_info = resolve_temp_info->ai_next) 
