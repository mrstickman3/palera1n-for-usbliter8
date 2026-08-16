#include <stdio.h>
#include <libusb.h>

int main(void) {
    libusb_context *ctx = NULL;

    /* try UsbDk backend first — works without replacing device drivers */
#ifdef LIBUSB_OPTION_USE_USBDK
    libusb_set_option(NULL, LIBUSB_OPTION_USE_USBDK);
    printf("UsbDk option set\n");
#else
    printf("UsbDk option not compiled in\n");
#endif

    libusb_init(&ctx);

    libusb_device **list;
    ssize_t cnt = libusb_get_device_list(ctx, &list);
    printf("libusb sees %zd devices\n", cnt);

    for (ssize_t i = 0; i < cnt; i++) {
        struct libusb_device_descriptor d;
        libusb_get_device_descriptor(list[i], &d);
        if (d.idVendor == 0x05ac && d.idProduct == 0x1227) {
            printf("Found 05ac:1227 — trying to open...\n");
            libusb_device_handle *h = NULL;
            int r = libusb_open(list[i], &h);
            if (r < 0) {
                printf("Open failed: %s (%d)\n", libusb_error_name(r), r);
            } else {
                unsigned char serial[256] = {0};
                libusb_get_string_descriptor_ascii(h, d.iSerialNumber, serial, sizeof(serial)-1);
                printf("Opened OK! Serial: %s\n", serial);
                libusb_close(h);
            }
        }
    }
    libusb_free_device_list(list, 1);
    libusb_exit(ctx);
    return 0;
}
