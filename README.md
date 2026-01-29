# Progetto Reti Informatiche - Lavagna Condivisa

Questo progetto implementa un sistema distribuito client-server per la gestione di una lavagna condivisa (stile Kanban) che permette agli utenti di creare e gestire attività (Cards).

## Descrizione

Il sistema è composto da due entità principali:
- **Lavagna (Server)**: Gestisce lo stato globale delle attività e coordina le comunicazioni.
- **Utente (Client)**: Si connette alla lavagna per aggiungere nuove attività o visualizzare lo stato.

Le attività (Cards) possono trovarsi in tre stati:
1. **TODO**: Attività da svolgere.
2. **DOING**: Attività in corso di svolgimento da parte di un utente.
3. **DONE**: Attività completata.

## Struttura del Progetto

- `src/`: Contiene il codice sorgente C è diviso in moduli (`lavagna`, `utente`, `socket_util`).
- `include/`: Contiene gli header file condivisi e specifici.
- `makefile`: Script per la compilazione automatica del progetto.

## Compilazione

Per compilare il progetto è necessario avere `gcc` e `make` installati.
Eseguire il comando nella root del progetto:

```bash
make
```

Verranno generati:
- La cartella `eseguibili/` contenente i binari compilati.
- Due link simbolici nella root: `lavagna` e `utente` per un avvio rapido.

Per pulire i file di compilazione:
```bash
make clean
```

## Esecuzione

### Passo 1: Avviare la Lavagna

Il server deve essere avviato per primo. Ascolta di default sulla porta **5678** (configurata in `include/strutture.h`).

```bash
./lavagna
```

**Comandi Server:**
- `SHOW_LAVAGNA`: Stampa a video lo stato corrente delle colonne e delle card.
- `QUIT`: Termina il server.

### Passo 2: Avviare un Utente

È possibile avviare molteplici istanze di utenti. Ogni utente deve specificare una propria porta di ascolto (necessaria per ricevere aggiornamenti dal server), che deve essere **>= 5679**.

```bash
./utente <porta_personale>
```

Esempio:
```bash
./utente 6000
```
*(In un altro terminale)*
```bash
./utente 6001
```

**Comandi Utente:**
- `CREATE_CARD`: Inizia la procedura di creazione di una nuova attività. Verrà richiesto di inserire un ID numerico e una descrizione testuale.
- `QUIT`: Disconnette l'utente e chiude l'applicazione.

## Dettagli Implementativi

- **Socket TCP**: Protocollo utilizzato per tutta la comunicazione client-server.
- **Multithreading**: Il server utilizza i thread POSIX (`pthread`) per gestire più client contemporaneamente.
- **Sincronizzazione**: Utilizzo di Mutex e Condition Variables per l'accesso sicuro alle risorse condivise.

