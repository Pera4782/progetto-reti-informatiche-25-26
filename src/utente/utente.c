#include "../../include/strutture.h"
#include "../../include/socket_util.h"
#include "../../include/utente/utente_functions.h"


socket_t u2l_socket; // socket per inviare comandi alla lavagna
pthread_mutex_t u2l_socket_mutex; //semaforo per il socket delle richieste alla lavagna

socket_t l2u_socket; // socket per ricevere dalla lavagna SEND_USER_LIST e AVAILABLE_CARD

socket_t listener_socket; // socket utilizzato per ricevere richieste di connessione da altri utenti e lavagna

short* porte_utenti = NULL; // lista delle porte degli utenti registrati
int num_utenti = 0;

card_t working_card; // card ricevuta dalla lavagna in AVAILABLE_CARD

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

    if(create_card(u2l_socket.socket, id, testo, colonna) < 0){
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
            
            pthread_mutex_lock(&u2l_socket_mutex);
            create_card_command();
            pthread_mutex_unlock(&u2l_socket_mutex);
        
        }else if(strcmp(input, "QUIT") == 0){

            pthread_mutex_lock(&u2l_socket_mutex);
            quit(u2l_socket.socket);
            pthread_mutex_unlock(&u2l_socket_mutex);

        }else printf("COMANDO NON RICONOSCIUTO\n");
    }

    pthread_exit(NULL);
}


static void* request_handler(void*){

    while(1){

        char command = recv_command(l2u_socket.socket);

        if(command == -1){
            printf("ERRORE NELLA RICEZIONE DEL COMANDO\n");
            exit(1);
        }


        switch(command){

            case SEND_USER_LIST:
                if(recv_user_list() < 0){
                    printf("ERRORE NELLA RICEZIONE DELLA LISTA DEGLI UTENTI\n");
                    exit(1);
                }
                choose_user();
                break;

            case AVAILABLE_CARD:
                
                if(recv_available_card() < 0){
                    printf("ERRORE NELLA RICEZIONE DELLA CARD DISPONIBILE\n");
                    exit(1);
                }
                break;

            default:
                printf("COMANDO NON RICONOSCIUTO\n");
                break;
        }

    }

    
    return NULL;
}


int main(int argc, char** argv) {

    //creazione del socket per la comunicazione utente -> lavagna
    if(create_socket(&u2l_socket, LAVAGNAPORT, 1) < 0){
        printf("ERRORE NELLA CREAZIONE DEL SOCKET PER LA COMUNICAZIONE CON LA LAVAGNA\n");
        exit(1);
    }
    
    //inizializzazione semafori
    pthread_mutex_init(&u2l_socket_mutex, NULL);

    //acquisizione della porta da linea di comando
    if(argc == 1){
        printf("FORMATO DI ESECUZIONE ERRATO, INSERIRE LA PORTA\n");
        exit(1);
    }
    const unsigned short PORT = (unsigned short)atoi(argv[1]);

    if(PORT < 5679){
        printf("NUMERO DI PORTA TROPPO BASSO, DEVE ESSERE ALMENO 5679\n");
        exit(1);
    }

    //ignorare il segnale SIGING (CTRL + C)
    signal(SIGINT, SIG_IGN);

    //connessione al socket della lavagna e HELLO
    if(socket_connect(&u2l_socket) < 0) exit(1);

    if(hello(u2l_socket.socket, PORT) < 0){
        printf("ERRORE NELLA HELLO\n");
        exit(1);
    }

    //creazione del thread che riceverà richieste dalla lavagna
    pthread_t request_handling_t;
    pthread_create(&request_handling_t, NULL, request_handler, NULL);

    //creazione del thread per ricezione dell'input
    pthread_t input_handling_t;
    pthread_create(&input_handling_t, NULL, input_handler, NULL);


    pthread_join(input_handling_t, NULL);
    pthread_join(request_handling_t, NULL);
    pthread_exit(NULL);
    return 0;
}