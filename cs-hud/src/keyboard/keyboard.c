//-----------------------------------------------------------------------------
/*
 * The GPL v3 License
 *
 * Kite's Circuit Sword
 * Copyright (C) 2017 Giles Burgess (Kite's Item Shop)
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

/*
 * Add udev rule (will overwrite if present)
 * echo "SUBSYSTEM==\"input\", ATTRS{name}==\"circuitsword\", ENV{ID_INPUT_KEYBOARD}=\"1\"" > /etc/udev/rules.d/10-circuitsword.rules
 * SUBSYSTEM=="input", ATTRS{name}=="circuitsword", ENV{ID_INPUT_KEYBOARD}="1"
 */

 //----------------------------------------------------------------------------

#include "keyboard.h"

//-----------------------------------------------------------------------------
// PRIVATE VARIABLES

int fileWatch;   // inotify file descriptor
int keyfd1 = -1; // /dev/uinput file descriptor
int keyfd2 = -1; // /dev/input/eventX file descriptor
int keyfd  = -1; // = (keyfd2 >= 0) ? keyfd2 : keyfd1;

struct input_event keyEv, synEv; // uinput events

dict keyTable[113] =
{
  { "RESERVED", KEY_RESERVED },
  { "ESC", KEY_ESC },
  { "1", KEY_1 },
  { "2", KEY_2 },
  { "3", KEY_3 },
  { "4", KEY_4 },
  { "5", KEY_5 },
  { "6", KEY_6 },
  { "7", KEY_7 },
  { "8", KEY_8 },
  { "9", KEY_9 },
  { "0", KEY_0 },
  { "MINUS", KEY_MINUS },
  { "EQUAL", KEY_EQUAL },
  { "BACKSPACE", KEY_BACKSPACE },
  { "TAB", KEY_TAB },
  { "Q", KEY_Q },
  { "W", KEY_W },
  { "E", KEY_E },
  { "R", KEY_R },
  { "T", KEY_T },
  { "Y", KEY_Y },
  { "U", KEY_U },
  { "I", KEY_I },
  { "O", KEY_O },
  { "P", KEY_P },
  { "LEFTBRACE", KEY_LEFTBRACE },
  { "RIGHTBRACE", KEY_RIGHTBRACE },
  { "ENTER", KEY_ENTER },
  { "LEFTCTRL", KEY_LEFTCTRL },
  { "A", KEY_A },
  { "S", KEY_S },
  { "D", KEY_D },
  { "F", KEY_F },
  { "G", KEY_G },
  { "H", KEY_H },
  { "J", KEY_J },
  { "K", KEY_K },
  { "L", KEY_L },
  { "SEMICOLON", KEY_SEMICOLON },
  { "APOSTROPHE", KEY_APOSTROPHE },
  { "GRAVE", KEY_GRAVE },
  { "LEFTSHIFT", KEY_LEFTSHIFT },
  { "BACKSLASH", KEY_BACKSLASH },
  { "Z", KEY_Z },
  { "X", KEY_X },
  { "C", KEY_C },
  { "V", KEY_V },
  { "B", KEY_B },
  { "N", KEY_N },
  { "M", KEY_M },
  { "COMMA", KEY_COMMA },
  { "DOT", KEY_DOT },
  { "SLASH", KEY_SLASH },
  { "RIGHTSHIFT", KEY_RIGHTSHIFT },
  { "KPASTERISK", KEY_KPASTERISK },
  { "LEFTALT", KEY_LEFTALT },
  { "SPACE", KEY_SPACE },
  { "CAPSLOCK", KEY_CAPSLOCK },
  { "F1", KEY_F1 },
  { "F2", KEY_F2 },
  { "F3", KEY_F3 },
  { "F4", KEY_F4 },
  { "F5", KEY_F5 },
  { "F6", KEY_F6 },
  { "F7", KEY_F7 },
  { "F8", KEY_F8 },
  { "F9", KEY_F9 },
  { "F10", KEY_F10 },
  { "NUMLOCK", KEY_NUMLOCK },
  { "SCROLLLOCK", KEY_SCROLLLOCK },
  { "KP7", KEY_KP7 },
  { "KP8", KEY_KP8 },
  { "KP9", KEY_KP9 },
  { "KPMINUS", KEY_KPMINUS },
  { "KP4", KEY_KP4 },
  { "KP5", KEY_KP5 },
  { "KP6", KEY_KP6 },
  { "KPPLUS", KEY_KPPLUS },
  { "KP1", KEY_KP1 },
  { "KP2", KEY_KP2 },
  { "KP3", KEY_KP3 },
  { "KP0", KEY_KP0 },
  { "KPDOT", KEY_KPDOT },
  { "ZENKAKUHANKAKU", KEY_ZENKAKUHANKAKU },
  { "102ND", KEY_102ND },
  { "F11", KEY_F11 },
  { "F12", KEY_F12 },
  { "RO", KEY_RO },
  { "KATAKANA", KEY_KATAKANA },
  { "HIRAGANA", KEY_HIRAGANA },
  { "HENKAN", KEY_HENKAN },
  { "KATAKANAHIRAGANA", KEY_KATAKANAHIRAGANA },
  { "MUHENKAN", KEY_MUHENKAN },
  { "KPJPCOMMA", KEY_KPJPCOMMA },
  { "KPENTER", KEY_KPENTER },
  { "RIGHTCTRL", KEY_RIGHTCTRL },
  { "KPSLASH", KEY_KPSLASH },
  { "SYSRQ", KEY_SYSRQ },
  { "RIGHTALT", KEY_RIGHTALT },
  { "LINEFEED", KEY_LINEFEED },
  { "HOME", KEY_HOME },
  { "UP", KEY_UP },
  { "PAGEUP", KEY_PAGEUP },
  { "LEFT", KEY_LEFT },
  { "RIGHT", KEY_RIGHT },
  { "END", KEY_END },
  { "DOWN", KEY_DOWN },
  { "PAGEDOWN", KEY_PAGEDOWN },
  { "INSERT", KEY_INSERT },
  { "DELETE", KEY_DELETE },
  { "MACRO", KEY_MACRO },
  { NULL, -1 } // END-OF-LIST
};

//-----------------------------------------------------------------------------
// METHODS

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

//-------------------------------------------------------------------------

// A second scandir() filter, checks for filename of 'event' + #
static int filter2(const struct dirent *d)
{
	return !strncmp(d->d_name, "event", 5);
}

//-------------------------------------------------------------------------

// Configure the keyboard device
void keyboard_init()
{
  // Set up uinput
  int i;
  char buf[50];

	// Attempt to create uidev virtual keyboard
	if((keyfd1 = open("/dev/uinput", O_WRONLY | O_NONBLOCK)) >= 0) {
		(void)ioctl(keyfd1, UI_SET_EVBIT, EV_KEY);

    // for (i=0; i<NUMBER_OF_KEYS; i++) { //TODO
    //   if(kb_keys_map[i].value >= KEY_RESERVED) {
		// 		(void)ioctl(keyfd1, UI_SET_KEYBIT, kb_keys_map[i].value);
    //   }
    // }

    for(i=0; i<111; i++) {
			if(keyTable[i].value >= KEY_RESERVED)
      {
				(void)ioctl(keyfd1, UI_SET_KEYBIT, keyTable[i].value);
      }
		}

		struct uinput_user_dev uidev;
		memset(&uidev, 0, sizeof(uidev));
		snprintf(uidev.name, UINPUT_MAX_NAME_SIZE, "circuitsword");
		uidev.id.bustype = BUS_USB;
		uidev.id.vendor  = 0x1;
		uidev.id.product = 0x1;
		uidev.id.version = 1;
		if(write(keyfd1, &uidev, sizeof(uidev)) < 0) {
      printf("[!] write failed\n");
      exit(EXIT_FAILURE);
    }
		if(ioctl(keyfd1, UI_DEV_CREATE) < 0) {
      printf("[!] DEV_CREATE failed\n");
      exit(EXIT_FAILURE);
    }
		printf("[i] uidev init OK\n");
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
	if(keyfd2) fprintf(stdout, "[i] SDL2 init OK\n");


	memset(&keyEv    , 0, sizeof(keyEv));
	memset(&synEv    , 0, sizeof(synEv));
	keyEv.type = EV_KEY;
	synEv.type = EV_SYN;
	synEv.code = SYN_REPORT;
}

//-------------------------------------------------------------------------

// Remove keyboard safely
void keyboard_unload()
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

  fprintf(stdout, "[i] Closed keyboard\n");
}

//-------------------------------------------------------------------------

// Hold down a key
void keyboard_press(int id)
{
  keyEv.code  = id;
  keyEv.value = 1;
  write(keyfd, &keyEv, sizeof(keyEv));
  write(keyfd, &synEv, sizeof(synEv));
}

// Release a held key
void keyboard_release(int id)
{
  keyEv.code  = id;
  keyEv.value = 0;
  write(keyfd, &keyEv, sizeof(keyEv));
  write(keyfd, &synEv, sizeof(synEv));
}

// Press and release
void keyboard_button(int id)
{
  keyboard_press(id);
  usleep(1000);

  keyboard_release(id);
  usleep(1000);
}

//-----------------------------------------------------------------------------
