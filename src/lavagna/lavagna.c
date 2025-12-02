#include "../../include/lavagna/lavagna_functions.h"

/**
 * @brief funzione che verrà utilizzata per creare un thread per la gestione di richieste in arrivo da parte dei client
 * @param arg argomento del thread contente un puntatore al descrittore del socket della connessione con il client
 * @return NULL
 */
void* request_handler(void* arg){

    pthread_detach(pthread_self());
    int sd = *((int*) arg);

    while(1){

        //ricezione del comando inviato dal client
        char command = recv_command(sd);
        if(command == -1){
            pthread_mutex_lock(&mutex_lavagna);
            utente_t* u = remove_utente(sd);
            free(u);
            pthread_mutex_unlock(&mutex_lavagna);
            break;
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
    }

    close(sd);
    free(arg);
    pthread_exit(NULL);
}


int main(){

    init_lavagna();

    card_t* card = create_card("soono una card", 0, TODO);
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
            printf("ERRORE SULLA ACCEPT\n");
            exit(1);
        }

        int* thread_new_sd = (int*) malloc(sizeof(int));
        *thread_new_sd = new_sd;

        //creazione del thread di gestione della richiesta
        pthread_t request_handling_t;
        if(pthread_create(&request_handling_t, NULL, request_handler, thread_new_sd) != 0){
            printf("ERRORE NELLA CREAZIONE DEL THREAD DI GESTIONE\n");
            exit(1);
        }
    }

    destroy_lavagna();
    return 0;
}