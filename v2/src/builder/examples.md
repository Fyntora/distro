Usage Examples:
# Build everything (kernel + rootfs + ISO)
./linux_builder --version 6.6.8 --rootfs busybox --iso mydistro.iso
# Build kernel only
./linux_builder --kernel-only --version 6.6.8
# Build rootfs only  
./linux_builder --rootfs-only --rootfs ubuntu --rootfs-dir ./myrootfs
# Create ISO from existing kernel/rootfs
./linux_builder --iso-only --iso mydistro.iso --version 6.6.8 --rootfs-dir ./myrootfs
# Full custom build
./linux_builder --version 6.6.8 --url "https://custom.kernel.org/linux-6.6.8.tar.xz" \
                --rootfs ubuntu --arch x86_64 --rootfs-dir ./ubuntu_rootfs \
                --iso ubuntu-6.6.8.iso --label "Ubuntu Custom"