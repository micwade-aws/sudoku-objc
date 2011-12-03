#!/bin/bash

x="../puzzles/p"
y="0"
z=".pz"
datDir="runStats/"
datExt=".dat"

LIMIT=81
for((i=0; i <= LIMIT; i++))
 do
	if [ $i -lt 10 ]
	then
	#	j=`expr "$LIMIT" "-" "$i"`
		./a.out $x$y$i$z 9 9 3 3 243 $datDir$y$i$datExt 
	else
	#	j=`expr "$LIMIT" "-" "$i"`
		./a.out $x$i$z 9 9 3 3 243 $datDir$i$datExt 
	fi
done
