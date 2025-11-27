#include "../../include/lavagna/lavagna_functions.h"
#include "../../include/socket_util.h"

#define REQUESTQUEUE 256

int main(){

    init_lavagna();

    card_t* card1 = create_card("implementazione integrazione per il pagamento");
    card_t* card2 = create_card("diagramma delle classi UML");
    card_t* card3 = create_card("studio dei requisiti dell'applicazione");
    card_t* card4 = create_card("implementazione sito web servizio");

    card2->colonna = DOING;
    card3->colonna = DONE;

    insert_card(card1);
    insert_card(card2);
    insert_card(card3);
    insert_card(card4);

    show_lavagna();

    socket_t server_sock;
    if(create_socket(&server_sock, LAVAGNAPORT) < 0){
        printf("IMPOSSIBILE CREARE IL SOCKET\n");
        exit(1);
    }

    if(bind(server_sock.socket, (struct sockaddr*) &server_sock.addr, sizeof(struct sockaddr))){
        perror("ERRORE NELLA BIND");
        exit(1);
    }

    if(listen(server_sock.socket, REQUESTQUEUE) < 0){
        perror("ERRORE NELLA LISTEN");
        exit(1);
    }

    while(1){

        struct sockaddr_in client_addr;
        unsigned int len = sizeof(struct sockaddr);
        int new_sd = accept(server_sock.socket, (struct sockaddr*) &client_addr, &len);
        if(new_sd < 0){
            perror("ERRORE SULLA ACCEPT");
            exit(1);
        }

        printf("connessione accettata\n");
    }

    destroy_lavagna();
    return 0;
}