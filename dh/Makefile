CC:= gcc

all:	main clean

main:	
	$(CC) -Wall -o dh main.c -lgmp

sources: 
	rm -f Makefile.bak dh a.out *.o

clean: 
	rm -f Makefile.bak *.o
