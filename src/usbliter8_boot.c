/*
 * usbliter8_boot.c
 *
 * Minimal C port of the "boot" command from usbliter8ctl (Python/pyusb).
 * Based on AldazActivator/usbliter8-c, itself a port of prdgmshift/usbliter8.
 *
 * Sends a raw image (PongoOS / iBoot payload) to a device that is already
 * in "Pwned DFU" mode and triggers execution. Does NOT perform the
 * SecureROM exploit itself -- caller must confirm "PWND:[" is present
 * in the device's DFU serial string before calling.
 *
 * Build: link against libusb-1.0.
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <libusb.h>

#include "usbliter8_boot.h"

#define DFU_VID          0x05AC
#define DFU_PID          0x1227

#define DFU_DNLOAD       1
#define DFU_ABORT        6
#define CUSTOM_BOOT      8

#define TRANSFER_SIZE    0x800u
#define CTRL_TIMEOUT_MS  5000

#define REQTYPE_CLASS_OUT (LIBUSB_REQUEST_TYPE_CLASS | LIBUSB_RECIPIENT_INTERFACE | LIBUSB_ENDPOINT_OUT)

static libusb_device_handle *open_pwnd_device(void)
{
    libusb_device_handle *handle = libusb_open_device_with_vid_pid(NULL, DFU_VID, DFU_PID);
    if (!handle) {
        fprintf(stderr, "error: no DFU device found (%04x:%04x). "
                        "Is the device plugged in and in DFU mode?\n", DFU_VID, DFU_PID);
        return NULL;
    }

    libusb_set_auto_detach_kernel_driver(handle, 1);

    libusb_device *dev = libusb_get_device(handle);
    struct libusb_device_descriptor desc;
    unsigned char serial[256] = {0};

    if (libusb_get_device_descriptor(dev, &desc) == 0 && desc.iSerialNumber) {
        libusb_get_string_descriptor_ascii(handle, desc.iSerialNumber, serial, sizeof(serial) - 1);
    }

    if (!strstr((const char *)serial, "PWND:[")) {
        fprintf(stderr, "error: device is not in Pwned DFU mode\n");
        if (serial[0]) fprintf(stderr, "serial: %s\n", serial);
        libusb_close(handle);
        return NULL;
    }

    printf("Serial: %s\n", serial);
    return handle;
}

static int download_image(libusb_device_handle *h, const unsigned char *buf, size_t len)
{
    size_t offset = 0;
    while (offset < len) {
        size_t chunk = (len - offset < TRANSFER_SIZE) ? (len - offset) : TRANSFER_SIZE;
        int r = libusb_control_transfer(h, REQTYPE_CLASS_OUT, DFU_DNLOAD, 0, 0,
                                         (unsigned char *)(buf + offset), (uint16_t)chunk,
                                         CTRL_TIMEOUT_MS);
        if (r < 0) {
            fprintf(stderr, "\nerror: DFU_DNLOAD failed at offset 0x%zx: %s\n",
                    offset, libusb_error_name(r));
            return -1;
        }
        offset += chunk;
        printf("\rsent 0x%zx / 0x%zx", offset, len);
        fflush(stdout);
    }
    printf("\n");

    int r = libusb_control_transfer(h, REQTYPE_CLASS_OUT, DFU_DNLOAD, 0, 0, NULL, 0, 1000);
    if (r < 0) {
        fprintf(stderr, "warning: closing DFU_DNLOAD failed: %s\n", libusb_error_name(r));
    }
    return 0;
}

int usbliter8_boot(const unsigned char *buf, size_t len)
{
    if (!buf || len == 0) {
        fprintf(stderr, "error: empty image buffer\n");
        return -1;
    }

    int r = libusb_init(NULL);
    if (r < 0) {
        fprintf(stderr, "error: libusb_init failed: %s\n", libusb_error_name(r));
        return -1;
    }

    libusb_device_handle *handle = open_pwnd_device();
    if (!handle) { libusb_exit(NULL); return -1; }

    r = libusb_claim_interface(handle, 0);
    if (r < 0) {
        fprintf(stderr, "error: could not claim interface 0: %s\n", libusb_error_name(r));
        libusb_close(handle);
        libusb_exit(NULL);
        return -1;
    }

    int rc = download_image(handle, buf, len);
    if (rc == 0) {
        r = libusb_control_transfer(handle, REQTYPE_CLASS_OUT, CUSTOM_BOOT, 0, 0, NULL, 0, 1000);
        if (r < 0) {
            fprintf(stderr, "warning: CUSTOM_BOOT failed: %s\n", libusb_error_name(r));
        }
        libusb_control_transfer(handle, REQTYPE_CLASS_OUT, DFU_ABORT, 0, 0, NULL, 0, 1000);
        printf("Boot triggered\n");
    }

    libusb_release_interface(handle, 0);
    libusb_close(handle);
    libusb_exit(NULL);

    return rc;
}

int usbliter8_boot_file(const char *path)
{
    FILE *f = fopen(path, "rb");
    if (!f) { perror("fopen"); return -1; }

    fseek(f, 0, SEEK_END);
    long size = ftell(f);
    fseek(f, 0, SEEK_SET);
    if (size <= 0) { fprintf(stderr, "error: empty or unreadable file\n"); fclose(f); return -1; }

    unsigned char *buf = malloc((size_t)size);
    if (!buf) { fclose(f); fprintf(stderr, "error: out of memory\n"); return -1; }

    if (fread(buf, 1, (size_t)size, f) != (size_t)size) {
        fprintf(stderr, "error: short read on %s\n", path);
        fclose(f); free(buf); return -1;
    }
    fclose(f);

    printf("Image: %s (%ld bytes)\n", path, size);
    int rc = usbliter8_boot(buf, (size_t)size);
    free(buf);
    return rc;
}
