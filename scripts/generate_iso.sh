#!/bin/bash
# Generate ISO script

set -e

ROOTFS_DIR="build/rootfs"
ISO_FILE="build/fyntora.iso"

echo "Generating ISO from $ROOTFS_DIR"

# Generate initramfs using dracut
if command -v dracut >/dev/null; then
    chroot "$ROOTFS_DIR" dracut --kver "$KERNEL_VERSION" /boot/initrd.img
else
    # Fallback, create simple initrd
    echo "Dracut not found, skipping initrd"
fi

# Create bootable ISO using grub-mkrescue
grub-mkrescue -o "$ISO_FILE" "$ROOTFS_DIR"

echo "ISO generated: $ISO_FILE"