#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "strutture.h"
#include <time.h>
#include <unistd.h>

#define LAVAGNAPORT 5678

extern int nextCardId;
extern int nextPortaUtente;
extern lavagna_t lavagna;


void init_lavagna();
card_t* create_card(const char*);
void stampa_card(card_t);
void insert_card(card_t*);