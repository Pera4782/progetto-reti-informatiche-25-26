
#ifndef _STRUTTURE_H_
#define _STRUTTURE_H_

#define NUMCOLONNE 3
#define TEXTLEN 100

enum colonna {TODO = 0, DOING = 1, DONE = 2};
typedef enum colonna colonna_t;

struct card {

    int id;
    colonna_t colonna;
    char* testoAttivita;
    unsigned short portaUtente;
    time_t ultimaModifica;

    struct card* nextCard;
};
typedef struct card card_t;


struct lavagna {
    
    int id;
    card_t* colonne[NUMCOLONNE];
};
typedef struct lavagna lavagna_t;

#endif