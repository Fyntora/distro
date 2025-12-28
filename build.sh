#!/bin/bash
# Fyntora Linux Distro Build Suite
# Main build script

set -e

echo "Starting Fyntora distro build..."

# Configuration
CONFIG_FILE="config/distro.json"
if [ ! -f "$CONFIG_FILE" ]; then
    echo "Error: Config file $CONFIG_FILE not found"
    exit 1
fi

# Parse config using jq
echo "Parsing configuration..."
DISTRO_NAME=$(jq -r '.distro.name' "$CONFIG_FILE")
VERSION=$(jq -r '.distro.version' "$CONFIG_FILE")
ARCH=$(jq -r '.distro.arch' "$CONFIG_FILE")
HOST_OS=$(jq -r '.host_os' "$CONFIG_FILE")
TARGET_BASE=$(jq -r '.target_base' "$CONFIG_FILE")
PACKAGES=$(jq -r '.packages.base[]' "$CONFIG_FILE" | tr '\n' ' ')
KERNEL_VERSION=$(jq -r '.kernel.version' "$CONFIG_FILE")
INIT_SYSTEM=$(jq -r '.init' "$CONFIG_FILE")
BOOTLOADER=$(jq -r '.bootloader' "$CONFIG_FILE")
OUTPUT_ISO=$(jq -r '.output' "$CONFIG_FILE")

export DISTRO_NAME VERSION ARCH HOST_OS TARGET_BASE PACKAGES KERNEL_VERSION INIT_SYSTEM BOOTLOADER OUTPUT_ISO

# Create root filesystem (with cache)
if [ ! -d "build/cache/rootfs" ]; then
    echo "Creating root filesystem..."
    bash scripts/create_rootfs.sh
    mkdir -p build/cache/rootfs
else
    echo "Using cached rootfs"
fi

# Build kernel (with cache)
if [ ! -d "build/cache/kernel" ] || [ "build/cache/kernel/version" != "$KERNEL_VERSION" ]; then
    echo "Building kernel..."
    bash scripts/build_kernel.sh
    mkdir -p build/cache/kernel
    echo "$KERNEL_VERSION" > build/cache/kernel/version
else
    echo "Using cached kernel"
fi

# Setup init system
echo "Setting up init system..."
bash scripts/setup_init.sh

# Configure bootloader
echo "Configuring bootloader..."
bash scripts/setup_bootloader.sh

# Generate initramfs and ISO
echo "Generating ISO image..."
bash scripts/generate_iso.sh

# Run tests
echo "Running tests..."
bash scripts/test_build.sh

echo "Build completed successfully!"