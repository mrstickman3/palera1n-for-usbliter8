#ifndef USBLITER8_BOOT_H
#define USBLITER8_BOOT_H

#include <stddef.h>

/*
 * Sends `buf` (len bytes) to a device already in Pwned DFU mode and
 * triggers execution via CUSTOM_BOOT.
 *
 * Returns 0 on success, -1 on failure.
 */
int usbliter8_boot(const unsigned char *buf, size_t len);

/* Convenience wrapper: reads a file from disk, then calls usbliter8_boot(). */
int usbliter8_boot_file(const char *path);

#endif