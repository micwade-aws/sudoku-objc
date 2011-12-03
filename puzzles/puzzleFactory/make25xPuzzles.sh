#!/bin/bash

x="p"
y="0"
z=".pz"
datDir="runStats/"
datExt=".pz"

LIMIT=625
for((i=0; i <= LIMIT; i++))
 do
	if [ $i -lt 10 ]
	then
		./a.out $1 25 25 5 5 1875 $i > $2/$x$y$i$datExt
	else
		./a.out $1 25 25 5 5 1875 $i > $2/$x$i$datExt
	fi
done
