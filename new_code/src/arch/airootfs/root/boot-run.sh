# Create user
useradd -m -s /bin/bash liveuser

# Set password (LIVE SYSTEM ONLY)
echo "liveuser:live" | chpasswd

# Safe sudo configuration
echo "liveuser ALL=(ALL) NOPASSWD: ALL" > /etc/sudoers.d/liveuser
chmod 0440 /etc/sudoers.d/liveuser

# Locale
echo "export LANG=en_US.UTF-8" >> /home/liveuser/.bashrc

# Auto-start Hyprland on TTY1
cat << 'EOF' >> /home/liveuser/.bash_profile
if [[ -z "$DISPLAY" && "$(tty)" == "/dev/tty1" ]]; then
  exec Hyprland
fi
EOF

# Fix ownership
chown liveuser:liveuser /home/liveuser/.bashrc
chown liveuser:liveuser /home/liveuser/.bash_profile
