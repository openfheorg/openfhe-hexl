#!/bin/sh

. ./scripts/functions.sh
. ./scripts/variants.sh
. ./scripts/benchmarks.sh

RUNDIR=`pwd`

echo "Extracting row names"
variant=`echo $VARIANTS | cut -d " " -f 1 | sed 's/,/-/g'`
cd "$RUNDIR/builds/$variant/openfhe-staging/openfhe-development/build/bin/benchmark"
for benchmark in $BENCHMARKS; do
  cat $benchmark.out | sed -e '1,/Iterations/d' | egrep -v "^-+" | \
     awk -- 'BEGIN {ORS="\t"} {print $1} END {ORS="\n"; print ""}' > $RUNDIR/$benchmark
done
cd $RUNDIR

echo "Extracting columnar data"
for variant in $VARIANTS; do
  variant=`echo $variant | sed 's/,/-/g'`
  for benchmark in $BENCHMARKS; do
    cat $RUNDIR/builds/$variant/openfhe-staging/openfhe-development/build/bin/benchmark/$benchmark.out | \
       sed -e '1,/Iterations/d' | egrep -v "^-+" | \
       awk -- 'BEGIN {ORS="\t"} { print $4 } END {ORS="\n"; print ""}' >> $RUNDIR/$benchmark
  done
done

echo "Rotating output"
for benchmark in $BENCHMARKS; do
  cat $benchmark | awk --         \
    ' BEGIN                       \
      {                           \
        s = 0;                    \
      }                           \
      {                           \
        if (s < NF) s = NF;       \
        for (i = 1; i <= NF; i++) \
          out[i] = out[i]"\t"$i;  \
      }                           \
      END                         \
      {                           \
        for (i = 1; i <= s; i++)  \
          print out[i];           \
      }'                              >> $benchmark.rot
  rm $benchmark
done

echo "Calculating speedups"
for benchmark in $BENCHMARKS; do
  echo $benchmark                              > $benchmark.tsv
  echo                                        >> $benchmark.tsv
  echo -n "CPU "                              >> $benchmark.tsv
  egrep "^Model name:" < lscpu.out | \
    sed 's/  \+/ /g'                          >> $benchmark.tsv
  echo -n "g++ version\t"                     >> $benchmark.tsv
  egrep "^g++" < g++-version.out              >> $benchmark.tsv
  echo -n "clang version\t"                   >> $benchmark.tsv
  grep "^clang version" < clang-version.out   >> $benchmark.tsv
  echo                                        >> $benchmark.tsv
  echo -n "BENCHMARK"                         >> $benchmark.tsv
  for variant in $VARIANTS; do
    variant=`echo $variant | sed 's/,/-/g'`
    echo -n "\t$variant\tspeedup"             >> $benchmark.tsv
  done
  echo                                        >> $benchmark.tsv
  cat $benchmark.rot | awk --                 \
    ' {                                       \
        printf $1;                            \
        for (i = 2; i <= NF; i++)             \
        {                                     \
          printf "\t%03f\t%03f", $i, ($2/$i)  \
        }                                     \
        print out;                            \
      }'                                      >> $benchmark.tsv
  rm $benchmark.rot
done

for benchmark in $BENCHMARKS; do
  sed -r 's/(\s+)?\S+//3' < $benchmark.tsv > $benchmark.tsv3
  mv $benchmark.tsv3 $benchmark.tsv
done

if [ -f $RUNDIR/utils/bench-color-html ]; then
  for benchmark in $BENCHMARKS; do
    ./utils/bench-color-html 0.1 < $benchmark.tsv > $benchmark.html
  done
fi
