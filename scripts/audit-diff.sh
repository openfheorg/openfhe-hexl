#!/bin/sh

SOURCES=`find src -type f | grep -v dcrtpoly`

src/core/include/lattice/hal/hexl/lat-backend-hexl.h

for file in $SOURCES; do
  file_nohexl=`echo $file | sed 's/lattice\/hal\/hexl\/lat-backend-hexl/lattice\/hal\/default\/lat-backend-default/ ; s/-hexl//g ; s/hexl//g'`
  echo "FILE: " $file " -> " $file_nohexl
  kdiff3 $file ../openfhe-development/$file_nohexl
done
