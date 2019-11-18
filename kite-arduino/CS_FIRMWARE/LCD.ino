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

#define SDO   PIN_LCD_SDO
#define SCK   PIN_LCD_CLK
#define CS    PIN_LCD_CS
#define RESET PIN_LCD_RESET

// Serial buffer
uint8_t serial_size = 0;
uint8_t serial_buf[64];
bool serial_complete = false;

//---------------------------------------------------------------
// Process LCD serial input
bool lcdSerial() {
  delay(200);
  serial_size = 0;
  serial_complete = false;
  
  while (Serial.available() > 0) {
    char c = Serial.read();

    if (c == '!') { //End char
      serial_complete = true;
    } else { //Add to buffer
      serial_buf[serial_size] = c;
      serial_size++;
    }
  }

  if (serial_complete) { //Successfully complete

    if (!isEven(serial_size)) { //Error check
      Serial.print(FAILED_NOT_EVEN_ARGS);
      return false;
    }
    
    lcdWriteEn(1); //Enable LCD select
    for (uint8_t i=0; i < serial_size; i++) { //For each byte
      if(!isEven(i)) { //Do we have 2x bytes yet?
        uint8_t dat = byteval(serial_buf[i-1], serial_buf[i]); //Convert ASCII into HEX
        if (i==1) { //First byte is command
          lcdCommand(dat);
        } else { //Remaining are data
          lcdData(dat);
        }
      }
    }
    lcdWriteEn(0); //Disable LCD select

  } else {
    Serial.print(FAILED_TIMEOUT_NO_END);
    return false;
  }

  return true;
}

//---------------------------------------------------------------
// Initialise lcd pins
void lcdPreInit() {
  pinMode(SDO, OUTPUT);
  pinMode(SCK, OUTPUT);
  pinMode(CS, OUTPUT);
  pinMode(RESET, OUTPUT);

  // This is a bit hacky for the older hardware boards - to prevent false 'powered on' detection
  digitalWrite(SDO, LOW);
  digitalWrite(SCK, LOW);
  digitalWrite(CS, LOW);
  digitalWrite(RESET, LOW);

  delay(100);
}

//---------------------------------------------------------------
// Init the LCD
void lcdInit() {
  pinMode(SDO, OUTPUT);
  pinMode(SCK, OUTPUT);
  pinMode(CS, OUTPUT);
  pinMode(RESET, OUTPUT);
  
  digitalWrite(RESET, LOW);
  delay(10);
  digitalWrite(RESET, HIGH);
  delay(10);
  
/*#ifdef USE_RESETPULLUP
  pinMode(RESET, INPUT_PULLUP); // Low power pullup
#else
  pinMode(RESET, INPUT);
  delay(1);
  digitalWrite(RESET, LOW);
#endif*/

#if LCD == LCD_26_320
  lcdInit_26_320();
#elif LCD == LCD_35_640
  lcdInit_35_640();
#elif LCD == LCD_35_640_54P
  lcdInit_35_640_54p();
#elif LCD == LCD_32_800
  lcdInit_32_800();
#else
  return;
#endif
  
}

//---------------------------------------------------------------
void lcdInit_26_320() {
  // Removed for now, sorry ;)
}

void lcdInit_26_320_1() {
  // Removed for now, sorry ;)
}

void lcdInit_35_640() {
  lcdCommand(0xB9); //enable extention command
  lcdData(0xFF); 
  lcdData(0X83);
  lcdData(0X63);
  
  lcdCommand(0xB1);  //set power control
  lcdData(0x81);
  lcdData(0x30);
  lcdData(0x08); //was 0x04
  lcdData(0x36); //was 0x34
  lcdData(0x01); //was 0x02 
  lcdData(0x13); 
  lcdData(0x10); //was 0x11
  lcdData(0x10); //was 0x00
  lcdData(0x35); //was 0x3A
  lcdData(0x3D); //was 0x42
  lcdData(0x1A); //was 0x3F
  lcdData(0x1A); //was 0x3F
  
  lcdCommand(0x3A); //colour mode
  lcdData(0x60);  //262K-18bit/pixel
  
  lcdCommand(0x36); //Set panel 
  lcdData(0x0A); //flip + BGR
  
  /*lcdCommand(0xC0); //new
  lcdData(0x41); //new
  lcdData(0x19); //new*/
  
  lcdCommand(0xBF); //PTBA
  lcdData(0x00);
  lcdData(0x10);
  
  lcdCommand(0xB3); //RGB interface polarity
  lcdData(0x01);
  
  lcdCommand(0xB4); //display waveform cycle
  lcdData(0x01); //was 0x08
  lcdData(0x12);
  lcdData(0x72);
  lcdData(0x12);
  lcdData(0x06);
  lcdData(0x03);
  lcdData(0x54);
  lcdData(0x03);
  lcdData(0x4E);
  lcdData(0x00); //new
  lcdData(0x00); //new
  
  lcdCommand(0xB6); //VCOM voltage
  lcdData(0x33); //was 0x12

  lcdCommand(0xCC); //Set panel
  lcdData(0x02); //reverse
  
  delay(120); //new

  lcdCommand(0xE0); // Set Gamma 2.2
  lcdData(0x01); //ALL NEW
  lcdData(0x07);
  lcdData(0x4C);
  lcdData(0xB0);
  lcdData(0x36);
  lcdData(0x3F);
  lcdData(0x06);
  lcdData(0x49);
  lcdData(0x51);
  lcdData(0x96);
  lcdData(0x18);
  lcdData(0xD8);
  lcdData(0x18);
  lcdData(0x50);
  lcdData(0x13);
  
  lcdData(0x01);
  lcdData(0x07);
  lcdData(0x4C);
  lcdData(0xB0);
  lcdData(0x36);
  lcdData(0x3F);
  lcdData(0x06);
  lcdData(0x49);
  lcdData(0x51);
  lcdData(0x96);
  lcdData(0x18);
  lcdData(0xD8);
  lcdData(0x18);
  lcdData(0x50);
  lcdData(0x13);
  
  delay(150); //new
  
  lcdCommand(0x11); // Exit Sleep 

  delay(200); 
  
  lcdCommand(0x29); // Display on
}

void lcdInit_35_640_54p() {
  // Init code is a secret (for now)
}

void lcdInit_32_800() {
  // Removed for now, sorry ;)
}

//---------------------------------------------------------------
// Set CS
void lcdWriteEn(bool state) {
  if (state) {
    digitalWrite(CS, LOW);
#ifdef DEBUG
    Serial.print("[!]");
#endif
  } else {
    digitalWrite(CS, HIGH);
#ifdef DEBUG
    Serial.println("[.]");
#endif
  }
}

// Send a command
void lcdCommand(uint8_t cmd) {

  lcdWriteEn(1);
  
  // COMMAND BIT/MODE
#if LCD == LCD_26_320
  lcdClockOut(0);
#elif LCD == LCD_35_640
  lcdClockOut(0);
#elif LCD == LCD_35_640_54P
  lcdClockOut(0);
#elif LCD == LCD_32_800
  uint8_t cmdbyte = 0x70;
  for (uint8_t i=8; i>0; i--) {
    uint8_t pos = i-1;
    lcdClockOut(bitRead(cmdbyte,pos));
  }
#endif

  // COMMAND BYTE
  for (uint8_t i=8; i>0; i--) {
    uint8_t pos = i-1;
    lcdClockOut(bitRead(cmd,pos));
  }

#ifdef DEBUG
  Serial.print("[Cx");
  Serial.print(cmd, HEX);
  Serial.print("]");
#endif

  lcdWriteEn(0);
  
}

// Send data
void lcdData(uint8_t data) {

  lcdWriteEn(1);
  
  // COMMAND BIT/MODE
#if LCD == LCD_26_320
  lcdClockOut(1);
#elif LCD == LCD_35_640
  lcdClockOut(1);
#elif LCD == LCD_35_640_54P
  lcdClockOut(1);
#elif LCD == LCD_32_800
  uint8_t databyte = 0x72;
  for (uint8_t i=8; i>0; i--) {
    uint8_t pos = i-1;
    lcdClockOut(bitRead(databyte,pos));
  }
#endif

  // DATA BYTE
  for (uint8_t i=8; i>0; i--) {
    uint8_t pos = i-1;
    lcdClockOut(bitRead(data,pos));
  }

#ifdef DEBUG
  Serial.print("[Dx");
  Serial.print(data, HEX);
  Serial.print("]");
#endif

  lcdWriteEn(0);
  
}

// Clock out a bit
void lcdClockOut(bool b) {
  // SET MOSI
  digitalWrite(SDO, b);
  
  // CLOCK OUT
  digitalWrite(SCK, LOW);
  digitalWrite(SCK, HIGH);
}

//---------------------------------------------------------------
// Converts two chars to one byte
// c1 is MS, c2 is LS
uint8_t byteval(char c1, char c2){
  uint8_t by;
  if(c1 <= '9'){
    by = c1-'0';
  }else{
    by = c1-'A'+10;
  }
  by = by << 4;
  if(c2 <= '9'){
    by += c2-'0';
  }else{
    by += c2-'A'+10;
  }
  return by;
}

// Check if even
bool isEven(byte val) {
  if (val % 2) {
    return 0;
  } else {
    return 1;
  }
}

