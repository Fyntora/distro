#!/bin/bash
# Generate ISO script

set -e

ROOTFS_DIR="build/rootfs"
ISO_FILE="build/fyntora.iso"

echo "Generating ISO from $ROOTFS_DIR"

# Check boot files
echo "Boot files in rootfs:"
ls -la "$ROOTFS_DIR"/boot/

# Generate initramfs using dracut
if chroot "$ROOTFS_DIR" which dracut >/dev/null 2>&1; then
    chroot "$ROOTFS_DIR" dracut --kver "$KERNEL_VERSION" /boot/initrd.img
else
    echo "Dracut not found in rootfs, skipping initrd"
fi

# Create bootable ISO using grub-mkrescue
grub-mkrescue -o "$ISO_FILE" "$ROOTFS_DIR"

echo "ISO generated: $ISO_FILE"