#ifndef DEVICE_SUPPORT_H
#define DEVICE_SUPPORT_H

#include <stdbool.h>
#include <libirecovery.h>

typedef enum {
    DEVICE_PATH_CHECKRA1N,
    DEVICE_PATH_USBLITER8,
    DEVICE_PATH_UNSUPPORTED
} device_path_t;

device_path_t get_device_path(struct irecv_device_info *info);

bool is_a12_a13(struct irecv_device_info *info);

#endif

