/*
 * This file originates from Kite's Circuit Sword control board project.
 * Author: Kite/Geebles (Giles Burgess)
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

// Configuration data (loaded from EEPROM)
struct Config {
  // Version info
  uint8_t version  = EEPROM_VERSION;
  // Hardware settings
  uint8_t bl_val   = BL_MAX;
  uint8_t wifi_val = 1;
  uint8_t aud_val  = 1;
  uint8_t info_val = 0;
  uint8_t vol_val = 50;
  bool is_a_vol = 0;
  bool is_dpad_joy = 0;
  // Joystick settings
  int16_t xmid1 = 0;
  int16_t ymid1 = 0;
  int16_t xmin1 = 0;
  int16_t ymin1 = 0;
  int16_t xmax1 = 0;
  int16_t ymax1 = 0;
  int16_t xmid2 = 0;
  int16_t ymid2 = 0;
  int16_t xmin2 = 0;
  int16_t ymin2 = 0;
  int16_t xmax2 = 0;
  int16_t ymax2 = 0;
  int16_t dz    = DEADZONE;
  bool iscalib1 = 0;
  bool iscalib2 = 0;
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

//--------------------------------------------------------------------------------------
// MAIN SETUP
void setup() {
  // Startup delay
  delay(1000);
  
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

#ifdef CONFIG_WIFI_DEFAULT_OFF
  cfg.wifi_val = 0;
#endif

  // Read from EEPROM
  eepromCheck();

  // Always default to no info mode
  cfg.info_val = 0;
  cfg.is_dpad_joy = 0;

  // Set defaults
  setBl(cfg.bl_val);
  setWifi(cfg.wifi_val);
  setAud(cfg.aud_val);
  setInfo(cfg.info_val);

  // Set up I2C
  Wire.begin(); // wake up I2C bus
  //attachInterrupt(digitalPinToInterrupt(PIN_INT), change, FALLING);

  // Init buttons
  TCA_init();
  
  // Get button initial states
  readButtons();

  // Calibrate joystick?
  if (btns[B_START]) {
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

  // Misc functions (1Hz)
  static uint32_t tnow3 = 0;
  if (millis() - tnow3 > 500) {

    // Read some analog values
    readAnalogData();
    
    tnow3 = millis();
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
      case 'V': //voltage val (0-1023)
        Serial.print(voltVal);
        break;
        
      case 'i': //is info (0-1)
        Serial.print(cfg.info_val);
        break;

      case 'e': //read volume (H0-100)
        Serial.write(cfg.vol_val);
        break;

      case 'f': //read volume (S0-100)
        Serial.print(cfg.vol_val);
        break;

      case 'E': //set volume (E + H0-100)
        if (Serial.available() == 1) {
          tmp = Serial.read();
          setVol(tmp);
          Serial.print("OK");
        } else {
          Serial.print('?');
        }
        break;

      case 'q': //read backlight (H0-100)
        Serial.write(cfg.bl_val);
        break;

      case 'x': //read backlight (S0-100)
        Serial.print(cfg.bl_val);
        break;

      case 'Q': //set backlight (Q + H0-100)
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
        
      case 'b'://button state NOW (HEXHEX)
        Serial.write(btns_char[0]);
        Serial.write(btns_char[1]);
        break;
        
      case 'B': //button last full state (HEXHEX) also freezes USB
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
        
      case 's': //get status (b[][][][][INFO][AUD][WIFI][MODE])
        bitWrite(tmp, STATUS_MODE, mode);
        bitWrite(tmp, STATUS_WIFI, cfg.wifi_val);
        bitWrite(tmp, STATUS_AUD, cfg.aud_val);
        bitWrite(tmp, STATUS_INFO, cfg.info_val);
        Serial.write(tmp);
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

