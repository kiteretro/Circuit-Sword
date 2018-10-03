/*
 * This file originates from Kite's Circuit Sword control board project.
 * Author: Kite/Geebles/kiteretro (Giles Burgess)
 * 
 * THIS HEADER MUST REMAIN WITH THIS FILE AT ALL TIMES
 *
 * This firmware is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This firmware is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this repo. If not, see <http://www.gnu.org/licenses/>.
 */

 /*
  *  -------------
  * | USAGE NOTES |
  *  -------------
  *  
  * This firmware does the following:
  * - Reads an EEPROM configuration of saved data on boot
  *   - Any changes to configuration are saved automatically
  * - Flashes the LEDs 4 times each in alternating rapid succession (BOOT OK)
  * - If the 'START' button is held on boot, a joystick calibration is started for 10 seconds (default)
  *   - During these 10 seconds, the LED will flash quickly
  *   - During these 10 seconds, slowly rotate the joystick around the full edge of its limits
  *     - You should get about 3-4 rotations done
  *   - NOTE: The joystick WILL NOT FUNCTION UNLESS IT HAS BEEN CALIBRATED!
  *   - ALSO if you are using an analog pot, you will need turn turn the volume up to MAX and perform the joystick calibration
  * - A loop is started that then processes the following data, and repeats forever:
  *   - @ 62.5Hz
  *     - Read the buttons and joystick
  *     - Write them over USB (as a gamepad)
  *     - If the 'mode' button is pressed with a button combo, action these (this will stall the loop to 1-5Hz)
  *   - @ 5Hz
  *     - Check to see if the 'mode' button is pressed
  *     - Process any incoming serial data (see the function processSerial())
  *   - @ 2Hz
  *     - Read the battery voltage
  *     - Read analog pot (if configured)
  * 
  *  -------------
  * | MODE BUTTON |
  *  -------------
  * 
  * By holding down the 'MODE' button while the board is powered on (and the main power switch is on),
  * the following can be done:
  * 
  * MODE + BTN | Result
  * ---------------------------
  * UP         | Volume increase
  * DOWN       | Volume decreases
  * RIGHT      | Brightness of LCD increases
  * LEFT       | Brightness of LCD decreases
  * B          | WIFI OFF
  * A          | WIFI ON
  * X          | AUDIO AMP OFF
  * Y          | AUDIO AMP ON
  * START      | INFO MODE ON (reset to OFF on power cycle)
  * SELECT     | INFO MODE OFF
  * R1         | DPAD IS BUTTONS ON (reset to OFF on power cycle)
  * L1         | DPAD IS BUTTONS OFF
  * 
  */

#include "config.h"
#include "defs.h"

#include "Wire.h"
#include "HID-Project.h" //https://github.com/NicoHood/HID
#include <EEPROM.h>

// BUILD DATE
#define BUILD_DATE_LEN 15
static uint8_t build_date[BUILD_DATE_LEN] = { __DATE__[0], __DATE__[1], __DATE__[2], __DATE__[5], __DATE__[6], __DATE__[7], __DATE__[8], __DATE__[9], __DATE__[10], __TIME__[0], __TIME__[1], __TIME__[3], __TIME__[4], __TIME__[6], __TIME__[7] };

// Configuration data (loaded from EEPROM)
struct Config {
  // Version info
  uint8_t version  = EEPROM_VERSION;
  uint8_t build_date[BUILD_DATE_LEN] = {};
  // Hardware settings
  uint8_t bl_val   = BL_MAX;
  uint8_t wifi_val = 1;
  uint8_t aud_val  = 1;
  uint8_t info_val = 0;
  uint8_t vol_val = VOL_DEFAULT;
  bool is_a_vol = 0;
  bool is_d_vol = 1;
  bool is_dpad_joy = 0;
  // Joystick settings
  int16_t xmid1 = 511;
  int16_t ymid1 = 511;
  int16_t xmin1 = 211;
  int16_t ymin1 = 211;
  int16_t xmax1 = 811;
  int16_t ymax1 = 811;
  int16_t xmid2 = 511;
  int16_t ymid2 = 511;
  int16_t xmin2 = 211;
  int16_t ymin2 = 211;
  int16_t xmax2 = 811;
  int16_t ymax2 = 811;
  int16_t dz    = DEADZONE;
  bool iscalib1 = 0;
  bool iscalib2 = 0;
  bool xinvert1 = INVERT_X1; //JOY1X
  bool yinvert1 = INVERT_Y1; //JOY1Y
  bool xinvert2 = INVERT_X2; //JOY2X
  bool yinvert2 = INVERT_Y2; //JOY2Y
} cfg;

static uint8_t model = MODEL;
static uint8_t hardware = HARDWARE;

// Button array
volatile bool btns[16] = {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0};
volatile uint8_t btns_char[2] = {0,0};
volatile bool mode = 0;

// Button freeze mode
volatile uint8_t btns_char_last[2] = {0,0};
uint32_t tfrozen = 0;
bool isfrozen = 0;

// Analog
volatile uint16_t voltVal = 0;
volatile uint16_t currVal = 0;

// Button update state
volatile byte state = LOW;

// Auto dimming variables
uint32_t tSinceInput = 0;
uint8_t lastBlVal = 0;
bool isDim = false;
uint8_t dim_btns_char_last[2] = {0,0};

//--------------------------------------------------------------------------------------
// MAIN SETUP
void setup() {
  // Startup delay
  delay(500);
  
  // Set up SERIAL
  Serial.begin(115200);

  // Set up STATUS LED
  pinMode(PIN_LED, OUTPUT);
  for (uint8_t i=0; i<4; i++) {
    led(LED_ON);
    delay(50);
    led(LED_OFF);
    delay(50);
  }

  // Set up BACKLIGHT
  pinMode(PIN_BL, OUTPUT);

  // Set up LCD pins
  lcdInit();

  // Analogs
  pinMode(PIN_BTN_MODE, INPUT);
#ifdef USE_JOYSTICK_1
  pinMode(PIN_JOY1_X, INPUT);
  pinMode(PIN_JOY1_Y, INPUT);
#endif
#ifdef USE_JOYSTICK_2
  pinMode(PIN_JOY2_X, INPUT);
  pinMode(PIN_JOY2_Y, INPUT);
#endif
  pinMode(PIN_VOLT, INPUT);
#ifdef USE_VOLUME_ANALOG
  pinMode(PIN_A_VOL, INPUT);
#endif

  // Pre-config
#ifdef CONFIG_WIFI_DEFAULT_OFF
  cfg.wifi_val = 0;
#endif

#ifdef USE_VOLUME_DIGITAL
  cfg.is_d_vol = 1;
#endif

  // Set build date (temporary)
  for (uint8_t x = 0; x < BUILD_DATE_LEN; x++) {
    cfg.build_date[x] = build_date[x];
  }

  // Read from EEPROM
  eepromCheck();

  // Always default to no info mode
  cfg.info_val = 0;
  cfg.is_dpad_joy = 0;
  cfg.aud_val = 1;

  // Set defaults from config
  setBl(cfg.bl_val);
  setWifi(cfg.wifi_val);
  setAud(cfg.aud_val);
  setInfo(cfg.info_val);

  // Set up I2C
  Wire.begin(); // wake up I2C bus
  //attachInterrupt(digitalPinToInterrupt(PIN_INT), change, FALLING);

  // Init buttons
  TCA_init();

  // Alt volume combo mode
#if defined(USE_ALT_PINS_VOLUME_DIGITAL)
  pinMode(PIN_VOL_D_ALT_UP, INPUT_PULLUP);
  pinMode(PIN_VOL_D_ALT_DOWN, INPUT_PULLUP);
#endif
  
  // Get button initial states
  readButtons();

  // Calibrate joystick?
  if (btns[B_START]) {
    delay(2000);
    calibrateJoystick();
  }
  
  // Sends a clean report to the host. This is important on any Arduino type.
  Gamepad.begin();
}

//--------------------------------------------------------------------------------------
// MAIN LOOP
void loop() {

  // Check serial console
  processSerial();
  
  // Button loop (62.5Hz)
  static uint32_t tnow = 0;
  if (millis() - tnow > 16) {

    // Led ON for debug
    led(LED_ON);
    
    // Read Buttons
    readButtons();

    // If not frozen
    if (!isfrozen) {

      // Set gamepad buttons (USB)
      setGamepad();

    } else { //reset frozen counter
      if (millis() - tfrozen > FREEZE_DURATION) {
        isfrozen = 0;
      }
    }

    // Set modes and aux things
    setModes();

    // Led OFF for debug
    led(LED_OFF);
    
    tnow = millis();
  }

  // Misc functions (5Hz)
  static uint32_t tnow2 = 0;
  if (millis() - tnow2 > 200) {

    // See if mode presses
    readMode();

    // Reset last frozen buttons
    if (mode) {
      btns_char_last[0] = 0;
      btns_char_last[1] = 0;
    }
    
    tnow2 = millis();
  }

  // Misc functions (2Hz)
  static uint32_t tnow3 = 0;
  if (millis() - tnow3 > 500) {

    // Read some analog values
    readAnalogData();

    // Auto dim
#ifdef USE_AUTODIM
    // Check for button change
    if (dim_btns_char_last[0] != btns_char[0] || dim_btns_char_last[1] != btns_char[1]) {
      dim_btns_char_last[0] = btns_char[0];
      dim_btns_char_last[1] = btns_char[1];
      tSinceInput = 0;
    } else {
      if (!isDim) {
        tSinceInput++;
      }
    }

    // Check if in timeout or not
    if (tSinceInput > AUTODIM_TIMEOUT) {
      if (!isDim) {
        isDim = true;
        // Set BL to low
        lastBlVal = cfg.bl_val;
        setBl(BL_MIN);
        // Save back to EEPROM the old val (without updating PWM)
        cfg.bl_val = lastBlVal;
        eepromWrite();
      }
    } else {
      if (isDim) {
        isDim = false;
        setBl(lastBlVal);
      }
    }
#endif
    
    tnow3 = millis();
  }
  
}

//--------------------------------------------------------------------------------------
// DELAY WHILE PROCESSING SERIAL
void cs_delay(uint16_t ms) {

  bool looping = true;
  uint32_t tnowdelay = millis();
  while (looping) {

    // Do serial stuff
    processSerial();

    // Timeout reached?
    if (millis() - tnowdelay > ms) {
      looping = false;
    } else {
      delayMicroseconds(50);
    }
  }
}

//--------------------------------------------------------------------------------------
// PROCESS SERIAL
void processSerial() {
  if (Serial.available() > 0) {

    // temporary var
    uint8_t tmp = 0;
    
    // read the incoming byte:
    char in = Serial.read();

    switch (in) {
      case 'V': //voltage val (0-1023 (string))
        Serial.print(voltVal);
        break;

      case 'c': //voltage val (0-1023 (16bit)
        serialWrite2(voltVal);
        break;
      
      case 'i': //is info (0-1)
        Serial.print(cfg.info_val);
        break;

      case 'e': //read volume (B0-100)
        Serial.write(cfg.vol_val);
        break;

      case 'f': //read volume (S0-100)
        Serial.print(cfg.vol_val);
        break;

      case 'E': //set volume (E + B0-100)
        if (Serial.available() == 1) {
          tmp = Serial.read();
          setVol(tmp);
          Serial.print("OK");
        } else {
          Serial.print('?');
        }
        break;

      case 'q': //read backlight (B0-100)
        Serial.write(cfg.bl_val);
        break;

      case 'x': //read backlight (S0-100)
        Serial.print(cfg.bl_val);
        break;

      case 'Q': //set backlight (Q + B0-100)
        if (Serial.available() == 1) {
          tmp = Serial.read();
          setBl(tmp);
          Serial.print("OK");
        } else {
          Serial.print('?');
        }
        break;
        
      case 'I': //set info (I0-1)
        if (Serial.available() == 1) {
          tmp = Serial.read();
          if (tmp == '1') {
            setInfo(1);
          } else {
            setInfo(0);
          }
          Serial.print("OK");
        } else {
          Serial.print('?');
        }
        break;
        
      case 'w': //is wifi (0-1)
        Serial.print(cfg.wifi_val);
        break;
        
      case 'W': //set wifi (W0-1)
        if (Serial.available() == 1) {
          tmp = Serial.read();
          if (tmp == '1') {
            setWifi(1);
          } else {
            setWifi(0);
          }
          Serial.print("OK");
        } else {
          Serial.print('?');
        }
        break;
        
      case 'a': //is audio (0-1)
        Serial.print(cfg.aud_val);
        break;
        
      case 'A': //set audio (A0-1)
        if (Serial.available() == 1) {
          tmp = Serial.read();
          if (tmp == '1') {
            setAud(1);
          } else {
            setAud(0);
          }
          Serial.print("OK");
        } else {
          Serial.print('?');
        }
        break;
        
      case 'b'://button state NOW (BINBIN)
        Serial.write(btns_char[0]);
        Serial.write(btns_char[1]);
        break;
        
      case 'B': //button last full state (BINBIN) also freezes USB
        Serial.write(btns_char_last[0]);
        Serial.write(btns_char_last[1]);
        btns_char_last[0] = 0;
        btns_char_last[1] = 0;
        tfrozen = millis();
        isfrozen = 1;
        break;
        
      case 'r': //reset button last full state
        btns_char_last[0] = 0;
        btns_char_last[1] = 0;
        Serial.print("OK");
        break;
        
      case 's': //get status (b[][DVOL][DPAD][AVOL][INFO][AUD][WIFI][MODE])
        bitWrite(tmp, STATUS_MODE, mode);
        bitWrite(tmp, STATUS_WIFI, cfg.wifi_val);
        bitWrite(tmp, STATUS_AUD, cfg.aud_val);
        bitWrite(tmp, STATUS_INFO, cfg.info_val);
        bitWrite(tmp, STATUS_AVOL, cfg.is_a_vol);
        bitWrite(tmp, STATUS_DPAD_JOY, cfg.is_dpad_joy);
        bitWrite(tmp, STATUS_DVOL, cfg.is_d_vol);
        bitWrite(tmp, 7, 0);
        Serial.write(tmp);
        break;

      case 'd': //Toggle dpad joy
        cfg.is_dpad_joy = !cfg.is_dpad_joy;
        Serial.print("OK");
        break;

      case 'm': //get model (b[V][V][V][V][M][M][M][M])
        bitWrite(tmp, 0, bitRead(model, 0));
        bitWrite(tmp, 1, bitRead(model, 1));
        bitWrite(tmp, 2, bitRead(model, 2));
        bitWrite(tmp, 3, bitRead(model, 3));
        bitWrite(tmp, 4, bitRead(hardware, 0));
        bitWrite(tmp, 5, bitRead(hardware, 1));
        bitWrite(tmp, 6, bitRead(hardware, 2));
        bitWrite(tmp, 7, bitRead(hardware, 3));
        Serial.write(tmp);
        break;

      case 'J': //Calibrate joystick
        calibrateJoystick();
        Serial.print("OK");
        break;

      case '(': //Invert Joy1 X
        cfg.xinvert1 = !cfg.xinvert1;
        eepromWrite();
        Serial.print("OK");
        break;

      case ')': //Invert Joy1 Y
        cfg.yinvert1 = !cfg.yinvert1;
        eepromWrite();
        Serial.print("OK");
        break;

      case '[': //Invert Joy2 X
        cfg.xinvert2 = !cfg.xinvert2;
        eepromWrite();
        Serial.print("OK");
        break;

      case ']': //Invert Joy2 Y
        cfg.yinvert2 = !cfg.yinvert2;
        eepromWrite();
        Serial.print("OK");
        break;

      case '{': //Toggle Joy1
        cfg.iscalib1 = !cfg.iscalib1;
        eepromWrite();
        Serial.print("OK");
        break;

      case '}': //Toggle Joy2
        cfg.iscalib2 = !cfg.iscalib2;
        eepromWrite();
        Serial.print("OK");
        break;

      case 'j': //Get joy config
        bitWrite(tmp, 0, cfg.iscalib1);
        bitWrite(tmp, 1, cfg.iscalib2);
        bitWrite(tmp, 2, cfg.xinvert1);
        bitWrite(tmp, 3, cfg.yinvert1);
        bitWrite(tmp, 4, cfg.xinvert2);
        bitWrite(tmp, 5, cfg.yinvert2);
        bitWrite(tmp, 6, 0);
        bitWrite(tmp, 7, 0);
        Serial.write(tmp);
        break;

      case 'u': //Get JOY1X ADC
        serialWrite2(analogRead(PIN_JOY1_X));
        break;

      case 'o': //Get JOY1Y ADC
        serialWrite2(analogRead(PIN_JOY1_Y));
        break;

      case 'p': //Get JOY2X ADC
        serialWrite2(analogRead(PIN_JOY2_X));
        break;

      case 'y': //Get JOY2Y ADC
        serialWrite2(analogRead(PIN_JOY2_Y));
        break;

      case 't': //Get AVOL ADC
        serialWrite2(analogRead(PIN_A_VOL));
        break;

      case 'C': //Toggle AVOL
        cfg.is_a_vol = !cfg.is_a_vol;
        if (cfg.is_a_vol == 1) {
          cfg.is_d_vol = 0;
        }
        eepromWrite();
        Serial.print("OK");
        break;

      case 'z': //Toggle DVOL
        cfg.is_d_vol = !cfg.is_d_vol;
        if (cfg.is_d_vol == 1) {
          cfg.is_a_vol = 0;
          cfg.vol_val = VOL_DEFAULT;
        }
        eepromWrite();
        Serial.print("OK");
        break;

      case 'D': //Build date
        for (uint8_t x = 0; x < BUILD_DATE_LEN; x++) {
          Serial.print(cfg.build_date[x]);
        }
        Serial.print("OK");
        break;
        
      case 'L': //lcd serial input
        if (lcdSerial()) {
          Serial.print(LCD_SUCCESS);
        }
        break;
        
      case 'l': //lcd init
        lcdInit();
        Serial.print("OK");
        break;
        
      default: //error
        Serial.print('?');
      break;
    }
  }
}

