#include <string.h>
#include <libirecovery.h>
#include <palerain.h>
#include <pwned_dfu.h>

int detect_pwned_dfu(irecv_client_t device) {
    if (!device) {
        LOG(LOG_ERROR, "Invalid DFU device handle");
        return -1;
    }
    const struct irecv_device_info *info = irecv_get_device_info(device);
    if (!info) {
        LOG(LOG_ERROR, "Unable to query DFU device");
        return -1;
    }
if (info->srnm && strstr(info->srnm, "PWND:")) {
    LOG(LOG_INFO, "Pwned DFU marker detected: %s", info->srnm);
    return 1;
}
    return 0;
}
