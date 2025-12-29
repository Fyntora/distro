#ifndef ROOTFS_H
#define ROOTFS_H

typedef enum {
    ROOTFS_BUSYBOX,
    ROOTFS_UBUNTU,
    ROOTFS_OPENSUSE,
    ROOTFS_MINIMAL
} rootfs_type_t;

typedef struct {
    char *target_dir;
    char *arch;
    rootfs_type_t type;
    char *version;
    char **packages;
    int num_packages;
} rootfs_config_t;

int create_rootfs(const rootfs_config_t *config);

#endif