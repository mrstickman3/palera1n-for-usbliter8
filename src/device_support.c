#include <stddef.h>
#include <palerain.h>
#include "device_support.h"

bool is_a12_a13(struct irecv_device_info *info)
{
    if (info == NULL)
        return false;

    switch (info->cpid) {
        /*
         * Add your A12/A13 CPIDs here.
         * Leave this empty until you confirm the CPID table
         * used by your target devices.
         */
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

