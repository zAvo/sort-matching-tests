#!/bin/bash

DEFAULT=correctness_default
LOWMEM=correctness_lowmem

DEFOUT=correctness_default.out
LOWOUT=correctness_lowmem.out

SIZE=10000
DIM=3

echo Removing previous files...
rm -f $DEFOUT $LOWOUT
echo done.
echo Checking correctness...

./$DEFAULT $SIZE $SIZE $DIM > $DEFOUT && ./$LOWMEM $SIZE $SIZE $DIM > $LOWOUT && DIFFCOUNT=$(diff $DEFOUT $LOWOUT | wc -c) && rm $DEFOUT $LOWOUT &&

if [ $DIFFCOUNT -eq 0 ]; then
	echo The solutions are identical.
	exit 0
else
	echo The solutions are different.
	exit 0
fi 

exit 1
