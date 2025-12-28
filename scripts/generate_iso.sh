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
chroot "$ROOTFS_DIR" /usr/sbin/mkinitramfs -o /boot/initrd.img "$KERNEL_VERSION" 2>/dev/null || echo "mkinitramfs failed, skipping initrd"

# Create bootable ISO using grub-mkrescue
grub-mkrescue -o "$ISO_FILE" "$ROOTFS_DIR"

echo "ISO generated: $ISO_FILE"