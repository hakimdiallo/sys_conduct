CC=gcc
CFLAGS=-g -Wall -pthread
CC_RAS=arm-linux-gnueabi-gcc
SRC=$(wildcard *.c)
HEADER=$(wildcard *.h)
OBJ=$(SRC:.c=.o)
EXEC=reader writer reader_v writer_v read_pipe write_pipe serveur client julia

all: $(EXEC)

reader: conduct.c reader.c $(HEADER)
	$(CC) $(CFLAGS) -o $@ $^

writer: conduct.c write.c $(HEADER)
	$(CC) $(CFLAGS) -o $@ $^

reader_v: conduct.c read_v.c $(HEADER)
	$(CC) $(CFLAGS) -o $@ $^

writer_v: conduct.c write_v.c $(HEADER)
	$(CC) $(CFLAGS) -o $@ $^

read_pipe: read_pipe.c $(HEADER)
	$(CC) $(CFLAGS) -o $@ $^

write_pipe: write_pipe.c $(HEADER)
	$(CC) $(CFLAGS) -o $@ $^

server: serveur.c $(HEADER)
	$(CC) $(CFLAGS) -o $@ $^

client: client.c $(HEADER)
	$(CC) $(CFLAGS) -o $@ $^

julia: conduct.c julia.c $(HEADER)
	$(CC) -o3 -ffast-math $(FLAGS) `pkg-config --cflags gtk+-3.0` -o $@ $^ `pkg-config --libs gtk+-3.0` -lm

%.o: %.c
	$(CC) $(CFLAGS) $(CLIBS) -o $@ -c $<

clean:
	rm -f *.o core

mrproper:clean
	rm -f $(EXEC)
