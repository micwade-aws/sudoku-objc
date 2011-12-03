#
# Makefile
# Michael J Wade
# 2-4-06
#

#CC=/home/csdev/gcc/bin/gcc
CC=gcc

.SUFFIXES: .m .c

OBJS = main.o rngs.o rvgs.o Individual.o Astar.o 

PROJ=a.out

LIBS= /home/mike/Projects/mocs/icpak102/libicpak102.a -lobjc -lm 
#-lefence

opts= -O -I/home/csdev/gcc-source/libobjc -I/home/mike/Projects/mocs/icpak102

$(PROJ): $(OBJS)
	$(CC) -o $(PROJ) $(OBJS) $(LIBS)

.m.o:
	$(CC) $(opts) -c $*.m

.c.o:
	$(CC)  $(opts) -c $*.c

clean:
	rm *.P *.i *.o test
