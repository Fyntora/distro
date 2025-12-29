#!/bin/bash
# Setup init system script

set -e

ROOTFS_DIR="build/rootfs"

echo "Setting up $INIT_SYSTEM in $ROOTFS_DIR"

if [ "$INIT_SYSTEM" = "systemd" ]; then
    # Enable basic services
    chroot "$ROOTFS_DIR" systemctl enable systemd-networkd systemd-resolved getty@tty1.service
    # Create default network
    cat > "$ROOTFS_DIR/etc/systemd/network/20-wired.network" << EOF
[Match]
Name=en*

[Network]
DHCP=yes
EOF
    # Configure auto-login as root
    mkdir -p "$ROOTFS_DIR/etc/systemd/system/getty@tty1.service.d"
    cat > "$ROOTFS_DIR/etc/systemd/system/getty@tty1.service.d/override.conf" << EOF
[Service]
ExecStart=
ExecStart=-/sbin/agetty --autologin root --noclear %I \$TERM
EOF
fi

echo "Init system configured"