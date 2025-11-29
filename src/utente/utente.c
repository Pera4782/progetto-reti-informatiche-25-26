#include "../../include/strutture.h"
#include "../../include/socket_util.h"
#include <stdio.h>
#include <stdlib.h>



int main() {

    socket_t client_sock;
    create_socket(&client_sock, LAVAGNAPORT);

    if(connect(client_sock.socket, (struct sockaddr*) &client_sock.addr, sizeof(struct sockaddr)) < 0){
        perror("ERRORE NELLA CONNECT");
        exit(1);
    }


    printf("connessione accettata\n");

}