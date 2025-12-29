#include "iso.h"
#include "utils.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/stat.h>

static int create_iso_structure(const char *iso_dir) {
    char cmd[512];
    snprintf(cmd, sizeof(cmd), "mkdir -p '%s'/boot/isolinux", iso_dir);
    return run_command(cmd);
}

static int setup_grub(const char *iso_dir) {
    char cmd[512];

    // Create GRUB directory structure
    snprintf(cmd, sizeof(cmd), "mkdir -p '%s/boot/grub'", iso_dir);
    if (run_command(cmd) != 0) return 1;

    // Create grub.cfg
    char grub_cfg[256];
    snprintf(grub_cfg, sizeof(grub_cfg), "%s/boot/grub/grub.cfg", iso_dir);
    FILE *f = fopen(grub_cfg, "w");
    if (!f) return 1;

    fprintf(f, "set timeout=5\n");
    fprintf(f, "set default=0\n\n");
    fprintf(f, "menuentry \"Linux Distribution\" {\n");
    fprintf(f, "    linux /boot/vmlinuz root=/dev/ram0 rw console=ttyS0 console=tty0\n");
    fprintf(f, "    initrd /boot/initrd.img\n");
    fprintf(f, "}\n");

    fclose(f);

    return 0;
}

static int copy_kernel_and_rootfs(const iso_config_t *config, const char *iso_dir) {
    char cmd[1024];

    // Copy kernel
    if (config->kernel_path) {
        snprintf(cmd, sizeof(cmd), "cp '%s' '%s/boot/vmlinuz'",
                 config->kernel_path, iso_dir);
        if (run_command(cmd) != 0) return 1;
    }

    // Copy initrd if provided
    if (config->initrd_path) {
        snprintf(cmd, sizeof(cmd), "cp '%s' '%s/boot/initrd.img'",
                 config->initrd_path, iso_dir);
        if (run_command(cmd) != 0) return 1;
    }

    // Copy rootfs
    if (config->rootfs_path) {
        snprintf(cmd, sizeof(cmd), "cp -r '%s'/* '%s/'",
                 config->rootfs_path, iso_dir);
        if (run_command(cmd) != 0) return 1;
    }

    return 0;
}

static int setup_bootloader(const char *iso_dir) {
    // Try GRUB (more reliable than isolinux for our use case)
    return setup_grub(iso_dir);
}

static int create_isolinux_config(const iso_config_t *config, const char *iso_dir) {
    char cfg_path[256];
    snprintf(cfg_path, sizeof(cfg_path), "%s/boot/isolinux/isolinux.cfg", iso_dir);

    FILE *f = fopen(cfg_path, "w");
    if (!f) {
        fprintf(stderr, "Error: Could not create isolinux.cfg\n");
        return 1;
    }

    fprintf(f, "DEFAULT linux\n");
    fprintf(f, "LABEL linux\n");
    fprintf(f, "  KERNEL /boot/vmlinuz\n");
    if (config->initrd_path) {
        fprintf(f, "  INITRD /boot/initrd.img\n");
    }
    fprintf(f, "  APPEND root=/dev/ram0 rw console=ttyS0 console=tty0\n");
    fprintf(f, "TIMEOUT 50\n");
    fprintf(f, "PROMPT 1\n");

    fclose(f);
    return 0;
}

static int create_iso_image(const iso_config_t *config, const char *iso_dir) {
    char cmd[1024];

    // Use xorriso with GRUB for EFI support, or fallback to basic ISO
    if (run_command("command -v xorriso >/dev/null") == 0) {
        // Use xorriso with GRUB
        snprintf(cmd, sizeof(cmd),
                 "xorriso -as mkisofs -o '%s' -V '%s' "
                 "-r -J -l -b boot/grub/i386-pc/eltorito.img "
                 "-no-emul-boot -boot-load-size 4 -boot-info-table "
                 "--grub2-boot-info --grub2-mbr /usr/lib/grub/i386-pc/boot_hybrid.img "
                 "-eltorito-alt-boot -e EFI/BOOT/BOOTX64.EFI -no-emul-boot "
                 "'%s' 2>/dev/null || "
                 // Fallback to basic ISO if GRUB EFI fails
                 "xorriso -as mkisofs -o '%s' -V '%s' -r -J '%s'",
                 config->output_path, config->label ? config->label : "LinuxDist",
                 iso_dir, config->output_path, config->label ? config->label : "LinuxDist", iso_dir);
    } else if (run_command("command -v mkisofs >/dev/null") == 0) {
        // Use mkisofs
        snprintf(cmd, sizeof(cmd),
                 "mkisofs -o '%s' -V '%s' -r -J '%s'",
                 config->output_path, config->label ? config->label : "LinuxDist", iso_dir);
    } else if (run_command("command -v genisoimage >/dev/null") == 0) {
        // Use genisoimage
        snprintf(cmd, sizeof(cmd),
                 "genisoimage -o '%s' -V '%s' -r -J '%s'",
                 config->output_path, config->label ? config->label : "LinuxDist", iso_dir);
    } else {
        fprintf(stderr, "Error: No ISO creation tool found (xorriso, mkisofs, genisoimage)\n");
        return 1;
    }

    return run_command(cmd);
}

int create_iso(const iso_config_t *config) {
    if (!config || !config->output_path) {
        fprintf(stderr, "Error: Invalid ISO configuration\n");
        return 1;
    }

    char iso_dir[256] = "/tmp/iso_build_XXXXXX";
    if (!mkdtemp(iso_dir)) {
        fprintf(stderr, "Error: Could not create temporary directory\n");
        return 1;
    }

    printf("Creating bootable ISO...\n");
    printf("Output: %s\n", config->output_path);
    printf("Kernel: %s\n", config->kernel_path ? config->kernel_path : "none");
    printf("InitRD: %s\n", config->initrd_path ? config->initrd_path : "none");
    printf("RootFS: %s\n", config->rootfs_path ? config->rootfs_path : "none");

    // Create ISO structure
    if (create_iso_structure(iso_dir) != 0) {
        goto cleanup;
    }

    // Setup bootloader
    if (setup_bootloader(iso_dir) != 0) {
        goto cleanup;
    }

    // Copy kernel and rootfs
    if (copy_kernel_and_rootfs(config, iso_dir) != 0) {
        goto cleanup;
    }

    // Create isolinux config
    if (create_isolinux_config(config, iso_dir) != 0) {
        goto cleanup;
    }

    // Create the ISO image
    if (create_iso_image(config, iso_dir) != 0) {
        goto cleanup;
    }

    // Cleanup temporary directory
    char cmd[512];
    snprintf(cmd, sizeof(cmd), "rm -rf '%s'", iso_dir);
    run_command(cmd);

    printf("Bootable ISO created successfully\n");
    return 0;

cleanup:
    snprintf(cmd, sizeof(cmd), "rm -rf '%s'", iso_dir);
    run_command(cmd);
    return 1;
}