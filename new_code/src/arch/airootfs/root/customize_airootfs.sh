#!/bin/bash

set -euxo pipefail

# Create user
useradd -m -G wheel -s /bin/bash live

# Set no password for the user
passwd -d live

# Create config directories
mkdir -p /home/live/.config/hypr
mkdir -p /home/live/.config/waybar

# Move config files
mv /root/hyprland.conf /home/live/.config/hypr/hyprland.conf
mv /root/waybar.config /home/live/.config/waybar/config
mv /root/bash_profile /home/live/.bash_profile

# Set ownership
chown -R live:live /home/live