#ifndef _SOCKET_UTIL_H_
#define _SOCKET_UTIL_H_

#include "strutture.h"

/**
 * @brief inizializza un socket
 * @param sock puntatore alla struttura da riempire
 * @param port porta con cui inizializzare il socket
 * @return -1 in caso di fallimento
 */
int create_socket(socket_t*, const unsigned short);

#endif