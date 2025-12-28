#!/bin/bash
# Generate ISO script

set -e

ROOTFS_DIR="build/rootfs"
ISO_FILE="build/fyntora.iso"

echo "Generating ISO from $ROOTFS_DIR"

# Generate initramfs (simple version, assume dracut or mkinitrd)
# For simplicity, assume kernel and initrd are copied
# In real, use dracut --hostonly --kver $(uname -r) -f "$ROOTFS_DIR"/boot/initrd.img

# Create ISO using xorriso
xorriso -as mkisofs \
    -o "$ISO_FILE" \
    -b boot/grub/i386-pc/eltorito.img \
    -no-emul-boot \
    -boot-load-size 4 \
    -boot-info-table \
    --protective-msdos-label \
    "$ROOTFS_DIR"

echo "ISO generated: $ISO_FILE"