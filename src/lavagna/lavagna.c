#include "../../include/lavagna/lavagna_functions.h"
#include "../../include/lavagna/request_handlers.h"

socket_t utl_socket; //socket che sarà destinato ad accettare le richieste di comandi da parte dei client

/**
 * @brief funzione che verrà utilizzata per creare un thread per la gestione di richieste in arrivo da parte dei client
 * @param arg argomento del thread contente un puntatore al descrittore del socket della connessione con il client
 * @return NULL
 */
static void* request_handler(void* arg){

    pthread_detach(pthread_self());
    int sd = *((int*) arg);

    int quitted = 0;
    while(quitted == 0){

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
            case HELLO_CMD:

                hello_handler(sd);
                break;

            case CREATE_CARD_CMD:

                create_card_handler(sd);
                break;

            case QUIT_CMD:

                quit_handler(sd);
                quitted = 1;
                break;

            default: printf("COMANDO NON RICONOSCIUTO\n");
        }
    }

    close(sd);
    free(arg);
    pthread_exit(NULL);
}


/**
 * @brief funzione da passare al thread per gestire i comandi in entrata da STDIN
*/
static void* input_handler(void*){
 
    char input[81];

    while (1){

        //acquisizione del comando da STDIN
        if(fgets(input, 81, stdin) == NULL){
            printf("ERRORE NELL'ACQUISIZIONE DEL COMANDO\n");
            exit(1);
        }
        //rimozione del newline nell'input
        input[strcspn(input, "\n")] = 0;
        
        if(strcmp(input, "SHOW_LAVAGNA") == 0){

            pthread_mutex_lock(&mutex_lavagna);
            show_lavagna();
            pthread_mutex_unlock(&mutex_lavagna);
        
        }else printf("COMANDO NON RICONOSCIUTO\n");
        
    }

    pthread_exit(NULL);
}



int main(){

    init_lavagna();

    show_lavagna();

    //creazione del thread per ricevere input da linea di comando
    pthread_t input_handling_t;
    pthread_create(&input_handling_t, NULL, input_handler, NULL);
    
    prepare_listener_socket(&utl_socket, LAVAGNAPORT, 1);

    while(1){
        
        //il server si mette in attesa di una richiesta dal client
        struct sockaddr_in client_addr;
        unsigned int len = sizeof(struct sockaddr);
        int u2l_sd = accept(utl_socket.socket, (struct sockaddr*) &client_addr, &len);
        if(u2l_sd < 0){
            printf("ERRORE SULLA ACCEPT\n");
            exit(1);
        }

        int* thread_u2l_sd = (int*) malloc(sizeof(int));
        *thread_u2l_sd = u2l_sd;

        //creazione del thread di gestione della richiesta
        pthread_t request_handling_t;
        if(pthread_create(&request_handling_t, NULL, request_handler, thread_u2l_sd) != 0){
            printf("ERRORE NELLA CREAZIONE DEL THREAD DI GESTIONE\n");
            exit(1);
        }
    }

    destroy_lavagna();
    pthread_exit(NULL);
    return 0;
}