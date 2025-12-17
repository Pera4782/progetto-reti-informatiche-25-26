#ifndef _LAVAGNA_FUNCTIONS_H_
#define _LAVAGNA_FUNCTIONS_H_

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <unistd.h>
#include <pthread.h>
#include <signal.h>
#include "../strutture.h"
#include "../../include/socket_util.h"

#define CARDROW 5

extern lavagna_t lavagna;
extern pthread_mutex_t mutex_lavagna;

/**
 * @brief inizializzazione della lavagna e dei semafori
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
 * @brief funzione che restiruisce la colonna dove si trova la card con un determinato id
 * @param id id della card di cui si vuole sapere la colonna
 * @return la colonna o -1 se la card non c'è
 */
colonna_t find_card(const int id);

/**
 * @brief funzione per rimuovere una card
 * @param id id della card che si vuole rimuovere
 * @return puntatore alla card rimossa dalla lista NULL se non c'era
 */
card_t* remove_card(const int id);

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
 * @brief funzione che inserisce nella lista degli utenti regiatrati un nuovo utente
 * @param PORT porta dell'utente
 * @param u2l_sd descrittore di socket per la comunicazione utente -> lavagna
 * @param l2u_sd descrittore di socket per la comunicazione lavagna -> utente
 */
void insert_utente(const unsigned short PORT, int u2l_sd, int l2u_sd);

/**
 * @brief funzione che cerca un utente con una certa porta negli utenti registrati
 * @param PORT la porta che deve avere l'utente che stiamo cercando
 * @return 0 se non è stato trovato 1 se è stato trovato
 */
int find_utente(const unsigned short PORT);

/**
 * @brief funzione utilizzata per rimuovere un utente dalla lista degli utenti registrati
 * @param u2l_sd descrittore di socket utente -> lavagna dell'utente da rimuovere
 * @return puntatore all'utente rimosso NULL altrimenti
 */
utente_t* remove_utente(int u2l_sd);

/**
 * @brief distruzione di un utente con chiusura dei socket
 * @param u puntatore all'utente da distrugere 
 */
void destroy_utente(utente_t*);

/**
 * @brief funzione per mandare la lista delle porte degli utenti a tutti gli utenti
 */
void send_user_list();

/**
 * @brief funzione per mandare la card disponibile
 */
void send_available_card();

#endif