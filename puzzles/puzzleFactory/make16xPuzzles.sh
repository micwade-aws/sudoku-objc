#!/bin/bash

x="p"
y="0"
z=".pz"
datDir="runStats/"
datExt=".pz"

LIMIT=256
for((i=0; i <= LIMIT; i++))
 do
	if [ $i -lt 10 ]
	then
		./a.out $1 $2 $3 $4 $5 $6 $i > $7/$x$y$i$datExt
	else
		./a.out $1 $2 $3 $4 $5 $6 $i > $7/$x$i$datExt
	fi
done
