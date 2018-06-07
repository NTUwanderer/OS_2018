#!/bin/bash

function random_range()
{
    if [ "$#" -lt "2" ]; then 
        echo "Usage: random_range <low> <high>"
        return
    fi
    low=$1
    range=$(($2 - $1))
    echo -e "$(($low+$RANDOM % $range)), \c"
} 


for (( i=0; i<=1024;i=i+1 ));
do
	random_range 0 128*32
done

for (( i=128*32; i<=128*40;i=i+1 ));
do
	echo -e "$i, \c";
done


for (( i=0; i<=128*40;i=i+1 ));
do
	random_range 128*40+1 128*256
done
