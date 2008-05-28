#!/bin/bash
#-----------------------------------------------------------------------------
# Script to identify correct Boost library extensions to use.
#-----------------------------------------------------------------------------

ALL_LIBRARY_PATHS=/usr/local/lib:/usr/lib:$LIBRARY_PATH:$LD_LIBRARY_PATH
SEARCH_PATHS=`echo $ALL_LIBRARY_PATHS | tr ':' '\n'`
EXTENSIONS="-mt -gcc -gcc-mt"
POSTFIX="-mt" # Default extension
FOUND=False
for LIB_PATH in $SEARCH_PATHS
do
    for EXT in "" $EXTENSIONS
    do
        if [[ -e $LIB_PATH/libboost_thread$EXT.a || -e $LIB_PATH/libboost_thread$EXT.so ]]
        then
            if [[ $FOUND == "True" ]]
            then
                echo "Warning: Multiple Boost postfixes found" >&2
            fi
            POSTFIX=$EXT
            FOUND=True
        fi
    done
done

if [[ $FOUND == "False" ]]
then
    echo "Warning: No Boost postfixes found" >&2
fi

echo $POSTFIX
