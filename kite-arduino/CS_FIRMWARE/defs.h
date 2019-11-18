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

// I2C Bits
#define TCAADDR 0x24

#define PORT_0 0
#define PORT_1 1

#define DIR_OUTPUT 0
#define DIR_INPUT  1

#define POLARITY_NORMAL 0
#define POLARITY_INV    1

#define CR_IN0  0x0
#define CR_IN1  0x1
#define CR_OUT0 0x2
#define CR_OUT1 0x3
#define CR_INV0 0x4
#define CR_INV1 0x5
#define CR_CFG0 0x6
#define CR_CFG1 0x7

// Button order
#define B_UP     0
#define B_DOWN   1
#define B_LEFT   2
#define B_RIGHT  3
#define B_A      4
#define B_B      5
#define B_X      6
#define B_Y      7
#define B_START  8
#define B_SELECT 9
#define B_L1     10
#define B_L2     11
#define B_R1     12
#define B_R2     13
#define B_C1     14
#define B_C2     15

// Gamepad buttons assignment
#define GP_A       1
#define GP_B       2
#define GP_X       3
#define GP_Y       4
#define GP_START   5
#define GP_SELECT  6
#define GP_L1      7
#define GP_L2      8
#define GP_R1      9
#define GP_R2      10
#define GP_C1      11
#define GP_C2      12
#define GP_VOLUP   13
#define GP_VOLDOWN 14
#define GP_DPAD_JOY_UP    15
#define GP_DPAD_JOY_DOWN  16
#define GP_DPAD_JOY_LEFT  17
#define GP_DPAD_JOY_RIGHT 18

// VOLUME
#define VOL_UP 1
#define VOL_DOWN 0

#define VOL_MIN 0
#define VOL_MAX 100
#define VOL_INC 5
#define VOL_DEFAULT 50

#define VOL_A_MIN 20
#define VOL_A_MAX 1000

// BACKLIGHT
#define BL_UP   1
#define BL_DOWN 0

#define BL_MIN 0
#define BL_MAX 100
#define BL_INC 10
#define BL_PWM_MIN 235  // High val = LOW
#define BL_PWM_MAX 0    // 0 = FULL ON

// Button freeze settings
#define FREEZE_DURATION 1000

// EEPROM
#define EEPROM_VERSION 5
#define EEPROM_START   0

// MISC
#define LED_OFF 0
#define LED_ON  1

//List of models, MAX is 15 (4bits)
#define MODEL_SAIO 1
#define MODEL_CSORIGINAL 2
#define MODEL_CSADVANCE 3
#define MODEL_CSPOCKET 4

//List of versions, MAX is 15 (4bits)
#define HARDWARE_CSO_V1_1E 1
#define HARDWARE_CSO_V1_2A 2

#define MODECHECK_A0       1 //Read voltage on A0 to determine board power check
#define MODECHECK_VBATHIGH 2 //Read the VBAT voltage to determine board power check, HIGH = OFF
#define MODECHECK_VBATLOW  3 //Read the VBAT voltage to determine board power check, LOW = OFF
#define MODECHECK_BOOTPIN  4 //Read a pin to check if booted or not

#define LCD_26_320 1
#define LCD_35_640 2
#define LCD_32_800 3
#define LCD_35_640_54P 4

#define LCD_SUCCESS "OK"
#define FAILED_TIMEOUT_NO_END "F1"
#define FAILED_NOT_EVEN_ARGS  "F2"

#define STATUS_MODE     0
#define STATUS_WIFI     1
#define STATUS_AUD      2
#define STATUS_INFO     3
#define STATUS_AVOL     4
#define STATUS_DPAD_JOY 5
#define STATUS_DVOL     6

