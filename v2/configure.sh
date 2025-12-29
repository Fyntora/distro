#!/bin/bash
# Build kernel script

set -e

# Variables
KERNEL_VERSION="6.6.8"
SYSTEM_SPECIFIC="true"

if [ "$SYSTEM_SPECIFIC" = "true" ]; then
    make defconfig
else
    echo "Running make menuconfig..."
    make menuconfig
fi