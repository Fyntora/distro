CC=gcc
CFLAGS=-Wall -O2

OBJS = main.o kernel.o utils.o rootfs.o iso.o
TARGET = linux_builder

all: $(TARGET)

$(TARGET): $(OBJS)
	$(CC) $(CFLAGS) -o $(TARGET) $(OBJS)

main.o: main.c kernel.h utils.h rootfs.h iso.h
	$(CC) $(CFLAGS) -c main.c

kernel.o: kernel.c kernel.h utils.h
	$(CC) $(CFLAGS) -c kernel.c

utils.o: utils.c utils.h
	$(CC) $(CFLAGS) -c utils.c

rootfs.o: rootfs.c rootfs.h utils.h
	$(CC) $(CFLAGS) -c rootfs.c

iso.o: iso.c iso.h utils.h
	$(CC) $(CFLAGS) -c iso.c

clean:
	rm -f $(OBJS) $(TARGET)
