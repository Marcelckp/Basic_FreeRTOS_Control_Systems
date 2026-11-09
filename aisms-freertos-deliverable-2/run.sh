#!/usr/bin/env bash
# run.sh - build the image, run the simulation (no network, no ports), save output files
set -euo pipefail
cd "$(dirname "$0")"

SECONDS_TO_RUN=${1:-5}

docker build -t sitsc-freertos .
# --network none: the program needs no network; stdout is captured to sample-output.txt; Timeouts stops the forever loop (exit code 137 is expected)
docker run --rm --network none sitsc-freertos timeout -s KILL ${SECONDS_TO_RUN} /src/app | tee sample-output.txt