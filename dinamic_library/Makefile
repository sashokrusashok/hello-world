objects = functions libcomplex.a main
CC = gcc

.PHONY: clean

vpath %.c lib:src
vpath %.o lib:src
vpath %.h include

all: $(objects)

functions: add.o sub.o mul.o division.o 
		#$(CC) -c add.c sub.c mul.c division.c

libcomplex.a:
		ar cr libcomplex.a add.o sub.o mul.o division.o

main: mycomplex.o
		$(CC) mycomplex.o -o clean -L. -lcomplex

start: 
		./clean

clean: 
		cd src	&& $(MAKE) messege
		cd lib	&& $(MAKE) messege
		rm *.o clean libcomplex.a