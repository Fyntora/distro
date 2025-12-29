# #!/bin/bash
# # Generate ISO script

# set -e

# ROOTFS_DIR="build/rootfs"
# ISO_FILE="build/fyntora.iso"

# echo "Generating ISO from $ROOTFS_DIR"

# # Check boot files
# echo "Boot files in rootfs:"
# ls -la "$ROOTFS_DIR"/boot/

# # Generate initramfs using mkinitramfs
# chroot "$ROOTFS_DIR" /usr/sbin/mkinitramfs -o /boot/initrd.img "$KERNEL_VERSION" 2>/dev/null || echo "mkinitramfs failed, skipping initrd"

# # Create bootable ISO using grub-mkrescue
# grub-mkrescue -o "$ISO_FILE" "$ROOTFS_DIR"

# echo "ISO generated: $ISO_FILE"
#############################

#!/bin/bash
set -e

ROOTFS_DIR="build/rootfs"
ISO_DIR="build/iso"
ISO_FILE="build/fyntora.iso"

KERNEL="/boot/vmlinuz-$(uname -r)"
INITRD="/boot/initrd.img-$(uname -r)"

echo "Generating ISO..."

# Clean ISO dir
rm -rf "$ISO_DIR"
mkdir -p "$ISO_DIR"/{boot/grub,installer}

# Copy live kernel & initrd
cp "$KERNEL" "$ISO_DIR/boot/vmlinuz"
cp "$INITRD" "$ISO_DIR/boot/initrd.img"

# Copy installer payload
cp -a "$ROOTFS_DIR" "$ISO_DIR/installer/rootfs"

# GRUB config
cat > "$ISO_DIR/boot/grub/grub.cfg" << 'EOF'
set timeout=5
set default=0

menuentry "Install Fyntora Linux" {
    linux /boot/vmlinuz quiet
    initrd /boot/initrd.img
}
EOF

# Build ISO
grub-mkrescue -o "$ISO_FILE" "$ISO_DIR"

echo "ISO generated: $ISO_FILE"
