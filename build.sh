#!/usr/bin/env bash

PROJECT="example"

CPU_NUM=`grep -n processor /proc/cpuinfo|wc -l`
# SHELL_FOLDER=$(dirname "$0")
SHELL_FOLDER=$(dirname $(readlink -f "$0"))

WORK_FOLDER=`pwd`

echo "SHELL_FOLDER:${SHELL_FOLDER}"
echo "WORK_FOLDER:${WORK_FOLDER}"

BUILD_FOLDER="${PROJECT}_build"

if [ ${SHELL_FOLDER} == ${WORK_FOLDER} ];then
    BUILD_FOLDER="build"
fi
echo "BUILD_FOLDER:${BUILD_FOLDER}"

mkdir -p "$BUILD_FOLDER"
cd "$BUILD_FOLDER"

cmake $SHELL_FOLDER -DCMAKE_PREFIX_PATH="/usr/local/libzmq;/usr/local/zeromq;/usr/local/cppzmq;/usr/local/gflags;/usr/local/protobuf;/usr/local/libgonet"
make -j${CPU_NUM}

