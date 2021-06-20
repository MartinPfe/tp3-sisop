CC=gcc
CFLAGS=-lpthread -lrt -pthread
OBJSV = servidor.c 
OBJCLI = cliente.c

all:cliente servidor
	
cliente: $(OBJCLI)
	$(CC) -o $@ $^ $(CFLAGS)

servidor: $(OBJSV)
	$(CC) -o $@ $^ $(CFLAGS)
