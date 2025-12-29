#!/bin/bash
# Build kernel script
# TODO

set -e

KERNEL_VERSION="6.6.8"
KERNEL_URL="https://cdn.kernel.org/pub/linux/kernel/v6.x/linux-$KERNEL_VERSION.tar.xz"
DIRROOT="build"
DIR="build/kernel"

# Download kernel if it doesn't exist
if [ ! -f "$DIR/linux-$KERNEL_VERSION.tar.xz" ]; then
    mkdir -p $DIRROOT
    mkdir -p $DIR
    wget "$KERNEL_URL"
    mv  "linux-$KERNEL_VERSION.tar.xz" $DIR
    tar -xvf "$DIR/linux-$KERNEL_VERSION.tar.xz"
    mv  "linux-$KERNEL_VERSION" $DIR
fi

# Extract and enter directory

# cd "linux-$KERNEL_VERSION"
