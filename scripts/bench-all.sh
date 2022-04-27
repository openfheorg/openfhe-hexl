#!/bin/sh

. ./scripts/functions.sh

./scripts/bench-build.sh   || abort "bench-build.sh failed."
./scripts/bench-run.sh     || abort "bench-run.sh failed."
./scripts/bench-to-tsv.sh  || abort "bench-to-tsv.sh failed."
