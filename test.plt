#!/usr/local/bin/gnuplot -persist
set terminal x11

set yrange [0:3500]

plot "test16.dat" using 1:12 with lines, \
	"../sudoku-astar/results16x16.dat" using 1:4 with lines, \
	x title 'linear'
	  #    EOF
