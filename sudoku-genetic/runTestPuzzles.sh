#!/bin/bash

x="../puzzles/p"
y="0"
z=".pz"
datDir="runStats/"
datExt=".dat"

LIMIT=81
POP=200
PMUT=0.02
PCO=0.8
for((i=0; i <= LIMIT; i++))
 do
	if [ $i -lt 10 ]
	then
	#	j=`expr "$LIMIT" "-" "$i"`
		./a.out $x$y$i$z 9 9 3 3 243 $POP $PMUT $PCO $datDir$y$i$datExt 
	else
	#	j=`expr "$LIMIT" "-" "$i"`
		./a.out $x$i$z 9 9 3 3 243 $POP $PMUT $PCO $datDir$i$datExt 
	fi
done
