
#include "../../include/lavagna/lavagna_functions.h"

#define RIGALENGTH 20
#define REQUESTQUEUE 256

lavagna_t lavagna;


void init_lavagna(){

    lavagna.id = 0;
    for(int i = 0; i < NUMCOLONNE; ++i) lavagna.colonne[i] = NULL;
    lavagna.utenti = NULL;
    lavagna.numUtenti = 0;
}



card_t* create_card(const char* testo, const int id, colonna_t colonna){

    if(strlen(testo) > TEXTLEN){
        printf("TESTO TROPPO LUNGO, MASSIMO 100 CARATTERI\n");
        return NULL;
    }

    card_t* card = (card_t*) malloc(sizeof(card_t));
    if(card == NULL){
        printf("ERRORE NELLA CREAZIONE DELLA CARD\n");
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

    card->testoAttivita[TEXTLEN] = '\0';
    for(int i = strlen(testo); i < TEXTLEN; ++i) card->testoAttivita[i] = ' '; // formattazione del testo per la stampa 

    return card;
}




void insert_card(card_t* card){

    if(card == NULL) return;

    card_t* temp = lavagna.colonne[card->colonna];
    lavagna.colonne[card->colonna] = card;
    lavagna.colonne[card->colonna]->nextCard = temp; //inserimento in testa nella lista corretta
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
    const int LEFTSPACES = (RIGALENGTH - cifre) / 2;    
    const int RIGHTSPACES = (cifre % 2 == 0)? LEFTSPACES : LEFTSPACES + 1; // calcolo degli spazi a sinistra e destra per "centrare" l'id della card
    
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

    for(int i = 0; i < NUMCOLONNE; ++i) destroy_colonna(lavagna.colonne[i]);
    
    utente_t* utenti = lavagna.utenti;
    while(utenti){
        utente_t* p = utenti->nextUtente;
        free(utenti);
        utenti = p;
    }

}


int prepare_server_socket(socket_t* sock){

    //creazione del socket del server
    if(create_socket(sock, LAVAGNAPORT) < 0){
        printf("IMPOSSIBILE CREARE IL SOCKET\n");
        return -1;
    }

    if(bind(sock->socket, (struct sockaddr*) &sock->addr, sizeof(struct sockaddr))){
        perror("ERRORE NELLA BIND");
        return -1;
    }

    if(listen(sock->socket, REQUESTQUEUE) < 0){
        perror("ERRORE NELLA LISTEN");
        return -1;
    }

    return 0;
}

char recv_command(int sd){
    
    char byte;
    if(recv(sd, &byte, 1, MSG_WAITALL) < 1){
        perror("ERRORE NELLA RICEZIONE DEL COMANDO");
        return 0xFF;
    }

    //invio dell'ACK al client
    char ACK = 0;
    if(send(sd, &ACK, 1, 0) < 1){
        perror("ERRORE NELL'INVIO DELL'ACK");
        return -1;
    }

    printf("ACK inviato\n");

    return byte;
}




/**
 * @brief funzione che inserisce nella lista degli utenti regiatrati un nuovo utente
 * @param PORT porta dell'utente
 */
static void insert_utente(const unsigned short PORT){

    utente_t* utente = (utente_t*) malloc(sizeof(utente_t));
    utente->PORT = PORT;

    utente_t* tmp = lavagna.utenti;
    lavagna.utenti = utente;
    utente->nextUtente = tmp;
}


int hello_answer(const int sd){


    //ricezione della porta da parte del client
    unsigned short net_port;
    if(recv(sd, &net_port, 2, MSG_WAITALL) < 2){
        perror("ERRORE NELLA RICEZIONE DELLA PORTA");
        return -1;
    }

    unsigned short PORT = ntohs(net_port);

    printf("ricevuta porta %d\n", PORT);
    insert_utente(PORT);
    printf("inserito utente\n");
    return 0;

}


int create_card_answer(const int sd){

    char buffer[106];
    
    if(recv(sd, buffer, 106, MSG_WAITALL) < 106){
        perror("ERRORE NELLA RICEZIONE DEI DATI DELLA CARD");
        return -1;
    }

    int net_id;
    memcpy(&net_id, buffer, 4);
    int id = ntohl(net_id);
    
    char testo[101];
    // Copia sicura: buffer + 4 è l'inizio del testo. Assicuriamo la terminazione.
    memcpy(testo, buffer + 4, 101);
    testo[100] = '\0'; 

    colonna_t colonna = (colonna_t)buffer[105];

    card_t* card = create_card(testo, id, colonna);
    insert_card(card);
    
    return 0;
}
