
#ifndef _UTENTE_FUNCTIONS_H_
#define _UTENTE_FUNCTIONS_H_

#include "../../include/strutture.h"
#include "../../include/socket_util.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <pthread.h>
#include <signal.h>

/*

OGNI primo messaggio che il client invia al server avrà il solo scopo di informare il server dell'azione che si vuole compiere,
esso sarà 1 byte da interpretare come intero, il client poi aspetterà un "ACK" (1 byte) da parte del server per procedere al passaggio dei dati, 
i valori assegnati al primo byte inviato hanno i seguenti significati:

    - 0 richiesta di HELLO da parte di un client (HELLO)
    - 1 richiesta di creazzione di una card (CREATE_CARD)
    - 2 richiesta di disconnessione (QUIT)

*/

extern socket_t listener_socket;
extern socket_t l2u_socket;

extern short* porte_utenti;
extern int num_utenti;

extern card_t working_card;

/**
 * @brief funzione per fare la HELLO (registrazione) da parte del client
 * @param sd descrittore del socket locale
 * @param PORT porta dell'utente da comunicare alla lavagna
 * @return -1 in caso di errore 0 altrimenti
 */
int hello(const int, const unsigned short);

/**
 * @brief funzione per fare richiesta di creare una card
 * @param sd descrittore del socket locale
 * @param id id da dare alla card
 * @param testo testo descrittivo della card
 * @param colonna colonna in cui verrà inserita la card
 * @return -1 in caso di errore 0 altrimenti
 */
int create_card(const int, const int, const char*, const colonna_t);


/**
 * @brief funzione per richiedere la disconnessione al server
 * @param sd descrittore del socket locale
 * @return -1 in caso di errore 0 altrimenti 
 */
int quit(const int);

/**
 * @brief funzione per ricevere la lista degli utenti dalla lavagna
 * @return -1 in caso di errore 0 altrimenti
 */
int recv_user_list();

/**
 * @brief riceve dalla lavagna la card disponibile
 * @return -1 in caso di errore 0 altrimenti
 */
int recv_available_card();

/**
 * @brief funzione per scambiare il messaggio CHOOSE_USER
 * @return -1 in caso di errore 0 altrimenti
 */
int choose_user();


#endif