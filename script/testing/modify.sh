#!/bin/sh

i=5
cd root

while true
        do
        t=$(ls | wc -l)
        sleep $(expr $t % 10)
        w=`cat /proc/sys/kernel/random/entropy_avail`
        if [ $t -eq 0 ]
                then
                continue
        fi

        t=$(expr $t % $w / 2)

        if test $(expr $t % 2) -eq 0
                then
                cat /proc/sys/kernel/random/uuid | tee `ls | tail -n $t` >> /dev/null
        else
                cat /proc/sys/kernel/random/uuid | tee `ls | head -n $t` >> /dev/null
        fi

        done

