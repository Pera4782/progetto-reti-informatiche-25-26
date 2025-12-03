#include "../include/socket_util.h"



#define REQUESTQUEUE 256

int create_socket(socket_t* sock, const unsigned short PORT, const int block ){

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

    if(!block){
        int flags = fcntl(sock->socket, F_GETFL, NULL);
        fcntl(sock->socket, F_SETFL, flags | O_NONBLOCK);
    }

    return 0;
}


int prepare_listener_socket(socket_t* sock, const unsigned short PORT ,const int block){

    //creazione del socket del server
    if(create_socket(sock, PORT, block) < 0){
        printf("IMPOSSIBILE CREARE IL SOCKET\n");
        return -1;
    }

    if(bind(sock->socket, (struct sockaddr*) &sock->addr, sizeof(struct sockaddr))){
        printf("ERRORE NELLA BIND\n");
        return -1;
    }

    if(listen(sock->socket, REQUESTQUEUE) < 0){
        printf("ERRORE NELLA LISTEN\n");
        return -1;
    }

    return 0;
}

