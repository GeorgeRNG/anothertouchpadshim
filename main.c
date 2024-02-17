#include <assert.h>
#include <errno.h>
#include <fcntl.h>
#include <libevdev-1.0/libevdev/libevdev-uinput.h>
#include <libevdev-1.0/libevdev/libevdev.h>
#include <linux/input-event-codes.h>
#include <linux/input.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/ioctl.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>
#include <stdbool.h> 

#include <libevdev/libevdev-uinput.h>
#include <libevdev/libevdev.h>

int main() {
  // Open the event device.
  struct libevdev *rdev = NULL;
  int fd;
  int rc = 1;

  fd = open("/dev/input/event7", O_RDONLY);
  rc = libevdev_new_from_fd(fd, &rdev);
  if (rc < 0) {
    fprintf(stderr, "Failed to init libevdev (%s)\n", strerror(-rc));
    exit(1);
  }
  printf("Input device name: \"%s\"\n", libevdev_get_name(rdev));
  printf("Input device ID: bus %#x vendor %#x product %#x\n",
         libevdev_get_id_bustype(rdev), libevdev_get_id_vendor(rdev),
         libevdev_get_id_product(rdev));

  if (!libevdev_has_event_type(rdev, EV_ABS)) {
    printf("Device missing EV_ABS event type.\n");
    exit(1);
  }
  if (!libevdev_has_event_code(rdev, EV_ABS, ABS_MT_TRACKING_ID)) {
    printf("Deive missing ABS_MT_TRACKING_ID event code.\n");
  }
  // Done that.

  // Create an output device.
  int err;
  struct libevdev *wdev;
  struct libevdev_uinput *uidev;
  wdev = libevdev_new();
  libevdev_set_name(wdev, "test device");
  libevdev_enable_event_type(wdev, EV_REL);
  libevdev_enable_event_code(wdev, EV_REL, REL_X, NULL);
  libevdev_enable_event_code(wdev, EV_REL, REL_Y, NULL);
  libevdev_enable_event_type(wdev, EV_KEY);
  libevdev_enable_event_code(wdev, EV_KEY, BTN_LEFT, NULL);
  libevdev_enable_event_code(wdev, EV_KEY, BTN_MIDDLE, NULL);
  libevdev_enable_event_code(wdev, EV_KEY, BTN_RIGHT, NULL);
  err = libevdev_uinput_create_from_device(wdev, LIBEVDEV_UINPUT_OPEN_MANAGED,
                                           &uidev);
  if (err != 0)
    return err;
  // Done that.

  // Read from the device.
  bool left = false;
  bool right = false;
  int fingers = 0;
  do {
    struct input_event ev;
    rc = libevdev_next_event(rdev, LIBEVDEV_READ_FLAG_NORMAL, &ev);
    if (rc == 0) {
      if (ev.type != EV_ABS)
        continue;
      if (ev.code != ABS_MT_TRACKING_ID)
        continue;
      if (ev.value == -1)
        fingers -= 1;
      else
        fingers += 1;
      printf("fingers: %d\n", fingers);

      // Make our events.
      if (fingers == 0) {
        if(left) {
          libevdev_uinput_write_event(uidev, EV_KEY, BTN_LEFT, 0);
          left = false;
        }
        libevdev_uinput_write_event(uidev, EV_SYN, SYN_REPORT, 0);
      }
      if (fingers == 1) {
        if(!left) {
          libevdev_uinput_write_event(uidev, EV_KEY, BTN_LEFT, 1);
          left = true;
        }
        if(right) {
          libevdev_uinput_write_event(uidev, EV_KEY, BTN_RIGHT, 0);
          right = false;
        }
        libevdev_uinput_write_event(uidev, EV_SYN, SYN_REPORT, 0);
      }
      if (fingers == 2) {
        if(!right) {
          libevdev_uinput_write_event(uidev, EV_KEY, BTN_RIGHT, 1);
          right = true;
        }
        libevdev_uinput_write_event(uidev, EV_SYN, SYN_REPORT, 0);
      }
    }
  } while (rc == 1 || rc == 0 || rc == -EAGAIN);

  libevdev_uinput_destroy(uidev);
  libevdev_free(rdev);
  close(fd);
  return 0;
}