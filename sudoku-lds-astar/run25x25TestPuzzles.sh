#!/bin/bash

x="../puzzles/25x25/p"
y="0"
z=".pz"
datDir="runStats/25x25/"
datExt=".dat"

LIMIT=625
for((i=0; i <= LIMIT; i++))
 do
	if [ $i -lt 10 ]
	then
	#	j=`expr "$LIMIT" "-" "$i"`
		./a.out $x$y$i$z 25 25 5 5 1875  
	else
	#	j=`expr "$LIMIT" "-" "$i"`
		./a.out $x$i$z 25 25 5 5 1875 
	fi
done
