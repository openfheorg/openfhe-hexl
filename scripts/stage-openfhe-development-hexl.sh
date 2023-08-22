#!/bin/sh

. ./scripts/vars.sh

OPENFHE_DEST_REPO="../../openfhe-staging/openfhe-development"
INTEL_HEXL_INSTALL_DIR="../../openfhe-staging"

if [ ! -f $INTEL_HEXL_INSTALL_DIR/lib/libhexl.so.1.2.5 ]; then
  pwd
  ./scripts/build-hexl.sh
fi

ROOT=`pwd`
if [ ! -d $OPENFHE_DEST_REPO ]; then
  echo "ERROR: OpenFHE destination staging repo $OPENFHE_DEST_REPO does not exist."
  exit 1
fi

cp CMakeLists.txt $OPENFHE_DEST_REPO
cp -r src $OPENFHE_DEST_REPO
cp -r benchmark $OPENFHE_DEST_REPO
cp -r third-party $OPENFHE_DEST_REPO
