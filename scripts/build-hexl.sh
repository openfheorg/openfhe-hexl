#!/bin/sh

. ./scripts/vars.sh

echo ===============================================================================
echo
echo Prebuilt HEXL library does not exist.  Building...
echo
echo ===============================================================================

if [ ! -d hexl ]; then
  git clone https://github.com/intel/hexl.git || exit 1
fi
cd hexl || exit 1

#git checkout v1.2.5 || exit 1
git checkout 2d196fdd71f24511bd7e0e23dc07d37c888f53e7 || exit 1
make clean
cmake -DCMAKE_INSTALL_PREFIX=../../../openfhe-staging -DHEXL_BENCHMARK=OFF -DHEXL_COVERAGE=OFF -DHEXL_DOCS=OFF -DHEXL_EXPERIMENTAL=OFF -DHEXL_SHARED_LIB=ON -DHEXL_TESTING=OFF . || exit 1
make -j || exit 1
make install
