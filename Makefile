#
# Student makefile for cs154 Project 4
#
# For this project we require that your code compiles
# cleanly (without warnings), hence the -Werror option
sash: sash.c
	gcc -Wall -Werror -o sash -lreadline sash.c

clean:
	rm -f sash
