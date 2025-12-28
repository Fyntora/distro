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