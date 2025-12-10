#include "../../include/utente/utente_functions.h"



static int send_command(const char command, const int sd){

    //invio del comando
    if(send(sd, &command, 1, 0) < 1){  
        printf("ERRORE NELL'INVIO DEL COMANDO\n");
        return -1;
    }
    
    return 0;
}


int hello(const int sd, const unsigned short PORT){

    if(send_command(HELLO_CMD, sd) < 0) return -1;

    // una volta ricevuto l'ACK del server si puà procedere con l'invio della porta, 2 byte
    unsigned short net_port = htons(PORT);
    if(send(sd, &net_port, 2, 0) < 2){
        printf("ERRORE NELL'INVIO DELLA PORTA\n");
        return -1;
    }
    
    //una volta inviata la porta si aspetta che il server risponda con un byte per capire se era disponibile
    char disponibile;

    if(recv(sd, &disponibile, 1, MSG_WAITALL) < 1){
        printf("ERRORE NELLA RICEZIONE DEL DISPONIBILE\n");
        return -1;
    }

    if(!disponibile){
        printf("PORTA NON DISPONIBILE\n");
        return -1;
    }
    

    return 0;
}

int create_card(const int sd, const int id, const char* testo, const colonna_t colonna){


    if(strlen(testo) > 100){
        printf("TESTO TROPPO LUNGO\n");
        return -1;
    }

    if(send_command(CREATE_CARD_CMD, sd) < 0) return -1;

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
        printf("ERRORE NELL'INVIO DEI DATI\n");
        return -1;
    }

    //una volta inviati i dati si aspetta che il server risponda con 1 byte per capire se l'id era disponibile

    char disponibile;
    if(recv(sd, &disponibile, 1, MSG_WAITALL) < 1){
        printf("ERRORE NELLA RECEIVE DEL DISPONIBILE\n");
        return -1;
    }

    if(!disponibile) {
        printf("ID NON DISPONIBILE\n");
        return -1;
    }

    return 0;
}



int quit(const int sd){

    if(send_command(QUIT_CMD, sd) < 0) return -1;
    exit(0);
}



