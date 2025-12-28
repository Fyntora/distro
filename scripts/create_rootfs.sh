#!/bin/bash
# Create root filesystem script

set -e

ROOTFS_DIR="build/rootfs"

echo "Creating root filesystem in $ROOTFS_DIR"

# Create directory
mkdir -p "$ROOTFS_DIR"

# Install base packages using zypper in chroot
mount -t proc proc "$ROOTFS_DIR/proc"
mount -t sysfs sys "$ROOTFS_DIR/sys"
mount --bind /dev "$ROOTFS_DIR/dev"
mount --bind /dev/pts "$ROOTFS_DIR/dev/pts"

chroot "$ROOTFS_DIR" zypper --gpg-auto-import-keys install -y $PACKAGES

# Unmount
umount "$ROOTFS_DIR/dev/pts"
umount "$ROOTFS_DIR/dev"
umount "$ROOTFS_DIR/sys"
umount "$ROOTFS_DIR/proc"

echo "Root filesystem created"