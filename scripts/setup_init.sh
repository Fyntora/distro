#!/bin/bash
# Setup init system script

set -e

ROOTFS_DIR="build/rootfs"

echo "Setting up $INIT_SYSTEM in $ROOTFS_DIR"

if [ "$INIT_SYSTEM" = "systemd" ]; then
    # Enable basic services
    chroot "$ROOTFS_DIR" systemctl enable systemd-networkd systemd-resolved
    # Create default network
    cat > "$ROOTFS_DIR/etc/systemd/network/20-wired.network" << EOF
[Match]
Name=en*

[Network]
DHCP=yes
EOF
fi

echo "Init system configured"