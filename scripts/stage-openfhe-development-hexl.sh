#!/bin/sh


# TODO: test more recent version of hexl


OPENFHE_DEST_REPO="../../openfhe-staging/openfhe-development"
OPENFHE_HEXL_SOURCE_REPO="."

if [ ! -f /usr/local/lib/libhexl.so.1.2.5 ]; then
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
