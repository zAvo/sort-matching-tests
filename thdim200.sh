#!/bin/bash

FILE=thdim
SIZE=100000
TIMES=100

echo Starting test $FILE:

for DIM in 1 2 3 4 5 6 7 8 9 10 11 12 13 14 15 16
do
	echo Testing $DIM dimensions with $SIZE updates and $SIZE subscriptions
	for ((i=1; i <= TIMES; i++))
	do
		echo -ne "\riteration ${i}...\033[0K"
		./$FILE $SIZE $SIZE $DIM
	done

	echo -e "\rdone.\033[0K"
done

echo Test $FILE completed.

exit 0
