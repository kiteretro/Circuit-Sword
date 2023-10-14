/*
 * giles test shift in
 */
 
#include <stdio.h>
#include <wiringPi.h>
#include <stdbool.h>
#include <stdint.h>
// #include <time.h>
 
#define PIN_DATA  22 
#define PIN_LATCH 27
#define PIN_CLOCK 23

void shiftin(uint8_t count)
{
  digitalWrite(PIN_CLOCK, 0);
  digitalWrite(PIN_LATCH, 0);
  digitalWrite(PIN_LATCH, 1);

  uint8_t by = 0;
  uint8_t bi = 0;
  int32_t result = 0;
  uint8_t pos = 0;

  for (by = 0; by < count; by++)
  {
    for (bi = 0; bi < 8; bi++)
    {
      bool bit = digitalRead(PIN_DATA);
      result |= bit << pos;
      pos++;
      
      digitalWrite(PIN_CLOCK, 1);
      digitalWrite(PIN_CLOCK, 0);
    }
  }
  
  printf(" - %#08x\n", result);
}

int main(void)
{
  printf("Raspberry Pi shift in\n");
 
  if (wiringPiSetupGpio() == -1)
    return 1;
 
  pinMode(PIN_DATA,  INPUT);
  pinMode(PIN_LATCH, OUTPUT);
  pinMode(PIN_CLOCK, OUTPUT);

  for (;;)
  {
    shiftin(3);
    delay(16);
  }
  return 0;
}

