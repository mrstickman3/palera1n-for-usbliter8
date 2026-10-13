#ifndef USBLITER8_BOOT_H
#define USBLITER8_BOOT_H
#include <stddef.h>

typedef void (*usbliter8_progress_cb_t)(size_t sent, size_t total, void *user_data);

int usbliter8_boot(const unsigned char *buf, size_t len);
int usbliter8_boot_file(const char *path);
int usbliter8_boot_with_progress(const unsigned char *buf, size_t len, usbliter8_progress_cb_t cb, void *user_data);
int usbliter8_boot_file_with_progress(const char *path, usbliter8_progress_cb_t cb, void *user_data);

#endif
