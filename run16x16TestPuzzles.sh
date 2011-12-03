#!/bin/bash

x="puzzles/16x16/p"
y="0"
z=".pz"
datDir="runStats/"
datExt=".dat"

LIMIT=256
POP=100
PMUT=0.01
PCO=0.8
for((i=0; i <= LIMIT; i++))
 do
	if [ $i -lt 10 ]
	then
		./a.out $x$y$i$z 16 16 4 4 768 $POP $PMUT $PCO 
	else
		./a.out $x$i$z 16 16 4 4 768 $POP $PMUT $PCO
	fi
done
