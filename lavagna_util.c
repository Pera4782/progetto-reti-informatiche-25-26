
#include "lavagna_util.h"


int nextCardId = 0;
int nextPortaUtente = LAVAGNAPORT + 1;
lavagna_t lavagna;


void init_lavagna(){

    lavagna.id = 0;
    for(int i = 0; i < NUMCOLONNE; ++i) lavagna.colonne[i] = NULL;
}

card_t* create_card(const char* testo, unsigned short porta, int timestamp){

    card_t* card = (card_t*) malloc(sizeof(card_t));
    if(card == NULL){
        printf("ERRORE NELLA CREAZIONE DI UNA CARD\n");
        return NULL;
    }

    card->id = nextCardId;
    nextCardId++;

    card->nextCard = NULL;
    card->colonna = TODO;
    card->portaUtente = porta;
    card->ultimaModifica = timestamp;

    card->testoAttivita = (char*) malloc((strlen(testo) + 1) * sizeof(char));

    if(card->testoAttivita == NULL){
        printf("ERRORE NELLA CREAZIONE DI UNA CARD\n");
        return NULL;
    }

    strcpy(card->testoAttivita, testo);

    return card;
}

void stampa_card(card_t card){
    printf("\t%d\n\"%s\"\n\n", card.id, card.testoAttivita);
}

void insert_card(card_t* card){

    card_t* temp = lavagna.colonne[TODO];
    lavagna.colonne[TODO] = card;
    lavagna.colonne[TODO]->nextCard = temp;
}