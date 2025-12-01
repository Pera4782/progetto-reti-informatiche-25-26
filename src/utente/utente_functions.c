#include "../../include/utente/utente_functions.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <arpa/inet.h>

int hello(const int sd, const unsigned short PORT){

    char command = HELLO_CMD;

    if(send(sd, &command, 1, 0) < 1){  // invio al server dell' azione che si vuole compiere (0 = HELLO)
        perror("ERRORE NELL'INVIO DEL COMANDO");
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
    unsigned short net_port = htons(PORT);
    if(send(sd, &net_port, 2, 0) < 2){
        perror("ERRORE NELL'INVIO DELLA PORTA");
        return -1;
    }

    printf("porta %d inviata\n", PORT);  //TODO FIXARE IL CONTROLLO DELLE PORTE DISPONIBILI

    /*
    //una volta inviata la porta si aspetta che il server risponda con un byte per capire se era disponibile
    char disponibile;

    if(recv(sd, &disponibile, 1, MSG_WAITALL) < 1){
        perror("ERRORE NELLA RICEZIONE DEL DISPONIBILE");
        return -1;
    }

    if(!disponibile){
        printf("PORTA NON DISPONIBILE\n");
        return -1;
    }
    */

    return 0;
}

int create_card(const int sd, const int id, const char* testo, const colonna_t colonna){


    if(strlen(testo) > 100){
        printf("TESTO TROPPO LUNGO\n");
        return -1;
    }

    char command = CREATE_CARD_CMD;

    if(send(sd, &command, 1, 0) < 1){
        perror("ERRORE NELL'INVIO DEL COMANDO");
        return -1;
    }

    printf("comando inviato\n");

    char ACK;

    if(recv(sd, &ACK, 1, MSG_WAITALL) < 1){
        perror("ERRORE NELLA RECEIVE DELL'ACK");
        return -1;
    }

    printf("ricevuto ACK\n");

    /*
    una volta ricevuto l'ACK si inviano i dati nel seguente formato:
    | 4 byte ID | 101 byte testo + \0 | 1 byte colonna |
    */

    char buf[106];
    
    //serializzazione dell'id

    int net_id = htonl(id);
    memcpy(buf, &net_id, 4);

    //serializzazione del testo
    size_t len = strlen(testo);
    memcpy(buf + 4, testo, len);
    if (len < 100) {
        memset(buf + 4 + len, ' ', 100 - len);
    }
    buf[104] = '\0';


    //serializzazione della colonna
    buf[105] = (char)colonna;

    if(send(sd, buf, 106, 0) < 106){
        perror("ERRORE NELL'INVIO DEI DATI");
        return -1;
    }

    printf("dati inviati\n");


    //una volta inviati i dati si aspetta che il server risponda con 1 byte per capire se l'id era disponibile

    char disponibile;
    if(recv(sd, &disponibile, 1, MSG_WAITALL) < 1){
        perror("ERRORE NELLA RECEIVE DEL DISPONIBILE");
        return -1;
    }

    if(!disponibile) {
        printf("ID NON DISPONIBILE\n");
        return -1;
    }

    return 0;
}