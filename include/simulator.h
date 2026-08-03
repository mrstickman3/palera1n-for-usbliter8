#ifndef SIMULATOR_H
#define SIMULATOR_H

#define SIM_DEVICE_A12 0
#define SIM_DEVICE_A13 1

#define SIM_MODE_DFU 0
#define SIM_MODE_PWNED_DFU 1

void simulator_init(int device, int mode);
int simulator_is_enabled(void);

#endif


void *simulator_pongo_helper(void *ptr);

