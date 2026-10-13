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



/* Simulate checkm8 exploit for A12/A13 devices */
int simulator_checkm8_exploit(void) {
    LOG(LOG_INFO, "Simulator: running checkm8 exploit for A12/A13...");
    if (!simulator_fast) sleep(2);
    LOG(LOG_INFO, "Simulator: checkm8 exploit succeeded - device in pwned DFU");
    return 0;
}

/* Simulate usbliter8 for A12/A13 pwned DFU */
int simulator_usbliter8(void) {
    LOG(LOG_INFO, "Simulator: running usbliter8 (iBoot pwn) for A12/A13...");
    if (!simulator_fast) sleep(3);
    LOG(LOG_INFO, "Simulator: usbliter8 succeeded - device in pwned DFU");
    return 0;
}

/* Simulate A12/A13 DFU mode detection */
int simulator_detect_dfu_mode(void) {
    if (sim_mode == SIM_MODE_PWNED_DFU) {
        LOG(LOG_INFO, "Simulator: A12/A13 device (CPID 0x%04x) detected in pwned DFU mode",
            sim_device == SIM_DEVICE_A12 ? 0x8000 : 0x8015);
        return 0;
    }
    LOG(LOG_INFO, "Simulator: A12/A13 device (CPID 0x%04x) detected in normal DFU mode",
        sim_device == SIM_DEVICE_A12 ? 0x8000 : 0x8015);
    return 1;
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
    set_device_has_booted(true);

    set_spin(0);
    return NULL;
}

