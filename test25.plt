#!/usr/local/bin/gnuplot -persist
set terminal x11

set yrange [5000:10100]

plot \
	"../sudoku-astar/results25x25.dat" using 1:4 with linespoints, \
"test25.dat" using 1:12 with linespoints, \
	x title 'linear'
	  #    EOF
