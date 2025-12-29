#!/usr/bin/env python3
import subprocess
import os
import sys

def run_cmd(cmd, shell=False):
    result = subprocess.run(cmd, shell=shell, capture_output=True, text=True)
    if result.returncode != 0:
        print(f"Error: {result.stderr}")
        raise Exception(f"Command failed: {' '.join(cmd) if isinstance(cmd, list) else cmd}")
    return result.stdout.strip()

def list_disks():
    result = run_cmd(["lsblk", "-d", "-o", "NAME,SIZE", "-n"])
    disks = []
    print("Available disks:")
    for line in result.split('\n'):
        if line.strip():
            parts = line.split()
            if len(parts) >= 2:
                disk = f"/dev/{parts[0]}"
                size = parts[1]
                print(f"  {disk}: {size}")
                disks.append(disk)
    return disks

def select_disk():
    disks = list_disks()
    while True:
        disk = input("Enter target disk (e.g., /dev/sda): ").strip()
        if disk in disks:
            confirm = input(f"WARNING: This will erase {disk}. Continue? (yes/no): ").lower()
            if confirm == 'yes':
                return disk
        else:
            print("Invalid disk. Try again.")

def use_zfs():
    return input("Use ZFS for root filesystem? (yes/no): ").lower() == 'yes'

def partition_disk(disk, use_zfs):
    # Wipe and create GPT
    run_cmd(["parted", "-s", disk, "mklabel", "gpt"])
    
    # EFI partition 1MiB to 1GiB
    run_cmd(["parted", "-s", disk, "mkpart", "EFI", "fat32", "1MiB", "1GiB"])
    run_cmd(["parted", "-s", disk, "set", "1", "esp", "on"])
    
    # Root partition 1GiB to end (or -4GiB for swap)
    if use_zfs:
        run_cmd(["parted", "-s", disk, "mkpart", "root", "1GiB", "-4GiB"])
        run_cmd(["parted", "-s", disk, "mkpart", "swap", "linux-swap", "-4GiB", "100%"])
        parts = [f"{disk}1", f"{disk}2", f"{disk}3"]  # EFI, root, swap
    else:
        run_cmd(["parted", "-s", disk, "mkpart", "root", "ext4", "1GiB", "100%"])
        parts = [f"{disk}1", f"{disk}2"]  # EFI, root
    
    return parts

def format_partitions(parts, use_zfs):
    efi_part, root_part = parts[0], parts[1]
    run_cmd(["mkfs.vfat", "-F32", efi_part])
    if use_zfs:
        run_cmd(["zpool", "create", "-f", "rpool", root_part])
        run_cmd(["zfs", "create", "-o", "mountpoint=/", "rpool/ROOT"])
        run_cmd(["zfs", "create", "-o", "mountpoint=/home", "rpool/HOME"])
    else:
        run_cmd(["mkfs.ext4", root_part])
        if len(parts) > 2:
            run_cmd(["mkswap", parts[2]])

def copy_rootfs(root_part, use_zfs):
    mount_point = "/tmp/install"
    os.makedirs(mount_point, exist_ok=True)
    if use_zfs:
        run_cmd(["zfs", "set", "mountpoint=/tmp/install", "rpool/ROOT"])
    else:
        run_cmd(["mount", root_part, mount_point])
    # Check if writable
    try:
        with open(f"{mount_point}/test_write", "w") as f:
            f.write("test")
        os.remove(f"{mount_point}/test_write")
    except:
        raise Exception(f"Target filesystem at {mount_point} is readonly")
    print("Copying rootfs...")
    excludes = ["--exclude=/mnt", "--exclude=/proc", "--exclude=/sys", "--exclude=/dev", "--exclude=/tmp"]
    run_cmd(["rsync", "-a"] + excludes + ["/", f"{mount_point}/"])
    return mount_point

def dualboot_detect(mount_point):
    for bind in ["/dev", "/proc", "/sys"]:
        run_cmd(["mount", "--bind", bind, f"{mount_point}{bind}"])
    run_cmd(["chroot", mount_point, "os-prober"])
    run_cmd(["chroot", mount_point, "update-grub"])

def install_grub(mount_point, disk, efi_part, dualboot):
    for bind in ["/dev", "/proc", "/sys"]:
        run_cmd(["mount", "--bind", bind, f"{mount_point}{bind}"])
    
    uefi = os.path.exists("/sys/firmware/efi")
    if uefi:
        run_cmd(["chroot", mount_point, "apt", "update"])
        run_cmd(["chroot", mount_point, "apt", "install", "-y", "grub-efi-amd64"])
        run_cmd(["chroot", mount_point, "grub-install", "--target=x86_64-efi", "--efi-directory=/boot/efi", "--bootloader-id=Fyntora", disk])
    else:
        run_cmd(["chroot", mount_point, "grub-install", disk])
    
    if dualboot:
        dualboot_detect(mount_point)
    else:
        run_cmd(["chroot", mount_point, "update-grub"])

def post_install(mount_point, use_zfs):
    for bind in ["/dev", "/proc", "/sys"]:
        run_cmd(["mount", "--bind", bind, f"{mount_point}{bind}"])
    
    hostname = input("Enter hostname (default: fyntora): ").strip() or "fyntora"
    run_cmd(["chroot", mount_point, "hostnamectl", "set-hostname", hostname])
    
    username = input("Enter new user name: ").strip()
    if username:
        run_cmd(["chroot", mount_point, "useradd", "-m", "-s", "/bin/bash", username])
        run_cmd(["chroot", mount_point, "passwd", username])
    
    extras = input("Extra packages to install (comma-separated, e.g., vim,git): ").strip()
    if extras:
        run_cmd(["chroot", mount_point, "apt", "update"])
        run_cmd(["chroot", mount_point, "apt", "install", "-y"] + extras.split(","))
    
    if use_zfs:
        run_cmd(["chroot", mount_point, "update-initramfs", "-u"])
    else:
        run_cmd(["chroot", mount_point, "update-initramfs", "-u"])

def main():
    try:
        print("Fyntora Installer")
        disk = select_disk()
        use_z = use_zfs()
        parts = partition_disk(disk, use_z)
        format_partitions(parts, use_z)
        mount_point = copy_rootfs(parts[1], use_z)
        dual = input("Enable dual-boot detection? (yes/no): ").lower() == 'yes'
        install_grub(mount_point, disk, parts[0], dual)
        post_install(mount_point, use_z)
        print("Installation complete! Reboot to boot from disk.")
        run_cmd(["touch", "/etc/fyntora_installed"])
    except Exception as e:
        print(f"Installation failed: {e}")
        sys.exit(1)

if __name__ == "__main__":
    main()