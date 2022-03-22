#!/bin/sh

# this does a copy/rename of all HEXL related files from the openfhe-development repo
# into the openfhe-hexl repo

OPENFHE_REPO="../openfhe-development"

CPUS=`lscpu | egrep "^CPU\(s\)" | awk '{print $2}'`

cd $OPENFHE_REPO
mkdir build
cd build
make clean
cmake -DWITH_INTEL_HEXL=ON ..
make -j$CPUS
