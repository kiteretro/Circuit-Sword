/*
 * giles test shift in gamepad
 */

 /*
  * Add udev rule (will overwrite if present)
	* echo "SUBSYSTEM==\"input\", ATTRS{name}==\"circuitsword\", ENV{ID_INPUT_KEYBOARD}=\"1\"" > /etc/udev/rules.d/10-circuitsword.rules
  */

#include <stdio.h>
#include "shift_in.h"

#include </usr/include/linux/input.h>
#include <linux/input.h>
#include <linux/uinput.h>

#include <stdbool.h>
#include <stdlib.h>
#include <inttypes.h>
#include <stdint.h>
#include <string.h>

#include <unistd.h>
#include <ctype.h>
#include <fcntl.h>
#include <poll.h>
#include <signal.h>
#include <dirent.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <sys/signalfd.h>
#include <sys/inotify.h>

#define PIN_DATA  22
#define PIN_LATCH 27
#define PIN_CLOCK 23

#define DATA_LENGTH 3

//-----------------------------------------------------------------------------

typedef struct {
  char *name;
  int   value;
  uint32_t mask;
  bool pressed;
} KB_KEY_T;

#define NUMBER_OF_KEYS 20
#define IDLE 0

uint32_t button_mask    = 0b111100001111111111111111;
uint32_t aux_mask       = 0b000011110000000000000000;

uint32_t mode_mask      = 0b000010000000000000000000;
uint32_t ext_mask       = 0b000001000000000000000000;
uint32_t pgood_mask     = 0b000000100000000000000000;
uint32_t chrg_mask      = 0b000000010000000000000000;

KB_KEY_T kb_keys_map[NUMBER_OF_KEYS] =
{
  { "UP",      KEY_UP,    0b000000000000000000000001, IDLE },
  { "DOWN",    KEY_DOWN,  0b000000000000000000000100, IDLE },
  { "LEFT",    KEY_LEFT,  0b000000000000000000000010, IDLE },
  { "RIGHT",   KEY_RIGHT, 0b000000000000000000001000, IDLE },
  { "START",   KEY_ENTER, 0b000000000000000001000000, IDLE },
  { "SELECT",  KEY_ESC,   0b000000000000000010000000, IDLE },
  { "A",       KEY_Z,     0b000000000100000000000000, IDLE },
  { "B",       KEY_X,     0b000000000010000000000000, IDLE },
  { "X",       KEY_A,     0b000000001000000000000000, IDLE },
  { "Y",       KEY_S,     0b000000000001000000000000, IDLE },
  { "C1",      KEY_Q,     0b000000000000000000100000, IDLE },
  { "C2",      KEY_W,     0b000000000000000000010000, IDLE },
  { "L1",      KEY_1,     0b000000000000001000000000, IDLE },
  { "L2",      KEY_2,     0b000000000000010000000000, IDLE },
  { "R1",      KEY_3,     0b000000000000000100000000, IDLE },
  { "R2",      KEY_4,     0b000000000000100000000000, IDLE },
  { "J_UP",    KEY_I,     0b100000000000000000000000, IDLE },
  { "J_DOWN",  KEY_K,     0b010000000000000000000000, IDLE },
  { "J_LEFT",  KEY_J,     0b001000000000000000000000, IDLE },
  { "J_RIGHT", KEY_L,     0b000100000000000000000000, IDLE }
};

int fileWatch;   // inotify file descriptor
int keyfd1 = -1; // /dev/uinput file descriptor
int keyfd2 = -1; // /dev/input/eventX file descriptor
int keyfd  = -1; // = (keyfd2 >= 0) ? keyfd2 : keyfd1;

struct input_event keyEv, synEv; // uinput events

// Filter function for scandir(), identifies possible device candidates for
// simulated keypress events (distinct from actual USB keyboard(s)).
static int filter1(const struct dirent *d)
{
	if(!strncmp(d->d_name, "input", 5)) { // Name usu. 'input' + #
		// Read contents of 'name' file inside this subdirectory,
		// if it matches the retrogame executable, that's probably
		// the device we want...
		char  filename[100], line[100];
		FILE *fp;
		sprintf(filename, "/sys/devices/virtual/input/%s/name",
		  d->d_name);
		memset(line, 0, sizeof(line));
		if((fp = fopen(filename, "r"))) {
			fgets(line, sizeof(line), fp);
			fclose(fp);
		}
		//if(!strncmp(line, __progname, strlen(__progname))) return 1;
    if(!strncmp(line, "circuitsword", strlen("circuitsword"))) return 1;
	}
  return 0;
}

// A second scandir() filter, checks for filename of 'event' + #
static int filter2(const struct dirent *d)
{
	return !strncmp(d->d_name, "event", 5);
}

//-------------------------------------------------------------------------

void keyboardInit()
{
  // Set up uinput
  int i;
  char buf[50];

	// Attempt to create uidev virtual keyboard
	if((keyfd1 = open("/dev/uinput", O_WRONLY | O_NONBLOCK)) >= 0) {
		(void)ioctl(keyfd1, UI_SET_EVBIT, EV_KEY);

    for (i=0; i<NUMBER_OF_KEYS; i++) { //TODO
      if(kb_keys_map[i].value >= KEY_RESERVED) {
				(void)ioctl(keyfd1, UI_SET_KEYBIT, kb_keys_map[i].value);
      }
    }

    /*for(i=0; i<161; i++) {
			if((key[i] >= KEY_RESERVED) && (key[i] < GND))
				(void)ioctl(keyfd1, UI_SET_KEYBIT, key[i]);
		}*/

		struct uinput_user_dev uidev;
		memset(&uidev, 0, sizeof(uidev));
		snprintf(uidev.name, UINPUT_MAX_NAME_SIZE, "circuitsword");
		uidev.id.bustype = BUS_USB;
		uidev.id.vendor  = 0x1;
		uidev.id.product = 0x1;
		uidev.id.version = 1;
		if(write(keyfd1, &uidev, sizeof(uidev)) < 0) {
      perror("write failed");
      exit(EXIT_FAILURE);
    }
		if(ioctl(keyfd1, UI_DEV_CREATE) < 0) {
      perror("DEV_CREATE failed");
      exit(EXIT_FAILURE);
    }
		fprintf(stdout, "uidev init OK\n");
	}

	// SDL2 (used by some newer emulators) wants /dev/input/eventX
	// instead -- BUT -- this only exists if there's a physical USB
	// keyboard attached or if the above code has run and created a
	// virtual keyboard.  On older systems this method doesn't apply,
	// events can be sent to the keyfd1 virtual keyboard above...so,
	// this code looks for an eventX device and (if present) will use
	// that as the destination for events, else fallback on keyfd1.

	// The 'X' in eventX is a unique identifier (typically a numeric
	// digit or two) for each input device, dynamically assigned as
	// USB input devices are plugged in or disconnected (or when the
	// above code runs, creating a virtual keyboard).  As it's
	// dynamically assigned, we can't rely on a fixed number -- it
	// will vary if there's a keyboard connected at startup.

	struct dirent **namelist;
	int             n;
	char            evName[100] = "";

	if((n = scandir("/sys/devices/virtual/input", &namelist, filter1, NULL)) > 0) {
		// Got a list of device(s).  In theory there should
		// be only one that makes it through the filter (name
		// matches retrogame)...if there's multiples, only
		// the first is used.  (namelist can then be freed)
		char path[100];
		sprintf(path, "/sys/devices/virtual/input/%s", namelist[0]->d_name);
		for(i=0; i<n; i++) free(namelist[i]);
		free(namelist);
		// Within the given device path should be a subpath with
		// the name 'eventX' (X varies), again theoretically
		// should be only one, first in list is used.
		if((n = scandir(path, &namelist, filter2, NULL)) > 0) {
			sprintf(evName, "/dev/input/%s",
			  namelist[0]->d_name);
			for(i=0; i<n; i++) free(namelist[i]);
			free(namelist);
		}
	}

	if(!evName[0]) { // Nothing found?  Use fallback method...
		// Kinda lazy skim for last item in /dev/input/event*
		// This is NOT guaranteed to be retrogame, but if the
		// above method fails for some reason, this may be
		// adequate.  If there's a USB keyboard attached at
		// boot, it usually instantiates in /dev/input before
		// retrogame, so even if it's then removed, the index
		// assigned to retrogame stays put...thus the last
		// index mmmmight be what we need.
		struct stat st;
		for(i=99; i>=0; i--) {
			sprintf(buf, "/dev/input/event%d", i);
			if(!stat(buf, &st)) break; // last valid device
		}
		strcpy(evName, (i >= 0) ? buf : "/dev/input/event0");
	}

	keyfd2 = open(evName, O_WRONLY | O_NONBLOCK);
	keyfd  = (keyfd2 >= 0) ? keyfd2 : keyfd1;
	// keyfd1 and 2 are global and are held open (as a destination for
	// key events) until pinConfigUnload() is called.
	if(keyfd2) fprintf(stdout, "SDL2 init OK\n");


	memset(&keyEv    , 0, sizeof(keyEv));
	memset(&synEv    , 0, sizeof(synEv));
	keyEv.type = EV_KEY;
	synEv.type = EV_SYN;
	synEv.code = SYN_REPORT;
}

void keyboardUnload()
{
  // Close uinput file descriptors
	keyfd = -1;
	if(keyfd2 >= 0) {
		close(keyfd2);
		keyfd2 = -1;
	}
	if(keyfd1 >= 0) {
		ioctl(keyfd1, UI_DEV_DESTROY);
		close(keyfd1);
		keyfd1 = -1;
	}

  fprintf(stdout, "Closed keyboard\n");
}

void keyboard_press(int id)
{
  keyEv.code  = id;
  keyEv.value = 1;
  write(keyfd, &keyEv, sizeof(keyEv));
  write(keyfd, &synEv, sizeof(synEv));
}

void keyboard_release(int id)
{
  keyEv.code  = id;
  keyEv.value = 0;
  write(keyfd, &keyEv, sizeof(keyEv));
  write(keyfd, &synEv, sizeof(synEv));
}

void keyboard_button(int id)
{
  keyboard_press(id);
  usleep(1000);

  keyboard_release(id);
  usleep(1000);
}

//-----------------------------------------------------------------------------

// MAIN
int main(void)
{
  printf("Raspberry Pi shift in\n");

  if (wiringPiSetupGpio() == -1)
    return 1;

  shift_in_init(PIN_DATA, PIN_LATCH, PIN_CLOCK);

  keyboardInit();

  for (;;) {

    // Get data
    uint32_t res = shift_in_data_32(DATA_LENGTH);
    // printf("%#06x\n", res);

    // Set keys
    // uint8_t k = 0;
    // for (k = 0; k < NUMBER_OF_KEYS ; k++) {
    //   if (!(res & kb_keys_map[k].mask)) {
    //     printf("%s\n", kb_keys_map[k].name);
    //     keyboard_button(kb_keys_map[k].value);
    //   }
    // }

    // Check each key that is registered
    uint8_t k = 0;
    for (k = 0; k < NUMBER_OF_KEYS ; k++) {

      if (!(res & kb_keys_map[k].mask)) {
        // Key is pressed

        if (!kb_keys_map[k].pressed) {
          printf("%s PRESSED\n", kb_keys_map[k].name);
          keyboard_press(kb_keys_map[k].value);
          kb_keys_map[k].pressed = 1;
        } else {
          // printf("%s HELD\n", kb_keys_map[k].name);
        }

      } else if (kb_keys_map[k].pressed) {
        // Release the key

        // printf("%s RELEASED\n", kb_keys_map[k].name);
        keyboard_release(kb_keys_map[k].value);
        kb_keys_map[k].pressed = 0;

      }

    }

    delay(16);

  }

  keyboardUnload();

  return 0;
}
