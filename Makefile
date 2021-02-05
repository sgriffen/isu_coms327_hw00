all: sandpile.o

sandpile.o: sandpile.c sandpile.h
	gcc -Wall -Werror -g3 sandpile.c -o sandpile.o
	
clean:
	rm -f *.o