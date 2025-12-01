#include "../include/socket_util.h"
#include <string.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <sys/types.h>
#include <errno.h>
#include <stdio.h>

int create_socket(socket_t* sock, const unsigned short PORT){

    strcpy(sock->IP, "127.0.0.1");      //inizializzazione del socket
    sock->porta = PORT;
    sock->addr.sin_family = AF_INET;
    sock->addr.sin_port = htons(sock->porta);
    inet_pton(AF_INET, "127.0.0.1", &sock->addr.sin_addr);
    sock->addr.sin_addr.s_addr = INADDR_ANY;

    sock->socket = socket(AF_INET, SOCK_STREAM, 0);
    if(sock->socket < 0){
        printf("ERRORE NELLA CREAZIONE DEL SOCKET\n");
        return -1;
    }

    return 0;
}