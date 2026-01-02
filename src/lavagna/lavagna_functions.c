
#include "../../include/lavagna/lavagna_functions.h"

#define RIGALENGTH 20

lavagna_t lavagna;
pthread_mutex_t mutex_lavagna;

void init_lavagna(){

    lavagna.id = 0;
    for(int i = 0; i < NUMCOLONNE; ++i) lavagna.colonne[i] = NULL;
    lavagna.utenti = NULL;
    lavagna.numUtenti = 0;
    lavagna.state = NONE;

    pthread_mutex_init(&mutex_lavagna, NULL);
}


/**
 * @brief calcolo di quante cifre ha un numero
 * @param n numero di cui si vuole calcolare il numero di cifre
 * @return numero di cifre
 */
static int quante_cifre(int n){

    if(n == 0) return 1;
    
    int cifre = 0;
    while(n){
        cifre ++;
        n /= 10;
    }
    return cifre;
}


/**
 * @brief stampa la prima riga di una card contenente l'id
 * @param id id della card da stampare
 */
static void stampa_card_header(int id){
    if(id == -1)  {
        printf("                      ");
        return;
    }

    int cifre = quante_cifre(id);
     int LEFTSPACES = (RIGALENGTH - cifre) / 2;    
     int RIGHTSPACES = (cifre % 2 == 0)? LEFTSPACES : LEFTSPACES + 1; // calcolo degli spazi a sinistra e destra per "centrare" l'id della card
    
    printf(" ");
    for(int i = 0; i < LEFTSPACES; ++i) printf(" ");
    printf("%d", id);
    for(int i = 0; i < RIGHTSPACES; ++i) printf(" "); //stampa degli spazi calcolati prima
    printf(" ");
}

/**
 * @brief ottenere una riga di 20 caratteri da stampare di una card
 * @param card card di cui si vuole ottenere una riga
 * @param index indice da cui partire per ottenere i 20 caratteri
 * @param line la stringa che conterrà il testo
 */
static void get_riga_card(card_t* card, int index, char* line){

    if(card == NULL){
        strcpy(line, "                    \0");
        return;
    }

    for(int i = 0; i < RIGALENGTH; ++i) line[i] = card->testoAttivita[i + index]; //riempio line con i 20 caratteri del testo della card a partire da index
    line[RIGALENGTH] = '\0';
}



void show_lavagna(){

    printf(" --------------------------------------------------------------------\n");
    printf("|                            Lavagna - %d                             |\n", lavagna.id);
    printf(" --------------------------------------------------------------------\n");
    printf("|         To Do        |        Doing         |         Done         |\n");
    printf(" --------------------------------------------------------------------\n");
    
    
    card_t* cardTODO = lavagna.colonne[TODO];
    card_t* cardDOING = lavagna.colonne[DOING];
    card_t* cardDONE = lavagna.colonne[DONE];

    while(cardTODO || cardDOING || cardDONE){
        
        int id[3];
        id[TODO] = (cardTODO)? cardTODO->id : -1;  // calcolo degli id da passare alla funzione stampa_card_header
        id[DOING] = (cardDOING)? cardDOING->id : -1;
        id[DONE] = (cardDONE)? cardDONE->id : -1;

        for(int i = 0; i < NUMCOLONNE; ++i){ // stampa dell'intestazione contenente l'id per ogni riga di card da stampare
            printf("|");
            stampa_card_header(id[i]); 
        }
        printf("|\n");

        
        for(int i = 0; i < CARDROW ; ++i){ // stampa di una riga di testo delle card da stampare

            char line1[21], line2[21], line3[21];
            get_riga_card(cardTODO, RIGALENGTH * i, line1);
            get_riga_card(cardDOING, RIGALENGTH * i, line2);
            get_riga_card(cardDONE, RIGALENGTH * i, line3);

            printf("| %s | %s | %s |\n", line1, line2, line3);

        }

        printf(" --------------------------------------------------------------------\n");

        cardTODO = (cardTODO)? cardTODO->nextCard : cardTODO;   //avanzmento dei puntatori
        cardDOING = (cardDOING)? cardDOING->nextCard : cardDOING;
        cardDONE = (cardDONE)? cardDONE->nextCard : cardDONE;
    }
}

/**
 * @brief dealloca una colonna
 * @param colonna colonna da deallocare
 */
static void destroy_colonna(card_t* colonna){

    while(colonna){
        card_t* p = colonna->nextCard;
        free(colonna);
        colonna = p;
    }

}

void destroy_lavagna(){

    pthread_mutex_lock(&mutex_lavagna);

    for(int i = 0; i < NUMCOLONNE; ++i) destroy_colonna(lavagna.colonne[i]);
    
    utente_t* utenti = lavagna.utenti;
    while(utenti){
        utente_t* p = utenti->nextUtente;
        destroy_utente(utenti);
        utenti = p;
    }

    pthread_mutex_unlock(&mutex_lavagna);
}


card_t* create_card( char* testo, uint32_t id, colonna_t colonna){

    if(strlen(testo) > TEXTLEN){
        printf("[ERR] TESTO TROPPO LUNGO, MASSIMO 100 CARATTERI\n");
        return NULL;
    }

    card_t* card = (card_t*) malloc(sizeof(card_t));
    if(card == NULL){
        printf("[ERR] ERRORE NELLA CREAZIONE DELLA CARD\n");
        return NULL;
    }

    //inizializzazione dei parametri della card
    card->id = id;  

    card->nextCard = NULL;
    card->colonna = colonna;
    card->portaUtente = 0;

    time_t rawtime;
    time(&rawtime);
    card->ultimaModifica = rawtime;

    strcpy(card->testoAttivita, testo);

    //formattazione del testo per la stampa
    int len = strlen(card->testoAttivita);
    memset(card->testoAttivita + len, ' ', TEXTLEN - len);
    card->testoAttivita[TEXTLEN] = '\0';

    return card;
}




void insert_card(card_t* card){

    if(card == NULL) return;

    card_t* temp = lavagna.colonne[card->colonna];
    lavagna.colonne[card->colonna] = card;
    lavagna.colonne[card->colonna]->nextCard = temp; //inserimento in testa nella lista corretta
}



colonna_t find_card(uint32_t id){

    for(int i = 0; i < NUMCOLONNE; ++i){
        card_t* lista = lavagna.colonne[i];
        while(lista){
            if(lista->id == id) return i;
            lista = lista->nextCard;
        }
    }
    return -1;
}


card_t* remove_card(uint32_t id){

    for(int i = 0; i < NUMCOLONNE; ++i){

        if(!lavagna.colonne[i]) continue;

        card_t* prev = NULL;
        card_t* current = lavagna.colonne[i];
        card_t* next = lavagna.colonne[i]->nextCard;

        if(current->id == id){
            lavagna.colonne[i] = next;
            return current;
        }

        prev = current;
        current = current->nextCard;

        while(current){

            next = current->nextCard;
            if(current->id == id){
                prev->nextCard = next;
                return current;
            }
            prev = current;
            current = current->nextCard;
        }
    }
    return NULL;
}

/**
 * @brief funzione per la serializzazione di una card
 * @param card card da serializzare
 * @param buf buffer della dimensione corretta (105) in cui serializzare la card
 */
static void serialize_card(card_t card, char* buf){

    uint32_t net_card_id = htonl(card.id);
    memcpy(buf, &net_card_id, sizeof(uint32_t));
    memcpy(buf + sizeof(int), card.testoAttivita, TEXTLEN + 1);

}




void insert_utente(uint16_t PORT, int u2l_sd, int l2u_sd, pthread_t command_sender){

    utente_t* utente = (utente_t*) malloc(sizeof(utente_t));
    utente->PORT = PORT;
    utente->u2l_sd = u2l_sd;
    utente->l2u_sd = l2u_sd;
    utente->doingCardId = -1;

    lavagna.numUtenti ++;

    utente_t* tmp = lavagna.utenti;
    lavagna.utenti = utente;
    utente->nextUtente = tmp;

    pthread_mutex_init(&utente->l2u_command_mutex, NULL);
    pthread_cond_init(&utente->l2u_command_condition, NULL);
    utente->has_pending_command = 0;

    utente->command_sender = command_sender;
}


int find_utente(uint16_t PORT){

    utente_t* lista = lavagna.utenti;
    while(lista){
        if(lista->PORT == PORT) return 1;
        lista = lista->nextUtente;
    }

    return 0;
}

utente_t* remove_utente(int u2l_sd){

    //scorrimento della lista e rimozione dell'utente se c'è
    if(lavagna.utenti == NULL) return NULL;

    //controllo se quello da rimuovere è l'utente in testa
    if(lavagna.utenti->u2l_sd == u2l_sd){
        utente_t* current = lavagna.utenti;
        lavagna.utenti = lavagna.utenti->nextUtente;
        lavagna.numUtenti --;

        return current;
    }

    utente_t* prev = lavagna.utenti;
    utente_t* current = prev->nextUtente;
    utente_t* next;

    while(current){
        next = current->nextUtente;
        if(current->u2l_sd == u2l_sd){
            prev->nextUtente = next;
            lavagna.numUtenti --;

            return current;
        }
        prev = current;
        current = current->nextUtente;
    }
    return NULL;
}

void destroy_utente(utente_t* u){
    close(u->l2u_sd);
    close(u->u2l_sd);
    free(u);
}



/**
 * @brief funzione per serializzare all'interno di un buffer le porte degli utenti
 * @param buf buffer all'interno del quale si vuole serializzare le porte
 */
static void serialize_ports(char* buf){

    if(lavagna.utenti == NULL) return;

    utente_t* utenti = lavagna.utenti;
    while(utenti){

        uint16_t net_port = htons(utenti->PORT);
        memcpy(buf, &net_port, sizeof(uint16_t));
        buf += sizeof(uint16_t);

        utenti = utenti->nextUtente;
    }

}

void wakeup_command_senders(int command){

    utente_t* utente = lavagna.utenti;
    while(utente){

        pthread_mutex_lock(&utente->l2u_command_mutex);

        utente->has_pending_command |= command;

        pthread_cond_broadcast(&utente->l2u_command_condition);
        pthread_mutex_unlock(&utente->l2u_command_mutex);

        utente = utente->nextUtente;
    }

}

void send_user_list(utente_t* utente){

    pthread_mutex_lock(&mutex_lavagna);

    uint32_t LEN = lavagna.numUtenti * sizeof(uint16_t);
    char buf[LEN];
    serialize_ports(buf);

    pthread_mutex_unlock(&mutex_lavagna);
    
    int failed = 0;

    //per ogni invio si controlla se fallisce
    if(send_command(SEND_USER_LIST, utente->l2u_sd) < 0) failed = 1;
    
    uint32_t net_len = htonl(LEN);
    //invio della lunghezza
    if(!failed && send(utente->l2u_sd, &net_len, sizeof(int), 0) < 0) failed = 1;

    //invio del buffer contenente le porte
    if(!failed && send(utente->l2u_sd, buf, LEN, 0) < 0) failed = 1;


    if(failed)
        printf("[ERR] ERRORE NELL'INVIO DELLE PORTE ALL'UTENTE CON PORTA: %d\n", (int) utente->PORT);
    else
        printf("[INFO] PORTE INVIATE ALL'UTENTE CON PORTA: %d\n", (int) utente->PORT);

}


void send_available_card(utente_t* utente){

    //serializzazione della card
    pthread_mutex_lock(&mutex_lavagna);

    lavagna.state = WAITING_FOR_ACK;

    char buf[TEXTLEN + 1 + sizeof(int)];
    serialize_card(*(lavagna.colonne[TODO]), buf);

    pthread_mutex_unlock(&mutex_lavagna);

    int failed = 0;

    //invio del comando AVAILABLE_CARD
    if(send_command(AVAILABLE_CARD, utente->l2u_sd) < 0) failed = 1;

    //invio del buffer contenente la card
    if(!failed && send(utente->l2u_sd, buf, TEXTLEN + 1 + sizeof(int), 0) < TEXTLEN + 1 + sizeof(int)) failed = 1;

    if(failed)
        printf("[ERR] ERRORE NELL'INVIO DELLA AVAILABLE CARD ALL'UTENTE CON PORTA: %d\n", (int) utente->PORT);
    else
        printf("[INFO] AVAILABLE CARD INVIATA ALL'UTENTE CON PORTA: %d\n", (int) utente->PORT);
    
    pthread_mutex_lock(&acked_card_mutex);

    // inizializzazione per la timed wait
    struct timespec ts;
    time_t now = time(NULL);
    
    ts.tv_sec = now + 15;
    ts.tv_nsec = 0;

    int ret = 0;
    while(card_acked == -1 && !ret)
        ret = pthread_cond_timedwait(&acked_card_cond, &acked_card_mutex, &ts);

    if(ret == ETIMEDOUT){
        //il tempo è scaduto quindi la card non è stata ackata rifaccio SEND_USER_LIST E AVAILABLE_CARD
        pthread_mutex_lock(&utente->l2u_command_mutex);
        utente->has_pending_command |= (SEND_USER_LIST | AVAILABLE_CARD);
        pthread_mutex_unlock(&utente->l2u_command_mutex);
    }
    
    pthread_mutex_unlock(&acked_card_mutex);

}


void send_ping(utente_t* utente){

    // inizializzazione per la timed wait
    struct timespec ts;
    time_t now = time(NULL);
    
    ts.tv_sec = now + 90;
    ts.tv_nsec = 0;

    pthread_mutex_lock(&utente->l2u_command_mutex);
    
    //mi metto in attesa o che il request_handler mi svegli o che scatti il timeout
    utente->has_pending_command |= WAITING_ON_PING;

    int ret = 0;
    while(!(utente->has_pending_command & CARD_DONE_WAKE_UP || utente->has_pending_command & USER_QUITTED) && !ret) 
        ret = pthread_cond_timedwait(&utente->l2u_command_condition, &utente->l2u_command_mutex, &ts);

    utente->has_pending_command &= ~WAITING_ON_PING;

    if(ret == ETIMEDOUT){
        //il tempo è scaduto devo mandare il ping
        send_command(PING_USER, utente->l2u_sd);
        printf("[INFO] PING INVIATO ALL'UTENTE CON PORTA: %d\n", (int) utente->PORT);


        //inizializzazione delle strutture per la select
        struct timeval timeout;
        timeout.tv_sec = 30;
        timeout.tv_usec = 0;

        fd_set read;
        FD_ZERO(&read);
        FD_SET(utente->l2u_sd, &read);
        int fd_max = utente->l2u_sd;

        //aspetto 30 secondi in ascolto
        int select_ret = select(fd_max + 1, &read, NULL, NULL, &timeout);

        if(select_ret == 0){
            //timeout scaduto spengo il socket u2l in modo che il request_handler si svegli e rimuova l'utente
            shutdown(utente->u2l_sd, SHUT_RDWR);
            //termino il command sender
            pthread_exit(NULL);
        }else{
            //è arrivato il pong
            char pong;
            if(recv(utente->l2u_sd, &pong, 1, MSG_WAITALL) < 1){
                printf("[ERR] ERRORE NELLA RICEZIONE DEL PONG\n");
                shutdown(utente->u2l_sd, SHUT_RDWR);
                return;
            }
            printf("[INFO] PONG RICEVUTO\n");
        }
    }else {
        
        if(utente->has_pending_command & USER_QUITTED){
            pthread_mutex_unlock(&utente->l2u_command_mutex);
            pthread_exit(NULL);
        }

        //il request handler mi ha svegliato
        utente->has_pending_command &= ~ CARD_DONE_WAKE_UP;
    }

    pthread_mutex_unlock(&utente->l2u_command_mutex);
}
