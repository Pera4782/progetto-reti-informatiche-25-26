
#ifndef _UTENTE_FUNCTIONS_H_
#define _UTENTE_FUNCTIONS_H_

#include "../../include/strutture.h"

/*

OGNI primo messaggio che il client invia al server avrà il solo scopo di informare il server dell'azione che si vuole compiere,
esso sarà 1 byte da interpretare come intero, il client poi aspetterà un "ACK" (1 byte) da parte del server per procedere al passaggio dei dati, 
i valori assegnati al primo byte inviato hanno i seguenti significati:

    - 0 richiesta di HELLO da parte di un client


*/



/**
 * @brief funzione per fare la HELLO (registrazione) da parte del client
 * @param sd descrittore del socket locale
 * @param PORT porta dell'utente da comunicare alla lavagna
 * @return -1 in caso di errore 0 altrimenti
 */
int hello(const int, const unsigned short);

#endif