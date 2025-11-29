#include "../../include/lavagna/lavagna_functions.h"


int main(){

    init_lavagna();

    card_t* card = create_card("soono una card", 0, DOING);
    insert_card(card);
    show_lavagna();


    socket_t server_sock; //socket che sarà destinato ad accettare le richieste da parte dei client
    
    prepare_server_socket(&server_sock);

    while(1){

        struct sockaddr_in client_addr;
        unsigned int len = sizeof(struct sockaddr);
        int new_sd = accept(server_sock.socket, (struct sockaddr*) &client_addr, &len);
        if(new_sd < 0){
            perror("ERRORE SULLA ACCEPT");
            exit(1);
        }
        printf("connessione accettata\n");

        char command = recv_command(new_sd);
        if(command == 0xFF){
            printf("ERRORE NELLA RICEZIONE DEL COMANDO\n");
            exit(1);
        }
        

        switch(command){

            case 0: 
                hello_answer(new_sd);
                break;
            case 1:
                create_card_answer(new_sd);
                break;
            default: printf("COMANDO NON RICONOSCIUTO\n");
        }

        show_lavagna();
        close(new_sd);
    }

    destroy_lavagna();
    return 0;
}