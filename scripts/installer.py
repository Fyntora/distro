#!/usr/bin/env python3
import parted
import subprocess
import os
import sys

def run_cmd(cmd, shell=False):
    result = subprocess.run(cmd if shell else cmd.split(), shell=shell, capture_output=True, text=True)
    if result.returncode != 0:
        print(f"Error: {result.stderr}")
        raise Exception(f"Command failed: {' '.join(cmd) if not shell else cmd}")
    return result.stdout.strip()

def list_disks():
    devices = parted.getAllDevices()
    print("Available disks:")
    for dev in devices:
        print(f"  {dev.path}: {dev.getSize() / (1024**3):.2f} GB")
    return [dev.path for dev in devices]

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
    device = parted.getDevice(disk)
    disk_obj = parted.Disk(device)
    disk_obj.deleteAllPartitions()
    disk_obj.commit()
    
    efi_size = 1024 * 1024 * 1024  # 1GB EFI
    if use_zfs:
        root_size = device.getSize() - efi_size - 4 * 1024 * 1024 * 1024  # Rest -4GB swap
        swap_size = 4 * 1024 * 1024 * 1024
    else:
        root_size = device.getSize() - efi_size  # No swap for ZFS
    
    efi_geom = parted.Geometry(device, 2048, 2048 + int(efi_size / device.sectorSize) - 1)
    efi_part = parted.Partition(disk_obj, parted.PARTITION_NORMAL, parted.FileSystem(ext2, efi_geom))
    efi_part.setFlag(parted.PARTITION_BOOT)
    disk_obj.addPartition(efi_part, parted.Constraint(device))
    
    root_geom = parted.Geometry(device, efi_geom.end + 1, efi_geom.end + 1 + int(root_size / device.sectorSize) - 1)
    root_part = parted.Partition(disk_obj, parted.PARTITION_NORMAL, parted.FileSystem(ext4 if not use_zfs else None, root_geom))
    disk_obj.addPartition(root_part, parted.Constraint(device))
    
    parts = [f"{disk}1", f"{disk}2"]  # EFI, root
    if not use_zfs:
        swap_geom = parted.Geometry(device, root_geom.end + 1, device.getLength() - 1)
        swap_part = parted.Partition(disk_obj, parted.PARTITION_NORMAL, parted.FileSystem(linux_swap, swap_geom))
        disk_obj.addPartition(swap_part, parted.Constraint(device))
        parts.append(f"{disk}3")  # Swap
    
    disk_obj.commit()
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
    mount_point = "/mnt/install"
    os.makedirs(mount_point, exist_ok=True)
    if use_zfs:
        run_cmd(["zfs", "set", "mountpoint=/mnt/install", "rpool/ROOT"])
    else:
        run_cmd(["mount", root_part, mount_point])
    print("Copying rootfs...")
    excludes = ["--exclude=/mnt", "--exclude=/proc", "--exclude=/sys", "--exclude=/dev"]
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