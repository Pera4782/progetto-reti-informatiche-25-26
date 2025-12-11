
#include "../../include/lavagna/lavagna_functions.h"

#define RIGALENGTH 20

lavagna_t lavagna;
pthread_mutex_t mutex_lavagna;

void init_lavagna(){

    lavagna.id = 0;
    for(int i = 0; i < NUMCOLONNE; ++i) lavagna.colonne[i] = NULL;
    lavagna.utenti = NULL;
    lavagna.numUtenti = 0;

    pthread_mutex_init(&mutex_lavagna, NULL);
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



colonna_t find_card(const int id){

    for(int i = 0; i < NUMCOLONNE; ++i){
        card_t* lista = lavagna.colonne[i];
        while(lista){
            if(lista->id == id) return i;
            lista = lista->nextCard;
        }
    }
    return -1;
}


card_t* remove_card(const int id){

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

    pthread_mutex_lock(&mutex_lavagna);

    for(int i = 0; i < NUMCOLONNE; ++i) destroy_colonna(lavagna.colonne[i]);
    
    utente_t* utenti = lavagna.utenti;
    while(utenti){
        utente_t* p = utenti->nextUtente;
        free(utenti);
        utenti = p;
    }

    pthread_mutex_unlock(&mutex_lavagna);
}



void insert_utente(const unsigned short PORT, int u2l_sd, int l2u_sd){

    utente_t* utente = (utente_t*) malloc(sizeof(utente_t));
    utente->PORT = PORT;
    utente->u2l_sd = u2l_sd;
    utente->l2u_sd = l2u_sd;
    utente->doingCardId = -1;

    lavagna.numUtenti ++;

    utente_t* tmp = lavagna.utenti;
    lavagna.utenti = utente;
    utente->nextUtente = tmp;

}


int find_utente(const unsigned short PORT){

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
    if(lavagna.utenti->u2l_sd == u2l_sd){
        utente_t* current = lavagna.utenti;
        lavagna.utenti = lavagna.utenti->nextUtente;
        lavagna.numUtenti --;

        //se l'utente aveva una card in DOING sposto la card in TODO
        if(current->doingCardId != -1){
            card_t* card = remove_card(current->doingCardId);
            card->colonna = TODO;
            insert_card(card);
        }
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

            //se l'utente aveva una card in DOING sposto la card in TODO
            if(current->doingCardId != -1){
                card_t* card = remove_card(current->doingCardId);
                card->colonna = TODO;
                insert_card(card);
            }

            return current;
        }
        prev = current;
        current = current->nextUtente;
    }
    return NULL;
}

/**
 * @brief funzione per serializzare all'interno di un buffer le porte degli utenti
 * @param buf buffer all'interno del quale si vuole serializzare le porte
 */
static void serialize_ports(char* buf){

    utente_t* utenti = lavagna.utenti;
    while(utenti){

        short net_port = htons(utenti->PORT);
        memcpy(buf, &net_port, sizeof(short));
        buf += sizeof(short);

        utenti = utenti->nextUtente;
    }

}

int send_user_list(){

    pthread_mutex_lock(&mutex_lavagna);

    //preparazione del buffer contenente tutte le porte da mandare a tutti gli utenti
    const int LEN = lavagna.numUtenti * sizeof(short);
    char buf[LEN];

    serialize_ports(buf);

    utente_t* utenti = lavagna.utenti;
    while(utenti){
        //TODO FARE LA SEND_USER_LIST
    }

    pthread_mutex_unlock(&mutex_lavagna);

    return 0;
}


