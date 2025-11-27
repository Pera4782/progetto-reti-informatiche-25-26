

#include "lavagna_functions.h"



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

    destroy_lavagna();
    return 0;
}