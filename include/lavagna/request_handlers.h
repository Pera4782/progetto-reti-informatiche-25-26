
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
 * @brief funzione chiamata dopo ACK_CARD che assegna la card all'utente
 * @param u2l_sd il socket u2l dell'utente a cui si vuole assegnare la card
 */
int ack_card_handler(const int);

/**
 * @brief funzione per gestire il comando di CARD_DONE
 * @param u2l_sd il socket u2l dell'utentente che ha mandato CARD_DONE
 */
void card_done_handler(const int);

#endif