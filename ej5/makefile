CC=gcc
CFLAGS=-lpthread -lrt -pthread
DEPS = game.h
OBJSV = game.c servidor.c 
OBJCLI = cliente.c

%.o: %.c $(DEPS)
	$(CC) -c -o $@ $< $(CFLAGS)

all:cliente servidor
	
cliente: $(OBJCLI)
	$(CC) -o $@ $^ $(CFLAGS)

servidor: $(OBJSV)
	$(CC) -o $@ $^ $(CFLAGS)
