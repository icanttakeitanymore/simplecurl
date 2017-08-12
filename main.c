#include "system_utils.h"
#include "net_utils.h"
#include <getopt.h>
#include <fcntl.h>
#include <unistd.h>
#define REQUEST_LEN 8000

int main(int argc, char **argv) {
	/* for logging and error messages */
	char *message = (char *)calloc(LINE_MAX, sizeof(char));
	/* for request */
	char *request = (char *)calloc(REQUEST_LEN, sizeof(char));
	/* for responce */
	int readed;
	char *buff = (char *)calloc(1024, sizeof(char));
	char *response = (char *)calloc(1024, sizeof(char));
	int response_size = 1024;
	/* for host */
	char *host = (char *)calloc(HOST_NAME_MAX, sizeof(char));
        /* for IP addr */
	char *ip = (char *)calloc(INET6_ADDRSTRLEN, sizeof(char*));	
	int retcode, opt, opt_index = 0;
	/* for strtol */
	char *endptr;

	/* network */
	int port = 0;
	int sockfd = 0;
        
	struct sockaddr_in *addr_info;
        struct sockaddr_in6 *addr_info6;

	struct addrinfo *resolve_info, *resolve_temp_info, hints;

	memset(&hints, 0, sizeof(struct addrinfo));
        hints.ai_family = AF_UNSPEC;    /* Allow IPv4 or IPv6 */
        hints.ai_socktype = SOCK_DGRAM; /* Datagram socket */
        hints.ai_flags = AI_PASSIVE;    /* For wildcard IP address */
        hints.ai_protocol = 0;          /* Any protocol */
        hints.ai_canonname = NULL;
        hints.ai_addr = NULL;
        hints.ai_next = NULL;


	struct option longopts[] = {
		{ "host", required_argument, 0, 'h' },
		{ "port", required_argument, 0, 'p' },
		{ "request", required_argument, 0, 'r'}
	};
	
	while ((opt = getopt_long(argc, argv, "h:p:r:", longopts, &opt_index)) != EOF) {
		switch (opt) {
			case 'h':
				strncpy(host,optarg, HOST_NAME_MAX);
				break;
			case 'p':
				port = atoi(optarg);
				break;
			case 'r':
				strncpy(request, optarg, REQUEST_LEN);
				strcat(request, "\r\n\r\n");
				break;
			default:
				sprintf(message, "\nUsage : %s \n\t-h --host <host>\n\t-p --port <port>\n\t-r --request <'request'>\n", argv[0]);
				SYSTEM(message, ERR);
		}	
	}
	if (strlen(host) && strlen(request) && port) {
		sprintf(message, "%s started, with params host -> %s, port -> %d, request -> %s", argv[0], host, port, request);
		SYSTEM(message, INFO);
	} else {
		sprintf(message, "\nUsage : %s \n\t-h --host <host>\n\t-p --port <port>\n\t-r --request <'request'>\n", argv[0]);
		SYSTEM(message, ERR);
	}
	
	retcode = getaddrinfo(host, NULL, &hints, &resolve_info);
        if (retcode != 0) {
                sprintf(message,"getaddrinfo -> %s", gai_strerror(retcode));
		SYSTEM(message, ERR);
        }

	foreach_addrinfo(resolve_info, resolve_temp_info) {
		switch(resolve_temp_info->ai_family) {
			case AF_INET:
					
				close(sockfd);

				memset(buff, 0, sizeof(char) * 1024);
				memset(response, 0, sizeof(char) * response_size);
				response_size = 1024;
				response = (char *)realloc(response, sizeof(char*) * 1024);
				
				memset(&addr_info, 0, sizeof(struct sockaddr_in));
				memset(ip, 0, sizeof(char *) * INET6_ADDRSTRLEN);
				
				addr_info = (struct sockaddr_in *) resolve_temp_info->ai_addr;
				inet_ntop(addr_info->sin_family, &addr_info->sin_addr, ip, INET6_ADDRSTRLEN);
				
				sprintf(message,"Trying to connect to %s IN A %s", host, ip);
				SYSTEM(message, INFO);
				
				sockfd = socket(AF_INET, SOCK_STREAM, 0);
				if (sockfd == -1) {
					sprintf(message, "socket() to addr %s -> %s", ip, strerror(errno));
					SYSTEM(message, INFO);
					continue;
				}
				struct sockaddr_in cur_addr;				
				cur_addr.sin_port =htons(port);
				cur_addr.sin_family = AF_INET;
				inet_pton(AF_INET,ip,&cur_addr.sin_addr);

				retcode = connect(sockfd,(struct sockaddr *) &cur_addr, sizeof(cur_addr));
				if (retcode == -1) {
					sprintf(message, "connect() to addr %s -> %s", ip, strerror(errno));
					SYSTEM(message, INFO);
					continue;
				}
				retcode = send(sockfd, request, sizeof(char) * strlen(request), 0);
				if (retcode == -1) {
					sprintf(message, "send() to addr %s -> %s", ip, strerror(errno));
					SYSTEM(message, INFO);
					continue;
				}
				while((retcode = read(sockfd, buff, 1024)) != 0) {
					readed += retcode;
					if (response_size < readed) {
						response_size += readed;
						response = (char *)realloc(response, sizeof(char *) *response_size);
					}
					strncat(response, buff, response_size);
				}
				retcode = printf("%s", response);
				sprintf(message,"End of connection to %s IN A %s", host, ip);
				SYSTEM(message, INFO);
				if (retcode) {
					sprintf(message,"Request is success %s IN A %s", host, ip);
					SYSTEM(message, INFO);
					exit(EXIT_SUCCESS);
				}
				break;
			case AF_INET6:
				addr_info6 = (struct sockaddr_in6 *) resolve_temp_info->ai_addr;
				inet_ntop(addr_info6->sin6_family, &addr_info6->sin6_addr, ip, INET6_ADDRSTRLEN);
				sprintf(message,"Trying to connect to %s IN A %s", host, ip);
				SYSTEM(message, INFO);

				sockfd = socket(AF_INET6, SOCK_STREAM, 0);
				if (sockfd == -1) {
					sprintf(message, "socket() to addr %s -> %s", ip, strerror(errno));
					SYSTEM(message, INFO);
					continue;
				}	
				
				memset(&addr_info6, 0, sizeof(struct sockaddr_in6));
				memset(ip, 0, sizeof(char *) * INET6_ADDRSTRLEN);
				break;
		}
	}



}
