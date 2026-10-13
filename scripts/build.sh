#!/usr/bin/env bash
set -e

echo "=== Building ManiMart (C++20 Drogon Marketplace) ==="

BUILD_DIR="build"
BUILD_TYPE="Release"

if [ "$1" == "debug" ]; then
    BUILD_TYPE="Debug"
fi

mkdir -p ${BUILD_DIR}
cd ${BUILD_DIR}

cmake -DCMAKE_BUILD_TYPE=${BUILD_TYPE} -DBUILD_TESTING=ON ..
cmake --build . --config ${BUILD_TYPE} -j$(nproc 2>/dev/null || echo 4)

echo "=== Build Successful! Binary located at ${BUILD_DIR}/manimart ==="
