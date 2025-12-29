#ifndef KERNEL_H
#define KERNEL_H

int download_kernel(const char *version, const char *url);
int extract_kernel(const char *version);
int configure_kernel(const char *version, int defconfig);
int compile_kernel(const char *version);

#endif
