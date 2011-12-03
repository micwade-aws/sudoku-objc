#!/bin/bash

x="../puzzles/25x25/p"
y="0"
z=".pz"
datDir="runStats/"
datExt=".dat"

LIMIT=625
POP=400
PMUT=0.04
PCO=0.8
for((i=0; i <= LIMIT; i++))
 do
	if [ $i -lt 10 ]
	then
	  ./a.out $x$y$i$z 16 16 4 4 768 $POP $PMUT $PCO $datDir$y$i$datExt 
	else
	  ./a.out $x$i$z 16 16 4 4 768 $POP $PMUT $PCO $datDir$i$datExt 
	fi
done
