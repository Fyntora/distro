#include "kernel.h"
#include "utils.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int download_kernel(const char *version, const char *url) {
    if (!version || !url) {
        fprintf(stderr, "Error: Invalid version or URL\n");
        return 1;
    }

    char cmd[1024];
    int ret = snprintf(cmd, sizeof(cmd),
             "if [ ! -f linux-%s.tar.xz ]; then wget -q --show-progress %s; fi",
             version, url);
    if (ret >= sizeof(cmd)) {
        fprintf(stderr, "Error: Command too long\n");
        return 1;
    }
    return run_command(cmd);
}

int extract_kernel(const char *version) {
    if (!version) {
        fprintf(stderr, "Error: Invalid version\n");
        return 1;
    }

    char cmd[512];
    int ret = snprintf(cmd, sizeof(cmd), "tar -xf linux-%s.tar.xz", version);
    if (ret >= sizeof(cmd)) {
        fprintf(stderr, "Error: Command too long\n");
        return 1;
    }
    return run_command(cmd);
}

int configure_kernel(const char *version, int defconfig) {
    if (!version) {
        fprintf(stderr, "Error: Invalid version\n");
        return 1;
    }

    char cmd[512];
    if (defconfig) {
        snprintf(cmd, sizeof(cmd), "cd linux-%s && make defconfig", version);
    } else {
        snprintf(cmd, sizeof(cmd), "cd linux-%s && make menuconfig", version);
    }
    return run_command(cmd);
}

int compile_kernel(const char *version) {
    if (!version) {
        fprintf(stderr, "Error: Invalid version\n");
        return 1;
    }

    char cmd[512];
    int ret = snprintf(cmd, sizeof(cmd), "cd linux-%s && make -j$(nproc)", version);
    if (ret >= sizeof(cmd)) {
        fprintf(stderr, "Error: Command too long\n");
        return 1;
    }
    return run_command(cmd);
}
