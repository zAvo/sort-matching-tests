#!/bin/bash

FILE=lowmem4
DIM=4
TIMES=100

echo Starting test $FILE:

for SIZE in 25000 50000 75000 100000 125000 150000 175000 200000 225000 250000
do
	echo $SIZE updates and $SIZE subscriptions: $TIMES iterations...
	for ((i=1; i <= TIMES; i++))
	do
		echo -ne "\riteration ${i}...\033[0K"
		./$FILE $SIZE $SIZE $DIM
	done

	echo -e "\rdone.\033[0K"
done

echo Test $FILE completed.

exit 0
