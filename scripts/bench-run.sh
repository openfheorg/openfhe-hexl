#!/bin/sh

. ./scripts/functions.sh
. ./scripts/variants.sh
. ./scripts/benchmarks.sh

RUNDIR=`pwd`

export OMP_NUM_THREADS=1

for variant in $VARIANTS; do
  variant=`echo $variant | sed 's/,/-/g'`
  for benchmark in $BENCHMARKS; do
    separator
    echo -n "Running   $variant $benchmark @ "
    date
    cd $RUNDIR/builds/$variant/openfhe-staging/openfhe-development/build/bin/benchmark
    LD_LIBRARY_PATH=$RUNDIR/builds/$variant/openfhe-staging/openfhe-development/build/lib ./$benchmark > $benchmark.out 2>&1
    echo -n "Completed $variant $benchmark @ "
    date
    separator
    cd $RUNDIR
  done
done

separator
echo "bench-run.sh: done."
separator
