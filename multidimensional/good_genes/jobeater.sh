#!/usr/bin/env bash

if [ "$#" -ne 2 ];
    then echo "provide file name and number cores"
    exit
fi

cat $1 | tr '\n' '\0' | xargs -0 -P$2 -I %  sh -c %
