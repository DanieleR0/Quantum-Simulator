
CC = gcc

CFLAGS = -Wall -O2 -pthread

LIBS = -lm

OBJS = main.o circuit.o complex.o complex_vector.o complex_matrix.o circparser.o initparser.o

all: quantum_sim

quantum_sim: $(OBJS)
	$(CC) $(CFLAGS) -o $@ $(OBJS) $(LIBS)

%.o: %.c
	$(CC) $(CFLAGS) -c $<

clean:
	rm -f *.o quantum_sim

.PHONY: all clean