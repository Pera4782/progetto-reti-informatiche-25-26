#ifndef _STRUTTURE_H_
#define _STRUTTURE_H_

#define NUMCOLONNE 3
#define TEXTLEN 100
#define LAVAGNAPORT 5678

//definizione dei numeri associati ai comandi da parte dell'utente
#define HELLO_CMD 0
#define CREATE_CARD_CMD 1
#define QUIT_CMD 2

//definizione dei numeri associati ai comandi da parte della lavagna
#define SEND_USER_LIST 0
#define CARD_AVAILABLE 1

#include<netinet/in.h>

enum colonna {TODO = 0, DOING = 1, DONE = 2};
typedef enum colonna colonna_t;

struct card_struct {

    int id;
    colonna_t colonna;
    char testoAttivita[TEXTLEN + 1];
    unsigned short portaUtente;
    time_t ultimaModifica;

    struct card_struct* nextCard;
};
typedef struct card_struct card_t;


struct utente_struct {
    unsigned short PORT;
    int u2l_sd;
    int l2u_sd;
    int doingCardId;
    struct utente_struct* nextUtente;
};
typedef struct utente_struct utente_t;

struct lavagna_struct {
    
    int id;
    card_t* colonne[NUMCOLONNE];
    utente_t* utenti;
    unsigned int numUtenti;
};
typedef struct lavagna_struct lavagna_t;


struct socket_struct {
    char IP[10];
    unsigned short porta;
    struct sockaddr_in addr;
    int socket;
};
typedef struct socket_struct socket_t;

#endif