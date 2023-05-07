myset: myset.o set.o
	gcc -ansi -Wall -pedantic -std=c99  myset.o set.o -o myset
myset.o: myset.c set.h
	gcc -c -ansi -Wall -pedantic -std=c99  myset.c -o myset.o
set.o: set.c set.h
	gcc -c -ansi -Wall -pedantic -std=c99  set.c -o set.o
