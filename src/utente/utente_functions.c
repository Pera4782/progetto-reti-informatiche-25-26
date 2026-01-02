#include "../../include/utente/utente_functions.h"




int hello( int sd, uint16_t PORT){

    if(send_command(HELLO_CMD, sd) < 0) return -1;

    // invio della porta
    uint16_t net_port = htons(PORT);
    if(send(sd, &net_port, 2, 0) < 2){
        printf("[ERR] ERRORE NELL'INVIO DELLA PORTA\n");
        return -1;
    }
    
    //una volta inviata la porta si aspetta che il server risponda con un byte per capire se era disponibile
    char disponibile;

    if(recv(sd, &disponibile, 1, MSG_WAITALL) < 1){
        printf("[ERR] ERRORE NELLA RICEZIONE DEL DISPONIBILE\n");
        return -1;
    }

    if(!disponibile){
        printf("[ERR] PORTA NON DISPONIBILE\n");
        return -1;
    }
    
    //preparo il socket dedicato a ricevere comandi
    prepare_listener_socket(&listener_socket, PORT, 1);

    //comunico alla lavagna che si può connettere
    char can_connect = 0;
    if(send(sd, &can_connect, 1, 0) < 1){
        printf("[ERR] ERRORE NELL'INVIO DEL CAN CONNECT\n");
        return -1;
    }

    //accetto la connessione dalla lavagna
    unsigned int len = sizeof(struct sockaddr);
    l2u_socket.socket = accept(listener_socket.socket, (struct sockaddr*) &l2u_socket.addr, &len);
    
    if(l2u_socket.socket < 0){
        printf("[ERR] ERRORE NELLA ACCEPT\n");
        return -1;
    }

    //setto il listener non bloccante per preparare allo scambio dei messaggi di CHOOSE_USER

    int flags = fcntl(listener_socket.socket, F_GETFL, 0);
    fcntl(listener_socket.socket, F_SETFL, flags | O_NONBLOCK);

    printf("[INFO] REGISTRAZIONE AVVENUTA CON SUCCESSO\n");
    return 0;
}

int create_card(int sd, uint32_t id,  char* testo,  colonna_t colonna){


    if(strlen(testo) > 100){
        printf("[ERR] TESTO TROPPO LUNGO\n");
        return -1;
    }

    if(send_command(CREATE_CARD_CMD, sd) < 0) return -1;

    //invio dei dati relativi alla card

    char buf[106];
    
    //serializzazione dell'id

    uint32_t net_id = htonl(id);
    memcpy(buf, &net_id, 4);

    //serializzazione del testo
    size_t len = strlen(testo);
    memcpy(buf + 4, testo, len);
    if (len < 100) {
        memset(buf + 4 + len, ' ', 100 - len);
    }
    buf[104] = '\0';


    //serializzazione della colonna
    buf[105] = (char)colonna;

    if(send(sd, buf, 106, 0) < 106){
        printf("[ERR] ERRORE NELL'INVIO DEI DATI\n");
        return -1;
    }

    //una volta inviati i dati si aspetta che il server risponda con 1 byte per capire se l'id era disponibile

    char disponibile;
    if(recv(sd, &disponibile, 1, MSG_WAITALL) < 1){
        printf("[ERR] ERRORE NELLA RECEIVE DEL DISPONIBILE\n");
        return -1;
    }

    if(!disponibile) {
        printf("[ERR] ID NON DISPONIBILE\n");
        return -1;
    }

    printf("[INFO] CARD CREATA CON SUCCESSO\n");

    return 0;
}


int quit( int sd){

    if(send_command(QUIT_CMD, sd) < 0) return -1;
    
    _exit(0);
}

int recv_user_list(){

    uint32_t net_len = 0;
    
    //ricevo la lunghezza del buffer (sizeof(short) * numUtenti)
    if(recv(l2u_socket.socket, &net_len, sizeof(int), MSG_WAITALL) < 0){
        printf("[ERR] ERRORE NELLA RICEZIONE DELLA LUNGHEZZA\n");
        return -1;
    }

    uint32_t len = ntohl(net_len);

    char buf[len];

    //ricevo il buffer delle porte
    if(recv(l2u_socket.socket, buf, len, MSG_WAITALL) < 0){
        printf("[ERR] ERRORE NELLA RICEZIONE DELLE PORTE\n");
        return -1;
    }

    //rimuovo il vecchio buffer
    free(porte_utenti);
    
    num_utenti = len / sizeof(uint16_t);
    //creo il nuovo buffer e lo riempio
    porte_utenti = (uint16_t*) malloc(sizeof(uint16_t) * num_utenti);

    for(int i = 0; i < len; i += 2){
        
        uint16_t net_port;
        memcpy(&net_port, buf + i, sizeof(uint16_t));
        uint16_t port = ntohs(net_port);
        porte_utenti[i / 2] = port;
        printf("[INFO] RICEVUTO UTENTE CON PORTA: %d\n", (int) port);
    }


    return 0;
}


int recv_available_card(){

    //ricezione delle info della card nel seguente formato:
    char buf[105];
    
    if(recv(l2u_socket.socket, buf, 105, MSG_WAITALL) < 0){
        printf("[ERR] ERRORE NELLA RICEZIONE DELLE INFORMAZIONI DELLA AVAILABLE CARD\n");
        return -1;
    }
    
    //deserializzazione della card
    uint32_t net_card_id;
    memcpy(&net_card_id, buf, sizeof(uint32_t));
    working_card.id = ntohl(net_card_id);


    strcpy(working_card.testoAttivita, buf + sizeof(uint32_t));

    printf("[INFO] RICEVUTA CARD CON ID: %d TESTO ATTIVITA: %s\n", working_card.id, working_card.testoAttivita);
    return 0;

}


int send_pong(){

    printf("[INFO] RICEVUTO PING\n");

    char pong = 0;
    if(send(l2u_socket.socket, &pong, 1, 0) < 1){
        printf("[ERR] ERRORE NELL'INVIO DEL PONG\n");
        return -1;
    }

    printf("[INFO] PONG INVIATO\n");
    return 0;
}


/**
 * @brief funzione per trovare la struttura del socket di un utente all'interno di un array
 * @param write_sd descrittore in scrittura da cercare
 * @param user_sockets array di strutture
 * @param len dimensione dell'array
 * @return l'indice a cui si trova la struttura cercata o -1 se non c'è
 */
static int find_user_sock(int write_sd, socket_t* user_sockets, int len){

    for(int i = 0; i < len; ++i)
        if(user_sockets[i].socket == write_sd) return i;

    return -1;
}

/**
 * @brief funzione da passare al thread che farà sleep per lavorare sulla card
 */
static void* card_done_handler(void*){

    pthread_detach(pthread_self());

    //generazione di un tempo random da 20 secondi a 2 minuti
    int time = (rand() % 100) + 21;
    printf("[INFO] TEMPO RICHIESTO PER COMPLETARE LA CARD %d: %d SECONDI\n", working_card.id, time);
    sleep(time);

    //dopo aver fatto sleep invio il messaggio di CARD_DONE
    pthread_mutex_lock(&u2l_socket_mutex);

    send_command(CARD_DONE_CMD, u2l_socket.socket);
    printf("[INFO] CARD %d COMPLETATA\n", working_card.id);
    
    //pulizia della working_card
    memset(&working_card, -1, sizeof(working_card));
    
    pthread_mutex_unlock(&u2l_socket_mutex);
    pthread_exit(NULL);
}



int choose_user(){

    socket_t user_sockets[num_utenti];

    //struttura per contenere, per ogni descrittore se è usato per la lettura il numero di byte ricevuti e il buffer di memorizzazione
    //se è utilizzato in scrittura il numero di byte mandati
    struct {
        int sent_bytes;

        int recv_bytes;
        char recv_buffer[6];
    } transfer_state[FD_SETSIZE];

    memset(transfer_state, 0, sizeof(transfer_state));

    //inizializzazione dei fari set
    fd_set connect_set; // set che contiene i descrittori per la connect
    FD_ZERO(&connect_set);

    fd_set write_set, write_ready_set; //set per la gestione dei descrittori in scrittura
    FD_ZERO(&write_set);
    FD_ZERO(&write_ready_set);

    fd_set read_set, read_ready_set; // set che contengono i descrittori per ricevere CHOOSE_USER e il listener
    FD_ZERO(&read_set);
    FD_ZERO(&read_ready_set);
    FD_SET(listener_socket.socket, &read_set);


    int fd_max = listener_socket.socket;
    int recv_count = 0;
    int sent_count = 0;

    for(int i = 0; i < num_utenti; ++i) {

        if(porte_utenti[i] == my_port) {
            memset(&user_sockets[i], 0, sizeof(socket_t));
            continue;
        }

        create_socket(&user_sockets[i], porte_utenti[i], 0);
        int ret = connect(user_sockets[i].socket, (struct sockaddr*) &user_sockets[i].addr, sizeof(struct sockaddr));

        //controllo se la connessione è andata a buon fine o è ancora in corso
        if(ret < 0 && errno == EINPROGRESS){
            //se la connessione è in corso aggiungo il socket al connect set e al write set
            FD_SET(user_sockets[i].socket, &connect_set);
            FD_SET(user_sockets[i].socket, &write_set);
            if(user_sockets[i].socket > fd_max) fd_max = user_sockets[i].socket;
        }else if(ret >= 0){

            // se la connessione è andata a buon fine aggiungo il socket al write set
            FD_SET(user_sockets[i].socket, &write_set);
            if(user_sockets[i].socket > fd_max) fd_max = user_sockets[i].socket;
        }else {

            close(user_sockets[i].socket);
            sent_count ++;
            //connessione fallita
            printf("[ERR] CONNESSIONE CON L'UTENTE CON PORTA: %d FALLITA\n", (int)porte_utenti[i]);
        }  
    }
    
    //generazione del costo e serializzazione
    uint32_t my_costo =  rand();
    int have_min_costo = 1;
    
    char send_buffer[6];
    uint32_t net_my_costo = htonl(my_costo);
    uint16_t net_my_port = htons(my_port);

    memcpy(send_buffer, &net_my_port, sizeof(uint16_t));
    memcpy(send_buffer + sizeof(uint16_t), &net_my_costo, sizeof(uint32_t));

    while(recv_count != num_utenti - 1 || sent_count != num_utenti - 1){

        write_ready_set = write_set;
        read_ready_set = read_set;

        //timeout di 10 secondi, se non si riceve nulla in questo tempo si considera valido quello già ricevuto
        struct timeval timeout;
        timeout.tv_sec = 10;
        timeout.tv_usec = 0;

        int select_ret = select(fd_max + 1, &read_ready_set, &write_ready_set, NULL, &timeout);

        if(select_ret == 0){
            printf("[WRN] TIMEOUT SCADUTO\n");
            break;
        }else if(select_ret < 0){
            printf("[ERR] ERRORE NELLA SELECT\n");

            //chiusura di tutti i socket e terminazione
            for(int i = 0; i < fd_max + 1; ++i){
                if(FD_ISSET(i, &write_set) || FD_ISSET(i, &read_set)){
                    if(i != listener_socket.socket) close(i);
                }
            }
            _exit(1);
        }

        for(int i = 0; i < fd_max + 1; ++i){

            if(FD_ISSET(i, &read_ready_set) && i == listener_socket.socket){

                //se il descrittore pronto è il listener accetto la nuova connessione
                struct sockaddr_in user_addr;
                unsigned int len = sizeof(user_addr);

                int new_fd = accept(listener_socket.socket, (struct sockaddr*) &user_addr, &len);

                if(new_fd < 0){
                    printf("[ERR] ERRORE NELLA ACCEPT\n");
                    continue;
                }

                memset(&transfer_state[new_fd], 0, sizeof(transfer_state[new_fd]));
                FD_SET(new_fd, &read_set);
                if(new_fd > fd_max) fd_max = new_fd;

            }else if(FD_ISSET(i, &write_ready_set) && FD_ISSET(i, &connect_set)){ // se il socket pronto è uno di quelli per la connect

                int index = find_user_sock(i, user_sockets, num_utenti);

                socket_t* user_socket = &user_sockets[index];
                int ret = connect(user_socket->socket, (struct sockaddr*) &user_socket->addr, sizeof(struct sockaddr));

                // se la connessione è ancora in corso non devo fare niente
                if(ret < 0 && errno == EINPROGRESS) continue;
                else if(ret == 0 || (ret < 0 && errno == EISCONN)) 
                    //se la connessione è andata a buon fine rimuovo il descrittore dal set dedicato alle connect
                    FD_CLR(i, &connect_set);
                else{
                    // se la connessione non è andata a buon fine rimuovo il descrittore da tutti i set
                    printf("[ERR] ERRORE NELLA CONNESSIONE CON L'UTENTE CON PORTA %d\n", (int)user_socket->porta);
                    close(i);
                    FD_CLR(i, &connect_set);
                    FD_CLR(i, &write_set);
                    //calcolo del nuovo fd_max
                    while(fd_max >= 0 && !FD_ISSET(fd_max, &read_set) && !FD_ISSET(fd_max, &write_set)) fd_max --;
                }
                
            }else if(FD_ISSET(i, &write_ready_set) && !FD_ISSET(i, &connect_set)){ // se il socket pronto è uno di quelli in scrittura mando il costo

                //mando il mio costo
                int n = send(i, send_buffer + transfer_state[i].sent_bytes, 6 - transfer_state[i].sent_bytes, 0);

                if(n > 0){
                    // se la send è andata a buon aggiorno i bytes mandati
                    transfer_state[i].sent_bytes += n;

                    //se ho mandato tutti i bytes ho finito chiudo il socket e pulisco il set
                    if(transfer_state[i].sent_bytes == 6){
                        
                        int index = find_user_sock(i, user_sockets, num_utenti);
                        printf("[INFO] MANDATO COSTO %d E PORTA ALL'UTENTE CON PORTA %d\n", my_costo, (int)user_sockets[index].porta);
                        
                        //chiusura del socket e pulizia del set
                        close(i);
                        FD_CLR(i, &write_set);
                        while(fd_max >= 0 && !FD_ISSET(fd_max, &read_set) && !FD_ISSET(fd_max, &write_set)) fd_max --;
                        
                        //incremento il numero di dati mandati con successo
                        sent_count ++;
                    }
                }else if(n < 0 && (errno == EAGAIN || errno == EWOULDBLOCK))
                    // il send buffer è pieno quindi non faccio niente
                    continue;
                else{

                    int index = find_user_sock(i, user_sockets, num_utenti);
                    printf("[ERR] ERRORE NELLA SEND DEL COSTO VERSO L'UTENTE CON PORTA: %d\n", (int) user_sockets[index].porta);

                    //la send è fallita chiudo il descrittore e pulisco il set in scrittura
                    close(i);
                    FD_CLR(i, &write_set);
                    while(fd_max >= 0 && !FD_ISSET(fd_max, &read_set) && !FD_ISSET(fd_max, &write_set)) fd_max --;
                    // se la send fallisce incremento sent_count perchè non posso più mandare dati a quell'utente
                    sent_count ++;
                }

            }else if(FD_ISSET(i, &read_ready_set) && i != listener_socket.socket){ // se il socket pronto è tra quelli in lettura ricevo il costo

                //ricevo i bytes che mi mancano da ricevere
                int n = recv(i, transfer_state[i].recv_buffer + transfer_state[i].recv_bytes, 6 - transfer_state[i].recv_bytes, 0);

                if(n > 0){
                    // la receive è andata a buon fine controllo quanti bytes ho ricevuto
                    transfer_state[i].recv_bytes += n;

                    if(transfer_state[i].recv_bytes == 6){
                        //ho ricevuto tutti i bytes deserializzo e controllo il costo

                        char* recv_buffer = transfer_state[i].recv_buffer;

                        uint16_t net_port;
                        uint32_t net_costo;

                        memcpy(&net_port, recv_buffer, sizeof(short));
                        memcpy(&net_costo, recv_buffer + sizeof(short), sizeof(int));

                        uint16_t port = ntohs(net_port);
                        uint32_t costo = ntohl(net_costo);

                        if(my_costo > costo || (my_costo == costo && my_port > port)) have_min_costo = 0;

                        printf("[INFO] RICEVUTO COSTO %d DALL'UTENTE CON PORTA %d\n", costo, (int) port);

                        //chiusura del socket e pulizia dei set
                        close(i);
                        FD_CLR(i, &read_set);
                        while(fd_max >= 0 && !FD_ISSET(fd_max, &read_set) && !FD_ISSET(fd_max, &write_set)) fd_max --;

                        //incremento il numero di dati ricevuti con successo
                        recv_count ++;
                    }

                }else if(n < 0 && (errno == EAGAIN || errno == EWOULDBLOCK)){
                    //se non ci sono dati non faccio niente
                    continue;
                }else {

                    printf("[ERR] ERRORE NELLA RECV DEL COSTO E PORTA DA UN UTENTE\n");

                    //la recv è fallita chiudo il descrittore e pulisco il set in lettura
                    close(i);
                    FD_CLR(i, &read_set);
                    while(fd_max >= 0 && !FD_ISSET(fd_max, &read_set) && !FD_ISSET(fd_max, &write_set)) fd_max --;

                    //se la receive fallisce incremento recv_count perchè non posso ricevere dati da quell'utente
                    recv_count ++;
                }
            }
        }
    }

    //nel caso il ciclo sopra fosse stato interrotto a cause del timeout chiudo i socket rimasti aperti nei set
    for(int i = 0; i < fd_max + 1; ++i){
        if(FD_ISSET(i, &write_set) || FD_ISSET(i, &read_set)){
            if(i != listener_socket.socket) close(i);
        }
    }

    if(!have_min_costo) {
        //pulizia della working_card
        memset(&working_card, -1, sizeof(working_card));
        return 0;
    }

    pthread_mutex_lock(&u2l_socket_mutex);

    //invio del comando di ACK_CARD
    send_command(ACK_CARD_CMD, u2l_socket.socket);


    //invio dell'id della card
    uint32_t net_id = htonl(working_card.id);
    if(send(u2l_socket.socket, &net_id, sizeof(uint32_t), 0) < sizeof(int)){
        printf("[ERR] ERRORE DURANTE L'INVIO DELL'ID DELLA CARD DA ACKARE\n");
        pthread_mutex_unlock(&u2l_socket_mutex);
        return -1;
    }


    //controllo se la card di cui sto mandando l'ACK è gia stata ACKATA 
    char already_acked;
    if(recv(u2l_socket.socket, &already_acked, 1, MSG_WAITALL) < 0){
        printf("[ERR] ERRORE NELLA RICEZIONE DELLA RISPOSTA A ACK CARD\n");
        pthread_mutex_unlock(&u2l_socket_mutex);
        return -1;
    }

    //se la card è gia stata ackata non faccio niente
    if(already_acked){
        printf("[WRN] CARD GIA ACKATA\n");
        pthread_mutex_unlock(&u2l_socket_mutex);
        return 0;
    }

    
    printf("[INFO] ASSEGNATA CARD %d\n", working_card.id);
    pthread_mutex_unlock(&u2l_socket_mutex);


    //creo un nuovo thread che farà la sleep e notificherà la lavagna della fine della card questo per evitare di bloccare la comunicazione
    //con la lavagna

    pthread_t card_done_handling_t;
    pthread_create(&card_done_handling_t, NULL, card_done_handler, NULL);


    return 0;
}

