### Comunicazione lavagna <-> utente 

la comunicazione tra lavagna e utente avviente attraverso una connessione TCP per motivi di affidabilità, non si vuole per esempio perdere una creazione di una card o registrare un utente su una porta sbagliata, i vari step della communicazione sono (dopo aver instaurato la connessione):

1. l'utente invia di un byte che rappresenta il comando che vuole eseguire (es. 0 per HELLO, 1 per CREATE_CARD) 
2. la lavagna risponde con 1 byte di ACK comando (può avere qualsiasi valore tanto non viene letto dall'utente)
3. l'utente può procedere con l'invio dei dati, il formato dipende dal comando che si sta eseguendo (vedi sezione formato messaggi)
4. per le comunicazioni che prevedono una scelta (per esempio la porta) da parte dell'utente la lavagna comunicherà 1 byte che se vale 1 la scelta è andata a buon fine 0 altrimenti

La concorrenza sulla lavagna è gestita tramite multi-threading in quanto un utente per completare un task potrebbe metterci un tempo considerevole quindi non avrebbe senso mantenere la lavagna in una sorta di "busy wait" dove ciclicamente controlla se ci sono dei socket pronti.

### Formato messaggi

questa sezione contiene il formato dei messaggi per ogni comando:

- comando **HELLO**: | 2 byte PORTA |
- comando **CREATE_CARD**: | 4 byte ID | 101 byte TESTO ATTIVITA | 1 byte COLONNA |