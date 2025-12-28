#!/bin/bash
# Setup branding script

set -e

ROOTFS_DIR="build/rootfs"

echo "Setting up branding for $DISTRO_NAME"

# Set /etc/os-release
cat > "$ROOTFS_DIR/etc/os-release" << EOF
NAME="$DISTRO_NAME"
VERSION="$VERSION"
ID=fyntora
VERSION_ID="$VERSION"
PRETTY_NAME="$DISTRO_NAME $VERSION"
HOME_URL="https://fyntora.os"
EOF

# Set hostname
echo "$DISTRO_NAME" > "$ROOTFS_DIR/etc/hostname"

# Set hosts
cat > "$ROOTFS_DIR/etc/hosts" << EOF
127.0.0.1 localhost
127.0.1.1 $DISTRO_NAME

# The following lines are desirable for IPv6 capable hosts
::1     ip6-localhost ip6-loopback
fe00::0 ip6-localnet
ff00::0 ip6-mcastprefix
ff02::1 ip6-allnodes
ff02::2 ip6-allrouters
EOF

# Add default user
chroot "$ROOTFS_DIR" useradd -m -s /bin/bash ubuntu
echo 'ubuntu:ubuntu' | chroot "$ROOTFS_DIR" chpasswd

# Create neofetch config
mkdir -p "$ROOTFS_DIR/etc/neofetch"
LOGO=$(jq -r '.distro.logo[]' "$CONFIG_FILE" | tr '\n' '\\n')
cat > "$ROOTFS_DIR/etc/neofetch/config.conf" << EOF
# Neofetch config for $DISTRO_NAME
print_info() {
    info title
    info underline
    info "OS" distro
    info "Host" model
    info "Kernel" kernel
    info "Uptime" uptime
    info "Packages" packages
    info "Shell" shell
    info "Resolution" resolution
    info "DE" de
    info "WM" wm
    info "WM Theme" wm_theme
    info "Theme" theme
    info "Icons" icons
    info "Terminal" term
    info "Terminal Font" term_font
    info "CPU" cpu
    info "GPU" gpu
    info "Memory" memory
}
ascii_distro="$DISTRO_NAME"
ascii_logo=(
$LOGO
)
EOF

echo "Branding configured"