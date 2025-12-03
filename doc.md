### Invio di comandi alla lavagna

l'invio dei comandi verso la lavagna avviente attraverso una connessione TCP per motivi di affidabilità, non si vuole per esempio perdere una creazione di una card o registrare un utente su una porta sbagliata, la connessione rimane aperta finchè l'utente non richiede la disconnessione o chiude il socket, i vari step della communicazione sono (dopo aver instaurato la connessione):

1. l'utente invia di un byte che rappresenta il comando che vuole eseguire (es. 0 per HELLO, 1 per CREATE_CARD) 
2. la lavagna risponde con 1 byte di ACK comando (può avere qualsiasi valore tanto non viene letto dall'utente)
3. l'utente può procedere con l'invio dei dati, il formato dipende dal comando che si sta eseguendo (vedi sezione formato messaggi)
4. per le comunicazioni che prevedono una scelta (per esempio la porta) da parte dell'utente la lavagna comunicherà 1 byte che se vale 1 la scelta è andata a buon fine 0 altrimenti

La concorrenza sulla lavagna è gestita tramite multi-threading (questo per limitare l'overhead di creazione/distruzione dei thread stessi) in quanto utilizzando IO multiplexing la gestione di ogni singolo tipo di comando e stato in cui si può trovare un descrittore sarebbe di complessità molto elevata inoltre l'accesso concorrente alle strutture dati non è un grande problema in quanto le operazioni sulla lavagna sono relativamente brevi questo permette quindi una parallelizzazione effettiva, d'altro canto utilizzare IO multiplexing avrebbe evitato l'overhead di gestione dei thread a costo di una complessità della gestione delle connessioni molto più elevata

### Formato messaggi

questa sezione contiene il formato dei messaggi per ogni comando:

- comando **HELLO**: | 2 byte PORTA |
- comando **CREATE_CARD**: | 4 byte ID | 101 byte TESTO ATTIVITA | 1 byte COLONNA |
- comando **QUIT**: non è necessario inviare nessun messaggio, l'invio del comando specificato sopra è sufficiente