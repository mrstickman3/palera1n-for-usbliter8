#include <unistd.h>
#include <palerain.h>

#include <stdio.h>
#include <string.h>
#include <simulator.h>

int simulator_fast = 0;
static int enabled = 0;
static int sim_device = 0;
static int sim_mode = 0;

void simulator_init(int device, int mode)
{
    enabled = 1;
    sim_device = device;
    sim_mode = mode;

    printf("[SIM] Detecting device...\n");
    sleep(2);
    printf("[SIM] Device connected\n");
    if (!simulator_fast) sleep(1);

    if (device == SIM_DEVICE_A12) {
        sleep(2);
        printf("[SIM] Chip: A12\n");
    }

    if (device == SIM_DEVICE_A13) {
        sleep(2);
        printf("[SIM] Chip: A13\n");
    }

    if (mode == SIM_MODE_PWNED_DFU) {
        sleep(2);
        printf("[SIM] Mode: PWNED DFU\n");
    }
    else
        printf("[SIM] Mode: DFU\n");
}


int simulator_is_enabled(void)
{
    return enabled;
}



void *simulator_pongo_helper(void *ptr) {
    (void)ptr;
    LOG(LOG_INFO, "Simulator: waiting for PongoOS...");
    sleep(3);
    LOG(LOG_INFO, "Simulator: PongoOS connected");
    set_found_pongo(1);

    if (!simulator_fast) sleep(1);
    LOG(LOG_INFO, "Simulator: accepting Pongo commands");
    if (!simulator_fast) sleep(1);
    LOG(LOG_INFO, "Simulator: KPF uploaded");
    if (!simulator_fast) sleep(1);
    LOG(LOG_INFO, "Simulator: ramdisk uploaded");
    if (!simulator_fast) sleep(1);
    LOG(LOG_INFO, "Simulator: boot sequence ready");
    device_has_booted = 1;

    set_spin(0);
    return NULL;
}

