
#ifndef _LAVAGNA_FUNCTIONS_H_
#define _LAVAGNA_FUNCTIONS_H_

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <unistd.h>
#include "strutture.h"

#define LAVAGNAPORT 5678
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
 * @return card_t* puntatore alla card appena creata
 */
card_t* create_card(const char*);

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


#endif