#!/bin/bash
# Setup bootloader script

set -e

ROOTFS_DIR="build/rootfs"

echo "Setting up bootloader in $ROOTFS_DIR"

# Create grub directory
mkdir -p "$ROOTFS_DIR"/boot/grub

# Grub will be handled by grub-mkrescue

# Write grub.cfg
cat > "$ROOTFS_DIR"/boot/grub/grub.cfg << EOF
set default=0
set timeout=5

menuentry "Fyntora Linux" {
    linux /boot/vmlinuz-6.6.0 root=/dev/sda1
    initrd /boot/initrd.img
}
EOF

echo "Bootloader configured"