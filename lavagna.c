#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define LAVAGNAPORT 5678
#define NUMCOLONNE 3

enum colonna {TODO = 0, DOING = 1, DONE = 2};
typedef enum colonna colonna_t;

struct card {
    
    int id;
    colonna_t colonna;
    char* testoAttivita;
    int portaUtente;
    int ultimaModifica;

    struct card* nextCard;
};
typedef struct card card_t;


struct lavagna {
    
    int id;
    colonna_t *colonne;
    card_t* listaCard;
};
typedef struct lavagna lavagna_t;


lavagna_t lavagna;

void init_lavagna(){
    lavagna.id = 0;
    lavagna.colonne = (colonna_t*) malloc(NUMCOLONNE * sizeof(colonna_t));
    lavagna.colonne[0] = TODO;
    lavagna.colonne[1] = DOING;
    lavagna.colonne[2] = DONE;

    lavagna.listaCard = NULL;
}

void inserisci_card(card_t* card){

    card_t* c = lavagna.listaCard;
    lavagna.listaCard = card;
    card->nextCard = c;
}

void init_card(card_t *card, int id, colonna_t colonna, char* testo, int porta, int ultimaModifica){

    card->id = id;
    card->colonna = colonna;
    card->portaUtente = porta;
    card->ultimaModifica = ultimaModifica;

    card->testoAttivita = (char*) malloc((strlen(testo) + 1) * sizeof(char));
    strcpy(card->testoAttivita, testo);
    card->nextCard = NULL;
}


char* colonna_to_text(colonna_t colonna){
    switch(colonna){
        case TODO: return "To Do";
        case DOING: return "Doing";
        default: return "Done";
    }
}

void stampa_card(card_t card){

    card_t* p = lavagna.listaCard;

    while(p){
        printf("\t%d\n\"%s\" \n\n", 
            p->id, p->testoAttivita);
        p = p->nextCard;
    }
    
}

void destroy_lavagna(){

    card_t* p = lavagna.listaCard;
    while(p){

        card_t* next = p->nextCard;
        free(p);
        p = next;
    }

    lavagna.listaCard = NULL;
}

int main(){

    init_lavagna();

    card_t* card1 = (card_t*) malloc(sizeof(card_t));
    card_t* card2 = (card_t*) malloc(sizeof(card_t));
    init_card(card1, 0, TODO, "sono una card1", 0, 10);
    init_card(card2, 1, DOING, "sono una card2", 14, 30);

    inserisci_card(card1);
    inserisci_card(card2);
    stampa_card(*lavagna.listaCard);
    
    destroy_lavagna();
    return 0;
}