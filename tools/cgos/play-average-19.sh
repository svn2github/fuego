#!/bin/bash

# Script for playing the AverageLib simple player on 19x19 CGOS

AVERAGELIB="../../fuegotest/fuego_test -player average"
NAME=AverageLib

echo "Enter CGOS password for $NAME:"
read PASSWORD

# Append 2>/dev/stderr to invocation, otherwise cgos3.tcl will not pass
# through stderr of the Go program
./cgos3-19.tcl "$NAME" "$PASSWORD" \
  "$AVERAGELIB 2>/dev/stderr" \
  gracefully_exit_server-average-19
