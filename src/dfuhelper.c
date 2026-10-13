#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include <fcntl.h>
#include <stddef.h>
#include <stdbool.h>
#include <unistd.h>
#include <ctype.h>

#include <sys/select.h>
#include <sys/socket.h>
#include <sys/un.h>
#include <sys/ioctl.h>
#include <signal.h>
#include <pthread.h>
#include <errno.h>
#include <inttypes.h>

#include <libimobiledevice/libimobiledevice.h>
#include <libimobiledevice/lockdown.h>
#include <libimobiledevice/diagnostics_relay.h>
#include <plist/plist.h>
#include <libirecovery.h>
#include <usbmuxd.h>

#include <ANSI-color-codes.h>
#include <palerain.h>
#include <pwned_dfu.h>
#include "usbliter8_boot.h"

#define FORMAT_KEY_VALUE 1
/* FORMAT_XML is defined in palerain.h */

static volatile sig_atomic_t dfuhelper_cancel = 0;

#define NO_PHYSICAL_HOME_BUTTON (cpid == 0x8015 || (cpid == 0x8010 && (bdid == 0x08 || bdid == 0x0a || bdid == 0x0c || bdid == 0x0e)))
#define IS_APPLE_TV_HD (cpid == 0x7000 && bdid == 0x34)
#define IS_APPLE_TV_4K (cpid == 0x8011 && bdid == 0x02)
#define IS_APPLE_HOME1 (cpid == 0x7000 && bdid == 0x38)
#define IS_APPLE_HOME2 (cpid == 0x7000 && bdid == 0x1a)
#define IS_APPLE_HOME  (IS_APPLE_HOME1 || IS_APPLE_HOME2)
#define IS_APPLETV (IS_APPLE_TV_4K || IS_APPLE_TV_HD || IS_APPLE_HOME)

/**
 * step - Display a countdown with optional early-exit condition
 * @time: Total countdown start value (seconds) - e.g., 10
 * @time2: Final value to display after countdown (e.g., 8 means count 10,9 then show 8)
 * @text: Message to display
 * @cond: Optional condition function to check for early exit
 * @cond_arg: Argument to pass to condition function
 *
 * Counts down from @time to @time2 + 1 (exclusive of time2), then displays @time2.
 * If @time2 is 0, counts down fully to 1 then prints newline.
 * If @cond returns true, exits the thread early.
 *
 * Example: step(10, 8, "Hold button", NULL, 0) will display:
 *   "Hold button (10)"
 *   "Hold button (9)"
 *   "Hold button (8)"  <- final value
 */
/**
 * sleep_with_cancel - Sleep for specified milliseconds with cancellation check
 * @ms: milliseconds to sleep
 * @cancel_flag: pointer to cancellation flag (can be NULL)
 *
 * Returns 0 if slept fully, -1 if cancelled
 */
static int sleep_with_cancel(int ms, volatile sig_atomic_t *cancel_flag) {
    int slept = 0;
    const int chunk = 100; /* 100ms chunks */
    while (slept < ms) {
        if (cancel_flag && *cancel_flag) return -1;
        int to_sleep = (ms - slept > chunk) ? chunk : (ms - slept);
        usleep(to_sleep * 1000);
        slept += to_sleep;
    }
    return 0;
}

static void step(int time, int time2, char *text, bool (*cond)(uint64_t), uint64_t cond_arg) {
    if (time <= time2) {
        /* Invalid parameters: just display final value */
        printf(
            PALERAIN_OPTION_TEST(palerain_flags, palerain_option_no_colors)
            ? "\r%s (%d)"
            : CYN "\r%s (%d)" CRESET, text, time2
        );
        if (time2 == 0) puts("");
        return;
    }

    /* Count down from time to time2 + 1 */
    for (int remaining = time; remaining > time2; remaining--) {
        printf(
            PALERAIN_OPTION_TEST(palerain_flags, palerain_option_no_colors)
            ? "\r\033[K%s (%d)"
            : BCYN "\r\033[K%s (%d)" CRESET, text, remaining
        );
        fflush(stdout);
        /* Sleep 1 second with cancellation check */
        if (sleep_with_cancel(1000, &dfuhelper_cancel) < 0) {
            pthread_exit(NULL);
        }
        if (cond != NULL && cond(cond_arg)) pthread_exit(NULL);
    }
    /* Display final value */
    printf(
        PALERAIN_OPTION_TEST(palerain_flags, palerain_option_no_colors)
        ? "\r%s (%d)"
        : CYN "\r%s (%d)" CRESET, text, time2
    );
    if (time2 == 0) puts("");
}

static int connected_normal_mode(const usbmuxd_device_info_t *usbmuxd_device) {
	devinfo_t dev;
	int ret;
	ret = devinfo_cmd(&dev, usbmuxd_device->udid);
	if (ret != 0) {
		LOG(LOG_ERROR, "Unable to get device information");
		return 0;
	}
if (!dev.CPUArchitecture ||
    (strcmp(dev.CPUArchitecture, "arm64") &&
     strcmp(dev.CPUArchitecture, "arm64e"))) {
    devinfo_free(&dev);
    LOG(LOG_WARNING,
        "Unsupported architecture: %s",
        dev.CPUArchitecture ? dev.CPUArchitecture : "(null)");
    return -1;
}

	if (PALERAIN_OPTION_TEST(palerain_flags, palerain_option_device_info)) {
		printf("Mode: normal\n");
		printf("ProductType: %s\n", dev.productType);
		printf("Architecture: %s\n", dev.CPUArchitecture);
		printf("Version: %s\n", dev.productVersion);
		printf("DisplayName: %s\n", dev.displayName);

		set_device_has_booted(true);
		set_spin(0);
		unsubscribe_cmd();
		return 0;
	} else {
		LOG(LOG_VERBOSE2, "Entering recovery mode");
	}
	enter_recovery_cmd(usbmuxd_device->udid);
	devinfo_free(&dev);
	if (PALERAIN_OPTION_TEST(palerain_flags, palerain_option_enter_recovery)) {
		set_device_has_booted(true);
		set_spin(0);
		unsubscribe_cmd();
	}
	return 0;
}

static bool conditional(uint64_t ecid) {
	return get_ecid_wait_for_dfu() != ecid;
}

static void* connected_recovery_mode(struct irecv_device_info* info) {
	/* Validate irecv device info */
	if (!info) {
		LOG(LOG_ERROR, "connected_recovery_mode: NULL device info");
		return NULL;
	}
	if (info->ecid == 0) {
		LOG(LOG_ERROR, "connected_recovery_mode: invalid ECID");
		return NULL;
	}
	if (info->cpid == 0) {
		LOG(LOG_ERROR, "connected_recovery_mode: invalid CPID");
		return NULL;
	}

	int ret;
	uint64_t ecid;
	uint32_t cpid, bdid;
	cpid = info->cpid;
	ecid = info->ecid;
	bdid = info->bdid;
	info = NULL;
	if (!cpid_is_arm64(cpid)) {
		LOG(LOG_WARNING, "Ignoring non-arm64 device...");
		return NULL;
	}
	sleep(1);
	ret = autoboot_cmd(ecid);
	if (ret) {
		LOG(LOG_ERROR, "Cannot set auto-boot back to true");
		return NULL;
	}
#if !defined(DFUHELPER_AUTO_ONLY)
	if (IS_APPLE_TV_4K) {
		LOG(LOG_INFO, "Depending on your connection method, you might need to press a button on your cable/board during reboot");
	}
	LOG(LOG_INFO, "Press Enter when ready for DFU mode (or wait 30 seconds for auto-continue)");
	/* Non-blocking input with 30 second timeout - check in 100ms chunks */
	fd_set fds;
	struct timeval tv;
	FD_ZERO(&fds);
	FD_SET(STDIN_FILENO, &fds);
	for (int i = 0; i < 300; i++) { /* 300 * 100ms = 30 seconds */
		if (dfuhelper_cancel) pthread_exit(NULL);
		tv.tv_sec = 0;
		tv.tv_usec = 100000; /* 100ms */
		if (select(STDIN_FILENO + 1, &fds, NULL, NULL, &tv) > 0) {
			/* Consume the input */
			while (getchar() != '\n' && !feof(stdin));
			break;
		}
	}
#endif
	if (IS_APPLETV) {
		if (IS_APPLE_TV_HD) {
			step(10, 8, "Hold menu + play button", NULL, 0);
			set_ecid_wait_for_dfu(ecid);
			ret = exitrecv_cmd(ecid);
			if (ret) {
				LOG(LOG_ERROR, "Cannot exit recovery mode");
				set_ecid_wait_for_dfu(0);
				return NULL;
			}
			printf("\r\033[K");
			step(8, 0, "Hold menu + play button", conditional, ecid);
		} else if (IS_APPLE_TV_4K) {
			step(2, 0, "About to reboot device", NULL, 0);
			set_ecid_wait_for_dfu(ecid);
			ret = exitrecv_cmd(ecid);
			if (ret) {
				LOG(LOG_ERROR, "Cannot exit recovery mode");
				set_ecid_wait_for_dfu(0);
				return NULL;
			}
			step(4, 0, "Waiting for device to reconnect in DFU mode", conditional, ecid);
		} else if (IS_APPLE_HOME) {
			step(6, 4, "Put device in upside down orientation", NULL, 0);
			set_ecid_wait_for_dfu(ecid);
			ret = exitrecv_cmd(ecid);
			if (ret) {
				LOG(LOG_ERROR, "Cannot exit recovery mode");
				set_ecid_wait_for_dfu(0);
				return NULL;
			}
			step(4, 0, "Put device upside down orientation", conditional, ecid);
		}
	} else if (cpid != 0x8012) {
		if (NO_PHYSICAL_HOME_BUTTON)
			step(4, 2, "Hold volume down + side button", NULL, 0);
		else
			step(4, 2, "Hold home + power button", NULL, 0);
		set_ecid_wait_for_dfu(ecid);
		ret = exitrecv_cmd(ecid);
		if (ret) {
			LOG(LOG_ERROR, "Cannot exit recovery mode");
			set_ecid_wait_for_dfu(0);
			return NULL;
		}
		printf("\r\033[K");
		if (NO_PHYSICAL_HOME_BUTTON) {
			step(2, 0, "Hold volume down + side button", NULL, 0);
			step(10, 0, "Hold volume down button", conditional, ecid);
		} else {
			step(2, 0, "Hold home + power button", NULL, 0);
			step(10, 0, "Hold home button", conditional, ecid);
		}
	}
	if (get_ecid_wait_for_dfu() == ecid) {
		LOG(LOG_WARNING, "Whoops, device did not enter DFU mode");
		LOG(LOG_INFO, "Waiting for device to reconnect...");
		set_ecid_wait_for_dfu(0);
		return NULL;
	}
	set_ecid_wait_for_dfu(0);
	pthread_exit(NULL);
	return NULL;
}

static void* connected_dfu_mode(struct irecv_device_info* info) {
	/* Validate irecv device info */
	if (!info) {
		LOG(LOG_ERROR, "connected_dfu_mode: NULL device info");
		return NULL;
	}
	if (info->ecid == 0) {
		LOG(LOG_ERROR, "connected_dfu_mode: invalid ECID");
		return NULL;
	}
	if (info->cpid == 0) {
		LOG(LOG_ERROR, "connected_dfu_mode: invalid CPID");
		return NULL;
	}

	if (get_ecid_wait_for_dfu() == info->ecid) {
		set_ecid_wait_for_dfu(0);
		puts("");
		LOG(LOG_INFO, "Device entered DFU mode successfully");
	}
	unsigned int bdid = info->bdid;
	unsigned int cpid = info->cpid;

	/* Check for pwned DFU mode if requested. This only confirms state;
	 * the normal Pongo pipeline remains responsible for boot handling. */
	if (PALERAIN_OPTION_TEST(palerain_flags, palerain_option_pwned_dfu)) {
		irecv_client_t device = NULL;
		LOG(LOG_INFO, "Checking for pwned DFU mode...");
		if (irecv_open_with_ecid(&device, info->ecid) == IRECV_E_SUCCESS) {
			const struct irecv_device_info *dev_info = irecv_get_device_info(device);
			if (dev_info && dev_info->serial_string && strstr(dev_info->serial_string, "PWND:[")) {
				LOG(LOG_INFO, "Pwned DFU detected: %s", dev_info->serial_string);

				if (cpid_needs_usbliter8(cpid)) {
					LOG(LOG_INFO, "usbliter8 device found! executing iBoot commands");
					if (device) irecv_close(device);
					device = NULL;

					/* Determine PongoOS path: use command-line override if provided,
					 * otherwise extract embedded PongoOS to a temporary file */
					char *boot_path = pongo_path;
					char *temp_pongo = NULL;
					if (!boot_path) {
						temp_pongo = get_embedded_pongo_path();
						boot_path = temp_pongo;
					}
					int boot_ret = -1;
					if (!boot_path) {
						LOG(LOG_ERROR, "No PongoOS image available");
					} else {
						boot_ret = usbliter8_boot_file(boot_path);
						/* Clean up temporary embedded PongoOS file on success */
						if (boot_ret == 0 && temp_pongo) {
							unlink(temp_pongo);
							free(temp_pongo);
							temp_pongo = NULL;
						}
					}
					if (boot_ret != 0) {
						LOG(LOG_ERROR, "usbliter8_boot_file() failed (%d)", boot_ret);
						/* Clean up temporary embedded PongoOS file on failure */
						if (temp_pongo) {
							unlink(temp_pongo);
							free(temp_pongo);
						}
					} else {
						LOG(LOG_INFO, "PongoOS sent, waiting for pongo_thread to complete boot process");
						/* Don't set device_has_booted or exit here - the pongo_thread will be
						 * created by main() after dfuhelper completes, and it will wait for
						 * PongoOS to re-enumerate via hotplug, then send KPF/ramdisk/overlay/bootx.
						 * We just return NULL to let this dfu_thread exit, and dfuhelper will
						 * continue monitoring for the PongoOS device. */
						/* Signal that we're waiting for PongoOS to boot */
						set_spin(1);
					}
				} else {
					LOG(LOG_WARNING, "Pwned DFU detected but cpid=0x%04x has no usbliter8 handler", cpid);
				}
			} else {
				LOG(LOG_WARNING, "Pwned DFU not detected, using normal flow");
				PALERAIN_OPTION_CLEAR(palerain_flags, palerain_option_pwned_dfu);
			}
			if (device) irecv_close(device);
		} else {
			LOG(LOG_WARNING, "Could not open DFU device");
			PALERAIN_OPTION_CLEAR(palerain_flags, palerain_option_pwned_dfu);
		}
	}

	if (IS_APPLE_HOME) {
		step(2, 0, "Put device in upright orientation", NULL, 0);
	}
	set_spin(0);
	unsubscribe_cmd();
	pthread_exit(NULL);
	return NULL;
}

static void device_event_cb(const usbmuxd_event_t *event, void* userdata) {
	if (event->device.conn_type != CONNECTION_TYPE_USB) return;
	switch (event->event) {
	case UE_DEVICE_ADD:
		LOG(LOG_VERBOSE, "Normal mode device connected");
		if (PALERAIN_OPTION_TEST(palerain_flags, palerain_option_exit_recovery)) {
			break;
		} else if (PALERAIN_OPTION_TEST(palerain_flags, palerain_option_reboot_device)) {
			int ret = reboot_cmd(event->device.udid);
			if (!ret) {
				LOG(LOG_INFO, "Restarted device");
				set_spin(0);
				unsubscribe_cmd();
			}
			pthread_exit(NULL);
			break;
		}
		connected_normal_mode(&event->device);
		break;
	case UE_DEVICE_REMOVE:
		LOG(LOG_VERBOSE, "Normal mode device disconnected");
		break;
	}
}

static void irecv_device_event_cb(const irecv_device_event_t *event, void* userdata) {
	pthread_t recovery_thread, dfu_thread;
	int ret;
	
	switch(event->type) {
		case IRECV_DEVICE_ADD:
			if (event->mode == IRECV_K_RECOVERY_MODE_1 || 
				event->mode == IRECV_K_RECOVERY_MODE_2 || 
				event->mode == IRECV_K_RECOVERY_MODE_3 || 
				event->mode == IRECV_K_RECOVERY_MODE_4) {
				if (!PALERAIN_OPTION_TEST(palerain_flags, palerain_option_device_info))
					LOG(LOG_VERBOSE, "Recovery mode device %" PRIu64 " connected", event->device_info->ecid);
				if (PALERAIN_OPTION_TEST(palerain_flags, palerain_option_exit_recovery)) {
					ret = exitrecv_cmd(event->device_info->ecid);
					if (!ret) {
						LOG(LOG_INFO, "Exited recovery mode");
						set_device_has_booted(true);
						set_spin(0);
						unsubscribe_cmd();
					} else {
						LOG(LOG_WARNING, "Could not exit recovery mode");
					}
					if (get_dfuhelper_thr_running()) pthread_cancel(dfuhelper_thread);
					pthread_exit(NULL);
					break;
				}

				if (PALERAIN_OPTION_TEST(palerain_flags, palerain_option_device_info)) {
					recvinfo_t info;
					ret = recvinfo_cmd(&info, event->device_info->ecid);
					if (ret) {
						LOG(LOG_WARNING, "Could not get info from device");
					} else {
						printf("Mode: Recovery\n");
						printf("ProductType: %s\n", info.product_type);
						printf("DisplayName: %s\n", info.display_name);

						set_device_has_booted(true);
						set_spin(0);
						unsubscribe_cmd();
					}
					if (get_dfuhelper_thr_running()) pthread_cancel(dfuhelper_thread);
					pthread_exit(NULL);
					break;
				}

				if (PALERAIN_OPTION_TEST(palerain_flags, palerain_option_enter_recovery) ||
					PALERAIN_OPTION_TEST(palerain_flags, palerain_option_reboot_device)) return;
				pthread_create(&recovery_thread, NULL, (pthread_start_t)connected_recovery_mode, event->device_info);
			} else if (event->mode == IRECV_K_DFU_MODE) {
				if (!PALERAIN_OPTION_TEST(palerain_flags, palerain_option_device_info))
					LOG(LOG_VERBOSE, "DFU mode device %" PRIu64 " connected", event->device_info->ecid);

				if (PALERAIN_OPTION_TEST(palerain_flags, palerain_option_device_info)) {
					recvinfo_t info;
					ret = recvinfo_cmd(&info, event->device_info->ecid);
					if (ret) {
						LOG(LOG_WARNING, "Could not get info from device");
					} else {
						printf("Mode: DFU\n");
						printf("ProductType: %s\n", info.product_type);
						printf("DisplayName: %s\n", info.display_name);

						set_device_has_booted(true);
						set_spin(0);
						unsubscribe_cmd();
					}
					if (get_dfuhelper_thr_running()) pthread_cancel(dfuhelper_thread);
					pthread_exit(NULL);
					break;
				}

				if (
					PALERAIN_OPTION_TEST(palerain_flags, palerain_option_exit_recovery) ||
					PALERAIN_OPTION_TEST(palerain_flags, palerain_option_enter_recovery) ||
					PALERAIN_OPTION_TEST(palerain_flags, palerain_option_reboot_device)) {
						break;
				}
				pthread_create(&dfu_thread, NULL, (pthread_start_t)connected_dfu_mode, event->device_info);
			}
			break;
		case IRECV_DEVICE_REMOVE:
			LOG(LOG_VERBOSE, "Recovery mode device disconnected");
		break;
	}
}

void *dfuhelper(void* ptr) {
	set_dfuhelper_thr_running(true);
	set_spin(1);
	subscribe_cmd(device_event_cb, irecv_device_event_cb);
	while (get_spin() && !dfuhelper_cancel) {
		sleep(1);
	};
	/* Unsubscribe on exit (whether normal or cancelled) */
	unsubscribe_cmd();
	set_dfuhelper_thr_running(false);
	return 0;
}

void dfuhelper_request_cancel(void) {
	dfuhelper_cancel = 1;
}
