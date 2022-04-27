#!/bin/sh

SOURCES=`find src -type f`

for file in $SOURCES; do
  file_nohexl=`echo $file | sed 's/-hexl//g ; s/hexl//g'`
  kdiff3 $file ../openfhe-development/$file_nohexl
done
