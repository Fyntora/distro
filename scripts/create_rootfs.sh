#!/bin/bash
# Create root filesystem script

set -e

ROOTFS_DIR="build/rootfs"

echo "Creating root filesystem in $ROOTFS_DIR for $TARGET_BASE"

# Clean and create directory
rm -rf "$ROOTFS_DIR"
mkdir -p "$ROOTFS_DIR"

if [ "$TARGET_BASE" = "ubuntu" ]; then
    # Install debootstrap if needed
    if ! command -v debootstrap >/dev/null; then
        sudo apt update && sudo apt install -y debootstrap
    fi
    # Map arch
    if [ "$ARCH" = "x86_64" ]; then
        ARCH_DEB="amd64"
    else
        ARCH_DEB="$ARCH"
    fi
    # Use debootstrap for Ubuntu
    debootstrap --no-check-gpg --arch="$ARCH_DEB" "$TARGET_VERSION" "$ROOTFS_DIR" http://archive.ubuntu.com/ubuntu/

    # Ensure boot directory exists
    mkdir -p "$ROOTFS_DIR"/boot

    # Mount and install extra packages
    mount -t proc proc "$ROOTFS_DIR/proc"
    mount -t sysfs sys "$ROOTFS_DIR/sys"
    mount --bind /dev "$ROOTFS_DIR/dev"
    mount --bind /dev/pts "$ROOTFS_DIR/dev/pts"

    chroot "$ROOTFS_DIR" apt update
    chroot "$ROOTFS_DIR" apt install -y initramfs-tools
    chroot "$ROOTFS_DIR" apt install -y parted
    chroot "$ROOTFS_DIR" apt install -y $PACKAGES
    chroot "$ROOTFS_DIR" /usr/sbin/mkinitramfs -o /boot/initrd.img 6.6.0
    # Disable Ubuntu welcome message
    chroot "$ROOTFS_DIR" chmod -x /etc/update-motd.d/* || true
    # Copy installer script
    cp scripts/installer.py "$ROOTFS_DIR/usr/local/bin/installer.py"
    chmod +x "$ROOTFS_DIR/usr/local/bin/installer.py"

    # Unmount
    umount "$ROOTFS_DIR/dev/pts"
    umount "$ROOTFS_DIR/dev"
    umount "$ROOTFS_DIR/sys"
    umount "$ROOTFS_DIR/proc"
elif [ "$TARGET_BASE" = "opensuse" ]; then
    # Create minimal rootfs structure
    mkdir -p "$ROOTFS_DIR"/{bin,boot,dev,etc,home,lib,lib64,mnt,opt,proc,root,sbin,srv,sys,tmp,usr,var}
    mkdir -p "$ROOTFS_DIR"/usr/{bin,lib,local,share}
    mkdir -p "$ROOTFS_DIR"/var/{log,run,spool}

    # Install base packages using zypper
    zypper --root "$ROOTFS_DIR" --gpg-auto-import-keys install -y $PACKAGES
else
    echo "Unsupported target base: $TARGET_BASE"
    exit 1
fi

echo "Root filesystem created"