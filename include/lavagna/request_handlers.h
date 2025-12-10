
#ifndef _REQUEST_HANDLERS_H_
#define _REQUEST_HANDLERS_H_
#include "lavagna_functions.h"

/**
 * @brief funzione per rispondere alla HELLO da parte di un client
 * @param sd descrittore del socket per la comunicazione con l'utente
 * @return -1 in caso di errore 0 altrimeenti
 */
int hello_handler(const int);

/**
 * @brief funzione per rispondere alla richiesta di creare una card
 * @param sd descrittore del socket per la comunicazione con l'utente
 * @return -1 in caso di errore 0 altrimenti
 */
int create_card_handler(const int);

/**
 * @brief funzione per rispondere alla richiesta di disconnessione
 * @param sd descrittore del socket per la comunicazione con l'utente
 */
void quit_handler(const int);

#endif