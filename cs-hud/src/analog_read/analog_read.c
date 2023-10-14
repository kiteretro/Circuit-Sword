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
 //----------------------------------------------------------------------------

#include "analog_read.h"

//-----------------------------------------------------------------------------
// PRIVATE VARIABLES

int pin_voltage = 0;

float config_val_vc = 2.0;  // Switching voltage (v)     (leave this)
float config_val_r  = 26.2; // RC resistance     (k ohm) (adjust this!)
float config_val_c  = 1.0;  // RC capacitance    (uF)    (leave this)

struct timeval st, et;
bool voltage_pin_ready = 0;
bool voltage_available = 0;

double voltage_averages[AVERAGE_SIZE];
uint8_t voltage_averages_pos = 0;
uint8_t voltage_average_count = 0;

//-----------------------------------------------------------------------------
// METHODS

// Add a number to the average
void voltage_add_to_averages(double volt) {
  voltage_averages[voltage_averages_pos] = volt;

  voltage_averages_pos++;
  if (voltage_averages_pos == AVERAGE_SIZE) {
    voltage_averages_pos = 0;
  }

  if (voltage_average_count < AVERAGE_SIZE - 1) {
    voltage_average_count++;
  }
}

//-----------------------------------------------------------------------------

// Calculate the average
double read_analog_voltage_average() {
  double ret = 0.0;

  // Add up all populated values
  uint8_t pos = 0;
  for (pos = 0; pos < voltage_average_count; pos++) {
    ret += voltage_averages[pos];
  }

  // Return divided by count unless DIV/0 case
  if (ret == 0.0) {
    return ret;
  } else {
    return ret / voltage_average_count;
  }
}

//-----------------------------------------------------------------------------

// Method to set the pin to input for the read
void read_analog_start()
{
  if (!voltage_pin_ready) {
    // Set pin low to start with
    pinMode(pin_voltage, OUTPUT);
    digitalWrite(pin_voltage, 0);
    voltage_pin_ready = 1;
  }

  // Start timer
  gettimeofday(&st,NULL);

  // Set pin to read
  pinMode(pin_voltage, INPUT);
  voltage_available = 0;
}

//-----------------------------------------------------------------------------

// Call this often
bool read_analog_process()
{
  // Only process when a new voltage is available
  if (voltage_available) {

    // Calc us elapsed
    double elapsed = (((et.tv_sec - st.tv_sec) * 1000000) + (et.tv_usec - st.tv_usec)) / 1000.0;

    // Convert to voltage
    double voltage = - ( config_val_vc / (exp(-((elapsed))/(config_val_r*config_val_c))-1.0) );

    // Perform averages
    voltage_add_to_averages(voltage);

#if defined DEBUG
    printf("[#] BATTERY! Time: %f, Voltage: %fV, Average: %fV\n", elapsed, voltage, read_analog_voltage_average());
#endif

    voltage_available = 0;

    return true;
  } else {
    return false;
  }
}

//-----------------------------------------------------------------------------

// Callback function for when the pin goes high
void read_analog_callback(void)
{
  // Prevent multiple calls as we only want the first
  if (voltage_pin_ready) {
    // Set the end time
    voltage_pin_ready = 0;
    gettimeofday(&et,NULL);
    voltage_available = 1;

    // Reset pin state
    pinMode(pin_voltage, OUTPUT);
    digitalWrite(pin_voltage, 0);
  }
}

//-----------------------------------------------------------------------------

// Init the analog read by dropping pin low and set up callback
void read_analog_init(int pin)
{
  // Set the pin
  pin_voltage = pin;

  // Reset variables
  voltage_pin_ready = 0;
  voltage_available = 0;
  voltage_averages_pos = 0;
  voltage_average_count = 0;

  // Set pin state
  pinMode(pin_voltage, OUTPUT);
  digitalWrite(pin_voltage, 0);
  delay(200);

  // Register callback
  wiringPiISR(pin_voltage, INT_EDGE_RISING, &read_analog_callback);
}

//-----------------------------------------------------------------------------
