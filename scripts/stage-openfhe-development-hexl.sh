#!/bin/sh

. ./scripts/vars.sh

if [ "x$OPENFHE_INSTALL_DIR" = "x" ]; then
  OPENFHE_INSTALL_DIR=../../openfhe-staging/install
fi

if [ ! -f $OPENFHE_INSTALL_DIR/lib/libhexl.so.1.2.5 ]; then
  pwd
  ./scripts/build-hexl.sh
fi

OPENFHE_DEST_REPO="../../openfhe-staging/openfhe-development"
if [ ! -d $OPENFHE_DEST_REPO ]; then
  echo "ERROR: OpenFHE destination staging repo $OPENFHE_DEST_REPO does not exist."
  exit 1
fi

cp CMakeLists.txt $OPENFHE_DEST_REPO
cp OpenFHEConfig.cmake.in $OPENFHE_DEST_REPO
cp -r benchmark $OPENFHE_DEST_REPO
cp -r src $OPENFHE_DEST_REPO
cp -r third-party $OPENFHE_DEST_REPO
