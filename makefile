
OBJLAVDIR = build/lavagna
SRCLAVDIR = src/lavagna

OBJUSRDIR = build/utente
SRCUSRDIR = src/utente

CFLAGS = -Wall -g

all: directories eseguibili/lavagna eseguibili/utente 

directories:
	mkdir -p eseguibili
	mkdir -p $(OBJLAVDIR)
	mkdir -p $(OBJUSRDIR)


eseguibili/lavagna: $(OBJLAVDIR)/lavagna.o $(OBJLAVDIR)/lavagna_functions.o build/socket_util.o
	gcc $(CFLAGS) -o eseguibili/lavagna $(OBJLAVDIR)/lavagna.o $(OBJLAVDIR)/lavagna_functions.o build/socket_util.o

eseguibili/utente: $(OBJUSRDIR)/utente.o build/socket_util.o
	gcc $(CFLAGS) -o eseguibili/utente $(OBJUSRDIR)/utente.o build/socket_util.o

$(OBJLAVDIR)/lavagna.o: $(SRCLAVDIR)/lavagna.c include/lavagna/lavagna_functions.h include/strutture.h
	gcc $(CFLAGS) -c $(SRCLAVDIR)/lavagna.c -o $(OBJLAVDIR)/lavagna.o

$(OBJLAVDIR)/lavagna_functions.o: $(SRCLAVDIR)/lavagna_functions.c include/lavagna/lavagna_functions.h include/strutture.h
	gcc $(CFLAGS) -c $(SRCLAVDIR)/lavagna_functions.c -o $(OBJLAVDIR)/lavagna_functions.o

build/socket_util.o: src/socket_util.c include/socket_util.h include/strutture.h
	gcc $(CFLAGS) -c src/socket_util.c -o build/socket_util.o

$(OBJUSRDIR)/utente.o: $(SRCUSRDIR)/utente.c include/strutture.h include/socket_util.h
	gcc $(CFLAGS) -c $(SRCUSRDIR)/utente.c -o $(OBJUSRDIR)/utente.o
