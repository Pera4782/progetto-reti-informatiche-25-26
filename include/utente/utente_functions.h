
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
#include <time.h>


extern socket_t u2l_socket;
extern pthread_mutex_t u2l_socket_mutex;

extern socket_t listener_socket;
extern socket_t l2u_socket;

extern uint16_t* porte_utenti;
extern uint32_t num_utenti;
extern uint16_t my_port;

extern card_t working_card;

/**
 * @brief funzione per fare la HELLO (registrazione) da parte del client
 * @param sd descrittore del socket locale
 * @param PORT porta dell'utente da comunicare alla lavagna
 * @return -1 in caso di errore 0 altrimenti
 */
int hello(int, uint16_t);

/**
 * @brief funzione per fare richiesta di creare una card
 * @param sd descrittore del socket locale
 * @param id id da dare alla card
 * @param testo testo descrittivo della card
 * @param colonna colonna in cui verrà inserita la card
 * @return -1 in caso di errore 0 altrimenti
 */
int create_card(int, uint32_t,  char*,  colonna_t);


/**
 * @brief funzione per richiedere la disconnessione al server
 * @param sd descrittore del socket locale
 * @return -1 in caso di errore 0 altrimenti 
 */
int quit( int);

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
 * @brief manda il pong alla lavagna
 * @return -1 in caso di errore 0 altrimenti
 */
int send_pong();

/**
 * @brief funzione per scambiare il messaggio CHOOSE_USER
 * @return -1 in caso di errore 0 altrimenti
 */
int choose_user();


#endif