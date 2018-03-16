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

#include "config.h"
#include "defs.h"

void TCA_init() {
  // Reset pointer
  Wire.beginTransmission(TCAADDR);
  Wire.write(0x00);
  Wire.endTransmission();
}

//--------------------------------------------------------------------------------------
// READ BUTTONS
void readButtons() {
  // Read
  uint8_t pos = 0;
  bool thisbtns[16];
  Wire.requestFrom(TCAADDR, 2); // request 2 bytes from slave device
  while(Wire.available()) // slave may send less than requested
  {
    uint8_t c = ~Wire.read(); // receive a byte as character

    for (uint8_t i=0; i<8; i++) {
      thisbtns[i+pos*8] = bitRead(c, i);
    }
    pos++;
  }

  // Set the correct mapping.. this is VERY inefficient!!!
  btns[B_UP] = thisbtns[BTN_UP];
  btns[B_DOWN] = thisbtns[BTN_DOWN];
  btns[B_LEFT] = thisbtns[BTN_LEFT];
  btns[B_RIGHT] = thisbtns[BTN_RIGHT];
  btns[B_A] = thisbtns[BTN_A];
  btns[B_B] = thisbtns[BTN_B];
  btns[B_X] = thisbtns[BTN_X];
  btns[B_Y] = thisbtns[BTN_Y];
  btns[B_START] = thisbtns[BTN_START];
  btns[B_SELECT] = thisbtns[BTN_SELECT];
  btns[B_L1] = thisbtns[BTN_L1];
  btns[B_L2] = thisbtns[BTN_L2];
  btns[B_R1] = thisbtns[BTN_R1];
  btns[B_R2] = thisbtns[BTN_R2];
  btns[B_C1] = thisbtns[BTN_C1];
  btns[B_C2] = thisbtns[BTN_C2];

  // Populate the btns_char for quicker access
  for (uint8_t i=0; i<8; i++) {
    bitWrite(btns_char[0], i, btns[i]);
  }
  for (uint8_t i=0; i<8; i++) {
    bitWrite(btns_char[1], i, btns[i+8]);
  }

  // Set the 'last button pressed' chars, useful to know without reading it live
  btns_char_last[0] |= btns_char[0];
  btns_char_last[1] |= btns_char[1];
}

//--------------------------------------------------------------------------------------
// READ MODE
void readMode() {
  bool btnVal = digitalRead(PIN_BTN_MODE);
  //uint16_t btnVal = analogRead(PIN_BTN_MODE);

#ifdef DEBUG
  Serial.print("BV[");
  Serial.print(btnVal);
  Serial.println("]");
#endif

#ifndef NOMODE
  if (!btnVal) { //Mode button pressed?
    if (isBooted()) {
      mode = 1;
    } else {
      mode = 0;
    }
  } else {
    mode = 0;
  }
#endif
}

//--------------------------------------------------------------------------------------
// SET GAMEPAD
void setGamepad() {
  //DPAD
  Gamepad.dPad1(GAMEPAD_DPAD_CENTERED);

  if (cfg.is_dpad_joy) {
    
    // DPAD acts as extra buttons (can be configured as a joystick)
    if (btns[B_UP]) {
      Gamepad.press(GP_DPAD_JOY_UP);
    } else {
      Gamepad.release(GP_DPAD_JOY_UP);
    }
    if (btns[B_DOWN]) {
      Gamepad.press(GP_DPAD_JOY_DOWN);
    } else {
      Gamepad.release(GP_DPAD_JOY_DOWN);
    }
    if (btns[B_LEFT]) {
      Gamepad.press(GP_DPAD_JOY_LEFT);
    } else {
      Gamepad.release(GP_DPAD_JOY_LEFT);
    }
    if (btns[B_RIGHT]) {
      Gamepad.press(GP_DPAD_JOY_RIGHT);
    } else {
      Gamepad.release(GP_DPAD_JOY_RIGHT);
    }
    
  } else {

    // DPAD acts as the HAT device
    if (btns[B_DOWN]) {
      Gamepad.dPad1(GAMEPAD_DPAD_DOWN);
      if (btns[B_LEFT]) {
        Gamepad.dPad1(GAMEPAD_DPAD_DOWN_LEFT);
      }
      if (btns[B_RIGHT]) {
        Gamepad.dPad1(GAMEPAD_DPAD_DOWN_RIGHT);
      }
    } else if(btns[B_UP]) {
      Gamepad.dPad1(GAMEPAD_DPAD_UP);
      if (btns[B_LEFT]) {
        Gamepad.dPad1(GAMEPAD_DPAD_UP_LEFT);
      }
      if (btns[B_RIGHT]) {
        Gamepad.dPad1(GAMEPAD_DPAD_UP_RIGHT);
      }
    } else if (btns[B_LEFT]) {
      Gamepad.dPad1(GAMEPAD_DPAD_LEFT);
    } else if (btns[B_RIGHT]) {
      Gamepad.dPad1(GAMEPAD_DPAD_RIGHT);
    }
    
  }
  
  //BUTTON
  if (btns[B_A]) {
    Gamepad.press(GP_A);
  } else {
    Gamepad.release(GP_A);
  }

  if (btns[B_B]) {
    Gamepad.press(GP_B);
  } else {
    Gamepad.release(GP_B);
  }

  if (btns[B_X]) {
    Gamepad.press(GP_X);
  } else {
    Gamepad.release(GP_X);
  }

  if (btns[B_Y]) {
    Gamepad.press(GP_Y);
  } else {
    Gamepad.release(GP_Y);
  }

  if (btns[B_START]) {
    Gamepad.press(GP_START);
  } else {
    Gamepad.release(GP_START);
  }

  if (btns[B_SELECT]) {
    Gamepad.press(GP_SELECT);
  } else {
    Gamepad.release(GP_SELECT);
  }

  if (btns[B_L1]) {
    Gamepad.press(GP_L1);
  } else {
    Gamepad.release(GP_L1);
  }

  if (btns[B_L2]) {
    Gamepad.press(GP_L2);
  } else {
    Gamepad.release(GP_L2);
  }

  if (btns[B_R1]) {
    Gamepad.press(GP_R1);
  } else {
    Gamepad.release(GP_R1);
  }

  if (btns[B_R2]) {
    Gamepad.press(GP_R2);
  } else {
    Gamepad.release(GP_R2);
  }

  // CC1/CC2 function
#if defined(USE_VOLUME_DIGITAL) && !defined(USE_ALT_PINS_VOLUME_DIGITAL)
    if (btns[B_C1]) {
      setVolInc(VOL_DOWN);
      delay(200);
    }
    if (btns[B_C2]) {
      setVolInc(VOL_UP);
      delay(200);
    }
#else
  if (btns[B_C1]) {
    Gamepad.press(GP_C1);
  } else {
    Gamepad.release(GP_C1);
  }

  if (btns[B_C2]) {
    Gamepad.press(GP_C2);
  } else {
    Gamepad.release(GP_C2);
  }
#endif

  // Alt volume combo mode
#if defined(USE_VOLUME_DIGITAL) && defined(USE_ALT_PINS_VOLUME_DIGITAL)
  if (!digitalRead(PIN_VOL_D_ALT_DOWN)) {
      setVolInc(VOL_DOWN);
      delay(200);
    }
    if (!digitalRead(PIN_VOL_D_ALT_UP)) {
      setVolInc(VOL_UP);
      delay(200);
    }
#endif

  // JOYSTICK 1
#ifdef USE_JOYSTICK_1
  if (cfg.iscalib1) {
    uint16_t x = analogRead(PIN_JOY1_X);
    uint16_t y = analogRead(PIN_JOY1_Y);
    
    int16_t xx = x-cfg.xmid1;
    int16_t yy = y-cfg.ymid1;

    if (xx > cfg.dz) {
      xx -= cfg.dz;
    } else if (xx<(cfg.dz-cfg.dz-cfg.dz)) {
      xx += cfg.dz;
    } else {
      xx = 0;
    }

#ifdef INVERT_X1
    xx = constrain(map(xx, cfg.xmin1+50, cfg.xmax1-50, 32678, -32678), -32678, 32678);
#else
    xx = constrain(map(xx, cfg.xmin1+50, cfg.xmax1-50, -32678, 32678), -32678, 32678);
#endif

    if (yy > cfg.dz) {
      yy -= cfg.dz;
    } else if (yy<(cfg.dz-cfg.dz-cfg.dz)) {
      yy += cfg.dz;
    } else {
      yy = 0;
    }

#ifdef INVERT_Y1
    yy = constrain(map(yy, cfg.ymin1+50, cfg.ymax1-50, 32678, -32678), -32678, 32678);
#else
    yy = constrain(map(yy, cfg.ymin1+50, cfg.ymax1-50, -32678, 32678), -32678, 32678);
#endif

    Gamepad.xAxis(xx);
    Gamepad.yAxis(yy);
  } else {
    Gamepad.xAxis(0);
    Gamepad.yAxis(0);
  }
#endif

  // JOYSTICK 2
#ifdef USE_JOYSTICK_2
  if (cfg.iscalib2) {
    uint16_t x = analogRead(PIN_JOY2_X);
    uint16_t y = analogRead(PIN_JOY2_Y);
    
    int16_t xx = x-cfg.xmid2;
    int16_t yy = y-cfg.ymid2;

    if (xx > cfg.dz) {
      xx -= cfg.dz;
    } else if (xx<(cfg.dz-cfg.dz-cfg.dz)) {
      xx += cfg.dz;
    } else {
      xx = 0;
    }

#ifdef INVERT_X2
    xx = constrain(map(xx, cfg.xmin2+50, cfg.xmax2-50, 32678, -32678), -32678, 32678);
#else
    xx = constrain(map(xx, cfg.xmin2+50, cfg.xmax2-50, -32678, 32678), -32678, 32678);
#endif

    if (yy > cfg.dz) {
      yy -= cfg.dz;
    } else if (yy<(cfg.dz-cfg.dz-cfg.dz)) {
      yy += cfg.dz;
    } else {
      yy = 0;
    }

#ifdef INVERT_Y2
    yy = constrain(map(yy, cfg.ymin2+50, cfg.ymax2-50, 32678, -32678), -32678, 32678);
#else
    yy = constrain(map(yy, cfg.ymin2+50, cfg.ymax2-50, -32678, 32678), -32678, 32678);
#endif

    Gamepad.rxAxis(xx);
    Gamepad.ryAxis(yy);
  } else {
    Gamepad.rxAxis(0);
    Gamepad.ryAxis(0);
  }
#endif

  if (!mode) {
    Gamepad.write();
  }
}

//--------------------------------------------------------------------------------------
// SET MODES
void setModes() {
  // WRITE
  if (mode) {
#ifdef USE_OLD_BUTTON_COMBOS
    if (btns[B_UP]) {
      setBlInc(BL_UP);
    }
    if (btns[B_DOWN]) {
      setBlInc(BL_DOWN);
    }
    if (btns[B_RIGHT]) {
      setVolInc(VOL_UP);
    }
    if (btns[B_LEFT]) {
      setVolInc(VOL_DOWN);
    }
#else
    if (btns[B_UP]) {
      setVolInc(VOL_UP);
    }
    if (btns[B_DOWN]) {
      setVolInc(VOL_DOWN);
    }
    if (btns[B_RIGHT]) {
      setBlInc(BL_UP);
    }
    if (btns[B_LEFT]) {
      setBlInc(BL_DOWN);
    }
#endif
    if (btns[B_A]) {
      setWifi(1);
    }
    if (btns[B_B]) {
      setWifi(0);
    }
    if (btns[B_Y]) {
      setAud(1);
    }
    if (btns[B_X]) {
      setAud(0);
    }
    if (btns[B_START]) {
      setInfo(1);
    }
    if (btns[B_SELECT]) {
      setInfo(0);
    }
    if (btns[B_R1]) {
      cfg.is_dpad_joy = 1;
    }
    if (btns[B_L1]) {
      cfg.is_dpad_joy = 0;
    }

    // Max 5Hz
    delay(200);
  }
}

//--------------------------------------------------------------------------------------
// ANALOG DATA
void readAnalogData() {

  // Read voltage
  voltVal = (analogRead(PIN_VOLT) + analogRead(PIN_VOLT) + analogRead(PIN_VOLT))/3;

#ifdef DEBUG
  Serial.print("V[");
  Serial.print(voltVal);
  Serial.println("]");
#endif

#ifdef USE_VOLUME_ANALOG
  if (cfg.is_a_vol) {
    uint16_t a_vol = constrain(analogRead(PIN_A_VOL), VOL_A_MIN, VOL_A_MAX);
    setVol(map(a_vol, VOL_A_MIN, VOL_A_MAX, VOL_MIN, VOL_MAX));
  }
#endif
}

//--------------------------------------------------------------------------------------
// INTERRUPT FOR I2C
void change() {
  state = HIGH;
}

//--------------------------------------------------------------------------------------
// CALIBRATE JOYSTICK
void calibrateJoystick() {
  
#ifdef DEBUG
  //while (!Serial) {}
  Serial.println("CALIB");
#endif

  cfg.iscalib1 = false;
  cfg.iscalib2 = false;
  cfg.is_a_vol = false;

#ifdef USE_VOLUME_ANALOG
  uint16_t av = analogRead(PIN_A_VOL);
  if(av > 250 && av < 1000) {
    cfg.is_a_vol = 1;
  } else {
    cfg.is_a_vol = 0;
  }
#else
  cfg.is_a_vol = 0;
#endif

#ifdef USE_JOYSTICK_1
  // Set up the vars 1
  uint16_t xmid1 = 0;
  uint16_t ymid1 = 0;
  int16_t xmin1 = 0;
  int16_t ymin1 = 0;
  int16_t xmax1 = 0;
  int16_t ymax1 = 0;

  uint16_t x1 = analogRead(PIN_JOY1_X);
  xmin1 = x1;
  xmax1 = x1;
  xmid1 = x1;
  uint16_t y1 = analogRead(PIN_JOY1_Y);
  ymin1 = y1;
  ymax1 = y1;
  ymid1 = y1;

  bool joy1_ok = 1;
  if (y1 < 20 || x1 < 20) {
    joy1_ok = 0;
  }
#endif

#ifdef USE_JOYSTICK_2
  // Set up the vars 2
  uint16_t xmid2 = 0;
  uint16_t ymid2 = 0;
  int16_t xmin2 = 0;
  int16_t ymin2 = 0;
  int16_t xmax2 = 0;
  int16_t ymax2 = 0;

  uint16_t x2 = analogRead(PIN_JOY2_X);
  xmin2 = x2;
  xmax2 = x2;
  xmid2 = x2;
  uint16_t y2 = analogRead(PIN_JOY2_Y);
  ymin2 = y2;
  ymax2 = y2;
  ymid2 = y2;

  bool joy2_ok = 1;
  if (y2 < 20 || x2 < 20) {
    joy2_ok = 0;
  }
#endif

  uint32_t tnow = millis();
  // Calibrate mid, min, and max for X secs
  //while(millis() < CALIBTIME * 1000) {
  while(millis() - tnow < CALIBTIME * 1000) {
    
    led(LED_ON);

    // JOY1
#ifdef USE_JOYSTICK_1
    if (joy1_ok) {
      uint16_t x1 = analogRead(PIN_JOY1_X); delay(1);
      uint16_t y1 = analogRead(PIN_JOY1_Y);
  
      xmid1 = (xmin1 + xmax1)/2;
      ymid1 = (ymin1 + ymax1)/2;
      
      int16_t nx1 = x1 - xmid1;
      int16_t ny1 = y1 - ymid1;
  
      if (x1 < xmin1)
        xmin1 = x1;
      if (y1 < ymin1)
        ymin1 = y1;
  
      if (x1 > xmax1)
        xmax1 = x1;
      if (y1 > ymax1)
        ymax1 = y1;

#ifdef DEBUG
      Serial.print("XM1:");
      Serial.print(xmid1);
      Serial.print(" YM1:");
      Serial.print(ymid1);
      Serial.print(" X1:");
      Serial.print(x1);
      Serial.print(" Y1:");
      Serial.print(y1);
      Serial.print(" NX1:");
      Serial.print(nx1);
      Serial.print(" NY1:");
      Serial.print(ny1);
      Serial.print(" XMI1:");
      Serial.print(xmin1);
      Serial.print(" YMI1:");
      Serial.print(ymin1);
      Serial.print(" XMA1:");
      Serial.print(xmax1);
      Serial.print(" YMA1:");
      Serial.print(ymax1);
      Serial.println();
#endif
    }
#endif

    // JOY 2
#ifdef USE_JOYSTICK_2
    if (joy2_ok) {
      uint16_t x2 = analogRead(PIN_JOY2_X); delay(1);
      uint16_t y2 = analogRead(PIN_JOY2_Y);
  
      xmid2 = (xmin2 + xmax2)/2;
      ymid2 = (ymin2 + ymax2)/2;
      
      int16_t nx2 = x2 - xmid2;
      int16_t ny2 = y2 - ymid2;
  
      if (x2 < xmin2)
        xmin2 = x2;
      if (y2 < ymin2)
        ymin2 = y2;
  
      if (x2 > xmax2)
        xmax2 = x2;
      if (y2 > ymax2)
        ymax2 = y2;

#ifdef DEBUG
      Serial.print("XM2:");
      Serial.print(xmid2);
      Serial.print(" YM2:");
      Serial.print(ymid2);
      Serial.print(" X2:");
      Serial.print(x2);
      Serial.print(" Y2:");
      Serial.print(y2);
      Serial.print(" NX2:");
      Serial.print(nx2);
      Serial.print(" NY2:");
      Serial.print(ny2);
      Serial.print(" XMI2:");
      Serial.print(xmin2);
      Serial.print(" YMI2:");
      Serial.print(ymin2);
      Serial.print(" XMA2:");
      Serial.print(xmax2);
      Serial.print(" YMA2:");
      Serial.print(ymax2);
      Serial.println();
#endif
    }
#endif
    
    delay(100);
    led(LED_OFF);
    delay(100);
  }

  // JOY 1
#ifdef USE_JOYSTICK_1
  xmin1 = xmin1 - xmid1;
  ymin1 = ymin1 - ymid1;
  xmax1 = xmax1 - xmid1;
  ymax1 = ymax1 - ymid1;

#ifdef DEBUG
  Serial.println("CALIB COMPLETE");
  Serial.print("XM1:");
  Serial.print(xmid1);
  Serial.print(" YM1:");
  Serial.print(ymid1);
  Serial.print(" XMI1:");
  Serial.print(xmin1);
  Serial.print(" YMI1:");
  Serial.print(ymin1);
  Serial.print(" XMA1:");
  Serial.print(xmax1);
  Serial.print(" YMA1:");
  Serial.print(ymax1);
  Serial.println();
#endif

  cfg.xmin1 = xmin1;
  cfg.ymin1 = ymin1;
  cfg.xmax1 = xmax1;
  cfg.ymax1 = ymax1;
  cfg.xmid1 = xmid1;
  cfg.ymid1 = ymid1;
  cfg.iscalib1 = joy1_ok;
#endif
  
  // JOY 2
#ifdef USE_JOYSTICK_2
  xmin2 = xmin2 - xmid2;
  ymin2 = ymin2 - ymid2;
  xmax2 = xmax2 - xmid2;
  ymax2 = ymax2 - ymid2;

#ifdef DEBUG
  Serial.print("XM2:");
  Serial.print(xmid2);
  Serial.print(" YM2:");
  Serial.print(ymid2);
  Serial.print(" XMI2:");
  Serial.print(xmin2);
  Serial.print(" YMI2:");
  Serial.print(ymin2);
  Serial.print(" XMA2:");
  Serial.print(xmax2);
  Serial.print(" YMA2:");
  Serial.print(ymax2);
  Serial.println();
#endif

  cfg.xmin2 = xmin2;
  cfg.ymin2 = ymin2;
  cfg.xmax2 = xmax2;
  cfg.ymax2 = ymax2;
  cfg.xmid2 = xmid2;
  cfg.ymid2 = ymid2;
  cfg.iscalib2 = joy2_ok;
#endif

  // SAVE ALL
  cfg.dz = DEADZONE;
  eepromWrite();
}

