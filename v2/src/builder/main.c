#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include "kernel.h"
#include "rootfs.h"
#include "iso.h"

void print_usage(const char *prog_name) {
    printf("Usage: %s [OPTIONS]\n", prog_name);
    printf("Build a Linux kernel, root filesystem, and/or bootable ISO.\n\n");
    printf("Options:\n");
    printf("  -v, --version VERSION    Kernel version to build (default: 6.6.8)\n");
    printf("  -u, --url URL            Kernel download URL (default: auto-generated)\n");
    printf("  -m, --menuconfig         Use menuconfig instead of defconfig\n");
    printf("  -r, --rootfs TYPE        Create root filesystem (busybox, ubuntu, opensuse, minimal)\n");
    printf("  -a, --arch ARCH          Architecture for rootfs (default: x86_64)\n");
    printf("  -d, --rootfs-dir DIR     Rootfs target directory (default: ./rootfs)\n");
    printf("  -p, --packages PKGS      Comma-separated list of packages to install\n");
    printf("  -i, --iso PATH           Create bootable ISO at specified path\n");
    printf("  -l, --label LABEL        ISO volume label (default: LinuxDist)\n");
    printf("  -k, --kernel-only        Build kernel only (skip rootfs and ISO)\n");
    printf("  -f, --rootfs-only        Create rootfs only (skip kernel and ISO)\n");
    printf("  -s, --iso-only           Create ISO only (skip kernel and rootfs)\n");
    printf("  -h, --help               Show this help message\n");
}

int main(int argc, char *argv[]) {
    char *kernel_version = "6.6.8";
    char *kernel_url = NULL;
    int use_defconfig = 1;
    int build_kernel = 1;
    int build_rootfs = 0;
    int build_iso = 0;
    char *rootfs_type_str = NULL;
    char *rootfs_arch = "x86_64";
    char *rootfs_dir = "./rootfs";
    char *packages_str = NULL;
    char *iso_path = NULL;
    char *iso_label = "LinuxDist";

    // Parse command line arguments
    for (int i = 1; i < argc; i++) {
        if (strcmp(argv[i], "-v") == 0 || strcmp(argv[i], "--version") == 0) {
            if (i + 1 < argc) {
                kernel_version = argv[++i];
            } else {
                fprintf(stderr, "Error: --version requires an argument\n");
                return 1;
            }
        } else if (strcmp(argv[i], "-u") == 0 || strcmp(argv[i], "--url") == 0) {
            if (i + 1 < argc) {
                kernel_url = argv[++i];
            } else {
                fprintf(stderr, "Error: --url requires an argument\n");
                return 1;
            }
        } else if (strcmp(argv[i], "-m") == 0 || strcmp(argv[i], "--menuconfig") == 0) {
            use_defconfig = 0;
        } else if (strcmp(argv[i], "-r") == 0 || strcmp(argv[i], "--rootfs") == 0) {
            if (i + 1 < argc) {
                rootfs_type_str = argv[++i];
                build_rootfs = 1;
            } else {
                fprintf(stderr, "Error: --rootfs requires an argument\n");
                return 1;
            }
        } else if (strcmp(argv[i], "-a") == 0 || strcmp(argv[i], "--arch") == 0) {
            if (i + 1 < argc) {
                rootfs_arch = argv[++i];
            } else {
                fprintf(stderr, "Error: --arch requires an argument\n");
                return 1;
            }
        } else if (strcmp(argv[i], "-d") == 0 || strcmp(argv[i], "--rootfs-dir") == 0) {
            if (i + 1 < argc) {
                rootfs_dir = argv[++i];
            } else {
                fprintf(stderr, "Error: --rootfs-dir requires an argument\n");
                return 1;
            }
        } else if (strcmp(argv[i], "-p") == 0 || strcmp(argv[i], "--packages") == 0) {
            if (i + 1 < argc) {
                packages_str = argv[++i];
            } else {
                fprintf(stderr, "Error: --packages requires an argument\n");
                return 1;
            }
        } else if (strcmp(argv[i], "-k") == 0 || strcmp(argv[i], "--kernel-only") == 0) {
            build_rootfs = 0;
        } else if (strcmp(argv[i], "-f") == 0 || strcmp(argv[i], "--rootfs-only") == 0) {
            build_kernel = 0;
            build_rootfs = 1;
        } else if (strcmp(argv[i], "-i") == 0 || strcmp(argv[i], "--iso") == 0) {
            if (i + 1 < argc) {
                iso_path = argv[++i];
                build_iso = 1;
            } else {
                fprintf(stderr, "Error: --iso requires an argument\n");
                return 1;
            }
        } else if (strcmp(argv[i], "-l") == 0 || strcmp(argv[i], "--label") == 0) {
            if (i + 1 < argc) {
                iso_label = argv[++i];
            } else {
                fprintf(stderr, "Error: --label requires an argument\n");
                return 1;
            }
        } else if (strcmp(argv[i], "-s") == 0 || strcmp(argv[i], "--iso-only") == 0) {
            build_kernel = 0;
            build_rootfs = 0;
            build_iso = 1;
        } else if (strcmp(argv[i], "-h") == 0 || strcmp(argv[i], "--help") == 0) {
            print_usage(argv[0]);
            return 0;
        } else {
            fprintf(stderr, "Unknown option: %s\n", argv[i]);
            print_usage(argv[0]);
            return 1;
        }
    }

    // Generate default URL if not provided
    if (!kernel_url) {
        kernel_url = malloc(256);
        if (!kernel_url) {
            fprintf(stderr, "Memory allocation failed\n");
            return 1;
        }
        snprintf(kernel_url, 256,
                 "https://cdn.kernel.org/pub/linux/kernel/v6.x/linux-%s.tar.xz",
                 kernel_version);
    }

    // Parse rootfs type
    rootfs_type_t rootfs_type = ROOTFS_BUSYBOX;
    if (rootfs_type_str) {
        if (strcmp(rootfs_type_str, "busybox") == 0) {
            rootfs_type = ROOTFS_BUSYBOX;
        } else if (strcmp(rootfs_type_str, "ubuntu") == 0) {
            rootfs_type = ROOTFS_UBUNTU;
        } else if (strcmp(rootfs_type_str, "opensuse") == 0) {
            rootfs_type = ROOTFS_OPENSUSE;
        } else if (strcmp(rootfs_type_str, "minimal") == 0) {
            rootfs_type = ROOTFS_MINIMAL;
        } else {
            fprintf(stderr, "Error: Invalid rootfs type '%s'\n", rootfs_type_str);
            free(kernel_url);
            return 1;
        }
    }

    // Parse packages
    char **packages = NULL;
    int num_packages = 0;
    if (packages_str) {
        char *token = strtok(packages_str, ",");
        while (token) {
            packages = realloc(packages, sizeof(char *) * (num_packages + 1));
            packages[num_packages] = strdup(token);
            num_packages++;
            token = strtok(NULL, ",");
        }
    }

    printf("Linux Distribution Builder v2.0\n");
    printf("===============================\n");
    if (build_kernel) {
        printf("Kernel Version: %s\n", kernel_version);
        printf("Kernel URL: %s\n", kernel_url);
        printf("Configuration: %s\n", use_defconfig ? "defconfig" : "menuconfig");
    }
    if (build_rootfs) {
        printf("RootFS Type: %s\n", rootfs_type_str);
        printf("RootFS Arch: %s\n", rootfs_arch);
        printf("RootFS Dir: %s\n", rootfs_dir);
    }
    if (build_iso) {
        printf("ISO Path: %s\n", iso_path);
        printf("ISO Label: %s\n", iso_label);
    }
    printf("\n");

    // Build kernel if requested
    if (build_kernel) {
        printf("Starting Linux kernel build process...\n");

        printf("Step 1: Downloading kernel %s\n", kernel_version);
        if (download_kernel(kernel_version, kernel_url) != 0) {
            fprintf(stderr, "Failed to download kernel\n");
            goto cleanup;
        }

        printf("Step 2: Extracting kernel\n");
        if (extract_kernel(kernel_version) != 0) {
            fprintf(stderr, "Failed to extract kernel\n");
            goto cleanup;
        }

        printf("Step 3: Configuring kernel\n");
        if (configure_kernel(kernel_version, use_defconfig) != 0) {
            fprintf(stderr, "Failed to configure kernel\n");
            goto cleanup;
        }

        printf("Step 4: Compiling kernel\n");
        if (compile_kernel(kernel_version) != 0) {
            fprintf(stderr, "Failed to compile kernel\n");
            goto cleanup;
        }

        printf("\n✓ Kernel build complete successfully!\n");
    }

    // Build rootfs if requested
    if (build_rootfs) {
        printf("Starting root filesystem creation...\n");

        rootfs_config_t config = {
            .target_dir = rootfs_dir,
            .arch = rootfs_arch,
            .type = rootfs_type,
            .version = "jammy",  // Default Ubuntu version
            .packages = packages,
            .num_packages = num_packages
        };

        if (create_rootfs(&config) != 0) {
            fprintf(stderr, "Failed to create root filesystem\n");
            goto cleanup;
        }

        printf("\n✓ Root filesystem created successfully!\n");
    }

    // Build ISO if requested
    if (build_iso) {
        printf("Starting ISO creation...\n");

        // Find kernel path - assume it's built
        char kernel_path[256];
        if (!kernel_version) {
            fprintf(stderr, "Error: Kernel version not specified for ISO creation\n");
            goto cleanup;
        }
        snprintf(kernel_path, sizeof(kernel_path), "linux-%s/arch/x86/boot/bzImage", kernel_version);

        iso_config_t iso_config = {
            .output_path = iso_path,
            .kernel_path = kernel_path,
            .rootfs_path = build_rootfs ? rootfs_dir : NULL,
            .initrd_path = NULL,  // No initrd for now
            .label = iso_label
        };

        if (create_iso(&iso_config) != 0) {
            fprintf(stderr, "Failed to create ISO\n");
            goto cleanup;
        }

        printf("\n✓ Bootable ISO created successfully!\n");
    }

    // Cleanup
    free(kernel_url);
    for (int i = 0; i < num_packages; i++) {
        free(packages[i]);
    }
    free(packages);

    return 0;

cleanup:
    free(kernel_url);
    for (int i = 0; i < num_packages; i++) {
        free(packages[i]);
    }
    free(packages);
    return 1;
}
