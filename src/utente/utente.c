#include "../../include/strutture.h"
#include "../../include/socket_util.h"
#include "../../include/utente/utente_functions.h"
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>


int main(int argc, char** argv) {

    socket_t client_sock;
    create_socket(&client_sock, LAVAGNAPORT);

    //connessione da parte dell'utente alla lavgna
    if(connect(client_sock.socket, (struct sockaddr*) &client_sock.addr, sizeof(struct sockaddr)) < 0){ 
        perror("ERRORE NELLA CONNECT");
        exit(1);
    }


    if(hello(client_sock.socket, 5679) < 0){
        printf("ERRORE NELLA HELLO\n");
        exit(1);
    }


    close(client_sock.socket);
    return 0;
}