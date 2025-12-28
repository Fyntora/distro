# Fyntora Linux Distro Build Suite

This is an advanced build suite for creating a professional custom Linux distribution based on OpenSuse.

## Features

- JSON-based configuration
- Custom kernel building
- Chroot-based isolated builds
- Systemd init system integration
- Multi-architecture support (x86_64, arm64)
- Multi-base support (OpenSuse, Ubuntu)
- Build caching for faster rebuilds
- Automated testing and validation

## Requirements

- Linux host system (OpenSuse or Ubuntu)
- gcc, bash, jq, wget, xorriso installed
- For Ubuntu host: debootstrap
- For OpenSuse host: zypper
- For arm64: aarch64-linux-gnu-gcc

## Usage

1. Edit `config/distro.json` to customize your distro.
2. Run `./build.sh` to build the distro.
3. The ISO will be generated as specified in config.

## Configuration

The `config/distro.json` file allows you to specify:
- Distro metadata
- Host OS and target base (opensuse, ubuntu)
- Package lists and repositories
- Kernel version and config
- Init system
- Bootloader
- Output path

## Structure

- `src/`: C source code
- `scripts/`: Shell scripts for build stages
- `config/`: JSON configuration files
- `tools/`: Compiled binaries
- `build/`: Build artifacts and cache