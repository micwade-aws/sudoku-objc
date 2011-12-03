#
# Makefile
# Michael J Wade
# 2-4-06
#

#CC=/home/csdev/gcc/bin/gcc
CC=gcc

.SUFFIXES: .m .c

OBJS = main.o rngs.o rvgs.o Individual.o Population.o Astar.o 

PROJ=a.out

LIBS= /home/mike/Projects/libs/icpak102/libicpak102.a -lobjc -lm 
#-lefence

#opts= -O -I/home/csdev/gcc-source/libobjc -I/home/mike/Projects/mocs/icpak102
# MJW - May 22, 2010
opts= -O -I/usr/lib/gcc/x86_64-redhat-linux/4.6.1/include/ -I/home/mike/Projects/libs/icpak102

$(PROJ): $(OBJS)
	$(CC) -o $(PROJ) $(OBJS) $(LIBS)

.m.o:
	$(CC) $(opts) -c $*.m

.c.o:
	$(CC)  $(opts) -c $*.c

clean:
	rm *.P *.i *.o test
