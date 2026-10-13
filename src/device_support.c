#include <stddef.h>
#include <palerain.h>
#include "device_support.h"

bool is_a12_a13(struct irecv_device_info *info)
{
    if (info == NULL)
        return false;

    /* A12/A13 device CPIDs:
     * 0x8000: A12 (iPhone XS/XS Max/XR, iPad 8th/9th gen, iPad Air 3rd, iPad mini 5th)
     * 0x8001: A12X/A12Z (iPad Pro)
     * 0x8003: A12 (different variant)
     * 0x8020: A13 (iPhone 11/11 Pro/11 Pro Max, iPhone SE 2nd gen)
     */
    switch (info->cpid) {
        case 0x8000:
        case 0x8001:
        case 0x8003:
        case 0x8020:
            return true;
        default:
            return false;
    }
}

device_path_t get_device_path(struct irecv_device_info *info)
{
    if (info == NULL)
        return DEVICE_PATH_UNSUPPORTED;

    if (is_a12_a13(info))
        return DEVICE_PATH_USBLITER8;

    if (cpid_is_arm64(info->cpid))
        return DEVICE_PATH_CHECKRA1N;

    return DEVICE_PATH_UNSUPPORTED;
}

