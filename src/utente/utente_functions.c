#include "../../include/utente/utente_functions.h"
#include <stdio.h>
#include <stdlib.h>

int hello(const int sd, const unsigned short PORT){

    char msg = 0;

    if(send(sd, &msg, 1, 0) < 1){  // invio al server dell' azione che si vuole compiere (0 = HELLO)
        perror("ERRORE NELL'INVIO DELL'AZIONE");
        return -1;
    }

    printf("comando inviato\n");

    char ACK;

    if(recv(sd, &ACK, 1, MSG_WAITALL) < 1){
        perror("ERRORE NELLA RECEIVE DELL'ACK");
        return -1;
    }

    printf("ricevuto ACK\n");

    // una volta ricevuto l'ACK del server si puà procedere con l'invio della porta, 2 byte
    if(send(sd, &PORT, 2, 0) < 2){
        perror("ERRORE NELL'INVIO DELLA PORTA");
        return -1;
    }

    printf("porta %d inviata\n", PORT);

    return 0;
}

