#
# Makefile
# Michael J Wade
# 2-4-06
#

CC=/home/csdev/gcc/bin/gcc

.SUFFIXES: .m .c

OBJS = main.o rngs.o rvgs.o Individual.o 

PROJ=a.out

LIBS= /home/mwade/Projects/mocs/icpak102/libicpak102.a /home/csdev/gcc/lib/libobjc.a -lm 
#-lefence

opts=-g -I/home/csdev/gcc-source/libobjc -I/home/mwade/Projects/mocs/icpak102

$(PROJ): $(OBJS)
	$(CC) -o $(PROJ) $(OBJS) $(LIBS)

.m.o:
	$(CC) $(opts) -c $*.m

.c.o:
	$(CC)  $(opts) -c $*.c

clean:
	rm *.P *.i *.o test
