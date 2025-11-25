

#include "lavagna_functions.h"



int main(){
    
    init_lavagna();

    card_t* card1 = create_card("sono card1");
    sleep(5);
    card_t* card2 = create_card("sono card2");

    insert_card(card1);
    insert_card(card2);

    card_t* lista = lavagna.colonne[TODO];
    while(lista){
        stampa_card(*lista);
        lista = lista->nextCard;
    }

    return 0;
}