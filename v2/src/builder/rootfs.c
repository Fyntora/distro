#include "rootfs.h"
#include "utils.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

static int create_directory_structure(const char *rootfs_dir) {
    const char *dirs[] = {
        "bin", "boot", "dev", "etc", "home", "lib", "lib64", "mnt",
        "opt", "proc", "root", "sbin", "srv", "sys", "tmp", "usr", "var",
        "usr/bin", "usr/lib", "usr/local", "usr/share",
        "var/log", "var/run", "var/spool",
        NULL
    };

    char cmd[2048] = "mkdir -p";
    for (int i = 0; dirs[i]; i++) {
        strncat(cmd, " '", sizeof(cmd) - strlen(cmd) - 1);
        strncat(cmd, rootfs_dir, sizeof(cmd) - strlen(cmd) - 1);
        strncat(cmd, "/", sizeof(cmd) - strlen(cmd) - 1);
        strncat(cmd, dirs[i], sizeof(cmd) - strlen(cmd) - 1);
        strncat(cmd, "'", sizeof(cmd) - strlen(cmd) - 1);
    }

    return run_command(cmd);
}

static int download_busybox(const char *target_dir) {
    char cmd[512];
    // Download busybox static binary
    snprintf(cmd, sizeof(cmd),
             "wget -q -O '%s/bin/busybox' https://busybox.net/downloads/binaries/1.35.0-x86_64-linux-musl/busybox",
             target_dir);
    if (run_command(cmd) != 0) {
        fprintf(stderr, "Failed to download busybox\n");
        return 1;
    }

    // Make it executable
    snprintf(cmd, sizeof(cmd), "chmod +x '%s/bin/busybox'", target_dir);
    return run_command(cmd);
}

static int setup_busybox_symlinks(const char *target_dir) {
    const char *applets[] = {
        "sh", "ls", "cat", "echo", "mkdir", "rm", "cp", "mv", "ln", "chmod", "chown",
        "ps", "kill", "mount", "umount", "df", "du", "free", "top", "grep", "sed", "awk",
        "find", "xargs", "sort", "uniq", "wc", "head", "tail", "cut", "tr", "tee",
        "tar", "gzip", "gunzip", "bzcat", "wget", "ping", "ifconfig", "route", "netstat",
        "hostname", "date", "sleep", "usleep", "time", "touch", "dd", "mkfifo", "mknod",
        NULL
    };

    char cmd[1024];
    snprintf(cmd, sizeof(cmd), "cd '%s/bin' && for app in", target_dir);
    for (int i = 0; applets[i]; i++) {
        strncat(cmd, " ", sizeof(cmd) - strlen(cmd) - 1);
        strncat(cmd, applets[i], sizeof(cmd) - strlen(cmd) - 1);
    }
    strncat(cmd, "; do ln -s busybox $app; done", sizeof(cmd) - strlen(cmd) - 1);

    return run_command(cmd);
}

static int setup_busybox_rootfs(const rootfs_config_t *config) {
    if (create_directory_structure(config->target_dir) != 0) return 1;

    // Download and install busybox
    if (download_busybox(config->target_dir) != 0) return 1;

    // Create symlinks for busybox applets
    if (setup_busybox_symlinks(config->target_dir) != 0) return 1;

    // Create basic device nodes
    char cmd[2048];
    snprintf(cmd, sizeof(cmd),
             "mknod -m 600 '%s/dev/console' c 5 1 2>/dev/null || true && "
             "mknod -m 666 '%s/dev/null' c 1 3 2>/dev/null || true && "
             "mknod -m 666 '%s/dev/zero' c 1 5 2>/dev/null || true && "
             "mknod -m 666 '%s/dev/tty' c 5 0 2>/dev/null || true",
             config->target_dir, config->target_dir, config->target_dir, config->target_dir);
    run_command(cmd); // Non-critical, continue on failure

    // Create a busybox-based init script
    char init_content[] = "#!/bin/busybox sh\n"
                         "mount -t proc proc /proc\n"
                         "mount -t sysfs sysfs /sys\n"
                         "mount -t devtmpfs devtmpfs /dev\n"
                         "echo 'BusyBox Linux init'\n"
                         "exec /bin/busybox sh\n";
    char init_path[256];
    snprintf(init_path, sizeof(init_path), "%s/init", config->target_dir);
    FILE *f = fopen(init_path, "w");
    if (f) {
        fputs(init_content, f);
        fclose(f);
        snprintf(cmd, sizeof(cmd), "chmod +x '%s'", init_path);
        run_command(cmd);
    }

    // Create basic /etc/passwd and /etc/group
    char passwd_content[] = "root:x:0:0:root:/root:/bin/sh\n";
    char passwd_path[256];
    snprintf(passwd_path, sizeof(passwd_path), "%s/etc/passwd", config->target_dir);
    f = fopen(passwd_path, "w");
    if (f) {
        fputs(passwd_content, f);
        fclose(f);
    }

    char group_content[] = "root:x:0:\n";
    char group_path[256];
    snprintf(group_path, sizeof(group_path), "%s/etc/group", config->target_dir);
    f = fopen(group_path, "w");
    if (f) {
        fputs(group_content, f);
        fclose(f);
    }

    return 0;
}

static int setup_minimal_rootfs(const rootfs_config_t *config) {
    // For minimal, just use busybox as well
    return setup_busybox_rootfs(config);
}

static int setup_ubuntu_rootfs(const rootfs_config_t *config) {
    // Ubuntu style - busybox based with Ubuntu-like branding
    printf("Creating Ubuntu-style rootfs with BusyBox\n");
    int result = setup_busybox_rootfs(config);
    if (result != 0) return result;

    // Add Ubuntu-specific files
    char motd_content[] = "Welcome to Ubuntu Linux (BusyBox)\n";
    char motd_path[256];
    snprintf(motd_path, sizeof(motd_path), "%s/etc/motd", config->target_dir);
    FILE *f = fopen(motd_path, "w");
    if (f) {
        fputs(motd_content, f);
        fclose(f);
    }

    return 0;
}

static int setup_opensuse_rootfs(const rootfs_config_t *config) {
    // OpenSUSE style - busybox based with SUSE-like branding
    printf("Creating OpenSUSE-style rootfs with BusyBox\n");
    int result = setup_busybox_rootfs(config);
    if (result != 0) return result;

    // Add SUSE-specific files
    char issue_content[] = "Welcome to openSUSE Linux (BusyBox)\n";
    char issue_path[256];
    snprintf(issue_path, sizeof(issue_path), "%s/etc/issue", config->target_dir);
    FILE *f = fopen(issue_path, "w");
    if (f) {
        fputs(issue_content, f);
        fclose(f);
    }

    return 0;
}

int create_rootfs(const rootfs_config_t *config) {
    if (!config || !config->target_dir || !config->arch) {
        fprintf(stderr, "Error: Invalid rootfs configuration\n");
        return 1;
    }

    printf("Creating root filesystem in %s\n", config->target_dir);
    printf("Architecture: %s\n", config->arch);
    const char *type_str;
    switch (config->type) {
        case ROOTFS_BUSYBOX: type_str = "BusyBox"; break;
        case ROOTFS_UBUNTU: type_str = "Ubuntu"; break;
        case ROOTFS_OPENSUSE: type_str = "openSUSE"; break;
        case ROOTFS_MINIMAL: type_str = "Minimal"; break;
        default: type_str = "Unknown"; break;
    }
    printf("Type: %s\n", type_str);

    // Clean and create directory
    char cmd[512];
    snprintf(cmd, sizeof(cmd), "rm -rf '%s' && mkdir -p '%s'", config->target_dir, config->target_dir);
    if (run_command(cmd) != 0) return 1;

    int result = 0;
    switch (config->type) {
        case ROOTFS_BUSYBOX:
            result = setup_busybox_rootfs(config);
            break;
        case ROOTFS_UBUNTU:
            result = setup_ubuntu_rootfs(config);
            break;
        case ROOTFS_OPENSUSE:
            result = setup_opensuse_rootfs(config);
            break;
        case ROOTFS_MINIMAL:
            result = setup_minimal_rootfs(config);
            break;
        default:
            fprintf(stderr, "Error: Unsupported rootfs type\n");
            result = 1;
    }

    if (result == 0) {
        printf("Root filesystem created successfully\n");
    }

    return result;
}