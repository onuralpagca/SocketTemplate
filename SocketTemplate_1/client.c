/**
 * @author  Hasan Onuralp AGCA
 * @date    07.12.2023
 * 
 * @brief   Simple TCP Client that connects to a server, sends messages 
 *          and terminates upon user input "quit"
 *          Client Side Steps
 *          socket -> bind(optional) -> gethostbyname(DNS) -> connect -> send/recv -> shutdown -> close
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

#define SERVER_NAME         "127.0.0.1"
#define BUF_SIZE            4096

struct hostent *hent;

void exit_sys(const char *msg);

int main(){

    int client_sock;
    struct sockaddr_in sin_server;
    char buf[BUF_SIZE];
    char *str;
    ssize_t res;

    if((client_sock = socket(AF_INET, SOCK_STREAM, 0)) == -1)
        exit_sys("socket");

    //IP address and Port number of the server that is desired to connect
    sin_server.sin_family = AF_INET;
    sin_server.sin_port = htons(55555);

    //If you want to indicate a spesific port and IP Address for the client you should bind the client socket. Otherwise Kernel will assign an automatic Port number and IP Address to client process.

    /* bind (Optional)
	{
		struct sockaddr_in sin_client;

		sin_client.sin_family = AF_INET;
		sin_client.sin_port = htons(50000);
		sin_client.sin_addr.s_addr = htonl(INADDR_ANY);

		if (bind(client_sock, (struct sockaddr *)&sin_client, sizeof(sin_client)) == -1)
			exit_sys("bind");
	}
	*/

    /*If server name given in dotted format (like 192.168.1.10) than this address directly 
    converted to binary data in network byte order and assigned to to the s_addr. But if 
    server name given with hostname (like google.com) inet_addr function will return with -1 
    and gethostbyname will start DNS lookup and will fill struct hostent.*/

    if((sin_server.sin_addr.s_addr = inet_addr(SERVER_NAME)) == -1){
        if ((hent = gethostbyname(SERVER_NAME)) == NULL) {
            fprintf(stderr, "gethostbyname: %s\n", hstrerror(h_errno));
            exit(EXIT_FAILURE);
        }

        memcpy(&sin_server.sin_addr.s_addr, hent->h_addr_list[0], hent->h_length);
    }

    if(connect(client_sock, (struct sockaddr *)&sin_server, sizeof(sin_server)) == -1){
        exit_sys("connect");
    }
        printf("connected \n");

    for(;;){
        printf("Message > ");
        fflush(stdout);

        if(fgets(buf, BUF_SIZE, stdin) == NULL)
            continue;
        if((str = strchr(buf, '\n')) != NULL)
            *str = '\0';

        if((res = send(client_sock, buf, strlen(buf), 0)) == -1){
            exit_sys("send");
        }
    
        if(!strcmp(buf, "quit"))
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
