#ifndef USBLITER8_BOOT_H
#define USBLITER8_BOOT_H
#include <stddef.h>

int usbliter8_boot(const unsigned char *buf, size_t len);
int usbliter8_boot_file(const char *path);

#endif
