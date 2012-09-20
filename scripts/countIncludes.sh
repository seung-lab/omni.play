#!/bin/bash

for t in `find $1 -iname "*.h*"`
do
	grep "#include" $t | wc -l | xargs echo -n
	echo -e "\t" $t
done | sort -n
