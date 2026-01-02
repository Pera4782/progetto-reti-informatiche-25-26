#ifndef _STRUTTURE_H_
#define _STRUTTURE_H_

#define NUMCOLONNE 3
#define TEXTLEN 100
#define LAVAGNAPORT 5678

//definizione dei numeri associati ai comandi da parte dell'utente
#define HELLO_CMD 0
#define CREATE_CARD_CMD 1
#define QUIT_CMD 2
#define ACK_CARD_CMD 3
#define CARD_DONE_CMD 4

//definizione dei numeri associati ai comandi da parte della lavagna
#define SEND_USER_LIST (1u << 0)
#define AVAILABLE_CARD (1u << 1)
#define PING_USER (1u << 2)
#define CARD_DONE_WAKE_UP (1u << 3)
#define WAITING_ON_PING (1u << 4)
#define USER_QUITTED (1u << 5)

#include<netinet/in.h>
#include<pthread.h>

enum colonna {TODO = 0, DOING = 1, DONE = 2};
typedef enum colonna colonna_t;

struct card_struct {

    uint32_t id;
    colonna_t colonna;
    char testoAttivita[TEXTLEN + 1];
    uint16_t portaUtente;
    time_t ultimaModifica;

    struct card_struct* nextCard;
};
typedef struct card_struct card_t;


struct utente_struct {
    uint16_t PORT;
    int u2l_sd;
    int l2u_sd;
    uint32_t doingCardId;
    struct utente_struct* nextUtente;

    pthread_mutex_t l2u_command_mutex;
    pthread_cond_t l2u_command_condition;

    /*
    la variabile ha la seguente struttura ordinata dal bit meno significativo
    
    | SEND_USER_LIST | AVAILABLE_CARD | PING_USER | CARD_DONE_WAKE_UP | WAITING_ON_PING | USER_QUITTED |
    */
    int has_pending_command;

    pthread_t command_sender;

};
typedef struct utente_struct utente_t;

struct lavagna_struct {
    
    int id;
    card_t* colonne[NUMCOLONNE];
    utente_t* utenti;
    uint32_t numUtenti;
    enum {NONE, WAITING_FOR_ACK, WORKING} state;
};
typedef struct lavagna_struct lavagna_t;


struct socket_struct {
    char IP[10];
    uint16_t porta;
    struct sockaddr_in addr;
    int socket;
};
typedef struct socket_struct socket_t;

#endif