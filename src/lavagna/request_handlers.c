#include "../../include/lavagna/request_handlers.h"


int hello_handler(const int sd){


    //ricezione della porta da parte del client
    unsigned short net_port;
    if(recv(sd, &net_port, 2, MSG_WAITALL) < 2){
        printf("ERRORE NELLA RICEZIONE DELLA PORTA\n");
        return -1;
    }

    unsigned short PORT = ntohs(net_port);

    printf("RICEVUTA PORTA\n");

    pthread_mutex_lock(&mutex_lavagna);

    //controllo se la porta è disponibile, se no invio disponibile = 0 al client
    if(find_utente(PORT)){
        
        printf("PORTA NON DISPONIBILE\n");
        char disponibile = 0;
        if(send(sd, &disponibile, 1, 0) < 1){
            printf("ERRORE NELL'INVIO DI DISPONIBILE\n");
        }
        pthread_mutex_unlock(&mutex_lavagna);
        return -1;
    }

    //se si invio disponibile = 1 al client
    char disponibile = 1;
    if(send(sd, &disponibile, 1, 0) < 1){
        printf("ERRORE NELL'INVIO DI DISPONIBILE\n");
        pthread_mutex_unlock(&mutex_lavagna);
        return -1;
    }
    

    //inserisco l'utente nella lista degli utenti registrati
    insert_utente(PORT, sd);
    lavagna.numUtenti ++;
    
    printf("UTENTE INSERITO\n");
    
    pthread_mutex_unlock(&mutex_lavagna);

    return 0;

}


int create_card_handler(const int sd){

    char dati[106];
    
    //ricezione dei dati della card
    if(recv(sd, dati, 106, MSG_WAITALL) < 106){
        printf("ERRORE NELLA RICEZIONE DEI DATI DELLA CARD\n");
        return -1;
    }

    printf("RICEVUTI DATI CARD\n");

    //deserializzazione dei dati della card
    int net_id;
    memcpy(&net_id, dati, 4);
    int id = ntohl(net_id);
    
    char testo[101];
    memcpy(testo, dati + 4, 101);
    testo[100] = '\0'; 

    colonna_t colonna = (colonna_t)dati[105];


    //controllo se l'ID della card è disponibile
    pthread_mutex_lock(&mutex_lavagna);
    int found = find_card(id);

    //se non lo è mando al client disponibile = 0
    if(found != -1){
        printf("ID NON DISPONIBILE\n");
        char disponibile = 0;
        if(send(sd, &disponibile, 1, 0) < 1){
            printf("ERRORE NELL'INVIO DEL DISPONIBILE\n");
        }
        pthread_mutex_unlock(&mutex_lavagna);
        return -1;
    }

    //se lo è mando al client disponibile = 1
    char disponibile = 1;
    if(send(sd, &disponibile, 1, 0) < 1){
        printf("ERRORE NELL'INVIO DEL DISPONIBILE\n");
        pthread_mutex_unlock(&mutex_lavagna);
        return -1;
    }

    //infine creo ed inserisco la card
    card_t* card = create_card(testo, id, colonna);
    insert_card(card);

    printf("CARD INSERITA\n");

    show_lavagna();
    pthread_mutex_unlock(&mutex_lavagna);

    return 0;
}


void quit_handler(const int sd){

    pthread_mutex_lock(&mutex_lavagna);

    utente_t* u = remove_utente(sd);
    printf("UTENTE CON PORTA: %d DISCONNESSO\n", (int) u->PORT);
    free(u);

    pthread_mutex_unlock(&mutex_lavagna);
}
