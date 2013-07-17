#!/bin/sh

root="/home/abijith/Project/PFS"

#temparary file created
if [ -f $root/config/tempSum ]
        then
        rm $root/config/tempSum
fi

touch $root/config/tempSum

if [ ! -z $1 ]
        then
        # zero arguement
        # for checkFiles function
        for i in `ls $root/root/`
                do
                md5sum $root/root/$i >> $root/config/tempSum
        done
else
        # one dummy arguement
        # for initialize function
        # this if clause is only for the first run
        if [ -f $root/config/checkSum ]
                then
                rm $root/config/checkSum
        fi
        touch $root/config/checkSum
        for i in `ls $root/root/`
                do
                md5sum $root/root/$i >> $root/config/checkSum
        done
fi
exit
