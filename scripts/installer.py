#!/usr/bin/env python3
import subprocess
import os
import sys

# Path inside ISO (read-only)
ROOTFS_SOURCE = "/installer/rootfs"
# Target mount point for installation
TARGET = "/mnt"

def run(cmd):
    print(f"> {' '.join(cmd)}")
    subprocess.run(cmd, check=True)

def list_disks():
    run(["lsblk"])
    return input("Enter target disk (e.g. /dev/sda): ").strip()

def partition_disk(disk):
    print(f"Partitioning {disk}...")
    run(["parted", "-s", disk, "mklabel", "gpt"])
    run(["parted", "-s", disk, "mkpart", "EFI", "fat32", "1MiB", "1GiB"])
    run(["parted", "-s", disk, "set", "1", "esp", "on"])
    run(["parted", "-s", disk, "mkpart", "root", "ext4", "1GiB", "100%"])
    run(["partprobe", disk])  # inform kernel
    return f"{disk}1", f"{disk}2"

def format_and_mount(efi, root):
    print("Formatting partitions...")
    run(["mkfs.vfat", "-F32", efi])
    run(["mkfs.ext4", "-F", root])

    os.makedirs(TARGET, exist_ok=True)
    run(["mount", root, TARGET])
    os.makedirs(f"{TARGET}/boot/efi", exist_ok=True)
    run(["mount", efi, f"{TARGET}/boot/efi"])

def write_fstab(root_dev, efi_dev):
    print("Creating /etc/fstab...")
    root_uuid = subprocess.check_output(["blkid", "-s", "UUID", "-o", "value", root_dev]).decode().strip()
    efi_uuid = subprocess.check_output(["blkid", "-s", "UUID", "-o", "value", efi_dev]).decode().strip()
    fstab_content = f"""# <file system> <mount point> <type> <options> <dump> <pass>
UUID={root_uuid} / ext4 defaults 0 1
UUID={efi_uuid} /boot/efi vfat defaults 0 2
"""
    os.makedirs(f"{TARGET}/etc", exist_ok=True)
    with open(f"{TARGET}/etc/fstab", "w") as f:
        f.write(fstab_content)
    return root_uuid

def copy_rootfs():
    print("Copying root filesystem...")
    if not os.path.exists(ROOTFS_SOURCE):
        raise RuntimeError(f"{ROOTFS_SOURCE} does not exist. Make sure ISO contains installer/rootfs")
    run([
        "rsync", "-aAX",
        "--exclude=/dev/*",
        "--exclude=/proc/*",
        "--exclude=/sys/*",
        "--exclude=/tmp/*",
        "--exclude=/run/*",
        ROOTFS_SOURCE + "/",
        TARGET + "/"
    ])

def prepare_chroot():
    for d in ("dev", "proc", "sys"):
        target_dir = f"{TARGET}/{d}"
        os.makedirs(target_dir, exist_ok=True)
        if not os.path.ismount(target_dir):
            run(["mount", "--bind", f"/{d}", target_dir])

def install_grub(disk, root_uuid):
    print("Installing GRUB...")
    # Ensure required packages inside chroot
    run(["chroot", TARGET, "apt-get", "update"])
    run(["chroot", TARGET, "apt-get", "install", "-y", "grub-efi-amd64", "shim-signed", "efibootmgr"])
    
    # Install GRUB to EFI
    run(["chroot", TARGET, "grub-install",
         "--target=x86_64-efi",
         "--efi-directory=/boot/efi",
         "--bootloader-id=Fyntora",
         disk])
    
    # Write grub.cfg with correct root UUID
    grub_cfg = f"""
set timeout=5
set default=0

menuentry "Fyntora Linux" {{
    linux /boot/vmlinuz root=UUID={root_uuid} ro quiet
    initrd /boot/initrd.img
}}
"""
    os.makedirs(f"{TARGET}/boot/grub", exist_ok=True)
    with open(f"{TARGET}/boot/grub/grub.cfg", "w") as f:
        f.write(grub_cfg)
    
    run(["chroot", TARGET, "update-grub"])

def finalize():
    print("Finalizing installation...")
    # Update initramfs
    run(["chroot", TARGET, "update-initramfs", "-u"])
    # Disable multipathd (optional)
    run(["chroot", TARGET, "systemctl", "disable", "multipathd"])
    # Touch installed marker
    run(["touch", f"{TARGET}/etc/fyntora_installed"])
    print("Installation complete. Reboot to boot from disk.")

def unmount_all():
    print("Unmounting all mounts...")
    for d in ("sys", "proc", "dev"):
        mount_point = f"{TARGET}/{d}"
        if os.path.ismount(mount_point):
            run(["umount", mount_point])
    if os.path.ismount(f"{TARGET}/boot/efi"):
        run(["umount", f"{TARGET}/boot/efi"])
    if os.path.ismount(TARGET):
        run(["umount", TARGET])

def main():
    disk = list_disks()
    confirm = input(f"WARNING: This will ERASE {disk}. Type 'yes' to continue: ")
    if confirm != "yes":
        print("Aborted.")
        sys.exit(1)

    try:
        efi, root = partition_disk(disk)
        format_and_mount(efi, root)
        root_uuid = write_fstab(root, efi)
        copy_rootfs()
        prepare_chroot()
        install_grub(disk, root_uuid)
        finalize()
    except Exception as e:
        print(f"Installation failed: {e}")
    finally:
        unmount_all()

if __name__ == "__main__":
    main()
