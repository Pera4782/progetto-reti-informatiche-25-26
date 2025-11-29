#include "../../include/lavagna/lavagna_functions.h"
#include "../../include/socket_util.h"

#define REQUESTQUEUE 256

int main(){

    init_lavagna();

    socket_t server_sock; //socket che sarà destinato ad accettare le richieste da parte dei client
    
    //creazione del socket del server
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

        char command = recv_command(new_sd);
        if(command == 0xFF){
            printf("ERRORE NELLA RICEZIONE DEL COMANDO\n");
            exit(1);
        }
        

        switch(command){

            case 0: 
                hello_answer(new_sd);
                break;
            default: printf("COMANDO NON RICONOSCIUTO\n");
        }

        close(new_sd);
    }

    destroy_lavagna();
    return 0;
}