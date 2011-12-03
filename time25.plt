#!/usr/local/bin/gnuplot -persist
set terminal x11

set yrange [0:15]

plot "test25.dat" using 1:18 with linespoints, \
	"../sudoku-astar/results25x25.dat" using 1:10 with linespoints, \
	x title 'linear'
	  #    EOF
