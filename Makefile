# target all means all targets currently defined in this file
all: shell.zip shell.c

WARNS=-Wall -pedantic -Wextra
CFLAGS=-g3 -std=gnu11 ${WARNS}
LIBS=

# this target is the .zip file that must be submitted to Carmen
shell.zip: Makefile shell.c
	zip -r shell.zip shell.c Makefile README.txt headers.sh headers.awk

shell: shell.o 
	gcc ${CFLAGS} shell.o -o shell -lm

shell.o: shell.c
	gcc -pedantic -g -c shell.c
# this target deletes all files produced from the Makefile
# so that a completely new compile of all items is required
clean:
	rm -rf *.o shell shell.zip

