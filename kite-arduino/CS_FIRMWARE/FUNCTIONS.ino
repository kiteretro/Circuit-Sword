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

bool bootcheck = false;

//--------------------------------------------------------------------------------------
void led(uint8_t state) {
  if (state == LED_ON) {
    digitalWrite(PIN_LED, HIGH);
  } else if (state == LED_OFF) {
    digitalWrite(PIN_LED, LOW);
  } else {
    return;
  }
}

//--------------------------------------------------------------------------------------
// Just booted check
bool isJustBooted() {
  bool ret = false;

  if (isBooted()) {
    if (!bootcheck) {
      bootcheck = true;
      ret = true;
    }
  } else {
    if (bootcheck) {
      bootcheck = false;
    }
  }

#ifdef DEBUG
  Serial.print("JB[");
  Serial.print(ret);
  Serial.println("]");
#endif

  return ret;
}

// Boot status
bool isBooted() {
  return true;
}

//--------------------------------------------------------------------------------------
// BACKLIGHT FUNCTION
void setBlInc(bool t) {
  uint8_t b = cfg.bl_val;

  if (t == BL_DOWN) {
    if (b < BL_INC) {
      b = BL_MIN;
    } else {
      b -= BL_INC;
    }
  } else if (t == BL_UP) {
    if (b + BL_INC > BL_MAX) {
      b = BL_MAX;
    } else {
      b += BL_INC;
    }
  }

  setBl(b);
}

void setBl(uint8_t t) {
  // Boundary check
  cfg.bl_val = constrain(t, BL_MIN, BL_MAX);

  // Mapping create
  uint8_t b = map(cfg.bl_val, BL_MIN, BL_MAX, BL_PWM_MIN, BL_PWM_MAX);

#ifdef DEBUG
  Serial.print("BACKLIGHT[");
  Serial.print(cfg.bl_val);
  Serial.println("]");
#endif
  
  analogWrite(PIN_BL, b);
  eepromWrite();
}

//--------------------------------------------------------------------------------------
// WIFI
void setWifi(bool val) {
  if (val) {
    cfg.wifi_val = 1;
#ifdef DEBUG
    Serial.println("W ON");
#endif
  } else {
    cfg.wifi_val = 0;
#ifdef DEBUG
    Serial.println("W OFF");
#endif
  }

  eepromWrite();
}

//--------------------------------------------------------------------------------------
// AUDIO
void setAud(bool val) {
  if (val) {
    cfg.aud_val = 1;
    pinMode(PIN_AUD, INPUT);
#ifdef DEBUG
    Serial.println("A ON");
#endif
  } else {
    cfg.aud_val = 0;
    pinMode(PIN_AUD, OUTPUT);
#ifdef DEBUG
    Serial.println("A OFF");
#endif
  }

  eepromWrite();
}

void setVolInc(bool t) {
  uint8_t v = cfg.vol_val;

  if (v > VOL_MAX) {
    v = VOL_MAX;
  }

  if (t == VOL_DOWN) {
    if (v > VOL_INC) {
      v -= VOL_INC;
    } else {
      if (v > VOL_MIN) {
        v--;
      } else {
        v = VOL_MIN;
      }
    }
  } else if (t == VOL_UP) {
    if (v + VOL_INC > VOL_MAX) {
      v = VOL_MAX;
    } else {
      if (v < VOL_INC) {
        v++;
      } else {
        v += VOL_INC;
      }
    }
  }

  setVol(v);
}

void setVolNow(uint8_t t) {
  // Boundary check
  uint8_t v = constrain(t, VOL_MIN, VOL_MAX);

  if (v > VOL_MAX) {
    v = VOL_MAX;
  }
  
  cfg.vol_val = v;
}

void setVol(uint8_t t) {
  setVolNow(t);

#ifdef DEBUG
  Serial.print("VOLUME[");
  Serial.print(cfg.vol_val);
  Serial.println("]");
#endif

  eepromWrite();
}

//--------------------------------------------------------------------------------------
// INFO
void setInfo(bool val) {
  if (val) {
    cfg.info_val = 1;
#ifdef DEBUG
    Serial.println("INFO ON");
#endif
  } else {
    cfg.info_val = 0;
#ifdef DEBUG
    Serial.println("INFO OFF");
#endif
  }

  eepromWrite();
}

//--------------------------------------------------------------------------------------
// Write a 2 character binary data
void serialWrite2(uint16_t data) {
  Serial.write(data & 0xFF);
  Serial.write(data >> 8 & 0xFF);
}

//--------------------------------------------------------------------------------------
// Check EEPROM is ok and read contents
void eepromCheck() {

  // Assume valid unless proven otherwise
  bool valid = true;

  // Check the compile date
  for (uint8_t x = 0; x < BUILD_DATE_LEN; x++) {
    if (build_date[x] != EEPROM.read(EEPROM_START + 1 + x)) {
      valid = false;
    }
  }

  // Check version
  if (EEPROM.read(EEPROM_START + 0) != EEPROM_VERSION) {
    valid = false;
  }

  // Read if valid, reset if not
  if (valid) {
    eepromRead();
  } else {
    eepromWrite();
    eepromRead();
  }
}

//--------------------------------------------------------------------------------------
// Write eeprom
void eepromWrite() {
  for (unsigned int t=0; t<sizeof(cfg); t++) {
    EEPROM.write(EEPROM_START + t, *((char*)&cfg + t));
  }
}

//--------------------------------------------------------------------------------------
// Read eeprom
void eepromRead() {
  for (unsigned int t=0; t<sizeof(cfg); t++) {
    *((char*)&cfg + t) = EEPROM.read(EEPROM_START + t);
  }
}

