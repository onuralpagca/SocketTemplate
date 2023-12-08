/**
 * @author  Hasan Onuralp AGCA
 * @date    08.12.2023
 * 
 * @brief   TCP Client
 *          ./client [-s <server_address>] [-p <server_port>] [-b <source_port>]
 *
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h>

#define DEF_SERVER_NAME		"127.0.0.1"
#define DEF_SERVER_PORT		"55555"
#define BUFFER_SIZE			4096

void exit_sys(const char *msg);

int main(int argc, char *argv[])
{
	int client_sock;
	struct sockaddr_in sin_client;
	struct addrinfo hints = {0, AF_INET, SOCK_STREAM};
    struct addrinfo *res, *ri;
    int gai_result;
	char buf[BUFFER_SIZE];
	char *str;
	int option;
	int s_flag, p_flag, b_flag, err_flag;
	const char *server_name;
	int bind_port;
    const char *server_port;

	s_flag = p_flag = b_flag = err_flag = 0;

	opterr = 0;
	while ((option = getopt(argc, argv, "s:p:b:")) != -1) {
		switch (option) {
			case 's':
				s_flag = 1;
				server_name = optarg;
				break;
			case 'p':
				p_flag = 1;
				server_port = optarg;
				break;
			case 'b':
				b_flag = 1;
				bind_port = atoi(optarg);
				break;
			case '?':
				if (optopt == 's' || optopt == 'p' || optopt == 'b')
					fprintf(stderr, "-%c Option must have an argument!\n", optopt);
				else
					fprintf(stderr, "-%c Invalid option!\n", optopt);
				err_flag = 1;
		}
	}

	if (err_flag)
		exit(EXIT_FAILURE);

	if (optind - argc != 0) {
		fprintf(stderr, "Too many arguments!..\n");
		exit(EXIT_FAILURE);
	}

	if (!s_flag)
		server_name = DEF_SERVER_NAME;
	if (!p_flag)
		server_port = DEF_SERVER_PORT;

	if ((client_sock = socket(AF_INET, SOCK_STREAM, 0)) == -1)
		exit_sys("socket");

	if (b_flag) {
		sin_client.sin_family = AF_INET;
		sin_client.sin_port = htons(bind_port);
		sin_client.sin_addr.s_addr = htonl(INADDR_ANY);

		if (bind(client_sock, (struct sockaddr *)&sin_client, sizeof(sin_client)) == -1)
			exit_sys("bind");
	}

    if ((gai_result = getaddrinfo(server_name, server_port, &hints, &res)) != 0) {
        fprintf(stderr, "getaddrinfo: %s\n", gai_strerror(gai_result));
        exit(EXIT_FAILURE);
    }

    for (ri = res; ri != NULL; ri = ri->ai_next) 
        if (connect(client_sock, ri->ai_addr, ri->ai_addrlen) != -1)
		    break;
    
    if (ri == NULL) 
        exit_sys("connect");

    freeaddrinfo(res);

 	printf("Connected to Server...\n");

	for (;;) {
		printf("Message> ");
		fflush(stdout);

		if (fgets(buf, BUFFER_SIZE, stdin) == NULL)
			continue;
		if ((str = strchr(buf, '\n')) != NULL)
			*str = '\0';

		if (send(client_sock, buf, strlen(buf), 0) == -1)
			exit_sys("send");
		if (!strcmp(buf, "quit"))
			break;
	}

	shutdown(client_sock, SHUT_RDWR);
	
    close(client_sock);

	return 0;
}

void exit_sys(const char *msg)
{
	perror(msg);

	exit(EXIT_FAILURE);
}
