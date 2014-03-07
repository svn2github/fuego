#!/bin/bash
set -eu

if [[ $OSTYPE == darwin* ]]; then
#mac
COLLECTION=/Users/mmueller/svns/sgf/pro-19x19/badukmovies-pro-collection
else
#unix
COLLECTION=/cshome/mmueller/projects/sgf/pro-19x19/badukmovies-pro-collection
fi

for YEARS in "$@"
do
echo training $YEARS in
pwd
for YEAR in `echo $COLLECTION/$YEARS/`; do
echo $YEAR
for GAMEDIR in `ls $YEAR/`; do
echo $GAMEDIR
gogui-statistics -program $FUEGO -commands "features_wistuba_file" -size 19 \
-quiet -force $YEAR/$GAMEDIR/*sgf
done
done
done

