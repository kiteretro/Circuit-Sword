/*
 * giles test analog in
 */

#include <stdio.h>
#include <wiringPi.h>
#include <stdbool.h>
#include <stdint.h>
#include <sys/time.h>
#include <math.h>

// Hardware variables
#define PIN_VOLTAGE  26

// Config variables
#define DELAY_TIME 10
#define AVERAGE_SIZE 20

float val_vc = 2.0;  // Switching voltage (v)     (leave this)
float val_r  = 25.5; // RC resistance     (k ohm) (adjust this!)
float val_c = 1.0;  // RC capacitance    (uF)    (leave this)

// Private variables
struct timeval st, et;
bool voltage_pin_ready = 0;
bool voltage_available = 0;

double voltage_averages[AVERAGE_SIZE];
uint8_t voltage_averages_pos = 0;
uint8_t voltage_average_count = 0;


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

// Calculate the average
double voltage_average() {
  double ret = 0.0;

  uint8_t pos = 0;
  for (pos = 0; pos < voltage_average_count; pos++) {
    ret += voltage_averages[pos];
  }

  if (ret == 0.0) {
    return ret;
  } else {
    return ret / voltage_average_count;
  }
}


// Method to set the pin to input for the read
void read_analog_start()
{
  if (!voltage_pin_ready) {
    // Set pin low to start with
    pinMode(PIN_VOLTAGE, OUTPUT);
    digitalWrite(PIN_VOLTAGE, 0);
    // delay(DELAY_TIME);
    voltage_pin_ready = 1;
  }

  // Start timer
  gettimeofday(&st,NULL);

  // Set pin to read
  pinMode(PIN_VOLTAGE, INPUT);
  voltage_available = 0;
}


// Call this often
bool read_analog_process()
{
  if (voltage_available) {

    double elapsed = (((et.tv_sec - st.tv_sec) * 1000000) + (et.tv_usec - st.tv_usec)) / 1000.0;
    printf("[#] ----- Time: %f micro seconds -----\n", elapsed);

    double voltage = - ( val_vc / (exp(-((elapsed))/(val_r*val_c))-1.0) );
    printf("[#] VOLTAGE: %f\n", voltage);

    voltage_add_to_averages(voltage);
    printf("[#] AVERAGE: %f\n", voltage_average());

    voltage_available = 0;

    return true;
  } else {
    // printf("[i] Process called but no voltage available yet..\n");
    return false;
  }
}


// Callback function for when the pin goes high
void read_analog_callback(void)
{
  if (voltage_pin_ready) {

    // Set the end time
    voltage_pin_ready = 0;
    gettimeofday(&et,NULL);
    voltage_available = 1;

    // Reset pin state
    pinMode(PIN_VOLTAGE, OUTPUT);
    digitalWrite(PIN_VOLTAGE, 0);

  }
}


// Init the analog read by dropping pin low and set up callback
void read_analog_init()
{
  // float val_vc = 2.0;  // Switching voltage (v)     (leave this)
  // float val_r  = 25.5; // RC resistance     (k ohm) (adjust this!)
  // float val_c = 1.0;  // RC capacitance    (uF)    (leave this)

  // Reset variables
  voltage_pin_ready = 0;
  voltage_available = 0;
  voltage_averages_pos = 0;
  voltage_average_count = 0;

  // Set pin state
  pinMode(PIN_VOLTAGE, OUTPUT);
  digitalWrite(PIN_VOLTAGE, 0);
  delay(200);

  // Register callback
  wiringPiISR(PIN_VOLTAGE, INT_EDGE_RISING, &read_analog_callback);
}


// MAIN
int main(void)
{
  printf("Raspberry Pi analog in\n");

  if (wiringPiSetupGpio() == -1)
    return 1;

  read_analog_init();
  read_analog_start();

  for (;;) {

    if (read_analog_process()) {
      read_analog_start();
    }

    delay(500);

  }

  return 0;
}
