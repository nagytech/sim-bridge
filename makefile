COMPILER = gcc

INCLUDES =

CFLAGS = -Wall -pedantic -pthread

EXES = simbridge

all: ${EXES}

debug: CFLAGS += -DDEBUG -g -pg
debug: all

simbridge: simbridge.c
	${COMPILER} ${INCLUDES} ${CFLAGS} -o simbridge simbridge.c ${LIBS}

clean:
	rm -f *~ *.o ${EXES}
