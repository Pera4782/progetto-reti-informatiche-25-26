#include "../../include/strutture.h"
#include "../../include/socket_util.h"
#include "../../include/utente/utente_functions.h"


socket_t client_sock;
pthread_mutex_t socket_mutex; //semaforo per il socket della connessione con la lavagna

/**
 * @brief funzione che gestisce l'inserimento di CREATE_CARD da linea di comando
 * @return -1 in caso di errore 0 altrimenti
 */
static int create_card_command(){
    
    //acquisizione dei vari parametri
    int id;
    printf("inserisci id: ");
    scanf("%d", &id);

    //pulizia del buffer
    while(getchar() != '\n');

    char testo[101];
    printf("inserisci testo attivita (max 100 caratteri): ");
    if(fgets(testo, 101, stdin) == NULL){
        printf("ERRORE NELL'ACQUISIZIONE DEL TESTO\n");
        return -1;
    }
    testo[strcspn(testo, "\n")] = 0;

    colonna_t colonna;
    printf("inserisci colonna (0, 1 , 2): ");
    scanf("%d", (int*) &colonna);
    if(colonna != 0 && colonna != 1 && colonna != 2){
        printf("ERRORE NELL'INSERIMENTO DELLA COLONNA\n");
        return -1;
    }

    //pulizia del buffer
    while(getchar() != '\n');

    if(create_card(client_sock.socket, id, testo, colonna) < 0){
        printf("ERRORE NELLA CREAZIONE DELLA CARD\n");
        return -1;
    }

    return 0;
}

/**
 * @brief funzione da passare al thread per gestire i comandi in entrata da STDIN
*/
void* input_handler(void*){

    char input[81];

    while (1){

        //acquisizione del comando da STDIN
        if(fgets(input, 81, stdin) == NULL){
            printf("ERRORE NELL'ACQUISIZIONE DEL COMANDO\n");
            exit(1);
        }
        //rimozione del newline nell'input
        input[strcspn(input, "\n")] = 0;
        
        //inserimento del comando CREATE_CARD da linea di comando
        if(strcmp(input, "CREATE_CARD") == 0){
            
            pthread_mutex_lock(&socket_mutex);
            create_card_command();
            pthread_mutex_unlock(&socket_mutex);
            
        }else {
            printf("COMANDO NON RICONOSCIUTO\n");
        }
    }

    pthread_mutex_lock(&socket_mutex);
    close(client_sock.socket);
    pthread_mutex_unlock(&socket_mutex);
    pthread_exit(NULL);
}

int main(int argc, char** argv) {

    create_socket(&client_sock, LAVAGNAPORT);
    pthread_mutex_init(&socket_mutex, NULL);

    if(argc == 1){
        printf("FORMATO DI ESECUZIONE ERRATO, INSERIRE LA PORTA\n");
        exit(1);
    }
    const unsigned short PORT = (unsigned short)atoi(argv[1]);

    //connessione al socket della lavagna e HELLO
    pthread_mutex_lock(&socket_mutex);
    if(socket_connect(&client_sock) < 0) exit(1);

    if(hello(client_sock.socket, PORT) < 0){
        printf("ERRORE NELLA HELLO\n");
        exit(1);
    }
    pthread_mutex_unlock(&socket_mutex);

    //creazione del thread per ricezione dell'input
    pthread_t input_handling_t;
    pthread_create(&input_handling_t, NULL, input_handler, NULL);


    pthread_join(input_handling_t, NULL);
    pthread_exit(NULL);
    return 0;
}