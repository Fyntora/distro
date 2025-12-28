#!/bin/bash
# Build kernel script

set -e

KERNEL_DIR="build/kernel"
KERNEL_SRC="linux-6.6.tar.xz"
KERNEL_URL="https://cdn.kernel.org/pub/linux/kernel/v6.x/linux-6.6.tar.xz"

echo "Building kernel 6.6.0"

# Install build dependencies if on Ubuntu
if [ "$HOST_OS" = "ubuntu" ]; then
    sudo apt update
    sudo apt install -y build-essential flex bison libssl-dev libelf-dev
fi

mkdir -p "$KERNEL_DIR"
cd "$KERNEL_DIR"

# Download kernel if not present
if [ ! -f "$KERNEL_SRC" ]; then
    wget "$KERNEL_URL"
    tar -xf "$KERNEL_SRC"
fi

cd linux-6.6

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
mkdir -p "$ROOTFS_DIR"/boot
# Manually install kernel files
cp arch/x86/boot/bzImage "$ROOTFS_DIR"/boot/vmlinuz-6.6.0
cp System.map "$ROOTFS_DIR"/boot/System.map-6.6.0
cp .config "$ROOTFS_DIR"/boot/config-6.6.0
make INSTALL_MOD_PATH="$ROOTFS_DIR" modules_install

# List boot files
echo "Boot files:"
ls -la "$ROOTFS_DIR"/boot/

cd ../..

echo "Kernel built and installed"