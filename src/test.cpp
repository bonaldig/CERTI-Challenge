#include <stdio.h>
#include <stdlib.h>

#include "libusb-1.0/libusb.h"

int done = 0;
libusb_device_handle *handle = NULL;

struct libusb_device_descriptor desc;

static int LIBUSB_CALL hotplug_callback(libusb_context *ctx, libusb_device *dev,
                                        libusb_hotplug_event event, void *user_data) {
  int rc;

  (void)ctx;
  (void)dev;
  (void)event;
  (void)user_data;

  rc = libusb_get_device_descriptor(dev, &desc);
  if (LIBUSB_SUCCESS != rc) {
    fprintf(stderr, "Error getting device descriptor\n");
  }

  printf("Device attached: %04x:%04x\n", desc.idVendor, desc.idProduct);

  if (handle) {
    libusb_close(handle);
    handle = NULL;
  }

  done++;

  return 0;
}

static int LIBUSB_CALL hotplug_callback_detach(libusb_context *ctx, libusb_device *dev,
                                               libusb_hotplug_event event, void *user_data) {
  int rc;

  (void)ctx;
  (void)dev;
  (void)event;
  (void)user_data;

  rc = libusb_get_device_descriptor(dev, &desc);
  if (LIBUSB_SUCCESS != rc) {
    fprintf(stderr, "Error getting device descriptor\n");
  }

  printf("Device detached: %04x:%04x\n", desc.idVendor, desc.idProduct);

  if (handle) {
    libusb_close(handle);
    handle = NULL;
  }

  done++;

  return 0;
}

int main(int argc, char *argv[]) {
  libusb_hotplug_callback_handle hp[2];
  int product_id, vendor_id, class_id;
  int rc;

  vendor_id = (argc > 1) ? (int)strtol(argv[1], NULL, 0) : 0x0951;
  product_id = (argc > 2) ? (int)strtol(argv[2], NULL, 0) : 0x170b;
  class_id = (argc > 3) ? (int)strtol(argv[3], NULL, 0) : LIBUSB_HOTPLUG_MATCH_ANY;

  rc = libusb_init(NULL);
  if (rc < 0) {
    printf("failed to initialise libusb: %s\n", libusb_error_name(rc));
    return EXIT_FAILURE;
  }

  if (!libusb_has_capability(LIBUSB_CAP_HAS_HOTPLUG)) {
    printf("Hotplug capabilities are not supported on this platform\n");
    libusb_exit(NULL);
    return EXIT_FAILURE;
  }

  rc = libusb_hotplug_register_callback(NULL, LIBUSB_HOTPLUG_EVENT_DEVICE_ARRIVED,
                                        LIBUSB_HOTPLUG_NO_FLAGS, vendor_id, product_id, class_id,
                                        hotplug_callback, NULL, &hp[0]);
  if (LIBUSB_SUCCESS != rc) {
    fprintf(stderr, "Error registering callback 0\n");
    libusb_exit(NULL);
    return EXIT_FAILURE;
  }

  rc = libusb_hotplug_register_callback(NULL, LIBUSB_HOTPLUG_EVENT_DEVICE_LEFT,
                                        LIBUSB_HOTPLUG_NO_FLAGS, vendor_id, product_id, class_id,
                                        hotplug_callback_detach, NULL, &hp[1]);
  if (LIBUSB_SUCCESS != rc) {
    fprintf(stderr, "Error registering callback 1\n");
    libusb_exit(NULL);
    return EXIT_FAILURE;
  }

  while (done < 2) {
    rc = libusb_handle_events(NULL);
    if (rc < 0) printf("libusb_handle_events() failed: %s\n", libusb_error_name(rc));
  }

  if (handle) {
    libusb_close(handle);
  }

  libusb_exit(NULL);

  return EXIT_SUCCESS;
}