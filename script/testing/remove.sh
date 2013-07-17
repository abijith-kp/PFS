#!/bin/sh

i=5
cd root

while [ 1 == 1 ]
        do
        j=$(expr `cat /proc/sys/kernel/random/entropy_avail` % $i)
        
        sleep $j
        j=`expr $j + 200`

        t=`ls | tail -n $j`
        
        if [ -z "$t" ]
                then
                continue
        fi
        
        rm $t
        
        done
