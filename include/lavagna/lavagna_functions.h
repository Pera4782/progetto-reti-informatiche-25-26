#ifndef _LAVAGNA_FUNCTIONS_H_
#define _LAVAGNA_FUNCTIONS_H_

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <unistd.h>
#include "../strutture.h"
#include "../../include/socket_util.h"

#define CARDROW 5


extern int nextCardId;
extern int nextPortaUtente;
extern lavagna_t lavagna;

/**
 * @brief inizializzazione della lavagna
 */
void init_lavagna();

/**
 * @brief creazione di una nuova card allocata nello heap
 * @param testo stringa contenente la descrizione dell'attività
 * @param id id della card
 * @param colonna colonna in cui deve essere inserita la card
 * @return card_t* puntatore alla card appena creata o NULL in caso di errore
 */
card_t* create_card(const char*, const int, const colonna_t);

/**
 * @brief inserimento di una card all'interno della colonna corretta
 * @param card card da inserire
 */
void insert_card(card_t*);

/**
 * @brief stampa a video la lavagna
 */
void show_lavagna();

/**
 * @brief dealloca la lavagna
 */
void destroy_lavagna();

/**
 * @brief inizializza un socket TCP per attendere richieste in entrata
 * @param sock il socket da inizializzare
 * @return -1 in caso di errore 0 altrimenti
 */
int prepare_server_socket(socket_t*);


/**
 * @brief funzione per ottenere il comando che vuole eseguire l'utente e inviare l'ACK dopo averlo ricevuto 
 * @param sd descrittore del socket per la comunicazione con l'utente
 * @return il comando inviato dall'utente 0xFF in caso di errore
 */
char recv_command(int sd);


/**
 * @brief funzione per rispondere alla HELLO da parte di un client
 * @param sd descrittore del socket per la comunicazione con il client
 * @return -1 in caso di errore 0 altrimeenti
 */
int hello_answer(const int);

/**
 * @brief funzione per rispondere alla richiesta di creare una card
 * @param sd descrittore del socket per la comunicazione con il client
 * @return -1 in caso di errore 0 altrimenti
 */
int create_card_answer(const int);

#endif