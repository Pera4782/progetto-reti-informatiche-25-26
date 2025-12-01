#include "../../include/lavagna/lavagna_functions.h"

/**
 * @brief funzione che verrà utilizzata per creare un thread per la gestione di richieste in arrivo da parte dei client
 * @param arg argomento del thread contente un puntatore al descrittore del socket della connessione con il client
 * @return NULL
 */
void* request_handler(void* arg){

    int sd = *((int*) arg);

    //ricezione del comando inviato dal client
    char command = recv_command(sd);
    if(command == 0xFF){
        printf("ERRORE NELLA RICEZIONE DEL COMANDO\n");
        exit(1);
    }
    
    //scelta dell'azione da eseguire in relazione al comando
    switch(command){
        case 0: 
            hello_handler(sd);
            break;
        case 1:
            create_card_handler(sd);
            break;
        default: printf("COMANDO NON RICONOSCIUTO\n");
    }

    close(sd);
    return NULL;
}


int main(){

    init_lavagna();

    card_t* card = create_card("soono una card", 0, DOING);
    insert_card(card);
    show_lavagna();


    socket_t server_sock; //socket che sarà destinato ad accettare le richieste da parte dei client
    
    prepare_server_socket(&server_sock);

    while(1){

        //il server si mette in attesa di una richiesta dal client

        struct sockaddr_in client_addr;
        unsigned int len = sizeof(struct sockaddr);
        int new_sd = accept(server_sock.socket, (struct sockaddr*) &client_addr, &len);
        if(new_sd < 0){
            perror("ERRORE SULLA ACCEPT");
            exit(1);
        }
        printf("connessione accettata\n");


        pthread_t request_handling_t;
        if(pthread_create(&request_handling_t, NULL, request_handler, &new_sd) != 0){
            perror("ERRORE NELLA CREAZIONE DEL THREAD DI GESTIONE");
            exit(1);
        }
    }

    destroy_lavagna();
    return 0;
}