#!/bin/bash


for i in $(objdump -d $1 | grep "^ " | cut -f2)
do 
	echo -n "\x"$i 
done

