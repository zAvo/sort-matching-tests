#!/bin/bash

TIMES=100

UPD=100000
SUB=100000
DIM=1

echo Starting tests on datatypes:

for ARG in datatype_int32 datatype_int64 datatype_float datatype_double
do
	echo Testing $ARG...
	for ((i=1; i <= TIMES; i++))
	do
		echo -ne "\riteration ${i}...\033[0K"
		./$ARG $UPD $SUB $DIM
	done

	echo -e "\rdone.\033[0K"
done

echo Tests on datatype completed.

exit 0
