
#include "lavagna_functions.h"

#define RIGALENGTH 20

int nextCardId = 0;
int nextPortaUtente = LAVAGNAPORT + 1;
lavagna_t lavagna;


void init_lavagna(){

    lavagna.id = 0;
    for(int i = 0; i < NUMCOLONNE; ++i) lavagna.colonne[i] = NULL;
}



card_t* create_card(const char* testo){

    if(strlen(testo) > TEXTLEN){
        printf("TESTO TROPPO LUNGO, MASSIMO 100 CARATTERI\n");
        return NULL;
    }

    card_t* card = (card_t*) malloc(sizeof(card_t));
    if(card == NULL){
        printf("ERRORE NELLA CREAZIONE DELLA CARD\n");
        return NULL;
    }


    card->id = nextCardId;  //inizializzazione dei parametri della card
    nextCardId++;

    card->nextCard = NULL;
    card->colonna = TODO;
    card->portaUtente = 0;

    time_t rawtime;
    time(&rawtime);
    card->ultimaModifica = rawtime;

    card->testoAttivita = (char*) malloc(TEXTLEN + 1);

    if(card->testoAttivita == NULL){
        printf("ERRORE NELLA CREAZIONE DELLA CARD\n");
        free(card);
        return NULL;
    }

    strcpy(card->testoAttivita, testo);

    card->testoAttivita[TEXTLEN] = '\0';
    for(int i = strlen(testo); i < TEXTLEN; ++i) card->testoAttivita[i] = ' '; // formattazione del testo per la stampa 

    return card;
}




void insert_card(card_t* card){

    card_t* temp = lavagna.colonne[card->colonna];
    lavagna.colonne[card->colonna] = card;
    lavagna.colonne[card->colonna]->nextCard = temp;
}

/**
 * @brief stampa la prima riga di una card contenente l'id
 * @param id id della card da stampare
 */
static void stampa_card_header(int id){ //TODO fare sta funzione a modo
    if(id == -1)  printf("                      ");
    else if(id < 10) printf("          %d           ", id);
    else if(id < 100) printf("          %d          ", id);
    else printf("         %d          ", id);
}

/**
 * @brief ottenere una riga di 20 caratteri da stampare di una card
 * @param card card di cui si vuole ottenere una riga
 * @param index indice da cui partire per ottenere i 20 caratteri
 * @return stringa contenente la riga del testo
 */
static char* get_riga_card(card_t* card, int index){

    char* line = (char*) malloc (21);

    if(card == NULL){
        strcpy(line, "                    \0");
        return line;
    }

    for(int i = 0; i < RIGALENGTH; ++i) line[i] = card->testoAttivita[i + index];
    line[RIGALENGTH] = '\0';
    return line;
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
            
            char* line1 = get_riga_card(cardTODO, RIGALENGTH * i);
            char* line2 = get_riga_card(cardDOING, RIGALENGTH * i);
            char* line3 = get_riga_card(cardDONE, RIGALENGTH * i);

            printf("| %s | %s | %s |\n", line1, line2, line3);

            free(line1);
            free(line2);
            free(line3);
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
        free(colonna->testoAttivita);
        free(colonna);
        colonna = p;
    }

}

void destroy_lavagna(){

    for(int i = 0; i < NUMCOLONNE; ++i) destroy_colonna(lavagna.colonne[i]);
    
}

