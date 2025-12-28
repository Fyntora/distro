# Fyntora Linux Distro Build Suite

This is an advanced build suite for creating a professional custom Linux distribution based on OpenSuse.

## Features

- JSON-based configuration
- Custom kernel building
- Chroot-based isolated builds
- Systemd init system integration
- Multi-architecture support (x86_64, arm64)
- Build caching for faster rebuilds
- Automated testing and validation

## Requirements

- OpenSuse host system
- gcc, bash, zypper, xorriso, jq, wget installed
- For arm64: aarch64-linux-gnu-gcc

## Usage

1. Edit `config/distro.json` to customize your distro.
2. Run `./build.sh` to build the distro.
3. The ISO will be generated as specified in config.

## Configuration

The `config/distro.json` file allows you to specify:
- Distro metadata
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