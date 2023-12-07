/**
 * @author  Hasan Onuralp AGCA
 * @date    07.12.2023
 * 
 * @brief   Simple TCP Server that accepts incoming connections from a client,
 *          receives messages, and displays them
 *          Server Side
 *          socket -> bind -> listen -> accept -> send/recv -> shutdown -> close
 *
 */

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <errno.h>
#include <unistd.h>

#include <sys/socket.h>
#include <netinet/in.h>
#include <netinet/ip.h>
#include <arpa/inet.h>

void exit_sys(const char *msg);

#define PORT_NO 55555
#define SERVER_NAME "127.0.0.1"
#define BUF_SIZE 4096

int main(){

    int server_sock;
    int client_sock;
    struct sockaddr_in sinserver;
    struct sockaddr_in sinclient;
    ssize_t res;
    socklen_t sin_len;

    char buf[BUF_SIZE];

    //Server info
    sinserver.sin_family = AF_INET;
    sinserver.sin_port = htons(PORT_NO);
    sinserver.sin_addr.s_addr = inet_addr(SERVER_NAME);

    if((server_sock = socket(AF_INET, SOCK_STREAM, 0)) == -1){
        exit_sys("socket");
    }

    //"struct sockaddr_in sinserver" will include the informations of the server. What will be the port number and IP Address of the server.
    if(bind(server_sock, (struct sockaddr *)&sinserver, sizeof(sinserver)) == -1){
        exit_sys("bind");
    }

    if(listen(server_sock, 8) == -1)
        exit_sys("listen");

    printf("waiting for connection\n\n");
    sin_len = sizeof(sinclient);
    
    //sinclient struct will be filled with connected client's informations
    if((client_sock = accept(server_sock, (struct sockaddr *)&sinclient, &sin_len)) == -1){
        exit_sys("accept");
    }

    printf("Connected Client's\nIP:%s\nPort: %d\n\n", inet_ntoa(sinclient.sin_addr), sinclient.sin_port);

    for(;;){
        if((res = recv(client_sock, buf, BUF_SIZE, 0)) == -1)
            exit_sys("recv");

        //if client side closed the socket res will be 0 (like on pipes)
        if (res == 0)
            break;

        buf[res] = '\0';
        
        if(!strcmp(buf, "quit"))
            break;

        printf("%s\n", buf);
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
