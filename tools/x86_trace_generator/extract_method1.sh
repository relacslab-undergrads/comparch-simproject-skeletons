#!/bin/bash

# ======================================================FIXME
PIN_ROOT=/hdd1/hynix-cxl/pin-3.13-98189-g60a6ef199-gcc-linux/
MAX_COUNT=10000000000
# ===========================================================

BENCH_ROOT=
BENCH_NAME=
EXTRACT_TOOL_ROOT=

# Directory of this script
DIR="$( cd -P "$( dirname "${BASH_SOURCE[0]}" )" && pwd )"
OUT_PATH=$DIR"/output"
mkdir -p $OUT_PATH

ARGC=$#
if [ $ARGC != 2 -a $ARGC != 3 ]; then
    echo "Arguments are required! [benchmark root path, workload name, extraction tool path]"
    exit
fi

BENCH_ROOT=$1
BENCH_NAME=$2
if [ $BENCH_NAME != "transformer" -a $BENCH_NAME != "bert-base" -a $BENCH_NAME != "bert-large" -a $BENCH_NAME != "t5-small" -a $BENCH_NAME != "t5-base" -a $BENCH_NAME != "gpt2" ]; then
    echo "Invalid benchmark"
    exit
fi

if [ $ARGC == 3 ]; then
    EXTRACT_TOOL_ROOT=$3
else
    EXTRACT_TOOL_ROOT=$DIR
fi

BENCH_BIN_PATH=$BENCH_ROOT'/build/TransformerNLP'

# Determine skip count (control knob)
SKIP_COUNT=0
if [ $BENCH_NAME == "transformer" ]; then
    SKIP_COUNT=1040608233509
elif [ $BENCH_NAME == "bert-base" ]; then
    SKIP_COUNT=2777724746924
elif [ $BENCH_NAME == "t5-small" ]; then
    SKIP_COUNT=5831074354680
elif [ $BENCH_NAME == "bert-large" ]; then
    SKIP_COUNT=6003200087828
elif [ $BENCH_NAME == "t5-base" ]; then
    SKIP_COUNT=10014529167514
elif [ $BENCH_NAME == "gpt2" ]; then
    SKIP_COUNT=5565731927237
fi 

cd $OUT_PATH

# Prepare necessary files
cp -R -u -p $BENCH_ROOT"/dictionary" $OUT_PATH
cp -R -u -p $BENCH_ROOT"/sentence" $OUT_PATH
cp -R -u -p $BENCH_ROOT"/params" $OUT_PATH

RUN_PATH=$OUT_PATH'/'$BENCH_NAME
cp -R -u -p $EXTRACT_TOOL_ROOT"/obj-intel64" $RUN_PATH
BENCH_CMD=$BENCH_BIN_PATH" -m "$BENCH_NAME

mkdir -p $RUN_PATH
cd $RUN_PATH

# Run extraction
echo "$PIN_ROOT/pin -t obj-intel64/trace_generator.so -thread 1 -manual 1 -skip $SKIP_COUNT -max $MAX_COUNT -- $BENCH_CMD"
$PIN_ROOT"/pin" -t obj-intel64/trace_generator.so -thread 1 -manual 1 -skip $SKIP_COUNT -max $MAX_COUNT -- $BENCH_CMD

