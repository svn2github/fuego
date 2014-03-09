#!/bin/bash

run() {
mkdir -p $DIR
cd $DIR
../train.sh $YEARS&
#../demo.sh $YEARS&
cd ..
}

DIR=1
YEARS="1[0-8]* 19[0-5]*"
run
DIR=2
YEARS="19[6-7]* 198[0-3]"
run
DIR=3
YEARS="198[4-9] 1990"
run
DIR=4
YEARS="199[1-6]"
run
DIR=5
YEARS="199[7-9] 2000"
run
DIR=6
YEARS="200[1-3]"
run
DIR=7
YEARS="200[4-8]"
run
DIR=8
YEARS="2009 201[0-1]"
run

# start commented out
if [ 1 -eq 0 ]; then

# TODO: create 2012.validate file.
mkdir -p 9
cd 9
../validate.sh 2012&
cd ..

10-1959
1960-1983
1984-1990
1991-1996
1997-2000
2001-2003
2004-2008
2009-2011
fi
# end commented out

