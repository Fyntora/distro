#!/bin/bash
# Create root filesystem script

set -e

ROOTFS_DIR="build/rootfs"

echo "Creating root filesystem in $ROOTFS_DIR for $TARGET_BASE"

# Create directory
mkdir -p "$ROOTFS_DIR"

if [ "$TARGET_BASE" = "ubuntu" ]; then
    # Install debootstrap if needed
    if ! command -v debootstrap >/dev/null; then
        sudo apt update && sudo apt install -y debootstrap
    fi
    # Use debootstrap for Ubuntu
    debootstrap --arch="$ARCH" "$TARGET_VERSION" "$ROOTFS_DIR" http://archive.ubuntu.com/ubuntu/

    # Mount and install extra packages
    mount -t proc proc "$ROOTFS_DIR/proc"
    mount -t sysfs sys "$ROOTFS_DIR/sys"
    mount --bind /dev "$ROOTFS_DIR/dev"
    mount --bind /dev/pts "$ROOTFS_DIR/dev/pts"

    chroot "$ROOTFS_DIR" apt update
    chroot "$ROOTFS_DIR" apt install -y $PACKAGES

    # Unmount
    umount "$ROOTFS_DIR/dev/pts"
    umount "$ROOTFS_DIR/dev"
    umount "$ROOTFS_DIR/sys"
    umount "$ROOTFS_DIR/proc"
elif [ "$TARGET_BASE" = "opensuse" ]; then
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
else
    echo "Unsupported target base: $TARGET_BASE"
    exit 1
fi

echo "Root filesystem created"