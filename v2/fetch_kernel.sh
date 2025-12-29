#!/bin/bash
# Build kernel script
# TODO

set -e

KERNEL_VERSION="6.6.8"
KERNEL_URL="https://cdn.kernel.org/pub/linux/kernel/v6.x/linux-$KERNEL_VERSION.tar.xz"

# Download kernel if it doesn't exist
if [ ! -f "linux-$KERNEL_VERSION.tar.xz" ]; then
    wget "$KERNEL_URL"
fi

# Extract and enter directory
tar -xvf "linux-$KERNEL_VERSION.tar.xz"
cd "linux-$KERNEL_VERSION"
