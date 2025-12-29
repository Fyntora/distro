#!/bin/bash
# Build kernel script

set -e

# Variables
KERNEL_VERSION="6.6.8"

if [ ! -f "linux-$KERNEL_VERSION" ]; then
    ./fetch_kernel.sh
fi

cd "linux-$KERNEL_VERSION"

../configure.sh
../build.sh