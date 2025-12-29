#ifndef ISO_H
#define ISO_H

typedef struct {
    char *output_path;
    char *kernel_path;
    char *rootfs_path;
    char *initrd_path;  // Optional
    char *label;
} iso_config_t;

int create_iso(const iso_config_t *config);

#endif