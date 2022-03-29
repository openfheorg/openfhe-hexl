#!/bin/sh

OPENFHE_DEST_REPO="../../openfhe-staging/openfhe-development"
OPENFHE_HEXL_SOURCE_REPO="."

ROOT=`pwd`
if [ ! -d $OPENFHE_DEST_REPO ]; then
  echo "ERROR: OpenFHE destination staging repo $OPENFHE_DEST_REPO does not exist."
  exit 1
fi

cp -r src $OPENFHE_DEST_REPO
cp -r benchmark $OPENFHE_DEST_REPO
