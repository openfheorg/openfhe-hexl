#!/bin/sh

. ./scripts/functions.sh
. ./scripts/variants.sh

if [ -d benchmarks ]; then
  echo "ERROR: benchmarks directory already exists; cowardly refusing to continue."
  exit 1
fi

mkdir benchmarks || abort "unable to create benchmarks directory."
cd benchmarks || abort "unable to enter benchmarks directory."

ROOT=`pwd`

for variant in $VARIANTS; do
  cd $ROOT
  CMAKE_FLAGS=`echo $variant | sed 's/^/WITH_/; s/WITH_DEFAULT// ; s/,/ WITH_/g ; s/WITH_HEXL/WITH_INTEL_HEXL/ ; s/-/=/g ; s/WITH/-DWITH/g'`
  variant=`echo $variant | sed 's/,/-/g'`
  git clone https://github.com/openfheorg/openfhe-configurator $variant
  cd $variant || abort "clone of variant $variant failed."
  ./scripts/stage-openfhe-development-hexl.sh
  HEXL=`echo $variant | grep "HEXL-ON" | wc -l`
  if [ $HEXL -ne 0 ]; then
    separator
    echo "bench-build.sh: $variant / $CMAKE_FLAGS is a HEXL build"
    separator
    ./scripts/build-openfhe-development-hexl.sh || abort "build of variant $variant failed."
  else
    separator
    echo "bench-build.sh: $variant / $CMAKE_FLAGS is NOT a HEXL build"
    separator
    ./scripts/build-openfhe-development.sh || abort "build of variant $variant failed."
  fi
done

separator
echo "bench-build.sh: done."
separator
