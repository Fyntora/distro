mkdir -p airootfs/root

cat > airootfs/root/.bash_profile << 'EOF'
#!/usr/bin/env bash

# Only auto-run on the first console
if [ "$(tty)" = "/dev/tty1" ]; then
    /root/install.sh
fi
EOF

chmod +x airootfs/root/.bash_profile
