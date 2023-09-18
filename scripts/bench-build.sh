#!/bin/sh

. ./scripts/functions.sh
. ./scripts/vars.sh
. ./scripts/variants.sh

if [ -d builds ]; then
  echo "ERROR: builds directory already exists; cowardly refusing to continue."
  exit 1
fi

mkdir builds || abort "unable to create builds directory."
cd builds || abort "unable to enter builds directory."

ROOT=`pwd`

for variant in $VARIANTS; do
  variant_orig=$variant
  cd $ROOT

  # determine compiler
  gnu9=`echo $variant    | grep "GCC9"    | wc -l`
  gnu10=`echo $variant   | grep "GCC10"   | wc -l`
  gnu11=`echo $variant   | grep "GCC11"   | wc -l`
  clang9=`echo $variant | grep "CLANG9" | wc -l`
  clang10=`echo $variant | grep "CLANG10" | wc -l`
  clang11=`echo $variant | grep "CLANG11" | wc -l`
  clang12=`echo $variant | grep "CLANG12" | wc -l`
  clang13=`echo $variant | grep "CLANG13" | wc -l`
  clang14=`echo $variant | grep "CLANG14" | wc -l`
  clang15=`echo $variant | grep "CLANG15" | wc -l`
  clang16=`echo $variant | grep "CLANG16" | wc -l`
  clang17=`echo $variant | grep "CLANG17" | wc -l`
  if [ $gnu9 -eq 1 ]; then
    cc=/usr/bin/gcc-9
    cxx=/usr/bin/g++-9
  elif [ $gnu10 -eq 1 ]; then
    cc=/usr/bin/gcc-10
    cxx=/usr/bin/g++-10
  elif [ $gnu11 -eq 1 ]; then
    cc=/usr/bin/gcc-11
    cxx=/usr/bin/g++-11
  elif [ $clang9 -eq 1 ]; then
    cc=/usr/bin/clang-9
    cxx=/usr/bin/clang++-9
  elif [ $clang10 -eq 1 ]; then
    cc=/usr/bin/clang-10
    cxx=/usr/bin/clang++-10
  elif [ $clang11 -eq 1 ]; then
    cc=/usr/bin/clang-11
    cxx=/usr/bin/clang++-11
  elif [ $clang12 -eq 1 ]; then
    cc=/usr/bin/clang-12
    cxx=/usr/bin/clang++-12
  elif [ $clang13 -eq 1 ]; then
    cc=/usr/bin/clang-13
    cxx=/usr/bin/clang++-13
  elif [ $clang14 -eq 1 ]; then
    cc=/usr/bin/clang-14
    cxx=/usr/bin/clang++-14
  elif [ $clang15 -eq 1 ]; then
    cc=/usr/bin/clang-15
    cxx=/usr/bin/clang++-15
  elif [ $clang16 -eq 1 ]; then
    cc=/usr/bin/clang-16
    cxx=/usr/bin/clang++-16
  elif [ $clang17 -eq 1 ]; then
    cc=/usr/bin/clang-17
    cxx=/usr/bin/clang++-17
  else
    abort "unable to parse compiler options"
  fi

  # transform variant to proper cmake flags
  cmake_flags=`echo $variant | sed 's/^/WITH_/;                   \
                                    s/WITH_GCC[0-9]\+//;          \
                                    s/WITH_CLANG[0-9]\+//;        \
                                    s/,/ WITH_/g;                 \
                                    s/WITH_HEXL/WITH_INTEL_HEXL/; \
                                    s/-/=/g;                      \
                                    s/WITH/-DWITH/g'`

  # no commas in directory names
  variant_dir=`echo $variant_orig | sed 's/,/-/g'`

  separator
  echo "Cloning openfhe-configurator for variant $variant_orig"

  separator
  git clone https://github.com/openfheorg/openfhe-configurator $variant_dir
  cd $variant_dir || abort "clone of variant $variant_orig failed."
  git checkout $OPENFHE_CONFIGURATOR_BRANCH

  CC=$cc CXX=$cxx ./scripts/stage-openfhe-development-hexl.sh

  cmake_flags=$cmake_flags" -DINTEL_HEXL_HINT_DIR="$ROOT/$variant_dir"/openfhe-staging/install"
  echo "Preparing to build variant $variant_orig with CC=$cc CXX=$cxx CMAKE_FLAGS=$cmake_flags"

  CC=$cc CXX=$cxx CMAKE_FLAGS=$cmake_flags ./scripts/build-openfhe-development.sh || abort "build of variant $variant_orig failed."
done

separator
echo "bench-build.sh: done."
separator
