#
# Makefile
# Michael J Wade
# 03-24-2009
#

CC=gcc

.SUFFIXES: .m .c

OBJS = main.o 

PROJ=sudoku-visual.out

LIBS= /home/mike/Projects/mocs/icpak102/libicpak102.a -lobjc -lm -lglut
#-lefence

opts= -g -I/home/csdev/gcc-source/libobjc -I/home/mike/Projects/mocs/icpak102

$(PROJ): $(OBJS)
	$(CC) -o $(PROJ) $(OBJS) $(LIBS)

.m.o:
	$(CC) $(opts) -c $*.m

.c.o:
	$(CC)  $(opts) -c $*.c

clean:
	rm *.P *.i *.o test

