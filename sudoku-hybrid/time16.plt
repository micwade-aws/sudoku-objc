#!/usr/local/bin/gnuplot -persist
set terminal x11

set yrange [0:5]

plot "test16.dat" using 1:18 with lines, \
	"../sudoku-astar/results16x16.dat" using 1:10 with lines, \
	x title 'linear'
	  #    EOF
