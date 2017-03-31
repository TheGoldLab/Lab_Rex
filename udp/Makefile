# Makefile under QNX

ifndef DEBUG
DEBUG = -g
endif

ifndef OPTIM
OPTIM = -O
endif

CC = gcc
LD = gcc

OUTPUT = udp

OBJ = main.o udp.o 

all: $(OUTPUT)

$(OUTPUT): $(OBJ) 

	$(LD) $(OBJ) ../../../lib/libsocket.a ../../../lib/libc.a -o $@

udp.o: 	udp.c udp.h

main.o: 	main.c
