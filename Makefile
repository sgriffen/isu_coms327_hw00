all: sandpile

sandpile: sandpile.c sandpile.h
	gcc -Wall -Werror -g3 sandpile.c -o sandpile
	
clean:
	rm -f *.o