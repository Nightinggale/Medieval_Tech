#!/bin/sh

for FILE in `ls *.cpp *.h *.hpp`
do
	echo "1" >>  ${FILE}
done
