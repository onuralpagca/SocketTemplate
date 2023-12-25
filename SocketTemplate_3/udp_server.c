/**
 * @author  Hasan Onuralp AGCA
 * @date    25.12.2023
 * 
 * @brief   UDP Server
 *          ./server [-p port]
 * 			UDP Server Side
 * 			socket (SOCK_DGRAM) -> bind -> recvfrom/sendto -> close
 *
 */

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <unistd.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <getopt.h>

#define DEF_SERVER_PORT		55555
#define BUFFER_SIZE			4096

char *revstr(char *str);
void exit_sys(const char *msg);

int main(int argc, char *argv[])
{
	int server_sock;
	struct sockaddr_in sin_server, sin_client;
	socklen_t sin_len;
	int server_port;
	char buf[BUFFER_SIZE + 1];
	char ntopbuf[INET_ADDRSTRLEN];
	ssize_t result;
	int option;
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

	if (!p_flag)
		server_port = DEF_SERVER_PORT;

	if ((server_sock = socket(AF_INET, SOCK_DGRAM, 0)) == -1)
		exit_sys("socket");

	sin_server.sin_family = AF_INET;
	sin_server.sin_port = htons(server_port);
	sin_server.sin_addr.s_addr = htonl(INADDR_ANY);

	if (bind(server_sock, (struct sockaddr *)&sin_server, sizeof(sin_server)) == -1)
			exit_sys("bind");
	
	printf("Waiting UDP packet...\n");
	for (;;) {
		sin_len = sizeof(sin_client);
		if ((result = recvfrom(server_sock, buf, BUFFER_SIZE, 0, (struct sockaddr *)&sin_client, &sin_len)) == -1)
			exit_sys("recvfrom");
		buf[result] = '\0';
		
		inet_ntop(AF_INET, &sin_client.sin_addr, ntopbuf, INET_ADDRSTRLEN);
		printf("%jd byte(s) received from %s : %u: \"%s\"\n", (intmax_t)result, ntopbuf, (unsigned)ntohs(sin_client.sin_port), buf);    

		revstr(buf);

		if (sendto(server_sock, buf, result, 0, (struct sockaddr *)&sin_client, sizeof(sin_client)) == -1)
			exit_sys("sendto");
	}
	
	close(server_sock);

	return 0;
}

char *revstr(char *str)
{
    size_t i, k;
    char temp;

    for (i = 0; str[i] != '\0'; ++i)
        ;
    
    for (--i, k = 0;  k < i; ++k, --i) {
        temp = str[k];
        str[k] = str[i];
        str[i] = temp;
    }

    return str;
}

void exit_sys(const char *msg)
{
	perror(msg);

	exit(EXIT_FAILURE);
}