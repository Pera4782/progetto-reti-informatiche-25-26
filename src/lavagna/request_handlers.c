#include "../../include/lavagna/request_handlers.h"


void* l2u_command_sender(void* arg){

    pthread_detach(pthread_self());

    int l2u_sd = *((int*) arg);
    free(arg);

    //cerco le info dell'utente corretto e me le salvo
    pthread_mutex_lock(&mutex_lavagna);
    
    utente_t* utente = lavagna.utenti;
    while(utente){
        if(utente->l2u_sd == l2u_sd) break;
        utente = utente->nextUtente;
    }

    pthread_mutex_unlock(&mutex_lavagna);

    while(1){
        //mi blocco in attesa di dover mandare un comando
        pthread_mutex_lock(&utente->l2u_command_mutex);
        while(!utente->has_pending_command) 
            pthread_cond_wait(&utente->l2u_command_condition, &utente->l2u_command_mutex);


        //controllo il comando da eseguire
        int command = -1;
        if(utente->has_pending_command & SEND_USER_LIST){
            command = SEND_USER_LIST;
            utente->has_pending_command &= ~SEND_USER_LIST;
        }else if(utente->has_pending_command & AVAILABLE_CARD){
            command = AVAILABLE_CARD;
            utente->has_pending_command &= ~AVAILABLE_CARD;
        }

        pthread_mutex_unlock(&utente->l2u_command_mutex);

        //appena mi sblocco controllo il comando che devo mandare e lo faccio
        if(command == SEND_USER_LIST) send_user_list(utente);
        else if(command == AVAILABLE_CARD) send_available_card(utente); 

    }
}

int hello_handler(const int u2l_sd){


    //ricezione della porta da parte del client
    unsigned short net_port;
    if(recv(u2l_sd, &net_port, 2, MSG_WAITALL) < 2){
        printf("[ERR] ERRORE NELLA RICEZIONE DELLA PORTA\n");
        return -1;
    }

    unsigned short PORT = ntohs(net_port);

    pthread_mutex_lock(&mutex_lavagna);

    //controllo se la porta è disponibile, se no invio disponibile = 0 al client
    if(find_utente(PORT)){
        
        printf("[ERR] PORTA NON DISPONIBILE\n");
        char disponibile = 0;
        if(send(u2l_sd, &disponibile, 1, 0) < 1){
            printf("[ERR] ERRORE NELL'INVIO DI DISPONIBILE\n");
        }
        pthread_mutex_unlock(&mutex_lavagna);
        return -1;
    }

    //se si invio disponibile = 1 al client
    char disponibile = 1;
    if(send(u2l_sd, &disponibile, 1, 0) < 1){
        printf("[ERR] ERRORE NELL'INVIO DI DISPONIBILE\n");
        pthread_mutex_unlock(&mutex_lavagna);
        return -1;
    }
    
   
    //aspetto che l'utente mi dica che mi posso connettere
    char can_connect;
    if(recv(u2l_sd, &can_connect, 1, MSG_WAITALL) < 1){
        printf("[ERR] ERRORE NELLA RICEZIONE DEL CAN CONNECT\n");
        pthread_mutex_unlock(&mutex_lavagna);
        return -1;
    }

    
    //creazione del socket e connessione con l'utente
    socket_t l2u_sock;
    if(create_socket(&l2u_sock, PORT, 1) < 0){
        printf("[ERR] ERRORE NELLA CREAZIONE DEL SOCKET L2U ALL'UTENTE\n");
        pthread_mutex_unlock(&mutex_lavagna);
        return -1;
    }

    if(socket_connect(&l2u_sock) < 0){
        printf("[ERR] ERRORE NELLA CONNESSIONE ALL'UTENTE\n");
        pthread_mutex_unlock(&mutex_lavagna);
        return -1;
    }

    //inserisco l'utente nella lista degli utenti registrati
    insert_utente(PORT, u2l_sd, l2u_sock.socket);
    
    printf("[INFO] UTENTE CON PORTA: %d REGISTRATO\n", (int) PORT);
    

    //creazione del thread che si occuperà di mandare SEND_USER_LIST e AVAILABLE_CARD all'utente
    //appena registrato
    int* thread_arg = (int*) malloc(sizeof(int));
    *thread_arg = l2u_sock.socket;
    pthread_t l2u_command_sending_t;
    pthread_create(&l2u_command_sending_t, NULL, l2u_command_sender, thread_arg);

    //mando il comando si SEND_USER_LIST a tutti i thread 
    wakeup_command_senders(SEND_USER_LIST);

    //se gli utenti sono 2 o più, c'è una card in TODO e nessun utente sta lavorando su una card si manda la card disponibile
    if(lavagna.numUtenti >= 2 && lavagna.colonne[TODO] != NULL && !lavagna.working)
        wakeup_command_senders(AVAILABLE_CARD);
    
    pthread_mutex_unlock(&mutex_lavagna);

    return 0;
}


int create_card_handler(const int sd){

    char dati[106];
    
    //ricezione dei dati della card
    if(recv(sd, dati, 106, MSG_WAITALL) < 106){
        printf("[ERR] ERRORE NELLA RICEZIONE DEI DATI DELLA CARD\n");
        return -1;
    }

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
        printf("[ERR] ID NON DISPONIBILE\n");
        char disponibile = 0;
        if(send(sd, &disponibile, 1, 0) < 1){
            printf("[ERR] ERRORE NELL'INVIO DEL DISPONIBILE\n");
        }
        pthread_mutex_unlock(&mutex_lavagna);
        return 0;
    }

    //se lo è mando al client disponibile = 1
    char disponibile = 1;
    if(send(sd, &disponibile, 1, 0) < 1){
        printf("[ERR] ERRORE NELL'INVIO DEL DISPONIBILE\n");
        pthread_mutex_unlock(&mutex_lavagna);
        return -1;
    }

    //infine creo ed inserisco la card
    card_t* card = create_card(testo, id, colonna);
    insert_card(card);

    printf("[INFO] CARD %d INSERITA\n", id);

    show_lavagna();

    //se gli utenti sono 2 o più, c'è una card in TODO e nessun utente sta lavorando su una card si manda la card disponibile
    if(lavagna.numUtenti >= 2 && lavagna.colonne[TODO] != NULL && !lavagna.working)
        wakeup_command_senders(AVAILABLE_CARD);

    pthread_mutex_unlock(&mutex_lavagna);

    return 0;
}



int ack_card_handler(const int u2l_sd){

    //ricevo l'id della card ackata
    int net_id;
    if(recv(u2l_sd, &net_id, sizeof(int), MSG_WAITALL) < 0){
        printf("[ERR] ERRORE NELLA RICEZIONE DELL'ACKED CARD\n");
        return -1;
    }

    int id = ntohl(net_id);

    //controllo se la card è gia stata ACKATA
    pthread_mutex_lock(&mutex_lavagna);
    colonna_t colonna = find_card(id);
    

    if(colonna != TODO){
        char already_acked = 1;
        if(send(u2l_sd, &already_acked, 1, 0) < 1){
            printf("[ERR] ERRORE NELLA RISPOSTA A ACK CARD\n");
            pthread_mutex_unlock(&mutex_lavagna);
            return -1;
        }
        printf("[WRN] CARD GIA ACKATA\n");
        pthread_mutex_unlock(&mutex_lavagna);
        return 0;
    }

    char already_acked = 0;
    if(send(u2l_sd, &already_acked, 1, 0) < 1){
        printf("[ERR] ERRORE NELLA RISPOSTA A ACK CARD\n");
        pthread_mutex_unlock(&mutex_lavagna);
        return -1;
    }

    
    // ricerca dell'utente con il descrittore passato per argomento
    utente_t* utente = lavagna.utenti;
    while(utente) {
        if(utente->u2l_sd == u2l_sd) break;
        utente = utente->nextUtente;
    }

    //muovo la card nella colonna DOING e la assegno all'utente
    card_t* work = remove_card(id);

    utente->doingCardId = work->id;
    time(&work->ultimaModifica);

    work->colonna = DOING;
    insert_card(work);

    printf("[INFO] CARD %d ASSEGNATA ALL'UTENTE CON PORTA: %d\n", work->id, (int)utente->PORT);

    show_lavagna();

    pthread_mutex_unlock(&mutex_lavagna);
    return 0;
}



void card_done_handler(const int u2l_sd){


    pthread_mutex_lock(&mutex_lavagna);
    // ricerca dell'utente con il descrittore passato per argomento
    utente_t* utente = lavagna.utenti;
    while(utente) {
        if(utente->u2l_sd == u2l_sd) break;
        utente = utente->nextUtente;
    }

    utente->doingCardId = -1;

    //rimozione della card da DOING e inserimento nella colonna DONE
    card_t* completed_card = remove_card(lavagna.colonne[DOING]->id);
    completed_card->colonna = DONE;
    insert_card(completed_card);

    time(&completed_card->ultimaModifica);

    lavagna.working = 0;

    printf("[INFO] CARD %d COMPLETATA DALL'UTENTE CON PORTA: %d\n", completed_card->id, (int) utente->PORT);
    show_lavagna();

    if(lavagna.numUtenti >= 2 && lavagna.colonne[TODO] != NULL)
        wakeup_command_senders(AVAILABLE_CARD);
    
    pthread_mutex_unlock(&mutex_lavagna);
}
