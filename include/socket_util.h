#ifndef _SOCKET_UTIL_H_
#define _SOCKET_UTIL_H_

#include "strutture.h"
#include <string.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <sys/types.h>
#include <errno.h>
#include <stdio.h>
#include <fcntl.h>
#include <sys/select.h>
#include <time.h>

/**
 * @brief inizializza un socket
 * @param sock puntatore alla struttura da riempire
 * @param port porta con cui inizializzare il socket
 * @param block 1 se si vuole creare un socket bloccante 0 altrimenti
 * @return -1 in caso di fallimento
 */
int create_socket(socket_t*, const unsigned short, const int);


/**
 * @brief inizializza un socket TCP per attendere richieste in entrata
 * @param sock il socket da inizializzare
 * @param PORT porta alla quale si vuole inizializzare il socket 
 * @param block 1 se si vuole creare un socket bloccante 0 altrimenti
 * @return -1 in caso di errore 0 altrimenti
 */
int prepare_listener_socket(socket_t*,const unsigned short, const int block);



#endif