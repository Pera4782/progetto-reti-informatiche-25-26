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
    unsigned short portaUtente;
    int ultimaModifica;

    struct card* nextCard;
};
typedef struct card card_t;


struct lavagna {
    
    int id;
    card_t* colonne[NUMCOLONNE];
};
typedef struct lavagna lavagna_t;


extern int nextCardId;
extern lavagna_t lavagna;


int init_lavagna();
card_t* create_card(const char*, unsigned short, int);
void stampa_card(card_t);
void insert_card(card_t*);