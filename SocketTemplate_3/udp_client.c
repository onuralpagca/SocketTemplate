/**
 * @author  Hasan Onuralp AGCA
 * @date    25.12.2023
 * 
 * @brief   UDP Client
 *          ./client [-s server] [-p port]
 * 			UDP Client Side
 * 			socket (SOCK_DGRAM) -> bind (optional) -> gethostbyname/getaddrinfo (optional) -> sendto/recvfrom -> close
 *
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include <unistd.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <arpa/inet.h>

#define DEF_SERVER_NAME		"localhost"
#define DEF_SERVER_PORT		"55555"
#define BUFFER_SIZE			4096

void exit_sys(const char *msg);

/* ./client [-s server] [-p port] */

int main(int argc, char *argv[])
{
	int client_sock;
	struct sockaddr_in sin_client, sin_server;
    socklen_t sin_len;
	struct addrinfo hints = {0, AF_INET, SOCK_STREAM};
    struct addrinfo *res;
    int gai_result;
	int option;
	int s_flag, p_flag, b_flag, err_flag;
	const char *server_name;
	int bind_port;
    const char *server_port;
    char buf[BUFFER_SIZE + 1];		/* BUFFER_SIZE is enough */
    char ntopbuf[INET_ADDRSTRLEN];
	ssize_t result;
    char *str;
    

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
					fprintf(stderr, "-%c option must have an argument!\n", optopt);
				else
					fprintf(stderr, "-%c invalid option!\n", optopt);
				err_flag = 1;
		}
	}

	if (err_flag)
		exit(EXIT_FAILURE);

	if (optind - argc != 0) {
		fprintf(stderr, "too many arguments!..\n");
		exit(EXIT_FAILURE);
	}

	if (!s_flag)
		server_name = DEF_SERVER_NAME;
	if (!p_flag)
		server_port = DEF_SERVER_PORT;

	if ((client_sock = socket(AF_INET, SOCK_DGRAM, 0)) == -1)
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
    
    freeaddrinfo(res);

    for (;;) {
		printf("Message> ");
		fflush(stdout);

		if (fgets(buf, BUFFER_SIZE, stdin) == NULL)
			continue;
		if ((str = strchr(buf, '\n')) != NULL)
			*str = '\0';

        if (!strcmp(buf, "quit"))
			break;

		if (sendto(client_sock, buf, strlen(buf), 0, res->ai_addr, sizeof(struct sockaddr_in)) == -1)
			exit_sys("send");
		
        sin_len = sizeof(sin_server);
        if ((result = recvfrom(client_sock, buf, BUFFER_SIZE, 0, (struct sockaddr *)&sin_server, &sin_len)) == -1)
            exit_sys("recvfrom");
        buf[result] = '\0';

        inet_ntop(AF_INET, &sin_server.sin_addr, ntopbuf, INET_ADDRSTRLEN);
        printf("%jd byte(s) received from server %s : %u: \"%s\"\n", (intmax_t)result, ntopbuf, (unsigned)ntohs(sin_server.sin_port), buf);    
	}

    close(client_sock);

    return 0;
}

void exit_sys(const char *msg)
{
	perror(msg);

	exit(EXIT_FAILURE);
}