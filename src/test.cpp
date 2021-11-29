#include <libusb-1.0/libusb.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>

#include <thread>

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

static void vendor_monitor(libusb_device **devs){};

int main(void) {
  int product_id, vendor_id, class_id;
  int rc;
  int j = 0;
  ssize_t cnt;

  libusb_device **devs;
  libusb_hotplug_callback_handle hp[2];
  libusb_device *dev;

  rc = libusb_init(NULL);
  if (rc < 0) {
    printf("failed to initialise libusb: %s\n", libusb_error_name(rc));
    return EXIT_FAILURE;
  }

  cnt = libusb_get_device_list(NULL, &devs);
  if (cnt < 0) {
    printf("failed to get device list. \n");
    return EXIT_FAILURE;
  }

  int i = 0;
  while ((dev = devs[i++]) != NULL) {
    struct libusb_device_descriptor desc;
    int r = libusb_get_device_descriptor(dev, &desc);
    if (r < 0) {
      fprintf(stderr, "failed to get device descriptor");
      return EXIT_FAILURE;
    }
    // printf("%04x:%04x (bus %d, device %d)\n", desc.idVendor, desc.idProduct,
    //        libusb_get_bus_number(dev), libusb_get_device_address(dev));

    vendor_id = desc.idVendor;
    product_id = desc.idProduct;
    class_id = LIBUSB_HOTPLUG_MATCH_ANY;

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
  }
  while (done < 2) {
    rc = libusb_handle_events(NULL);
    if (rc < 0) printf("libusb_handle_events() failed: %s\n", libusb_error_name(rc));
  }
  if (handle) {
    libusb_close(handle);
  }
  libusb_free_device_list(devs, 1);
}