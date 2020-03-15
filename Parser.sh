#!/bin/bash

if [ "$#" -ne 1 ]; then
    echo "Illegal number of parameters"
    echo "<inputFileName>"
    exit
fi

inDir=$1

#first remove the comma and space between town (county) and state
#second remove spaces between state and country, and in country names, etc
#third remove quotation marks

cat $inDir | sed "s@,\ @_@g" | sed "s@\ @_@g" | sed "s@\"@@g" > 'Parsed_'$inDir
