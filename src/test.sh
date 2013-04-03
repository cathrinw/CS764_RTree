#!/bin/bash

y=0
maxIter=10
if [ $# -gt 2 ];
then
	let maxIter=$3
fi

#Careful: this will remove all files prefixed with the first argument.
rm -f $1*

while [ $y -lt $maxIter ];
do
echo "Running iteration $(($y+1)) of $maxIter for $1"
./rtreeExpRunner $2 5 >> $1.out.2
./rtreeExpRunner $2 5 half >> $1.out
./rtreeExpRunner $2 10 >> $1.out.2
./rtreeExpRunner $2 10 half >> $1.out
./rtreeExpRunner $2 21 >> $1.out.2
./rtreeExpRunner $2 21 half >> $1.out
./rtreeExpRunner $2 42 >> $1.out.2
./rtreeExpRunner $2 42 half >> $1.out
./rtreeExpRunner $2 85 >> $1.out.2
./rtreeExpRunner $2 85 half >> $1.out
if [ $2 -lt 2 ]
then
	./rtreeExpRunner $2 170 >> $1.out.2
	./rtreeExpRunner $2 170 half >> $1.out
	./rtreeExpRunner $2 42 half num >> $1Num.out
else
	let y+=10
fi
let y+=1
done
