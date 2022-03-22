#!/bin/sh

# this does a copy/rename of all HEXL related files from the openfhe-development repo
# into the openfhe-hexl repo

OPENFHE_SOURCE_REPO="../openfhe-development"
OPENFHE_HEXL_DEST_REPO="."

#DIR="benchmark/src"
#FILES="lib-hexl-benchmark.cpp lib-hexl-util.h poly-hexl-benchmark-16k.cpp poly-hexl-benchmark-4k.cpp"
#mkdir -p $OPENFHE_HEXL_DEST_REPO/$DIR                
#for file in $FILES; do
#  cp $OPENFHE_SOURCE_REPO/$DIR/$file $OPENFHE_HEXL_DEST_REPO/$DIR
#  rm $OPENFHE_SOURCE_REPO/$DIR/$file
#done

DIR="src/core/include/lattice/hal/hexl"
FILES="hexldcrtpoly.h lat-backend-hexl.h"
mkdir -p $OPENFHE_HEXL_DEST_REPO/$DIR                
for file in $FILES; do
  cp $OPENFHE_SOURCE_REPO/$DIR/$file $OPENFHE_HEXL_DEST_REPO/$DIR
  rm $OPENFHE_SOURCE_REPO/$DIR/$file
done

DIR="src/core/lib/lattice/hal/hexl"
FILES="hexldcrtpoly.cpp"
mkdir -p $OPENFHE_HEXL_DEST_REPO/$DIR                
for file in $FILES; do
  cp $OPENFHE_SOURCE_REPO/$DIR/$file $OPENFHE_HEXL_DEST_REPO/$DIR
  rm $OPENFHE_SOURCE_REPO/$DIR/$file
done

DIR="src/core/include/math/hal/intnat-hexl"
FILES="backendnathexl.h mubintvecnathexl.h transformnathexl-impl.h transformnathexl.h ubintnathexl.h"
mkdir -p $OPENFHE_HEXL_DEST_REPO/$DIR                
for file in $FILES; do
  cp $OPENFHE_SOURCE_REPO/$DIR/$file $OPENFHE_HEXL_DEST_REPO/$DIR
  rm $OPENFHE_SOURCE_REPO/$DIR/$file
done

DIR="src/core/lib/math/hal/intnat-hexl"
FILES="benativehexl-math-impl.cpp mubintvecnathexl.cpp ubintnathexl.cpp"
mkdir -p $OPENFHE_HEXL_DEST_REPO/$DIR                
for file in $FILES; do
  cp $OPENFHE_SOURCE_REPO/$DIR/$file $OPENFHE_HEXL_DEST_REPO/$DIR
  rm $OPENFHE_SOURCE_REPO/$DIR/$file
done
