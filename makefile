
OBJLAVDIR = build/lavagna
SRCLAVDIR = src/lavagna

OBJUSRDIR = build/utente
SRCUSRDIR = src/utente

all: eseguibili/lavagna eseguibili/utente

eseguibili/lavagna: $(OBJLAVDIR)/lavagna.o $(OBJLAVDIR)/lavagna_functions.o build/socket_util.o
	gcc -Wall -o eseguibili/lavagna $(OBJLAVDIR)/lavagna.o $(OBJLAVDIR)/lavagna_functions.o build/socket_util.o

eseguibili/utente: $(OBJUSRDIR)/utente.o build/socket_util.o
	gcc -Wall -o eseguibili/utente $(OBJUSRDIR)/utente.o build/socket_util.o

$(OBJLAVDIR)/lavagna.o: $(SRCLAVDIR)/lavagna.c include/lavagna/lavagna_functions.h include/strutture.h
	gcc -Wall -c $(SRCLAVDIR)/lavagna.c -o $(OBJLAVDIR)/lavagna.o

$(OBJLAVDIR)/lavagna_functions.o: $(SRCLAVDIR)/lavagna_functions.c include/lavagna/lavagna_functions.h include/strutture.h
	gcc -Wall -c $(SRCLAVDIR)/lavagna_functions.c -o $(OBJLAVDIR)/lavagna_functions.o

build/socket_util.o: src/socket_util.c include/socket_util.h include/strutture.h
	gcc -Wall -c src/socket_util.c -o build/socket_util.o

$(OBJUSRDIR)/utente.o: $(SRCUSRDIR)/utente.c include/strutture.h include/socket_util.h
	gcc -Wall -c $(SRCUSRDIR)/utente.c -o $(OBJUSRDIR)/utente.o
