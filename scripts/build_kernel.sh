#!/bin/bash
# Build kernel script

set -e

KERNEL_DIR="build/kernel"
KERNEL_SRC="linux-$KERNEL_VERSION.tar.xz"
KERNEL_URL="https://cdn.kernel.org/pub/linux/kernel/v6.x/$KERNEL_SRC"

echo "Building kernel $KERNEL_VERSION"

mkdir -p "$KERNEL_DIR"
cd "$KERNEL_DIR"

# Download kernel if not present
if [ ! -f "$KERNEL_SRC" ]; then
    wget "$KERNEL_URL"
    tar -xf "$KERNEL_SRC"
fi

cd linux-$KERNEL_VERSION

# Copy config
if [ -f "../../../config/kernel.config" ]; then
    cp ../../../config/kernel.config .config
else
    make defconfig
fi

# Build
if [ "$ARCH" = "x86_64" ]; then
    make -j$(nproc)
elif [ "$ARCH" = "arm64" ]; then
    make ARCH=arm64 CROSS_COMPILE=aarch64-linux-gnu- -j$(nproc)
else
    echo "Unsupported architecture: $ARCH"
    exit 1
fi

# Install to rootfs
ROOTFS_DIR="../rootfs"
make INSTALL_PATH="$ROOTFS_DIR/boot" install
make INSTALL_MOD_PATH="$ROOTFS_DIR" modules_install

cd ../..

echo "Kernel built and installed"