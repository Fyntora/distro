#!/bin/bash
# Build kernel script

set -e

# Variables
KERNEL_VERSION="6.6.8"
THREADS=-j$(nproc)

make $THREADS

make modules