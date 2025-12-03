#include "../../include/strutture.h"
#include "../../include/socket_util.h"
#include "../../include/utente/utente_functions.h"


socket_t request_sock; // socket per inviare comandi alla lavagna
pthread_mutex_t request_sock_mutex; //semaforo per il socket delle richieste alla lavagna


socket_t user_sock; // socket utilizzato dalla lavagna per fare SEND_USER_LIST e AVAILABLE_CARD e dagli utenti per scambiarsi i messaggi di CHOOSE_USER

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

    if(create_card(request_sock.socket, id, testo, colonna) < 0){
        printf("ERRORE NELLA CREAZIONE DELLA CARD\n");
        return -1;
    }

    return 0;
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
        
        //inserimento del comando CREATE_CARD da linea di comando
        if(strcmp(input, "CREATE_CARD") == 0){
            
            pthread_mutex_lock(&request_sock_mutex);
            create_card_command();
            pthread_mutex_unlock(&request_sock_mutex);
        
        }else if(strcmp(input, "QUIT") == 0){

            pthread_mutex_lock(&request_sock_mutex);
            quit(request_sock.socket);
            pthread_mutex_unlock(&request_sock_mutex);

        }else printf("COMANDO NON RICONOSCIUTO\n");
    }

    pthread_exit(NULL);
}




int main(int argc, char** argv) {

    create_socket(&request_sock, LAVAGNAPORT, 1);
    pthread_mutex_init(&request_sock_mutex, NULL);

    if(argc == 1){
        printf("FORMATO DI ESECUZIONE ERRATO, INSERIRE LA PORTA\n");
        exit(1);
    }
    const unsigned short PORT = (unsigned short)atoi(argv[1]);

    if(PORT < 5679){
        printf("NUMERO DI PORTA TROPPO BASSO, DEVE ESSERE ALMENO 5679\n");
        exit(1);
    }

    //connessione al socket della lavagna e HELLO
    pthread_mutex_lock(&request_sock_mutex);

    if(socket_connect(&request_sock) < 0) exit(1);

    if(hello(request_sock.socket, PORT) < 0){
        printf("ERRORE NELLA HELLO\n");
        exit(1);
    }

    pthread_mutex_unlock(&request_sock_mutex);

    //creazione del thread per ricezione dell'input
    pthread_t input_handling_t;
    pthread_create(&input_handling_t, NULL, input_handler, NULL);



    //inizializzazione del socket che si occuperà di ricevere AVAILABLE_CARD e CHOOSE_USER
    prepare_listener_socket(&user_sock, PORT, 0);

    fd_set read_set, write_set; // tutti i socket di scrittura e lettura
    fd_set read_ready_set, write_ready_set; // set dove la select lascerà solo i socket pronti in lettura o scrittura
    int fd_max;

    //struttura contenente il socket dove la lavagna si connetterà e il suo status (cosa sta facendo)
    struct {
        int sd; 
        enum {CONNECTING, AVAILABLE_CARD, SEND_USER_LIST} status;
    } lavagna_sd_status; 

    //pulizia dei set
    FD_ZERO(&read_set); 
    FD_ZERO(&write_set);
    FD_ZERO(&read_ready_set);
    FD_ZERO(&write_ready_set);

    //inserimento del listener nel set di lettura
    FD_SET(user_sock.socket, &read_set);
    fd_max = user_sock.socket;

    while(1){

        read_ready_set = read_set;
        write_ready_set = write_set;

        select(fd_max + 1, &read_set, &write_set, NULL, NULL);


    }


    

    pthread_join(input_handling_t, NULL);
    pthread_exit(NULL);
    return 0;
}