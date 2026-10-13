#!/usr/bin/env bash
set -e

echo "=== Running ManiMart Test Suite ==="

BUILD_DIR="build"

if [ ! -d "${BUILD_DIR}" ]; then
    ./scripts/build.sh debug
fi

cd ${BUILD_DIR}
ctest --output-on-failure
