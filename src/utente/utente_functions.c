#include "../../include/utente/utente_functions.h"


int hello(const int sd, const unsigned short PORT){

    if(send_command(HELLO_CMD, sd) < 0) return -1;

    // una volta ricevuto l'ACK del server si puà procedere con l'invio della porta, 2 byte
    unsigned short net_port = htons(PORT);
    if(send(sd, &net_port, 2, 0) < 2){
        printf("ERRORE NELL'INVIO DELLA PORTA\n");
        return -1;
    }
    
    //una volta inviata la porta si aspetta che il server risponda con un byte per capire se era disponibile
    char disponibile;

    if(recv(sd, &disponibile, 1, MSG_WAITALL) < 1){
        printf("ERRORE NELLA RICEZIONE DEL DISPONIBILE\n");
        return -1;
    }

    if(!disponibile){
        printf("PORTA NON DISPONIBILE\n");
        return -1;
    }
    
    //preparo il socket dedicato a ricevere comandi
    prepare_listener_socket(&listener_socket, PORT, 1);

    //comunico alla lavagna che si può connettere
    char can_connect = 0;
    if(send(sd, &can_connect, 1, 0) < 1){
        printf("ERRORE NELL'INVIO DEL CAN CONNECT\n");
        return -1;
    }

    //accetto la connessione dalla lavagna
    unsigned int len = sizeof(struct sockaddr);
    l2u_socket.socket = accept(listener_socket.socket, (struct sockaddr*) &l2u_socket.addr, &len);
    
    if(l2u_socket.socket < 0){
        printf("ERRORE NELLA ACCEPT\n");
        return -1;
    }

    //setto il listener non bloccante per preparare allo scambio dei messaggi di CHOOSE_USER

    int flags = fcntl(listener_socket.socket, F_GETFL, 0);
    fcntl(listener_socket.socket, F_SETFL, flags | O_NONBLOCK);

    return 0;
}

int create_card(const int sd, const int id, const char* testo, const colonna_t colonna){


    if(strlen(testo) > 100){
        printf("TESTO TROPPO LUNGO\n");
        return -1;
    }

    if(send_command(CREATE_CARD_CMD, sd) < 0) return -1;

    /*
    una volta ricevuto l'ACK si inviano i dati nel seguente formato:
    | 4 byte ID | 101 byte testo + \0 | 1 byte colonna |
    */

    char buf[106];
    
    //serializzazione dell'id

    int net_id = htonl(id);
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
        printf("ERRORE NELL'INVIO DEI DATI\n");
        return -1;
    }

    //una volta inviati i dati si aspetta che il server risponda con 1 byte per capire se l'id era disponibile

    char disponibile;
    if(recv(sd, &disponibile, 1, MSG_WAITALL) < 1){
        printf("ERRORE NELLA RECEIVE DEL DISPONIBILE\n");
        return -1;
    }

    if(!disponibile) {
        printf("ID NON DISPONIBILE\n");
        return -1;
    }

    return 0;
}


int quit(const int sd){

    if(send_command(QUIT_CMD, sd) < 0) return -1;
    exit(0);
}

int recv_user_list(){

    int net_len = 0;
    
    //ricevo la lunghezza del buffer (sizeof(short) * numUtenti)
    if(recv(l2u_socket.socket, &net_len, sizeof(int), MSG_WAITALL) < 0){
        printf("ERRORE NELLA RICEZIONE DELLA LUNGHEZZA\n");
        return -1;
    }

    int len = ntohl(net_len);

    char buf[len];

    //ricevo il buffer delle porte
    if(recv(l2u_socket.socket, buf, len, MSG_WAITALL) < 0){
        printf("ERRORE NELLA RICEZIONE DELLE PORTE\n");
        return -1;
    }

    //rimuovo il vecchio buffer
    free(porte_utenti);
    
    num_utenti = len / sizeof(short);
    //creo il nuovo buffer e lo riempio
    porte_utenti = (short*) malloc(sizeof(short) * num_utenti);

    for(int i = 0; i < len; i += 2){
        
        short net_port;
        memcpy(&net_port, buf + i, sizeof(short));
        short port = ntohs(net_port);
        porte_utenti[i / 2] = port;
        printf("RICEVUTO UTENTE CON PORTA: %d\n", (int) port);
    }


    return 0;
}


int recv_available_card(){

    //ricezione delle info della card nel seguente formato:
    // | 4 byte ID | 101 byte TESTO ATTIVITA |
    char buf[105];
    
    if(recv(l2u_socket.socket, buf, 105, MSG_WAITALL) < 0){
        printf("ERRORE NELLA RICEZIONE DELLE INFORMAZIONI DELLA AVAILABLE CARD\n");
        return -1;
    }
    
    //deserializzazione della card
    int net_card_id;
    memcpy(&net_card_id, buf, sizeof(int));
    working_card.id = ntohl(net_card_id);


    strcpy(working_card.testoAttivita, buf + sizeof(int));

    printf("RICEVUTA CARD:\n ID: %d, TESTO: %s\n", working_card.id, working_card.testoAttivita);
    return 0;

}

//TODO FINIRE LA FUNZIONE
int choose_user(){

    socket_t user_sockets[num_utenti];

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

    for(int i = 0; i < num_utenti; ++i) {
        create_socket(&user_sockets[i], porte_utenti[i], 0);
        int ret = connect(user_sockets[i].socket, (struct sockaddr*) &user_sockets[i].addr, sizeof(struct sockaddr));

        //controllo se la connessione è andata a buon fine o è ancora in corso
        if(ret < 0 && errno == EINPROGRESS){
            FD_SET(user_sockets[i].socket, &connect_set);
            FD_SET(user_sockets[i].socket, &write_set);
        }else if(ret >= 0){
            FD_SET(user_sockets[i].socket, &read_set);
        }else {
            printf("CONNESSIONE CON L'UTENTE CON PORTA: %d FALLITA\n", (int)porte_utenti[i]);
        }
        if(user_sockets[i].socket > fd_max) fd_max = user_sockets[i].socket;
    }
    
    char my_costo = 0;

    while(1){

        write_ready_set = write_set;
        read_ready_set = read_set;

        select(fd_max + 1, &read_ready_set, &write_ready_set, NULL, NULL);

        for(int i = 0; i < fd_max + 1; ++i){

            if(FD_ISSET(i, &read_ready_set)){

                if(i == listener_socket.socket){
                    //se il descrittore pronto è il listener accetto la nuova connessione
                    struct sockaddr_in user_addr;
                    unsigned int len = sizeof(user_addr);

                    int new_fd = accept(listener_socket.socket, (struct sockaddr*) &user_addr, &len);

                    if(new_fd < 0){
                        printf("ERRORE NELLA ACCEPT\n");
                        continue;
                    }

                    FD_SET(new_fd, &read_set);
                    if(new_fd > fd_max) fd_max = new_fd;
                }else {



                }

            }else if(FD_ISSET(i, &write_ready_set)){

            }


        }

    }


    return 0;
}

