#!/bin/sh

echo ===============================================================================
echo
echo Prebuilt HEXL library does not exist.  Building...
echo
echo ===============================================================================

git clone https://github.com/intel/hexl.git || exit 1
cd hexl || exit 1
mkdir build || exit 1
cmake -DHEXL_BENCHMARK=OFF -DHEXL_COVERAGE=OFF -DHEXL_DOCS=OFF -DHEXL_EXPERIMENTAL=OFF -DHEXL_SHARED_LIB=ON -DHEXL_TESTING=OFF || exit 1
make -j || exit 1

echo ===============================================================================
echo
echo HEXL library built.  Requires sudo to install.
echo
echo ===============================================================================

sudo make install
