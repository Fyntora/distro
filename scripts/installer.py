#!/usr/bin/env python3
import subprocess
import os
import sys

ROOTFS_SOURCE = "/installer/rootfs"  
TARGET = "/mnt"

def run(cmd):
    subprocess.run(cmd, check=True)

def list_disks():
    run(["lsblk"])
    return input("Enter target disk (e.g. /dev/sda): ").strip()

def partition_disk(disk):
    run(["parted", "-s", disk, "mklabel", "gpt"])
    run(["parted", "-s", disk, "mkpart", "EFI", "fat32", "1MiB", "1GiB"])
    run(["parted", "-s", disk, "set", "1", "esp", "on"])
    run(["parted", "-s", disk, "mkpart", "root", "ext4", "1GiB", "100%"])
    return f"{disk}1", f"{disk}2"

def format_and_mount(efi, root):
    run(["mkfs.vfat", "-F32", efi])
    run(["mkfs.ext4", "-F", root])

    os.makedirs(TARGET, exist_ok=True)
    run(["mount", root, TARGET])
    os.makedirs(f"{TARGET}/boot/efi", exist_ok=True)
    run(["mount", efi, f"{TARGET}/boot/efi"])

def copy_rootfs():
    print("Copying root filesystem...")
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
        run(["mount", "--bind", f"/{d}", f"{TARGET}/{d}"])

def install_grub(disk):
    run(["chroot", TARGET, "grub-install",
         "--target=x86_64-efi",
         "--efi-directory=/boot/efi",
         "--bootloader-id=Fyntora",
         disk])
    run(["chroot", TARGET, "update-grub"])

def finalize():
    run(["chroot", TARGET, "update-initramfs", "-u"])
    run(["touch", f"{TARGET}/etc/fyntora_installed"])
    print("Installation complete. Reboot.")

def main():
    disk = list_disks()
    confirm = input(f"ERASE {disk}? (yes): ")
    if confirm != "yes":
        sys.exit(1)

    efi, root = partition_disk(disk)
    format_and_mount(efi, root)
    copy_rootfs()
    prepare_chroot()
    install_grub(disk)
    finalize()

if __name__ == "__main__":
    main()
