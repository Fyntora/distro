#!/bin/bash
# Generate ISO script

set -e

ROOTFS_DIR="build/rootfs"
ISO_FILE="build/fyntora.iso"

echo "Generating ISO from $ROOTFS_DIR"

# Check boot files
echo "Boot files in rootfs:"
ls -la "$ROOTFS_DIR"/boot/

# Generate initramfs using mkinitramfs
if chroot "$ROOTFS_DIR" which mkinitramfs >/dev/null 2>&1; then
    chroot "$ROOTFS_DIR" mkinitramfs -o /boot/initrd.img "$KERNEL_VERSION"
else
    echo "mkinitramfs not found in rootfs, skipping initrd"
fi

# Create bootable ISO using grub-mkrescue
grub-mkrescue -o "$ISO_FILE" "$ROOTFS_DIR"

echo "ISO generated: $ISO_FILE"