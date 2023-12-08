/**
 * @author  Hasan Onuralp AGCA
 * @date    08.12.2023
 * 
 * @brief   TCP Server
 *          ./server [-p port]
 *
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include <unistd.h>

#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>

#define DEF_SERVER_PORT		55555
#define BUFFER_SIZE			4096

void exit_sys(const char *msg);

int main(int argc, char *argv[])
{
	int server_sock, client_sock;
	struct sockaddr_in sin_server, sin_client;
	socklen_t sin_len;
	char buf[BUFFER_SIZE + 1];
    char ntopbuf[INET_ADDRSTRLEN];
	ssize_t result;
	int option;
	int server_port;
	int p_flag, err_flag;
	
	p_flag = err_flag = 0;

	opterr = 0;
	while ((option = getopt(argc, argv, "p:")) != -1) {
		switch (option) {
			case 'p':
				p_flag = 1;
				server_port = atoi(optarg);
				break;
			case '?':
				if (optopt == 'p')
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

	if (!p_flag)
		server_port = DEF_SERVER_PORT;

	if ((server_sock = socket(AF_INET, SOCK_STREAM, 0)) == -1)
		exit_sys("socket");

	sin_server.sin_family = AF_INET;
	sin_server.sin_port = htons(server_port);
	sin_server.sin_addr.s_addr = htonl(INADDR_ANY);

	if (bind(server_sock, (struct sockaddr *)&sin_server, sizeof(sin_server)) == -1)
		exit_sys("bind");

	if (listen(server_sock, 8) == -1)
		exit_sys("listen");

	printf("Listening Port: %d\n", server_port);

	printf("Waiting for Connection...\n");
	sin_len = sizeof(sin_client);
	if ((client_sock = accept(server_sock, (struct sockaddr *)&sin_client, &sin_len)) == -1)
		exit_sys("accept");

	printf("Connected Client ===> %s : %u\n", inet_ntop(AF_INET, &sin_client.sin_addr, ntopbuf, INET_ADDRSTRLEN), (unsigned)ntohs(sin_client.sin_port));    

	for (;;) {
		if ((result = recv(client_sock, buf, BUFFER_SIZE, 0)) == -1)
			exit_sys("recv");
		if (result == 0)
			break;
		buf[result] = '\0';
		if (!strcmp(buf, "quit"))
			break;
		printf("%jd byte(s) received: \"%s\"\n", (intmax_t)result, buf);
	}
	
	shutdown(client_sock, SHUT_RDWR);
	
    close(client_sock);
	close(server_sock);

	return 0;
}

void exit_sys(const char *msg)
{
	perror(msg);

	exit(EXIT_FAILURE);
}
